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

#include <cstddef>
#include <iterator>
#include <limits>
#include <stdexcept>
#include <type_traits>

#include "daw_exception.h"
#include "daw_math.h"

namespace daw {
	template<typename T, typename = void>
	struct span;

	template<typename>
	struct is_daw_span_t : std::false_type {};

	template<typename T>
	struct is_daw_span_t<daw::span<T>> : std::true_type {};

	template<typename T>
	inline constexpr bool const is_daw_span_v = is_daw_span_t<T>::value;

	// Const T
	template<typename T>
	struct span<T, std::enable_if_t<std::is_const_v<T>>> {
		using value_type = daw::remove_cvref_t<T>;
		using const_pointer = value_type const *;
		using const_reference = value_type const &;
		using const_iterator = const_pointer;
		using const_reverse_iterator = std::reverse_iterator<const_iterator>;
		using size_type = size_t;
		using difference_type = std::ptrdiff_t;

		static inline constexpr bool const has_mutable_pointer = false;

	private:
		const_pointer m_first = nullptr;
		size_type m_size = 0;

	public:
		constexpr span( ) noexcept = default;
		explicit constexpr span( std::nullptr_t ) noexcept {}
		constexpr span( std::nullptr_t, size_type ) noexcept {}

		constexpr span( const_pointer ptr, size_type count ) noexcept
		  : m_first( ptr )
		  , m_size( ptr == nullptr ? 0 : count ) {}

		constexpr span( const_pointer first, const_pointer last ) noexcept
		  : m_first( first )
		  , m_size( std::distance( first, last ) > 0
		              ? static_cast<size_type>( std::distance( first, last ) )
		              : 0 ) {}

		template<size_t N>
		explicit constexpr span( value_type const ( &arr )[N] ) noexcept
		  : m_first( arr )
		  , m_size( N ) {}

		template<typename Container, std::enable_if_t<not is_daw_span_v<Container>,
		                                              std::nullptr_t> = nullptr>
		constexpr span( Container const &c ) noexcept
		  : m_first( std::data( c ) )
		  , m_size( std::size( c ) ) {}

		// For symmetry with non-const span
		constexpr span copy( ) const noexcept {
			return {data( ), size( )};
		}

		constexpr const_iterator begin( ) const noexcept {
			return m_first;
		}

		constexpr const_iterator cbegin( ) const noexcept {
			return m_first;
		}

		constexpr const_iterator end( ) const noexcept {
			return m_first + m_size;
		}

		constexpr const_iterator cend( ) const noexcept {
			return m_first + m_size;
		}

		constexpr const_reverse_iterator rbegin( ) const noexcept {
			return std::make_reverse_iterator( end( ) );
		}

		constexpr const_reverse_iterator crbegin( ) const noexcept {
			return std::make_reverse_iterator( cend( ) );
		}

		constexpr const_reverse_iterator rend( ) const noexcept {
			return std::make_reverse_iterator( begin( ) );
		}

		constexpr const_reverse_iterator crend( ) const noexcept {
			return std::make_reverse_iterator( cend( ) );
		}

		constexpr const_reference operator[]( size_type pos ) const noexcept {
			return m_first[pos];
		}

		constexpr const_reference at( size_type pos ) const {
			daw::exception::precondition_check<std::out_of_range>(
			  pos < m_size, "Attempt to access span past end" );

			return m_first[pos];
		}

		constexpr const_reference front( ) const noexcept {
			return *m_first;
		}

		constexpr const_reference back( ) const noexcept {
			return *std::next( m_first, m_size - 1 );
		}

		constexpr const_pointer data( ) const noexcept {
			return m_first;
		}

		constexpr size_type size( ) const noexcept {
			return m_size;
		}

		constexpr size_type size_bytes( ) const noexcept {
			return m_size * sizeof( value_type );
		}

		[[nodiscard]] constexpr bool empty( ) const noexcept {
			return nullptr == m_first or 0 == m_size;
		}

		constexpr void remove_prefix( size_type n ) noexcept {
			m_first += n;
			m_size -= n;
		}

		constexpr void remove_prefix( ) noexcept {
			remove_prefix( 1 );
		}

		constexpr void remove_suffix( size_type n ) noexcept {
			m_size -= n;
		}

		constexpr void remove_suffix( ) noexcept {
			remove_suffix( 1 );
		}

		constexpr T pop_front( ) noexcept( std::is_nothrow_assignable_v<T> ) {
			auto result = front( );
			remove_prefix( );
			return result;
		}

		constexpr span<T const> pop_front( size_t n ) {
			auto result = subspan( 0, n );
			remove_prefix( n );
			return result;
		}

