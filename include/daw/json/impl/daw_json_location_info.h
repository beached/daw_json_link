// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/daw_json_link
//

#pragma once

#include "daw_murmur3.h"
#include "version.h"

#include <daw/daw_likely.h>
#include <daw/daw_sort_n.h>
#include <daw/daw_string_view.h>
#include <daw/daw_uint_buffer.h>

#include <ciso646>
#include <cstddef>

namespace daw::json {
	inline namespace DAW_JSON_VER {
		namespace json_details {
			template<bool FullNameMatch>
			struct location_info_t {
				daw::string_view name;
				char const *first = nullptr;
				char const *last = nullptr;
				char const *class_first = nullptr;
				char const *class_last = nullptr;
				std::size_t counter = 0;

				[[maybe_unused, nodiscard]] inline constexpr bool missing( ) const {
					return first == nullptr;
				}

				template<typename ParseState>
				constexpr void set_range( ParseState parse_state ) {
					first = parse_state.first;
					last = parse_state.last;
					class_first = parse_state.class_first;
					class_last = parse_state.class_last;
					counter = parse_state.counter;
				}

				template<typename ParseState>
				constexpr auto get_range( template_param<ParseState> ) const {
					using range_t = typename ParseState::without_allocator_type;
					auto result = range_t( first, last, class_first, class_last );
					result.counter = counter;
					return result;
				}
			};

			template<>
			struct location_info_t<false> {
				char const *first = nullptr;
				char const *last = nullptr;
				char const *class_first = nullptr;
				char const *class_last = nullptr;
				std::size_t counter = 0;

				[[maybe_unused, nodiscard]] inline constexpr bool missing( ) const {
					return first == nullptr;
				}

				template<typename ParseState>
				constexpr void set_range( ParseState parse_state ) {
					first = parse_state.first;
					last = parse_state.last;
					class_first = parse_state.class_first;
					class_last = parse_state.class_last;
					counter = parse_state.counter;
				}

				template<typename ParseState>
				constexpr auto get_range( template_param<ParseState> ) const {
					using range_t = typename ParseState::without_allocator_type;
					auto result = range_t( first, last, class_first, class_last );
					result.counter = counter;
					return result;
				}
			};

			/***
			 * Contains an array of member location_info mapped in a json_class
			 * @tparam MemberCount Number of mapped members from json_class
			 */
			template<std::size_t MemberCount, bool DoFullNameMatch = true>
			struct locations_info_t {
				using value_type = location_info_t<DoFullNameMatch>;
				using reference = value_type &;
				using const_reference = value_type const &;
				static constexpr bool do_full_name_match = DoFullNameMatch;
				daw::UInt32 hashes[MemberCount];
				value_type names[MemberCount];

				constexpr const_reference operator[]( std::size_t idx ) const {
					return names[idx];
				}

				constexpr reference operator[]( std::size_t idx ) {
					return names[idx];
				}

				static constexpr std::size_t size( ) {
					return MemberCount;
				}

				template<std::size_t start_pos>
				[[nodiscard]] constexpr std::size_t
				find_name( daw::string_view key ) const {
					UInt32 const hash = name_hash( key );
#if defined( _MSC_VER ) and not defined( __clang__ )
					// MSVC has a bug where the list initialization isn't sequenced in
					// order of appearance.
					for( std::size_t n = 0; n < MemberCount; ++n ) {
#else
					for( std::size_t n = start_pos; n < MemberCount; ++n ) {
#endif
						if( hashes[n] == hash ) {
							if constexpr( do_full_name_match ) {
								if( DAW_UNLIKELY( key != names[n].name ) ) {
									continue;
								}
							}
							return n;
						}
					}
					return MemberCount;
				}
			};

			// Should never be called outside a consteval context
			template<typename... MemberNames>
			inline constexpr bool do_hashes_collide( ) {
				daw::UInt32 hashes[sizeof...( MemberNames )]{
				  name_hash( MemberNames::name )... };

				daw::sort( std::data( hashes ), daw::data_end( hashes ) );
				return daw::algorithm::adjacent_find( std::data( hashes ),
				                                      daw::data_end( hashes ),
				                                      []( UInt32 l, UInt32 r ) {
					                                      return l == r;
				                                      } ) != daw::data_end( hashes );
			}

			// Should never be called outside a consteval context
			template<typename ParseState, typename... JsonMembers>
			constexpr auto make_locations_info( ) {
#if defined( __MSC_VER ) and not defined( __clang__ )
				constexpr bool do_full_name_match = true;
				return locations_info_t<sizeof...( JsonMembers ), do_full_name_match>{
				  { daw::name_hash( JsonMembers::name )... },
				  { location_info_t<do_full_name_match>{ JsonMembers::name }... } };
#else
				constexpr bool do_full_name_match =
				  ParseState::force_name_equal_check or
				  do_hashes_collide<JsonMembers...>( );
				return locations_info_t<sizeof...( JsonMembers ), do_full_name_match>{
				  { daw::name_hash( JsonMembers::name )... }, {} };
#endif
			}

			/***
			 * Get the position from already seen JSON members or move the parser
			 * forward until we reach the end of the class or the member.
			 * @tparam N Number of members in json_class
			 * @tparam ParseState see IteratorRange
			 * @param locations members location and names
			 * @param parse_state Current JSON data
			 * @return IteratorRange with begin( ) being start of value
			 */
			enum class AllMembersMustExist { yes, no };
			template<std::size_t pos, AllMembersMustExist must_exist,
			         bool from_start = false, std::size_t N, typename ParseState,
			         bool B>
			[[nodiscard]] inline constexpr ParseState
			find_class_member( locations_info_t<N, B> &locations,
			                   ParseState &parse_state, bool is_nullable,
			                   daw::string_view member_name ) {

				daw_json_assert_weak( is_nullable or
				                        ( not locations[pos].missing( ) ) or
				                        ( not parse_state.is_closing_brace_checked( ) ),
				                      missing_member( member_name ), parse_state );

				parse_state.trim_left_unchecked( );
				while( locations[pos].missing( ) & ( parse_state.front( ) != '}' ) ) {
					daw_json_assert_weak( parse_state.has_more( ),
					                      ErrorReason::UnexpectedEndOfData, parse_state );
					// TODO: fully unescape name
					auto const name = parse_name( parse_state );
					auto const name_pos =
					  locations.template find_name<( from_start ? 0 : pos )>( name );
					if constexpr( must_exist == AllMembersMustExist::yes ) {
						daw_json_assert_weak( name_pos < std::size( locations ),
						                      ErrorReason::UnknownMember, parse_state );
					} else {
						if( name_pos >= std::size( locations ) ) {
							// This is not a member we are concerned with
							(void)skip_value( parse_state );
							parse_state.clean_tail( );
							continue;
						}
					}
					if( name_pos == pos ) {
						locations[pos].set_range( parse_state );
						break;
					} else {
						// We are out of order, store position for later
						// OLDTODO:	use type knowledge to speed up skip
						// OLDTODO:	on skipped classes see if way to store
						// 				member positions so that we don't have to
						//				re-parse them after
						// RESULT: storing preparsed is slower, don't try 3 times
						// it also limits the type of things we can parse potentially
						// Using locations to switch on BaseType is slower too
						locations[name_pos].set_range( skip_value( parse_state ) );

						parse_state.clean_tail( );
					}
				}
				return locations[pos]
				  .get_range( template_arg<ParseState> )
				  .with_allocator( parse_state );
			}
		} // namespace json_details
	}   // namespace DAW_JSON_VER
} // namespace daw::json
