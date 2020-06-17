// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/daw_json_link
//

#pragma once

#include "daw_json_assert.h"

#include <daw/cpp_17.h>
#include <daw/daw_do_n.h>
#include <daw/daw_hide.h>

#include <iterator>
#include <type_traits>

namespace daw::json {
	namespace parse_policy_details {
		[[nodiscard]] DAW_ATTRIBUTE_FLATTEN inline constexpr bool
		in( char lhs, char rhs ) noexcept {
			return lhs == rhs;
		}

		template<std::size_t N>
		[[nodiscard]] DAW_ATTRIBUTE_FLATTEN inline constexpr bool
		in( char c, char const ( &set )[N] ) noexcept {
			unsigned result = 0;
			daw::algorithm::do_n_arg<N>( [&]( std::size_t n ) {
				result |= static_cast<unsigned>( set[n] == c );
			} );
			return static_cast<bool>( result );
		}

		[[nodiscard]] DAW_ATTRIBUTE_FLATTEN inline constexpr bool
		at_end_of_item( char c ) noexcept {
			return static_cast<bool>(
			  static_cast<unsigned>( c == ',' ) | static_cast<unsigned>( c == '}' ) |
			  static_cast<unsigned>( c == ']' ) | static_cast<unsigned>( c == ':' ) |
			  static_cast<unsigned>( c <= 0x20 ) );
		}

		[[nodiscard]] DAW_ATTRIBUTE_FLATTEN inline constexpr bool
		is_real_number_part( char c ) noexcept {
			switch( c ) {
			case '0':
			case '1':
			case '2':
			case '3':
			case '4':
			case '5':
			case '6':
			case '7':
			case '8':
			case '9':
			case 'e':
			case 'E':
			case '+':
			case '-':
				return true;
			}
			return false;
		}

		template<typename Iterator>
		[[nodiscard]] DAW_ATTRIBUTE_FLATTEN inline constexpr bool
		empty( Iterator first, Iterator last ) {
			return first == last;
		}

		template<typename Iterator>
		[[nodiscard]] DAW_ATTRIBUTE_FLATTEN inline constexpr bool
		has_more( Iterator first, Iterator last ) {
			return first != last;
		}

		[[nodiscard]] DAW_ATTRIBUTE_FLATTEN inline constexpr bool
		is_null( char const *const first ) {
			return first == nullptr;
		}

		[[nodiscard]] DAW_ATTRIBUTE_FLATTEN inline constexpr bool
		is_space( char const *const first, char const *const &last,
		          std::bool_constant<true> ) {
			daw_json_assert( has_more( first, last ), "Unexpected end of stream" );
			auto const c = *first;
			return c > 0x0 and c <= 0x20;
		}

		[[nodiscard]] DAW_ATTRIBUTE_FLATTEN inline constexpr bool
		is_space( char const *const first, char const *const &,
		          std::bool_constant<false> ) {
			auto const c = *first;
			return c > 0x0 and c <= 0x20;
		}

		template<typename Iterator>
		[[nodiscard]] DAW_ATTRIBUTE_FLATTEN inline constexpr bool
		can_parse_more( Iterator first, Iterator last ) {
			return not is_null( first ) and has_more( first, last );
		}
	} // namespace parse_policy_details

	template<typename Iterator, bool IsUncheckedInput>
	struct BasicNoCommentSkippingPolicy {
		using iterator = Iterator;
		static inline constexpr bool is_unchecked_input = IsUncheckedInput;
		using CharT = daw::remove_cvref_t<decltype( *std::declval<Iterator>( ) )>;

		using as_unchecked = BasicNoCommentSkippingPolicy<Iterator, true>;
		using as_checked = BasicNoCommentSkippingPolicy<Iterator, false>;
		//*********************************************************
		using policy = BasicNoCommentSkippingPolicy;
		static_assert( std::is_convertible_v<
		                 typename std::iterator_traits<iterator>::iterator_category,
		                 std::random_access_iterator_tag>,
		               "Expecting a Random Contiguous Iterator" );
		iterator first{ };
		iterator last{ };
		iterator class_first{ };
		iterator class_last{ };
		std::size_t counter = 0;
		using Range = BasicNoCommentSkippingPolicy;

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

