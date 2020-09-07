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
#include "daw_json_parse_policy_cpp_comments.h"
#include "daw_json_parse_policy_error.h"
#include "daw_json_parse_policy_hash_comments.h"
#include "daw_json_parse_policy_no_comments.h"
#include "daw_json_parse_policy_policy_details.h"

#include <daw/cpp_17.h>
#include <daw/daw_hide.h>

#include <cassert>
#include <cstddef>
#include <iterator>
#include <type_traits>

namespace daw::json {
	template<bool IsUncheckedInput, typename CommentPolicy, typename ExecMode,
	         bool AllowEscapedNames>
	struct BasicParsePolicy final {
		using iterator = char const *;
		static constexpr bool is_unchecked_input = IsUncheckedInput;
		using exec_tag_t = ExecMode;
		static constexpr exec_tag_t exec_tag = exec_tag_t{ };
		static constexpr bool allow_escaped_names = AllowEscapedNames;
		static constexpr bool force_name_equal_check = false;
		using CharT = char;

		using as_unchecked =
		  BasicParsePolicy<true, CommentPolicy, exec_tag_t, allow_escaped_names>;
		using as_checked =
		  BasicParsePolicy<false, CommentPolicy, exec_tag_t, allow_escaped_names>;

		iterator first{ };
		iterator last{ };
		iterator class_first{ };
		iterator class_last{ };
		std::size_t counter = 0;
		using Range = BasicParsePolicy;

		constexpr BasicParsePolicy( ) = default;

		constexpr BasicParsePolicy( iterator f, iterator l )
		  : first( f )
		  , last( l )
		  , class_first( f )
		  , class_last( l ) {}

		[[nodiscard]] DAW_ATTRIBUTE_FLATTEN constexpr iterator begin( ) const {
			return first;
		}

		[[nodiscard]] DAW_ATTRIBUTE_FLATTEN constexpr iterator end( ) const {
			return last;
		}

		[[nodiscard]] DAW_ATTRIBUTE_FLATTEN constexpr bool empty( ) const {
			return first >= last;
		}

		[[nodiscard]] DAW_ATTRIBUTE_FLATTEN constexpr bool has_more( ) const {
			return first < last;
		}

		template<std::size_t N>
		constexpr bool starts_with( char const ( &rhs )[N] ) const {
			if( size( ) < ( N - 1 ) ) {
				return false;
			}
			bool result = true;
			for( std::size_t n = 0; n < ( N - 1 ); ++n ) {
				result = result & ( first[n] == rhs[n] );
			}
			return result;
		}

		template<char c>
		DAW_ATTRIBUTE_FLATTEN constexpr void move_to_next_of_unchecked( ) {

			if constexpr( not std::is_same_v<Range::exec_tag_t,
			                                 constexpr_exec_tag> ) {
				first = reinterpret_cast<char const *>( std::memchr(
				  first, c, static_cast<std::size_t>( class_last - first ) ) );
			} else {
				while( *first != c ) {
					++first;
				}
			}
		}

		template<char c>
		DAW_ATTRIBUTE_FLATTEN constexpr void move_to_next_of_checked( ) {

			if constexpr( not std::is_same_v<Range::exec_tag_t,
			                                 constexpr_exec_tag> ) {
				first = reinterpret_cast<char const *>( std::memchr(
				  first, c, static_cast<std::size_t>( class_last - first ) ) );
				daw_json_assert( first != nullptr, "Expected token missing" );
			} else {
				while( DAW_JSON_LIKELY( first < last ) and *first != c ) {
					++first;
				}
			}
		}

		template<char c>
		DAW_ATTRIBUTE_FLATTEN constexpr void move_to_next_of( ) {
			if( is_unchecked_input ) {
				move_to_next_of_unchecked<c>( );
			} else {
				move_to_next_of_checked<c>( );
			}
		}

		[[nodiscard]] DAW_ATTRIBUTE_FLATTEN constexpr char front( ) const {
			return *first;
		}

		[[nodiscard]] constexpr std::size_t size( ) const {
			return static_cast<std::size_t>( last - first );
		}

		[[nodiscard]] constexpr bool is_null( ) const {
			return first == nullptr;
		}

		DAW_ATTRIBUTE_FLATTEN constexpr void remove_prefix( ) {
			++first;
		}

		DAW_ATTRIBUTE_FLATTEN constexpr void remove_prefix( std::size_t n ) {
			first += static_cast<std::ptrdiff_t>( n );
		}

		constexpr void set_class_position( ) {
			class_first = first;
			class_last = last;
		}

		constexpr void trim_left_checked( ) {
			return CommentPolicy::trim_left_checked( *this );
		}

		constexpr void trim_left_unchecked( ) {
			return CommentPolicy::trim_left_unchecked( *this );
		}

		constexpr void move_to_end_of_literal( ) {
			char const *f = first;
			char const *const l = last;
			if constexpr( IsUncheckedInput ) {
				while( ( static_cast<unsigned char>( *f ) > 0x20U ) &
				       not CommentPolicy::is_literal_end( *f ) ) {
					++f;
				}
			} else {
				while( ( f < l ) and ( ( static_cast<unsigned char>( *f ) > 0x20 ) &
				                       not CommentPolicy::is_literal_end( *f ) ) ) {
					++f;
				}
			}
			first = f;
		}

		[[nodiscard]] constexpr bool is_literal_end( ) const {
			return CommentPolicy::is_literal_end( *first );
		}

		DAW_ATTRIBUTE_FLATTEN [[nodiscard]] constexpr bool
		is_space_checked( ) const {
			daw_json_assert_weak( has_more( ), "Unexpected end" );
			return static_cast<unsigned char>( *first ) <= 0x20U;
		}