		constexpr T pop_back( ) noexcept( std::is_nothrow_assignable_v<T> ) {
			auto result = back( );
			remove_suffix( );
			return result;
		}

		constexpr span<T const> pop_back( size_t n ) {
			auto result = subspan( size( ) - n, n );
			remove_suffix( n );
			return result;
		}

		constexpr bool pop( T &value ) noexcept( std::is_nothrow_assignable_v<T> ) {
			if( empty( ) ) {
				return false;
			}
			value = back( );
			remove_suffix( );
			return true;
		}

		constexpr span
		subspan( size_type pos = 0,
		         size_type count = std::numeric_limits<size_type>::max( ) ) const {

			daw::exception::precondition_check<std::out_of_range>(
			  pos < size( ), "Attempt to access span past end" );

			count = daw::min( count, size( ) - pos );
			return {data( ) + pos, count};
		}
	};

	// non-const T span
	template<typename T>
	struct span<T, std::enable_if_t<not std::is_const_v<T>>> {
		using value_type = daw::remove_cvref_t<T>;
		using pointer = value_type *;
		using const_pointer = value_type const *;
		using reference = value_type &;
		using const_reference = value_type const &;
		using iterator = pointer;
		using const_iterator = const_pointer;
		using reverse_iterator = std::reverse_iterator<iterator>;
		using const_reverse_iterator = std::reverse_iterator<const_iterator>;
		using size_type = size_t;
		using difference_type = std::ptrdiff_t;

		static inline constexpr bool const has_mutable_pointer = true;

	private:
		pointer m_first = nullptr;
		size_type m_size = 0;

	public:
		constexpr span( ) noexcept = default;
		constexpr span( span && ) noexcept = default;
		constexpr span &operator=( span && ) noexcept = default;
		~span( ) = default;

		// Workaround for std::is_constructible check.  Have not found out
		// why it cannot see that this cannot happen.
		template<typename U>
		explicit span( span<U const> const & ) = delete;

		explicit constexpr span( std::nullptr_t ) noexcept {}
		constexpr span( std::nullptr_t, size_type ) noexcept {}

		constexpr span( pointer ptr, size_type count ) noexcept
		  : m_first( ptr )
		  , m_size( ptr == nullptr ? 0 : count ) {}

		constexpr span( pointer first, const_pointer last ) noexcept
		  : m_first( first )
		  , m_size( std::distance( static_cast<const_pointer>( first ), last ) > 0
		              ? static_cast<size_type>( std::distance(
		                  static_cast<const_pointer>( first ), last ) )
		              : 0 ) {}

		template<size_t N>
		explicit constexpr span( value_type ( &arr )[N] ) noexcept
		  : m_first( arr )
		  , m_size( N ) {}

		template<typename Container, std::enable_if_t<not is_daw_span_v<Container>,
		                                              std::nullptr_t> = nullptr>
		constexpr span( Container &c ) noexcept
		  : m_first( std::data( c ) )
		  , m_size( std::size( c ) ) {}

		// Only allow copy constructing from non-const span's.
		// This prevents the copy constructor loophole
		constexpr span( span &other ) noexcept
		  : m_first( other.m_first )
		  , m_size( other.m_size ) {}

		// Only allow copy assignment from non-const span's.
		// This prevents the copy constructor loophole
		constexpr span &operator=( span &rhs ) noexcept {
			m_first = rhs.m_first;
			m_size = rhs.m_size;
			return *this;
		}

		// Explicitly delete copy construction from a const span
		// This prevents span const -> span loophole.
		// Use copy member if this is desired
		span( span const & ) = delete;

		// Explicitly delete copy assignment from a const span
		// This prevents span const -> span loophole.
		// Use copy member if this is desired
		span &operator=( span const & ) = delete;

		// Conversion to const T span
		constexpr operator span<T const>( ) const noexcept {
			return {data( ), size( )};
		}

		// If one really wants to get mutable span
		// from a const span
		constexpr span copy( ) const noexcept {
			return {m_first, size( )};
		}

		constexpr iterator begin( ) noexcept {
			return m_first;
		}

		constexpr const_iterator begin( ) const noexcept {
			return m_first;
		}

		constexpr const_iterator cbegin( ) const noexcept {
			return m_first;
		}

		constexpr iterator end( ) noexcept {
			return m_first + m_size;
		}

		constexpr const_iterator end( ) const noexcept {
			return m_first + m_size;
		}

		constexpr const_iterator cend( ) const noexcept {
			return m_first + m_size;
		}

		constexpr reverse_iterator rbegin( ) noexcept {
			return std::make_reverse_iterator( end( ) );
		}

		constexpr const_reverse_iterator rbegin( ) const noexcept {
			return std::make_reverse_iterator( end( ) );
		}

