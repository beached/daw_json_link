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

#ifdef DAW_ALLOW_COMMENTS
#define skip_comments_checked( )                                               \
	if( front( '#' ) ) {                                                         \
		remove_prefix( );                                                          \
		while( has_more( ) and front( ) != '\n' ) {                                \
			remove_prefix( );                                                        \
		}                                                                          \
		remove_prefix( );                                                          \
	}                                                                            \
	while( false )

#define skip_comments_unchecked( )                                             \
	if( front( ) == '#' ) {                                                      \
		remove_prefix( );                                                          \
		while( front( ) != '\n' ) {                                                \
			remove_prefix( );                                                        \
		}                                                                          \
		remove_prefix( );                                                          \
	}                                                                            \
	while( false )
#define skip_comments( )                                                       \
	if constexpr( IsUncheckedInput ) {                                           \
		if( front( ) == '#' ) {                                                    \
			remove_prefix( );                                                        \
			while( front( ) != '\n' ) {                                              \
				remove_prefix( );                                                      \
			}                                                                        \
			remove_prefix( );                                                        \
		}                                                                          \
	} else {                                                                     \
		if( front( '#' ) ) {                                                       \
			remove_prefix( );                                                        \
			while( has_more( ) and front( ) != '\n' ) {                              \
				remove_prefix( );                                                      \
			}                                                                        \
			remove_prefix( );                                                        \
		}                                                                          \
	}                                                                            \
	while( false )
#else
#define skip_comments( )                                                       \
	do {                                                                         \
	} while( false )
#define skip_comments_checked( )                                               \
	do {                                                                         \
	} while( false )
#define skip_comments_unchecked( )                                             \
	do {                                                                         \
	} while( false )
#endif

namespace daw::json::json_details {
	template<typename Iterator, bool IsUnCheckedInput>
	struct IteratorRange {
		static_assert( std::is_convertible_v<
		                 typename std::iterator_traits<Iterator>::iterator_category,
		                 std::random_access_iterator_tag>,
		               "Expecting a Random Contiguous Iterator" );
		using iterator = Iterator;
		iterator first{ };
		iterator last{ };
		iterator class_first{ };
		iterator class_last{ };
		using Range = IteratorRange<iterator, IsUnCheckedInput>;

		static inline constexpr bool is_unchecked_input = IsUnCheckedInput;
		using CharT = daw::remove_cvref_t<decltype( *first )>;

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

		[[nodiscard]] DAW_ATTRIBUTE_FLATTEN inline constexpr bool empty( ) const {
			return first == last;
		}

		[[nodiscard]] DAW_ATTRIBUTE_FLATTEN inline constexpr bool
		has_more( ) const {
			return first != last;
		}

		[[nodiscard]] DAW_ATTRIBUTE_FLATTEN inline constexpr bool
		can_parse_more( ) const {
			return not is_null( ) and has_more( );
		}

		[[nodiscard]] DAW_ATTRIBUTE_FLATTEN inline constexpr decltype( auto )
		front( ) const {
			return *first;
		}

		[[nodiscard]] DAW_ATTRIBUTE_FLATTEN inline constexpr bool
		front( char c ) const {
			return first != last and *first == c;
		}

		[[nodiscard]] DAW_ATTRIBUTE_FLATTEN inline constexpr std::size_t
		size( ) const {
			return static_cast<std::size_t>( std::distance( first, last ) );
		}

