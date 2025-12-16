// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/daw_json_link
//

#pragma once

#include "daw/json/impl/version.h"

#include <daw/daw_attributes.h>

namespace daw::json {
	inline namespace DAW_JSON_VER {
		namespace json_details {
			[[nodiscard]] DAW_ATTRIB_INLINE constexpr unsigned parse_digit( char c ) {
				return static_cast<unsigned>( static_cast<unsigned char>( c ) ) -
				       static_cast<unsigned>( static_cast<unsigned char>( '0' ) );
			}

			namespace parsed_constants {
				inline constexpr unsigned decimal_char = parse_digit( '.' );
				inline constexpr unsigned e_char = parse_digit( 'e' );
				inline constexpr unsigned E_char = parse_digit( 'E' );
				inline constexpr unsigned plus_char = parse_digit( '+' );
				inline constexpr unsigned minus_char = parse_digit( '-' );
			} // namespace parsed_constants
		} // namespace json_details
	} // namespace DAW_JSON_VER
} // namespace daw::json
