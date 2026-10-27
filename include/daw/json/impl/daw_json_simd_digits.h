// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/daw_json_link
//

#pragma once

#include "daw/json/impl/version.h"

#include "daw/simd/daw_simd.h"

#include <daw/daw_attributes.h>

#include <cstdint>

#if defined( DAW_HAS_SIMD )
namespace daw::json {
	inline namespace DAW_JSON_VER {
		namespace json_details {
			/// The value of the 8 ASCII decimal digits at ptr, most significant
			/// first.  ptr must point to 8 readable digits
			[[nodiscard]] DAW_ATTRIB_INLINE std::uint32_t
			simd_parse_8_digits( char const *ptr ) noexcept {
				using v8 = daw::simd::vec128<std::uint8_t>;
				using v16 = daw::simd::vec128<std::uint16_t>;
				auto const digits = v8::load_low( ptr ) - v8( '0' );
				// 10 * first + second for each pair of digits, then 100 * first +
				// second for each pair of those, then 10000 * first + second
				auto const pairs = mul_add_pairs( digits, v8::pattern( 10, 1 ) );
				auto const quads = mul_add_pairs( pairs, v16::pattern( 100, 1 ) );
				auto const eight =
				  mul_add_pairs( pack( quads ), v16::pattern( 10000, 1 ) );
				return eight.get<0>( );
			}
		} // namespace json_details
	} // namespace DAW_JSON_VER
} // namespace daw::json
#endif
