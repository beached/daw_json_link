// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/daw_json_link
//

#pragma once

#include "version.h"

#include "daw_json_assert.h"
#include "power_of_five_128_table.h"

#include <daw/daw_attributes.h>
#include <daw/daw_cpp_feature_check.h>
#include <daw/daw_cxmath.h>

#include <cstdlib>
#include <limits>
#include <system_error>
#include <type_traits>

#if DAW_HAS_MSVC_VER_GTE( 1930 )
#include <__msvc_int128.hpp>
#endif

#if not defined( DAW_JSON_USE_STRTOD )
#include <charconv>
#endif

namespace daw::json {
	inline namespace DAW_JSON_VER {
		namespace json_details {
#if defined( DAW_HAS_GCC_LIKE )
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpedantic"
			using u128 = unsigned __int128;
#pragma GCC diagnostic pop
#elif DAW_HAS_MSVC_VER_GTE( 1930 )
			using u128 = std::_Unsigned128;
#endif
			DAW_CONSTEVAL u128 operator""_daw_json_u128( char const *str ) {
				u128 result = 0;
				char c = *str;
				while( c != '\0' ) {
					result *= 10U;
					result += static_cast<unsigned>( static_cast<unsigned char>( c ) ) -
					          static_cast<unsigned>( static_cast<unsigned char>( '0' ) );
					++str;
					c = *str;
				}
				return result;
			}

			struct adjusted_mantissa {
				std::uint64_t mantissa = 0;
				std::int32_t power2 = 0; // a negative value indicates an invalid result

				explicit adjusted_mantissa( ) = default;

				[[nodiscard]] constexpr bool
				operator==( adjusted_mantissa const &o ) const {
					return mantissa == o.mantissa and power2 == o.power2;
				}

				[[nodiscard]] constexpr bool
				operator!=( adjusted_mantissa const &o ) const {
					return mantissa != o.mantissa or power2 != o.power2;
				}
			};

			namespace float_info {
				template<typename T>
				inline constexpr auto mantissa_explicit_bits_v = daw::undefined_v<T>;

				template<>
				inline constexpr auto mantissa_explicit_bits_v<double> = 52;
				template<>
				inline constexpr auto mantissa_explicit_bits_v<float> = 23;

				template<typename T>
				inline constexpr auto smallest_power_of_ten_v = daw::undefined_v<T>;

				template<>
				inline constexpr auto smallest_power_of_ten_v<double> = -342;

				template<>
				inline constexpr auto smallest_power_of_ten_v<float> = -65;

				template<typename T>
				inline constexpr auto smallest_power_of_five_v =
				  smallest_power_of_ten_v<T>;

				template<typename T>
				inline auto max_exponent_round_to_even_v = daw::undefined_v<T>;
				template<>
				inline constexpr auto max_exponent_round_to_even_v<double> = 23;
				template<>
				inline constexpr auto max_exponent_round_to_even_v<float> = 10;

				template<typename T>
				inline auto min_exponent_round_to_even_v = daw::undefined_v<T>;
				template<>
				inline constexpr auto min_exponent_round_to_even_v<double> = -4;
				template<>
				inline constexpr auto min_exponent_round_to_even_v<float> = -17;

				template<typename T>
				inline auto minimum_exponent_v = daw::undefined_v<T>;
				template<>
				inline constexpr auto minimum_exponent_v<double> = -1023;
				template<>
				inline constexpr auto minimum_exponent_v<float> = -127;

				template<typename T>
				inline auto infinite_power_v = daw::undefined_v<T>;
				template<>
				inline constexpr auto infinite_power_v<double> = 0x7FF;
				template<>
				inline constexpr auto infinite_power_v<float> = 0xFF;
			} // namespace float_info

			DAW_ATTRIB_INLINE constexpr u128 full_multiplication( std::uint64_t a,
			                                                      std::uint64_t b ) {
				return static_cast<u128>( a ) * static_cast<u128>( b );
			}

			template<typename Result = std::uint64_t>
			DAW_ATTRIB_INLINE constexpr Result get_hi( u128 value ) {
				return static_cast<Result>( value >> 64 );
			}

			template<typename Result = std::uint64_t>
			DAW_ATTRIB_INLINE constexpr Result get_lo( u128 value ) {
				return static_cast<Result>( static_cast<std::uint64_t>( value ) );
			}

			DAW_ATTRIB_INLINE constexpr u128 set_hi( u128 &v0, std::uint64_t v1 ) {
				auto const v1b = static_cast<u128>( v1 ) << 64;
				v0 = get_lo<u128>( v0 ) | v1b;
				return v0;
			}

			DAW_ATTRIB_INLINE constexpr u128 set_lo( u128 &v0, std::uint64_t v1 ) {
				v0 = get_hi<u128>( v0 ) << 64;
				v0 = v0 | static_cast<u128>( v1 );
				return v0;
			}

			template<int bit_precision>
			DAW_ATTRIB_INLINE constexpr u128
			compute_product_approximation( std::int64_t q, std::uint64_t w ) {
				auto const index =
				  2 * int( q - float_info::smallest_power_of_five_v<double> );
				// For small values of q, e.g., q in [0,27], the answer is always exact
				// because The line u128 firstproduct = full_multiplication(w,
				// power_of_five_128[index]); gives the exact answer.
				auto firstproduct = full_multiplication( w, pow5_tbl[index] );
				static_assert( ( bit_precision >= 0 ) && ( bit_precision <= 64 ),
				               " precision should  be in (0,64]" );
				constexpr auto precision_mask =
				  ( bit_precision < 64 )
				    ? ( static_cast<std::uint64_t>( 0xFFFFFFFFFFFFFFFFULL ) >>
				        bit_precision )
				    : static_cast<std::uint64_t>( 0xFFFFFFFFFFFFFFFFULL );
				if( ( get_hi( firstproduct ) & precision_mask ) ==
				    precision_mask ) { // could further guard with  (lower + w < lower)
					// regarding the second product, we only need secondproduct.high, but
					// our expectation is that the compiler will optimize this extra work
					// away if needed.
					auto secondproduct = full_multiplication( w, pow5_tbl[index + 1] );
					set_lo( firstproduct,
					        get_lo( firstproduct ) + get_hi( secondproduct ) );
					if( get_hi( secondproduct ) > get_lo( firstproduct ) ) {
						set_hi( firstproduct, get_hi( firstproduct ) + 1 );
					}
				}
				return firstproduct;
			}
			/**
			 * For q in (0,350), we have that
			 *  f = (((152170 + 65536) * q ) >> 16);
			 * is equal to
			 *   floor(p) + q
			 * where
			 *   p = log(5**q)/log(2) = q * log(5)/log(2)
			 *
			 * For negative values of q in (-400,0), we have that
			 *  f = (((152170 + 65536) * q ) >> 16);
			 * is equal to
			 *   -ceil(p) + q
			 * where
			 *   p = log(5**-q)/log(2) = -q * log(5)/log(2)
			 */
			DAW_ATTRIB_INLINE constexpr std::int32_t power( int32_t q ) {
				return ( ( ( 152170 + 65536 ) * q ) >> 16 ) + 63;
			}

			// w * 10 ** q, without rounding the representation up.
			// the power2 in the exponent will be adjusted by invalid_am_bias.
			/*template<typename Real>
			DAW_ATTRIB_INLINE constexpr adjusted_mantissa
			compute_error( std::int64_t q, std::uint64_t w ) {
			  auto const lz = daw::cxmath::count_leading_zeroes( w );
			  w <<= lz;
			  auto const product = compute_product_approximation<
			    float_info::mantissa_explicit_bits_v<Real> + 3>( q, w );
			  return compute_error_scaled<Real>( q, product.high, lz );
			}*/

			template<typename Real>
			DAW_ATTRIB_NOINLINE [[nodiscard]] constexpr adjusted_mantissa
			compute_float( std::uint64_t significant_digits, std::int64_t exponent ) {

				auto const lz = daw::cxmath::count_leading_zeroes( significant_digits );
				significant_digits <<= lz;

				// The required precision is float_info::mantissa_explicit_bits_v + 3
				// because
				// 1. We need the implicit bit
				// 2. We need an extra bit for rounding purposes
				// 3. We might lose a bit due to the "upperbit" routine (result too
				// small, requiring a shift)

				auto product = compute_product_approximation<
				  float_info::mantissa_explicit_bits_v<Real> + 3>( exponent,
				                                                   significant_digits );
				// The computed 'product' is always sufficient.
				// Mathematical proof:
				// Noble Mushtak and Daniel Lemire, Fast Number Parsing Without Fallback
				// (to appear) See script/mushtak_lemire.py

				// The "compute_product_approximation" function can be slightly slower
				// than a branchless approach: u128 product = compute_product(q, w);
				// but in practice, we can win big with the
				// compute_product_approximation if its additional branch is easily
				// predicted. Which is best is data specific.
				auto const upperbit = static_cast<int>( product.high >> 63 );
				auto answer = adjusted_mantissa( );
				answer.mantissa =
				  product.high >>
				  ( upperbit + 64 - float_info::mantissa_explicit_bits_v<Real> - 3 );

				answer.power2 = static_cast<std::int32_t>(
				  power( static_cast<std::int32_t>( exponent ) ) + upperbit - lz -
				  float_info::minimum_exponent_v<Real> );
				if( answer.power2 <= 0 ) { // we have a subnormal?
					// Here have that answer.power2 <= 0 so -answer.power2 >= 0
					if( -answer.power2 + 1 >=
					    64 ) { // if we have more than 64 bits below the minimum exponent,
						         // you have a zero for sure.
						answer.power2 = 0;
						answer.mantissa = 0;
						// result should be zero
						return answer;
					}
					// next line is safe because -answer.power2 + 1 < 64
					answer.mantissa >>= -answer.power2 + 1;
					// Thankfully, we can't have both "round-to-even" and subnormals
					// because "round-to-even" only occurs for powers close to 0.
					answer.mantissa += ( answer.mantissa & 1 ); // round up
					answer.mantissa >>= 1;
					// There is a weird scenario where we don't have a subnormal but just.
					// Suppose we start with 2.2250738585072013e-308, we end up
					// with 0x3fffffffffffff x 2^-1023-53 which is technically subnormal
					// whereas 0x40000000000000 x 2^-1023-53  is normal. Now, we need to
					// round up 0x3fffffffffffff x 2^-1023-53  and once we do, we are no
					// longer subnormal, but we can only know this after rounding. So we
					// only declare a subnormal if we are smaller than the threshold.
					answer.power2 =
					  ( answer.mantissa < (std::uint64_t{ 1U }
					                       << float_info::mantissa_explicit_bits_v<Real>))
					    ? 0
					    : 1;
					return answer;
				}

				// usually, we round *up*, but if we fall right in between and and we
				// have an even basis, we need to round down We are only concerned with
				// the cases where 5**q fits in single 64-bit word.
				if( ( product.low <= 1 ) &&
				    (exponent >= float_info::min_exponent_round_to_even_v<Real>)&&(
				      exponent <= float_info::max_exponent_round_to_even_v<
				                    Real>)&&( ( answer.mantissa & 3 ) ==
				                              1 ) ) { // we may fall between two floats!
					// To be in-between two floats we need that in doing
					//   answer.mantissa = product.high >> (upperbit + 64 -
					//   float_info::mantissa_explicit_bits_v - 3);
					// ... we dropped out only zeroes. But if this happened, then we can
					// go back!!!
					if( ( answer.mantissa
					      << ( upperbit + 64 -
					           float_info::mantissa_explicit_bits_v<Real> - 3 ) ) ==
					    product.high ) {
						answer.mantissa &=
						  ~std::uint64_t{ 1 }; // flip it so that we do not round up
					}
				}

				answer.mantissa += ( answer.mantissa & 1 ); // round up
				answer.mantissa >>= 1;
				if( answer.mantissa >=
				    (std::uint64_t{ 2U }
				     << float_info::mantissa_explicit_bits_v<Real>)) {
					answer.mantissa = ( std::uint64_t{ 1U }
					                    << float_info::mantissa_explicit_bits_v<Real> );
					answer.power2++; // undo previous addition
				}

				answer.mantissa &= ~( std::uint64_t{ 1U }
				                      << float_info::mantissa_explicit_bits_v<Real> );
				if( answer.power2 >= float_info::infinite_power_v<Real> ) { // infinity
					answer.power2 = float_info::infinite_power_v<Real>;
					answer.mantissa = 0;
				}

				return answer;
			}

			/// @brief When normal FP parsing cannot handle the value, this method
			/// used as a fallback for std floating point types.  For others, it
			/// provides either a customization point or will call the overload found
			/// via ADL
			template<typename Real>
			DAW_ATTRIB_NOINLINE [[nodiscard]] Real
			parse_with_strtod( char const *first, char const *last ) {
				static_assert( std::is_floating_point_v<Real>,
				               "Unexpected type passed to parse_with_strtod" );
#if defined( DAW_JSON_USE_STRTOD )
				(void)last;
				char **end = nullptr;
				if constexpr( std::is_same_v<Real, float> ) {
					return static_cast<Real>( std::strtof( first, end ) );
				} else if( std::is_same_v<Real, double> ) {
					return static_cast<Real>( std::strtod( first, end ) );
				} else {
					return static_cast<Real>( std::strtold( first, end ) );
				}
#else
				Real result;
				auto fc_res = std::from_chars( first, last, result );
				if( fc_res.ec == std::errc::result_out_of_range ) {
					if( *first == '-' ) {
						return -std::numeric_limits<Real>::infinity( );
					}
					return std::numeric_limits<Real>::infinity( );
				}
				daw_json_ensure( fc_res.ec == std::errc( ),
				                 ErrorReason::InvalidNumber );
				return result;
#endif
			}
		} // namespace json_details
	} // namespace DAW_JSON_VER
} // namespace daw::json
