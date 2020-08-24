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

#include <daw/daw_hide.h>

namespace daw::json {
	namespace json_details {
		template<typename ExecTag>
		DAW_ATTRIBUTE_FLATTEN static inline constexpr char const *
		skip_cpp_comments_checked( ExecTag const &tag, char const *first,
		                           char const *last ) {
			if( first < last and *first == '/' ) {
				++first;
				daw_json_assert( first < last, "Unexpected end of stream" );
				switch( *first ) {
				case '/':
					++first;
					first =
					  json_details::mem_move_to_next_of<false, '\n'>( tag, first, last );
					break;
				case '*':
					++first;
					first =
					  json_details::mem_find_substr<false, '*', '/'>( tag, first, last );
					break;
				default:
					daw_json_error( "Unexpected character in stream" );
				}
				++first;
			}
			return first;
		}

		template<typename ExecTag>
		DAW_ATTRIBUTE_FLATTEN static inline constexpr char const *
		skip_cpp_comments_unchecked( ExecTag const &tag, char const *first,
		                             char const *last ) {
			if( *first == '/' ) {
				++first;
				switch( *first ) {
				case '/':
					++first;
					first =
					  json_details::mem_move_to_next_of<true, '\n'>( tag, first, last );
					break;
				case '*':
					++first;
					first =
					  json_details::mem_find_substr<true, '*', '/'>( tag, first, last );
					break;
				default:
					daw_json_error( "Unexpected character in stream" );
				}
				++first;
			}
			return first;
		}

		template<bool is_unchecked_input, typename ExecTag>
		DAW_ATTRIBUTE_FLATTEN static inline constexpr char const *
		skip_cpp_comments( ExecTag const &tag, char const *first,
		                   char const *last ) {
			if constexpr( is_unchecked_input ) {
				return skip_cpp_comments_unchecked( tag, first, last );
			} else {
				return skip_cpp_comments_checked( tag, first, last );
			}
		}
	} // namespace json_details

	struct CppCommentSkippingPolicy {
		template<typename Range>
		DAW_ATTRIBUTE_FLATTEN static constexpr void
		trim_left_checked( Range &rng ) {
			rng.first = json_details::skip_cpp_comments_checked(
			  Range::exec_tag, rng.first, rng.last );
			while( rng.has_more( ) and rng.is_space_unchecked( ) ) {
				rng.remove_prefix( );
				rng.first = json_details::skip_cpp_comments_checked(
				  Range::exec_tag, rng.first, rng.last );
			}
		}

		template<typename Range>
		DAW_ATTRIBUTE_FLATTEN static constexpr void
		trim_left_unchecked( Range &rng ) {
			rng.first = json_details::skip_cpp_comments_unchecked(
			  Range::exec_tag, rng.first, rng.last );
			while( rng.is_space_unchecked( ) ) {
				rng.remove_prefix( );
				rng.first = json_details::skip_cpp_comments_unchecked(
				  Range::exec_tag, rng.first, rng.last );
			}
		}

		template<char... keys, typename Range>
		DAW_ATTRIBUTE_FLATTEN static constexpr void move_to_next_of( Range &rng ) {
			rng.first = json_details::skip_cpp_comments<Range::is_unchecked_input>(
			  Range::exec_tag, rng.first, rng.last );
			daw_json_assert_weak( rng.has_more( ), "Unexpected end of data" );
			while( not parse_policy_details::in<keys...>( rng.front( ) ) ) {
				daw_json_assert_weak( rng.has_more( ), "Unexpected end of data" );
				rng.remove_prefix( );
				rng.first = json_details::skip_cpp_comments<Range::is_unchecked_input>(
				  Range::exec_tag, rng.first, rng.last );
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
			if( ptr_first < ptr_last and rng.front( ) == PrimLeft ) {
				++ptr_first;
			}
			while( ptr_first < ptr_last and [&] {
				switch( *ptr_first ) {
				case '\\':
					++ptr_first;
					return true;
				case '"':
					++ptr_first;
					ptr_first = json_details::mem_skip_until_end_of_string<false>(
					  Range::exec_tag, ptr_first, rng.last );
					daw_json_assert( ptr_first < ptr_last, "Unexpected end of stream" );
					return true;
				case ',':
					if( prime_bracket_count == 1 and second_bracket_count == 0 ) {
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
				case '/':
					ptr_first = json_details::skip_cpp_comments_checked(
					  Range::exec_tag, ptr_first, ptr_last );
					return true;
				default:
					return true;
				}
			}( ) ) {
				++ptr_first;
			}
			daw_json_assert_weak( prime_bracket_count == 0 and
			                        second_bracket_count == 0,
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
			// Not checking for Left as it is required to be skipped already
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
					if( prime_bracket_count == 1 and second_bracket_count == 0 ) {
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
				case '/':
					ptr_first = json_details::skip_cpp_comments_unchecked(
					  Range::exec_tag, ptr_first, rng.last );
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
	}; // namespace daw::json
} // namespace daw::json
