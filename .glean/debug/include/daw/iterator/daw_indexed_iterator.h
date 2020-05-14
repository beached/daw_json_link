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

#include <cstddef>
#include <iterator>

#include "../daw_exception.h"
#include "../daw_traits.h"

namespace daw {
	template<typename T>
	struct indexed_iterator {
		using difference_type = std::ptrdiff_t;
		using size_type = size_t;

		// don't want to depend on value_type being there
		using value_type = std::remove_reference_t<decltype(
		  std::declval<T>( )[std::declval<size_type>( )] )>;

		using pointer = value_type *;
		using const_pointer = std::remove_const_t<value_type> const *;
		using iterator_category = std::random_access_iterator_tag;
		using reference = value_type &;
		using const_reference = std::remove_const_t<value_type> const &;

	private:
		T *m_pointer;
		difference_type m_position;

	public:
		explicit constexpr indexed_iterator( T *container, size_t pos ) noexcept
		  : m_pointer( container )
		  , m_position( static_cast<difference_type>( pos ) ) {

			daw::exception::dbg_precondition_check( pos <= m_pointer->size( ) );
		}

		constexpr indexed_iterator &operator+=( difference_type n ) noexcept {
			daw::exception::dbg_precondition_check(
			  static_cast<size_type>( m_position + n ) <= m_pointer->size( ) );

			m_position += n;
			return *this;
		}

		constexpr indexed_iterator &operator-=( difference_type n ) noexcept {
			daw::exception::dbg_precondition_check( m_position - n >= 0 );

			m_position -= n;
			return *this;
		}

		constexpr reference operator*( ) noexcept {
			daw::exception::dbg_precondition_check( m_position >= 0 );
			daw::exception::dbg_precondition_check(
			  static_cast<size_type>( m_position ) <= m_pointer->size( ) );

			return m_pointer->operator[]( static_cast<size_type>( m_position ) );
		}

		constexpr const_reference operator*( ) const noexcept {
			daw::exception::dbg_precondition_check( m_position >= 0 );
			daw::exception::dbg_precondition_check(
			  static_cast<size_type>( m_position ) <= m_pointer->size( ) );

			return m_pointer->operator[]( static_cast<size_type>( m_position ) );
		}

		constexpr pointer operator->( ) noexcept {
			daw::exception::dbg_precondition_check( m_position >= 0 );
			daw::exception::dbg_precondition_check(
			  static_cast<size_type>( m_position ) <= m_pointer->size( ) );

			return &m_pointer->operator[]( static_cast<size_type>( m_position ) );
		}

		constexpr const_pointer operator->( ) const noexcept {
			daw::exception::dbg_precondition_check( m_position >= 0 );
			daw::exception::dbg_precondition_check(
			  static_cast<size_type>( m_position ) <= m_pointer->size( ) );

			return &m_pointer->operator[]( static_cast<size_type>( m_position ) );
		}

		constexpr indexed_iterator &operator++( ) noexcept {
			daw::exception::dbg_precondition_check(
			  static_cast<size_type>( m_position ) <= m_pointer->size( ) );

			++m_position;
			return *this;
		}

		constexpr const indexed_iterator operator++( int ) noexcept {
			daw::exception::dbg_precondition_check(
			  static_cast<size_type>( m_position ) <= m_pointer->size( ) );

			auto result = indexed_iterator( *this );
			++m_position;
			return result;
		}

		constexpr indexed_iterator &operator--( ) noexcept {
			daw::exception::dbg_precondition_check( m_position >= 0 );

			--m_position;
			return *this;
		}

		constexpr const indexed_iterator operator--( int ) noexcept {
			daw::exception::dbg_precondition_check( m_position >= 0 );

			auto result = indexed_iterator( *this );
			--m_position;
			return result;
		}

		constexpr indexed_iterator operator+( difference_type n ) const noexcept {
			auto const new_pos = m_position + n;
			daw::exception::dbg_precondition_check(
			  new_pos >= 0 &&
			  new_pos < static_cast<difference_type>( m_pointer->size( ) ) );
			auto result =
			  indexed_iterator( m_pointer, static_cast<size_type>( new_pos ) );
			return result;
		}

		constexpr indexed_iterator next( difference_type n = 0 ) const noexcept {
			auto const new_pos = m_position + n;
			daw::exception::dbg_precondition_check(
			  new_pos >= 0 &&
			  new_pos < static_cast<difference_type>( m_pointer->size( ) ) );
			auto result =
			  indexed_iterator( m_pointer, static_cast<size_type>( new_pos ) );
			return result;
		}

		constexpr indexed_iterator &advance( difference_type n = 0 ) noexcept {
			auto const new_pos = m_position + n;
			daw::exception::dbg_precondition_check(
			  new_pos >= 0 &&
			  new_pos < static_cast<difference_type>( m_pointer->size( ) ) );
			m_position = new_pos;
			return *this;
		}

		constexpr indexed_iterator operator-( difference_type n ) const noexcept {
			auto const new_pos = m_position - n;
			daw::exception::dbg_precondition_check(
			  new_pos >= 0 &&
			  new_pos < static_cast<difference_type>( m_pointer->size( ) ) );

			auto result =
			  indexed_iterator( m_pointer, static_cast<size_type>( new_pos ) );
			return result;
		}

		constexpr indexed_iterator prev( difference_type n = 0 ) const noexcept {
			auto const new_pos = m_position - n;
			daw::exception::dbg_precondition_check(
			  new_pos >= 0 &&
			  new_pos < static_cast<difference_type>( m_pointer->size( ) ) );
			auto result =
			  indexed_iterator( m_pointer, static_cast<size_type>( new_pos ) );
			return result;
		}

