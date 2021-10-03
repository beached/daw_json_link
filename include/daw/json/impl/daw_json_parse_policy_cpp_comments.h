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
#include "daw_not_const_ex_functions.h"
#include "version.h"

#include <daw/daw_attributes.h>
#include <daw/daw_likely.h>
#include <daw/daw_traits.h>

#include <ciso646>

namespace daw::json DAW_ATTRIB_PUBLIC {
	inline namespace DAW_JSON_VER {
		/***
		 * Allow skipping C++ style comments in JSON document
		 */
		class CppCommentSkippingPolicy final {
			template<typename ParseState>
			DAW_ATTRIB_FLATINLINE static constexpr void
			skip_comments_unchecked( ParseState &parse_state ) {
				while( parse_state.front( ) == '/' ) {
					switch( *( parse_state.first + 1 ) ) {
					case '/':
						parse_state.template move_to_next_of_unchecked<'\n'>( );
						parse_state.remove_prefix( );
						break;
					case '*':
						parse_state.remove_prefix( 2 );
						while( true ) {
							parse_state.template move_to_next_of_unchecked<'*'>( );
							parse_state.remove_prefix( );
							if( parse_state.front( ) == '/' ) {
								parse_state.remove_prefix( );
								break;
							}
							parse_state.remove_prefix( );
						}
						break;
					default:
						return;
					}
				}
			}

			template<typename ParseState>
			DAW_ATTRIB_FLATINLINE static constexpr void
			skip_comments_checked( ParseState &parse_state ) {
				while( parse_state.has_more( ) and parse_state.front( ) == '/' ) {
					if( not parse_state.has_more( ) ) {
						return;
					}
					switch( *( parse_state.first + 1 ) ) {
					case '/':
						parse_state.template move_to_next_of_checked<'\n'>( );
						if( parse_state.has_more( ) ) {
							parse_state.remove_prefix( );
						}
						break;
					case '*':
						parse_state.remove_prefix( 2 );
						while( parse_state.has_more( ) ) {
							parse_state.template move_to_next_of_checked<'*'>( );
							if( parse_state.has_more( ) ) {
								parse_state.remove_prefix( );
							}
							if( not parse_state.has_more( ) ) {
								break;
							} else if( parse_state.front( ) == '/' ) {
								parse_state.remove_prefix( );
								break;
							}
							parse_state.remove_prefix( );
						}
						break;
					default:
						return;
					}
				}
			}

			template<typename ParseState>
			DAW_ATTRIB_FLATINLINE static constexpr void
			skip_comments( ParseState &parse_state ) {
				if constexpr( ParseState::is_unchecked_input ) {
					skip_comments_unchecked( parse_state );
				} else {
					skip_comments_checked( parse_state );
				}
			}

		public:
			template<typename ParseState>
			DAW_ATTRIB_FLATINLINE static constexpr void
			trim_left_checked( ParseState &parse_state ) {
				skip_comments_checked( parse_state );
				while( parse_state.has_more( ) and parse_state.is_space_unchecked( ) ) {
					parse_state.remove_prefix( );
					skip_comments_checked( parse_state );
				}
			}

			template<typename ParseState>
			DAW_ATTRIB_FLATINLINE static constexpr void
			trim_left_unchecked( ParseState &parse_state ) {
				skip_comments_unchecked( parse_state );
				while( parse_state.is_space_unchecked( ) ) {
					parse_state.remove_prefix( );
				}
			}

			template<typename ParseState>
			DAW_ATTRIB_FLATINLINE static constexpr void
			move_next_member_unchecked( ParseState &parse_state ) {
				parse_state.move_next_member_or_end_unchecked( );
			}

			template<char... keys, typename ParseState>
			DAW_ATTRIB_FLATINLINE static constexpr void
			move_to_next_of( ParseState &parse_state ) {
				skip_comments( parse_state );
				daw_json_assert_weak( parse_state.has_more( ),
				                      ErrorReason::UnexpectedEndOfData, parse_state );
				while( not parse_policy_details::in<keys...>( parse_state.front( ) ) ) {
					daw_json_assert_weak( parse_state.has_more( ),
					                      ErrorReason::UnexpectedEndOfData, parse_state );
					parse_state.remove_prefix( );
					skip_comments( parse_state );
				}
			}

			DAW_ATTRIB_FLATINLINE static constexpr bool is_literal_end( char c ) {
				return c == '\0' or c == ',' or c == ']' or c == '}' or c == '#';
			}

			template<char PrimLeft, char PrimRight, char SecLeft, char SecRight,
			         typename ParseState>
			DAW_ATTRIB_FLATINLINE static constexpr ParseState
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
				while( DAW_LIKELY( ptr_first < ptr_last ) ) {
					switch( *ptr_first ) {
					case '\\':
						++ptr_first;
						break;
					case '"':
						++ptr_first;
						if constexpr( traits::not_same_v<typename ParseState::exec_tag_t,
						                                 constexpr_exec_tag> ) {
							ptr_first = json_details::mem_skip_until_end_of_string<
							  ParseState::is_unchecked_input>( ParseState::exec_tag,
							                                   ptr_first, parse_state.last );
						} else {
							while( ptr_first < ptr_last and *ptr_first != '"' ) {
								if( *ptr_first == '\\' ) {
									++ptr_first;
									if( ptr_first >= ptr_last ) {
										break;
									}
								}
								++ptr_first;
							}
						}
						daw_json_assert( ptr_first < ptr_last,
						                 ErrorReason::UnexpectedEndOfData, parse_state );
						break;
					case ',':
						if( prime_bracket_count == 1 and second_bracket_count == 0 ) {
							++cnt;
						}
						break;
					case PrimLeft:
						++prime_bracket_count;
						break;
					case PrimRight:
						--prime_bracket_count;
						if( prime_bracket_count == 0 ) {
							daw_json_assert( second_bracket_count == 0,
							                 ErrorReason::InvalidBracketing, parse_state );
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
					case '/':
						++ptr_first;
						daw_json_assert( ptr_first < ptr_last,
						                 ErrorReason::UnexpectedEndOfData, parse_state );
						switch( *ptr_first ) {
						case '/':
							++ptr_first;
							while( ( ptr_last - ptr_first ) > 1 and *ptr_first != '\n' ) {
								++ptr_first;
							}
							break;
						case '*':
							++ptr_first;
							while( ( ptr_last - ptr_first ) >= 3 and *ptr_first != '*' and
							       *std::next( ptr_first ) != '/' ) {
								++ptr_first;
							}
							break;
						default:
							++ptr_first;
						}
						break;
					}
					++ptr_first;
				}
				daw_json_assert_weak( ( prime_bracket_count == 0 ) &
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
			DAW_ATTRIB_FLATINLINE static constexpr ParseState
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
						if constexpr( traits::not_same_v<typename ParseState::exec_tag_t,
						                                 constexpr_exec_tag> ) {
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
						if( prime_bracket_count == 1 and second_bracket_count == 0 ) {
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
					case '/':
						++ptr_first;
						switch( *ptr_first ) {
						case '/':
							++ptr_first;
							while( *ptr_first != '\n' ) {
								++ptr_first;
							}
							break;
						case '*':
							++ptr_first;
							while( *ptr_first != '*' and *std::next( ptr_first ) != '/' ) {
								++ptr_first;
							}
							break;
						default:
							++ptr_first;
						}
						break;
					}
					++ptr_first;
				}
				DAW_UNREACHABLE( );
			}
		};
	} // namespace DAW_JSON_VER
} // namespace daw::json
