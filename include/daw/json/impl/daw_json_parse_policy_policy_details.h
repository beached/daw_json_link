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

#include <daw/daw_attributes.h>

namespace daw::json {
	inline namespace DAW_JSON_VER {
		namespace parse_policy_details {
			template<char... keys>
			[[nodiscard]] DAW_ATTRIB_FLATINLINE inline constexpr bool in( char c ) {
				auto const eq = [c]( char k ) {
					return c == k;
				};
				return nsc_or( eq( keys )... );
			}

			[[nodiscard]] DAW_ATTRIB_FLATINLINE inline constexpr bool
			at_end_of_item( char c ) {
				return static_cast<bool>( static_cast<unsigned>( c == ',' ) |
				                          static_cast<unsigned>( c == '}' ) |
				                          static_cast<unsigned>( c == ']' ) |
				                          static_cast<unsigned>( c == ':' ) |
				                          static_cast<unsigned>( c <= 0x20 ) );
			}

			[[nodiscard]] DAW_ATTRIB_FLATINLINE inline constexpr bool
			is_number( char c ) {
				return static_cast<unsigned>( static_cast<unsigned char>( c ) -
				                              static_cast<unsigned char>( '0' ) ) < 10U;
			}

			template<typename ParseState>
			DAW_ATTRIB_FLATINLINE inline constexpr void
			validate_unsigned_first( ParseState const &parse_state ) {
				if constexpr( not ParseState::is_unchecked_input ) {
					switch( parse_state.front( ) ) {
					case '1':
					case '2':
					case '3':
					case '4':
					case '5':
					case '6':
					case '7':
					case '8':
					case '9':
						return;
					case '0':
						if( parse_state.size( ) > 1 ) {
							daw_json_ensure( is_number( *( parse_state.first + 1 ) ),
							                 ErrorReason::InvalidNumberStart, parse_state );
						}
						return;
					default:
						daw_json_error( ErrorReason::InvalidNumberStart, parse_state );
					}
				}
			}

			/***
			 * Validate that this is a valid number start(e.g. not leading zero when
			 * not zero)
			 * @return sign value
			 */
			template<typename ParseState>
			[[nodiscard]] DAW_ATTRIB_FLATINLINE inline constexpr int
			validate_signed_first( ParseState &parse_state ) {
				daw_json_assert_weak( parse_state.has_more( ),
				                 ErrorReason::UnexpectedEndOfData, parse_state );
				auto const c = parse_state.front( );
				if( c == '-' ) {
					parse_state.remove_prefix( );
					return -1;
				}
				if constexpr( ParseState::is_unchecked_input ) {
					if( DAW_LIKELY( c >= '0' ) and DAW_LIKELY( c <= '9' ) ) {
						return 1;
					}
				} else {
					if( c >= '1' and c <= '9' ) {
						return 1;
					}
					if( DAW_LIKELY( c == '0' ) ) {
						if( parse_state.size( ) > 1 ) {
							auto const next_dig = static_cast<unsigned>(
							  static_cast<unsigned char>( *( parse_state.first + 1 ) ) );
							auto const tst = next_dig - static_cast<unsigned char>( '0' );
							// Cannot be a digit
							daw_json_assert_weak( tst >= 10U, ErrorReason::InvalidNumberStart,
							                      parse_state );
						}
						return 1;
					}
				}
				daw_json_error( ErrorReason::InvalidNumberStart, parse_state );
			}

			[[nodiscard]] DAW_ATTRIB_FLATINLINE inline constexpr bool
			is_number_start( char c ) {
				switch( c ) {
				case '0': // TODO: CONFORMANCE We are accepting starting with zero for
				          // now
				case '1':
				case '2':
				case '3':
				case '4':
				case '5':
				case '6':
				case '7':
				case '8':
				case '9':
				case '-':
					return true;
				}
				return false;
			}
		} // namespace parse_policy_details
	}   // namespace DAW_JSON_VER
} // namespace daw::json
