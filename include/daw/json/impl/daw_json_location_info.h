// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/daw_json_link
//

#pragma once

#include "daw_murmur3.h"
#include "namespace.h"

#include <daw/daw_sort_n.h>
#include <daw/daw_string_view.h>
#include <daw/daw_uint_buffer.h>

#include <ciso646>
#include <cstddef>

namespace DAW_JSON_NS::json_details {
	class location_info_t {
		daw::string_view name;
		char const *first = nullptr;
		char const *last = nullptr;
		char const *class_first = nullptr;
		char const *class_last = nullptr;
		std::size_t counter = 0;

	public:
		explicit constexpr location_info_t( daw::string_view Name )
		  : name( Name ) {}

		[[maybe_unused, nodiscard]] inline constexpr bool missing( ) const {
			return first == nullptr;
		}

		template<typename Range>
		constexpr void set_range( Range rng ) {
			first = rng.first;
			last = rng.last;
			class_first = rng.class_first;
			class_last = rng.class_last;
			counter = rng.counter;
		}

		template<typename Range>
		constexpr auto get_range( ) const {
			using range_t = typename Range::without_allocator_type;
			auto result = range_t( first, last, class_first, class_last );
			result.counter = counter;
			return result;
		}
	};

	/***
	 * Contains an array of member location_info mapped in a json_class
	 * @tparam MemberCount Number of mapped members from json_class
	 */
	template<std::size_t MemberCount, bool HasCollisions = true>
	struct locations_info_t {
		using value_type = location_info_t;
		using reference = value_type &;
		using const_reference = value_type const &;
		static constexpr bool has_collisions = HasCollisions;
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
			// MSVC has a bug where the list initialization isn't sequenced in order
			// of appearance.
			for( std::size_t n = 0; n < MemberCount; ++n ) {
#else
			for( std::size_t n = start_pos; n < MemberCount; ++n ) {
#endif
				if( hashes[n] == hash ) {
					if constexpr( has_collisions ) {
						if( DAW_JSON_UNLIKELY( key != names[n].name ) ) {
							continue;
						}
					}
					return n;
				}
			}
			return MemberCount;
		}
	};

	// Should never be called outsite a consteval context
	template<typename... MemberNames>
	static inline constexpr bool do_hashes_collide( ) {
		daw::UInt32 hashes[sizeof...( MemberNames )]{
		  name_hash( MemberNames::name )... };

		daw::sort( std::data( hashes ), daw::data_end( hashes ) );
		return daw::algorithm::adjacent_find( std::data( hashes ),
		                                      daw::data_end( hashes ),
		                                      []( UInt32 l, UInt32 r ) {
			                                      return l == r;
		                                      } ) != daw::data_end( hashes );
	}

	// Should never be called outsite a consteval context
	template<typename Range, typename... JsonMembers>
	constexpr auto make_locations_info( ) {
		constexpr bool hashes_collide =
		  Range::force_name_equal_check or do_hashes_collide<JsonMembers...>( );
		return locations_info_t<sizeof...( JsonMembers ), hashes_collide>{
		  { daw::name_hash( JsonMembers::name )... },
		  { location_info_t( JsonMembers::name )... } };
	}

	/***
	 * Get the position from already seen JSON members or move the parser
	 * forward until we reach the end of the class or the member.
	 * @tparam N Number of members in json_class
	 * @tparam Range see IteratorRange
	 * @param locations members location and names
	 * @param rng Current JSON data
	 * @return IteratorRange with begin( ) being start of value
	 */
	template<std::size_t pos, bool from_start = false, std::size_t N,
	         typename Range, bool B>
	[[nodiscard]] static inline constexpr Range
	find_class_member( locations_info_t<N, B> &locations, Range &rng,
	                   bool is_nullable, daw::string_view member_name ) {

		daw_json_assert_weak( is_nullable or ( not locations[pos].missing( ) ) or
		                        ( not rng.is_closing_brace_checked( ) ),
		                      missing_member( member_name ), rng );

		rng.trim_left_unchecked( );
		// TODO: should we check for end
		while( locations[pos].missing( ) & ( rng.front( ) != '}' ) ) {
			daw_json_assert_weak( rng.has_more( ), ErrorReason::UnexpectedEndOfData,
			                      rng );
			// TODO: fully unescape name
			auto const name = parse_name( rng );
			auto const name_pos =
			  locations.template find_name<( from_start ? 0 : pos )>( name );
			if( name_pos >= std::size( locations ) ) {
				// This is not a member we are concerned with
				(void)skip_value( rng );
				rng.clean_tail( );
				continue;
			}
			if( name_pos == pos ) {
				locations[pos].template set_range( rng );
				break;
			} else {
				// We are out of order, store position for later
				// OLDTODO:	use type knowledge to speed up skip
				// OLDTODO:	on skipped classes see if way to store
				// 				member positions so that we don't have to
				//				reparse them after
				// RESULT: storing preparsed is slower, don't try 3 times
				// it also limits the type of things we can parse potentially
				// Using locations to switch on BaseType is slower too
				locations[name_pos].set_range( skip_value( rng ) );

				rng.clean_tail( );
			}
		}
		return locations[pos].template get_range<Range>( ).with_allocator( rng );
	}
} // namespace DAW_JSON_NS::v3_0::json_details
