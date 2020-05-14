// The MIT License (MIT)
//
// Copyright (c) 2017-2020 Darrell Wright
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
#include <limits>

namespace daw {
	template<typename CharT = char>
	struct repeat_n_char_iterator {
		using value_type = CharT;
		using difference_type = ptrdiff_t;
		using pointer = value_type const *;
		using const_pointer = value_type const *;
		using reference = value_type const &;
		using const_reference = value_type const &;
		using iterator_category = std::random_access_iterator_tag;

		value_type m_value = 0;
		ptrdiff_t m_position = 0;

		constexpr repeat_n_char_iterator( ) noexcept = default;

		constexpr repeat_n_char_iterator( size_t count, value_type value ) noexcept
		  : m_value( value )
		  , m_position( static_cast<ptrdiff_t>( count ) ) {}

		constexpr reference operator*( ) const noexcept {
			return m_value;
		}

		constexpr pointer operator->( ) const noexcept {
			return &m_value;
		}

		constexpr repeat_n_char_iterator &operator++( ) noexcept {
			--m_position;
			return *this;
		}

		constexpr repeat_n_char_iterator operator++( int ) noexcept {
			auto tmp = *this;
			--m_position;
			return tmp;
		}

		constexpr repeat_n_char_iterator &operator--( ) noexcept {
			++m_position;
			return *this;
		}

		constexpr repeat_n_char_iterator operator--( int ) noexcept {
			auto tmp = *this;
			++m_position;
			return tmp;
		}

		constexpr reference operator[]( size_t ) const noexcept {
			return m_value;
		}

		constexpr bool equal( repeat_n_char_iterator const &rhs ) const noexcept {
			return m_position == rhs.m_position;
		}

		constexpr repeat_n_char_iterator const &operator+=( ptrdiff_t p ) noexcept {
			m_position += p;
			return *this;
		}

		constexpr repeat_n_char_iterator const &operator-=( ptrdiff_t p ) noexcept {
			m_position -= p;
			return *this;
		}

		constexpr repeat_n_char_iterator operator+( ptrdiff_t p ) const noexcept {
			auto tmp = *this;
			tmp += p;
			return tmp;
		}

		constexpr repeat_n_char_iterator operator-( ptrdiff_t p ) const noexcept {
			auto tmp = *this;
			tmp -= p;
			return tmp;
		}

		constexpr ptrdiff_t operator-( repeat_n_char_iterator const &rhs ) const
		  noexcept {
			return rhs.m_position - m_position;
		}
	};
	template<typename CharT>
	repeat_n_char_iterator( size_t, CharT )->repeat_n_char_iterator<CharT>;

	template<typename T = char>
	constexpr auto repeat_n_char_end( ) noexcept {
		return repeat_n_char_iterator<T>( );
	}

	template<typename CharT>
	constexpr bool
	operator==( repeat_n_char_iterator<CharT> const &lhs,
	            repeat_n_char_iterator<CharT> const &rhs ) noexcept {

		return lhs.equal( rhs );
	}

	template<typename CharT>
	constexpr bool
	operator!=( repeat_n_char_iterator<CharT> const &lhs,
	            repeat_n_char_iterator<CharT> const &rhs ) noexcept {

		return !lhs.equal( rhs );
	}
} // namespace daw
