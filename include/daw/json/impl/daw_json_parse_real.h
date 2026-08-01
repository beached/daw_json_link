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
			template<bool skip_end_check, typename Unsigned>
			DAW_ATTRIB_FLATINLINE constexpr void
			parse_digits_until_last( daw::not_null<char const *> first,
			                         daw::not_null<char const *> const last,
			                         Unsigned &DAW_RESTRICT v ) {
				auto value = v;
				if constexpr( skip_end_check ) {
					auto dig = parse_digit( *first );
					while( dig < 10U ) {
						value *= 10U;
						value += dig;
						++first;
						dig = parse_digit( *first );
					}
				} else {
					while( DAW_LIKELY( first < last ) ) {
						value *= 10U;
						value += parse_digit( *first );
						++first;
					}
				}
				v = value;
			}

			template<typename Unsigned>
			constexpr std::size_t count_digits( Unsigned value ) {
				if( DAW_LIKELY( value == 0 ) ) {
					DAW_LIKELY_BRANCH
					return 0;
				}
				if constexpr( sizeof( Unsigned ) <= sizeof( std::uint64_t ) ) {
					if( value >= 10000000000000000000ULL ) {
						return 20;
					}
					if( value >= 1000000000000000000ULL ) {
						return 19;
					}
					if( value >= 100000000000000000ULL ) {
						return 18;
					}
					if( value >= 10000000000000000ULL ) {
						return 17;
					}
					if( value >= 1000000000000000ULL ) {
						return 16;
					}
					if( value >= 100000000000000ULL ) {
						return 15;
					}
					if( value >= 10000000000000ULL ) {
						return 14;
					}
					if( value >= 1000000000000ULL ) {
						return 13;
					}
					if( value >= 100000000000ULL ) {
						return 12;
					}
					if( value >= 10000000000ULL ) {
						return 11;
					}
					if( value >= 1000000000ULL ) {
						return 10;
					}
					if( value >= 100000000ULL ) {
						return 9;
					}
					if( value >= 10000000ULL ) {
						return 8;
					}
					if( value >= 1000000ULL ) {
						return 7;
					}
					if( value >= 100000ULL ) {
						return 6;
					}
					if( value >= 10000ULL ) {
						return 5;
					}
					if( value >= 1000ULL ) {
						return 4;
					}
					if( value >= 100ULL ) {
						return 3;
					}
					if( value >= 10ULL ) {
						return 2;
					}
					return 1;
				} else {
					std::size_t count = 1;
					value /= Unsigned{ 10U };
					while( value > 0 ) {
						++count;
						value /= Unsigned{ 10U };
					}
					return count;
				}
			}

			template<typename Unsigned>
			[[nodiscard]] DAW_ATTRIB_FLATINLINE constexpr daw::not_null<char const *>
			parse_digits_while_number( daw::not_null<char const *> first,
			                           daw::not_null<char const *> const last,
			                           Unsigned &DAW_RESTRICT v ) {

				if( DAW_UNLIKELY( first >= last ) ) {
					DAW_UNLIKELY_BRANCH
					return first;
				}
				auto const sig_dig_in_use = count_digits( v );

				auto const last_pos =
				  (std::min)( { std::distance( first, last ),
				                static_cast<std::ptrdiff_t>( daw::digits10<Unsigned> -
				                                             sig_dig_in_use ) } );
				daw::not_null const new_last = std::next( first.get( ), last_pos );

				auto value = v;

				unsigned dig = 10U;
				do {
					dig = parse_digit( *first );
					if( dig >= 10U ) {
						break;
					}
					value *= 10U;
					value += dig;
					++first;
				} while( first < new_last );
				if( first < last and dig < 10U ) {
					++first;
				}
				while( first < last ) {
					dig = parse_digit( *first );
					if( dig >= 10U ) {
						break;
					}
					++first;
				}
				v = value;
				return first;
			}

			/// @brief Check if we have more significant digits that can be stored
			/// in the type, usually uint64_t
			template<typename ParseState, typename Result,
			         typename max_storage_digits>
			[[nodiscard]] constexpr bool should_use_fallback(
			  [[maybe_unused]] daw::not_null<char const *> whole_first,
			  [[maybe_unused]] daw::not_null<char const *> const whole_last,
			  [[maybe_unused]] char const *fract_first,
			  [[maybe_unused]] char const *fract_last ) {
				if constexpr( std::is_floating_point_v<Result> and
				              ParseState::precise_ieee754 ) {
					return DAW_UNLIKELY(
					  ( ( whole_last - whole_first ) +
					    ( fract_first ? fract_last - fract_first : 0 ) ) >
					  max_storage_digits::value );
				} else {
					return false;
				}
			}

			inline constexpr std::size_t eisellemire_max_digits = 19;

			template<typename Signed>
			[[nodiscard]] constexpr bool
			append_discarded_digits( char const *first, char const *last,
			                         std::uint64_t &significant_digits,
			                         Signed &exponent ) {
				if( first == nullptr ) {
					return false;
				}

				auto retained_digits = count_digits( significant_digits );
				bool discarded_nonzero = false;
				while( first < last ) {
					auto const digit = parse_digit( *first );
					if( digit >= 10U ) {
						break;
					}
					++first;
					if( retained_digits < eisellemire_max_digits ) {
						significant_digits =
						  significant_digits * std::uint64_t{ 10 } + digit;
						// Leading zeroes do not consume significant-digit capacity.
						if( significant_digits != 0 ) {
							++retained_digits;
						}
						if( exponent > std::numeric_limits<Signed>::lowest( ) ) {
							--exponent;
						}
					} else {
						discarded_nonzero |= digit != 0;
					}
				}
				return discarded_nonzero;
			}

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

			template<typename Result, typename ParseState>
			[[nodiscard]] DAW_ATTRIB_INLINE static constexpr Result
			parse_real_known( ParseState &parse_state ) {
				// [-]WHOLE[.FRACTION][(e|E)[+|-]EXPONENT]
				daw_json_assert_weak(
				  parse_state.has_more( ) and
				    parse_policy_details::is_number_start( parse_state.front( ) ),
				  ErrorReason::InvalidNumberStart,
				  parse_state );

				daw::not_null<char const *> whole_first = parse_state.first;
				char const *whole_last = parse_state.class_first
				                           ? parse_state.class_first
				                           : parse_state.class_last;
				char const *fract_first =
				  parse_state.class_first ? parse_state.class_first + 1 : nullptr;
				char const *fract_last = parse_state.class_last;
				char const *exp_first =
				  parse_state.class_last ? parse_state.class_last + 1 : nullptr;
				daw::not_null<char const *> const exp_last = parse_state.last;

				if( parse_state.class_first == nullptr ) {
					if( parse_state.class_last == nullptr ) {
						whole_last = parse_state.last;
					} else {
						whole_last = parse_state.class_last;
					}
				} else if( parse_state.class_last == nullptr ) {
					fract_last = parse_state.last;
				}
				char const *const all_whole_last = whole_last;
				char const *const all_fract_first = fract_first;
				char const *const all_fract_last = fract_last;

				using max_storage_digits = daw::constant<static_cast<std::ptrdiff_t>(
				  daw::digits10<std::uint64_t> )>;

				Result const sign = [&] {
					if( *whole_first == '-' ) {
						++whole_first;
						return static_cast<Result>( -1.0 );
					}
					return static_cast<Result>( 1.0 );
				}( );

				bool use_fallback =
				  should_use_fallback<ParseState, Result, max_storage_digits>(
				    whole_first, whole_last, fract_first, fract_last );

				using max_exponent = daw::constant<static_cast<std::ptrdiff_t>(
				  daw::max_digits10<Result> + 1 )>;
				using unsigned_t =
				  daw::conditional_t<max_storage_digits::value >= max_exponent::value,
				                     std::uint64_t,
				                     Result>;

				using signed_t =
				  typename daw::conditional_t<std::is_floating_point_v<unsigned_t>,
				                              daw::traits::identity<unsigned_t>,
				                              std::make_signed<unsigned_t>>::type;
				std::intmax_t whole_exponent_available = whole_last - whole_first;
				std::intmax_t fract_exponent_available =
				  fract_first ? fract_last - fract_first : 0;
				signed_t exponent = 0;

				if( whole_exponent_available > max_exponent::value ) {
					whole_last = whole_first + max_exponent::value;
					whole_exponent_available -= max_exponent::value;
					fract_exponent_available = 0;
					fract_first = nullptr;
					exponent = whole_exponent_available;
				} else {
					whole_exponent_available =
					  max_exponent::value - whole_exponent_available;
					if constexpr( ParseState::precise_ieee754 ) {
						use_fallback |= DAW_UNLIKELY( fract_exponent_available >
						                              whole_exponent_available );
					}
					if( whole_exponent_available < fract_exponent_available ) {
						fract_exponent_available = whole_exponent_available;
					}
					exponent = -fract_exponent_available;
					fract_last = fract_first + fract_exponent_available;
				}

				unsigned_t significant_digits = 0;
				parse_digits_until_last<( ParseState::is_zero_terminated_string or
				                          ParseState::is_unchecked_input )>(
				  whole_first, whole_last, significant_digits );
				if( fract_first ) {
					parse_digits_until_last<( ParseState::is_zero_terminated_string or
					                          ParseState::is_unchecked_input )>(
					  fract_first, fract_last, significant_digits );
				}

				if( exp_first and ( exp_last - exp_first ) > 0 ) {
					signed_t const exp_sign = [&] {
						switch( *exp_first ) {
						case '-':
							++exp_first;
							daw_json_assert_weak( exp_first < exp_last,
							                      ErrorReason::InvalidNumber );
							return -1;
						case '+':
							daw_json_assert_weak( exp_first < exp_last,
							                      ErrorReason::InvalidNumber );
							++exp_first;
							return 1;
						default:
							return 1;
						}
					}( );
					exponent += to_signed(
					  [&] {
						  unsigned_t exp_result = 0;
						  if constexpr( ParseState::is_zero_terminated_string ) {
							  auto dig = parse_digit( *exp_first );
							  while( dig < 10U ) {
								  ++exp_first;
								  exp_result *= 10U;
								  exp_result += dig;
								  dig = parse_digit( *exp_first );
							  }
						  } else {
							  if( exp_first < exp_last ) {
								  auto dig = parse_digit( *exp_first );
								  do {
									  if( dig >= 10U ) {
										  break;
									  }
									  ++exp_first;
									  exp_result *= 10U;
									  exp_result += dig;
									  if( exp_first >= exp_last ) {
										  break;
									  }
									  dig = parse_digit( *exp_first );
								  } while( true );
							  }
						  }
						  return exp_result;
					  }( ),
					  exp_sign );
				}
				if constexpr( std::is_floating_point_v<Result> and
				              ParseState::precise_ieee754 ) {
					// On std floating point types, check for conditions that cannot be
					// precisely calculated using the normal method and use the fallback
					// method(usually strtod/from_chars)
					use_fallback |= exponent > 22;
					use_fallback |= exponent < -22;
					if constexpr( std::is_same_v<Result, float> or
					              std::is_same_v<Result, double> ) {
						use_fallback |=
						  significant_digits >
						  ( std::uint64_t{ 1 } << std::numeric_limits<Result>::digits );
					}
					if( std::is_same_v<Result, long double> or
					    DAW_UNLIKELY( use_fallback ) ) {
						if constexpr( std::is_same_v<Result, float> or
						              std::is_same_v<Result, double> ) {
							bool discarded_nonzero = append_discarded_digits(
							  whole_last, all_whole_last, significant_digits, exponent );
							if( all_fract_first != nullptr ) {
								auto const *discarded_fract_first =
								  fract_first == nullptr ? all_fract_first : fract_last;
								discarded_nonzero |=
								  append_discarded_digits( discarded_fract_first,
								                           all_fract_last,
								                           significant_digits,
								                           exponent );
							}
							return parse_truncated_lemire<Result>( sign < Result{ 0 },
							                                       exponent,
							                                       significant_digits,
							                                       discarded_nonzero,
							                                       parse_state.first,
							                                       parse_state.last );
						} else {
							static_assert( std::is_same_v<Result, long double> );
							return json_details::parse_with_strtod<Result>(
							  parse_state.first, parse_state.last );
						}
					}
				}
				return sign *
				       power10<Result>( ParseState::exec_tag,
				                        static_cast<Result>( significant_digits ),
				                        exponent );
			}

			template<typename Result, typename ParseState>
			[[nodiscard]] DAW_ATTRIB_INLINE static constexpr Result
			parse_real_unknown( ParseState &parse_state ) {
				// [-]WHOLE[.FRACTION][(e|E)[+|-]EXPONENT]
				daw_json_assert_weak(
				  parse_state.has_more( ) and
				    parse_policy_details::is_number_start( parse_state.front( ) ),
				  ErrorReason::InvalidNumberStart,
				  parse_state );

				[[maybe_unused]] daw::not_null<char const *> const orig_first =
				  parse_state.first;
				[[maybe_unused]] daw::not_null<char const *> const orig_last =
				  parse_state.last;

				auto const sign = static_cast<Result>(
				  parse_policy_details::validate_signed_first( parse_state ) );

				using max_storage_digits = daw::constant<static_cast<std::int64_t>(
				  daw::digits10<std::uint64_t> )>;
				using max_exponent = daw::constant<static_cast<std::int64_t>(
				  daw::max_digits10<Result> + 1 )>;
				using unsigned_t =
				  daw::conditional_t<max_storage_digits::value >= max_exponent::value,
				                     std::uint64_t,
				                     Result>;
				using signed_t =
				  daw::conditional_t<max_storage_digits::value >= max_exponent::value,
				                     std::int64_t,
				                     Result>;

				daw::not_null<char const *> first = parse_state.first;
				daw::not_null<char const *> const last = parse_state.last;
				daw::not_null<char const *> const whole_last =
				  parse_state.first +
				  (std::min)( { parse_state.last - parse_state.first,
				                static_cast<std::ptrdiff_t>( max_exponent::value ) } );

				unsigned_t significant_digits = 0;
				char const *discarded_whole_first = nullptr;
				char const *discarded_whole_last = nullptr;
				char const *discarded_fract_first = nullptr;
				char const *discarded_fract_last = nullptr;
				daw::not_null<char const *> last_char = parse_digits_while_number(
				  first.get( ), whole_last.get( ), significant_digits );
				auto const sig_digit_count = last_char - parse_state.first;
				bool use_strtod =
				  std::is_floating_point_v<Result> and ParseState::precise_ieee754 and
				  DAW_UNLIKELY( sig_digit_count > max_storage_digits::value );
				signed_t exponent_p1 = [&] {
					if( DAW_UNLIKELY( last_char >= whole_last ) ) {
						if constexpr( std::is_floating_point_v<Result> and
						              ParseState::precise_ieee754 ) {
							use_strtod = true;
						}
						// We have sig digits we cannot parse because there isn't enough
						// room in a std::uint64_t
						daw::not_null<char const *> ptr =
						  skip_digits<( ParseState::is_zero_terminated_string or
						                ParseState::is_unchecked_input )>( last_char,
						                                                   last );
						discarded_whole_first = last_char.get( );
						discarded_whole_last = ptr.get( );
						auto const diff = ptr - last_char;

						last_char = ptr;
						if( significant_digits == 0 ) {
							return signed_t{ 0 };
						}
						return static_cast<signed_t>( diff );
					}
					return signed_t{ 0 };
				}( );

				first = last_char;
				if( ( ParseState::is_zero_terminated_string or
				      ParseState::is_unchecked_input or
				      DAW_LIKELY( first < parse_state.last ) ) and
				    *first == '.' ) {
					++first;
					if( exponent_p1 != 0 ) {
						if( first < parse_state.last ) {
							auto const discarded_first = first;
							first =
							  skip_digits<( ParseState::is_zero_terminated_string or
							                ParseState::is_unchecked_input )>( first, last );
							discarded_fract_first = discarded_first.get( );
							discarded_fract_last = first.get( );
						}
					} else {
						daw::not_null<char const *> fract_last =
						  first + (std::min)( parse_state.last - first,
						                      static_cast<std::ptrdiff_t>(
						                        max_exponent::value -
						                        ( first - parse_state.first ) ) );

						last_char = parse_digits_while_number(
						  first.get( ), fract_last.get( ), significant_digits );
						exponent_p1 -= static_cast<signed_t>( last_char - first );
						first = last_char;
						if( daw::nsc_and( first >= fract_last, first < last ) ) {
							auto new_first =
							  skip_digits<( ParseState::is_zero_terminated_string or
							                ParseState::is_unchecked_input )>( first, last );
							discarded_fract_first = first.get( );
							discarded_fract_last = new_first.get( );
							if constexpr( std::is_floating_point_v<Result> and
							              ParseState::precise_ieee754 ) {
								use_strtod |= new_first > first;
							}
							first = new_first;
						}
					}
				}

				signed_t const exponent_p2 = [&] {
					if( ( ParseState::is_unchecked_input or first < parse_state.last ) and
					    ( ( *first | 0x20 ) == 'e' ) ) {
						++first;
						signed_t const exp_sign = [&] {
							daw_json_assert_weak( ( ParseState::is_zero_terminated_string or
							                        first < parse_state.last ),
							                      ErrorReason::UnexpectedEndOfData,
							                      parse_state.copy( first ) );
							switch( *first ) {
							case '+':
								++first;
								daw_json_assert_weak( ( first < parse_state.last ) and
								                        ( parse_digit( *first ) < 10U ),
								                      ErrorReason::InvalidNumber );
								return signed_t{ 1 };
							case '-':
								++first;
								daw_json_assert_weak( first < parse_state.last and
								                        parse_digit( *first ) < 10U,
								                      ErrorReason::InvalidNumber );
								return signed_t{ -1 };
							default:
								daw_json_assert_weak( parse_policy_details::is_number( *first ),
								                      ErrorReason::InvalidNumber );
								return signed_t{ 1 };
							}
						}( );
						daw_json_assert_weak( first < parse_state.last,
						                      ErrorReason::UnexpectedEndOfData,
						                      parse_state );
						unsigned_t exp_tmp = 0;
						last_char =
						  parse_digits_while_number( first.get( ), last.get( ), exp_tmp );
						first = last_char;
						return to_signed( exp_tmp, exp_sign );
					}
					return signed_t{ 0 };
				}( );
				auto exponent = [&] {
					if constexpr( ParseState::is_unchecked_input or
					              not std::is_floating_point_v<Result> ) {
						return exponent_p1 + exponent_p2;
					} else {
						if( bool const matching_signs =
						      ( ( exponent_p1 < 0 ) == ( exponent_p2 < 0 ) );
						    not matching_signs ) {

							return exponent_p1 + exponent_p2;
						}
						auto const s = exponent_p1 < 0 ? signed_t{ -1 } : signed_t{ 1 };
						if( s < 0 ) {
							if( DAW_UNLIKELY( ( daw::min_value<signed_t> - exponent_p1 ) >
							                  exponent_p2 ) ) {
								// We don't have inf, but we can just saturate it to min as it
								// will be 0 anyways for the other result
								return daw::min_value<signed_t>;
							}
							return exponent_p1 + exponent_p2;
						}
						auto const r = static_cast<unsigned_t>( exponent_p1 ) +
						               static_cast<unsigned_t>( exponent_p2 );
						if( DAW_UNLIKELY(
						      r > static_cast<unsigned_t>( daw::max_value<signed_t> ) ) ) {
							return daw::max_value<signed_t>;
						}
						return static_cast<signed_t>( r );
					}
				}( );
				parse_state.first = first;

				if constexpr( std::is_floating_point_v<Result> and
				              ParseState::precise_ieee754 ) {
					use_strtod |= DAW_UNLIKELY( exponent > 22 );
					use_strtod |= DAW_UNLIKELY( exponent < -22 );
					if constexpr( std::is_same_v<Result, float> or
					              std::is_same_v<Result, double> ) {
						use_strtod |= DAW_UNLIKELY(
						  significant_digits >
						  ( std::uint64_t{ 1 } << std::numeric_limits<Result>::digits ) );
					}
					if( DAW_UNLIKELY( use_strtod ) ) {
						if constexpr( std::is_same_v<Result, float> or
						              std::is_same_v<Result, double> ) {
							bool discarded_nonzero =
							  append_discarded_digits( discarded_whole_first,
							                           discarded_whole_last,
							                           significant_digits,
							                           exponent );
							discarded_nonzero |=
							  append_discarded_digits( discarded_fract_first,
							                           discarded_fract_last,
							                           significant_digits,
							                           exponent );
							return parse_truncated_lemire<Result>( sign < Result{ 0 },
							                                       exponent,
							                                       significant_digits,
							                                       discarded_nonzero,
							                                       orig_first,
							                                       first );
						} else {
							static_assert( std::is_same_v<Result, long double> );
							return json_details::parse_with_strtod<Result>( orig_first,
							                                                orig_last );
						}
					}
				}
				return sign *
				       power10<Result>( ParseState::exec_tag,
				                        static_cast<Result>( significant_digits ),
				                        exponent );
			}

			template<typename Result, bool KnownRange, typename ParseState>
			[[nodiscard]] constexpr Result parse_real( ParseState &parse_state ) {
				if constexpr( KnownRange ) {
					return parse_real_known<Result>( parse_state );
				} else {
					return parse_real_unknown<Result>( parse_state );
				}
			}
		} // namespace json_details
	} // namespace DAW_JSON_VER
} // namespace daw::json
