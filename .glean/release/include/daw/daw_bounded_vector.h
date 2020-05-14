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

#include "cpp_17.h"
#include "daw_algorithm.h"
#include "daw_bounded_array.h"
#include "daw_math.h"
#include "daw_move.h"
#include "daw_swap.h"

#include <array>
#include <iterator>

namespace daw {
	template<typename T, size_t N>
	struct bounded_vector_t {
		static_assert( std::is_default_constructible_v<T>,
		               "T must be default constructible" );
		static_assert( std::is_trivially_destructible_v<T>,
		               "T must be trivially destructible" );
		using value_type = T;
		using reference = value_type &;
		using const_reference = value_type const &;
		using iterator = value_type *;
		using const_iterator = value_type const *;
		using reverse_iterator = std::reverse_iterator<iterator>;
		using const_reverse_iterator = std::reverse_iterator<const_iterator>;
		using pointer = value_type *;
		using const_pointer = value_type const *;
		using size_type = size_t;
		using difference_type = intmax_t;

	private:
		size_t m_index = 0;
		size_t m_first = 0;
		bounded_array_t<T, N> m_stack{};

	public:
		constexpr bounded_vector_t( ) noexcept = default;

		constexpr bounded_vector_t( const_pointer ptr, size_type count ) noexcept
		  : m_index{daw::min( count, N )} {

			daw::algorithm::copy_n( ptr, m_stack.begin( ), daw::min( count, N ) );
		}

		constexpr bool empty( ) const noexcept {
			return m_index == m_first;
		}

		constexpr bool full( ) const noexcept {
			return m_index - m_first == N;
		}

		constexpr size_type size( ) const noexcept {
			return m_index - m_first;
		}

		constexpr size_type capacity( ) const noexcept {
			return N;
		}

		constexpr bool has_room( size_type count ) noexcept {
			return count + size( ) >= N;
		}

		constexpr size_type available( ) const noexcept {
			return N - size( );
		}

		constexpr void clear( ) noexcept {
			m_index = 0;
			m_first = 0;
		}

		constexpr reference front( ) noexcept {
			return m_stack[m_first];
		}

		constexpr const_reference front( ) const noexcept {
			return m_stack[m_first];
		}

		constexpr reference back( ) noexcept {
			return m_stack[m_index - 1];
		}

		constexpr const_reference back( ) const noexcept {
			return m_stack[m_index - 1];
		}

		constexpr reference operator[]( size_type pos ) noexcept {
			return m_stack[m_first + pos];
		}

		constexpr const_reference operator[]( size_type pos ) const noexcept {
			return m_stack[m_first + pos];
		}

		constexpr reference at( size_type pos ) {
			daw::exception::precondition_check(
			  pos < size( ), "Attempt to access past end of fix_stack" );
			return m_stack[pos + m_first];
		}

		constexpr const_reference at( size_type pos ) const {
			daw::exception::precondition_check(
			  pos < size( ), "Attempt to access past end of fix_stack" );
			return m_stack[pos + m_first];
		}

		constexpr pointer data( ) noexcept {
			return m_stack.data( ) + m_first;
		}

		constexpr const_pointer data( ) const noexcept {
			return m_stack.data( ) + m_first;
		}

		constexpr iterator begin( ) noexcept {
			return m_stack.begin( ) + m_first;
		}

		constexpr const_iterator begin( ) const noexcept {
			return m_stack.begin( ) + m_first;
		}

		constexpr const_iterator cbegin( ) const noexcept {
			return m_stack.cbegin( ) + m_first;
		}

		constexpr reverse_iterator rbegin( ) noexcept {
			return reverse_iterator( end( ) );
		}

		constexpr const_reverse_iterator rbegin( ) const noexcept {
			return const_reverse_iterator( end( ) );
		}

		constexpr const_reverse_iterator crbegin( ) const noexcept {
			return const_reverse_iterator( cend( ) );
		}

		constexpr iterator end( ) noexcept {
			return &m_stack[m_index];
		}

		constexpr const_iterator end( ) const noexcept {
			return &m_stack[m_index];
		}

		constexpr const_iterator cend( ) const noexcept {
			return &m_stack[m_index];
		}

		constexpr reverse_iterator rend( ) noexcept {
			return reverse_iterator( begin( ) );
		}

		constexpr const_reverse_iterator rend( ) const noexcept {
			return const_reverse_iterator( begin( ) );
		}

		constexpr const_reverse_iterator crend( ) const noexcept {
			return const_reverse_iterator( cbegin( ) );
		}

	private:
		constexpr void do_move_to_front( ) noexcept {
			for( size_t n = m_first; n < m_index; ++n ) {
				m_stack[n - m_first] = m_stack[n];
			}
			m_index -= m_first;
			m_first = 0;
		}

