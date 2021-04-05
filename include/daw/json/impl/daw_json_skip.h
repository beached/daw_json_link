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
#include "namespace.h"

#include <daw/daw_hide.h>
#include <daw/daw_unreachable.h>

#include <ciso646>
#include <iterator>

namespace DAW_JSON_NS::json_details {
	/***
	 * Skip a string, after the initial quote has been skipped already
	 */
	template<typename Range>
	[[nodiscard]] DAW_ATTRIBUTE_FLATTEN static inline constexpr Range
	skip_string_nq( Range &rng ) {
		auto result = rng;
		result.counter = string_quote::string_quote_parser::parse_nq( rng );

		daw_json_assert_weak( rng.front( ) == '"', ErrorReason::InvalidString,
		                      rng );
		result.last = rng.first;
		rng.remove_prefix( );
		return result;
	}

	/***
	 * Skip a string and store the first escaped element's position, if any
	 */
	template<typename Range>
	[[nodiscard]] DAW_ATTRIBUTE_FLATTEN static inline constexpr Range
	skip_string( Range &rng ) {
		if( rng.empty( ) ) {
			return rng;
		}
		if( *std::prev( rng.first ) != '"' ) {
			daw_json_assert( rng.front( ) == '"', ErrorReason::InvalidString, rng );
			rng.remove_prefix( );
		}
		daw_json_assert_weak( rng.has_more( ), ErrorReason::InvalidString, rng );
		return skip_string_nq( rng );
	}

	template<typename Range>
	[[nodiscard]] static constexpr Range skip_true( Range &rng ) {
		auto result = rng;
		if constexpr( Range::is_unchecked_input ) {
			rng.remove_prefix( 4 );
		} else {
			rng.remove_prefix( );
			daw_json_assert( rng.starts_with( "rue" ), ErrorReason::InvalidTrue,
			                 rng );
			rng.remove_prefix( 3 );
		}
		result.last = rng.first;
		rng.trim_left( );
		daw_json_assert_weak( rng.is_at_token_after_value( ),
		                      ErrorReason::InvalidEndOfValue, rng );
		return result;
	}

	template<typename Range>
	[[nodiscard]] static constexpr Range skip_false( Range &rng ) {
		auto result = rng;
		if constexpr( Range::is_unchecked_input ) {
			rng.remove_prefix( 5 );
		} else {
			rng.remove_prefix( );
			daw_json_assert( rng.starts_with( "alse" ), ErrorReason::InvalidFalse,
			                 rng );
			rng.remove_prefix( 4 );
		}
		result.last = rng.first;
		rng.trim_left( );
		daw_json_assert_weak( rng.is_at_token_after_value( ),
		                      ErrorReason::InvalidEndOfValue, rng );
		return result;
	}

	template<typename Range>
	[[nodiscard]] static constexpr Range skip_null( Range &rng ) {
		if constexpr( Range::is_unchecked_input ) {
			rng.remove_prefix( 4 );
		} else {
			rng.remove_prefix( );
			daw_json_assert( rng.starts_with( "ull" ), ErrorReason::InvalidNull,
			                 rng );
			rng.remove_prefix( 3 );
		}
		daw_json_assert_weak( rng.has_more( ), ErrorReason::UnexpectedEndOfData,
		                      rng );
		rng.trim_left( );
		daw_json_assert_weak( rng.is_at_token_after_value( ),
		                      ErrorReason::UnexpectedEndOfData, rng );
		auto result = rng;
		result.first = nullptr;
		result.last = nullptr;
		return result;
	}

