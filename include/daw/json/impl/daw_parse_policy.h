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
		static constexpr bool force_name_equal_check = false;
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

		template<char c>
		DAW_ATTRIBUTE_FLATTEN constexpr void move_to_next_of_unchecked( ) {
#if defined( DAW_ALLOW_SSE42 )
			if constexpr( simd_mode == SIMDModes::SSE42 ) {
				first = reinterpret_cast<char const *>( std::memchr(
				  first, c, static_cast<std::size_t>( last - first ) ) );
			} else {
#endif
				while( *first != c ) {
					++first;
				}
#if defined( DAW_ALLOW_SSE42 )
			}
#endif
		}

		template<char c>
		DAW_ATTRIBUTE_FLATTEN constexpr void move_to_next_of_checked( ) {
#if defined( DAW_ALLOW_SSE42 )
			if constexpr( simd_mode == SIMDModes::SSE42 ) {
				char const * ptr = reinterpret_cast<char const *>( std::memchr(
				  first, c, static_cast<std::size_t>( last - first ) ) );
				daw_json_assert( ptr != nullptr, "Expected token missing" );
				first = ptr;
			} else {
#endif
				while( first < last and *first != c ) {
					++first;
				}
#if defined( DAW_ALLOW_SSE42 )
			}
#endif
		}

		template<char c>
		DAW_ATTRIBUTE_FLATTEN constexpr void move_to_next_of( ) {
			if( is_unchecked_input ) {
				move_to_next_of_unchecked<c>( );
			} else {
				move_to_next_of_checked<c>( );
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

		constexpr void move_to_end_of_literal( ) {
			char const *f = first;
			char const *const l = last;
			if constexpr( IsUncheckedInput ) {
				while( ( *f > 0x20 ) and not CommentPolicy::is_literal_end( *f ) ) {
					++f;
				}
			} else {
				while( ( f < l ) and ( *f > 0x20 ) and
				       not CommentPolicy::is_literal_end( *f ) ) {
					++f;
				}
			}
			first = f;
		}

		[[nodiscard]] constexpr bool is_literal_end( ) const {
			return CommentPolicy::is_literal_end( *first );
		}

		[[nodiscard]] constexpr bool is_space_checked( ) const {
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
			return first < last and ( ( *first == 'e' ) bitor ( *first == 'E' ) );
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
			if( first < last and *first == ',' ) {
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
