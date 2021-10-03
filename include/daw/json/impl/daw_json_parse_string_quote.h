// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/daw_json_link
//

#pragma once

#include "daw_json_assert.h"
#include "daw_not_const_ex_functions.h"
#include "version.h"

#include <daw/daw_traits.h>
#include <daw/daw_uint_buffer.h>

#include <ciso646>
#include <cstddef>
#include <type_traits>

namespace daw::json {
	inline namespace DAW_JSON_VER DAW_ATTRIB_PUBLIC {
		namespace json_details::string_quote {
			template<std::size_t N, char c>
			inline constexpr UInt8 test_at_byte( UInt64 b ) {
				auto const lhs = b & ( 0xFF_u64 << ( N * 8U ) );
				using rhs = daw::constant<to_uint64( static_cast<unsigned char>( c ) )
				                          << ( N * 8U )>;
				return to_uint8( not( lhs - rhs::value ) );
			}

			template<std::size_t N, char c>
			DAW_ATTRIB_INLINE inline constexpr UInt8 test_at_byte( UInt32 b ) {
				auto const lhs = b & ( 0xFF_u32 << ( N * 8U ) );
				using rhs = daw::constant<to_uint32( static_cast<unsigned char>( c ) )
				                          << ( N * 8U )>;
				return to_uint8( not( lhs - rhs::value ) );
			}

			template<typename CharT>
			inline constexpr void skip_to_first8( CharT *&first, CharT *const last ) {
				bool keep_going = last - first >= 8;
				while( keep_going ) {
					auto buff = daw::to_uint64_buffer( first );
					auto const q7 = test_at_byte<7U, '"'>( buff );
					auto const q6 = test_at_byte<6U, '"'>( buff );
					auto const q5 = test_at_byte<5U, '"'>( buff );
					auto const q4 = test_at_byte<4U, '"'>( buff );
					auto const q3 = test_at_byte<3U, '"'>( buff );
					auto const q2 = test_at_byte<2U, '"'>( buff );
					auto const q1 = test_at_byte<1U, '"'>( buff );
					auto const q0 = test_at_byte<0U, '"'>( buff );
					auto const s7 = test_at_byte<7U, '\\'>( buff );
					auto const s6 = test_at_byte<6U, '\\'>( buff );
					auto const s5 = test_at_byte<5U, '\\'>( buff );
					auto const s4 = test_at_byte<4U, '\\'>( buff );
					auto const s3 = test_at_byte<3U, '\\'>( buff );
					auto const s2 = test_at_byte<2U, '\\'>( buff );
					auto const s1 = test_at_byte<1U, '\\'>( buff );
					auto const s0 = test_at_byte<0U, '\\'>( buff );

					keep_going = not( q0 | q1 | q2 | q3 | q4 | q5 | q6 | q7 | s0 | s1 |
					                  s2 | s3 | s4 | s5 | s6 | s7 );
					keep_going = keep_going & static_cast<bool>( last - first >= 16 );
					first += static_cast<int>( keep_going ) * 8;
				}
				first -= *( first - 1 ) == '\\' ? 1 : 0;
			}

			template<typename CharT>
			inline constexpr void skip_to_first4( CharT *&first, CharT *const last ) {
				bool keep_going = last - first >= 4;
				while( keep_going ) {
					// Need to look for escapes as this is fast path
					auto buff = daw::to_uint32_buffer( first );
					auto const q3 = test_at_byte<3U, '"'>( buff );
					auto const q2 = test_at_byte<2U, '"'>( buff );
					auto const q1 = test_at_byte<1U, '"'>( buff );
					auto const q0 = test_at_byte<0U, '"'>( buff );
					auto const s3 = test_at_byte<3U, '\\'>( buff );
					auto const s2 = test_at_byte<2U, '\\'>( buff );
					auto const s1 = test_at_byte<1U, '\\'>( buff );
					auto const s0 = test_at_byte<0U, '\\'>( buff );
					keep_going = not( q0 | q1 | q2 | q3 | s0 | s1 | s2 | s3 );
					keep_going = keep_going & static_cast<bool>( last - first >= 8 );
					first += static_cast<int>( keep_going ) * 4;
				}
				first -= *( first - 1 ) == '\\' ? 1 : 0;
			}

			struct string_quote_parser {
				template<typename ParseState>
				[[nodiscard]] static constexpr auto parse_nq( ParseState &parse_state )
				  -> std::enable_if_t<ParseState::is_unchecked_input, std::size_t> {

					using CharT = typename ParseState::CharT;
					std::ptrdiff_t need_slow_path = -1;
					CharT *first = parse_state.first;
					CharT *const last = parse_state.last;
					// This is a logic error to happen.
					// daw_json_assert_weak( first != '"', "Unexpected quote", parse_state
					// );
					if constexpr( traits::not_same_v<typename ParseState::exec_tag_t,
					                                 constexpr_exec_tag> ) {
						first = mem_skip_until_end_of_string<true>(
						  ParseState::exec_tag, first, last, need_slow_path );
					} else {

						{
							auto const sz = last - first;
							if( sz >= 8 ) {
								skip_to_first8( first, last );
							} else if( sz >= 4 ) {
								skip_to_first4( first, last );
							}
						}
						while( *first != '"' ) {
							while( []( char c ) {
								return ( c != '"' ) & ( c != '\\' );
							}( *first ) ) {
								++first;
							}
							if( *first == '\\' ) {
								if( need_slow_path < 0 ) {
									need_slow_path = first - parse_state.first;
								}
								first += 2;
							} else {
								break;
							}
						}
					}
					parse_state.first = first;
					return static_cast<std::size_t>( need_slow_path );
				}

				template<typename ParseState>
				[[nodiscard]] static constexpr auto parse_nq( ParseState &parse_state )
				  -> std::enable_if_t<not ParseState::is_unchecked_input, std::size_t> {

					using CharT = typename ParseState::CharT;
					std::ptrdiff_t need_slow_path = -1;
					CharT *first = parse_state.first;
					CharT *const last = parse_state.class_last;
					if constexpr( traits::not_same_v<typename ParseState::exec_tag_t,
					                                 constexpr_exec_tag> ) {
						first = mem_skip_until_end_of_string<false>(
						  ParseState::exec_tag, first, last, need_slow_path );
					} else {
						if constexpr( not ParseState::exclude_special_escapes ) {
							if( CharT *const l = parse_state.last; l - first >= 8 ) {
								skip_to_first8( first, l );
							} else if( last - first >= 4 ) {
								skip_to_first4( first, l );
							}
						}
						if constexpr( ParseState::is_zero_terminated_string ) {
							if constexpr( ParseState::exclude_special_escapes ) {
								while( *first != '\0' ) {
									char c = *first;
									daw_json_assert( static_cast<unsigned char>( c ) >= 0x20U,
									                 ErrorReason::InvalidString, parse_state );
									if( c == '\\' ) {
										daw_json_assert( last - first > 1,
										                 ErrorReason::InvalidString, parse_state );
										if( need_slow_path < 0 ) {
											need_slow_path = first - parse_state.first;
										}
										++first;
										c = *first;
										switch( c ) {
										case '"':
										case '\\':
										case '/':
										case 'b':
										case 'f':
										case 'n':
										case 'r':
										case 't':
										case 'u':
											break;
										default:
											daw_json_error( ErrorReason::InvalidString, parse_state );
										}
									} else if( c == '"' ) {
										break;
									}
									++first;
								}
							} else {
								while( ( *first != 0 ) & ( *first != '"' ) ) {
									while( ( *first != 0 ) & ( *first != '"' ) &
									       ( *first != '\\' ) ) {
										++first;
									}

									if( ( ( *first != 0 ) & ( *first == '\\' ) ) ) {
										if( need_slow_path < 0 ) {
											need_slow_path = first - parse_state.first;
										}
										first += 2;
									} else {
										break;
									}
								}
							}
						} else {
							if constexpr( ParseState::exclude_special_escapes ) {
								while( first < last ) {
									char c = *first;
									daw_json_assert( static_cast<unsigned char>( c ) >= 0x20U,
									                 ErrorReason::InvalidString, parse_state );
									if( c == '\\' ) {
										daw_json_assert( last - first > 1,
										                 ErrorReason::InvalidString, parse_state );
										if( need_slow_path < 0 ) {
											need_slow_path = first - parse_state.first;
										}
										++first;
										c = *first;
										switch( c ) {
										case '"':
										case '\\':
										case '/':
										case 'b':
										case 'f':
										case 'n':
										case 'r':
										case 't':
										case 'u':
											break;
										default:
											daw_json_error( ErrorReason::InvalidString, parse_state );
										}
									} else if( c == '"' ) {
										break;
									}
									++first;
								}
							} else {
								while( first < last and *first != '"' ) {
									while( first < last and
									       ( ( *first != '"' ) & ( *first != '\\' ) ) ) {
										++first;
									}

									if( first < last and *first == '\\' ) {
										if( need_slow_path < 0 ) {
											need_slow_path = first - parse_state.first;
										}
										first += 2;
									} else {
										break;
									}
								}
							}
						}
					}
					if constexpr( ParseState::is_zero_terminated_string ) {
						daw_json_assert_weak( *first == '"', ErrorReason::InvalidString,
						                      parse_state );
					} else {
						daw_json_assert_weak( first < last and *first == '"',
						                      ErrorReason::InvalidString, parse_state );
					}
					parse_state.first = first;
					return static_cast<std::size_t>( need_slow_path );
				}
			};
		} // namespace json_details::string_quote
	}   // namespace DAW_JSON_VER
} // namespace daw::json
