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
#include "daw_json_parse_std_string.h"
#include "daw_not_const_ex_functions.h"

#include <ciso646>

namespace daw::json {
	inline namespace DAW_JSON_VER {
		namespace json_details::name::name_parser {
			/*
			 * end of string " -> name value separating : -> any white space
			 * the string can be escaped too
			 */
			template<typename ParseState>
			[[maybe_unused]] static constexpr void
			trim_end_of_name( ParseState &parse_state ) {
				parse_state.trim_left( );
				// TODO: should we check for end
				daw_json_assert_weak( parse_state.front( ) == ':',
				                      ErrorReason::InvalidMemberName, parse_state );
				parse_state.remove_prefix( );
				parse_state.trim_left( );
			}

			template<typename ParseState>
			[[nodiscard, maybe_unused]] inline constexpr daw::string_view
			parse_nq( ParseState &parse_state ) {
				if constexpr( ParseState::allow_escaped_names ) {
					auto r = skip_string_nq( parse_state );
					trim_end_of_name( parse_state );
					return daw::string_view( std::data( r ), std::size( r ) );
				} else {
					char const *const ptr = parse_state.first;
					if constexpr( ParseState::is_unchecked_input ) {
						parse_state.template move_to_next_of_unchecked<'"'>( );
					} else {
						parse_state.template move_to_next_of_checked<'"'>( );
					}
					daw_json_assert_weak( parse_state.is_quotes_checked( ) and
					                        *std::prev( parse_state.first ) != '\\',
					                      ErrorReason::InvalidString, parse_state );
					auto result = daw::string_view( ptr, parse_state.first );
					parse_state.remove_prefix( );
					trim_end_of_name( parse_state );
					return result;
				}
			}
		} // namespace json_details::name::name_parser

		namespace json_details {
			struct pop_json_path_result {
				daw::string_view current{ };
				char found_char = 0;
			};
			// Paths are specified with dot separators, if the name has a dot in it,
			// it must be escaped
			// memberA.memberB.member\.C has 3 parts['memberA', 'memberB', 'member.C']
			[[nodiscard]] constexpr pop_json_path_result
			pop_json_path( daw::string_view &path ) {
				auto result = pop_json_path_result{ };
				if( path.empty( ) ) {
					return result;
				}
				if( path.front( ) == '.' ) {
					path.remove_prefix( );
				}
				result.current =
				  path.pop_front( [&, in_escape = false]( char c ) mutable {
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
					if( not json_path_item.empty( ) and
					    json_path_item.front( ) == '\\' ) {
						json_path_item.remove_prefix( );
					}
					member_name.remove_prefix( );
				}
				return std::size( json_path_item ) == std::size( member_name );
			}

			// Get the next member name
			// Assumes that the current item in stream is a double quote
			// Ensures that the stream is left at the position of the associated
			// value(e.g after the colon(:) and trimmed)
			template<typename ParseState>
			[[nodiscard]] DAW_ATTRIB_FLATTEN constexpr daw::string_view
			parse_name( ParseState &parse_state ) {
				daw_json_assert_weak( parse_state.is_quotes_checked( ),
				                      ErrorReason::InvalidMemberName, parse_state );
				parse_state.remove_prefix( );
				return name::name_parser::parse_nq( parse_state );
			}

			template<typename ParseState>
			constexpr bool find_range2( ParseState &parse_state,
			                            daw::string_view path ) {

				auto pop_result = pop_json_path( path );
				while( not pop_result.current.empty( ) ) {
					if( pop_result.found_char == ']' ) {
						// Array Index
						daw_json_assert_weak( parse_state.is_opening_bracket_checked( ),
						                      ErrorReason::InvalidJSONPath, parse_state );
						parse_state.remove_prefix( );
						parse_state.trim_left_unchecked( );
						auto idx = daw::parser::parse_unsigned_int<std::size_t>(
						  pop_result.current );

						while( idx > 0 ) {
							--idx;
							(void)skip_value( parse_state );
							parse_state.trim_left_checked( );
							if( ( idx > 0 ) & ( parse_state.has_more( ) and
							                    ( parse_state.front( ) != ',' ) ) ) {
								return false;
							}
							parse_state.move_next_member_or_end( );
						}
					} else {
						daw_json_assert_weak( parse_state.is_opening_brace_checked( ),
						                      ErrorReason::InvalidJSONPath, parse_state );
						parse_state.remove_prefix( );
						parse_state.trim_left_unchecked( );
						auto name = parse_name( parse_state );
						while( not json_path_compare( pop_result.current, name ) ) {
							(void)skip_value( parse_state );
							parse_state.move_next_member_or_end( );
							if( parse_state.empty( ) or parse_state.front( ) != '"' ) {
								return false;
							}
							name = parse_name( parse_state );
						}
					}
					pop_result = pop_json_path( path );
				}
				return true;
			}

			template<typename ParsePolicy, typename String>
			[[nodiscard]] constexpr std::pair<bool, ParsePolicy>
			find_range( String &&str, daw::string_view start_path ) {
				static_assert( std::is_convertible_v<decltype( std::data( str ) ),
				                                     typename ParsePolicy::CharT *> );

				auto parse_state =
				  ParsePolicy( std::data( str ), daw::data_end( str ) );
				parse_state.trim_left_checked( );
				if( parse_state.has_more( ) and not start_path.empty( ) ) {
					if( not find_range2( parse_state, start_path ) ) {
						return std::pair<bool, ParsePolicy>( false, parse_state );
					}
				}
				return std::pair<bool, ParsePolicy>( true, parse_state );
			}

			template<typename ParsePolicy, typename String, typename Allocator>
			[[nodiscard]] constexpr auto find_range( String &&str,
			                                         daw::string_view start_path,
			                                         Allocator &alloc ) {
				static_assert(
				  std::is_same<char const *, typename ParsePolicy::iterator>::value,
				  "Only char const * ranges are currently supported" );
				auto parse_state = ParsePolicy::with_allocator(
				  std::data( str ), daw::data_end( str ), alloc );
				parse_state.trim_left_checked( );
				if( parse_state.has_more( ) and not start_path.empty( ) ) {
					if( not find_range2( parse_state, start_path ) ) {
						return std::pair{ false, parse_state };
					}
				}
				return std::pair{ true, parse_state };
			}
		} // namespace json_details
	}   // namespace DAW_JSON_VER
} // namespace daw::json