		inline constexpr BasicNoCommentSkippingPolicy( ) = default;

		inline constexpr BasicNoCommentSkippingPolicy( iterator f, iterator l )
		  : first( f )
		  , last( l )
		  , class_first( f )
		  , class_last( l ) {}

		[[nodiscard]] inline constexpr bool empty( ) const {
			return parse_policy_details::empty( first, last );
		}

		[[nodiscard]] inline constexpr bool has_more( ) const {
			return parse_policy_details::has_more( first, last );
		}

		[[nodiscard]] inline constexpr bool can_parse_more( ) const {
			return parse_policy_details::can_parse_more( first, last );
		}

		[[nodiscard]] inline constexpr decltype( auto ) front( ) const {
			return *first;
		}

		[[nodiscard]] inline constexpr bool front( char c ) const {
			return first != last and *first == c;
		}

		[[nodiscard]] inline constexpr std::size_t size( ) const {
			return static_cast<std::size_t>( std::distance( first, last ) );
		}

		[[nodiscard]] inline constexpr bool is_number( ) const {
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
			return parse_policy_details::is_null( first );
		}

		inline constexpr void remove_prefix( ) {
			++first;
		}

		inline constexpr void remove_prefix( std::size_t n ) {
			std::advance( first, static_cast<intmax_t>( n ) );
		}

		[[nodiscard]] inline constexpr iterator begin( ) const {
			return first;
		}

		[[nodiscard]] inline constexpr iterator data( ) const {
			return first;
		}

		[[nodiscard]] inline constexpr iterator end( ) const {
			return last;
		}

		[[nodiscard]] explicit inline constexpr operator bool( ) const {
			return not empty( );
		}

		inline constexpr void set_class_position( ) {
			class_first = first;
			class_last = last;
		}

		//*********************************************************
		inline constexpr bool at_literal_end( ) const noexcept {
			char const c = *first;
			return c == '\0' or c == ',' or c == ']' or c == '}';
		}

		inline constexpr bool is_space( ) const noexcept {
			return parse_policy_details::is_space(
			  first, last, std::bool_constant<is_unchecked_input>{ } );
		}

		inline constexpr void trim_left_checked( ) noexcept {
			while( parse_policy_details::has_more( first, last ) and is_space( ) ) {
				++first;
			}
		}

		inline constexpr void trim_left_unchecked( ) noexcept {
			while( is_space( ) ) {
				++first;
			}
		}

		inline constexpr void trim_left( ) {
			if constexpr( is_unchecked_input ) {
				trim_left_unchecked( );
			} else {
				trim_left_checked( );
			}
		}

		inline constexpr void clean_tail( ) noexcept {
			while( parse_policy_details::has_more( first, last ) and is_space( ) ) {
				++first;
			}
			if( *first == ',' ) {
				++first;
				trim_left( );
			}
		}

		inline constexpr void
		move_to_next_of( char c ) noexcept( is_unchecked_input ) {
			if constexpr( not is_unchecked_input ) {
				daw_json_assert( parse_policy_details::has_more( first, last ),
				                 "Unexpected end of data" );
			}
			while( *first != c ) {
				++first;
				if constexpr( not is_unchecked_input ) {
					daw_json_assert( parse_policy_details::has_more( first, last ),
					                 "Unexpected end of data" );
				}
			}
		}

		template<std::size_t N>
		inline constexpr void
		move_to_next_of( char const ( &str )[N] ) noexcept( is_unchecked_input ) {
			if constexpr( not is_unchecked_input ) {
				daw_json_assert( parse_policy_details::has_more( first, last ),
				                 "Unexpected end of data" );
			}
			while( not parse_policy_details::in( *first, str ) ) {
				++first;
				if constexpr( not is_unchecked_input ) {
					daw_json_assert( parse_policy_details::has_more( first, last ),
					                 "Unexpected end of data" );
				}
			}
		}

