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

#include <ciso646>
#include <cstddef>
#include <cstdint>
#include <limits>
#include <type_traits>

namespace daw::json {
	inline namespace DAW_JSON_VER {
		namespace json_details {
			template<bool IsZeroTerminated, typename Value>
			DAW_ATTRIBUTE_FLATTEN static inline constexpr void
			parse_digits( char const *first, char const *const last, Value &v ) {
				Value value = v;
				if constexpr( IsZeroTerminated ) {
					auto dig = parse_digit( *first );
					while( dig < 10U ) {
						value *= 10U;
						value += dig;
						++first;
						dig = parse_digit( *first );
					}
				} else {
					while( DAW_JSON_LIKELY( first < last ) ) {
						value *= 10U;
						value += parse_digit( *first );
						++first;
					}
				}
				v = value;
			}

			template<bool skip_end_check, typename Unsigned>
			DAW_ATTRIBUTE_FLATTEN static inline constexpr char const *
			parse_real_digits_while_number( char const *first, char const *const last,
			                                Unsigned &v ) {
				auto value = v;
				unsigned dig = 0;
				while( ( skip_end_check or DAW_JSON_LIKELY( first < last ) ) and
				       ( dig = parse_digit( *first ) ) < 10U ) {
					value *= 10U;
					value += dig;
					++first;
				}
				v = value;
				return first;
			}

			template<typename Result, bool KnownRange, typename ParseState,
			         std::enable_if_t<KnownRange, std::nullptr_t> = nullptr>
			[[nodiscard]] DAW_ATTRIBUTE_FLATTEN static inline constexpr Result
			parse_real( ParseState &parse_state ) {
				// [-]WHOLE[.FRACTION][(e|E)[+|-]EXPONENT]
				daw_json_assert_weak(
				  parse_state.has_more( ) and
				    parse_policy_details::is_number_start( parse_state.front( ) ),
				  ErrorReason::InvalidNumberStart, parse_state );

				char const *whole_first = parse_state.first;
				char const *whole_last = parse_state.class_first
				                           ? parse_state.class_first
				                           : parse_state.class_last;
				char const *fract_first =
				  parse_state.class_first ? parse_state.class_first + 1 : nullptr;
				char const *fract_last = parse_state.class_last;
				char const *exp_first =
				  parse_state.class_last ? parse_state.class_last + 1 : nullptr;
				char const *const exp_last = parse_state.last;

				if( parse_state.class_first == nullptr ) {
					if( parse_state.class_last == nullptr ) {
						whole_last = parse_state.last;
					} else {
						whole_last = parse_state.class_last;
					}
				} else if( parse_state.class_last == nullptr ) {
					fract_last = parse_state.last;
				}

				constexpr auto max_storage_digits = static_cast<std::ptrdiff_t>(
				  daw::numeric_limits<std::uint64_t>::digits10 );
				bool use_strtod = [&] {
					if constexpr( std::is_floating_point_v<Result> and
					              ParseState::precise_ieee754 ) {
						return DAW_UNLIKELY( ( ( whole_last - whole_first ) +
						                       ( fract_first ? fract_last - fract_first
						                                     : 0 ) ) > max_storage_digits );
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
				constexpr auto max_exponent = static_cast<std::ptrdiff_t>(
				  daw::numeric_limits<Result>::max_digits10 + 1 );
				using unsigned_t =
				  std::conditional_t<max_storage_digits >= max_exponent, std::uint64_t,
				                     Result>;

				std::ptrdiff_t whole_exponent_available = whole_last - whole_first;
				std::ptrdiff_t fract_exponent_available =
				  fract_first ? fract_last - fract_first : 0;
				std::ptrdiff_t exponent = 0;

				if( whole_exponent_available > max_exponent ) {
					whole_last = whole_first + max_exponent;
					whole_exponent_available -= max_exponent;
					fract_exponent_available = 0;
					fract_first = nullptr;
					exponent = whole_exponent_available;
				} else {
					whole_exponent_available = max_exponent - whole_exponent_available;
					if constexpr( ParseState::precise_ieee754 ) {
						use_strtod =
						  DAW_UNLIKELY( use_strtod | ( fract_exponent_available >
						                               whole_exponent_available ) );
					}
					fract_exponent_available =
					  std::min( fract_exponent_available, whole_exponent_available );
					exponent = -fract_exponent_available;
					fract_last = fract_first + fract_exponent_available;
				}

				unsigned_t significant_digits = 0;
				parse_digits<ParseState::is_zero_terminated_string>(
				  whole_first, whole_last, significant_digits );
				if( fract_first ) {
					parse_digits<ParseState::is_zero_terminated_string>(
					  fract_first, fract_last, significant_digits );
				}

				if( exp_first and ( exp_last - exp_first ) > 0 ) {
					int const exp_sign = [&] {
						switch( *exp_first ) {
						case '-':
							++exp_first;
							return -1;
						case '+':
							++exp_first;
							return 1;
						default:
							return 1;
						}
					}( );
					exponent += exp_sign * [&] {
						std::ptrdiff_t r = 0;
						while( exp_first < exp_last ) {
							r *= static_cast<std::ptrdiff_t>( 10 );
							r += static_cast<std::ptrdiff_t>( parse_digit( *exp_first ) );
							++exp_first;
						}
						return r;
					}( );
				}
				if constexpr( std::is_floating_point_v<Result> and
				              ParseState::precise_ieee754 and
				              ( std::is_same_v<Result, double> or
				                std::is_same_v<Result, float> ) ) {
					if( DAW_UNLIKELY( use_strtod or exponent > 22 or exponent < -22 ) ) {
						return json_details::parse_with_strtod<Result>( parse_state.first );
					}
					return sign * power10<Result>(
					                ParseState::exec_tag,
					                static_cast<Result>( significant_digits ), exponent );
				} else {
					return sign * power10<Result>(
					                ParseState::exec_tag,
					                static_cast<Result>( significant_digits ), exponent );
				}
			}

			template<typename Result, bool KnownRange, typename ParseState,
			         std::enable_if_t<not KnownRange, std::nullptr_t> = nullptr>
			[[nodiscard]] DAW_ATTRIBUTE_FLATTEN inline constexpr Result
			parse_real( ParseState &parse_state ) {
				// [-]WHOLE[.FRACTION][(e|E)[+|-]EXPONENT]
				daw_json_assert_weak(
				  ( ParseState::is_zero_terminated_string or
				    parse_state.has_more( ) ) and
				    parse_policy_details::is_number_start( parse_state.front( ) ),
				  ErrorReason::InvalidNumberStart, parse_state );

				char const *const orig_first = parse_state.first;
				Result const sign = [&] {
					if( parse_state.front( ) == '-' ) {
						parse_state.remove_prefix( );
						return static_cast<Result>( -1 );
					}
					return static_cast<Result>( 1 );
				}( );
				constexpr auto max_storage_digits = static_cast<std::ptrdiff_t>(
				  daw::numeric_limits<std::uint64_t>::digits10 );
				constexpr auto max_exponent = static_cast<std::ptrdiff_t>(
				  daw::numeric_limits<Result>::max_digits10 + 1 );
				using unsigned_t =
				  std::conditional_t<max_storage_digits >= max_exponent, std::uint64_t,
				                     Result>;
				using signed_t =
				  std::conditional_t<max_storage_digits >= max_exponent, int, Result>;

				char const *first = parse_state.first;
				char const *const whole_last =
				  parse_state.first +
				  std::min( parse_state.last - parse_state.first, max_exponent );

				unsigned_t significant_digits = 0;
				char const *last_char = parse_real_digits_while_number<(
				  ParseState::is_zero_terminated_string or
				  ParseState::is_unchecked_input )>( first, whole_last,
				                                     significant_digits );
				std::ptrdiff_t sig_digit_count = last_char - parse_state.first;
				bool use_strtod = DAW_UNLIKELY( sig_digit_count < max_storage_digits );
				signed_t exponent = [&] {
					if( DAW_UNLIKELY( last_char >= whole_last ) ) {
						use_strtod = true;
						// We have sig digits we cannot parse because there isn't enough
						// room in a std::uint64_t
						char const *ptr =
						  skip_digits<( ParseState::is_zero_terminated_string or
						                ParseState::is_unchecked_input or
						                ParseState::is_zero_terminated_string )>(
						    last_char, parse_state.last );
						auto const diff = ptr - last_char;
						last_char = ptr;
						return static_cast<signed_t>( diff );
					}
					return static_cast<signed_t>( 0 );
				}( );
				if( significant_digits == 0 ) {
					exponent = 0;
				}
				first = last_char;
				if( ( ( ParseState::is_unchecked_input or
				        DAW_JSON_LIKELY( first < parse_state.last ) ) and
				      *first == '.' ) ) {
					++first;
					if( exponent != 0 ) {
						if( first < parse_state.last ) {
							first = skip_digits<( ParseState::is_zero_terminated_string or
							                      ParseState::is_unchecked_input or
							                      ParseState::is_zero_terminated_string )>(
							  first, parse_state.last );
						}
					} else {
						char const *fract_last =
						  first +
						  std::min( parse_state.last - first,
						            static_cast<std::ptrdiff_t>(
						              max_exponent - ( first - parse_state.first ) ) );

						last_char =
						  parse_real_digits_while_number<ParseState::is_unchecked_input>(
						    first, fract_last, significant_digits );
						sig_digit_count += last_char - first;
						exponent -= static_cast<signed_t>( last_char - first );
						first = last_char;
						if( ( first >= fract_last ) & ( first < parse_state.last ) ) {
							first = skip_digits<( ParseState::is_zero_terminated_string or
							                      ParseState::is_unchecked_input or
							                      ParseState::is_zero_terminated_string )>(
							  first, parse_state.last );
						}
					}
				}

				exponent += [&] {
					if( ( ParseState::is_unchecked_input or first < parse_state.last ) and
					    ( ( *first | 0x20 ) == 'e' ) ) {
						++first;
						bool const exp_sign = [&] {
							daw_json_assert_weak( first < parse_state.last,
							                      ErrorReason::UnexpectedEndOfData,
							                      parse_state.copy( first ) );
							switch( *first ) {
							case '+':
								++first;
								return false;
							case '-':
								++first;
								return true;
							default:
								return false;
							}
						}( );
						daw_json_assert_weak( parse_state.has_more( ),
						                      ErrorReason::UnexpectedEndOfData,
						                      parse_state );
						unsigned_t exp_tmp = 0;
						last_char = parse_real_digits_while_number<(
						  ParseState::is_zero_terminated_string or
						  ParseState::is_unchecked_input or
						  ParseState::is_zero_terminated_string )>( first, parse_state.last,
						                                            exp_tmp );
						first = last_char;
						if( exp_sign ) {
							return -static_cast<signed_t>( exp_tmp );
						}
						return static_cast<signed_t>( exp_tmp );
					}
					return static_cast<signed_t>( 0 );
				}( );
				parse_state.first = first;

				if constexpr( std::is_floating_point_v<Result> and
				              ParseState::precise_ieee754 ) {
					if( DAW_UNLIKELY( use_strtod or exponent > 22 or exponent < -22 ) ) {
						return json_details::parse_with_strtod<Result>( orig_first );
					}
					return sign * power10<Result>(
					                ParseState::exec_tag,
					                static_cast<Result>( significant_digits ), exponent );
				} else {
					return sign * power10<Result>(
					                ParseState::exec_tag,
					                static_cast<Result>( significant_digits ), exponent );
				}
			}
		} // namespace json_details
	}   // namespace DAW_JSON_VER
} // namespace daw::json
