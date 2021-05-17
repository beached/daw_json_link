// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/daw_json_link
//

#pragma once

#include "../daw_json_exception.h"
#include "daw_json_assert.h"
#include "daw_json_parse_digit.h"
#include "daw_json_parse_string_quote.h"
#include "version.h"

#include <daw/daw_hide.h>
#include <daw/daw_unreachable.h>

#include <ciso646>
#include <iterator>

namespace daw::json {
	inline namespace DAW_JSON_VER {
		namespace json_details {
			/***
			 * Skip a string, after the initial quote has been skipped already
			 */
			template<typename ParseState>
			[[nodiscard]] DAW_ATTRIBUTE_FLATTEN static inline constexpr ParseState
			skip_string_nq( ParseState &parse_state ) {
				auto result = parse_state;
				result.counter =
				  string_quote::string_quote_parser::parse_nq( parse_state );

				daw_json_assert_weak( parse_state.front( ) == '"',
				                      ErrorReason::InvalidString, parse_state );
				result.last = parse_state.first;
				parse_state.remove_prefix( );
				return result;
			}

			/***
			 * Skip a string and store the first escaped element's position, if any
			 */
			template<typename ParseState>
			[[nodiscard]] DAW_ATTRIBUTE_FLATTEN static inline constexpr ParseState
			skip_string( ParseState &parse_state ) {
				if( parse_state.empty( ) ) {
					return parse_state;
				}
				if( *std::prev( parse_state.first ) != '"' ) {
					daw_json_assert( parse_state.front( ) == '"',
					                 ErrorReason::InvalidString, parse_state );
					parse_state.remove_prefix( );
				}
				daw_json_assert_weak( parse_state.has_more( ),
				                      ErrorReason::InvalidString, parse_state );
				return skip_string_nq( parse_state );
			}

			template<typename ParseState>
			[[nodiscard]] static constexpr ParseState
			skip_true( ParseState &parse_state ) {
				auto result = parse_state;
				if constexpr( ( ParseState::is_zero_terminated_string or
				                ParseState::is_unchecked_input or
				                ParseState::is_zero_terminated_string ) ) {
					parse_state.remove_prefix( 4 );
				} else {
					parse_state.remove_prefix( );
					daw_json_assert( parse_state.starts_with( "rue" ),
					                 ErrorReason::InvalidTrue, parse_state );
					parse_state.remove_prefix( 3 );
				}
				result.last = parse_state.first;
				parse_state.trim_left( );
				daw_json_assert_weak( parse_state.is_at_token_after_value( ),
				                      ErrorReason::InvalidEndOfValue, parse_state );
				return result;
			}

			template<typename ParseState>
			[[nodiscard]] static constexpr ParseState
			skip_false( ParseState &parse_state ) {
				auto result = parse_state;
				if constexpr( ( ParseState::is_zero_terminated_string or
				                ParseState::is_unchecked_input or
				                ParseState::is_zero_terminated_string ) ) {
					parse_state.remove_prefix( 5 );
				} else {
					parse_state.remove_prefix( );
					daw_json_assert( parse_state.starts_with( "alse" ),
					                 ErrorReason::InvalidFalse, parse_state );
					parse_state.remove_prefix( 4 );
				}
				result.last = parse_state.first;
				parse_state.trim_left( );
				daw_json_assert_weak( parse_state.is_at_token_after_value( ),
				                      ErrorReason::InvalidEndOfValue, parse_state );
				return result;
			}

			template<typename ParseState>
			[[nodiscard]] static constexpr ParseState
			skip_null( ParseState &parse_state ) {
				if constexpr( ( ParseState::is_zero_terminated_string or
				                ParseState::is_unchecked_input or
				                ParseState::is_zero_terminated_string ) ) {
					parse_state.remove_prefix( 4 );
				} else {
					parse_state.remove_prefix( );
					daw_json_assert( parse_state.starts_with( "ull" ),
					                 ErrorReason::InvalidNull, parse_state );
					parse_state.remove_prefix( 3 );
				}
				daw_json_assert_weak( parse_state.has_more( ),
				                      ErrorReason::UnexpectedEndOfData, parse_state );
				parse_state.trim_left( );
				daw_json_assert_weak( parse_state.is_at_token_after_value( ),
				                      ErrorReason::UnexpectedEndOfData, parse_state );
				auto result = parse_state;
				result.first = nullptr;
				result.last = nullptr;
				return result;
			}

			template<bool skip_end_check>
			DAW_ATTRIBUTE_FLATTEN [[nodiscard]] static inline constexpr char const *
			skip_digits( char const *first, char const *const last ) {
				unsigned dig = parse_digit( *first );
				while( dig < 10 ) {
					++first;
					if constexpr( not skip_end_check ) {
						if( DAW_JSON_UNLIKELY( first >= last ) ) {
							break;
						}
					}
					dig = parse_digit( *first );
				}
				return first;
			}
			/***
			 * Skip a number and store the position of it's components in the returned
			 * ParseState
			 */
			template<typename ParseState>
			[[nodiscard]] static constexpr ParseState
			skip_number( ParseState &parse_state ) {
				daw_json_assert_weak( parse_state.has_more( ),
				                      ErrorReason::UnexpectedEndOfData, parse_state );

				auto result = parse_state;
				char const *first = parse_state.first;
				char const *const last = parse_state.last;
				if( *first == '-' ) {
					++first;
				}
				if( DAW_JSON_LIKELY( first < last ) ) {
					first = skip_digits<( ParseState::is_zero_terminated_string or
					                      ParseState::is_unchecked_input or
					                      ParseState::is_zero_terminated_string )>(
					  first, last );
				}
				char const *decimal = nullptr;
				if( ( ( ParseState::is_zero_terminated_string or
				        ParseState::is_unchecked_input or
				        ParseState::is_zero_terminated_string ) or
				      first < last ) and
				    ( *first == '.' ) ) {
					decimal = first;
					++first;
					if( DAW_JSON_LIKELY( first < last ) ) {
						first = skip_digits<( ParseState::is_zero_terminated_string or
						                      ParseState::is_unchecked_input or
						                      ParseState::is_zero_terminated_string )>(
						  first, last );
					}
				}
				char const *exp = nullptr;
				unsigned dig = parse_digit( *first );
				if( ( ( ParseState::is_zero_terminated_string or
				        ParseState::is_unchecked_input or
				        ParseState::is_zero_terminated_string ) or
				      ( first < last ) ) &
				    ( ( dig == parsed_constants::e_char ) |
				      ( dig == parsed_constants::E_char ) ) ) {
					exp = first;
					++first;
					daw_json_assert_weak( first < last, ErrorReason::UnexpectedEndOfData,
					                      [&] {
						                      auto r = parse_state;
						                      r.first = first;
						                      return r;
					                      }( ) );
					dig = parse_digit( *first );
					if( ( dig == parsed_constants::plus_char ) |
					    ( dig == parsed_constants::minus_char ) ) {
						++first;
					}
					if( DAW_JSON_LIKELY( first < last ) ) {
						first = skip_digits<( ParseState::is_zero_terminated_string or
						                      ParseState::is_unchecked_input or
						                      ParseState::is_zero_terminated_string )>(
						  first, last );
					}
				}

				parse_state.first = first;
				result.last = first;
				result.class_first = decimal;
				result.class_last = exp;
				return result;
			}

			/***
			 * When we don't know ahead of time what we are skipping switch on the
			 * first value and call that types specific skipper
			 * TODO: Investigate if there is a difference for the times we know what
			 * the member should be if that can increase performance
			 */
			template<typename ParseState>
			[[nodiscard]] static inline constexpr ParseState
			skip_value( ParseState &parse_state ) {
				daw_json_assert_weak( parse_state.has_more( ),
				                      ErrorReason::UnexpectedEndOfData, parse_state );

				// reset counter
				parse_state.counter = 0;
				switch( parse_state.front( ) ) {
				case '"':
					return skip_string( parse_state );
				case '[':
					return parse_state.skip_array( );
				case '{':
					return parse_state.skip_class( );
				case 't':
					return skip_true( parse_state );
				case 'f':
					return skip_false( parse_state );
				case 'n':
					return skip_null( parse_state );
				case '-':
				case '0':
				case '1':
				case '2':
				case '3':
				case '4':
				case '5':
				case '6':
				case '7':
				case '8':
				case '9':
					return skip_number( parse_state );
				}
				if constexpr( ( ParseState::is_zero_terminated_string or
				                ParseState::is_unchecked_input or
				                ParseState::is_zero_terminated_string ) ) {
					DAW_UNREACHABLE( );
				} else {
					daw_json_error( ErrorReason::InvalidStartOfValue, parse_state );
				}
			}

			/***
			 * Used in json_array_iterator::operator++( ) as we know the type we are
			 * skipping
			 */
			template<typename JsonMember, typename ParseState>
			[[nodiscard]] DAW_ATTRIBUTE_FLATTEN inline constexpr ParseState
			skip_known_value( ParseState &parse_state ) {
				daw_json_assert_weak( parse_state.has_more( ),
				                      ErrorReason::UnexpectedEndOfData, parse_state );
				if constexpr( JsonMember::expected_type == JsonParseTypes::Date or
				              JsonMember::expected_type == JsonParseTypes::StringRaw or
				              JsonMember::expected_type ==
				                JsonParseTypes::StringEscaped or
				              JsonMember::expected_type == JsonParseTypes::Custom ) {
					// json string encodings
					daw_json_assert_weak( parse_state.front( ) == '"',
					                      ErrorReason::InvalidString, parse_state );
					parse_state.remove_prefix( );
					return json_details::skip_string_nq( parse_state );
				} else if constexpr( JsonMember::expected_type ==
				                       JsonParseTypes::Real or
				                     JsonMember::expected_type ==
				                       JsonParseTypes::Signed or
				                     JsonMember::expected_type ==
				                       JsonParseTypes::Unsigned or
				                     JsonMember::expected_type ==
				                       JsonParseTypes::Bool or
				                     JsonMember::expected_type ==
				                       JsonParseTypes::Null ) {
					// All literals
					return skip_number( parse_state );
				} else if constexpr( JsonMember::expected_type ==
				                     JsonParseTypes::Array ) {
					daw_json_assert_weak( parse_state.is_opening_bracket_checked( ),
					                      ErrorReason::InvalidArrayStart, parse_state );
					return parse_state.skip_array( );
				} else if constexpr( JsonMember::expected_type ==
				                     JsonParseTypes::Class ) {
					daw_json_assert_weak( parse_state.is_opening_brace_checked( ),
					                      ErrorReason::InvalidClassStart, parse_state );
					return parse_state.skip_class( );
				} else {
					// Woah there
					static_assert( JsonMember::expected_type == JsonParseTypes::Class,
					               "Unknown JsonParseTypes value.  This is a programmer "
					               "error and the preceding did not check for it" );
					DAW_UNREACHABLE( );
				}
			}
		} // namespace json_details
	}   // namespace DAW_JSON_VER
} // namespace daw::json
