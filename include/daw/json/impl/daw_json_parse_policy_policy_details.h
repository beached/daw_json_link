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

#include <cstddef>
#include <type_traits>

namespace daw::json {
	inline namespace DAW_JSON_VER {
		namespace json_details {
			// Translate a read-only scanner result back into the input range's
			// pointer type. Non-null positions must belong to the same buffer.
			template<typename Char>
			[[nodiscard]] constexpr Char *input_pointer( Char *origin, char const *position ) {
				if constexpr( std::is_const_v<Char> ) {
					return position;
				} else {
					return position ? origin + ( position - origin ) : nullptr;
				}
			}

			enum class SkipBracketedType { Array, Class };
		} // namespace json_details

		namespace parse_policy_details {
			template<char... keys>
			[[nodiscard]] DAW_ATTRIB_FLATINLINE static constexpr bool in( char c ) {
				auto const eq = [c]( char k ) {
					return c == k;
				};
				return nsc_or( eq( keys )... );
			}

			[[nodiscard]] DAW_ATTRIB_FLATINLINE static constexpr bool
			at_end_of_item( char c ) {
				return static_cast<bool>( static_cast<unsigned>( c == ',' ) |
				                          static_cast<unsigned>( c == '}' ) |
				                          static_cast<unsigned>( c == ']' ) |
				                          static_cast<unsigned>( c == ':' ) |
				                          static_cast<unsigned>( c <= 0x20 ) );
			}

			[[nodiscard]] DAW_ATTRIB_FLATINLINE static inline constexpr bool
			is_number( char c ) {
				return static_cast<unsigned>( static_cast<unsigned char>( c ) -
				                              static_cast<unsigned char>( '0' ) ) < 10U;
			}

			template<typename ParseState>
			DAW_ATTRIB_FLATINLINE static inline constexpr void
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
							daw_json_ensure( not is_number( *( parse_state.first + 1 ) ),
							                 ErrorReason::InvalidNumberStart,
							                 parse_state );
						}
						return;
					default:
						daw_json_error( true, ErrorReason::InvalidNumberStart, parse_state );
					}
				}
			}

			/***
			 * Validate that this is a valid number start, an optional '-' then
			 * 1-9 or a 0 that is not followed by a digit.  When checked, callers
			 * can rely on at least one whole digit and no leading zero
			 * @return sign value
			 */
			template<typename ParseState>
			[[nodiscard]] DAW_ATTRIB_FLATINLINE static constexpr int
			validate_signed_first( ParseState &parse_state ) {
				daw_json_assert_weak( parse_state.has_more( ),
				                      ErrorReason::UnexpectedEndOfData,
				                      parse_state );
				if constexpr( ParseState::is_unchecked_input ) {
					auto const c = parse_state.front( );
					if( c == '-' ) {
						parse_state.remove_prefix( );
						return -1;
					}
					if( DAW_LIKELY( c >= '0' ) and DAW_LIKELY( c <= '9' ) ) {
						return 1;
					}
				} else {
					int sign = 1;
					if( parse_state.front( ) == '-' ) {
						parse_state.remove_prefix( );
						sign = -1;
						// A digit must follow, e.g. "-" and "-.5" are invalid
						daw_json_assert_weak( ( ParseState::is_zero_terminated_string or
						                        parse_state.has_more( ) ) and
						                        is_number( parse_state.front( ) ),
						                      ErrorReason::InvalidNumber,
						                      parse_state );
					}
					auto const c = parse_state.front( );
					if( c >= '1' and c <= '9' ) {
						return sign;
					}
					if( DAW_LIKELY( c == '0' ) ) {
						if( parse_state.size( ) > 1 ) {
							auto const next_dig = static_cast<unsigned>(
							  static_cast<unsigned char>( *( parse_state.first + 1 ) ) );
							auto const tst = next_dig - static_cast<unsigned char>( '0' );
							// Cannot be a digit
							daw_json_assert_weak(
							  tst >= 10U, ErrorReason::InvalidNumberStart, parse_state );
						}
						return sign;
					}
				}
				daw_json_error( true, ErrorReason::InvalidNumberStart, parse_state );
			}

			[[nodiscard]] DAW_ATTRIB_FLATINLINE static constexpr bool
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

			/// Check the literal or number that starts at first and return its
			/// end.  The bracket skips use this for the scalars they pass over.
			/// Numbers follow the JSON grammar, except that a leading '+' and
			/// leading zeros are allowed.  Skipped values may never be parsed, the
			/// number parsers check the spelling of the numbers they parse.
			/// A token has to end at the end of the data, whitespace, a structural
			/// character, a quote or, when CommentStart is not '\0', the start of a
			/// comment
			template<char CommentStart = '\0', typename ParseState>
			[[nodiscard]] constexpr char const *
			skip_scalar_token_checked( char const *first, char const *const last,
			                           ParseState const &parse_state ) {
				auto const literal =
				  [&]( char const *word, std::ptrdiff_t size, ErrorReason reason ) {
					  bool matches = last - first >= size;
					  for( std::ptrdiff_t n = 1; matches and n < size; ++n ) {
						  matches = first[n] == word[n];
					  }
					  daw_json_ensure( matches, reason, parse_state );
					  first += size;
				  };
				auto const digits = [&] {
					auto const start = first;
					while( first < last and is_number( *first ) ) {
						++first;
					}
					return first != start;
				};

				switch( *first ) {
				case 't':
					literal( "true", 4, ErrorReason::InvalidTrue );
					break;
				case 'f':
					literal( "false", 5, ErrorReason::InvalidFalse );
					break;
				case 'n':
					literal( "null", 4, ErrorReason::InvalidNull );
					break;
				default:
					daw_json_ensure( *first == '-' or *first == '+' or
					                   is_number( *first ),
					                 ErrorReason::InvalidStartOfValue,
					                 parse_state );
					if( *first == '-' or *first == '+' ) {
						++first;
					}
					daw_json_ensure(
					  digits( ), ErrorReason::InvalidNumberStart, parse_state );
					if( first < last and *first == '.' ) {
						++first;
						daw_json_ensure(
						  digits( ), ErrorReason::InvalidNumber, parse_state );
					}
					if( first < last and ( *first == 'e' or *first == 'E' ) ) {
						++first;
						if( first < last and ( *first == '+' or *first == '-' ) ) {
							++first;
						}
						daw_json_ensure(
						  digits( ), ErrorReason::InvalidNumber, parse_state );
					}
					break;
				}
				if( first < last ) {
					switch( *first ) {
					case '{':
					case '}':
					case '[':
					case ']':
					case ',':
					case ':':
					case '"':
						break;
					default:
						if constexpr( CommentStart != '\0' ) {
							if( *first == CommentStart ) {
								break;
							}
						}
						daw_json_ensure(
						  ( static_cast<unsigned>( static_cast<unsigned char>( *first ) ) -
						    1U ) <= 0x1FU,
						  ErrorReason::InvalidEndOfValue,
						  parse_state );
					}
				}
				return first;
			}
		} // namespace parse_policy_details
	} // namespace DAW_JSON_VER
} // namespace daw::json
