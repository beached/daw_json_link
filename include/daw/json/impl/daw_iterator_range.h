// The MIT License( MIT )
//
// Copyright (c) 2019 Darrell Wright
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files( the "Software" ), to
// deal in the Software without restriction, including without limitation the
// rights to use, copy, modify, merge, publish, distribute, sublicense, and / or
// sell copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#pragma once

#include <iterator>
#include <type_traits>

#include <daw/daw_parser_helper_sv.h>
#include <daw/daw_string_view.h>

#include "daw_json_assert.h"

namespace daw::json::impl {
	template<typename First, typename Last>
	struct IteratorRange {
		First first{};
		Last last{};
#ifndef NDEBUG
		size_t pos = 0;
#endif

		constexpr IteratorRange( ) noexcept = default;

		constexpr IteratorRange( First f, Last l ) noexcept
		  : first( f )
		  , last( l ) {}

		[[nodiscard]] constexpr bool empty( ) const noexcept {
			return first == last;
		}

		[[nodiscard]] constexpr bool has_more( ) const noexcept {
			return first != last;
		}

		[[nodiscard]] constexpr decltype( auto ) front( ) const noexcept {
			return *first;
		}

		[[nodiscard]] constexpr bool front( char c ) const noexcept {
			return not empty( ) and in( c );
		}

		[[nodiscard]] constexpr size_t size( ) const noexcept {
			return static_cast<size_t>( std::distance( first, last ) );
		}

		template<size_t N>
		[[nodiscard]] constexpr bool front( char const ( &set )[N] ) const
		  noexcept {
			if( empty( ) ) {
				return false;
			}
			bool result = false;
			for( size_t n = 0; n < ( N - 1 ); ++n ) {
				result |= in( set[n] );
			}
			return result;
		}

		[[nodiscard]] constexpr bool is_null( ) const noexcept {
			if constexpr( std::is_pointer_v<First> ) {
				return first == nullptr;
			} else {
				return false;
			}
		}

		constexpr void remove_prefix( size_t n = 1 ) {
			first = std::next( first, static_cast<intmax_t>( n ) );
#ifndef NDEBUG
			pos += n;
#endif
		}

		template<typename Char>
		static bool constexpr is_space( Char c ) noexcept {
			switch( c ) {
			case 0x20: // space
			case 0x09: // tab
			case 0x0A: // new line
			case 0x0D: // carriage return
				return true;
			}
			return false;
		}

		constexpr void trim_left( ) noexcept {
			while( first != last && is_space( *first ) ) {
				++first;
#ifndef NDEBUG
				++pos;
#endif
			}
		}

		constexpr void trim_left_no_check( ) noexcept {
			while( is_space( *first ) ) {
				++first;
				json_assert( first != last, "Unexpected end of stream" );
#ifndef NDEBUG
				++pos;
#endif
			}
		}

		[[nodiscard]] constexpr decltype( auto ) begin( ) const noexcept {
			return first;
		}

		[[nodiscard]] constexpr decltype( auto ) end( ) const noexcept {
			return last;
		}

		[[nodiscard]] explicit constexpr operator bool( ) const noexcept {
			return not empty( );
		}

		[[nodiscard]] constexpr daw::string_view
		move_to_next_of( char c ) noexcept {
			auto p = begin( );
			size_t sz = 0;
			while( not in( c ) ) {
				remove_prefix( );
				++sz;
			}
			return {p, sz};
		}

		[[nodiscard]] constexpr IteratorRange
		move_to_first_of( daw::string_view const chars ) noexcept {
			auto result = *this;
			while( chars.find( front( ) ) == daw::string_view::npos ) {
				remove_prefix( );
			}
			result.last = first;
			return result;
		}

		[[nodiscard]] constexpr bool in( char c ) const noexcept {
			json_assert( first != nullptr, "Empty or null InteratorRange" );
			return *first == c;
		}

		template<size_t N>
		[[nodiscard]] constexpr bool in( char const ( &set )[N] ) const noexcept {
			bool result = false;
			for( size_t n = 0; n < ( N - 1 ); ++n ) {
				result |= ( set[n] == *first );
			}
			return result;
		}

		[[nodiscard]] constexpr bool is_real_number_part( ) const noexcept {
			return in( "0123456789eE+-" );
		}

		[[nodiscard]] constexpr daw::string_view munch( char c ) noexcept {
			auto result = move_to_next_of( c );
			if( in( c ) ) {
				remove_prefix( );
			}
			return result;
		}

		[[nodiscard]] constexpr bool at_end_of_item( ) const noexcept {
			return in( ",}]" ) or daw::parser::is_unicode_whitespace( front( ) );
		}

		constexpr void clean_tail( ) noexcept {
			trim_left( );
			if( in( ',' ) ) {
				remove_prefix( );
				trim_left( );
			}
		}
	};

	template<typename First, typename Last>
	IteratorRange( First, Last ) -> IteratorRange<First, Last>;
} // namespace daw::json::impl
