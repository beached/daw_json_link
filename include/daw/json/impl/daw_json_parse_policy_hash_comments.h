// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/daw_json_link
//

#pragma once

#include "../daw_json_assert.h"
#include "daw_json_parse_common.h"
#include "daw_json_parse_policy_policy_details.h"
#include "daw_not_const_ex_functions.h"

#include <daw/daw_hide.h>

#include <ciso646>
#include <cstddef>
#include <cstdint>
#include <type_traits>

namespace daw::json {
	class HashCommentSkippingPolicy final {
		template<typename Range>
		DAW_ATTRIBUTE_FLATTEN static constexpr void
		skip_comments_unchecked( Range &rng ) {
			while( rng.front( ) == '#' ) {
				rng.remove_prefix( );
				rng.template move_to_next_of_unchecked<'\n'>( );
				rng.remove_prefix( );
			}
		}

		template<typename Range>
		DAW_ATTRIBUTE_FLATTEN static constexpr void
		skip_comments_checked( Range &rng ) {
			while( rng.has_more( ) and rng.front( ) == '#' ) {
				rng.remove_prefix( );
				rng.template move_to_next_of_checked<'\n'>( );
				if( rng.front( ) == '\n' ) {
					rng.remove_prefix( );
				}
			}
		}

		template<typename Range>
		DAW_ATTRIBUTE_FLATTEN static constexpr void skip_comments( Range &rng ) {
			if constexpr( Range::is_unchecked_input ) {
				skip_comments_unchecked( rng );
			} else {
				skip_comments_checked( rng );
			}
		}

	public:
		template<typename Range>
		DAW_ATTRIBUTE_FLATTEN static constexpr void
		trim_left_checked( Range &rng ) {
			skip_comments_checked( rng );
			while( rng.has_more( ) and rng.is_space_unchecked( ) ) {
				rng.remove_prefix( );
				skip_comments_checked( rng );
			}
		}

		template<typename Range>
		DAW_ATTRIBUTE_FLATTEN static constexpr void
		trim_left_unchecked( Range &rng ) {
			skip_comments_unchecked( rng );
			while( rng.is_space_unchecked( ) ) {
				rng.remove_prefix( );
			}
		}

		template<char... keys, typename Range>
		DAW_ATTRIBUTE_FLATTEN static constexpr void move_to_next_of( Range &rng ) {
			skip_comments( rng );

			daw_json_assert_weak( rng.has_more( ), "Unexpected end of data", rng );
			while( not parse_policy_details::in<keys...>( rng.front( ) ) ) {
				daw_json_assert_weak( rng.has_more( ), "Unexpected end of data", rng );
				rng.remove_prefix( );
				skip_comments( rng );
			}
		}

		DAW_ATTRIBUTE_FLATTEN static constexpr bool is_literal_end( char c ) {
			return c == '\0' or c == ',' or c == ']' or c == '}' or c == '#';
		}

		template<char PrimLeft, char PrimRight, char SecLeft, char SecRight,
		         typename Range>
		DAW_ATTRIBUTE_FLATTEN static constexpr Range
		skip_bracketed_item_checked( Range &rng ) {
			// Not checking for Left as it is required to be skipped already
			auto result = rng;
			std::size_t cnt = 0;
			std::uint32_t prime_bracket_count = 1;
			std::uint32_t second_bracket_count = 0;
			char const *ptr_first = rng.first;
			char const *const ptr_last = rng.last;
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
					if constexpr( not std::is_same_v<typename Range::exec_tag_t,
					                                 constexpr_exec_tag> ) {
						ptr_first = json_details::mem_skip_until_end_of_string<
						  Range::is_unchecked_input>( Range::exec_tag, ptr_first,
						                              rng.last );
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
					daw_json_assert( ptr_first < ptr_last, "Unexpected end of stream",
					                 rng );
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
						daw_json_assert( second_bracket_count == 0, "Unexpected bracketing",
						                 rng );
						++ptr_first;
						// We include the close primary bracket in the range so that
						// subsequent parsers have a terminator inside their range
						result.last = ptr_first;
						result.counter = cnt;
						rng.first = ptr_first;
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
			daw_json_assert( prime_bracket_count == 0 and second_bracket_count == 0,
			                 "Unexpected bracketing", rng );
			// We include the close primary bracket in the range so that subsequent
			// parsers have a terminator inside their range
			result.last = ptr_first;
			result.counter = cnt;
			rng.first = ptr_first;
			return result;
		}

		template<char PrimLeft, char PrimRight, char SecLeft, char SecRight,
		         typename Range>
		DAW_ATTRIBUTE_FLATTEN static constexpr Range
		skip_bracketed_item_unchecked( Range &rng ) {
			// Not checking for Left as it is required to be skipped already
			auto result = rng;
			std::size_t cnt = 0;
			std::uint32_t prime_bracket_count = 1;
			std::uint32_t second_bracket_count = 0;
			char const *ptr_first = rng.first;
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
					if constexpr( not std::is_same_v<typename Range::exec_tag_t,
					                                 constexpr_exec_tag> ) {
						ptr_first = json_details::mem_skip_until_end_of_string<
						  Range::is_unchecked_input>( Range::exec_tag, ptr_first,
						                              rng.last );
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
						rng.first = ptr_first;
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
			DAW_JSON_UNREACHABLE( );
		}
	};
} // namespace daw::json
