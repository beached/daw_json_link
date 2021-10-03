// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/daw_json_link
//

#pragma once

#include "daw_json_assert.h"
#include "daw_json_parse_common.h"
#include "daw_json_parse_policy_policy_details.h"
#include "daw_json_string_util.h"
#include "daw_not_const_ex_functions.h"
#include "version.h"

#include <daw/daw_attributes.h>
#include <daw/daw_function_table.h>
#include <daw/daw_likely.h>
#include <daw/daw_traits.h>

#include <ciso646>
#include <cstddef>
#include <cstdint>
#include <type_traits>

namespace daw::json DAW_ATTRIB_PUBLIC {
	inline namespace DAW_JSON_VER {
		struct NoCommentSkippingPolicy final {
			template<typename ParseState>
			DAW_ATTRIB_FLATINLINE static constexpr void
			trim_left_checked( ParseState &parse_state ) {
				if constexpr( ParseState::minified_document ) {
					return;
				} else {
					using CharT = typename ParseState::CharT;
					// SIMD here was much slower, most JSON has very minimal whitespace
					CharT *first = parse_state.first;
					CharT *const last = parse_state.last;

					// only used when not zero terminated string and gcc9 warns
					(void)last;

					if constexpr( ParseState::is_zero_terminated_string ) {
						// Ensure that zero terminator isn't included in skipable value
						while( DAW_UNLIKELY(
						  ( static_cast<unsigned>( static_cast<unsigned char>( *first ) ) -
						    1U ) <= 0x1FU ) ) {

							++first;
						}
					} else {
						while(
						  DAW_LIKELY( first < last ) and
						  ( static_cast<unsigned>( static_cast<unsigned char>( *first ) ) -
						    1U ) <= 0x1FU ) {
							++first;
						}
					}
					parse_state.first = first;
				}
			}

			template<typename ParseState>
			DAW_ATTRIB_FLATINLINE static constexpr void
			trim_left_unchecked( ParseState &parse_state ) {
				if constexpr( ParseState::minified_document ) {
					return;
				} else {
					using CharT = typename ParseState::CharT;
					CharT *first = parse_state.first;
					while( DAW_UNLIKELY(
					  ( static_cast<unsigned>( static_cast<unsigned char>( *first ) ) -
					    1U ) <= 0x1F ) ) {

						++first;
					}
					parse_state.first = first;
				}
			}

			template<typename ParseState>
			DAW_ATTRIB_FLATINLINE static constexpr void
			move_next_member_unchecked( ParseState &parse_state ) {
				parse_state.first =
				  json_details::memchr_unchecked<'"', typename ParseState::exec_tag_t,
				                                 ParseState::expect_long_strings>(
				    parse_state.first, parse_state.last );
			}

			template<char... keys, typename ParseState>
			DAW_ATTRIB_FLATINLINE static constexpr void
			move_to_next_of( ParseState &parse_state ) {
				static_assert( sizeof...( keys ) > 0 );
				static_assert( sizeof...( keys ) <= 16 );

				using CharT = typename ParseState::CharT;

				if constexpr( traits::not_same<typename ParseState::exec_tag_t,
				                               constexpr_exec_tag>::value ) {
					parse_state.first =
					  json_details::mempbrk<ParseState::is_unchecked_input,
					                        typename ParseState::exec_tag_t,
					                        ParseState::expect_long_strings, keys...>(
					    parse_state.first, parse_state.last );
				} else {
					CharT *first = parse_state.first;
					CharT *const last = parse_state.last;

					// silencing gcc9 unused warning.  last is used inside if constexpr
					// blocks
					(void)last;

					if( ParseState::is_zero_terminated_string ) {
						daw_json_assert_weak( first < last and *first != '\0',
						                      ErrorReason::UnexpectedEndOfData,
						                      parse_state );
						while( not parse_policy_details::in<keys...>( *first ) ) {
							++first;
						}
						daw_json_assert_weak( *first != 0, ErrorReason::UnexpectedEndOfData,
						                      parse_state );
					} else {
						daw_json_assert_weak(
						  first < last, ErrorReason::UnexpectedEndOfData, parse_state );
						while( not parse_policy_details::in<keys...>( *first ) ) {
							++first;
							daw_json_assert_weak(
							  first < last, ErrorReason::UnexpectedEndOfData, parse_state );
						}
					}
					parse_state.first = first;
				}
			}

			DAW_ATTRIB_INLINE static constexpr bool is_literal_end( char c ) {
				return ( c == '\0' ) | ( c == ',' ) | ( c == ']' ) | ( c == '}' );
			}

			template<char PrimLeft, char PrimRight, char SecLeft, char SecRight,
			         typename ParseState>
			DAW_ATTRIB_FLATTEN static constexpr ParseState
			skip_bracketed_item_checked( ParseState &parse_state ) {
				using CharT = typename ParseState::CharT;
				// Not checking for Left as it is required to be skipped already
				auto result = parse_state;
				std::size_t cnt = 0;
				std::uint32_t prime_bracket_count = 1;
				std::uint32_t second_bracket_count = 0;
				CharT *ptr_first = parse_state.first;
				CharT *const ptr_last = parse_state.last;

				if( DAW_UNLIKELY( ptr_first >= ptr_last ) ) {
					return result;
				}
				if( *ptr_first == PrimLeft ) {
					++ptr_first;
				}
				if constexpr( ParseState::is_zero_terminated_string ) {
					daw_json_assert( ptr_first < ptr_last,
					                 ErrorReason::UnexpectedEndOfData, parse_state );
					while( *ptr_first != 0 ) {
						switch( *ptr_first ) {
						case '\\':
							++ptr_first;
							break;
						case '"':
							++ptr_first;
							if constexpr( traits::not_same<typename ParseState::exec_tag_t,
							                               constexpr_exec_tag>::value ) {
								ptr_first = json_details::mem_skip_until_end_of_string<
								  ParseState::is_unchecked_input>(
								  ParseState::exec_tag, ptr_first, parse_state.last );
							} else {
								char c = *ptr_first;
								while( ( c != '\0' ) & ( c != '"' ) ) {
									if( c == '\\' ) {
										if( ptr_first + 1 < ptr_last ) {
											ptr_first += 2;
											c = *ptr_first;
											continue;
										} else {
											ptr_first = ptr_last;
											break;
										}
									}
									++ptr_first;
									c = *ptr_first;
								}
							}
							daw_json_assert( ( *ptr_first != '\0' ) & ( *ptr_first == '"' ),
							                 ErrorReason::UnexpectedEndOfData, parse_state );
							break;
						case ',':
							if( DAW_UNLIKELY( ( prime_bracket_count == 1 ) &
							                  ( second_bracket_count == 0 ) ) ) {
								++cnt;
							}
							break;
						case PrimLeft:
							++prime_bracket_count;
							break;
						case PrimRight:
							--prime_bracket_count;
							if( prime_bracket_count == 0 ) {
								++ptr_first;
								daw_json_assert( second_bracket_count == 0,
								                 ErrorReason::InvalidBracketing, parse_state );
								result.last = ptr_first;
								result.counter = cnt;
								parse_state.first = ptr_first;
								return result;
							}
							break;
						case SecLeft:
							++second_bracket_count;
							break;
						case SecRight:
							--second_bracket_count;
							break;
						}
						++ptr_first;
					}
				} else {
					while( DAW_LIKELY( ptr_first < ptr_last ) ) {
						switch( *ptr_first ) {
						case '\\':
							++ptr_first;
							break;
						case '"':
							++ptr_first;
							if constexpr( traits::not_same<typename ParseState::exec_tag_t,
							                               constexpr_exec_tag>::value ) {
								ptr_first = json_details::mem_skip_until_end_of_string<
								  ParseState::is_unchecked_input>(
								  ParseState::exec_tag, ptr_first, parse_state.last );
							} else {
								while( DAW_LIKELY( ptr_first < ptr_last ) and
								       *ptr_first != '"' ) {
									if( *ptr_first == '\\' ) {
										if( ptr_first + 1 < ptr_last ) {
											ptr_first += 2;
											continue;
										} else {
											ptr_first = ptr_last;
											break;
										}
									}
									++ptr_first;
								}
							}
							daw_json_assert( ptr_first < ptr_last and *ptr_first == '"',
							                 ErrorReason::UnexpectedEndOfData, parse_state );
							break;
						case ',':
							if( DAW_UNLIKELY( ( prime_bracket_count == 1 ) &
							                  ( second_bracket_count == 0 ) ) ) {
								++cnt;
							}
							break;
						case PrimLeft:
							++prime_bracket_count;
							break;
						case PrimRight:
							--prime_bracket_count;
							if( prime_bracket_count == 0 ) {
								++ptr_first;
								daw_json_assert( second_bracket_count == 0,
								                 ErrorReason::InvalidBracketing, parse_state );
								result.last = ptr_first;
								result.counter = cnt;
								parse_state.first = ptr_first;
								return result;
							}
							break;
						case SecLeft:
							++second_bracket_count;
							break;
						case SecRight:
							--second_bracket_count;
							break;
						}
						++ptr_first;
					}
				}
				daw_json_assert( ( prime_bracket_count == 0 ) &
				                   ( second_bracket_count == 0 ),
				                 ErrorReason::InvalidBracketing, parse_state );
				// We include the close primary bracket in the range so that subsequent
				// parsers have a terminator inside their range
				result.last = ptr_first;
				result.counter = cnt;
				parse_state.first = ptr_first;
				return result;
			}

			template<char PrimLeft, char PrimRight, char SecLeft, char SecRight,
			         typename ParseState>
			DAW_ATTRIB_FLATTEN static constexpr ParseState
			skip_bracketed_item_unchecked( ParseState &parse_state ) {
				// Not checking for Left as it is required to be skipped already
				using CharT = typename ParseState::CharT;
				auto result = parse_state;
				std::size_t cnt = 0;
				std::uint32_t prime_bracket_count = 1;
				std::uint32_t second_bracket_count = 0;
				CharT *ptr_first = parse_state.first;

				if( *ptr_first == PrimLeft ) {
					++ptr_first;
				}
				while( true ) {
					switch( *ptr_first ) {
					case '\\':
						++ptr_first;
						break;
					case '"':
						++ptr_first;
						if constexpr( traits::not_same<typename ParseState::exec_tag_t,
						                               constexpr_exec_tag>::value ) {
							ptr_first = json_details::mem_skip_until_end_of_string<
							  ParseState::is_unchecked_input>( ParseState::exec_tag,
							                                   ptr_first, parse_state.last );
						} else {
							while( *ptr_first != '"' ) {
								if( *ptr_first == '\\' ) {
									++ptr_first;
								}
								++ptr_first;
							}
						}
						break;
					case ',':
						if( DAW_UNLIKELY( ( prime_bracket_count == 1 ) &
						                  ( second_bracket_count == 0 ) ) ) {
							++cnt;
						}
						break;
					case PrimLeft:
						++prime_bracket_count;
						break;
					case PrimRight:
						--prime_bracket_count;
						if( prime_bracket_count == 0 ) {
							++ptr_first;
							// We include the close primary bracket in the range so that
							// subsequent parsers have a terminator inside their range
							result.last = ptr_first;
							result.counter = cnt;
							parse_state.first = ptr_first;
							return result;
						}
						break;
					case SecLeft:
						++second_bracket_count;
						break;
					case SecRight:
						--second_bracket_count;
						break;
					}
					++ptr_first;
				}
				// Should never get here, only loop exit is when PrimaryRight is found
				// and count == 0
				DAW_UNREACHABLE( );
			}
		};
	} // namespace DAW_JSON_VER
} // namespace daw::json
