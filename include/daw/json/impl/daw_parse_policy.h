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
	template<bool IsUncheckedInput, typename CommentPolicy, SIMDModes SIMDMode,
	         bool AllowEscapedNames>
	struct BasicParsePolicy final {
		using iterator = char const *;
		static constexpr bool is_unchecked_input = IsUncheckedInput;
		static constexpr SIMDModes simd_mode = SIMDMode;
		static constexpr bool allow_escaped_names = AllowEscapedNames;
		using CharT = char;

		using as_unchecked =
		  BasicParsePolicy<true, CommentPolicy, simd_mode, allow_escaped_names>;
		using as_checked =
		  BasicParsePolicy<false, CommentPolicy, simd_mode, allow_escaped_names>;

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

		DAW_ATTRIBUTE_FLATTEN constexpr void move_to_next_of_unchecked( char c ) {
			return CommentPolicy::move_to_next_of_unchecked( *this, c );
		}

		DAW_ATTRIBUTE_FLATTEN constexpr void move_to_next_of_checked( char c ) {
			return CommentPolicy::move_to_next_of_checked( *this, c );
		}

		template<std::size_t N, std::size_t... Is>
		constexpr void move_to_next_of_nc_unchecked( char const ( &str )[N],
		                                             std::index_sequence<Is...> ) {
			while( ( ( *first != str[Is] ) and ... ) ) {
				++first;
			}
		}

		template<std::size_t N, std::size_t... Is>
		constexpr void move_to_next_of_nc_checked( char const ( &str )[N],
		                                           std::index_sequence<Is...> ) {
			while( has_more( ) and ( ( *first != str[Is] ) and ... ) ) {
				++first;
			}
		}

		template<std::size_t N>
		constexpr void move_to_next_of_nc( char const ( &str )[N] ) {
			if constexpr( is_unchecked_input ) {
				move_to_next_of_nc_unchecked( str, std::make_index_sequence<N>{ } );
			} else {
				move_to_next_of_nc_checked( str, std::make_index_sequence<N>{ } );
			}
		}

		[[nodiscard]] constexpr char front( ) const {
			return *first;
		}

		[[nodiscard]] constexpr std::size_t size( ) const {
			return static_cast<std::size_t>( std::distance( first, last ) );
		}

		[[nodiscard]] constexpr bool is_number( ) const {
			return static_cast<unsigned>( front( ) ) - static_cast<unsigned>( '0' ) <
			       10U;
		}

		/*
		template<JSONNAMETYPE Set>
		[[nodiscard]] constexpr bool in( ) const {
		  assert( first );
		  if( empty( ) ) {
		    return false;
		  }
		  return parse_policy_details::template in<Set>( *first );
		}*/

		[[nodiscard]] constexpr bool is_null( ) const {
			return first == nullptr;
		}

		constexpr void remove_prefix( ) {
			++first;
		}

		constexpr void remove_prefix( std::size_t n ) {
			first += static_cast<intmax_t>( n );
		}

		[[nodiscard]] constexpr iterator begin( ) const {
			return first;
		}

		[[nodiscard]] constexpr iterator data( ) const {
			return first;
		}

		[[nodiscard]] constexpr iterator end( ) const {
			return last;
		}

		[[nodiscard]] explicit constexpr operator bool( ) const {
			return not empty( );
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

		[[nodiscard]] constexpr bool at_literal_end( ) const {
			return CommentPolicy::at_literal_end( *first );
			char const c = *first;
			return c == '\0' or c == ',' or c == ']' or c == '}';
		}

		[[nodiscard]] constexpr bool is_space( ) const {
			daw_json_assert_weak( has_more( ), "Unexpected end" );
			return *first <= 0x20;
		}

		[[nodiscard]] constexpr bool is_space_unchecked( ) const {
			return *first <= 0x20;
		}

		[[nodiscard]] constexpr bool is_opening_bracket_checked( ) const {
			return first < last and *first == '[';
		}

		[[nodiscard]] constexpr bool is_opening_bracket_unchecked( ) const {
			return *first == '[';
		}

		[[nodiscard]] constexpr bool is_opening_brace_checked( ) const {
			return first < last and *first == '{';
		}

		[[nodiscard]] constexpr bool is_closing_brace_checked( ) const {
			return first < last and *first == '}';
		}

		[[nodiscard]] constexpr bool is_closing_brace_unchecked( ) const {
			return *first == '}';
		}

		[[nodiscard]] constexpr bool is_closing_bracket_checked( ) const {
			return first < last and *first == ']';
		}

		[[nodiscard]] constexpr bool is_closing_bracket_unchecked( ) const {
			return *first == ']';
		}

		[[nodiscard]] constexpr bool is_quotes_unchecked( ) const {
			return *first == '"';
		}

		[[nodiscard]] constexpr bool is_quotes_checked( ) const {
			return first < last and *first == '"';
		}

		[[nodiscard]] constexpr bool is_comma_checked( ) const {
			return first < last and *first == ',';
		}

		[[nodiscard]] constexpr bool is_colon_unchecked( ) const {
			return *first == ':';
		}

		[[nodiscard]] constexpr bool is_minus_unchecked( ) const {
			return *first == '-';
		}

		[[nodiscard]] constexpr bool is_plus_unchecked( ) const {
			return *first == '+';
		}

		[[nodiscard]] constexpr bool is_n_unchecked( ) const {
			return *first == 'n';
		}

		[[nodiscard]] constexpr bool is_exponent_checked( ) const {
			return first < last and ( *first == 'e' bitor *first == 'E' );
		}

		[[nodiscard]] constexpr bool is_t_unchecked( ) const {
			return *first == 't';
		}

		[[nodiscard]] constexpr bool is_u_unchecked( ) const {
			return *first == 'u';
		}

		[[nodiscard]] constexpr bool is_escape_unchecked( ) const {
			return *first == '\\';
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
			if( front( ',' ) ) {
				++first;
				trim_left_checked( );
			}
		}

		constexpr void clean_tail( ) {
			if constexpr( is_unchecked_input ) {
				clean_tail_unchecked( );
			} else {
				clean_tail_unchecked( );
			}
		}

		template<char c>
		constexpr void move_to_next_char( ) {
			CommentPolicy::template move_to_next_char<c>( *this );
		}

	private:
		static constexpr char const next_class_member_tokens[] = "\"}";
		static constexpr char const token_after_value[] = ",}]";

	public:
		constexpr void move_to_next_class_member( ) {
			CommentPolicy::template move_to_next_of<next_class_member_tokens>(
			  *this );
		}

		constexpr bool is_at_next_class_member( ) const {
			return parse_policy_details::in<next_class_member_tokens>( *first );
		}

		constexpr bool is_at_token_after_value( ) const {
			return parse_policy_details::in<token_after_value>( *first );
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
	};

	using NoCommentSkippingPolicyChecked =
	  BasicParsePolicy<false, NoCommentSkippingPolicy, SIMDModes::None, false>;

	using NoCommentSkippingPolicyUnchecked =
	  BasicParsePolicy<true, NoCommentSkippingPolicy, SIMDModes::None, false>;

	template<SIMDModes mode>
	using SIMDNoCommentSkippingPolicyChecked =
	  BasicParsePolicy<false, NoCommentSkippingPolicy, mode, false>;

	template<SIMDModes mode>
	using SIMDNoCommentSkippingPolicyUnchecked =
	  BasicParsePolicy<true, NoCommentSkippingPolicy, mode, false>;

	using HashCommentSkippingPolicyChecked =
	  BasicParsePolicy<false, HashCommentSkippingPolicy, SIMDModes::None, false>;

	using HashCommentSkippingPolicyUnchecked =
	  BasicParsePolicy<true, HashCommentSkippingPolicy, SIMDModes::None, false>;

	template<SIMDModes mode>
	using SIMDHashCommentSkippingPolicyChecked =
	  BasicParsePolicy<false, HashCommentSkippingPolicy, mode, false>;

	template<SIMDModes mode>
	using SIMDHashCommentSkippingPolicyUnchecked =
	  BasicParsePolicy<true, HashCommentSkippingPolicy, mode, false>;

	using CppCommentSkippingPolicyChecked =
	  BasicParsePolicy<false, CppCommentSkippingPolicy, SIMDModes::None, false>;

	using CppCommentSkippingPolicyUnchecked =
	  BasicParsePolicy<true, CppCommentSkippingPolicy, SIMDModes::None, false>;

	template<SIMDModes mode>
	using SIMDCppCommentSkippingPolicyChecked =
	  BasicParsePolicy<false, CppCommentSkippingPolicy, mode, false>;

	template<SIMDModes mode>
	using SIMDCppCommentSkippingPolicyUnchecked =
	  BasicParsePolicy<true, CppCommentSkippingPolicy, mode, false>;
} // namespace daw::json
