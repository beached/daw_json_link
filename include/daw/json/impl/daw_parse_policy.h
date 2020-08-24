// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/daw_json_link
//

#pragma once

#include "daw_exec_modes.h"
#include "daw_json_assert.h"
#include "daw_json_parse_common.h"
#include "daw_parse_policy_cpp_comments.h"
#include "daw_parse_policy_error.h"
#include "daw_parse_policy_hash_comments.h"
#include "daw_parse_policy_no_comments.h"
#include "daw_parse_policy_policy_details.h"

#include <daw/cpp_17.h>
#include <daw/daw_hide.h>

#include <cassert>
#include <cstddef>
#include <cstdint>
#include <iterator>
#include <type_traits>

namespace daw::json {
	template<bool IsUncheckedInput, typename CommentPolicy, typename ExecTag,
	         bool AllowEscapedNames>
	struct BasicParsePolicy {
		using iterator = char const *;
		static constexpr bool is_unchecked_input = IsUncheckedInput;
		static_assert( std::is_base_of_v<constexpr_exec_tag, ExecTag>,
		               "Unexpected exec tag" );
		using exec_tag_t = ExecTag;
		static constexpr ExecTag exec_tag = ExecTag{ };
		static constexpr bool allow_escaped_names = AllowEscapedNames;
		static constexpr bool force_name_equal_check = false;
		using CharT = char;

		using as_unchecked =
		  BasicParsePolicy<true, CommentPolicy, ExecTag, allow_escaped_names>;
		using as_checked =
		  BasicParsePolicy<false, CommentPolicy, ExecTag, allow_escaped_names>;

		iterator first{ };
		iterator last{ };
		iterator class_first{ };
		iterator class_last{ };
		std::size_t counter = 0;
		using Range = BasicParsePolicy;

		template<std::size_t N>
		constexpr bool operator==( char const ( &rhs )[N] ) const {
			if( size( ) < ( N - 1 ) ) {
				return false;
			}
			bool result = true;
			for( std::size_t n = 0; n < ( N - 1 ); ++n ) {
				result = result and ( first[n] == rhs[n] );
			}
			return result;
		}

		constexpr BasicParsePolicy( ) = default;

		constexpr BasicParsePolicy( iterator f, iterator l )
		  : first( f )
		  , last( l )
		  , class_first( f )
		  , class_last( l ) {

			assert( f );
			assert( l );
			daw_json_assert( not is_null( ), "Unexpceted null start of range" );
		}

		[[nodiscard]] DAW_ATTRIBUTE_FLATTEN constexpr bool empty( ) const {
			if constexpr( is_unchecked_input ) {
				return first == last;
			} else {
				return first != nullptr and first == last;
			}
		}

		[[nodiscard]] DAW_ATTRIBUTE_FLATTEN constexpr bool has_more( ) const {
			return first < last;
		}

		template<char c>
		DAW_ATTRIBUTE_FLATTEN constexpr void move_to_next_of_unchecked( ) {
			first = json_details::mem_chr<true>( exec_tag, first, last, c );
		}

		template<char c>
		DAW_ATTRIBUTE_FLATTEN constexpr void move_to_next_of_checked( ) {
			first = json_details::mem_chr<false>( exec_tag, first, last, c );
		}

		[[nodiscard]] DAW_ATTRIBUTE_FLATTEN constexpr char front( ) const {
			return *first;
		}

		[[nodiscard]] DAW_ATTRIBUTE_FLATTEN constexpr std::size_t size( ) const {
			return static_cast<std::size_t>( std::distance( first, last ) );
		}

		[[nodiscard]] DAW_ATTRIBUTE_FLATTEN constexpr bool is_number( ) const {
			return static_cast<unsigned>( front( ) ) - static_cast<unsigned>( '0' ) <
			       10U;
		}

		[[nodiscard]] DAW_ATTRIBUTE_FLATTEN constexpr bool is_null( ) const {
			return first == nullptr;
		}

		DAW_ATTRIBUTE_FLATTEN constexpr void remove_prefix( ) {
			++first;
		}

		DAW_ATTRIBUTE_FLATTEN constexpr void remove_prefix( std::size_t n ) {
			first += static_cast<intmax_t>( n );
		}

		[[nodiscard]] DAW_ATTRIBUTE_FLATTEN constexpr iterator begin( ) const {
			return first;
		}

		[[nodiscard]] DAW_ATTRIBUTE_FLATTEN constexpr iterator data( ) const {
			return first;
		}

