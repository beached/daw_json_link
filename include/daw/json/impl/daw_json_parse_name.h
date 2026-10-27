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

#include <daw/daw_arith_traits.h>
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
					                      ErrorReason::InvalidMemberName,
					                      parse_state );
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
						auto const ptr = parse_state.first;
						if constexpr( ParseState::is_unchecked_input ) {
							parse_state.template move_to_next_of_unchecked<'"'>( );
						} else {
							parse_state.template move_to_next_of_checked<'"'>( );
						}
						daw_json_assert_weak( parse_state.is_quotes_checked( ) and
						                        *std::prev( parse_state.first ) != '\\',
						                      ErrorReason::InvalidString,
						                      parse_state );
						auto result = daw::string_view( ptr, parse_state.first );
						parse_state.remove_prefix( );
						trim_end_of_name( parse_state );
						return result;
					}
				}
			} // namespace name::name_parser

			namespace name {
				/// @brief Receives a decoded member name in a fixed buffer.  A name
				/// longer than MaxSize cannot be any of the mapped names, so the
				/// decode stops once the buffer is full
				template<std::size_t MaxSize>
				struct name_buffer {
					char data[MaxSize > 0 ? MaxSize : 1]{ };
					std::size_t size = 0;

					[[nodiscard]] constexpr bool operator( )( char c ) {
						if( size == MaxSize ) {
							return false;
						}
						data[size] = c;
						++size;
						return true;
					}

					[[nodiscard]] constexpr daw::string_view view( ) const {
						return daw::string_view( data, size );
					}
				};

				/// @brief Receives a decoded member name in a string, with no limit
				template<typename String>
				struct name_string_appender {
					String *str;

					[[nodiscard]] constexpr bool operator( )( char c ) const {
						str->push_back( c );
						return true;
					}
				};

				template<typename Appender>
				[[nodiscard]] DAW_ATTRIB_INLINE constexpr bool
				append_utf8( UInt32 cp, Appender &app ) {
					if( cp < 0x80U ) {
						return app( u32toC( cp ) );
					}
					if( cp < 0x800U ) {
						return app( u32toC( ( cp >> 6U ) | 0b1100'0000U ) ) and
						       app( u32toC( ( cp & 0b0011'1111U ) | 0b1000'0000U ) );
					}
					if( cp < 0x10000U ) {
						return app( u32toC( ( cp >> 12U ) | 0b1110'0000U ) ) and
						       app( u32toC( ( ( cp >> 6U ) & 0b0011'1111U ) |
						                    0b1000'0000U ) ) and
						       app( u32toC( ( cp & 0b0011'1111U ) | 0b1000'0000U ) );
					}
					return app( u32toC( ( cp >> 18U ) | 0b1111'0000U ) ) and
					       app( u32toC( ( ( cp >> 12U ) & 0b0011'1111U ) |
					                    0b1000'0000U ) ) and
					       app( u32toC( ( ( cp >> 6U ) & 0b0011'1111U ) |
					                    0b1000'0000U ) ) and
					       app( u32toC( ( cp & 0b0011'1111U ) | 0b1000'0000U ) );
				}

				/// @brief The 4 hex digits of the UTF-16 escape at first
				template<typename ParseState>
				[[nodiscard]] DAW_ATTRIB_INLINE constexpr UInt32
				parse_utf16_unit( char const *first ) {
					auto pos = daw::not_null<char const *>( daw::never_null, first + 2 );
					UInt32 cp =
					  to_uint32(
					    byte_from_nibbles<ParseState::is_unchecked_input>( pos ) )
					  << 8U;
					cp |= byte_from_nibbles<ParseState::is_unchecked_input>( pos );
					return cp;
				}

				/// @brief The char a two char escape stands for, or 0 when c does not
				/// follow a backslash in JSON.  UTF-16 escapes are decoded separately
				[[nodiscard]] DAW_ATTRIB_INLINE constexpr char escaped_char( char c ) {
					switch( c ) {
					case '"':
					case '\\':
					case '/':
						return c;
					case 'b':
						return '\b';
					case 'f':
						return '\f';
					case 'n':
						return '\n';
					case 'r':
						return '\r';
					case 't':
						return '\t';
					default:
						return 0;
					}
				}

				/// @brief Decode the escapes in the name between its quotes, raw, and
				/// give the result to app one char at a time.  Returns false when app
				/// stops taking chars.  Invalid escapes and unpaired surrogates are
				/// errors.  With unchecked input, an escape that runs past the end of
				/// the name stops the decode instead of reading past it
				template<typename ParseState, typename Appender>
				[[nodiscard]] constexpr bool
				decode_name( daw::string_view raw, Appender &app,
				             ParseState const &parse_state ) {
					(void)parse_state;
					char const *first = std::data( raw );
					char const *const last = daw::data_end( raw );
					while( first < last ) {
						if( *first != '\\' ) {
							if( not app( *first ) ) {
								return false;
							}
							++first;
							continue;
						}
						daw_json_assert_weak(
						  last - first >= 2, ErrorReason::InvalidString, parse_state );
						if( last - first < 2 ) {
							return false;
						}
						if( first[1] != 'u' ) {
							char const c = escaped_char( first[1] );
							daw_json_assert_weak(
							  c != 0, ErrorReason::InvalidString, parse_state );
							if( not app( c ) ) {
								return false;
							}
							first += 2;
							continue;
						}
						daw_json_assert_weak(
						  last - first >= 6, ErrorReason::InvalidUTFEscape, parse_state );
						if( last - first < 6 ) {
							return false;
						}
						UInt32 cp = parse_utf16_unit<ParseState>( first );
						first += 6;
						daw_json_assert_weak( cp < 0xDC00U or cp > 0xDFFFU,
						                      ErrorReason::InvalidUTFEscape,
						                      parse_state ); // Unpaired low surrogate
						if( 0xD800U <= cp and cp <= 0xDBFFU ) {
							// A high surrogate must be followed by a low one
							daw_json_assert_weak( last - first >= 6 and first[0] == '\\' and
							                        first[1] == 'u',
							                      ErrorReason::InvalidUTFEscape,
							                      parse_state );
							if( last - first < 6 ) {
								return false;
							}
							UInt32 const trailing = parse_utf16_unit<ParseState>( first );
							first += 6;
							daw_json_assert_weak( 0xDC00U <= trailing and
							                        trailing <= 0xDFFFU,
							                      ErrorReason::InvalidUTFEscape,
							                      parse_state );
							cp = 0x10000U + ( ( cp - 0xD800U ) * 0x400U ) +
							     ( trailing - 0xDC00U );
						}
						if( not append_utf8( cp, app ) ) {
							return false;
						}
					}
					return true;
				}
			} // namespace name

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
				std::size_t count = daw::digits<Result>;

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
			[[nodiscard]] DAW_ATTRIB_FLATTEN static constexpr daw::string_view
			parse_name( ParseState &parse_state ) {
				daw_json_assert_weak( parse_state.is_quotes_checked( ),
				                      ErrorReason::InvalidMemberName,
				                      parse_state );
				parse_state.remove_prefix( );
				return name::name_parser::parse_nq( parse_state );
			}

			template<typename ParseState>
			static constexpr bool find_range2( ParseState &parse_state,
			                                   daw::string_view path ) {

				auto pop_result = pop_json_path( path );
				while( not pop_result.current.empty( ) or
				       pop_result.found_char != 0 ) {
					// A path relative to an array starts with '['.  In that case the
					// first popped component is empty and the index is the next one.
					if( pop_result.current.empty( ) ) {
						daw_json_assert_weak( pop_result.found_char == '[',
						                      ErrorReason::InvalidJSONPath,
						                      parse_state );
						pop_result = pop_json_path( path );
						continue;
					}
					if( pop_result.found_char == ']' ) {
						// Array Index
						daw_json_assert_weak( parse_state.is_opening_bracket_checked( ),
						                      ErrorReason::InvalidJSONPath,
						                      parse_state );
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
						                      ErrorReason::InvalidJSONPath,
						                      parse_state );
						parse_state.remove_prefix( );
						parse_state.trim_left_unchecked( );
						if( parse_state.empty( ) or
						    parse_state.is_closing_brace_checked( ) ) {
							return false;
						}
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