		[[nodiscard]] DAW_ATTRIBUTE_FLATTEN inline constexpr bool
		is_number( ) const {
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

		[[nodiscard]] DAW_ATTRIBUTE_FLATTEN inline constexpr bool is_null( ) const {
			if constexpr( std::is_pointer_v<iterator> ) {
				return first == nullptr;
			} else if constexpr( std::is_convertible_v<iterator, bool> ) {
				return not static_cast<bool>( first );
			} else {
				return false;
			}
		}

		DAW_ATTRIBUTE_FLATTEN inline constexpr void remove_prefix( ) {
			++first;
		}

		DAW_ATTRIBUTE_FLATTEN inline constexpr void remove_prefix( std::size_t n ) {
			std::advance( first, static_cast<intmax_t>( n ) );
		}

		[[nodiscard]] DAW_ATTRIBUTE_FLATTEN inline constexpr bool
		is_space( ) const {
			// Faster to be liberal here and accept <= 0x20
			daw_json_assert_weak( has_more( ), "Unexpected end of stream" );
			auto const c = *first;
			return c > 0x0 and c <= 0x20;
		}

		inline constexpr void trim_left( ) {
			if constexpr( is_unchecked_input ) {
				trim_left_unchecked( );
			} else {
				trim_left_checked( );
			}
		}

		inline constexpr void trim_left_checked( ) {
			skip_comments_checked( );
			while( has_more( ) and is_space( ) ) {
				remove_prefix( );
				skip_comments_checked( );
			}
		}

		inline constexpr void trim_left_unchecked( ) {
			skip_comments_unchecked( );
			while( is_space( ) ) {
				remove_prefix( );
				daw_json_assert_weak( first != last, "Unexpected end of stream" );
			}
		}

		inline constexpr void trim_left_checked_raw( ) {
			while( is_space( ) ) {
				remove_prefix( );
				daw_json_assert_weak( first != last, "Unexpected end of stream" );
			}
		}

		[[nodiscard]] DAW_ATTRIBUTE_FLATTEN inline constexpr iterator
		begin( ) const {
			return first;
		}

		[[nodiscard]] DAW_ATTRIBUTE_FLATTEN inline constexpr iterator
		data( ) const {
			return first;
		}

		[[nodiscard]] DAW_ATTRIBUTE_FLATTEN inline constexpr iterator end( ) const {
			return last;
		}

		[[nodiscard]] DAW_ATTRIBUTE_FLATTEN explicit inline constexpr
		operator bool( ) const {
			return not empty( );
		}

		inline constexpr void move_to_next_of( char c ) {
			skip_comments( );
			daw_json_assert_weak( has_more( ), "Unexpected end of data" );
			while( front( ) != c ) {
				daw_json_assert_weak( has_more( ), "Unexpected end of data" );
				remove_prefix( );
				skip_comments( );
			}
		}

		template<std::size_t N>
		inline constexpr void move_to_next_of( char const ( &str )[N] ) {
			skip_comments( );
			daw_json_assert_weak( has_more( ), "Unexpected end of data" );
			while( not in( str ) ) {
				daw_json_assert_weak( has_more( ), "Unexpected end of data" );
				remove_prefix( );
				skip_comments( );
			}
		}

		[[nodiscard]] DAW_ATTRIBUTE_FLATTEN inline constexpr bool
		in( char c ) const {
			daw_json_assert_weak( first != nullptr, "Empty or null InteratorRange" );
			return *first == c;
		}

		template<std::size_t N>
		[[nodiscard]] inline constexpr bool in( char const ( &set )[N] ) const {
			unsigned result = 0;
			daw::algorithm::do_n_arg<N>( [&]( std::size_t n ) {
				result |= static_cast<unsigned>( set[n] == *first );
			} );
			return static_cast<bool>( result );
		}

		[[nodiscard]] inline constexpr bool is_real_number_part( ) const {
			if constexpr( IsUnCheckedInput ) {
				return true;
			} else {
				auto const c = *first;
				auto const b0 = static_cast<unsigned>(
				  ( static_cast<unsigned>( c ) - static_cast<unsigned>( '0' ) ) < 10U );
				auto const b1 =
				  static_cast<unsigned>( c == 'e' ) | static_cast<unsigned>( c == 'E' );
				auto const b2 =
				  static_cast<unsigned>( c == '+' ) | static_cast<unsigned>( c == '-' );
				auto result = static_cast<bool>( b0 | b1 | b2 );
				return result;
			}
		}

		[[nodiscard]] inline constexpr bool at_end_of_item( ) const {
			auto const c = front( );
			return static_cast<bool>(
			  static_cast<unsigned>( c == ',' ) | static_cast<unsigned>( c == '}' ) |
			  static_cast<unsigned>( c == ']' ) | static_cast<unsigned>( c == ':' ) |
			  static_cast<unsigned>( c <= 0x20 ) );
		}

		DAW_ATTRIBUTE_FLATTEN inline constexpr void clean_tail( ) {
			// trim_left_checked
			while( has_more( ) and is_space( ) ) {
				remove_prefix( );
			}
			if( front( ) == ',' ) {
				remove_prefix( );
				trim_left( );
			}
		}

		DAW_ATTRIBUTE_FLATTEN inline constexpr void set_class_position( ) {
			class_first = first;
			class_last = last;
		}
	}; // namespace daw::json::json_details

	template<typename CharT>
	IteratorRange( CharT const *, CharT const * )
	  -> IteratorRange<CharT const *, false>;
} // namespace daw::json::json_details
