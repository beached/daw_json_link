// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/daw_json_link
//

#pragma once

#include <daw/daw_hide.h>

namespace daw::json::json_details {

	DAW_ATTRIBUTE_FLATTEN [[nodiscard]] static inline constexpr unsigned
	parse_digit( char c ) noexcept {
		return static_cast<unsigned>( static_cast<unsigned char>(
		  static_cast<unsigned char>( c ) - static_cast<unsigned char>( '0' ) ) );
	}

	namespace parsed_constants {
		static inline constexpr unsigned decimal_char = parse_digit( '.' );
		static inline constexpr unsigned e_char = parse_digit( 'e' );
		static inline constexpr unsigned E_char = parse_digit( 'E' );
		static inline constexpr unsigned plus_char = parse_digit( '+' );
		static inline constexpr unsigned minus_char = parse_digit( '-' );
	} // namespace parsed_constants
} // namespace daw::json::json_details
