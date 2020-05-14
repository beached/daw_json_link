// The MIT License (MIT)
//
// Copyright (c) 2013-2020 Darrell Wright
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

#include <algorithm>
#include <stdexcept>
#include <utility>

#include "daw_exception.h"
#include "daw_exchange.h"
#include "daw_swap.h"

namespace daw {
	template<typename T>
	struct heap_array {
		using value_type = T;
		using reference = T &;
		using const_reference = T const &;
		using iterator = T *;
		using const_iterator = T const *;

	private:
		value_type *m_begin = nullptr;
		value_type *m_end = nullptr;
		size_t m_size = 0;

		static value_type *create_value( size_t n ) {
			try {
				return new value_type[n];
			} catch( std::bad_alloc const & ) { std::abort( ); } catch( ... ) {
				return nullptr;
			}
		}

	public:
		constexpr heap_array( ) noexcept = default;

		heap_array( size_t Size )
		  : m_begin( create_value( Size + 1 ) )
		  , m_end( m_begin + Size )
		  , m_size( Size ) {}

		heap_array( size_t Size, value_type const &def_value )
		  : heap_array( Size ) {

			std::fill( m_begin, m_end, def_value );
		}

		constexpr heap_array( heap_array &&other ) noexcept
		  : m_begin( daw::exchange( other.m_begin, nullptr ) )
		  , m_end( daw::exchange( other.m_begin, nullptr ) )
		  , m_size( daw::exchange( other.m_size, 0ULL ) ) {}

		constexpr heap_array &operator=( heap_array &&rhs ) noexcept {
			m_begin = daw::exchange( rhs.m_begin, nullptr );
			m_end = daw::exchange( rhs.m_end, nullptr );
			m_size = daw::exchange( rhs.m_size, 0ULL );
			return *this;
		}

		heap_array( heap_array const &other )
		  : m_begin( other.m_size == 0 ? nullptr : create_value( other.m_size ) )
		  , m_end( other.m_size == 0 ? nullptr : m_begin + other.m_size )
		  , m_size( other.m_size ) {

			std::copy_n( other.m_begin, m_size, m_begin );
		}

		heap_array &operator=( heap_array const &rhs ) {
			if( this != &rhs ) {
				clear( );
				if( rhs.m_begin == nullptr ) {
					return *this;
				}
				m_size = rhs.m_size;
				m_begin = create_value( m_size );
				m_end = m_begin + m_size;
				std::copy_n( rhs.m_begin, m_size, m_begin );
			}
			return *this;
		}

		heap_array &operator=( std::initializer_list<value_type> const &values ) {
			heap_array tmp( values.size( ) );
			std::copy_n( values.begin( ), values.size( ), tmp.m_begin );
			daw::cswap( *this, tmp );
			return *this;
		}

		heap_array( iterator arry, size_t Size )
		  : m_begin( create_value( Size ) )
		  , m_end( m_begin + Size )
		  , m_size( Size ) {

			std::copy_n( arry, Size, m_begin );
		}

		constexpr void clear( ) noexcept {
			auto tmp = daw::exchange( m_begin, nullptr );
			m_size = 0;
			m_end = nullptr;
			delete[] tmp;
		}

		constexpr void swap( heap_array &rhs ) noexcept {
			daw::cswap( m_begin, rhs.m_begin );
			daw::cswap( m_end, rhs.m_end );
			daw::cswap( m_size, rhs.m_size );
		}

		~heap_array( ) {
			clear( );
		}

		explicit constexpr operator bool( ) const noexcept {
			return nullptr == m_begin;
		}

		constexpr size_t size( ) const noexcept {
			return m_size;
		}

		constexpr bool empty( ) const noexcept {
			return m_begin == m_end;
		}

		constexpr iterator data( ) noexcept {
			return m_begin;
		}

		constexpr const_iterator data( ) const noexcept {
			return m_begin;
		}

		constexpr iterator begin( ) noexcept {
			return m_begin;
		}

		constexpr const_iterator begin( ) const noexcept {
			return m_begin;
		}

		constexpr const_iterator cbegin( ) const noexcept {
			return m_begin;
		}

		constexpr iterator end( ) noexcept {
			return m_end;
		}

		constexpr const_iterator end( ) const noexcept {
			return m_end;
		}

		constexpr const_iterator cend( ) const noexcept {
			return m_end;
		}

		constexpr reference operator[]( size_t pos ) {
			return *( m_begin + pos );
		}

		constexpr const_reference operator[]( size_t pos ) const {
			return *( m_begin + pos );
		}

		constexpr reference at( size_t pos ) {
			daw::exception::precondition_check<std::out_of_range>(
			  pos < m_size, "position is beyond end of heap_array" );

			return operator[]( pos );
		}

		constexpr const_reference at( size_t pos ) const {
			daw::exception::precondition_check<std::out_of_range>(
			  pos < m_size, "position is beyond end of heap_array" );

			return operator[]( pos );
		}

		constexpr reference front( ) {
			return *m_begin;
		}

		constexpr const_reference front( ) const {
			return *m_begin;
		}

		constexpr reference back( ) {
			auto tmp = m_end;
			--tmp;
			return *tmp;
		}

		constexpr const_reference back( ) const {
			auto tmp = m_end;
			--tmp;
			return *tmp;
		}

		constexpr iterator find_first_of( const_reference value,
		                                  const_iterator start_at ) const {
			assert( m_begin != nullptr );
			assert( start_at != nullptr );
			*m_end = value;
			if( start_at != nullptr ) {
				while( *start_at != value ) {
					++start_at;
				}
			}
			return const_cast<iterator>( start_at );
		}

		iterator find_first_of( const_reference value ) const {
			return find_first_of( value, cbegin( ) );
		}
	}; // struct heap_array

	template<typename T>
	constexpr void swap( daw::heap_array<T> &lhs,
	                     daw::heap_array<T> &rhs ) noexcept {
		lhs.swap( rhs );
	}
} // namespace daw
