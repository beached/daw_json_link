// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/daw_json_link
//

#pragma once

#include "version.h"

#include <daw/daw_attributes.h>

namespace daw::json DAW_ATTRIB_PUBLIC {
	inline namespace DAW_JSON_VER {
		namespace parse_policy_details {
			template<char... keys>
			[[nodiscard]] DAW_ATTRIB_FLATINLINE inline constexpr bool in( char c ) {
				auto const eq = [c]( char k ) { return c == k; };
				return ( eq( keys ) | ... );
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
							daw_json_assert( is_number( *( parse_state.first + 1 ) ),
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
				daw_json_assert( parse_state.has_more( ),
				                 ErrorReason::UnexpectedEndOfData, parse_state );
				switch( parse_state.front( ) ) {
				case '-':
					parse_state.remove_prefix( );
					return -1;
				case '0':
					if( parse_state.size( ) > 1 ) {
						auto const dig = static_cast<unsigned>(
						  static_cast<unsigned char>( *( parse_state.first + 1 ) ) );
						// Cannot be a digit
						daw_json_assert( dig - static_cast<unsigned char>( '0' ) >= 10U,
						                 ErrorReason::InvalidNumberStart, parse_state );
					}
					break;
				case '1':
				case '2':
				case '3':
				case '4':
				case '5':
				case '6':
				case '7':
				case '8':
				case '9':
					return 1;
				default:
					daw_json_error( ErrorReason::InvalidNumberStart, parse_state );
				}
				return 1;
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