		constexpr const_reverse_iterator crbegin( ) const noexcept {
			return std::make_reverse_iterator( cend( ) );
		}

		constexpr reverse_iterator rend( ) noexcept {
			return std::make_reverse_iterator( begin( ) );
		}

		constexpr const_reverse_iterator rend( ) const noexcept {
			return std::make_reverse_iterator( begin( ) );
		}

		constexpr const_reverse_iterator crend( ) const noexcept {
			return std::make_reverse_iterator( cbegin( ) );
		}

		constexpr reference operator[]( size_type pos ) noexcept {
			return m_first[pos];
		}

		constexpr const_reference operator[]( size_type pos ) const noexcept {
			return m_first[pos];
		}

		constexpr reference at( size_type pos ) {
			daw::exception::precondition_check<std::out_of_range>(
			  pos < m_size, "Attempt to access span past end" );
			return m_first[pos];
		}

		constexpr const_reference at( size_type pos ) const {
			daw::exception::precondition_check<std::out_of_range>(
			  pos < m_size, "Attempt to access span past end" );

			return m_first[pos];
		}

		constexpr reference front( ) noexcept {
			return *m_first;
		}

		constexpr const_reference front( ) const noexcept {
			return *m_first;
		}

		constexpr reference back( ) noexcept {
			return *std::next( m_first, m_size - 1 );
		}

		constexpr const_reference back( ) const noexcept {
			return *std::next( m_first, m_size - 1 );
		}

		constexpr pointer data( ) noexcept {
			return m_first;
		}

		constexpr const_pointer data( ) const noexcept {
			return m_first;
		}

		constexpr size_type size( ) const noexcept {
			return m_size;
		}

		constexpr size_type size_bytes( ) const noexcept {
			return m_size * sizeof( value_type );
		}

		[[nodiscard]] constexpr bool empty( ) const noexcept {
			return nullptr == m_first or 0 == m_size;
		}

		constexpr void remove_prefix( size_type n ) noexcept {
			m_first += n;
			m_size -= n;
		}

		constexpr void remove_prefix( ) noexcept {
			remove_prefix( 1 );
		}

		constexpr void remove_suffix( size_type n ) noexcept {
			m_size -= n;
		}

		constexpr void remove_suffix( ) noexcept {
			remove_suffix( 1 );
		}

		constexpr T pop_front( ) noexcept( std::is_nothrow_assignable_v<T> ) {
			auto result = front( );
			remove_prefix( );
			return result;
		}

		constexpr span<T> pop_front( size_t n ) noexcept {
			auto result = subspan( 0, n );
			remove_prefix( n );
			return result;
		}

		constexpr T pop_back( ) noexcept( std::is_nothrow_assignable_v<T> ) {
			auto result = back( );
			remove_suffix( );
			return result;
		}

		constexpr span<T> pop_back( size_t n ) {
			auto result = subspan( size( ) - n, n );
			remove_suffix( n );
			return result;
		}

		constexpr bool pop( T &value ) noexcept( std::is_nothrow_assignable_v<T> ) {
			if( empty( ) ) {
				return false;
			}
			value = back( );
			remove_suffix( );
			return true;
		}

		constexpr span
		subspan( size_type pos = 0,
		         size_type count = std::numeric_limits<size_type>::max( ) ) {

			daw::exception::precondition_check<std::out_of_range>(
			  pos < size( ), "Attempt to access span past end" );

			count = daw::min( count, size( ) - pos );
			return {data( ) + pos, count};
		}

		constexpr span
		subspan( size_type pos = 0,
		         size_type count = std::numeric_limits<size_type>::max( ) ) const {

			daw::exception::precondition_check<std::out_of_range>(
			  pos < size( ), "Attempt to access span past end" );

			count = daw::min( count, size( ) - pos );
			return {data( ) + pos, count};
		}
	};

	template<typename T>
	span( T *, size_t )->span<T>;

	template<typename T>
	span( T const *, size_t )->span<T const>;

	template<typename T, size_t N>
	span( T ( & )[N] )->span<T>;

	template<typename T>
	span( T *, T * )->span<T>;

	template<typename T>
	span( T *, T const * )->span<T>;

	template<typename T>
	span( T const *, T const * )->span<T const>;

	template<typename T>
	span( T const *, T * )->span<T const>;

	template<typename T, size_t N>
	span( T const ( & )[N] )->span<T const>;

	template<typename Container>
	span( Container & )
	  ->span<std::remove_reference_t<
	    decltype( *std::data( std::declval<Container>( ) ) )>>;

	template<typename Container>
	span( Container const & )
	  ->span<daw::remove_cvref_t<
	    decltype( *std::data( std::declval<Container>( ) ) )> const>;
} // namespace daw
