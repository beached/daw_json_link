// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/daw_json_link
//

#pragma once

#include "daw/json/impl/version.h"

#include "daw/json/impl/daw_json_parse_real_eisellemire.h"
#include "daw/json/impl/daw_json_parse_unsigned_int.h"

#include <daw/daw_bit_cast.h>
#include <daw/daw_not_null.h>

#include <cstddef>
#include <cstdint>
#include <limits>
#include <type_traits>

namespace daw::json {
	inline namespace DAW_JSON_VER {
		namespace json_details {
			namespace decimal_details {
				// Simple Decimal Conversion, specialized for the rare case where a
				// long JSON significand straddles an IEEE-754 rounding boundary.
				// Binary64 requires at most 767 significant decimal digits to resolve
				// rounding, so one additional digit is retained.
				struct decimal_sequence {
					static constexpr std::size_t max_digits = 768;
					static constexpr std::int32_t decimal_point_limit = 2047;

					std::uint8_t digits[max_digits]{ };
					std::size_t digit_count = 0;
					std::int32_t decimal_point = 0;
					bool truncated = false;

					constexpr void push_digit( unsigned digit ) {
						if( digit_count < max_digits ) {
							digits[digit_count++] = static_cast<std::uint8_t>( digit );
						} else {
							truncated |= digit != 0;
						}
					}

					constexpr void trim( ) {
						while( digit_count != 0 and digits[digit_count - 1] == 0 ) {
							--digit_count;
						}
						if( digit_count == 0 ) {
							decimal_point = 0;
						}
					}

					// Compute decimal * 2^shift.  shift is at most 60, so each
					// digit-times-factor operation and its carry fit in uint64_t.
					constexpr void shift_left( unsigned shift ) {
						if( digit_count == 0 or shift == 0 ) {
							return;
						}

						constexpr std::size_t max_growth = 19;
						std::uint8_t result[max_digits + max_growth]{ };
						auto write = max_digits + max_growth;
						auto read = digit_count;
						std::uint64_t carry = 0;
						auto const factor = std::uint64_t{ 1 } << shift;

						while( read != 0 ) {
							--read;
							auto const value =
							  static_cast<std::uint64_t>( digits[read] ) * factor + carry;
							result[--write] = static_cast<std::uint8_t>( value % 10U );
							carry = value / 10U;
						}
						while( carry != 0 ) {
							result[--write] = static_cast<std::uint8_t>( carry % 10U );
							carry /= 10U;
						}

						auto const result_count = max_digits + max_growth - write;
						auto const growth = result_count - digit_count;
						decimal_point += static_cast<std::int32_t>( growth );

						auto const stored_count =
						  result_count < max_digits ? result_count : max_digits;
						for( std::size_t n = 0; n < stored_count; ++n ) {
							digits[n] = result[write + n];
						}
						for( auto n = stored_count; n < result_count; ++n ) {
							truncated |= result[write + n] != 0;
						}
						digit_count = stored_count;
						trim( );
					}

					// Compute decimal * 2^-shift.  shift is at most 60.
					constexpr void shift_right( unsigned shift ) {
						if( digit_count == 0 or shift == 0 ) {
							return;
						}

						std::size_t read = 0;
						std::size_t write = 0;
						std::uint64_t value = 0;
						while( ( value >> shift ) == 0 ) {
							if( read < digit_count ) {
								value =
								  value * 10U + static_cast<std::uint64_t>( digits[read++] );
							} else if( value == 0 ) {
								return;
							} else {
								do {
									value *= 10U;
									++read;
								} while( ( value >> shift ) == 0 );
								break;
							}
						}

						decimal_point -= static_cast<std::int32_t>( read - 1 );
						if( decimal_point < -decimal_point_limit ) {
							digit_count = 0;
							decimal_point = 0;
							truncated = false;
							return;
						}

						auto const mask = ( std::uint64_t{ 1 } << shift ) - 1U;
						while( read < digit_count ) {
							auto const digit = static_cast<std::uint8_t>( value >> shift );
							value = 10U * ( value & mask ) +
							        static_cast<std::uint64_t>( digits[read++] );
							digits[write++] = digit;
						}
						while( value != 0 ) {
							auto const digit = static_cast<std::uint8_t>( value >> shift );
							value = 10U * ( value & mask );
							if( write < max_digits ) {
								digits[write++] = digit;
							} else {
								truncated |= digit != 0;
							}
						}
						digit_count = write;
						trim( );
					}

					[[nodiscard]] constexpr std::uint64_t round( ) const {
						if( digit_count == 0 or decimal_point < 0 ) {
							return 0;
						}
						if( decimal_point >= 19 ) {
							return std::numeric_limits<std::uint64_t>::max( );
						}

						auto const point = static_cast<std::size_t>( decimal_point );
						std::uint64_t result = 0;
						for( std::size_t n = 0; n < point; ++n ) {
							result *= 10U;
							if( n < digit_count ) {
								result += digits[n];
							}
						}

						bool round_up = false;
						if( point < digit_count ) {
							round_up = digits[point] >= 5;
							if( digits[point] == 5 and point + 1 == digit_count ) {
								round_up =
								  truncated or ( point != 0 and ( digits[point - 1] & 1U ) );
							}
						}
						return result + static_cast<std::uint64_t>( round_up );
					}
				};

				[[nodiscard]] constexpr std::int64_t
				saturating_add( std::int64_t lhs, std::int64_t rhs ) {
					constexpr auto max_value = std::numeric_limits<std::int64_t>::max( );
					constexpr auto min_value =
					  std::numeric_limits<std::int64_t>::lowest( );
					if( rhs > 0 and lhs > max_value - rhs ) {
						return max_value;
					}
					if( rhs < 0 and lhs < min_value - rhs ) {
						return min_value;
					}
					return lhs + rhs;
				}

				[[nodiscard]] constexpr std::int64_t
				parse_exponent( char const *first, char const *last ) {
					if( first == nullptr or first == last ) {
						return 0;
					}

					bool negative = false;
					if( *first == '+' or *first == '-' ) {
						negative = *first == '-';
						++first;
					}

					constexpr auto positive_limit = static_cast<std::uint64_t>(
					  std::numeric_limits<std::int64_t>::max( ) );
					constexpr auto negative_limit = positive_limit + 1U;
					auto const limit = negative ? negative_limit : positive_limit;
					std::uint64_t result = 0;
					bool overflow = false;
					while( first < last ) {
						auto const digit = parse_digit( *first++ );
						if( digit >= 10U ) {
							break;
						}
						if( result > ( limit - digit ) / 10U ) {
							result = limit;
							overflow = true;
						} else if( not overflow ) {
							result = result * 10U + digit;
						}
					}

					if( negative ) {
						if( result == negative_limit ) {
							return std::numeric_limits<std::int64_t>::lowest( );
						}
						return -static_cast<std::int64_t>( result );
					}
					return static_cast<std::int64_t>( result );
				}

				[[nodiscard]] constexpr decimal_sequence
				parse_json_decimal( daw::not_null<char const *> first,
				                    daw::not_null<char const *> const last ) {
					decimal_sequence result{ };
					if( *first == '-' ) {
						++first;
					}

					std::int64_t decimal_point = 0;
					bool seen_nonzero = false;
					while( first < last and parse_digit( *first ) < 10U ) {
						auto const digit = parse_digit( *first++ );
						if( seen_nonzero or digit != 0 ) {
							seen_nonzero = true;
							result.push_digit( digit );
							++decimal_point;
						}
					}

					if( first < last and *first == '.' ) {
						++first;
						while( first < last and parse_digit( *first ) < 10U ) {
							auto const digit = parse_digit( *first++ );
							if( seen_nonzero or digit != 0 ) {
								seen_nonzero = true;
								result.push_digit( digit );
							} else {
								--decimal_point;
							}
						}
					}

					char const *exponent_first = nullptr;
					if( first < last and ( *first == 'e' or *first == 'E' ) ) {
						exponent_first = ++first;
					}
					decimal_point = saturating_add(
					  decimal_point, parse_exponent( exponent_first, last.get( ) ) );

					if( not seen_nonzero ) {
						result.digit_count = 0;
						result.decimal_point = 0;
						result.truncated = false;
						return result;
					}

					if( decimal_point > decimal_sequence::decimal_point_limit ) {
						result.decimal_point = decimal_sequence::decimal_point_limit;
					} else if( decimal_point < -decimal_sequence::decimal_point_limit ) {
						result.decimal_point = -decimal_sequence::decimal_point_limit;
					} else {
						result.decimal_point = static_cast<std::int32_t>( decimal_point );
					}
					result.trim( );
					return result;
				}

				[[nodiscard]] constexpr unsigned
				shift_amount( std::int32_t decimal_places ) {
					constexpr unsigned powers[19] = {
					  0,  3,  6,  9,  13, 16, 19, 23, 26, 29,
					  33, 36, 39, 43, 46, 49, 53, 56, 59,
					};
					auto const places = static_cast<std::uint32_t>(
					  decimal_places < 0 ? -decimal_places : decimal_places );
					return places < 19U ? powers[places] : 60U;
				}

				template<typename Real>
				[[nodiscard]] constexpr Real pack_real( bool negative,
				                                        std::uint64_t mantissa,
				                                        std::int32_t power2 ) {
					using format = eisellemire_details::binary_format<Real>;
					using uint_type = typename format::uint_type;
					auto const bits = static_cast<uint_type>(
					  mantissa |
					  ( static_cast<std::uint64_t>( power2 ) << format::mantissa_bits ) |
					  ( static_cast<std::uint64_t>( negative ) << format::sign_bit ) );
					return DAW_BIT_CAST( Real, bits );
				}
			} // namespace decimal_details

			template<typename Real>
			[[nodiscard]] constexpr Real
			parse_json_real_exact( bool negative, daw::not_null<char const *> first,
			                       daw::not_null<char const *> last ) {
				static_assert( std::is_same_v<Real, float> or
				               std::is_same_v<Real, double> );
				using format = eisellemire_details::binary_format<Real>;
				auto decimal = decimal_details::parse_json_decimal( first, last );

				if( decimal.digit_count == 0 ) {
					return decimal_details::pack_real<Real>( negative, 0, 0 );
				}

				constexpr std::int32_t smallest_decimal_point =
				  std::is_same_v<Real, double> ? -324 : -45;
				constexpr std::int32_t largest_decimal_point =
				  std::is_same_v<Real, double> ? 310 : 40;
				if( decimal.decimal_point < smallest_decimal_point ) {
					return decimal_details::pack_real<Real>( negative, 0, 0 );
				}
				if( decimal.decimal_point >= largest_decimal_point ) {
					return decimal_details::pack_real<Real>(
					  negative, 0, format::infinite_power );
				}

				std::int32_t exponent2 = 0;
				while( decimal.decimal_point > 0 ) {
					auto const shift =
					  decimal_details::shift_amount( decimal.decimal_point );
					decimal.shift_right( shift );
					exponent2 += static_cast<std::int32_t>( shift );
				}
				while( decimal.decimal_point <= 0 ) {
					unsigned shift = 0;
					if( decimal.decimal_point == 0 ) {
						if( decimal.digits[0] >= 5 ) {
							break;
						}
						shift = decimal.digits[0] <= 1 ? 2U : 1U;
					} else {
						shift = decimal_details::shift_amount( decimal.decimal_point );
					}
					decimal.shift_left( shift );
					exponent2 -= static_cast<std::int32_t>( shift );
				}

				// decimal is now in [0.5, 1); IEEE formats use [1, 2).
				--exponent2;
				constexpr auto minimum_exponent = 1 - format::exponent_bias;
				while( exponent2 < minimum_exponent ) {
					auto shift = minimum_exponent - exponent2;
					if( shift > 60 ) {
						shift = 60;
					}
					decimal.shift_right( static_cast<unsigned>( shift ) );
					exponent2 += shift;
				}

				if( exponent2 - minimum_exponent + 1 >= format::infinite_power ) {
					return decimal_details::pack_real<Real>(
					  negative, 0, format::infinite_power );
				}

				decimal.shift_left(
				  static_cast<unsigned>( format::mantissa_bits + 1 ) );
				auto mantissa = decimal.round( );
				if( mantissa >=
				    ( std::uint64_t{ 1 } << ( format::mantissa_bits + 1 ) ) ) {
					decimal.shift_right( 1 );
					++exponent2;
					mantissa = decimal.round( );
					if( exponent2 - minimum_exponent + 1 >= format::infinite_power ) {
						return decimal_details::pack_real<Real>(
						  negative, 0, format::infinite_power );
					}
				}

				auto power2 = exponent2 - minimum_exponent + 1;
				if( mantissa < ( std::uint64_t{ 1 } << format::mantissa_bits ) ) {
					--power2;
				}
				mantissa &= ( std::uint64_t{ 1 } << format::mantissa_bits ) - 1U;
				return decimal_details::pack_real<Real>( negative, mantissa, power2 );
			}
		} // namespace json_details
	} // namespace DAW_JSON_VER
} // namespace daw::json
