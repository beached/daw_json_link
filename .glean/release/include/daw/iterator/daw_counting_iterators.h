// The MIT License (MIT)
//
// Copyright (c) 2019-2020 Darrell Wright
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

namespace daw {
	template<typename ForwardIterator>
	struct forward_counting_iterator {
		using value_type =
		  typename std::iterator_traits<ForwardIterator>::value_type;
		using pointer = typename std::iterator_traits<ForwardIterator>::pointer;
		using iterator_category = std::forward_iterator_tag;
		using reference = typename std::iterator_traits<ForwardIterator>::reference;
		using difference_type =
		  typename std::iterator_traits<ForwardIterator>::difference_type;

		ForwardIterator m_iter{};
		difference_type m_distance = 0;

		constexpr forward_counting_iterator( ) = default;

		explicit constexpr forward_counting_iterator( ForwardIterator const &fwd )
		  : m_iter( fwd ) {}

		explicit constexpr forward_counting_iterator( ForwardIterator &&fwd )
		  : m_iter( std::move( fwd ) ) {}

		constexpr decltype( auto ) operator*( ) {
			return *m_iter;
		}

		constexpr decltype( auto ) operator*( ) const {
			return *m_iter;
		}

		constexpr decltype( auto ) operator-> ( ) {
			return m_iter;
		}

		constexpr decltype( auto ) operator-> ( ) const {
			return m_iter;
		}

		constexpr forward_counting_iterator const &operator++( ) {
			++m_iter;
			++m_distance;
			return *this;
		}

		constexpr forward_counting_iterator operator++( int ) {
			auto result = *this;
			++m_iter;
			++m_distance;
			return result;
		}

		constexpr difference_type distance( ) const {
			return m_distance;
		}

		friend constexpr bool operator==( forward_counting_iterator const &lhs,
		                                  forward_counting_iterator const &rhs ) {
			return lhs.m_iter == rhs.m_iter;
		}

		friend constexpr bool operator!=( forward_counting_iterator const &lhs,
		                                  forward_counting_iterator const &rhs ) {
			return lhs.m_iter != rhs.m_iter;
		}

		friend constexpr bool operator==( forward_counting_iterator const &lhs,
		                                  ForwardIterator const &rhs ) {
			return lhs.m_iter == rhs;
		}

		friend constexpr bool operator!=( forward_counting_iterator const &lhs,
		                                  ForwardIterator const &rhs ) {
			return lhs.m_iter != rhs;
		}

		friend constexpr bool operator==( ForwardIterator const &lhs,
		                                  forward_counting_iterator const &rhs ) {
			return lhs == rhs.m_iter;
		}

		friend constexpr bool operator!=( ForwardIterator const &lhs,
		                                  forward_counting_iterator const &rhs ) {
			return lhs != rhs.m_iter;
		}
	};

	template<typename ForwardIterator>
	forward_counting_iterator( ForwardIterator )
	  ->forward_counting_iterator<ForwardIterator>;

	template<typename BiDirectionalIterator>
	struct bidirectional_counting_iterator {
		using value_type =
		  typename std::iterator_traits<BiDirectionalIterator>::value_type;
		using pointer =
		  typename std::iterator_traits<BiDirectionalIterator>::pointer;
		using iterator_category = std::bidirectional_iterator_tag;
		using reference =
		  typename std::iterator_traits<BiDirectionalIterator>::reference;
		using difference_type =
		  typename std::iterator_traits<BiDirectionalIterator>::difference_type;

		BiDirectionalIterator m_iter{};
		difference_type m_distance = 0;

		constexpr bidirectional_counting_iterator( ) = default;

		explicit constexpr bidirectional_counting_iterator(
		  BiDirectionalIterator const &bidir )
		  : m_iter( bidir ) {}

		explicit constexpr bidirectional_counting_iterator(
		  BiDirectionalIterator &&bidir )
		  : m_iter( std::move( bidir ) ) {}

		constexpr decltype( auto ) operator*( ) {
			return *m_iter;
		}

		constexpr decltype( auto ) operator*( ) const {
			return *m_iter;
		}

		constexpr decltype( auto ) operator-> ( ) {
			return m_iter;
		}

		constexpr decltype( auto ) operator-> ( ) const {
			return m_iter;
		}

		constexpr bidirectional_counting_iterator &operator++( ) {
			++m_iter;
			++m_distance;
			return *this;
		}

		constexpr bidirectional_counting_iterator operator++( int ) {
			auto result = *this;
			++m_iter;
			++m_distance;
			return result;
		}

		constexpr bidirectional_counting_iterator &operator--( ) {
			--m_iter;
			--m_distance;
			return *this;
		}

		constexpr bidirectional_counting_iterator operator--( int ) {
			auto result = *this;
			--m_iter;
			--m_distance;
			return result;
		}

		constexpr difference_type distance( ) const {
			return m_distance;
		}

		friend constexpr bool
		operator==( bidirectional_counting_iterator const &lhs,
		            bidirectional_counting_iterator const &rhs ) {
			return lhs.m_iter == rhs.m_iter;
		}

		friend constexpr bool
		operator!=( bidirectional_counting_iterator const &lhs,
		            bidirectional_counting_iterator const &rhs ) {
			return lhs.m_iter != rhs.m_iter;
		}

		friend constexpr bool
		operator==( bidirectional_counting_iterator const &lhs,
		            BiDirectionalIterator const &rhs ) {
			return lhs.m_iter == rhs;
		}

		friend constexpr bool
		operator!=( bidirectional_counting_iterator const &lhs,
		            BiDirectionalIterator const &rhs ) {
			return lhs.m_iter != rhs;
		}

		friend constexpr bool
		operator==( BiDirectionalIterator const &lhs,
		            bidirectional_counting_iterator const &rhs ) {
			return lhs == rhs.m_iter;
		}

		friend constexpr bool
		operator!=( BiDirectionalIterator const &lhs,
		            bidirectional_counting_iterator const &rhs ) {
			return lhs != rhs.m_iter;
		}
	};

	template<typename BiDirectionalIterator>
	bidirectional_counting_iterator( BiDirectionalIterator )
	  ->bidirectional_counting_iterator<BiDirectionalIterator>;
} // namespace daw
