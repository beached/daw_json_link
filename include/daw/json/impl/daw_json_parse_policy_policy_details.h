// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/daw_json_link
//

#pragma once

#include "version.h"

#include <daw/daw_attributes.h>

namespace daw::json {
	inline namespace DAW_JSON_VER {
		namespace parse_policy_details {
			template<char... keys>
			[[nodiscard]] DAW_ATTRIB_FLATINLINE inline constexpr bool in( char c ) {
				auto const eq = [c]( char k ) { return c == k; };
				return ( eq( keys ) | ... );
			}

			[[nodiscard]] DAW_ATTRIB_FLATINLINE inline constexpr bool
			at_end_of_item( char c ) {
				return static_cast<bool>( static_cast<unsigned>( c == ',' ) |
				                          static_cast<unsigned>( c == '}' ) |
				                          static_cast<unsigned>( c == ']' ) |
				                          static_cast<unsigned>( c == ':' ) |
				                          static_cast<unsigned>( c <= 0x20 ) );
			}

			[[nodiscard]] DAW_ATTRIB_FLATINLINE inline constexpr bool
			is_number( char c ) {
				return static_cast<unsigned>( static_cast<unsigned char>( c ) -
				                              static_cast<unsigned char>( '0' ) ) < 10U;
			}

			[[nodiscard]] DAW_ATTRIB_FLATINLINE inline constexpr bool
			is_number_start( char c ) {
				switch( c ) {
				case '0': // TODO: CONFORMANCE We are accepting starting with zero for now
				case '1':
				case '2':
				case '3':
				case '4':
				case '5':
				case '6':
				case '7':
				case '8':
				case '9':
				case '-':
					return true;
				}
				return false;
			}
		} // namespace parse_policy_details
	}   // namespace DAW_JSON_VER
} // namespace daw::json
