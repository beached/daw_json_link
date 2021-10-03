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

#include <daw/daw_consteval.h>
#include <daw/daw_likely.h>
#include <daw/daw_sort_n.h>
#include <daw/daw_string_view.h>
#include <daw/daw_uint_buffer.h>

#include <ciso646>
#include <cstddef>

#if defined( DAW_JSON_PARSER_DIAGNOSTICS )
#include <cmath>
#include <iostream>
#endif

namespace daw::json DAW_ATTRIB_PUBLIC {
	inline namespace DAW_JSON_VER {
		namespace json_details DAW_ATTRIB_HIDDEN {
			template<bool FullNameMatch, typename CharT>
			struct location_info_t {
				daw::string_view name;
				CharT *first = nullptr;
				CharT *last = nullptr;
				CharT *class_first = nullptr;
				CharT *class_last = nullptr;
				std::size_t counter = 0;

				[[maybe_unused, nodiscard]] inline constexpr bool missing( ) const {
					return first == nullptr;
				}

				template<typename ParseState>
				constexpr void set_range( ParseState const &parse_state ) {
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

			template<typename CharT>
			struct location_info_t<false, CharT> {
				CharT *first = nullptr;
				CharT *last = nullptr;
				CharT *class_first = nullptr;
				CharT *class_last = nullptr;
				std::size_t counter = 0;

				[[maybe_unused, nodiscard]] inline constexpr bool missing( ) const {
					return first == nullptr;
				}

				template<typename ParseState>
				constexpr void set_range( ParseState const &parse_state ) {
					first = parse_state.first;
					last = parse_state.last;
					class_first = parse_state.class_first;
					class_last = parse_state.class_last;
					counter = parse_state.counter;
				}

				template<typename ParseState>
				constexpr auto get_range( template_param<ParseState> ) const {
					// Not copying allocator as it may contain state that needs copying in
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
			template<std::size_t MemberCount, typename CharT,
			         bool DoFullNameMatch = true>
			struct locations_info_t {
				using value_type = location_info_t<DoFullNameMatch, CharT>;
				using reference = value_type &;
				using const_reference = value_type const &;
				static constexpr bool do_full_name_match = DoFullNameMatch;
				daw::UInt32 hashes[MemberCount];
				value_type names[MemberCount];

				constexpr const_reference operator[]( std::size_t idx ) const {
					daw_json_assert( idx < MemberCount, ErrorReason::NumberOutOfRange );
					return names[idx];
				}

				constexpr reference operator[]( std::size_t idx ) {
					daw_json_assert( idx < MemberCount, ErrorReason::NumberOutOfRange );
					return names[idx];
				}

				static constexpr std::size_t size( ) {
					return MemberCount;
				}

				template<bool expect_long_strings, std::size_t start_pos>
				DAW_ATTRIB_INLINE [[nodiscard]] inline constexpr std::size_t
				find_name( daw::template_vals_t<start_pos>,
				           daw::string_view key ) const {
					UInt32 const hash = name_hash<expect_long_strings>( key );
#if defined( _MSC_VER ) and not defined( __clang__ )
					// MSVC has a bug where the list initialization isn't sequenced in
					// order of appearance.
					(void)start_pos;
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
			inline DAW_CONSTEVAL bool do_hashes_collide( ) {
				daw::UInt32 hashes[sizeof...( MemberNames )]{
				  name_hash<false>( MemberNames::name )... };

				daw::sort( std::data( hashes ), daw::data_end( hashes ) );
				return daw::algorithm::adjacent_find( std::data( hashes ),
				                                      daw::data_end( hashes ),
				                                      []( UInt32 l, UInt32 r ) {
					                                      return l == r;
				                                      } ) != daw::data_end( hashes );
			}
#if defined( _MSC_VER ) and not defined( __clang__ )
#define DAW_JSON_MAKE_LOC_INFO_CONSTEVAL constexpr
#else
#define DAW_JSON_MAKE_LOC_INFO_CONSTEVAL DAW_CONSTEVAL
#endif
			// Should never be called outside a consteval context
			template<typename ParseState, typename... JsonMembers>
			DAW_ATTRIB_FLATINLINE inline DAW_JSON_MAKE_LOC_INFO_CONSTEVAL auto
			make_locations_info( ) {
				using CharT = typename ParseState::CharT;
#if defined( DAW_JSON_PARSER_DIAGNOSTICS ) or \
  ( defined( __MSC_VER ) and not defined( __clang__ ) )
				constexpr bool do_full_name_match = true;
				return locations_info_t<sizeof...( JsonMembers ), CharT,
				                        do_full_name_match>{
				  { daw::name_hash<false>( JsonMembers::name )... },
				  { location_info_t<do_full_name_match, CharT>{
				    JsonMembers::name }... } };
#else
				// DAW
				constexpr bool do_full_name_match =
				  ParseState::force_name_equal_check or
				  do_hashes_collide<JsonMembers...>( );
				if constexpr( do_full_name_match ) {
					return locations_info_t<sizeof...( JsonMembers ), CharT,
					                        do_full_name_match>{
					  { daw::name_hash<false>( JsonMembers::name )... },
					  { location_info_t<do_full_name_match, CharT>{
					    JsonMembers::name }... } };
				} else {
					return locations_info_t<sizeof...( JsonMembers ), CharT,
					                        do_full_name_match>{
					  { daw::name_hash<false>( JsonMembers::name )... }, {} };
				}
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
			         bool B, typename CharT>
			[[nodiscard]] inline constexpr std::pair<ParseState, bool>
			find_class_member( ParseState &parse_state,
			                   locations_info_t<N, CharT, B> &locations,
			                   bool is_nullable, daw::string_view member_name ) {

				// silencing gcc9 warning as these are selectively used
				(void)is_nullable;
				(void)member_name;

				daw_json_assert_weak( is_nullable | ( not locations[pos].missing( ) ) |
				                        ( not parse_state.is_closing_brace_checked( ) ),
				                      missing_member( member_name ), parse_state );

				parse_state.trim_left_unchecked( );
				bool known = not locations[pos].missing( );
				while( locations[pos].missing( ) & ( parse_state.front( ) != '}' ) ) {
					// TODO: fully unescape name
					// parse_name checks if we have more and are quotes
					auto const name = parse_name( parse_state );
					auto const name_pos =
					  locations.template find_name<ParseState::expect_long_strings>(
					    template_vals<( from_start ? 0 : pos )>, name );
					if constexpr( must_exist == AllMembersMustExist::yes ) {
						daw_json_assert_weak( name_pos < std::size( locations ),
						                      ErrorReason::UnknownMember, parse_state );
					} else {
#if defined( DAW_JSON_PARSER_DIAGNOSTICS )
						std::cerr << "DEBUG: Unknown member '" << name << '\n';
#endif
						if( name_pos >= std::size( locations ) ) {
							// This is not a member we are concerned with
							(void)skip_value( parse_state );
							parse_state.move_next_member_or_end( );
							continue;
						}
					}
					if( name_pos == pos ) {
						locations[pos].set_range( parse_state );
						break;
					} else {
#if defined( DAW_JSON_PARSER_DIAGNOSTICS )
						std::cerr << "DEBUG:	Out of order member '"
						          << locations.names[name_pos].name
						          << "' found, looking for '" << locations.names[pos].name
						          << ". It is "
						          << std::abs( static_cast<long long>( pos ) -
						                       static_cast<long long>( name_pos ) )
						          << " members ahead in constructor\n";
#endif
						// We are out of order, store position for later
						// OLDTODO:	use type knowledge to speed up skip
						// OLDTODO:	on skipped classes see if way to store
						// 				member positions so that we don't have to
						//				re-parse them after
						// RESULT: storing preparsed is slower, don't try 3 times
						// it also limits the type of things we can parse potentially
						// Using locations to switch on BaseType is slower too
						locations[name_pos].set_range( skip_value( parse_state ) );

						if constexpr( ParseState::is_unchecked_input ) {
							if( name_pos + 1 < std::size( locations ) ) {
								parse_state.move_next_member( );
							} else {
								parse_state.move_next_member_or_end( );
							}
						} else {
							parse_state.move_next_member_or_end( );
						}
					}
				}
				if( locations[pos].missing( ) ) {
					known = true;
				}
				if constexpr( ParseState::has_allocator ) {
					return std::pair<ParseState, bool>{
					  locations[pos]
					    .get_range( template_arg<ParseState> )
					    .with_allocator( parse_state ),
					  known };
				} else {
					return std::pair<ParseState, bool>{
					  locations[pos].get_range( template_arg<ParseState> ), known };
				}
			}
		} // namespace DAW_ATTRIB_HIDDEN
	}   // namespace DAW_JSON_VER
} // namespace daw::json
