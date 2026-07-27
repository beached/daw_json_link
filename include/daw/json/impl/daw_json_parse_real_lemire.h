// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/daw_json_link
//

#pragma once

#include "daw/json/impl/version.h"

#include "daw/json/impl/power_of_five_128_table.h"

#include <daw/daw_attributes.h>
#include <daw/daw_bit_cast.h>
#include <daw/daw_cxmath.h>
#include <daw/daw_is_constant_evaluated.h>

#include <cstddef>
#include <cstdint>
#include <limits>
#include <type_traits>

#if defined( DAW_HAS_MSVC ) and defined( _M_X64 )
#include <intrin.h>
#endif

namespace daw::json {
	inline namespace DAW_JSON_VER {
		namespace json_details {
			namespace lemire_details {
				struct uint128 {
					std::uint64_t low;
					std::uint64_t high;
				};

				template<typename Real>
				struct binary_format;

				template<>
				struct binary_format<double> {
					using uint_type = std::uint64_t;
					static constexpr std::int32_t mantissa_bits = 52;
					static constexpr std::int32_t exponent_bias = 1023;
					static constexpr std::int32_t infinite_power = 0x7FF;
					static constexpr std::int32_t sign_bit = 63;
					static constexpr std::int32_t smallest_power_of_ten = -342;
					static constexpr std::int32_t largest_power_of_ten = 308;
					static constexpr std::int32_t min_round_to_even = -4;
					static constexpr std::int32_t max_round_to_even = 23;
				};

				template<>
				struct binary_format<float> {
					using uint_type = std::uint32_t;
					static constexpr std::int32_t mantissa_bits = 23;
					static constexpr std::int32_t exponent_bias = 127;
					static constexpr std::int32_t infinite_power = 0xFF;
					static constexpr std::int32_t sign_bit = 31;
					static constexpr std::int32_t smallest_power_of_ten = -64;
					static constexpr std::int32_t largest_power_of_ten = 38;
					static constexpr std::int32_t min_round_to_even = -17;
					static constexpr std::int32_t max_round_to_even = 10;
				};

				[[nodiscard]] constexpr uint128
				full_multiplication_generic( std::uint64_t lhs,
				                             std::uint64_t rhs ) noexcept {
					auto const lhs_hi = lhs >> 32U;
					auto const lhs_lo = lhs & 0xFFFF'FFFFULL;
					auto const rhs_hi = rhs >> 32U;
					auto const rhs_lo = rhs & 0xFFFF'FFFFULL;

					auto const lhs_hi_rhs_lo = lhs_hi * rhs_lo;
					auto const lhs_lo_rhs_lo = lhs_lo * rhs_lo;
					auto const middle = lhs_hi_rhs_lo + lhs_lo * rhs_hi;
					auto const middle_carry = middle < lhs_hi_rhs_lo;
					auto const low = lhs_lo_rhs_lo + ( middle << 32U );
					auto const high = lhs_hi * rhs_hi + ( middle >> 32U ) +
					                  ( static_cast<std::uint64_t>( middle_carry ) << 32U ) +
					                  static_cast<std::uint64_t>( low < lhs_lo_rhs_lo );
					return { low, high };
				}

				[[nodiscard]] DAW_ATTRIB_FLATINLINE constexpr uint128
				full_multiplication( std::uint64_t lhs, std::uint64_t rhs ) noexcept {
#if ( defined( __GNUC__ ) or defined( __clang__ ) ) and \
  defined( __SIZEOF_INT128__ )
					if( not DAW_IS_CONSTANT_EVALUATED_COMPAT( ) ) {
#if defined( __GNUC__ )
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpedantic"
#endif
						auto const result = static_cast<unsigned __int128>( lhs ) * rhs;
						return { static_cast<std::uint64_t>( result ),
						         static_cast<std::uint64_t>( result >> 64U ) };
#if defined( __GNUC__ )
#pragma GCC diagnostic pop
#endif
					}
#elif defined( DAW_HAS_MSVC ) and defined( _M_X64 )
					if( not DAW_IS_CONSTANT_EVALUATED_COMPAT( ) ) {
						uint128 result{ };
						result.low = _umul128( lhs, rhs, &result.high );
						return result;
					}
#endif
					return full_multiplication_generic( lhs, rhs );
				}

				template<typename Real>
				[[nodiscard]] DAW_ATTRIB_FLATINLINE constexpr uint128
				compute_product( std::int64_t exponent,
				                 std::uint64_t significant_digits ) noexcept {
					auto const index = static_cast<std::size_t>(
					  2 * ( exponent - ( -342 /* smallest cached power */ ) ) );
					auto product =
					  full_multiplication( significant_digits, pow5_tbl[index] );

					// Three extra bits are required: the implicit bit, the rounding bit,
					// and one bit that may be lost while normalizing the product.
					constexpr std::uint64_t precision_mask =
					  std::numeric_limits<std::uint64_t>::max( ) >>
					  ( binary_format<Real>::mantissa_bits + 3 );
					if( ( product.high & precision_mask ) == precision_mask ) {
						auto const second =
						  full_multiplication( significant_digits, pow5_tbl[index + 1] );
						product.low += second.high;
						product.high += static_cast<std::uint64_t>( second.high > product.low );
					}
					return product;
				}

				[[nodiscard]] constexpr std::int32_t
				binary_power( std::int32_t exponent ) noexcept {
					// floor(log2(10^exponent)) plus the cached-power normalization.
					return ( ( 217706 * exponent ) >> 16 ) + 63;
				}
			} // namespace lemire_details

			/// Convert the exact decimal value
			///   (-1 if negative else 1) * significant_digits * 10^exponent
			/// to the correctly rounded IEEE-754 binary32 or binary64 value.
			///
			/// significant_digits must contain all parsed significant digits.  If the
			/// scanner discarded digits, it must resolve truncation before calling this
			/// function.
			template<typename Real = double>
			[[nodiscard]] DAW_ATTRIB_FLATINLINE constexpr Real
			parse_real_lemire( bool negative, std::int64_t exponent,
			                   std::uint64_t significant_digits ) noexcept {
				static_assert( std::is_same_v<Real, float> or
				                 std::is_same_v<Real, double>,
				               "parse_real_lemire supports float and double" );
				static_assert( std::numeric_limits<Real>::is_iec559 );
				using format = lemire_details::binary_format<Real>;
				using uint_type = typename format::uint_type;

				std::uint64_t mantissa = 0;
				std::int32_t power2 = 0;
				if( significant_digits == 0 or
				    exponent < format::smallest_power_of_ten ) {
					// The sign is applied while packing so underflow preserves -0.0.
				} else if( exponent > format::largest_power_of_ten ) {
					power2 = format::infinite_power;
				} else {
					auto const leading_zeroes = static_cast<std::int32_t>(
					  daw::cxmath::count_leading_zeroes( significant_digits ) );
					auto const normalized_digits = significant_digits << leading_zeroes;
					auto const product =
					  lemire_details::compute_product<Real>( exponent, normalized_digits );
					auto const upper_bit = static_cast<std::int32_t>( product.high >> 63U );
					auto const shift =
					  upper_bit + 64 - format::mantissa_bits - 3;

					mantissa = product.high >> shift;
					power2 = lemire_details::binary_power(
					           static_cast<std::int32_t>( exponent ) ) +
					         upper_bit - leading_zeroes + format::exponent_bias;

					if( power2 <= 0 ) {
						if( -power2 + 1 >= 64 ) {
							mantissa = 0;
							power2 = 0;
						} else {
							mantissa >>= -power2 + 1;
							mantissa += mantissa & 1U;
							mantissa >>= 1U;
							power2 =
							  mantissa < ( std::uint64_t{ 1 } << format::mantissa_bits )
							    ? 0
							    : 1;
						}
					} else {
						// Correct exact halfway cases to round-to-even before the usual
						// round-up operation.
						if( product.low <= 1 and exponent >= format::min_round_to_even and
						    exponent <= format::max_round_to_even and
						    ( mantissa & 3U ) == 1U and
						    ( mantissa << shift ) == product.high ) {
							mantissa &= ~std::uint64_t{ 1 };
						}
						mantissa += mantissa & 1U;
						mantissa >>= 1U;
						if( mantissa >=
						    ( std::uint64_t{ 2 } << format::mantissa_bits ) ) {
							mantissa = std::uint64_t{ 1 } << format::mantissa_bits;
							++power2;
						}
						mantissa &=
						  ~( std::uint64_t{ 1 } << format::mantissa_bits );
						if( power2 >= format::infinite_power ) {
							mantissa = 0;
							power2 = format::infinite_power;
						}
					}
				}

				auto const bits = static_cast<uint_type>(
				  mantissa |
				  ( static_cast<std::uint64_t>( power2 ) << format::mantissa_bits ) |
				  ( static_cast<std::uint64_t>( negative ) << format::sign_bit ) );
				return DAW_BIT_CAST( Real, bits );
			}
		} // namespace json_details
	} // namespace DAW_JSON_VER
} // namespace daw::json
