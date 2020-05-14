// The MIT License (MIT)
//
// Copyright (c) 2018-2020 Darrell Wright
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

#include <stdexcept>
#include <string_view>

#include "../daw_exception.h"
#include "../daw_traits.h"

namespace daw {
	class arg_iterator_t {
		int m_argc;
		int m_pos = 0;
		char const *const *m_argv;

	public:
		using iterator_category = std::random_access_iterator_tag;
		using pointer = std::string_view *;
		using difference_type = std::ptrdiff_t;
		using value_type = std::string_view;
		using reference = std::string_view;

		constexpr arg_iterator_t( ) noexcept
		  : m_argc( 0 )
		  , m_argv( nullptr ) {}

		constexpr arg_iterator_t( int argc, char const *const *argv ) noexcept
		  : m_argc( argc )
		  , m_argv( argv ) {}

		constexpr std::string_view operator*( ) const noexcept {
			return std::string_view( m_argv[m_pos] );
		}

		constexpr arg_iterator_t &operator++( ) noexcept {
			++m_pos;
			return *this;
		}

		constexpr arg_iterator_t &operator--( ) noexcept {
			--m_pos;
			return *this;
		}

		constexpr arg_iterator_t operator++( int ) noexcept {
			auto result = arg_iterator_t( *this );
			++m_pos;
			return result;
		}

		constexpr arg_iterator_t operator--( int ) noexcept {
			auto result = arg_iterator_t( *this );
			--m_pos;
			return result;
		}

		constexpr arg_iterator_t operator+( difference_type n ) const noexcept {
			auto result = arg_iterator_t( *this );
			result.m_pos += n;
			return result;
		}

		constexpr arg_iterator_t &operator+=( difference_type n ) noexcept {
			m_pos += n;
			return *this;
		}

		constexpr arg_iterator_t operator-( difference_type n ) const noexcept {
			auto result = arg_iterator_t( *this );
			result.m_pos -= n;
			return result;
		}

		constexpr arg_iterator_t &operator-=( difference_type n ) noexcept {
			m_pos -= n;
			return *this;
		}

		constexpr difference_type operator-( arg_iterator_t const &rhs ) const
		  noexcept {
			return m_pos - rhs.m_pos;
		}

		constexpr difference_type operator+( arg_iterator_t const &rhs ) const
		  noexcept {
			return m_pos + rhs.m_pos;
		}

		constexpr std::string_view operator[]( difference_type n ) const noexcept {
			return std::string_view( m_argv[m_pos + n] );
		}

		constexpr std::string_view at( difference_type n ) const {
			auto const pos = m_pos + n;

			daw::exception::precondition_check<std::out_of_range>(
			  0 < pos and pos < m_argc, "Attempt to access invalid argument" );

			return std::string_view( m_argv[pos] );
		}

		constexpr bool at_end( ) const noexcept {
			return m_argv == nullptr or m_pos >= m_argc;
		}

		constexpr bool operator==( arg_iterator_t const &rhs ) const noexcept {
			if( m_argv == rhs.m_argv ) {
				return m_pos == rhs.m_pos;
			}
			return at_end( ) and rhs.at_end( );
		}

		constexpr bool operator!=( arg_iterator_t const &rhs ) const noexcept {
			if( m_argv == rhs.m_argv ) {
				return m_pos != rhs.m_pos;
			}
			return !( at_end( ) and rhs.at_end( ) );
		}

		constexpr bool operator<( arg_iterator_t const &rhs ) const noexcept {
			if( m_argv == rhs.m_argv ) {
				return m_argc < rhs.m_argc;
			}
			return !at_end( ) and rhs.at_end( );
		}

		constexpr bool operator<=( arg_iterator_t const &rhs ) const noexcept {
			if( m_argv == rhs.m_argv ) {
				return m_argc <= rhs.m_argc;
			}
			return rhs.at_end( );
		}

		constexpr bool operator>( arg_iterator_t const &rhs ) const noexcept {
			if( m_argv == rhs.m_argv ) {
				return m_argc > rhs.m_argc;
			}
			return at_end( );
		}

		constexpr bool operator>=( arg_iterator_t const &rhs ) const noexcept {
			if( m_argv == rhs.m_argv ) {
				return m_argc <= rhs.m_argc;
			}
			return at_end( );
		}

		constexpr size_t size( ) const noexcept {
			return static_cast<size_t>( m_argc );
		}

		constexpr difference_type position( ) const noexcept {
			return static_cast<difference_type>( m_pos );
		}

		constexpr arg_iterator_t begin( ) const noexcept {
			return *this;
		}

		static constexpr arg_iterator_t end( ) noexcept {
			return arg_iterator_t( );
		}

		constexpr operator bool( ) const noexcept {
			return 0 <= m_pos and m_pos < m_argc;
		}
	};
} // namespace daw
