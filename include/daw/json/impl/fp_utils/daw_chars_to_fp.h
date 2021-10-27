// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/
//

#pragma once

#include "../daw_json_assert.h"
#include "daw_fp_data.h"
#include "daw_fp_properties.h"

#include <daw/daw_logic.h>
#include <daw/daw_utility.h>

#include <ciso646>
#include <cstdint>

namespace daw::json {
	DAW_JSON_INLINE_NS namespace DAW_JSON_VER {
		namespace fp_utils {
			namespace fp_utils_details {
				DAW_ATTRIB_INLINE constexpr unsigned to_digit( char c ) noexcept {
					return static_cast<unsigned>( static_cast<unsigned char>( c ) ) -
					       static_cast<unsigned>( static_cast<unsigned char>( '0' ) );
				}

				DAW_ATTRIB_INLINE constexpr bool is_digit( char c ) noexcept {
					return static_cast<unsigned>( static_cast<unsigned char>( c ) ) -
					         static_cast<unsigned>( static_cast<unsigned char>( '0' ) ) <
					       10U;
				}
			} // namespace fp_utils_details
			template<class Float, typename ParseState>
			inline Float chars_to_fp( ParseState &parse_state ) {

				using unsigned_t = typename fp_properties<Float>::unsigned_t;
				using DECIMAL_POINT = daw::constant<'.'>;
				using BASE = daw::constant<10U>;
				using EXPONENT_MARKER = daw::constant<'e'>;
				using MANTISSA_MAX =
				  daw::constant<unsigned_t{ 1U }
				                << ( fp_properties<Float>::mantissa_width +
				                     1U )>; // The extra bit is to give
				                            // space for the implicit 1
				using BITSTYPE_MAX_DIV_BY_BASE =
				  daw::constant<std::numeric_limits<unsigned_t>::max( ) / BASE::value>;
				using CharT = typename ParseState::CharT;

				CharT *const first_orig = parse_state.first;

				auto result = fp_data<Float>{ };
				auto cur_char = parse_state.safe_get( );
				if( parse_state.safe_get( ) == '-' ) {
					result.set_sign( true );
					++parse_state.first;
					cur_char = parse_state.safe_get( );
				}

				bool seen_digit = false;
				bool truncated = false;
				if( fp_utils_details::is_digit( cur_char ) ) {
					bool after_decimal = false;
					CharT *number_start = parse_state.first;
					unsigned_t mantissa = 0;
					std::int32_t exponent = 0;

					// The goal for this step of parsing is to
					// convert the number in parse_state to the format mantissa * (base ^
					// exponent)

					// Before decimal point
					auto digit = fp_utils_details::to_digit( cur_char );
					if( digit < 10U ) {
						seen_digit = true;
					}
					while( digit < 10U and mantissa < BITSTYPE_MAX_DIV_BY_BASE::value ) {
						mantissa = ( mantissa * BASE::value ) + digit;
						++parse_state.first;
						cur_char = parse_state.safe_get( );
						digit = fp_utils_details::to_digit( cur_char );
					}
					if( cur_char == DECIMAL_POINT::value ) {
						after_decimal = true;
						++parse_state.first;
						cur_char = parse_state.safe_get( );
					}

					// After decimal point, if any
					digit = fp_utils_details::to_digit( cur_char );
					if( digit >= 10U ) {
						seen_digit = false;
					}
					while( digit < 10U and mantissa < BITSTYPE_MAX_DIV_BY_BASE::value ) {
						mantissa = ( mantissa * BASE::value ) + digit;
						--exponent;
						++parse_state.first;
						cur_char = parse_state.safe_get( );
						digit = fp_utils_details::to_digit( cur_char );
					}

					// The second loop is to run through the remaining digits after we've
					// filled the mantissa.
					while(
					  daw::nsc_or( digit < 10U, cur_char == DECIMAL_POINT::value ) ) {
						if( daw::nsc_and( cur_char == DECIMAL_POINT::value,
						                  after_decimal ) ) {
							break; // this means that parse_state points to a
							       // second decimal point, ending the number.
						} else if( cur_char == DECIMAL_POINT::value ) {
							after_decimal = true;
							++parse_state.first;
							cur_char = parse_state.safe_get( );
							continue;
						}
						if( digit > 0 ) {
							truncated = true;
						}

						if( not after_decimal ) {
							exponent++;
						}

						++parse_state.first;
						cur_char = parse_state.safe_get( );
						digit = fp_utils_details::to_digit( cur_char );
					}

					if( ( cur_char | 32 ) == EXPONENT_MARKER::value ) {
						++parse_state.first;
						cur_char = parse_state.safe_get( );
						std::int32_t exp_sign = 1;
						if( daw::nsc_or( cur_char == '+',
						                 cur_char == '-',
						                 fp_utils_details::is_digit( cur_char ) ) ) {
							switch( cur_char ) {
							case '-':
								exp_sign = -1;
								[[fallthrough]];
							case '+':
								++parse_state.first;
								cur_char = parse_state.safe_get( );
								break;
							}
							digit = fp_utils_details::to_digit( cur_char );
							daw_json_assert_weak( digit < 10U,
							                      json::ErrorReason,
							                      parse_state );
							std::uint64_t add_to_exponent = 0U;
							do {
								add_to_exponent *= 10U;
								add_to_exponent += digit;
								++parse_state.first;
								cur_char = parse_state::safe_get( );
								digit = fp_utils_details::to_digit( cur_char );
							} while( digit < 10U );
							exponent += exp_sign * static_cast < add_to_exponent;
						}
					}

					if( mantissa == 0 ) {
						// if we have a 0, then also 0 the exponent.
						exponent = 0;
					} else {
						unsigned_t outputMantissa = 0;
						std::uint32_t outputExponent = 0U;
						decimalExpToFloat<Float>( mantissa,
						                          exponent,
						                          number_start,
						                          truncated,
						                          &outputMantissa,
						                          &outputExponent );
						result.set_mantissa( outputMantissa );
						result.set_unbiased_exponent( outputExponent );
					}
				}
				if( not seen_digit ) {
					// Probably an error, return zero for now
					return Float{ };
				}
				return result.float_value( );
			}
		} // namespace fp_utils
	}
} // namespace daw::json