		[[nodiscard]] DAW_ATTRIBUTE_FLATTEN constexpr iterator end( ) const {
			return last;
		}

		[[nodiscard]] DAW_ATTRIBUTE_FLATTEN explicit constexpr
		operator bool( ) const {
			return not empty( );
		}

		DAW_ATTRIBUTE_FLATTEN constexpr void set_class_position( ) {
			class_first = first;
			class_last = last;
		}

		DAW_ATTRIBUTE_FLATTEN constexpr void move_to_end_of_literal( ) {
			char const *f = first;
			char const *const l = last;
			if constexpr( IsUncheckedInput ) {
				while( ( *f > 0x20 ) and not is_literal_end( *f ) ) {
					++f;
				}
			} else {
				while( ( f < l ) and ( *f > 0x20 ) and not is_literal_end( *f ) ) {
					++f;
				}
			}
			first = f;
		}

		DAW_ATTRIBUTE_FLATTEN constexpr void trim_left_checked( ) {
			if constexpr( CommentPolicy::has_alternate_parse_tokens ) {
				first =
				  CommentPolicy::skip_alternate_token_checked( exec_tag, first, last );
				while( first < last and is_space_unchecked( ) ) {
					++first;
					first = CommentPolicy::skip_alternate_token_checked( exec_tag, first,
					                                                     last );
				}
			} else {
				// SIMD here was much slower
				while( first < last and
				       parse_policy_details::is_space_unchecked( *first ) ) {
					++first;
				}
			}
		}

		DAW_ATTRIBUTE_FLATTEN constexpr void trim_left_unchecked( ) {
			if constexpr( CommentPolicy::has_alternate_parse_tokens ) {
				first = CommentPolicy::skip_alternate_token_unchecked( exec_tag, first,
				                                                       last );
				while( is_space_unchecked( ) ) {
					++first;
					first = CommentPolicy::skip_alternate_token_unchecked( exec_tag,
					                                                       first, last );
				}
			} else {
				while( parse_policy_details::is_space_unchecked( *first ) ) {
					++first;
				}
			}
		}

		template<char... keys>
		DAW_ATTRIBUTE_FLATTEN constexpr void move_to_next_of( ) {
			static_assert( sizeof...( keys ) <= 16 );
			if constexpr( CommentPolicy::has_alternate_parse_tokens ) {
				first =
				  CommentPolicy::template skip_alternate_token<is_unchecked_input>(
				    exec_tag, first, last );
				while( not parse_policy_details::in<keys...>( *first ) ) {
					daw_json_assert_weak( first < last, "Unexpected end of data" );
					++first;
					first =
					  CommentPolicy::template skip_alternate_token<is_unchecked_input>(
					    exec_tag, first, last );
				}
			} else {
				first = json_details::mem_move_to_next_of<Range::is_unchecked_input,
				                                          sizeof...( keys ), keys...>(
				  Range::exec_tag, first, last );
			}
		}

		[[nodiscard]] DAW_ATTRIBUTE_FLATTEN static constexpr bool
		is_literal_end( char c ) {
			if constexpr( CommentPolicy::has_alternate_parse_tokens ) {
				return c == '\0' or c == ',' or c == ']' or c == '}' or
				       CommentPolicy::is_alternate_token( c );
			} else {
				return c == '\0' or c == ',' or c == ']' or c == '}';
			}
		}

		[[nodiscard]] DAW_ATTRIBUTE_FLATTEN constexpr bool
		is_space_checked( ) const {
			daw_json_assert_weak( has_more( ), "Unexpected end" );
			return *first <= 0x20;
		}

		[[nodiscard]] DAW_ATTRIBUTE_FLATTEN constexpr bool
		is_space_unchecked( ) const {
			return *first <= 0x20;
		}

		[[nodiscard]] DAW_ATTRIBUTE_FLATTEN constexpr bool
		is_opening_bracket_checked( ) const {
			return first < last and *first == '[';
		}

		[[nodiscard]] DAW_ATTRIBUTE_FLATTEN constexpr bool
		is_opening_bracket_unchecked( ) const {
			return *first == '[';
		}

		[[nodiscard]] DAW_ATTRIBUTE_FLATTEN constexpr bool
		is_opening_brace_checked( ) const {
			return first < last and *first == '{';
		}

		[[nodiscard]] DAW_ATTRIBUTE_FLATTEN constexpr bool
		is_closing_brace_checked( ) const {
			return first < last and *first == '}';
		}

		[[nodiscard]] DAW_ATTRIBUTE_FLATTEN constexpr bool
		is_closing_brace_unchecked( ) const {
			return *first == '}';
		}

