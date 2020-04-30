// The MIT License (MIT)
//
// Copyright (c) Darrell Wright
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files( the "Software" ), to
// deal in the Software without restriction, including without limitation the
// rights to use, copy, modify, merge, publish, distribute, sublicense, and / or
// sell copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#pragma once

#include "daw_json_assert.h"
#include "daw_json_parse_string_quote.h"

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
		template<typename First, typename Last, bool IsUnCheckedInput>
		[[maybe_unused]] static constexpr void
		trim_end_of_name( IteratorRange<First, Last, IsUnCheckedInput> &rng ) {
			while( rng.is_space( ) ) {
				rng.remove_prefix( );
			}
			daw_json_assert_weak( rng.front( ) == ':', "Expected a ':'" );
			rng.remove_prefix( );
			while( rng.is_space( ) ) {
				rng.remove_prefix( );
			}
		}

		template<typename First, typename Last, bool IsUnCheckedInput>
		[[nodiscard, maybe_unused]] static constexpr daw::string_view
		parse_nq( IteratorRange<First, Last, IsUnCheckedInput> &rng ) {
			auto ptr = rng.begin( );
			while( rng.front( ) != '"' ) {
				while( rng.front( ) != '"' and rng.front( ) != '\\' ) {
					rng.remove_prefix( );
				}
				if( rng.front( ) == '\\' ) {
					rng.remove_prefix( 2 );
				}
			}
			daw_json_assert_weak( rng.front( ) == '"', "Expected a '\"' at the end of string" );
			auto result =
			  daw::string_view( ptr, static_cast<std::size_t>( rng.begin( ) - ptr ) );
			rng.remove_prefix( );
			trim_end_of_name( rng );
			return result;
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
	template<typename First, typename Last, bool IsUnCheckedInput>
	[[nodiscard]] constexpr daw::string_view
	parse_name( IteratorRange<First, Last, IsUnCheckedInput> &rng ) {
		daw_json_assert_weak( rng.front( '"' ),
		                      "Expected name to start with a quote" );
		rng.remove_prefix( );
		return name::name_parser::parse_nq( rng );
	}

	template<typename First, typename Last, bool IsUnCheckedInput>
	constexpr bool find_range2( IteratorRange<First, Last, IsUnCheckedInput> &rng,
	                            daw::string_view path ) {

		auto pop_result = pop_json_path( path );
		while( not pop_result.current.empty( ) ) {
			if( pop_result.found_char == ']' ) {
				// Array Index
				daw_json_assert_weak( rng.front( '[' ), "Invalid Path Entry" );
				rng.remove_prefix( );
				rng.trim_left_no_check( );
				auto idx =
				  daw::parser::parse_unsigned_int<std::size_t>( pop_result.current );

				while( idx > 0 ) {
					--idx;
					(void)skip_value( rng );
					rng.trim_left( );
					if( idx > 0 and not rng.front( ',' ) ) {
						return false;
					}
					rng.clean_tail( );
				}
			} else {
				daw_json_assert_weak( rng.front( '{' ), "Invalid Path Entry" );
				rng.remove_prefix( );
				rng.trim_left_no_check( );
				auto name = parse_name( rng );
				while( not json_path_compare( pop_result.current, name ) ) {
					(void)skip_value( rng );
					rng.clean_tail( );
					if( rng.empty( ) or rng.front( ) != '"' ) {
						return false;
					}
					name = parse_name( rng );
				}
			}
			pop_result = pop_json_path( path );
		}
		return true;
	}

	template<bool IsUnCheckedInput, typename String>
	[[nodiscard]] constexpr std::pair<
	  bool, IteratorRange<char const *, char const *, IsUnCheckedInput>>
	find_range( String &&str, daw::string_view start_path ) {

		auto rng = IteratorRange<char const *, char const *, IsUnCheckedInput>(
		  std::data( str ), std::data( str ) + std::size( str ) );
		rng.trim_left( );
		if( rng.has_more( ) and not start_path.empty( ) ) {
			if( not find_range2( rng, start_path ) ) {
				return { false, rng };
			}
		}
		return { true, rng };
	}

} // namespace daw::json::json_details
