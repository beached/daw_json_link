// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/daw_json_link
//

#pragma once

#include "daw_json_assert.h"
#include "daw_parse_policy_policy_details.h"

#include <daw/cpp_17.h>
#include <daw/daw_hide.h>

#include <cstddef>
#include <cstdint>
#include <iterator>
#include <type_traits>

namespace daw::json {
	template<typename Iterator, bool IsUncheckedInput>
	struct BasicHashCommentSkippingPolicy {
		using iterator = Iterator;
		static inline constexpr bool is_unchecked_input = IsUncheckedInput;
		using CharT = daw::remove_cvref_t<decltype( *std::declval<Iterator>( ) )>;

		using as_unchecked = BasicHashCommentSkippingPolicy<Iterator, true>;
		using as_checked = BasicHashCommentSkippingPolicy<Iterator, false>;

		inline constexpr void skip_comments( ) noexcept {
			if constexpr( is_unchecked_input ) {
				skip_comments_unchecked( );
			} else {
				skip_comments_checked( );
			}
		}

		[[nodiscard]] DAW_ATTRIBUTE_FLATTEN inline constexpr bool has_more( ) const
		  noexcept {
			return first < last;
		}

		inline constexpr void skip_comments_unchecked( ) noexcept {
			if( *first == '#' ) {
				++first;
				while( *first != '\n' ) {
					++first;
				}
				if( *first == '\n' ) {
					++first;
				}
			}
		}

		inline constexpr void skip_comments_checked( ) noexcept {
			if( first < last and *first == '#' ) {
				++first;
				while( first < last and *first != '\n' ) {
					++first;
				}
				if( *first == '\n' ) {
					++first;
				}
			}
		}

		inline constexpr bool at_literal_end( ) const noexcept {
			char const c = *first;
			return c == '\0' or c == ',' or c == ']' or c == '}' or c == '#';
		}

		inline constexpr bool is_space( ) const noexcept( is_unchecked_input ) {
			daw_json_assert_weak( has_more( ), "Unexpected end" );
			return *first <= 0x20;
		}

		inline constexpr bool is_space_unchecked( ) const noexcept {
			return *first <= 0x20;
		}

		inline constexpr void trim_left_checked( ) {
			skip_comments_checked( );
			while( has_more( ) and is_space_unchecked( ) ) {
				++first;
				skip_comments_checked( );
			}
		}

		inline constexpr void
		trim_left_unchecked( ) noexcept( is_unchecked_input ) {
			skip_comments_unchecked( );
			while( is_space_unchecked( ) ) {
				++first;
			}
		}

		inline constexpr void trim_left( ) noexcept( is_unchecked_input ) {
			if constexpr( is_unchecked_input ) {
				trim_left_unchecked( );
			} else {
				trim_left_checked( );
			}
		}

		inline constexpr void clean_tail( ) noexcept( is_unchecked_input ) {
			trim_left( );
			if( *first == ',' ) {
				++first;
				trim_left( );
			}
		}

		inline constexpr void
		move_to_next_of( char c ) noexcept( is_unchecked_input ) {
			skip_comments( );
			daw_json_assert_weak( has_more( ), "Unexpected end of data" );
			while( *first != c ) {
				daw_json_assert_weak( has_more( ), "Unexpected end of data" );
				++first;
				skip_comments( );
			}
		}

		template<std::size_t N>
		inline constexpr void move_to_next_of( char const ( &str )[N] ) {
			skip_comments( );

			daw_json_assert_weak( has_more( ), "Unexpected end of data" );
			while( not parse_policy_details::in( *first, str ) ) {
				daw_json_assert_weak( has_more( ), "Unexpected end of data" );
				++first;
				skip_comments( );
			}
		}

		//*********************************************************
		using policy = BasicHashCommentSkippingPolicy;
		static_assert( std::is_convertible_v<
		                 typename std::iterator_traits<iterator>::iterator_category,
		                 std::random_access_iterator_tag>,
		               "Expecting a Random Contiguous Iterator" );
		iterator first{};
		iterator last{};
		iterator class_first{};
		iterator class_last{};
		std::size_t counter = 0;
		using Range = BasicHashCommentSkippingPolicy;

		template<std::size_t N>
		inline constexpr bool operator==( char const ( &rhs )[N] ) const {
			if( size( ) < ( N - 1 ) ) {
				return false;
			}
			bool result = true;
			for( std::size_t n = 0; n < ( N - 1 ); ++n ) {
				result = result and ( first[n] == rhs[n] );
			}
			return result;
		}

		inline constexpr BasicHashCommentSkippingPolicy( ) = default;

		inline constexpr BasicHashCommentSkippingPolicy( iterator f, iterator l )
		  : first( f )
		  , last( l )
		  , class_first( f )
		  , class_last( l ) {

			daw_json_assert( not is_null( ), "Unexpected null start of range" );
		}

		[[nodiscard]] DAW_ATTRIBUTE_FLATTEN inline constexpr bool empty( ) const
		  noexcept {
			return first == last;
		}

		[[nodiscard]] inline constexpr decltype( auto ) front( ) const noexcept {
			return *first;
		}

		[[nodiscard]] inline constexpr bool front( char c ) const noexcept {
			return first < last and *first == c;
		}

		[[nodiscard]] inline constexpr std::size_t size( ) const noexcept {
			return static_cast<std::size_t>( std::distance( first, last ) );
		}

		[[nodiscard]] inline constexpr bool is_number( ) const noexcept {
			return static_cast<unsigned>( front( ) ) - static_cast<unsigned>( '0' ) <
			       10U;
		}