		constexpr bool can_move_front( size_type how_many ) noexcept {
			return m_first > 0 and m_index >= N - how_many;
		}

	public:
		constexpr void push_back( const_reference value ) noexcept {
			if( can_move_front( 1 ) ) {
				do_move_to_front( );
			}
			m_stack[m_index++] = value;
		}

		constexpr void push_back( value_type &&value ) noexcept {
			if( can_move_front( 1 ) ) {
				do_move_to_front( );
			}
			m_stack[m_index++] = daw::move( value );
		}

		/**
		 * This gets around things like std::pair not having constexpr operator=
		 * @return position of pushed value
		 */
		constexpr size_t push_back( ) noexcept {
			if( can_move_front( 1 ) ) {
				do_move_to_front( );
			}
			return m_index++;
		}
		constexpr void push_back( const_pointer ptr, size_type sz ) noexcept {
			if( can_move_front( sz ) ) {
				do_move_to_front( );
			}
			auto const start = m_index;
			m_index += sz;
			for( size_t n = start; n < m_index; ++n ) {
				m_stack[n] = *ptr++;
			}
		}

		constexpr void assign( size_type count, const_reference value ) noexcept {
			clear( );
			for( size_t n = 0; n < count; ++n ) {
				push_back( value );
			}
		}

		template<typename Ptr>
		constexpr void push_back( Ptr const *ptr, size_type sz ) noexcept {
			if( can_move_front( sz ) ) {
				do_move_to_front( );
			}
			auto const start = m_index;
			m_index += sz;
			for( size_t n = start; n < m_index; ++n ) {
				m_stack[n] = static_cast<value_type>( *ptr++ );
			}
		}

		template<typename... Args>
		constexpr void emplace_back( Args &&... args ) noexcept {
			if( can_move_front( sizeof...( Args ) ) ) {
				do_move_to_front( );
			}
			m_stack[m_index++] =
			  daw::construct_a<value_type>( std::forward<Args>( args )... );
		}

		constexpr value_type pop_back( ) noexcept {
			return m_stack[--m_index];
		}

		constexpr void pop_front( size_type const count ) {
			daw::exception::precondition_check<std::out_of_range>(
			  m_index <= m_index - count, "Attempt to pop_front past end of stack" );

			m_index += count;
		}

		///	take care calling as it is slow
		constexpr value_type pop_front( ) {
			auto result = front( );
			pop_front( 1 );
			return result;
		}

		constexpr void resize( size_type const count ) {

			daw::exception::precondition_check<std::out_of_range>(
			  count <= capacity( ), "Attempt to resize past capacity of fix_stack" );

			if( count > size( ) ) {
				if( can_move_front( count ) ) {
					do_move_to_front( );
				}
				for( size_type n = size( ); n < count; ++n ) {
					m_stack[n] = value_type{};
				}
			}
			m_index = count;
		}

		constexpr iterator erase( const_iterator pos ) {
			auto first = std::next( begin( ), std::distance( cbegin( ), pos ) );
			auto last = std::next( first, 1 );
			while( last != end( ) ) {
				daw::iter_swap( first, last );
				std::advance( first, 1 );
				std::advance( last, 1 );
			}
			resize( size( ) - 1 );
			if( pos == cend( ) ) {
				return end( );
			}
			return std::next( begin( ), std::distance( cbegin( ), pos ) + 1 );
		}

		constexpr iterator erase( const_iterator first, const_iterator last ) {
			auto first1 = std::next( begin( ), std::distance( cbegin( ), first ) );
			auto first2 = std::next( begin( ), std::distance( cbegin( ), last ) );
			while( first2 != end( ) ) {
				daw::iter_swap( first1, first2 );
				std::advance( first1, 1 );
				std::advance( first2, 1 );
			}
			auto const dist = std::distance( first, last );

			daw::exception::dbg_precondition_check<std::out_of_range>(
			  static_cast<difference_type>( size( ) ) > dist and dist >= 0,
			  "Attempt to resize past capacity of fix_stack" );

			resize( size( ) - static_cast<size_type>( dist ) );
			if( last == cend( ) ) {
				return end( );
			}
			return std::next( begin( ), std::distance( cbegin( ), last ) + 1 );
		}

		constexpr void zero( ) noexcept {
			for( size_t n = 0; n < N; ++n ) {
				m_stack[n] = 0;
			}
		}

		constexpr void swap( bounded_vector_t &rhs ) {
			daw::cswap( m_index, rhs.m_index );
			daw::cswap( m_first, rhs.m_first );
			for( size_t n = 0; n < size( ); ++n ) {
				daw::cswap( operator[]( n ), rhs[n] );
			}
		}
	};
} // namespace daw