		template<char Left, char Right>
		inline constexpr void
		skip_to_end_of_bracketed_item( ) noexcept( is_unchecked_input ) {
			// Not checking for Left as it is required to be skipped already
			std::size_t bracket_count = 1;
			bool in_quotes = false;
			clean_tail( );
			if( not parse_policy_details::empty( first, last ) and
			    bracket_count > 0 ) {
				if( not in_quotes ) {
					trim_left( );
				}
				switch( *first ) {
				case '\\':
					++first;
					break;
				case '"':
					in_quotes = not in_quotes;
					break;
				case Left:
					if( not in_quotes ) {
						++bracket_count;
					}
					break;
				case Right:
					if( not in_quotes ) {
						--bracket_count;
					}
					break;
				}
			}
			while( parse_policy_details::has_more( first, last ) and
			       bracket_count > 0 ) {
				++first;
				if( not in_quotes ) {
					trim_left( );
				}
				switch( *first ) {
				case '\\':
					++first;
					break;
				case '"':
					in_quotes = not in_quotes;
					break;
				case Left:
					if( not in_quotes ) {
						++bracket_count;
					}
					break;
				case Right:
					if( not in_quotes ) {
						--bracket_count;
					}
					break;
				}
			}
			if constexpr( not is_unchecked_input ) {
				daw_json_assert( parse_policy_details::has_more( first, last ) and
				                   *first == Right,
				                 "Expected closing bracket/brace" );
			}
		}

		template<char Left, char Right>
		[[nodiscard]] inline constexpr Range
		skip_bracketed_item( ) noexcept( is_unchecked_input ) {
			// Not checking for Left as it is required to be skipped already
			std::size_t bracket_count = 1;
			std::size_t comma_count = 0;
			bool in_quotes = false;
			auto result = *this;
			if( *first == Right ) {
				return result;
			}
			while( parse_policy_details::has_more( first, last ) and
			       bracket_count > 0 ) {
				++first;
				if( not in_quotes ) {
					trim_left( );
				}
				switch( *first ) {
				case '\\':
					++first;
					break;
				case '"':
					in_quotes = not in_quotes;
					break;
				case ',':
					if( not in_quotes and bracket_count == 1 ) {
						++comma_count;
					}
					break;
				case Left:
					if( not in_quotes ) {
						++bracket_count;
					}
					break;
				case Right:
					if( not in_quotes ) {
						--bracket_count;
					}
					break;
				}
			}
			if constexpr( not is_unchecked_input ) {
				daw_json_assert( parse_policy_details::has_more( first, last ) and
				                   *first == Right,
				                 "Expected closing bracket/brace" );
			}

			++first;
			result.last = first;
			result.counter = comma_count;
			return result;
		}
	}; // namespace daw::json
	//*******************************************
	template<typename Iterator, bool IsUncheckedInput>
	struct BasicHashCommentSkippingPolicy {
		using iterator = Iterator;
		static inline constexpr bool is_unchecked_input = IsUncheckedInput;
		using CharT = daw::remove_cvref_t<decltype( *std::declval<Iterator>( ) )>;

		using as_unchecked = BasicHashCommentSkippingPolicy<Iterator, true>;
		using as_checked = BasicHashCommentSkippingPolicy<Iterator, false>;

		DAW_ATTRIBUTE_FLATTEN static inline constexpr void
		skip_comments( Iterator &first, Iterator &last ) noexcept {
			if constexpr( is_unchecked_input ) {
				skip_comments_unchecked( first, last );
			} else {
				skip_comments_checked( first, last );
			}
		}