	template<bool is_unchecked_input>
	DAW_ATTRIBUTE_FLATTEN [[nodiscard]] static inline constexpr char const *
	skip_digits( char const *first, char const *const last ) {
		if( DAW_JSON_UNLIKELY( first >= last ) ) {
			return first;
		}
		unsigned dig = parse_digit( *first );
		while( dig < 10 ) {
			++first;
			if constexpr( not is_unchecked_input ) {
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
	 * Range
	 */
	template<typename Range>
	[[nodiscard]] static constexpr Range skip_number( Range &rng ) {
		auto result = rng;
		char const *first = rng.first;
		char const *const last = rng.last;
		daw_json_assert_weak( first < last, ErrorReason::UnexpectedEndOfData, rng );
		if( *first == '-' ) {
			++first;
		}
		first = skip_digits<Range::is_unchecked_input>( first, last );
		char const *decimal = nullptr;
		if( ( Range::is_unchecked_input or first < last ) and ( *first == '.' ) ) {
			decimal = first;
			++first;
			first = skip_digits<Range::is_unchecked_input>( first, last );
		}
		char const *exp = nullptr;
		unsigned dig = parse_digit( *first );
		if( ( Range::is_unchecked_input or ( first < last ) ) &
		    ( ( dig == parsed_constants::e_char ) |
		      ( dig == parsed_constants::E_char ) ) ) {
			exp = first;
			++first;
			daw_json_assert_weak( first < last, ErrorReason::UnexpectedEndOfData,
			                      [&] {
				                      auto r = rng;
				                      r.first = first;
				                      return r;
			                      }( ) );
			dig = parse_digit( *first );
			if( ( dig == parsed_constants::plus_char ) |
			    ( dig == parsed_constants::minus_char ) ) {
				++first;
			}
			first = skip_digits<Range::is_unchecked_input>( first, last );
		}

		rng.first = first;
		result.last = first;
		result.class_first = decimal;
		result.class_last = exp;
		return result;
	}

	/***
	 * When we don't know ahead of time what we are skipping switch on the first
	 * value and call that types specific skipper
	 * TODO: Investigate if there is a difference for the times we know what the
	 * member should be if that can increase performance
	 */
	template<typename Range>
	[[nodiscard]] static inline constexpr Range skip_value( Range &rng ) {
		daw_json_assert_weak( rng.has_more( ), ErrorReason::UnexpectedEndOfData,
		                      rng );

		// reset counter
		rng.counter = 0;
		switch( rng.front( ) ) {
		case '"':
			return skip_string( rng );
		case '[':
			return rng.skip_array( );
		case '{':
			return rng.skip_class( );
		case 't':
			return skip_true( rng );
		case 'f':
			return skip_false( rng );
		case 'n':
			return skip_null( rng );
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
			return skip_number( rng );
		}
		if constexpr( Range::is_unchecked_input ) {
			DAW_UNREACHABLE( );
		} else {
			daw_json_error( ErrorReason::InvalidStartOfValue, rng );
		}
	}

	/***
	 * Used in json_array_iterator::operator++( ) as we know the type we are
	 * skipping
	 */
	template<typename JsonMember, typename Range>
	[[nodiscard]] DAW_ATTRIBUTE_FLATTEN inline constexpr Range
	skip_known_value( Range &rng ) {
		daw_json_assert_weak( rng.has_more( ), ErrorReason::UnexpectedEndOfData,
		                      rng );
		if constexpr( JsonMember::expected_type == JsonParseTypes::Date or
		              JsonMember::expected_type == JsonParseTypes::StringRaw or
		              JsonMember::expected_type == JsonParseTypes::StringEscaped or
		              JsonMember::expected_type == JsonParseTypes::Custom ) {
			// json string encodings
			daw_json_assert_weak( rng.front( ) == '"', ErrorReason::InvalidString,
			                      rng );
			rng.remove_prefix( );
			return json_details::skip_string_nq( rng );
		} else if constexpr( JsonMember::expected_type == JsonParseTypes::Real or
		                     JsonMember::expected_type == JsonParseTypes::Signed or
		                     JsonMember::expected_type ==
		                       JsonParseTypes::Unsigned or
		                     JsonMember::expected_type == JsonParseTypes::Bool or
		                     JsonMember::expected_type == JsonParseTypes::Null ) {
			// All literals
			return skip_number( rng );
		} else if constexpr( JsonMember::expected_type == JsonParseTypes::Array ) {
			daw_json_assert_weak( rng.is_opening_bracket_checked( ),
			                      ErrorReason::InvalidArrayStart, rng );
			return rng.skip_array( );
		} else if constexpr( JsonMember::expected_type == JsonParseTypes::Class ) {
			daw_json_assert_weak( rng.is_opening_brace_checked( ),
			                      ErrorReason::InvalidClassStart, rng );
			return rng.skip_class( );
		} else {
			// Woah there
			static_assert( JsonMember::expected_type == JsonParseTypes::Class,
			               "Unknown JsonParseTypes value.  This is a programmer "
			               "error and the preceding did not check for it" );
			DAW_UNREACHABLE( );
		}
	}
} // namespace DAW_JSON_NS::json_details
