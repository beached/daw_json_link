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
#include "namespace.h"

#include <daw/daw_hide.h>
#include <daw/daw_traits.h>

#include <ciso646>
#include <cstddef>
#include <cstdint>
#include <type_traits>

namespace DAW_JSON_NS {
	class HashCommentSkippingPolicy final {
		template<typename ParseState>
		DAW_ATTRIBUTE_FLATTEN static constexpr void
		skip_comments_unchecked( ParseState &parse_state ) {
			while( parse_state.front( ) == '#' ) {
				parse_state.remove_prefix( );
				parse_state.template move_to_next_of_unchecked<'\n'>( );
				parse_state.remove_prefix( );
			}
		}

		template<typename ParseState>
		DAW_ATTRIBUTE_FLATTEN static constexpr void
		skip_comments_checked( ParseState &parse_state ) {
			while( parse_state.has_more( ) and parse_state.front( ) == '#' ) {
				parse_state.remove_prefix( );
				parse_state.template move_to_next_of_checked<'\n'>( );
				if( parse_state.front( ) == '\n' ) {
					parse_state.remove_prefix( );
				}
			}
		}

		template<typename ParseState>
		DAW_ATTRIBUTE_FLATTEN static constexpr void
		skip_comments( ParseState &parse_state ) {
			if constexpr( ParseState::is_unchecked_input ) {
				skip_comments_unchecked( parse_state );
			} else {
				skip_comments_checked( parse_state );
			}
		}

	public:
		template<typename ParseState>
		DAW_ATTRIBUTE_FLATTEN static constexpr void
		trim_left_checked( ParseState &parse_state ) {
			skip_comments_checked( parse_state );
			while( parse_state.has_more( ) and parse_state.is_space_unchecked( ) ) {
				parse_state.remove_prefix( );
				skip_comments_checked( parse_state );
			}
		}

		template<typename ParseState>
		DAW_ATTRIBUTE_FLATTEN static constexpr void
		trim_left_unchecked( ParseState &parse_state ) {
			skip_comments_unchecked( parse_state );
			while( parse_state.is_space_unchecked( ) ) {
				parse_state.remove_prefix( );
			}
		}

		template<char... keys, typename ParseState>
		DAW_ATTRIBUTE_FLATTEN static constexpr void
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

		DAW_ATTRIBUTE_FLATTEN static constexpr bool is_literal_end( char c ) {
			return c == '\0' or c == ',' or c == ']' or c == '}' or c == '#';
		}

		template<char PrimLeft, char PrimRight, char SecLeft, char SecRight,
		         typename ParseState>
		DAW_ATTRIBUTE_FLATTEN static constexpr ParseState
		skip_bracketed_item_checked( ParseState &parse_state ) {
			// Not checking for Left as it is required to be skipped already
			auto result = parse_state;
			std::size_t cnt = 0;
			std::uint32_t prime_bracket_count = 1;
			std::uint32_t second_bracket_count = 0;
			char const *ptr_first = parse_state.first;
			char const *const ptr_last = parse_state.last;
			if( DAW_JSON_UNLIKELY( ptr_first >= ptr_last ) ) {
				return result;
			}
			if( *ptr_first == PrimLeft ) {
				++ptr_first;
			}
			while( DAW_JSON_LIKELY( ptr_first < ptr_last ) ) {
				// TODO: use if/else if or put switch into IILE
				switch( *ptr_first ) {
				case '\\':
					++ptr_first;
					break;
				case '"':
					++ptr_first;
					if constexpr( traits::not_same_v<typename ParseState::exec_tag_t,
					                                 constexpr_exec_tag> ) {
						ptr_first = json_details::mem_skip_until_end_of_string<
						  ParseState::is_unchecked_input>( ParseState::exec_tag, ptr_first,
						                                   parse_state.last );
					} else {
						while( DAW_JSON_LIKELY( ptr_first < ptr_last ) and
						       *ptr_first != '"' ) {
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
				case '#':
					++ptr_first;
					while( ptr_first < ptr_last and *ptr_first != '\n' ) {
						++ptr_first;
					}
					if( ptr_first < ptr_last ) {
						continue;
					}
					break;
				}
				++ptr_first;
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
		DAW_ATTRIBUTE_FLATTEN static constexpr ParseState
		skip_bracketed_item_unchecked( ParseState &parse_state ) {
			// Not checking for Left as it is required to be skipped already
			auto result = parse_state;
			std::size_t cnt = 0;
			std::uint32_t prime_bracket_count = 1;
			std::uint32_t second_bracket_count = 0;
			char const *ptr_first = parse_state.first;
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
						  ParseState::is_unchecked_input>( ParseState::exec_tag, ptr_first,
						                                   parse_state.last );
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
				case '#':
					++ptr_first;
					while( *ptr_first != '\n' ) {
						++ptr_first;
					}
					break;
				}
				++ptr_first;
			}
			DAW_UNREACHABLE( );
		}
	};
} // namespace DAW_JSON_NS
