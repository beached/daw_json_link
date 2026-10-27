// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/daw_json_link
//

#pragma once

#include "daw/json/impl/version.h"

#include "daw/json/impl/daw_json_assert.h"
#include "daw/json/impl/daw_json_exec_modes.h"
#include "daw/json/impl/daw_json_parse_digit.h"
#include "daw/json/impl/daw_json_parse_unsigned_int.h"
#include "daw/json/impl/daw_json_simd_digits.h"
#include "daw/json/impl/daw_json_type_options.h"

#include <daw/daw_arith_traits.h>
#include <daw/daw_construct_a.h>
#include <daw/daw_cxmath.h>
#include <daw/daw_not_null.h>
#include <daw/daw_remove_cvref.h>
#include <daw/daw_restrict.h>
#include <daw/daw_uint_buffer.h>

#include <cstddef>
#include <daw/stdinc/data_access.h>
#include <limits>

namespace daw::json {
	inline namespace DAW_JSON_VER {
		namespace json_details {
			template<typename Signed, typename Unsigned>
			DAW_ATTRIB_INLINE constexpr Signed to_signed( Unsigned &&u,
			                                              Signed sign ) {
				if( sign <= Signed{ 0 } ) {
					return static_cast<Signed>( -u );
				}
				return static_cast<Signed>( u );
			}

			[[nodiscard]]
			constexpr bool
			is_made_of_eight_digits_cx( daw::not_null<char const *> const ptr ) {
				// The copy to local buffer is to get the compiler to treat it like a
				// reinterpret_cast

				auto const val = daw::to_uint64_buffer( ptr.get( ) );
				return ( ( ( val & daw::UInt64{ 0xF0F0'F0F0'F0F0'F0F0ULL } ) |
				           ( ( ( val + daw::UInt64{ 0x0606'0606'0606'0606ULL } ) &
				               daw::UInt64{ 0xF0F0'F0F0'F0F0'F0F0ULL } ) >>
				             4U ) ) == daw::UInt64{ 0x3333'3333'3333'3333ULL } );
			}

			template<options::JsonRangeCheck RangeCheck, typename Unsigned,
			         typename MaxArithUnsigned>
			using max_unsigned_t = daw::conditional_t<
			  daw::is_integral_v<Unsigned> or std::is_enum_v<Unsigned>,
			  daw::conditional_t<( sizeof( Unsigned ) > sizeof( MaxArithUnsigned ) ),
			                     Unsigned, MaxArithUnsigned>,
			  Unsigned>;

			// Constexpr'ified version from
			// https://kholdstare.github.io/technical/2020/05/26/faster-integer-parsing.html
			constexpr UInt64 parse_8_digits( daw::not_null<char const *> const str ) {
#if defined( DAW_HAS_SIMD )
				if( not DAW_IS_CONSTANT_EVALUATED_COMPAT( ) ) {
					return UInt64( simd_parse_8_digits( str.get( ) ) );
				}
#endif
				auto const chunk = daw::to_uint64_buffer( str.get( ) );
				// 1-byte mask trick (works on 4 pairs of single digits)
				auto const lower_digits =
				  ( chunk & daw::UInt64{ 0x0F'00'0F'00'0F'00'0F'00ULL } ) >> 8U;
				auto const upper_digits =
				  ( chunk & daw::UInt64{ 0x00'0F'00'0F'00'0F'00'0FULL } ) * 10U;
				auto const chunk2 = lower_digits + upper_digits;

				// 2-byte mask trick (works on 2 pairs of two digits)
				auto const lower_digits2 =
				  ( chunk2 & daw::UInt64{ 0x00'FF'00'00'00'FF'00'00ULL } ) >> 16U;
				auto const upper_digits2 =
				  ( chunk2 & daw::UInt64{ 0x00'00'00'FF'00'00'00'FFULL } ) * 100U;
				auto const chunk3 = lower_digits2 + upper_digits2;

				// 4-byte mask trick (works on pair of four digits)
				auto const lower_digits3 =
				  ( chunk3 & daw::UInt64{ 0x00'00'FF'FF'00'00'00'00ULL } ) >> 32U;
				auto const upper_digits3 =
				  ( chunk3 & daw::UInt64{ 0x00'00'00'00'00'00'FF'FFULL } ) * 10000U;
				auto const chunk4 = lower_digits3 + upper_digits3;

				return chunk4 & daw::UInt64{ 0xFFFF'FFFFULL };
			}

			static_assert( parse_8_digits( "12345678" ) == daw::UInt64{ 1234'5678 },
			               "8 digit parser does not work on this platform" );

			constexpr UInt64
			parse_16_digits( daw::not_null<char const *> const str ) {
				auto const upper = parse_8_digits( str );
				auto const lower = parse_8_digits( str + 8 );
				return upper * daw::UInt64{ 100'000'000ULL } + lower;
			}

			static_assert( parse_16_digits( "1234567890123456" ) ==
			                 daw::UInt64{ 1234567890123456ULL },
			               "16 digit parser does not work on this platform" );

			template<typename T>
			struct make_unsigned_with_bool : daw::make_unsigned<T> {};

			template<>
			struct make_unsigned_with_bool<bool> {
				using type = bool;
			};

			template<typename Integer, typename T, typename ParseState>
			[[nodiscard]] constexpr Integer
			narrow_cast( T value, ParseState const &parse_state ) {
				if constexpr( std::is_signed_v<T> ) {
					if constexpr( std::is_signed_v<Integer> ) {
						if constexpr( sizeof( T ) <= sizeof( Integer ) ) {
							return value;
						} else if( value <= static_cast<T>( daw::max_value<Integer> ) ) {
							return static_cast<Integer>( value );
						} else {
							daw_json_error(
							  true, ErrorReason::NumberOutOfRange, parse_state );
						}
					} else if constexpr( sizeof( T ) <= sizeof( Integer ) ) {
						if( value >= 0 ) {
							return value;
						}
						daw_json_error( true, ErrorReason::NumberOutOfRange, parse_state );
					} else {
						if( value >= 0 and
						    value <= static_cast<T>( daw::max_value<Integer> ) ) {
							return value;
						}
						daw_json_error( true, ErrorReason::NumberOutOfRange, parse_state );
					}
				} else if constexpr( std::is_signed_v<Integer> ) {
					if constexpr( sizeof( T ) < sizeof( Integer ) ) {
						return static_cast<Integer>( value );
					} else {
						if( value > static_cast<T>( daw::max_value<Integer> ) ) {
							daw_json_error(
							  true, ErrorReason::NumberOutOfRange, parse_state );
						}
						return static_cast<Integer>( value );
					}
				} else if constexpr( sizeof( T ) <= sizeof( Integer ) ) {
					return static_cast<Integer>( value );
				} else {
					if( value <= static_cast<T>( daw::max_value<Integer> ) ) {
						return static_cast<Integer>( value );
					}
					daw_json_error( true, ErrorReason::NumberOutOfRange, parse_state );
				}
			}

			template<typename T>
			using make_unsigned_with_bool_t =
			  typename make_unsigned_with_bool<T>::type;

			template<typename Unsigned, options::JsonRangeCheck RangeChecked,
			         typename ParseState>
			[[nodiscard]] static constexpr Unsigned
			unsigned_parser_known( ParseState &parse_state ) {
				// We know how many digits are in the number
				using result_t = max_unsigned_t<RangeChecked, Unsigned, UInt64>;
				using uresult_t = max_unsigned_t<RangeChecked,
				                                 make_unsigned_with_bool_t<Unsigned>,
				                                 UInt64>;
				static_assert(
				  not static_cast<bool>( RangeChecked ) or
				    std::is_same_v<uresult_t, UInt64>,
				  "Range checking is only supported for std integral types" );

				daw::not_null<char const *> first = parse_state.first;
				daw::not_null<char const *> const last = parse_state.last;
				uresult_t result = uresult_t( );
				// The bounds may come from a skip, which takes any number, so a
				// fraction or exponent is caught here
				constexpr bool check_digits = not ParseState::is_unchecked_input;
				daw_json_assert_weak( last - first == 1 or *first != '0',
				                      ErrorReason::InvalidNumberStart,
				                      parse_state );
				while( last - first >= 16 ) {
					if constexpr( check_digits ) {
						daw_json_ensure( is_made_of_eight_digits_cx( first ) and
						                   is_made_of_eight_digits_cx( first + 8 ),
						                 ErrorReason::InvalidNumber,
						                 parse_state );
					}
					result *= static_cast<uresult_t>( 10'000'000'000'000'000ULL );
					result += static_cast<uresult_t>( parse_16_digits( first ) );
					first += 16;
				}
				if( last - first >= 8 ) {
					if constexpr( check_digits ) {
						daw_json_ensure( is_made_of_eight_digits_cx( first ),
						                 ErrorReason::InvalidNumber,
						                 parse_state );
					}
					result *= static_cast<uresult_t>( 100'000'000ULL );
					result += static_cast<uresult_t>( parse_8_digits( first ) );
					first += 8;
				}
				if constexpr( ParseState::is_zero_terminated_string ) {
					auto dig = parse_digit( *first );
					while( dig < 10U ) {
						result *= 10U;
						result += dig;
						++first;
						dig = parse_digit( *first );
					}
					if constexpr( check_digits ) {
						daw_json_ensure(
						  first >= last, ErrorReason::InvalidNumber, parse_state );
					}
				} else {
					while( first < last ) {
						auto const dig = parse_digit( *first );
						if constexpr( check_digits ) {
							daw_json_ensure(
							  dig < 10U, ErrorReason::InvalidNumber, parse_state );
						}
						result *= 10U;
						result += dig;
						++first;
					}
				}
				if constexpr( RangeChecked != options::JsonRangeCheck::Never ) {
					auto const count =
					  ( daw::digits10<result_t> + 1U ) - std::size( parse_state );
					daw_json_ensure(
					  ( ( result <= static_cast<uresult_t>( daw::max_value<result_t> ) ) &
					    ( count >= 0 ) ),
					  ErrorReason::NumberOutOfRange,
					  parse_state );
				}
				if constexpr( std::is_same_v<char const *,
				                             typename ParseState::iterator> ) {
					parse_state.first = first;
				} else {
					parse_state.first += first.get( ) - parse_state.first;
				}
				if constexpr( RangeChecked == options::JsonRangeCheck::Never ) {
					return daw::construct_a<Unsigned>( static_cast<Unsigned>( result ) );
				} else {
					return daw::construct_a<Unsigned>(
					  narrow_cast<Unsigned>( result, parse_state ) );
				}
			}

			//**************************
			template<typename Unsigned, options::JsonRangeCheck RangeChecked,
			         typename ParseState>
			[[nodiscard]] static constexpr Unsigned
			unsigned_parser_not_known( ParseState &parse_state ) {
				// We do not know how long the string is
				using result_t = max_unsigned_t<RangeChecked, Unsigned, UInt64>;
				using uresult_t = max_unsigned_t<RangeChecked,
				                                 make_unsigned_with_bool_t<Unsigned>,
				                                 UInt64>;
				static_assert(
				  not static_cast<bool>( RangeChecked ) or
				    std::is_same_v<uresult_t, UInt64>,
				  "Range checking is only supported for std integral types" );
				daw_json_assert_weak( parse_state.has_more( ),
				                      ErrorReason::UnexpectedEndOfData,
				                      parse_state );
				daw::not_null<char const *> first = parse_state.first;
				auto const orig_first = first;
				(void)orig_first; // only used inside if constexpr and gcc9 warns
				daw::not_null<char const *> const last = parse_state.last;
				uresult_t result = uresult_t( );
				bool has_eight =
				  last - first >= 8 ? is_made_of_eight_digits_cx( first ) : false;

				daw_json_assert_weak(
				  last - first == 1 or
				    not(
				      *first == '0' and
				      ( static_cast<unsigned>(
				          static_cast<unsigned char>( *std::next( first ) ) ) -
				          static_cast<unsigned>( static_cast<unsigned char>( '0' ) ) <
				        10U ) ),
				  ErrorReason::InvalidNumberStart,
				  parse_state );
				if( has_eight & ( last - first >= 16 ) ) {
					bool has_sixteen = is_made_of_eight_digits_cx( first + 8 );
					while( has_sixteen ) {
						result *= static_cast<uresult_t>( 10'000'000'000'000'000ULL );
						result += static_cast<uresult_t>( parse_16_digits( first ) );
						first += 16;
						has_eight =
						  last - first >= 8 ? is_made_of_eight_digits_cx( first ) : false;
						has_sixteen =
						  has_eight and
						  ( last - first >= 16 ? is_made_of_eight_digits_cx( first + 8 )
						                       : false );
					}
				}
				if( has_eight ) {
					result *= static_cast<uresult_t>( 100'000'000ULL );
					result += static_cast<uresult_t>( parse_8_digits( first ) );
					first += 8;
				}
				if constexpr( ParseState::is_zero_terminated_string ) {
					auto dig = parse_digit( *first );
					while( dig < 10U ) {
						result *= 10U;
						result += dig;
						++first;
						dig = parse_digit( *first );
					}
				} else {
					while( first < last ) {
						auto const dig = parse_digit( *first );
						if( dig >= 10U ) {
							break;
						}
						result *= 10U;
						result += dig;
						++first;
					}
				}

				if constexpr( RangeChecked != options::JsonRangeCheck::Never ) {
					auto const count =
					  static_cast<std::ptrdiff_t>( daw::digits10<result_t> + 1 ) -
					  ( first - orig_first );
					daw_json_ensure(
					  count >= 0, ErrorReason::NumberOutOfRange, parse_state );
				}

				if constexpr( std::is_same_v<char const *,
				                             typename ParseState::iterator> ) {
					parse_state.first = first;
				} else {
					parse_state.first += first.get( ) - parse_state.first;
				}
				if constexpr( RangeChecked == options::JsonRangeCheck::Never ) {
					return daw::construct_a<Unsigned>(
					  static_cast<Unsigned>( static_cast<result_t>( result ) ) );
				} else {
					return daw::construct_a<Unsigned>(
					  narrow_cast<Unsigned>( result, parse_state ) );
				}
			}

			template<typename Unsigned, options::JsonRangeCheck RangeChecked,
			         bool KnownBounds, typename ParseState>
			[[nodiscard]] DAW_ATTRIB_INLINE static constexpr Unsigned
			unsigned_parser( ParseState &parse_state ) {
				if constexpr( KnownBounds ) {
					return unsigned_parser_known<Unsigned, RangeChecked>( parse_state );
				} else {
					return unsigned_parser_not_known<Unsigned, RangeChecked>(
					  parse_state );
				}
			}
		} // namespace json_details
	} // namespace DAW_JSON_VER
} // namespace daw::json
