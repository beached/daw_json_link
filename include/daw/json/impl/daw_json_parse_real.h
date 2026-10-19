// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/daw_json_link
//

#pragma once

#include "daw/json/impl/version.h"

#include "daw/json/impl/daw_fp_fallback.h"
#include "daw/json/impl/daw_json_assert.h"
#include "daw/json/impl/daw_json_parse_policy_policy_details.h"
#include "daw/json/impl/daw_json_parse_real_decimal.h"
#include "daw/json/impl/daw_json_parse_real_eisellemire.h"
#include "daw/json/impl/daw_json_parse_real_power10.h"
#include "daw/json/impl/daw_json_parse_unsigned_int.h"
#include "daw/json/impl/daw_json_skip.h"
#include "daw/json/impl/daw_json_type_options.h"

#include <daw/daw_arith_traits.h>
#include <daw/daw_bit_cast.h>
#include <daw/daw_cxmath.h>
#include <daw/daw_likely.h>
#include <daw/daw_not_null.h>
#include <daw/daw_restrict.h>
#include <daw/daw_utility.h>

#include <cstddef>
#include <cstdint>
#include <limits>
#include <type_traits>

namespace daw::json {
	inline namespace DAW_JSON_VER {
		namespace json_details {
			/// @brief std floating point types narrower than float, e.g.
			/// std::float16_t and std::bfloat16_t.  These are parsed as float and
			/// then narrowed.
			template<typename Real, bool = std::is_floating_point_v<Real>>
			inline constexpr bool is_narrow_real_v = false;

			template<typename Real>
			inline constexpr bool is_narrow_real_v<Real, true> =
			  std::numeric_limits<Real>::is_iec559 and
			  std::numeric_limits<Real>::radix == 2 and
			  std::numeric_limits<Real>::digits <
			    std::numeric_limits<float>::digits and
			  std::numeric_limits<Real>::max_exponent <=
			    std::numeric_limits<float>::max_exponent and
			  std::numeric_limits<Real>::min_exponent >=
			    std::numeric_limits<float>::min_exponent;

			/// @brief Types that IEEE754Precise::yes can correctly round.  Others,
			/// such as user defined types, use the general path
			template<typename Real>
			inline constexpr bool is_precise_real_v =
			  std::is_floating_point_v<Real> and not is_narrow_real_v<Real> and
			  ( is_lemire_capable_v<Real> or has_precise_fallback_v<Real> );

			template<typename Result, typename Signed>
			[[nodiscard]] constexpr Result parse_truncated_lemire(
			  bool negative, Signed exponent, std::uint64_t significant_digits,
			  bool discarded_nonzero, daw::not_null<char const *> number_first,
			  daw::not_null<char const *> number_last ) {
				auto const lower = json_details::parse_real_eisellemire<Result>(
				  negative, exponent, significant_digits );
				if( not discarded_nonzero ) {
					return lower;
				}

				// significant_digits contains at most 19 decimal digits, so adding
				// one cannot overflow a uint64_t.
				auto const upper = json_details::parse_real_eisellemire<Result>(
				  negative, exponent, significant_digits + std::uint64_t{ 1 } );
				if( lower == upper ) {
					return lower;
				}

				return json_details::parse_json_real_exact<Result>(
				  negative, number_first, number_last );
			}

			namespace single_pass_details {
				/// Significant decimal digits that always fit in a std::uint64_t
				inline constexpr std::ptrdiff_t max_u64_digits = 19;

				/// The explicit exponent stops growing here.  Anything this large is
				/// already 0 or infinity for every supported type, and it keeps the
				/// exponent arithmetic from overflowing
				inline constexpr std::int64_t max_explicit_exponent = 0x1000'0000;

				/// Largest |exponent| where 10^exponent is exact in Result, so that
				/// mantissa * 10^exponent is a single correctly rounded operation
				template<typename Result>
				inline constexpr std::int64_t max_exact_power10 =
				  std::is_same_v<Result, long double> and
				      std::numeric_limits<long double>::digits == 113
				    ? 48
				  : std::is_same_v<Result, long double> and
				      std::numeric_limits<long double>::digits == 64
				    ? 27
				  : std::is_same_v<lemire_compute_t<Result>, float> ? 10
				                                                    : 22;

				/// Accumulate a run of digits into value, 8 at a time when possible
				template<typename Accumulator, typename Iterator>
				[[nodiscard]] DAW_ATTRIB_INLINE constexpr Iterator
				parse_digit_run( Iterator first, Iterator const last,
				                 Accumulator &value ) {
					while( last - first >= 8 and is_made_of_eight_digits_cx( first ) ) {
						value = value * static_cast<Accumulator>( 100'000'000U ) +
						        static_cast<Accumulator>(
						          static_cast<std::uint64_t>( parse_8_digits( first ) ) );
						first += 8;
					}
					while( first < last ) {
						auto const digit = parse_digit( *first );
						if( digit >= 10U ) {
							break;
						}
						value = value * static_cast<Accumulator>( 10U ) +
						        static_cast<Accumulator>( digit );
						++first;
					}
					return first;
				}

				struct significand_t {
					std::uint64_t mantissa = 0;
					/// value is mantissa * 10^exponent, before the explicit exponent
					std::int64_t exponent = 0;
					std::ptrdiff_t significant_digits = 0;
					bool discarded_nonzero = false;
				};

				/// Rescan a number with more than 19 digits.  Leading zeros do not
				/// count, and only the first 19 significant digits are kept.
				template<typename Iterator>
				[[nodiscard]] constexpr significand_t
				parse_long_significand( Iterator whole_first, Iterator const whole_last,
				                        Iterator fract_first,
				                        Iterator const fract_last ) {
					significand_t result{ };
					bool seen_nonzero = false;
					for( ; whole_first < whole_last; ++whole_first ) {
						auto const digit = parse_digit( *whole_first );
						seen_nonzero |= digit != 0;
						if( not seen_nonzero ) {
							continue;
						}
						++result.significant_digits;
						if( result.significant_digits <= max_u64_digits ) {
							result.mantissa = result.mantissa * 10U + digit;
						} else {
							++result.exponent;
							result.discarded_nonzero |= digit != 0;
						}
					}
					for( ; fract_first < fract_last; ++fract_first ) {
						auto const digit = parse_digit( *fract_first );
						seen_nonzero |= digit != 0;
						if( not seen_nonzero ) {
							--result.exponent;
							continue;
						}
						++result.significant_digits;
						if( result.significant_digits <= max_u64_digits ) {
							result.mantissa = result.mantissa * 10U + digit;
							--result.exponent;
						} else {
							result.discarded_nonzero |= digit != 0;
						}
					}
					return result;
				}
			} // namespace single_pass_details

			/// @brief Finish a parsed number: handle more than 19 digits, then use
			/// the power10 fast path, Eisel-Lemire, or the exact fallback.  The
			/// digit ranges exclude the sign, decimal point and exponent.
			template<typename Result, typename ParseState, typename Accumulator,
			         typename Iterator>
			[[nodiscard]] DAW_ATTRIB_INLINE constexpr Result finish_real(
			  int sign, Accumulator mantissa, std::int64_t explicit_exponent,
			  Iterator whole_first, Iterator whole_last, Iterator fract_first,
			  Iterator fract_last, Iterator number_first, Iterator number_last ) {
				namespace sp = single_pass_details;
				constexpr bool integer_accumulator =
				  std::is_same_v<Accumulator, std::uint64_t>;
				auto const fract_digits = fract_last - fract_first;
				auto const digit_count = ( whole_last - whole_first ) + fract_digits;
				std::int64_t exponent =
				  explicit_exponent - static_cast<std::int64_t>( fract_digits );
				std::ptrdiff_t significant_digits = digit_count;
				bool discarded_nonzero = false;
				if( DAW_UNLIKELY( digit_count > sp::max_u64_digits ) ) {
					auto const sig = sp::parse_long_significand(
					  whole_first, whole_last, fract_first, fract_last );
					significant_digits = sig.significant_digits;
					if constexpr( integer_accumulator ) {
						// The accumulator wrapped, use the first 19 significant digits
						mantissa = sig.mantissa;
						exponent = explicit_exponent + sig.exponent;
						discarded_nonzero = sig.discarded_nonzero;
					}
				}

				if constexpr( is_precise_real_v<Result> and
				              ParseState::precise_ieee754 ) {
					if constexpr( is_lemire_capable_v<Result> ) {
						static_assert( integer_accumulator );
						using compute_t = lemire_compute_t<Result>;
						if( DAW_LIKELY( significant_digits <= sp::max_u64_digits and
						                mantissa <= ( std::uint64_t{ 1 }
						                              << daw::digits<compute_t> ) and
						                exponent <= sp::max_exact_power10<Result> and
						                exponent >= -sp::max_exact_power10<Result> ) ) {
							return static_cast<Result>( sign ) *
							       power10<Result>( ParseState::exec_tag,
							                        static_cast<Result>( mantissa ),
							                        exponent );
						}
						return static_cast<Result>(
						  parse_truncated_lemire<compute_t>( sign < 0,
						                                     exponent,
						                                     mantissa,
						                                     discarded_nonzero,
						                                     number_first,
						                                     number_last ) );
					} else {
						// Exact when the digits fit and 10^exponent is exact in Result
						if( DAW_LIKELY( significant_digits <= sp::max_u64_digits and
						                exponent <= sp::max_exact_power10<Result> and
						                exponent >= -sp::max_exact_power10<Result> ) ) {
							return static_cast<Result>( sign ) *
							       power10<Result>( ParseState::exec_tag,
							                        static_cast<Result>( mantissa ),
							                        exponent );
						}
						return parse_with_strtod<Result>( number_first, number_last );
					}
				} else if constexpr( daw::digits<Result> > daw::digits<double> ) {
					// The double table is only accurate to double's precision
					return static_cast<Result>( sign ) *
					       power10_wide( static_cast<Result>( mantissa ), exponent );
				} else {
					return static_cast<Result>( sign ) *
					       power10<Result>( ParseState::exec_tag,
					                        static_cast<Result>( mantissa ),
					                        exponent );
				}
			}

			/// @brief Parse a JSON number in one pass over the characters.  The
			/// digits, decimal point and exponent are each scanned once, then the
			/// value is finished by the fast power10 path, Eisel-Lemire, or the
			/// exact fallback.  parse_state.first is left at the end of the number.
			template<typename Result, typename ParseState>
			[[nodiscard]] constexpr Result
			parse_real_single_pass( ParseState &parse_state ) {
				namespace sp = single_pass_details;
				// [-]WHOLE[.FRACTION][(e|E)[+|-]EXPONENT]
				daw_json_assert_weak(
				  parse_state.has_more( ) and
				    parse_policy_details::is_number_start( parse_state.front( ) ),
				  ErrorReason::InvalidNumberStart,
				  parse_state );

				using iterator_t = typename ParseState::iterator;
				constexpr bool skip_end_check = ParseState::is_zero_terminated_string or
				                                ParseState::is_unchecked_input;
				// Types with more precision than 19 digits accumulate in Result so
				// that user types like multiprecision decimals keep every digit
				constexpr bool integer_accumulator =
				  sp::max_u64_digits >= daw::max_digits10<Result> + 1;
				using accumulator_t =
				  std::conditional_t<integer_accumulator, std::uint64_t, Result>;

				iterator_t const number_first = parse_state.first;
				auto const sign =
				  parse_policy_details::validate_signed_first( parse_state );
				iterator_t first = parse_state.first;
				iterator_t const last = parse_state.last;

				accumulator_t mantissa = accumulator_t( );
				iterator_t const whole_first = first;
				first = sp::parse_digit_run( first, last, mantissa );
				iterator_t const whole_last = first;
				// JSON requires at least one whole digit, e.g. "-" and "-.1" are
				// invalid, and no leading zeros.  validate_signed_first only checks
				// the leading zero when there is no '-'
				daw_json_assert_weak( whole_first < whole_last,
				                      ErrorReason::InvalidNumber,
				                      parse_state.copy( whole_first ) );
				daw_json_assert_weak( *whole_first != '0' or
				                        whole_last - whole_first == 1,
				                      ErrorReason::InvalidNumberStart,
				                      parse_state.copy( whole_first ) );
				iterator_t fract_first = first;
				if( ( skip_end_check or first < last ) and *first == '.' ) {
					++first;
					fract_first = first;
					first = sp::parse_digit_run( first, last, mantissa );
					// and at least one digit after the decimal point, e.g. "1." is
					// invalid
					daw_json_assert_weak( fract_first < first,
					                      ErrorReason::InvalidNumber,
					                      parse_state.copy( first ) );
				}
				iterator_t const fract_last = first;

				std::int64_t explicit_exponent = 0;
				if( ( skip_end_check or first < last ) and
				    ( ( *first | 0x20 ) == 'e' ) ) {
					++first;
					daw_json_assert_weak( skip_end_check or first < last,
					                      ErrorReason::UnexpectedEndOfData,
					                      parse_state.copy( first ) );
					bool negative_exponent = false;
					switch( *first ) {
					case '-':
						negative_exponent = true;
						[[fallthrough]];
					case '+':
						++first;
						daw_json_assert_weak( first < last and parse_digit( *first ) < 10U,
						                      ErrorReason::InvalidNumber );
						break;
					default:
						daw_json_assert_weak( parse_policy_details::is_number( *first ),
						                      ErrorReason::InvalidNumber );
					}
					while( first < last ) {
						auto const digit = parse_digit( *first );
						if( digit >= 10U ) {
							break;
						}
						if( explicit_exponent < sp::max_explicit_exponent ) {
							explicit_exponent = explicit_exponent * 10 + digit;
						}
						++first;
					}
					if( negative_exponent ) {
						explicit_exponent = -explicit_exponent;
					}
				}
				parse_state.first = first;

				return finish_real<Result, ParseState>( sign,
				                                        mantissa,
				                                        explicit_exponent,
				                                        whole_first,
				                                        whole_last,
				                                        fract_first,
				                                        fract_last,
				                                        number_first,
				                                        first );
			}

			namespace single_pass_details {
				/// Accumulate exactly [first, last), which is known to be digits
				template<typename Accumulator, typename Iterator>
				DAW_ATTRIB_INLINE constexpr void
				parse_known_digits( Iterator first, Iterator const last,
				                    Accumulator &value ) {
					while( last - first >= 8 ) {
						value = value * static_cast<Accumulator>( 100'000'000U ) +
						        static_cast<Accumulator>(
						          static_cast<std::uint64_t>( parse_8_digits( first ) ) );
						first += 8;
					}
					for( ; first < last; ++first ) {
						value = value * static_cast<Accumulator>( 10U ) +
						        static_cast<Accumulator>( parse_digit( *first ) );
					}
				}
			} // namespace single_pass_details

			/// @brief Parse a number whose bounds are already known, e.g. from
			/// skip_number or the SIMD number iterator.  [first, last) is exactly
			/// the number, class_first is the decimal point and class_last the
			/// exponent marker, or null when absent.  With the digit counts known
			/// up front, the digit runs are parsed without checking each character
			/// or searching for the decimal point and exponent.
			template<typename Result, typename ParseState>
			[[nodiscard]] constexpr Result
			parse_real_known_bounds( ParseState &parse_state ) {
				namespace sp = single_pass_details;
				daw_json_assert_weak(
				  parse_state.has_more( ) and
				    parse_policy_details::is_number_start( parse_state.front( ) ),
				  ErrorReason::InvalidNumberStart,
				  parse_state );

				using iterator_t = typename ParseState::iterator;
				constexpr bool integer_accumulator =
				  sp::max_u64_digits >= daw::max_digits10<Result> + 1;
				using accumulator_t =
				  std::conditional_t<integer_accumulator, std::uint64_t, Result>;

				iterator_t const number_first = parse_state.first;
				iterator_t const last = parse_state.last;
				iterator_t const decimal_point = parse_state.class_first;
				iterator_t const exponent_marker = parse_state.class_last;
				auto const sign =
				  parse_policy_details::validate_signed_first( parse_state );

				iterator_t const whole_first = parse_state.first;
				iterator_t const whole_last =
				  decimal_point ? decimal_point
				                : ( exponent_marker ? exponent_marker : last );
				iterator_t const fract_first =
				  decimal_point ? decimal_point + 1 : whole_last;
				iterator_t const fract_last =
				  decimal_point ? ( exponent_marker ? exponent_marker : last )
				                : whole_last;
				// Same rules as parse_real_single_pass: at least one whole digit, no
				// leading zeros, and at least one digit after the decimal point
				daw_json_assert_weak( whole_first < whole_last,
				                      ErrorReason::InvalidNumber,
				                      parse_state.copy( whole_first ) );
				daw_json_assert_weak( *whole_first != '0' or
				                        whole_last - whole_first == 1,
				                      ErrorReason::InvalidNumberStart,
				                      parse_state.copy( whole_first ) );
				daw_json_assert_weak( decimal_point == nullptr or
				                        fract_first < fract_last,
				                      ErrorReason::InvalidNumber,
				                      parse_state.copy( fract_first ) );

				std::int64_t explicit_exponent = 0;
				if( exponent_marker != nullptr ) {
					iterator_t first = exponent_marker + 1;
					daw_json_assert_weak( first < last,
					                      ErrorReason::InvalidNumber,
					                      parse_state.copy( first ) );
					bool const negative_exponent = *first == '-';
					if( negative_exponent or *first == '+' ) {
						++first;
					}
					daw_json_assert_weak( first < last and parse_digit( *first ) < 10U,
					                      ErrorReason::InvalidNumber,
					                      parse_state.copy( first ) );
					for( ; first < last; ++first ) {
						if( explicit_exponent < sp::max_explicit_exponent ) {
							explicit_exponent =
							  explicit_exponent * 10 + parse_digit( *first );
						}
					}
					if( negative_exponent ) {
						explicit_exponent = -explicit_exponent;
					}
				}
				parse_state.first = last;

				accumulator_t mantissa = accumulator_t( );
				// More than 19 digits overflow the integer accumulator, and
				// finish_real rescans those
				if( not integer_accumulator or
				    DAW_LIKELY( ( whole_last - whole_first ) +
				                  ( fract_last - fract_first ) <=
				                sp::max_u64_digits ) ) {
					sp::parse_known_digits( whole_first, whole_last, mantissa );
					sp::parse_known_digits( fract_first, fract_last, mantissa );
				}
				return finish_real<Result, ParseState>( sign,
				                                        mantissa,
				                                        explicit_exponent,
				                                        whole_first,
				                                        whole_last,
				                                        fract_first,
				                                        fract_last,
				                                        number_first,
				                                        last );
			}

			template<typename Result, bool KnownRange, typename ParseState>
			[[nodiscard]] constexpr Result parse_real( ParseState &parse_state );

			/// @brief True when f is exactly halfway between two values of the
			/// narrower Result.  f is already rounded, so the exact value may have
			/// been on either side and narrowing f could double round.
			template<typename Result>
			[[nodiscard]] constexpr bool is_halfway_for_narrowing( float f ) {
				auto const bits = DAW_BIT_CAST( std::uint32_t, f );
				auto const biased_exponent =
				  static_cast<std::int32_t>( ( bits >> 23U ) & 0xFFU );
				if( biased_exponent == 0xFF ) {
					return false;
				}
				std::uint32_t mantissa = bits & 0x7F'FFFFU;
				std::int32_t exponent = -126;
				if( biased_exponent != 0 ) {
					mantissa |= 0x80'0000U;
					exponent = biased_exponent - 127;
				}
				// Bits of f below the precision of Result, more when the value is in
				// the subnormal range of Result
				DAW_CPP23_STATIC_LOCAL constexpr std::int32_t result_min_exponent =
				  std::numeric_limits<Result>::min_exponent - 1;
				std::int32_t dropped_bits = std::numeric_limits<float>::digits -
				                            std::numeric_limits<Result>::digits;
				if( exponent < result_min_exponent ) {
					dropped_bits += result_min_exponent - exponent;
				}
				if( dropped_bits > std::numeric_limits<float>::digits ) {
					return false;
				}
				auto const half = std::uint32_t{ 1 }
				                  << static_cast<std::uint32_t>( dropped_bits - 1 );
				return ( mantissa & ( ( half << 1U ) - 1U ) ) == half;
			}

			template<typename Result, bool KnownRange, typename ParseState>
			[[nodiscard]] constexpr Result
			parse_narrow_real( ParseState &parse_state ) {
				daw::not_null<char const *> const number_first = parse_state.first;
				float const f = parse_real<float, KnownRange>( parse_state );
				if constexpr( ParseState::precise_ieee754 ) {
					if( DAW_UNLIKELY( is_halfway_for_narrowing<Result>( f ) ) ) {
						auto const number_last =
						  KnownRange ? parse_state.last : parse_state.first;
						return json_details::parse_json_real_exact<Result>(
						  f < 0.0f, number_first, number_last );
					}
				}
				return static_cast<Result>( f );
			}

			template<typename Result, bool KnownRange, typename ParseState>
			[[nodiscard]] constexpr Result parse_real( ParseState &parse_state ) {
				if constexpr( is_narrow_real_v<Result> ) {
					return parse_narrow_real<Result, KnownRange>( parse_state );
				} else if constexpr( KnownRange ) {
					// The skipper already found the end, decimal point and exponent
					return parse_real_known_bounds<Result>( parse_state );
				} else {
					return parse_real_single_pass<Result>( parse_state );
				}
			}
		} // namespace json_details
	} // namespace DAW_JSON_VER
} // namespace daw::json