		DAW_ATTRIBUTE_FLATTEN static inline constexpr void
		skip_comments_unchecked( Iterator &first, Iterator &last ) noexcept {
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

		DAW_ATTRIBUTE_FLATTEN static inline constexpr void
		skip_comments_checked( Iterator &first, Iterator &last ) noexcept {
			if( first != last and *first == '#' ) {
				++first;
				while( first != last and *first != '\n' ) {
					++first;
				}
				if( *first == '\n' ) {
					++first;
				}
			}
		}

		DAW_ATTRIBUTE_FLATTEN static inline constexpr bool
		at_literal_end( char c ) {
			return c == '\0' or c == ',' or c == ']' or c == '}' or c == '#';
		}

		DAW_ATTRIBUTE_FLATTEN static inline constexpr bool
		is_space( Iterator first, Iterator last ) {
			return parse_policy_details::is_space(
			  first, last, std::bool_constant<is_unchecked_input>{ } );
		}

		DAW_ATTRIBUTE_FLATTEN static inline constexpr void
		trim_left_checked( Iterator &first, Iterator last ) {
			skip_comments_checked( first, last );
			while( parse_policy_details::has_more( first, last ) and
			       is_space( first, last ) ) {
				++first;
				skip_comments_checked( first, last );
			}
		}

		DAW_ATTRIBUTE_FLATTEN static inline constexpr void
		trim_left_unchecked( Iterator &first, Iterator last ) {
			skip_comments_unchecked( first, last );
			while( is_space( first, last ) ) {
				++first;
				if constexpr( not is_unchecked_input ) {
					daw_json_assert( parse_policy_details::has_more( first, last ),
					                 "Unexpected end of stream" );
				}
			}
		}

		DAW_ATTRIBUTE_FLATTEN static inline constexpr void
		trim_left( Iterator &first, Iterator last ) {
			if constexpr( is_unchecked_input ) {
				trim_left_unchecked( first, last );
			} else {
				trim_left_checked( first, last );
			}
		}

		DAW_ATTRIBUTE_FLATTEN static inline constexpr void
		clean_tail( Iterator &first, Iterator last ) {
			while( parse_policy_details::has_more( first, last ) and
			       is_space( first, last ) ) {
				++first;
			}
			if( *first == ',' ) {
				++first;
				trim_left( first, last );
			}
		}

		DAW_ATTRIBUTE_FLATTEN static inline constexpr void
		move_to_next_of( Iterator &first, Iterator last, char c ) {
			skip_comments( first, last );
			if constexpr( not is_unchecked_input ) {
				daw_json_assert( parse_policy_details::has_more( first, last ),
				                 "Unexpected end of data" );
			}
			while( *first != c ) {
				if constexpr( not is_unchecked_input ) {
					daw_json_assert( parse_policy_details::has_more( first, last ),
					                 "Unexpected end of data" );
				}
				++first;
				skip_comments( first, last );
			}
		}

		template<std::size_t N>
		DAW_ATTRIBUTE_FLATTEN static inline constexpr void
		move_to_next_of( Iterator &first, Iterator last, char const ( &str )[N] ) {
			skip_comments( first, last );

			if constexpr( not is_unchecked_input ) {
				daw_json_assert( parse_policy_details::has_more( first, last ),
				                 "Unexpected end of data" );
			}
			while( not parse_policy_details::in( *first, str ) ) {
				if constexpr( not is_unchecked_input ) {
					daw_json_assert( parse_policy_details::has_more( first, last ),
					                 "Unexpected end of data" );
				}
				++first;
				skip_comments( first, last );
			}
		}

		template<char Left, char Right>
		DAW_ATTRIBUTE_FLATTEN static inline constexpr void
		skip_to_end_of_bracketed_item( Iterator &first, Iterator const &last ) {
			// Not checking for Left as it is required to be skipped already
			std::size_t bracket_count = 1;
			bool in_quotes = false;
			clean_tail( first, last );
			if( not parse_policy_details::empty( first, last ) and
			    bracket_count > 0 ) {
				if( not in_quotes ) {
					trim_left( first, last );
				}
				switch( *first ) {
				case '\\':
					++first;
					break;
				case '"':
					in_quotes = not in_quotes;
					break;
				case Left:
					if( not in_quotes ) {
						++bracket_count;
					}
					break;
				case Right:
					if( not in_quotes ) {
						--bracket_count;
					}
					break;
				case '#':
					if( not in_quotes ) {
						trim_left_checked( first, last );
					}
					break;
				}
			}
			while( parse_policy_details::has_more( first, last ) and
			       bracket_count > 0 ) {
				++first;
				if( not in_quotes ) {
					trim_left( first, last );
				}
				switch( *first ) {
				case '\\':
					++first;
					break;
				case '"':
					in_quotes = not in_quotes;
					break;
				case Left:
					if( not in_quotes ) {
						++bracket_count;
					}
					break;
				case Right:
					if( not in_quotes ) {
						--bracket_count;
					}
					break;
				case '#':
					if( not in_quotes ) {
						trim_left_checked( first, last );
					}
					break;
				}
			}
			if constexpr( not is_unchecked_input ) {
				daw_json_assert( parse_policy_details::has_more( first, last ) and
				                   *first == Right,
				                 "Expected closing bracket/brace" );
			}
		}

		template<char Left, char Right, typename Range>
		[[nodiscard]] DAW_ATTRIBUTE_FLATTEN static inline constexpr Range
		skip_bracketed_item( Range &rng ) {
			// Not checking for Left as it is required to be skipped already
			std::size_t bracket_count = 1;
			bool in_quotes = false;
			auto result = rng;
			std::size_t comma_count = 0;
			while( parse_policy_details::has_more( rng.first, rng.last ) and
			       bracket_count > 0 ) {
				++rng.first;
				if( not in_quotes ) {
					trim_left( rng.first, rng.last );
				}
				switch( *rng.first ) {
				case '\\':
					++rng.first;
					break;
				case '"':
					in_quotes = not in_quotes;
					break;
				case ',':
					if( not in_quotes and bracket_count == 1 ) {
						++comma_count;
					}
					break;
				case Left:
					if( not in_quotes ) {
						++bracket_count;
					}
					break;
				case Right:
					if( not in_quotes ) {
						--bracket_count;
					}
					break;
				case '#':
					if( not in_quotes ) {
						trim_left_checked( rng.first, rng.last );
					}
					break;
				}
			}
			if constexpr( not is_unchecked_input ) {
				daw_json_assert(
				  parse_policy_details::has_more( rng.first, rng.last ) and
				    *rng.first == Right,
				  "Expected closing bracket/brace" );
			}

			++rng.first;
			result.last = rng.first;
			result.counter = comma_count;
			return result;
		}
	};

