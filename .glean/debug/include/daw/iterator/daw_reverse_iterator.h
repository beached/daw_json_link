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

#include "../daw_move.h"

namespace daw {
	template<typename Iterator>
	struct reverse_iterator {
		using value_type = typename std::iterator_traits<Iterator>::value_type;
		using difference_type =
		  typename std::iterator_traits<Iterator>::difference_type;
		using size_type = size_t;
		using pointer = typename std::iterator_traits<Iterator>::pointer;
		using reference = typename std::iterator_traits<Iterator>::reference;
		using iterator_category =
		  typename std::iterator_traits<Iterator>::iterator_category;

	private:
		Iterator m_base{};

	public:
		constexpr reverse_iterator( ) = default;

		constexpr explicit reverse_iterator( Iterator it )
		  : m_base( daw::move( it ) ) {}

		template<typename U>
		constexpr reverse_iterator( reverse_iterator<U> const &other )
		  : m_base( other.m_base ) {}

		template<typename U>
		constexpr reverse_iterator &operator=( reverse_iterator<U> const &other ) {
			m_base = other.m_base;
			return *this;
		}

		constexpr Iterator base( ) const {
			Iterator it = m_base;
			return it;
		}

		constexpr Iterator base( ) {
			Iterator it = m_base;
			return it;
		}

		constexpr auto operator*( ) -> decltype( *m_base ) {
			auto tmp = m_base;
			return *( --tmp );
		}

		constexpr auto operator*( ) const -> decltype( *m_base ) {
			auto tmp = m_base;
			return *( --tmp );
		}

		constexpr decltype( auto ) operator-> ( ) {
			return &( operator*( ) );
		}

		constexpr decltype( auto ) operator-> ( ) const {
			return &( operator*( ) );
		}

		constexpr decltype( auto ) operator[]( size_type n ) {
			return *( *this + static_cast<difference_type>( n ) );
		}

		constexpr decltype( auto ) operator[]( size_type n ) const {
			return *( *this + static_cast<difference_type>( n ) );
		}

		constexpr reverse_iterator &operator++( ) noexcept {
			--m_base;
			return *this;
		}

		constexpr reverse_iterator operator++( int ) noexcept {
			reverse_iterator tmp{*this};
			--m_base;
			return tmp;
		}

		constexpr reverse_iterator &operator--( ) noexcept {
			++m_base;
			return *this;
		}

		constexpr reverse_iterator operator--( int ) noexcept {
			reverse_iterator tmp{*this};
			++m_base;
			return tmp;
		}

		constexpr reverse_iterator operator+( difference_type n ) const {
			return reverse_iterator{m_base - n};
		}

		constexpr reverse_iterator operator-( difference_type n ) const {
			return reverse_iterator{m_base + n};
		}

		constexpr reverse_iterator &operator+=( difference_type n ) {
			m_base -= n;
			return *this;
		}

		constexpr reverse_iterator &operator-=( difference_type n ) {
			m_base += n;
			return *this;
		}
	}; // reverse_iterator

	template<typename Iterator>
	constexpr reverse_iterator<Iterator>
	operator+( typename reverse_iterator<Iterator>::difference_type n,
	           reverse_iterator<Iterator> const &it ) {

		return reverse_iterator<Iterator>{it.current - n};
	}

	template<typename Iterator>
	constexpr reverse_iterator<Iterator>
	operator-( typename reverse_iterator<Iterator>::difference_type n,
	           reverse_iterator<Iterator> const &it ) {

		return reverse_iterator<Iterator>{it.current + n};
	}

	template<class Iterator1, class Iterator2>
	constexpr auto operator-( reverse_iterator<Iterator1> const &lhs,
	                          reverse_iterator<Iterator2> const &rhs )
	  -> decltype( rhs.base( ) - lhs.base( ) ) {

		return rhs.base( ) - lhs.base( );
	}

	template<typename Iterator1, typename Iterator2>
	constexpr bool operator==( reverse_iterator<Iterator1> const &lhs,
	                           reverse_iterator<Iterator2> const &rhs ) {
		return lhs.base( ) == rhs.base( );
	}

	template<typename Iterator1, typename Iterator2>
	constexpr bool operator!=( reverse_iterator<Iterator1> const &lhs,
	                           reverse_iterator<Iterator2> const &rhs ) {
		return lhs.base( ) != rhs.base( );
	}

	template<typename Iterator1, typename Iterator2>
	constexpr bool operator<( reverse_iterator<Iterator1> const &lhs,
	                          reverse_iterator<Iterator2> const &rhs ) {
		return lhs.base( ) < rhs.base( );
	}

	template<typename Iterator1, typename Iterator2>
	constexpr bool operator<=( reverse_iterator<Iterator1> const &lhs,
	                           reverse_iterator<Iterator2> const &rhs ) {
		return lhs.base( ) <= rhs.base( );
	}

	template<typename Iterator1, typename Iterator2>
	constexpr bool operator>( reverse_iterator<Iterator1> const &lhs,
	                          reverse_iterator<Iterator2> const &rhs ) {
		return lhs.base( ) > rhs.base( );
	}

	template<typename Iterator1, typename Iterator2>
	constexpr bool operator>=( reverse_iterator<Iterator1> const &lhs,
	                           reverse_iterator<Iterator2> const &rhs ) {
		return lhs.base( ) >= rhs.base( );
	}

	template<typename Iterator>
	constexpr reverse_iterator<Iterator> make_reverse_iterator( Iterator i ) {
		return reverse_iterator<Iterator>{daw::move( i )};
	}
} // namespace daw