		DAW_ATTRIBUTE_FLATTEN [[nodiscard]] constexpr bool
		is_space_unchecked( ) const {
			return static_cast<unsigned char>( *first ) <= 0x20U;
		}

		[[nodiscard]] constexpr bool is_opening_bracket_checked( ) const {
			return DAW_JSON_LIKELY( first < last ) and *first == '[';
		}

		[[nodiscard]] constexpr bool is_opening_brace_checked( ) const {
			return DAW_JSON_LIKELY( first < last ) and *first == '{';
		}

		[[nodiscard]] constexpr bool is_closing_brace_checked( ) const {
			return DAW_JSON_LIKELY( first < last ) and *first == '}';
		}

		[[nodiscard]] constexpr bool is_quotes_checked( ) const {
			return DAW_JSON_LIKELY( first < last ) and *first == '"';
		}

		[[nodiscard]] constexpr bool is_exponent_checked( ) const {
			return DAW_JSON_LIKELY( first < last ) and
			       ( ( *first == 'e' ) bitor ( *first == 'E' ) );
		}

		constexpr void trim_left( ) {
			if constexpr( is_unchecked_input ) {
				trim_left_unchecked( );
			} else {
				trim_left_checked( );
			}
		}

		constexpr void clean_tail_unchecked( ) {
			trim_left_unchecked( );
			if( *first == ',' ) {
				++first;
				trim_left_unchecked( );
			}
		}

		constexpr void clean_tail_checked( ) {
			trim_left_checked( );
			if( DAW_JSON_LIKELY( first < last ) and *first == ',' ) {
				++first;
				trim_left_checked( );
			}
		}

		constexpr void clean_tail( ) {
			if constexpr( is_unchecked_input ) {
				clean_tail_unchecked( );
			} else {
				clean_tail_checked( );
			}
		}

		constexpr void move_to_next_class_member( ) {
			CommentPolicy::template move_to_next_of<'"', '}'>( *this );
		}

		constexpr bool is_at_next_class_member( ) const {
			return parse_policy_details::in<'"', '}'>( *first );
		}

		constexpr bool is_at_token_after_value( ) const {
			return parse_policy_details::in<',', '}', ']'>( *first );
		}

		template<char PrimLeft, char PrimRight, char SecLeft, char SecRight>
		[[nodiscard]] DAW_ATTRIBUTE_FLATTEN constexpr Range
		skip_bracketed_item_checked( ) {
			return CommentPolicy::template skip_bracketed_item_checked<
			  PrimLeft, PrimRight, SecLeft, SecRight>( *this );
		}

		template<char PrimLeft, char PrimRight, char SecLeft, char SecRight>
		[[nodiscard]] DAW_ATTRIBUTE_FLATTEN constexpr Range
		skip_bracketed_item_unchecked( ) {
			return CommentPolicy::template skip_bracketed_item_unchecked<
			  PrimLeft, PrimRight, SecLeft, SecRight>( *this );
		}

		[[nodiscard]] constexpr Range skip_class( ) {
			if constexpr( is_unchecked_input ) {
				return skip_bracketed_item_unchecked<'{', '}', '[', ']'>( );
			} else {
				return skip_bracketed_item_checked<'{', '}', '[', ']'>( );
			}
		}

		[[nodiscard]] constexpr Range skip_array( ) {
			if constexpr( is_unchecked_input ) {
				return skip_bracketed_item_unchecked<'[', ']', '{', '}'>( );
			} else {
				return skip_bracketed_item_checked<'[', ']', '{', '}'>( );
			}
		}
	}; // namespace daw::json

	using NoCommentSkippingPolicyChecked =
	  BasicParsePolicy<false, NoCommentSkippingPolicy, default_exec_tag, false>;

	using NoCommentSkippingPolicyUnchecked =
	  BasicParsePolicy<true, NoCommentSkippingPolicy, default_exec_tag, false>;

	template<typename ExecTag>
	using SIMDNoCommentSkippingPolicyChecked =
	  BasicParsePolicy<false, NoCommentSkippingPolicy, ExecTag, false>;

	template<typename ExecTag>
	using SIMDNoCommentSkippingPolicyUnchecked =
	  BasicParsePolicy<true, NoCommentSkippingPolicy, ExecTag, false>;

	using HashCommentSkippingPolicyChecked =
	  BasicParsePolicy<false, HashCommentSkippingPolicy, default_exec_tag, false>;

	using HashCommentSkippingPolicyUnchecked =
	  BasicParsePolicy<true, HashCommentSkippingPolicy, default_exec_tag, false>;

	template<typename ExecTag>
	using SIMDHashCommentSkippingPolicyChecked =
	  BasicParsePolicy<false, HashCommentSkippingPolicy, ExecTag, false>;

	template<typename ExecTag>
	using SIMDHashCommentSkippingPolicyUnchecked =
	  BasicParsePolicy<true, HashCommentSkippingPolicy, ExecTag, false>;

	using CppCommentSkippingPolicyChecked =
	  BasicParsePolicy<false, CppCommentSkippingPolicy, default_exec_tag, false>;

	using CppCommentSkippingPolicyUnchecked =
	  BasicParsePolicy<true, CppCommentSkippingPolicy, default_exec_tag, false>;

	template<typename ExecTag>
	using SIMDCppCommentSkippingPolicyChecked =
	  BasicParsePolicy<false, CppCommentSkippingPolicy, ExecTag, false>;

	template<typename ExecTag>
	using SIMDCppCommentSkippingPolicyUnchecked =
	  BasicParsePolicy<true, CppCommentSkippingPolicy, ExecTag, false>;

	namespace json_details {
		template<typename ParsePolicy>
		inline constexpr bool is_guaranteed_rvo_v = ParsePolicy::exec_tag_t::always_rvo;
	}
} // namespace daw::json
