// The MIT License (MIT)
//
// Copyright (c) 2016-2020 Darrell Wright
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

#include <exception>
#include <memory>
#include <stdexcept>
#include <string>
#include <type_traits>
#include <utility>

#include "daw_traits.h"

namespace daw {
	template<class ValueType>
	struct optional_poly {
		using value_type = std::decay_t<ValueType>;
		using reference = value_type &;
		using const_reference = value_type const &;
		using pointer = value_type *;
		using pointer_const = value_type const *;

	private:
		std::unique_ptr<value_type> m_value;

		template<typename T, typename = std::enable_if_t<
		                       std::is_base_of<value_type, T>::value or
		                       std::is_same<value_type, T>::value>>
		constexpr static T *make_copy( T *ptr ) {
			if( !ptr ) {
				return nullptr;
			}
			using v_t = std::decay_t<T>;
			return new v_t( *ptr );
		}

	public:
		template<typename T>
		friend struct optional_poly;

		optional_poly( ) noexcept = default;
		~optional_poly( ) = default;
		optional_poly( optional_poly && ) noexcept = default;
		optional_poly &operator=( optional_poly && ) noexcept = default;

		optional_poly( optional_poly const &other )
		  : m_value{make_copy( other.m_value.get( ) )} {}

		optional_poly &operator=( optional_poly const &rhs ) {
			if( this != &rhs ) {
				m_value.reset( make_copy( rhs.m_value.get( ) ) );
			}
			return *this;
		}

		template<typename... Args>
		void emplace( Args &&... args ) {
			m_value.reset( new value_type{std::forward<Args>( args )...} );
		}

		template<typename T, typename = std::enable_if_t<
		                       std::is_base_of<value_type, T>::value or
		                       std::is_same<value_type, T>::value>>
		optional_poly( T &&value )
		  : m_value{new std::decay_t<T>{std::forward<T>( value )}} {}

		template<typename T, typename = std::enable_if_t<
		                       std::is_base_of<value_type, T>::value or
		                       std::is_same<value_type, T>::value>>
		optional_poly( T const &value )
		  : m_value{new std::decay_t<T>{value}} {}

		template<typename T, typename = std::enable_if_t<
		                       std::is_base_of<value_type, T>::value or
		                       std::is_same<value_type, T>::value>>
		explicit optional_poly( optional_poly<T> const &other )
		  : m_value{make_copy( other.m_value.get( ) )} {}

		template<typename T, typename = std::enable_if_t<
		                       std::is_base_of<value_type, T>::value or
		                       std::is_same<value_type, T>::value>>
		explicit optional_poly( optional_poly<T> &&other ) noexcept
		  : m_value{other.m_value.release( )} {}

		template<typename T, typename = std::enable_if_t<
		                       std::is_base_of<value_type, T>::value or
		                       std::is_same<value_type, T>::value>>
		optional_poly &operator=( optional_poly<T> const &rhs ) {
			m_value.reset( make_copy( rhs.m_value.get( ) ) );
			return *this;
		}

		template<typename T,
		         std::enable_if_t<( std::is_base_of<value_type, T>::value or
		                            std::is_same<value_type, T>::value ),
		                          std::nullptr_t> = nullptr>
		optional_poly &operator=( optional_poly<T> &&rhs ) {
			if( this != static_cast<optional_poly *>( &rhs ) ) {
				m_value.reset( rhs.m_value.release( ) );
			}
			return *this;
		}

		template<typename T,
		         std::enable_if_t<( std::is_base_of<value_type, T>::value or
		                            std::is_same<value_type, T>::value ),
		                          std::nullptr_t> = nullptr>
		optional_poly &operator=( T const &value ) {
			m_value.reset( new value_type{value} );
			return *this;
		}

		void swap( optional_poly &rhs ) noexcept {
			m_value.swap( rhs.m_value );
		}

		bool empty( ) const noexcept {
			return !m_value;
		}

		bool has_value( ) const noexcept {
			return static_cast<bool>( m_value );
		}

		explicit operator bool( ) const noexcept {
			return static_cast<bool>( m_value );
		}

		reference get( ) {
			return *m_value;
		}

		const_reference get( ) const {
			return *m_value;
		}

		reference operator*( ) {
			return get( );
		}

		const_reference operator*( ) const {
			return get( );
		}

		pointer operator->( ) {
			return m_value.get( );
		}

		pointer_const operator->( ) const {
			return m_value.get( );
		}

		void reset( ) {
			m_value.reset( );
		}
	}; // class optional_poly

	template<typename T>
	void swap( daw::optional_poly<T> &lhs, daw::optional_poly<T> &rhs ) noexcept {
		lhs.swap( rhs );
	}

	template<typename T, typename... Args>
	auto make_optional_poly( Args &&... args ) {
		optional_poly<T> result{};
		result.emplace( std::forward<Args>( args )... );
		return result;
	}

	template<typename T>
	bool operator==( daw::optional_poly<T> const &lhs,
	                 daw::optional_poly<T> const &rhs ) {
		if( lhs ) {
			if( rhs ) {
				return *lhs == *rhs;
			}
			return false;
		}
		return !static_cast<bool>( rhs );
	}

	template<typename T>
	bool operator!=( daw::optional_poly<T> const &lhs,
	                 daw::optional_poly<T> const &rhs ) {
		if( lhs ) {
			if( rhs ) {
				return *lhs != *rhs;
			}
			return true;
		}
		return static_cast<bool>( rhs );
	}

	template<typename T>
	bool operator<( daw::optional_poly<T> const &lhs,
	                daw::optional_poly<T> const &rhs ) {
		if( lhs ) {
			if( rhs ) {
				return *lhs < *rhs;
			}
			return false;
		}
		return true;
	}

	template<typename T>
	bool operator<=( daw::optional_poly<T> const &lhs,
	                 daw::optional_poly<T> const &rhs ) {
		if( lhs ) {
			if( rhs ) {
				return *lhs <= *rhs;
			}
			return false;
		}
		return true;
	}

	template<typename T>
	bool operator>( daw::optional_poly<T> const &lhs,
	                daw::optional_poly<T> const &rhs ) {
		if( lhs ) {
			if( rhs ) {
				return *lhs > *rhs;
			}
			return true;
		}
		return false;
	}

	template<typename T>
	bool operator>=( daw::optional_poly<T> const &lhs,
	                 daw::optional_poly<T> const &rhs ) {
		if( lhs ) {
			if( rhs ) {
				return *lhs >= *rhs;
			}
			return true;
		}
		return false;
	}

} // namespace daw
