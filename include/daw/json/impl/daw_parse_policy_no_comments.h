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
#include "daw_mem_functions.h"
#include "daw_parse_policy_policy_details.h"

#include <daw/daw_function_table.h>
#include <daw/daw_hide.h>

namespace daw::json {
	struct NoCommentSkippingPolicy final {
		template<typename Range>
		DAW_ATTRIBUTE_FLATTEN static constexpr void
		trim_left_checked( Range &rng ) {
			// SIMD here was much slower
			char const *first = rng.first;
			char const *const last = rng.last;
			while( first < last and
			       parse_policy_details::is_space_unchecked( *first ) ) {
			  ++first;
			}
			rng.first = first;
		}

		template<typename Range>
		DAW_ATTRIBUTE_FLATTEN static constexpr void
		trim_left_unchecked( Range &rng ) {
			char const *first = rng.first;
			while( parse_policy_details::is_space_unchecked( *first ) ) {
				++first;
			}
			rng.first = first;
		}

		template<char... keys, typename Range>
		DAW_ATTRIBUTE_FLATTEN static constexpr void move_to_next_of( Range &rng ) {
			static_assert( sizeof...( keys ) <= 16 );
			rng.first = json_details::mem_move_to_next_of<Range::is_unchecked_input,
			                                              sizeof...( keys ), keys...>(
			  Range::exec_tag, rng.first, rng.last );
		}

		DAW_ATTRIBUTE_FLATTEN static constexpr bool is_literal_end( char c ) {
			return c == '\0' or c == ',' or c == ']' or c == '}';
		}

		template<char PrimLeft, char PrimRight, char SecLeft, char SecRight,
		         typename Range>
		DAW_ATTRIBUTE_FLATTEN static constexpr Range
		skip_bracketed_item_checked( Range &rng ) {
			auto result = rng;
			std::size_t cnt = 0;
			std::uint32_t prime_bracket_count = 1;
			std::uint32_t second_bracket_count = 0;
			char const *ptr_first = rng.first;
			char const *const ptr_last = rng.last;
			if( ptr_first < ptr_last and *ptr_first == PrimLeft ) {
				++ptr_first;
			}
			while( ptr_first < ptr_last and [&] {
				switch( *ptr_first ) {
				case '\\':
					++ptr_first;
					return ptr_first < ptr_last;
				case '"':
					++ptr_first;
					ptr_first = json_details::mem_skip_until_end_of_string<false>(
					  Range::exec_tag, ptr_first, rng.last );
					daw_json_assert( ptr_first < ptr_last, "Unexpected end of stream" );
					return true;
				case ',':
					if( ( prime_bracket_count == 1 ) bitand
					    ( second_bracket_count == 0 ) ) {
						++cnt;
					}
					return true;
				case PrimLeft:
					++prime_bracket_count;
					return true;
				case PrimRight:
					--prime_bracket_count;
					if( prime_bracket_count == 0 ) {
						++ptr_first;
						return false;
					}
					return true;
				case SecLeft:
					++second_bracket_count;
					return true;
				case SecRight:
					--second_bracket_count;
					return true;
				default:
					return true;
				}
			}( ) ) {
				++ptr_first;
			}
			daw_json_assert_weak( ( prime_bracket_count == 0 ) bitand
			                        ( second_bracket_count == 0 ),
			                      "Unexpected bracketing" );
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
			auto result = rng;
			std::size_t cnt = 0;
			std::uint32_t prime_bracket_count = 1;
			std::uint32_t second_bracket_count = 0;
			char const *ptr_first = rng.first;
			if( *ptr_first == PrimLeft ) {
				++ptr_first;
			}
			while( [&] {
				switch( *ptr_first ) {
				case '\\':
					++ptr_first;
					return true;
				case '"':
					++ptr_first;
					ptr_first = json_details::mem_skip_until_end_of_string<true>(
					  Range::exec_tag, ptr_first, rng.last );
					return true;
				case ',':
					if( ( prime_bracket_count == 1 ) bitand
					    ( second_bracket_count == 0 ) ) {
						++cnt;
					}
					return true;
				case PrimLeft:
					++prime_bracket_count;
					return true;
				case PrimRight:
					--prime_bracket_count;
					if( prime_bracket_count == 0 ) {
						++ptr_first;
						return false;
					}
					return true;
				case SecLeft:
					++second_bracket_count;
					return true;
				case SecRight:
					--second_bracket_count;
					return true;
				default:
					return true;
				}
			}( ) ) {
				++ptr_first;
			}
			// We include the close primary bracket in the range so that subsequent
			// parsers have a terminator inside their range
			result.last = ptr_first;
			result.counter = cnt;
			rng.first = ptr_first;
			return result;
		}
	};
} // namespace daw::json
