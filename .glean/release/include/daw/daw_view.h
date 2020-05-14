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

#include "daw_exception.h"
#include "daw_move.h"

#include <iterator>
#include <stdexcept>
#include <type_traits>

namespace daw::view_impl {
	template<typename T>
	struct const_iterator_test {
		using type = T;
	};

	template<typename T>
	struct const_iterator_test<T const *> {
		using type = T const *;
	};

	template<typename T>
	struct const_iterator_test<T *> {
		using type = T const *;
	};

	template<typename T>
	using get_const_iterator_t = typename const_iterator_test<T>::type;
} // namespace daw::view_impl

namespace daw {
	template<typename BidirectionalIterator>
	struct [[nodiscard]] view {
		using value_type =
		  typename std::iterator_traits<BidirectionalIterator>::value_type;

		using pointer = value_type *;
		using const_pointer = value_type const *;
		using reference =
		  typename std::iterator_traits<BidirectionalIterator>::reference;

		using const_reference = value_type const &;
		using iterator = BidirectionalIterator;
		using const_iterator = view_impl::get_const_iterator_t<iterator>;
		using reverse_iterator = std::reverse_iterator<iterator>;
		using const_reverse_iterator = std::reverse_iterator<const_iterator>;
		using size_type = size_t;
		using difference_type = std::ptrdiff_t;

	private:
		BidirectionalIterator m_first;
		BidirectionalIterator m_last;
		difference_type m_size;

	public:
		constexpr view( BidirectionalIterator first, BidirectionalIterator last )
		  : m_first( daw::move( first ) )
		  , m_last( daw::move( last ) )
		  , m_size( std::distance( m_first, m_last ) ) {}

		[[nodiscard]] constexpr bool empty( ) const {
			return m_size <= 0;
		}

		[[nodiscard]] constexpr size_type size( ) const {
			if( empty( ) ) {
				return 0;
			}
			return static_cast<size_type>( m_size );
		}

		[[nodiscard]] constexpr difference_type ssize( ) const {
			return m_size;
		}

		[[nodiscard]] constexpr iterator data( ) {
			return m_first;
		}

		[[nodiscard]] constexpr const_iterator data( ) const {
			return m_first;
		}

		// Iterator access
		[[nodiscard]] constexpr iterator begin( ) {
			return m_first;
		}

		[[nodiscard]] constexpr const_iterator begin( ) const {
			return m_first;
		}

		[[nodiscard]] constexpr const_iterator cbegin( ) const {
			return m_first;
		}

		[[nodiscard]] constexpr reverse_iterator rbegin( ) {
			return std::make_reverse_iterator( end( ) );
		}

		[[nodiscard]] constexpr const_reverse_iterator rbegin( ) const {
			return std::make_reverse_iterator( end( ) );
		}

		[[nodiscard]] constexpr const_reverse_iterator crbegin( ) const {
			return std::make_reverse_iterator( cend( ) );
		}

		[[nodiscard]] constexpr iterator end( ) {
			return m_last;
		}

		[[nodiscard]] constexpr const_iterator end( ) const {
			return m_last;
		}

		[[nodiscard]] constexpr const_iterator cend( ) const {
			return m_last;
		}

		[[nodiscard]] constexpr reverse_iterator rend( ) {
			return std::make_reverse_iterator( begin( ) );
		}

		[[nodiscard]] constexpr const_reverse_iterator rend( ) const {
			return std::make_reverse_iterator( begin( ) );
		}

		[[nodiscard]] constexpr const_reverse_iterator crend( ) const {
			return std::make_reverse_iterator( cbegin( ) );
		}

		// Data access
		[[nodiscard]] constexpr reference front( ) {
			return *m_first;
		}

		[[nodiscard]] constexpr const_reference front( ) const {
			return *m_first;
		}

		[[nodiscard]] constexpr reference back( ) {
			return *std::prev( m_last );
		}

		[[nodiscard]] constexpr const_reference back( ) const {
			return *std::prev( m_last );
		}

		[[nodiscard]] constexpr reference operator[]( size_type index ) {
			return *std::next( m_first, index );
		}

		[[nodiscard]] constexpr const_reference operator[]( size_type index )
		  const {
			return *std::next( m_first, index );
		}

		[[nodiscard]] constexpr reference at( size_type index ) {
			daw::exception::precondition_check<std::out_of_range>( index < size( ) );
			return operator[]( index );
		}

		[[nodiscard]] constexpr const_reference at( size_type index ) const {
			daw::exception::precondition_check<std::out_of_range>( index < size( ) );
			return operator[]( index );
		}

		constexpr void remove_prefix( size_type n ) {
			m_size -= static_cast<difference_type>( n );
			std::advance( m_first, static_cast<difference_type>( n ) );
		}

		constexpr void remove_prefix( ) {
			remove_prefix( 1U );
		}

		constexpr void remove_suffix( size_type n ) {
			m_size -= static_cast<difference_type>( n );
			std::advance( m_last, -static_cast<difference_type>( n ) );
		}

		constexpr void remove_suffix( ) {
			remove_suffix( 1 );
		}

		[[nodiscard]] constexpr view pop_front( size_t n ) {
			if( n > size( ) ) {
				n = size( );
			}
			auto result = view( m_first, std::next( m_first, n ) );
			remove_prefix( n );
			return result;
		}

		[[nodiscard]] constexpr value_type pop_front( ) {
			auto result = *m_first;
			remove_prefix( );
			return result;
		}

		[[nodiscard]] constexpr view pop_back( size_t n ) {
			if( n > size( ) ) {
				n = size( );
			}
			auto result = view( std::prev( m_last, n ), m_last );
			remove_suffix( n );
			return result;
		}

		[[nodiscard]] constexpr value_type pop_back( ) {
			auto result = back( );
			remove_suffix( );
			return result;
		}

		[[nodiscard]] constexpr bool pop( value_type & result ) {
			if( empty( ) ) {
				return false;
			}
			result = front( );
			remove_prefix( );
			return true;
		}
	};

	template<typename BidirectionalIterator>
	view( BidirectionalIterator, BidirectionalIterator )
	  ->view<BidirectionalIterator>;

	template<typename>
	struct is_daw_view_t : std::false_type {};

	template<typename T>
	struct is_daw_view_t<daw::view<T>> : std::true_type {};
} // namespace daw
