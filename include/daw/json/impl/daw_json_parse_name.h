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
#include "daw_json_find_result.h"
#include "daw_json_parse_std_string.h"
#include "daw_not_const_ex_functions.h"

#include <daw/daw_string_view.h>

#include <cstddef>
#include <daw/stdinc/data_access.h>
#include <daw/stdinc/range_access.h>
#include <limits>

namespace daw::json {
	inline namespace DAW_JSON_VER {
		namespace json_details {
			namespace name::name_parser {
				/*
				 * end of string " -> name value separating : -> any white space
				 * the string can be escaped too
				 */
				template<typename ParseState>
				static constexpr void trim_end_of_name( ParseState &parse_state ) {
					parse_state.trim_left( );
					// TODO: should we check for end
					daw_json_assert_weak( parse_state.front_checked( ) == ':',
					                      ErrorReason::InvalidMemberName, parse_state );
					parse_state.remove_prefix( );
					parse_state.trim_left( );
				}

				template<typename ParseState>
				[[nodiscard]] DAW_ATTRIB_INLINE static constexpr daw::string_view
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
			} // namespace name::name_parser

			struct pop_json_path_result {
				daw::string_view current{ };
				char found_char = 0;
			};
			// Paths are specified with dot separators, if the name has a dot in it,
			// it must be escaped
			// memberA.memberB.member\.C has 3 parts['memberA', 'memberB', 'member.C']
			[[nodiscard]] static constexpr pop_json_path_result
			pop_json_path( daw::string_view &path ) {
				auto result = pop_json_path_result{ };
				if( path.empty( ) ) {
					return result;
				}
				if( path.front( ) == '.' ) {
					path.remove_prefix( );
				}
				result.current =
				  path.pop_front_until( [&, in_escape = false]( char c ) mutable {
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

			[[nodiscard]] static constexpr bool
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

			template<typename Result, typename ForwardIterator>
			constexpr Result parse_unsigned_int( ForwardIterator first,
			                                     ForwardIterator last ) {
				std::size_t count = std::numeric_limits<Result>::digits;

				daw_json_ensure( '-' != *first, ErrorReason::InvalidNumber );

				Result result = 0;
				for( ; first != last and count > 0; ++first, --count ) {
					result *= static_cast<Result>( 10 );
					Result val =
					  static_cast<Result>( *first ) - static_cast<Result>( '0' );
					result += val;
				}
				daw_json_ensure( first == last, ErrorReason::InvalidNumber );
				return result;
			}

			// Get the next member name
			// Assumes that the current item in stream is a double quote
			// Ensures that the stream is left at the position of the associated
			// value(e.g after the colon(:) and trimmed)
			template<typename ParseState>
			[[nodiscard]] DAW_ATTRIB_FLATTEN static inline constexpr daw::string_view
			parse_name( ParseState &parse_state ) {
				daw_json_assert_weak( parse_state.is_quotes_checked( ),
				                      ErrorReason::InvalidMemberName, parse_state );
				parse_state.remove_prefix( );
				return name::name_parser::parse_nq( parse_state );
			}

			template<typename ParseState>
			static constexpr bool find_range2( ParseState &parse_state,
			                                   daw::string_view path ) {

				auto pop_result = pop_json_path( path );
				while( not pop_result.current.empty( ) ) {
					if( pop_result.found_char == ']' ) {
						// Array Index
						daw_json_assert_weak( parse_state.is_opening_bracket_checked( ),
						                      ErrorReason::InvalidJSONPath, parse_state );
						parse_state.remove_prefix( );
						parse_state.trim_left_unchecked( );
						auto idx = parse_unsigned_int<std::size_t>(
						  pop_result.current.data( ), pop_result.current.data_end( ) );

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

			template<typename ParsePolicy>
			[[nodiscard]] static constexpr find_result<ParsePolicy>
			find_range( daw::string_view str, daw::string_view start_path ) {
				auto parse_state =
				  ParsePolicy( std::data( str ), daw::data_end( str ) );
				parse_state.trim_left_checked( );
				bool found = true;
				if( parse_state.has_more( ) and not start_path.empty( ) ) {
					found = find_range2( parse_state, start_path );
				}
				return find_result<ParsePolicy>{ parse_state, found };
			}

			template<typename ParsePolicy, typename Allocator>
			[[nodiscard]] static constexpr find_result<ParsePolicy>
			find_range( daw::string_view str, daw::string_view start_path,
			            Allocator &alloc ) {
				static_assert(
				  std::is_same_v<char const *, typename ParsePolicy::iterator>,
				  "Only char const * ranges are currently supported" );
				auto parse_state = ParsePolicy::with_allocator(
				  std::data( str ), daw::data_end( str ), alloc );
				parse_state.trim_left_checked( );
				if( parse_state.has_more( ) and not start_path.empty( ) ) {
					if( not find_range2( parse_state, start_path ) ) {
						return find_result<ParsePolicy>{ parse_state, false };
					}
				}
				return find_result<ParsePolicy>{ parse_state, true };
			}
		} // namespace json_details
	} // namespace DAW_JSON_VER
} // namespace daw::json