	using NoCommentSkippingPolicyChecked =
	  BasicNoCommentSkippingPolicy<char const *, false>;
	using NoCommentSkippingPolicyUnchecked =
	  BasicNoCommentSkippingPolicy<char const *, true>;
	using HashCommentSkippingPolicyChecked =
	  BasicHashCommentSkippingPolicy<char const *, false>;
	using HashCommentSkippingPolicyUnchecked =
	  BasicHashCommentSkippingPolicy<char const *, true>;

} // namespace daw::json

namespace daw::json::json_details {
	using IteratorRange = NoCommentSkippingPolicyChecked;
}
/*
  template<typename ParsePolicy>
  struct IteratorRange {
    using policy = ParsePolicy;
    using iterator = typename ParsePolicy::iterator;
    static_assert( std::is_convertible_v<
                     typename std::iterator_traits<iterator>::iterator_category,
                     std::random_access_iterator_tag>,
                   "Expecting a Random Contiguous Iterator" );
    iterator first{ };
    iterator last{ };
    iterator class_first{ };
    iterator class_last{ };
    std::size_t counter = 0;
    using Range = IteratorRange<IteratorRange>;

    static inline constexpr bool is_unchecked_input =
      policy::is_unchecked_input;
    using CharT = typename policy::CharT;

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

    inline constexpr IteratorRange( ) = default;

    inline constexpr IteratorRange( iterator f, iterator l )
      : first( f )
      , last( l )
      , class_first( f )
      , class_last( l ) {}

    [[nodiscard]] inline constexpr bool empty( ) const {
      return parse_policy_details::empty( first, last );
    }

    [[nodiscard]] inline constexpr bool has_more( ) const {
      return parse_policy_details::has_more( first, last );
    }

    [[nodiscard]] inline constexpr bool can_parse_more( ) const {
      return parse_policy_details::can_parse_more( first, last );
    }

    [[nodiscard]] inline constexpr decltype( auto ) front( ) const {
      return *first;
    }

    [[nodiscard]] inline constexpr bool front( char c ) const {
      return first != last and *first == c;
    }

    [[nodiscard]] inline constexpr std::size_t size( ) const {
      return static_cast<std::size_t>( std::distance( first, last ) );
    }

    [[nodiscard]] inline constexpr bool is_number( ) const {
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
        result |= in( set[n] );
      }
      return result;
    }

    [[nodiscard]] inline constexpr bool is_null( ) const {
      return parse_policy_details::is_null( first );
    }

    inline constexpr void remove_prefix( ) {
      ++first;
    }

    inline constexpr void remove_prefix( std::size_t n ) {
      std::advance( first, static_cast<intmax_t>( n ) );
    }

    [[nodiscard]] inline constexpr bool is_space( ) const {
      return policy::is_space( first, last );
    }

    inline constexpr void trim_left( ) {
      policy::trim_left( first, last );
    }

    inline constexpr void trim_left_checked( ) {
      policy::trim_left_checked( first, last );
    }

    inline constexpr void trim_left_unchecked( ) {
      policy::trim_left_unchecked( first, last );
    }

    [[nodiscard]] inline constexpr iterator begin( ) const {
      return first;
    }

    [[nodiscard]] inline constexpr iterator data( ) const {
      return first;
    }

    [[nodiscard]] inline constexpr iterator end( ) const {
      return last;
    }

    [[nodiscard]] explicit inline constexpr operator bool( ) const {
      return not empty( );
    }

    inline constexpr void set_class_position( ) {
      class_first = first;
      class_last = last;
    }

    inline constexpr void move_to_next_of( char c ) {
      policy::move_to_next_of( first, last, c );
    }

    template<std::size_t N>
    inline constexpr void move_to_next_of( char const ( &str )[N] ) {
      policy::move_to_next_of( first, last, str );
    }

    [[nodiscard]] inline constexpr bool in( char c ) const noexcept {
      return parse_policy_details::in( *first, c );
    }

    template<std::size_t N>
    [[nodiscard]] inline constexpr bool
    in( char const ( &set )[N] ) const noexcept {
      return parse_policy_details::in( *first, set );
    }

    [[nodiscard]] inline constexpr bool at_end_of_item( ) const noexcept {
      return parse_policy_details::at_end_of_item( *first );
    }

    inline constexpr void clean_tail( ) {
      policy::clean_tail( first, last );
    }

    DAW_ATTRIBUTE_FLATTEN inline constexpr bool at_literal_end( ) const {
      return policy::at_literal_end( *first );
    }

    template<char Left, char Right>
    inline constexpr void skip_to_end_of_bracketed_item( ) {
      policy::template skip_to_end_of_bracketed_item<Left, Right>( first,
                                                                   last );
    }

    template<char Left, char Right>
    [[nodiscard]] inline constexpr IteratorRange skip_bracketed_item( ) {
      return policy::template skip_bracketed_item<Left, Right>( *this );
    }

    [[nodiscard]] inline constexpr bool is_real_number_part( ) const noexcept {
      return parse_policy_details::is_real_number_part( *first );
    }

  }; // namespace daw::json::json_details

  template<typename CharT>
  IteratorRange( CharT const *, CharT const * )
    -> IteratorRange<NoCommentSkippingPolicyChecked>;
 */