		[[nodiscard]] DAW_ATTRIBUTE_FLATTEN constexpr bool
		is_array_end_checked( ) const {
			return first < last and *first == ']';
		}

		[[nodiscard]] DAW_ATTRIBUTE_FLATTEN constexpr bool
		is_array_end_unchecked( ) const {
			return *first == ']';
		}

		[[nodiscard]] DAW_ATTRIBUTE_FLATTEN constexpr bool
		is_quotes_unchecked( ) const {
			return *first == '"';
		}

		[[nodiscard]] DAW_ATTRIBUTE_FLATTEN constexpr bool
		is_quotes_checked( ) const {
			return first < last and *first == '"';
		}

		[[nodiscard]] DAW_ATTRIBUTE_FLATTEN constexpr bool
		is_comma_checked( ) const {
			return first < last and *first == ',';
		}

		[[nodiscard]] DAW_ATTRIBUTE_FLATTEN constexpr bool
		is_colon_unchecked( ) const {
			return *first == ':';
		}

		[[nodiscard]] DAW_ATTRIBUTE_FLATTEN constexpr bool
		is_minus_unchecked( ) const {
			return *first == '-';
		}

		[[nodiscard]] DAW_ATTRIBUTE_FLATTEN constexpr bool
		is_plus_unchecked( ) const {
			return *first == '+';
		}

		[[nodiscard]] DAW_ATTRIBUTE_FLATTEN constexpr bool is_n_unchecked( ) const {
			return *first == 'n';
		}

		[[nodiscard]] DAW_ATTRIBUTE_FLATTEN constexpr bool
		is_exponent_checked( ) const {
			return first < last and ( ( *first == 'e' ) bitor ( *first == 'E' ) );
		}

		[[nodiscard]] DAW_ATTRIBUTE_FLATTEN constexpr bool is_t_unchecked( ) const {
			return *first == 't';
		}

		[[nodiscard]] DAW_ATTRIBUTE_FLATTEN constexpr bool is_u_unchecked( ) const {
			return *first == 'u';
		}

		[[nodiscard]] DAW_ATTRIBUTE_FLATTEN constexpr bool
		is_escape_unchecked( ) const {
			return *first == '\\';
		}

		DAW_ATTRIBUTE_FLATTEN constexpr void trim_left( ) {
			if constexpr( is_unchecked_input ) {
				trim_left_unchecked( );
			} else {
				trim_left_checked( );
			}
		}

		DAW_ATTRIBUTE_FLATTEN constexpr void clean_tail_unchecked( ) {
			trim_left_unchecked( );
			if( *first == ',' ) {
				++first;
				trim_left_unchecked( );
			}
		}

		DAW_ATTRIBUTE_FLATTEN constexpr void clean_tail_checked( ) {
			trim_left_checked( );
			if( first < last and *first == ',' ) {
				++first;
				trim_left_checked( );
			}
		}

		DAW_ATTRIBUTE_FLATTEN constexpr void clean_tail( ) {
			if constexpr( is_unchecked_input ) {
				clean_tail_unchecked( );
			} else {
				clean_tail_checked( );
			}
		}

		DAW_ATTRIBUTE_FLATTEN constexpr void move_to_next_class_member( ) {
			move_to_next_of<'"', '}'>( );
		}

		DAW_ATTRIBUTE_FLATTEN constexpr bool is_at_next_class_member( ) const {
			return parse_policy_details::in<'"', '}'>( *first );
		}

		DAW_ATTRIBUTE_FLATTEN constexpr bool is_at_token_after_value( ) const {
			return parse_policy_details::in<',', '}', ']'>( *first );
		}

