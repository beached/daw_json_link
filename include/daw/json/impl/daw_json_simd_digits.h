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
			namespace simd_digits_details {
				/// The value of 8 digits, 0 to 9, in the low lanes of digits, most
				/// significant first
				[[nodiscard]] DAW_ATTRIB_INLINE std::uint32_t
				combine_8_digits( daw::simd::vec128<std::uint8_t> digits ) noexcept {
					using v8 = daw::simd::vec128<std::uint8_t>;
					using v16 = daw::simd::vec128<std::uint16_t>;
					// 10 * first + second for each pair of digits, then 100 * first +
					// second for each pair of those, then 10000 * first + second
					auto const pairs = mul_add_pairs( digits, v8::pattern( 10, 1 ) );
					auto const quads = mul_add_pairs( pairs, v16::pattern( 100, 1 ) );
					auto const eight =
					  mul_add_pairs( pack( quads ), v16::pattern( 10000, 1 ) );
					return eight.get<0>( );
				}
			} // namespace simd_digits_details

			/// The value of the 8 ASCII decimal digits at ptr, most significant
			/// first.  ptr must point to 8 readable digits
			[[nodiscard]] DAW_ATTRIB_INLINE std::uint32_t
			simd_parse_8_digits( char const *ptr ) noexcept {
				using v8 = daw::simd::vec128<std::uint8_t>;
				return simd_digits_details::combine_8_digits( v8::load_low( ptr ) -
				                                              v8( '0' ) );
			}

			/// When the 8 bytes at ptr are all ASCII decimal digits, set value to
			/// their value, most significant first, and return true.  ptr must
			/// point to 8 readable bytes.  The check and the conversion share one
			/// load and stay in vector registers
			[[nodiscard]] DAW_ATTRIB_INLINE bool
			simd_try_parse_8_digits( char const *ptr, std::uint32_t &value ) noexcept {
				using v8 = daw::simd::vec128<std::uint8_t>;
				auto const digits = v8::load_low( ptr ) - v8( '0' );
				// Lanes above 9 stay non-zero.  Only the low 8 lanes are checked, the
				// high half is 0 - '0'
				if( sub_sat( digits, v8( 9 ) ).low64( ) != 0 ) {
					return false;
				}
				value = simd_digits_details::combine_8_digits( digits );
				return true;
			}
		} // namespace json_details
	} // namespace DAW_JSON_VER
} // namespace daw::json
#endif
