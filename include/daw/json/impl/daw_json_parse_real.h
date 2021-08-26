// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/daw_json_link
//

#pragma once

#include "daw_fp_fallback.h"
#include "daw_json_assert.h"
#include "daw_json_parse_policy.h"
#include "daw_json_parse_real_power10.h"
#include "daw_json_parse_unsigned_int.h"
#include "version.h"

#include <daw/daw_cxmath.h>
#include <daw/daw_likely.h>
#include <daw/daw_utility.h>

#include <ciso646>
#include <cstddef>
#include <cstdint>
#include <limits>
#include <type_traits>

namespace daw::json {
	inline namespace DAW_JSON_VER {
		namespace json_details {
			template<bool skip_end_check, typename Unsigned>
			DAW_ATTRIB_FLATINLINE inline constexpr void
			parse_digits_until_last( char const *first, char const *const last,
			                         Unsigned &v ) {
				Unsigned value = v;
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

			template<bool skip_end_check, typename Unsigned, typename CharT>
			DAW_ATTRIB_FLATINLINE inline constexpr CharT *
			parse_digits_while_number( CharT *first, CharT *const last,
			                           Unsigned &v ) {

				// silencing gcc9 unused warning.  last is used inside if constexpr
				// blocks
				(void)last;

				Unsigned value = v;
				if constexpr( skip_end_check ) {
					for( auto dig = parse_digit( *first ); dig < 10U;
					     ++first, dig = parse_digit( *first ) ) {
						value *= 10U;
						value += dig;
					}
				} else {
					unsigned dig = 0;
					for( ; DAW_LIKELY( first < last ) and
					       ( dig = parse_digit( *first ) ) < 10U;
					     ++first ) {
						value *= 10U;
						value += dig;
					}
				}
				v = value;
				return first;
			}

			template<typename Result, bool KnownRange, typename ParseState,
			         std::enable_if_t<KnownRange, std::nullptr_t> = nullptr>
			[[nodiscard]] DAW_ATTRIB_FLATINLINE inline constexpr Result
			parse_real( ParseState &parse_state ) {
				using CharT = typename ParseState::CharT;
				// [-]WHOLE[.FRACTION][(e|E)[+|-]EXPONENT]
				daw_json_assert_weak(
				  parse_state.has_more( ) and
				    parse_policy_details::is_number_start( parse_state.front( ) ),
				  ErrorReason::InvalidNumberStart, parse_state );

				CharT *whole_first = parse_state.first;
				CharT *whole_last = parse_state.class_first ? parse_state.class_first
				                                            : parse_state.class_last;
				CharT *fract_first =
				  parse_state.class_first ? parse_state.class_first + 1 : nullptr;
				CharT *fract_last = parse_state.class_last;
				CharT *exp_first =
				  parse_state.class_last ? parse_state.class_last + 1 : nullptr;
				CharT *const exp_last = parse_state.last;

				if( parse_state.class_first == nullptr ) {
					if( parse_state.class_last == nullptr ) {
						whole_last = parse_state.last;
					} else {
						whole_last = parse_state.class_last;
					}
				} else if( parse_state.class_last == nullptr ) {
					fract_last = parse_state.last;
				}

				using max_storage_digits = daw::constant<static_cast<std::ptrdiff_t>(
				  daw::numeric_limits<std::uint64_t>::digits10 )>;
				bool use_strtod = [&] {
					if constexpr( std::is_floating_point_v<Result> and
					              ParseState::precise_ieee754 ) {
						return DAW_UNLIKELY(
						  ( ( whole_last - whole_first ) +
						    ( fract_first ? fract_last - fract_first : 0 ) ) >
						  max_storage_digits::value );
					} else {
						return false;
					}
				}( );

				Result const sign = [&] {
					if( *whole_first == '-' ) {
						++whole_first;
						return static_cast<Result>( -1.0 );
					}
					return static_cast<Result>( 1.0 );
				}( );
				using max_exponent = daw::constant<static_cast<std::ptrdiff_t>(
				  daw::numeric_limits<Result>::max_digits10 + 1 )>;
				using unsigned_t =
				  std::conditional_t<max_storage_digits::value >= max_exponent::value,
				                     std::uint64_t, Result>;

				using signed_t =
				  typename std::conditional_t<std::is_floating_point_v<unsigned_t>,
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
						use_strtod |= DAW_UNLIKELY( fract_exponent_available >
						                            whole_exponent_available );
					}
					fract_exponent_available =
					  ( std::min )( fract_exponent_available, whole_exponent_available );
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
						  // TODO use zstringopt
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
					use_strtod |= DAW_UNLIKELY( exponent > 22 );
					use_strtod |= DAW_UNLIKELY( exponent < -22 );
					use_strtod |=
					  DAW_UNLIKELY( significant_digits > 9007199254740992ULL );
					if( DAW_UNLIKELY( use_strtod ) ) {
						return json_details::parse_with_strtod<Result>( parse_state.first,
						                                                parse_state.last );
					}
				}
				return sign * power10<Result>(
				                ParseState::exec_tag,
				                static_cast<Result>( significant_digits ), exponent );
			}

			template<typename Result, bool KnownRange, typename ParseState,
			         std::enable_if_t<not KnownRange, std::nullptr_t> = nullptr>
			[[nodiscard]] DAW_ATTRIB_FLATINLINE inline constexpr Result
			parse_real( ParseState &parse_state ) {
				// [-]WHOLE[.FRACTION][(e|E)[+|-]EXPONENT]
				using CharT = typename ParseState::CharT;
				daw_json_assert_weak(
				  parse_state.has_more( ) and
				    parse_policy_details::is_number_start( parse_state.front( ) ),
				  ErrorReason::InvalidNumberStart, parse_state );

				CharT *const orig_first = parse_state.first;
				CharT *const orig_last = parse_state.last;

				// silencing gcc9 warning as these are only used when precise ieee is in
				// play.
				(void)orig_first;
				(void)orig_last;

				auto const sign = static_cast<Result>(
				  parse_policy_details::validate_signed_first( parse_state ) );

				using max_storage_digits = daw::constant<static_cast<std::int64_t>(
				  daw::numeric_limits<std::uint64_t>::digits10 )>;
				using max_exponent = daw::constant<static_cast<std::int64_t>(
				  daw::numeric_limits<Result>::max_digits10 + 1 )>;
				using unsigned_t =
				  std::conditional_t<max_storage_digits::value >= max_exponent::value,
				                     std::uint64_t, Result>;
				using signed_t =
				  std::conditional_t<max_storage_digits::value >= max_exponent::value,
				                     std::int64_t, Result>;

				CharT *first = parse_state.first;
				CharT *const whole_last =
				  parse_state.first +
				  ( std::min )( parse_state.last - parse_state.first,
				                static_cast<std::ptrdiff_t>( max_exponent::value ) );

				unsigned_t significant_digits = 0;
				CharT *last_char =
				  parse_digits_while_number<( ParseState::is_zero_terminated_string or
				                              ParseState::is_unchecked_input )>(
				    first, whole_last, significant_digits );
				std::ptrdiff_t sig_digit_count = last_char - parse_state.first;
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
						CharT *ptr = skip_digits<( ParseState::is_zero_terminated_string or
						                           ParseState::is_unchecked_input )>(
						  last_char, parse_state.last );
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
							first = skip_digits<( ParseState::is_zero_terminated_string or
							                      ParseState::is_unchecked_input )>(
							  first, parse_state.last );
						}
					} else {
						CharT *fract_last =
						  first + ( std::min )( parse_state.last - first,
						                        static_cast<std::ptrdiff_t>(
						                          max_exponent::value -
						                          ( first - parse_state.first ) ) );

						last_char = parse_digits_while_number<(
						  ParseState::is_zero_terminated_string or
						  ParseState::is_unchecked_input )>( first, fract_last,
						                                     significant_digits );
						sig_digit_count += last_char - first;
						exponent_p1 -= static_cast<signed_t>( last_char - first );
						first = last_char;
						if( ( first >= fract_last ) & ( first < parse_state.last ) ) {
							auto new_first =
							  skip_digits<( ParseState::is_zero_terminated_string or
							                ParseState::is_unchecked_input )>(
							    first, parse_state.last );
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
						last_char = parse_digits_while_number<(
						  ParseState::is_zero_terminated_string or
						  ParseState::is_unchecked_input )>( first, parse_state.last,
						                                     exp_tmp );
						first = last_char;
						return to_signed( exp_tmp, exp_sign );
					}
					return signed_t{ 0 };
				}( );
				signed_t const exponent = [&] {
					if constexpr( ParseState::is_unchecked_input or
					              not std::is_floating_point_v<Result> ) {
						return exponent_p1 + exponent_p2;
					} else {
						if( bool matching_signs =
						      ( exponent_p1 < 0 ) == ( exponent_p2 < 0 );
						    not matching_signs ) {

							return exponent_p1 + exponent_p2;
						}
						auto const s = exponent_p1 < 0 ? signed_t{ -1 } : signed_t{ 1 };
						if( s < 0 ) {
							if( DAW_UNLIKELY( ( daw::numeric_limits<signed_t>::min( ) -
							                    exponent_p1 ) > exponent_p2 ) ) {
								// We don't have inf, but we can just saturate it to min as it
								// will be 0 anyways for the other result
								return daw::numeric_limits<signed_t>::min( );
							}
							return exponent_p1 + exponent_p2;
						}
						auto r = static_cast<unsigned_t>( exponent_p1 ) +
						         static_cast<unsigned_t>( exponent_p2 );
						if( DAW_UNLIKELY( r >
						                  static_cast<unsigned_t>(
						                    daw::numeric_limits<signed_t>::max( ) ) ) ) {
							return daw::numeric_limits<signed_t>::max( );
						}
						return static_cast<signed_t>( r );
					}
				}( );
				parse_state.first = first;

				if constexpr( std::is_floating_point_v<Result> and
				              ParseState::precise_ieee754 ) {
					use_strtod |= DAW_UNLIKELY( exponent > 22 );
					use_strtod |= DAW_UNLIKELY( exponent < -22 );
					use_strtod |=
					  DAW_UNLIKELY( significant_digits > 9007199254740992ULL );
					if( DAW_UNLIKELY( use_strtod ) ) {
						return json_details::parse_with_strtod<Result>( orig_first,
						                                                orig_last );
					}
				}
				return sign * power10<Result>(
				                ParseState::exec_tag,
				                static_cast<Result>( significant_digits ), exponent );
			}
		} // namespace json_details
	}   // namespace DAW_JSON_VER
} // namespace daw::json