		template<char PrimLeft, char PrimRight, char SecLeft, char SecRight>
		constexpr BasicParsePolicy
		skip_bracketed_item_checked( ) {
			auto result = *this;
			std::size_t cnt = 0;
			std::uint32_t prime_bracket_count = 1;
			std::uint32_t second_bracket_count = 0;
			char const *ptr_first = first;
			char const *const ptr_last = last;
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
					  exec_tag, ptr_first, last );
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
					if constexpr( CommentPolicy::has_alternate_parse_tokens ) {
						ptr_first = CommentPolicy::skip_alternate_token_checked(
						  exec_tag, ptr_first, last );
					}
					return true;
				}
			}( ) ) {
				++ptr_first;
			}
			daw_json_assert( first <= last, "Unexpected end of stream" );
			daw_json_assert( ( prime_bracket_count == 0 ) bitand
			                        ( second_bracket_count == 0 ),
			                      "Unexpected bracketing" );
			// We include the close primary bracket in the range so that subsequent
			// parsers have a terminator inside their range
			result.last = ptr_first;
			result.counter = cnt;
			first = ptr_first;
			return result;
		}

		template<char PrimLeft, char PrimRight, char SecLeft, char SecRight>
		constexpr BasicParsePolicy
		skip_bracketed_item_unchecked( ) {
			auto result = *this;
			std::size_t cnt = 0;
			std::uint32_t prime_bracket_count = 1;
			std::uint32_t second_bracket_count = 0;
			char const *ptr_first = first;
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
					  exec_tag, ptr_first, last );
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
					if constexpr( CommentPolicy::has_alternate_parse_tokens ) {
						ptr_first = CommentPolicy::skip_alternate_token_unchecked(
						  exec_tag, ptr_first, last );
					}
					return true;
				}
			}( ) ) {
				++ptr_first;
			}
			// We include the close primary bracket in the range so that subsequent
			// parsers have a terminator inside their range
			result.last = ptr_first;
			result.counter = cnt;
			first = ptr_first;
			return result;
		}

		[[nodiscard]] DAW_ATTRIBUTE_FLATTEN inline constexpr BasicParsePolicy
		skip_class( ) {
			if constexpr( is_unchecked_input ) {
				return skip_bracketed_item_unchecked<'{', '}', '[', ']'>( );
			} else {
				return skip_bracketed_item_checked<'{', '}', '[', ']'>( );
			}
		}

		[[nodiscard]] DAW_ATTRIBUTE_FLATTEN inline constexpr BasicParsePolicy
		skip_array( ) {
			if constexpr( is_unchecked_input ) {
				return skip_bracketed_item_unchecked<'[', ']', '{', '}'>( );
			} else {
				return skip_bracketed_item_checked<'[', ']', '{', '}'>( );
			}
		}
	};

	using NoCommentSkippingPolicyChecked =
	  BasicParsePolicy<false, NoCommentSkippingPolicy, constexpr_exec_tag, false>;

	using NoCommentSkippingPolicyUnchecked =
	  BasicParsePolicy<true, NoCommentSkippingPolicy, constexpr_exec_tag, false>;

	template<typename ExecTag, bool AllowEscapedNames = false>
	using SIMDNoCommentSkippingPolicyChecked =
	  BasicParsePolicy<false, NoCommentSkippingPolicy, ExecTag,
	                   AllowEscapedNames>;

	template<typename ExecTag, bool AllowEscapedNames = false>
	using SIMDNoCommentSkippingPolicyUnchecked =
	  BasicParsePolicy<true, NoCommentSkippingPolicy, ExecTag, AllowEscapedNames>;

	using HashCommentSkippingPolicyChecked =
	  BasicParsePolicy<false, HashCommentSkippingPolicy, constexpr_exec_tag,
	                   false>;

	using HashCommentSkippingPolicyUnchecked =
	  BasicParsePolicy<true, HashCommentSkippingPolicy, constexpr_exec_tag,
	                   false>;

	template<typename ExecTag, bool AllowEscapedNames = false>
	using SIMDHashCommentSkippingPolicyChecked =
	  BasicParsePolicy<false, HashCommentSkippingPolicy, ExecTag,
	                   AllowEscapedNames>;

	template<typename ExecTag, bool AllowEscapedNames = false>
	using SIMDHashCommentSkippingPolicyUnchecked =
	  BasicParsePolicy<true, HashCommentSkippingPolicy, ExecTag,
	                   AllowEscapedNames>;

	using CppCommentSkippingPolicyChecked =
	  BasicParsePolicy<false, CppCommentSkippingPolicy, constexpr_exec_tag,
	                   false>;

	using CppCommentSkippingPolicyUnchecked =
	  BasicParsePolicy<true, CppCommentSkippingPolicy, constexpr_exec_tag, false>;

	template<typename ExecTag, bool AllowEscapedNames = false>
	using SIMDCppCommentSkippingPolicyChecked =
	  BasicParsePolicy<false, CppCommentSkippingPolicy, ExecTag,
	                   AllowEscapedNames>;

	template<typename ExecTag, bool AllowEscapedNames = false>
	using SIMDCppCommentSkippingPolicyUnchecked =
	  BasicParsePolicy<true, CppCommentSkippingPolicy, ExecTag,
	                   AllowEscapedNames>;
} // namespace daw::json
