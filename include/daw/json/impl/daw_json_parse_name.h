// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/daw_json_link
//

#pragma once

#include "daw_json_assert.h"
#include "daw_parse_std_string.h"
#include "daw_simd_functions.h"

namespace daw::json::json_details::name {

	struct name_parser_result {
		char const *end_of_name;
		char const *end_of_whitespace;
	};

	struct name_parser {
		/*
		 * end of string " -> name value separating : -> any white space
		 * the string can be escaped too
		 */
		template<typename Range>
		[[maybe_unused]] static constexpr void trim_end_of_name( Range &rng ) {
			rng.trim_left( );
			// TODO: should we check for end
			daw_json_assert_weak( rng.is_colon_unchecked( ), "Expected a ':'" );
			rng.remove_prefix( );
			rng.trim_left( );
		}

		template<typename Range>
		[[nodiscard,
		  maybe_unused]] DAW_ATTRIBUTE_FLATTEN static inline constexpr daw::
		  string_view
		  parse_nq( Range &rng ) {

			if constexpr( Range::allow_escaped_names ) {
				auto r = skip_string_nq( rng );
				trim_end_of_name( rng );
				return daw::string_view( r.data( ), r.size( ) );
			} else {
				char const *const ptr = rng.first;
#if defined( DAW_ALLOW_SSE3 )
				if constexpr( Range::simd_mode == daw::json::SIMDModes::SSE3 ) {
					rng.first = sse3_skip_string( rng.first, rng.last );
				}
#endif
				if constexpr( Range::is_unchecked_input ) {
					rng.move_to_next_of_unchecked( '"' );
				} else {
					rng.move_to_next_of_checked( '"' );
				}
				daw_json_assert_weak( rng.is_quotes_checked( ) and
				                        *std::prev( rng.first ) != '\\',
				                      "Expected a '\"' at the end of string" );
				auto result = daw::string_view( ptr, rng.first );
				rng.remove_prefix( );
				trim_end_of_name( rng );
				return result;
			}
		}
	};
} // namespace daw::json::json_details::name

namespace daw::json::json_details {

	// Paths are specified with dot separators, if the name has a dot in it,
	// it must be escaped
	// memberA.memberB.member\.C has 3 parts['memberA', 'memberB', 'member.C']
	[[nodiscard]] constexpr auto pop_json_path( daw::string_view &path ) {
		struct pop_json_path_result {
			daw::string_view current{ };
			char found_char = 0;
		} result{ };
		if( path.empty( ) ) {
			return result;
		}
		if( path.front( ) == '.' ) {
			path.remove_prefix( );
		}
		result.current = path.pop_front( [&, in_escape = false]( char c ) mutable {
			if( in_escape ) {
				in_escape = false;
				return false;
			}
			switch( c ) {
			case '\\':
				in_escape = true;
				return false;
			case '.':
			case '[':
			case ']':
				result.found_char = c;
				return true;
			default:
				return false;
			}
		} );
		return result;
	}

	[[nodiscard]] constexpr bool
	json_path_compare( daw::string_view json_path_item,
	                   daw::string_view member_name ) {
		if( json_path_item.front( ) == '\\' ) {
			json_path_item.remove_prefix( );
		}
		while( not json_path_item.empty( ) and not member_name.empty( ) ) {
			if( json_path_item.front( ) != member_name.front( ) ) {
				return false;
			}
			json_path_item.remove_prefix( );
			if( not json_path_item.empty( ) and json_path_item.front( ) == '\\' ) {
				json_path_item.remove_prefix( );
			}
			member_name.remove_prefix( );
		}
		return json_path_item.size( ) == member_name.size( );
	}

	// Get the next member name
	// Assumes that the current item in stream is a double quote
	// Ensures that the stream is left at the position of the associated
	// value(e.g after the colon(:) and trimmed)
	template<typename Range>
	[[nodiscard]] constexpr daw::string_view parse_name( Range &rng ) {
		daw_json_assert_weak( rng.is_quotes_checked( ),
		                      "Expected name to start with a quote" );
		rng.remove_prefix( );
		return name::name_parser::parse_nq( rng );
	}

	template<typename Range>
	constexpr bool find_range2( Range &rng, daw::string_view path ) {

		auto pop_result = pop_json_path( path );
		while( not pop_result.current.empty( ) ) {
			if( pop_result.found_char == ']' ) {
				// Array Index
				daw_json_assert_weak( rng.is_opening_bracket_checked( ),
				                      "Invalid Path Entry" );
				rng.remove_prefix( );
				rng.trim_left_unchecked( );
				auto idx =
				  daw::parser::parse_unsigned_int<std::size_t>( pop_result.current );

				while( idx > 0 ) {
					--idx;
					(void)skip_value( rng );
					rng.trim_left_checked( );
					if( idx > 0 and not rng.is_comma_checked( ) ) {
						return false;
					}
					rng.clean_tail( );
				}
			} else {
				daw_json_assert_weak( rng.is_opening_brace_checked( ),
				                      "Invalid Path Entry" );
				rng.remove_prefix( );
				rng.trim_left_unchecked( );
				auto name = parse_name( rng );
				while( not json_path_compare( pop_result.current, name ) ) {
					(void)skip_value( rng );
					rng.clean_tail( );
					if( rng.empty( ) or not rng.is_quotes_unchecked( ) ) {
						return false;
					}
					name = parse_name( rng );
				}
			}
			pop_result = pop_json_path( path );
		}
		return true;
	}

	template<typename ParsePolicy, typename String>
	[[nodiscard]] constexpr std::pair<bool, ParsePolicy>
	find_range( String &&str, daw::string_view start_path ) {
		static_assert( std::is_same_v<char const *, typename ParsePolicy::iterator>,
		               "Only char const * ranges are currently supported" );
		auto rng =
		  ParsePolicy( std::data( str ), std::data( str ) + std::size( str ) );
		rng.trim_left_checked( );
		if( rng.has_more( ) and not start_path.empty( ) ) {
			if( not find_range2( rng, start_path ) ) {
				return { false, rng };
			}
		}
		return { true, rng };
	}
} // namespace daw::json::json_details