		template<typename U>
		constexpr difference_type compare( indexed_iterator<U> const &rhs ) const
		  noexcept {
			daw::exception::dbg_precondition_check(
			  std::equal_to<>{}( m_pointer, rhs.m_pointer ) );
			daw::exception::dbg_precondition_check( m_position >= 0 );
			daw::exception::dbg_precondition_check( rhs.m_position >= 0 );

			auto const result = m_position - rhs.m_position;
			if( result < 0 ) {
				return -1;
			}
			if( result > 0 ) {
				return 1;
			}
			return result;
		}

		constexpr friend difference_type
		operator-( indexed_iterator const &lhs,
		           indexed_iterator const &rhs ) noexcept {
			daw::exception::dbg_precondition_check( lhs.m_pointer == rhs.m_pointer );
			daw::exception::dbg_precondition_check( lhs.m_position >= 0 );
			daw::exception::dbg_precondition_check( rhs.m_position >= 0 );
			daw::exception::dbg_precondition_check( lhs.m_position >=
			                                        rhs.m_position );

			return lhs.m_position - rhs.m_position;
		}
	}; // indexed_iterator

	template<typename T>
	constexpr auto ibegin( T &&container ) noexcept {
		static_assert( std::is_lvalue_reference_v<T>,
		               "Passing a temporary to ibegin will cause errors via "
		               "dangling references" );

		return indexed_iterator<std::remove_reference_t<T>>( &container, 0U );
	}

	template<typename T>
	constexpr auto cibegin( T &&container ) noexcept {
		static_assert( std::is_lvalue_reference_v<T>,
		               "Passing a temporary to cibegin will cause errors via "
		               "dangling references" );

		return indexed_iterator<daw::remove_cvref_t<T> const>( &container, 0U );
	}

	template<typename T>
	constexpr auto iend( T &&container ) noexcept {
		static_assert( std::is_lvalue_reference_v<T>,
		               "Passing a temporary to iend will cause errors via "
		               "dangling references" );

		return indexed_iterator<std::remove_reference_t<T>>( &container,
		                                                     container.size( ) );
	}

	template<typename T>
	constexpr auto ciend( T &&container ) noexcept {
		static_assert( std::is_lvalue_reference_v<T>,
		               "Passing a temporary to ciend will cause errors via "
		               "dangling references" );

		return indexed_iterator<daw::remove_cvref_t<T> const>( &container,
		                                                       container.size( ) );
	}

	template<typename T, size_t N>
	constexpr auto ibegin( T ( &container )[N] ) noexcept {
		return std::begin( container );
	}

	template<typename T, size_t N>
	constexpr auto ibegin( T const ( &container )[N] ) noexcept {
		return std::begin( container );
	}

	template<typename T, size_t N>
	constexpr auto cibegin( T ( &container )[N] ) noexcept {
		return std::cbegin( container );
	}

	template<typename T, size_t N>
	constexpr auto cibegin( T const ( &container )[N] ) noexcept {
		return std::cbegin( container );
	}

	template<typename T, size_t N>
	constexpr auto iend( T ( &container )[N] ) noexcept {
		return std::end( container );
	}

	template<typename T, size_t N>
	constexpr auto iend( T const ( &container )[N] ) noexcept {
		return std::end( container );
	}

	template<typename T, size_t N>
	constexpr auto ciend( T ( &container )[N] ) noexcept {
		return std::cend( container );
	}

	template<typename T, size_t N>
	constexpr auto ciend( T const ( &container )[N] ) noexcept {
		return std::cend( container );
	}

	template<typename T>
	constexpr bool operator==( indexed_iterator<T> const &lhs,
	                           indexed_iterator<T> const &rhs ) noexcept {
		return lhs.compare( rhs ) == 0;
	}

	template<
	  typename T, typename U,
	  std::enable_if_t<not std::is_convertible_v<T, U>, std::nullptr_t> = nullptr>
	constexpr bool operator==( indexed_iterator<T> const &,
	                           indexed_iterator<U> const & ) noexcept {
		return false;
	}

	template<typename T>
	constexpr bool operator!=( indexed_iterator<T> const &lhs,
	                           indexed_iterator<T> const &rhs ) noexcept {
		return lhs.compare( rhs ) != 0;
	}

	template<
	  typename T, typename U,
	  std::enable_if_t<not std::is_convertible_v<T, U>, std::nullptr_t> = nullptr>
	constexpr bool operator!=( indexed_iterator<T> const &,
	                           indexed_iterator<U> const & ) noexcept {
		return true;
	}

	template<typename T>
	constexpr bool operator<( indexed_iterator<T> const &lhs,
	                          indexed_iterator<T> const &rhs ) noexcept {
		return lhs.compare( rhs ) < 0;
	}

	template<typename T>
	constexpr bool operator>( indexed_iterator<T> const &lhs,
	                          indexed_iterator<T> const &rhs ) noexcept {
		return lhs.compare( rhs ) > 0;
	}

	template<typename T>
	constexpr bool operator<=( indexed_iterator<T> const &lhs,
	                           indexed_iterator<T> const &rhs ) noexcept {
		return lhs.compare( rhs ) <= 0;
	}

	template<typename T>
	constexpr bool operator>=( indexed_iterator<T> const &lhs,
	                           indexed_iterator<T> const &rhs ) noexcept {
		return lhs.compare( rhs ) >= 0;
	}
} // namespace daw