		template<std::size_t N>
		[[nodiscard]] inline constexpr bool front( char const ( &set )[N] ) const {
			if( empty( ) ) {
				return false;
			}
			bool result = false;
			for( std::size_t n = 0; n < ( N - 1 ); ++n ) {
				result |= parse_policy_details::in( *first, set[n] );
			}
			return result;
		}

		[[nodiscard]] inline constexpr bool is_null( ) const {
			return first == nullptr;
		}

		inline constexpr void remove_prefix( ) {
			++first;
		}

		inline constexpr void remove_prefix( std::size_t n ) noexcept {
			first += static_cast<intmax_t>( n );
		}

		[[nodiscard]] inline constexpr iterator begin( ) const noexcept {
			return first;
		}

		[[nodiscard]] inline constexpr iterator data( ) const noexcept {
			return first;
		}

		[[nodiscard]] inline constexpr iterator end( ) const noexcept {
			return last;
		}

		[[nodiscard]] explicit inline constexpr operator bool( ) const noexcept {
			return not empty( );
		}

		inline constexpr void set_class_position( ) noexcept {
			class_first = first;
			class_last = last;
			counter = 0;
		}

		template<char PrimLeft, char PrimRight, char SecLeft, char SecRight>
		[[nodiscard]] DAW_ATTRIBUTE_FLATTEN inline constexpr Range
		skip_bracketed_item_checked( ) noexcept( is_unchecked_input ) {
			// Not checking for Left as it is required to be skipped already
			auto result = *this;
			result.counter = 0;
			std::int_fast64_t prime_bracket_count = 1;
			std::int_fast64_t second_bracket_count = 0;
			bool in_quotes = false;
			if( has_more( ) and *first == PrimLeft ) {
				++first;
			}
			while( has_more( ) and prime_bracket_count > 0 and
			       second_bracket_count >= 0 ) {
				switch( *first ) {
				case '\\':
					++first;
					break;
				case '"':
					in_quotes = not in_quotes;
					break;
				case ',':
					if( not in_quotes and prime_bracket_count == 1 and
					    second_bracket_count == 0 ) {
						++result.counter;
					}
					break;
				case PrimLeft:
					if( not in_quotes ) {
						++prime_bracket_count;
					}
					break;
				case PrimRight:
					if( not in_quotes ) {
						--prime_bracket_count;
					}
					break;
				case SecLeft:
					if( not in_quotes ) {
						++second_bracket_count;
					}
					break;
				case SecRight:
					if( not in_quotes ) {
						--second_bracket_count;
					}
					break;
				case '#':
					if( not in_quotes ) {
						while( has_more( ) and *first != '\n' ) {
							++first;
						}
						if( not has_more( ) ) {
							continue;
						}
					}
					break;
				}
				++first;
			}
			daw_json_assert_weak( prime_bracket_count == 0 and
			                        second_bracket_count == 0,
			                      "Unexpected bracketing" );
			// We include the close primary bracket in the range so that subsequent
			// parsers have a terminator inside their range
			result.last = first;
			return result;
		}

		template<char PrimLeft, char PrimRight, char SecLeft, char SecRight>
		[[nodiscard]] DAW_ATTRIBUTE_FLATTEN inline constexpr Range
		skip_bracketed_item_unchecked( ) noexcept( is_unchecked_input ) {
			// Not checking for Left as it is required to be skipped already
			auto result = *this;
			result.counter = 0;
			std::int_fast64_t prime_bracket_count = 1;
			std::int_fast64_t second_bracket_count = 0;
			bool in_quotes = false;
			if( *first == PrimLeft ) {
				++first;
			}
			while( prime_bracket_count > 0 ) {
				switch( *first ) {
				case '\\':
					++first;
					break;
				case '"':
					in_quotes = not in_quotes;
					break;
				case ',':
					if( not in_quotes and prime_bracket_count == 1 and
					    second_bracket_count == 0 ) {
						++result.counter;
					}
					break;
				case PrimLeft:
					if( not in_quotes ) {
						++prime_bracket_count;
					}
					break;
				case PrimRight:
					if( not in_quotes ) {
						--prime_bracket_count;
					}
					break;
				case SecLeft:
					if( not in_quotes ) {
						++second_bracket_count;
					}
					break;
				case SecRight:
					if( not in_quotes ) {
						--second_bracket_count;
					}
					break;
				case '#':
					if( not in_quotes ) {
						while( *first != '\n' ) {
							++first;
						}
					}
					break;
				}
				++first;
			}
			// We include the close primary bracket in the range so that subsequent
			// parsers have a terminator inside their range
			result.last = first;
			return result;
		}

		[[nodiscard]] inline constexpr Range
		skip_class( ) noexcept( is_unchecked_input ) {
			if constexpr( is_unchecked_input ) {
				return skip_bracketed_item_unchecked<'{', '}', '[', ']'>( );
			} else {
				return skip_bracketed_item_checked<'{', '}', '[', ']'>( );
			}
		}

		[[nodiscard]] inline constexpr Range
		skip_array( ) noexcept( is_unchecked_input ) {
			if constexpr( is_unchecked_input ) {
				return skip_bracketed_item_unchecked<'[', ']', '{', '}'>( );
			} else {
				return skip_bracketed_item_checked<'[', ']', '{', '}'>( );
			}
		}
	};

	using HashCommentSkippingPolicyChecked =
	  BasicHashCommentSkippingPolicy<char const *, false>;
	using HashCommentSkippingPolicyUnchecked =
	  BasicHashCommentSkippingPolicy<char const *, true>;
} // namespace daw::json
