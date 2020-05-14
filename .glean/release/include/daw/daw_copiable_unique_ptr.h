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
#include <memory>
#include <type_traits>
#include <utility>

#include "daw_exchange.h"
#include "daw_swap.h"
#include "daw_traits.h"

namespace daw {
	template<typename T, typename Deleter = std::default_delete<T>>
	struct copiable_unique_ptr {
		using value_type = T;
		using reference = value_type &;
		using const_reference = value_type const &;
		using pointer = value_type *;
		using const_pointer = value_type const *;

	private:
		pointer m_value = nullptr;

		template<typename U = value_type, typename... Args>
		static pointer make_ptr( Args &&... args ) noexcept(
		  std::is_nothrow_constructible_v<value_type, Args...> ) {

			return new U( std::forward<Args>( args )... );
		}
		struct private_constructor {};

		constexpr explicit copiable_unique_ptr( private_constructor ) noexcept {}

	public:
		constexpr copiable_unique_ptr( ) noexcept = default;
		constexpr copiable_unique_ptr( copiable_unique_ptr &&other ) noexcept
		  : m_value( daw::exchange( other.m_value, nullptr ) ) {}

		constexpr copiable_unique_ptr &
		operator=( copiable_unique_ptr &&rhs ) noexcept {
			auto tmp = m_value;
			m_value = daw::exchange( rhs.m_value, tmp );
			return *this;
		}

		constexpr copiable_unique_ptr( pointer ptr ) noexcept
		  : m_value( ptr ) {}

		copiable_unique_ptr( copiable_unique_ptr const &other )
		  : m_value( other.m_value ? make_ptr( *other.m_value ) : nullptr ) {}

		copiable_unique_ptr &operator=( copiable_unique_ptr const &rhs ) {
			if( this == &rhs ) {
				return *this;
			}
			if( !rhs.m_value ) {
				reset( );
				return *this;
			}
			if( !m_value ) {
				m_value = make_ptr( *rhs.m_value );
				return *this;
			}
			*m_value = *rhs.m_value;
			return *this;
		}

		void reset( ) {
			auto tmp = daw::exchange( m_value, nullptr );
			if( tmp ) {
				Deleter( )( tmp );
			}
		}

		~copiable_unique_ptr( ) {
			reset( );
		}

		pointer release( ) noexcept {
			return daw::exchange( m_value, nullptr );
		}

		explicit constexpr operator bool( ) const noexcept {
			return static_cast<bool>( m_value );
		}

		constexpr void swap( copiable_unique_ptr &rhs ) noexcept {
			pointer tmp = m_value;
			m_value = daw::exchange( rhs.m_value, tmp );
		}

		constexpr pointer get( ) noexcept {
			return m_value;
		}

		constexpr const_pointer get( ) const noexcept {
			return m_value;
		}

		constexpr reference operator*( ) noexcept {
			return *m_value;
		}

		constexpr const_reference operator*( ) const noexcept {
			return *m_value;
		}

		constexpr pointer operator->( ) noexcept {
			return m_value;
		}

		constexpr const_pointer operator->( ) const noexcept {
			return m_value;
		}
	};

	template<typename T, typename Deleter>
	constexpr void swap( copiable_unique_ptr<T, Deleter> &lhs,
	                     copiable_unique_ptr<T, Deleter> &rhs ) noexcept {
		lhs.swap( rhs );
	}

	template<typename T, typename... Args>
	copiable_unique_ptr<T> make_copiable_unique_ptr( Args &&... args ) noexcept(
	  std::is_nothrow_constructible_v<T, Args...> ) {

		auto ptr = new T( std::forward<Args>( args )... );
		return {ptr};
	}

	template<typename T1, typename D1, typename T2, typename D2>
	constexpr bool operator==( copiable_unique_ptr<T1, D1> const &lhs,
	                           copiable_unique_ptr<T2, D2> const &rhs ) noexcept {
		return lhs.get( ) == rhs.get( );
	}

	template<typename T1, typename D1, typename T2, typename D2>
	constexpr bool operator==( copiable_unique_ptr<T1, D1> const &lhs,
	                           std::nullptr_t ) noexcept {
		return !lhs;
	}

	template<typename T1, typename D1, typename T2, typename D2>
	constexpr bool operator==( std::nullptr_t,
	                           copiable_unique_ptr<T1, D1> const &rhs ) noexcept {
		return !rhs;
	}

	template<typename T1, typename D1, typename T2, typename D2>
	constexpr bool operator!=( copiable_unique_ptr<T1, D1> const &lhs,
	                           copiable_unique_ptr<T2, D2> const &rhs ) noexcept {

		return lhs.get( ) != rhs.get( );
	}

	template<typename T1, typename D1, typename T2, typename D2>
	constexpr bool operator!=( copiable_unique_ptr<T1, D1> const &lhs,
	                           std::nullptr_t ) noexcept {

		return static_cast<bool>( lhs );
	}

	template<typename T1, typename D1, typename T2, typename D2>
	constexpr bool operator!=( std::nullptr_t,
	                           copiable_unique_ptr<T2, D2> const &rhs ) noexcept {

		return static_cast<bool>( rhs );
	}

	template<typename T1, typename D1, typename T2, typename D2>
	constexpr bool operator<( copiable_unique_ptr<T1, D1> const &lhs,
	                          copiable_unique_ptr<T2, D2> const &rhs ) noexcept {
		using CT =
		  std::common_type_t<typename copiable_unique_ptr<T1, D1>::pointer,
		                     typename copiable_unique_ptr<T2, D2>::pointer>;
		return std::less<CT>( lhs.get( ), rhs.get( ) );
	}

	template<typename T1, typename D1, typename T2, typename D2>
	constexpr bool operator<( copiable_unique_ptr<T1, D1> const &lhs,
	                          std::nullptr_t ) noexcept {

		return std::less<typename copiable_unique_ptr<T1, D1>::pointer>( lhs.get( ),
		                                                                 nullptr );
	}

	template<typename T1, typename D1, typename T2, typename D2>
	constexpr bool operator<( std::nullptr_t,
	                          copiable_unique_ptr<T1, D1> const &rhs ) noexcept {

		return std::less<typename copiable_unique_ptr<T1, D1>::pointer>(
		  nullptr, rhs.get( ) );
	}

	template<typename T1, typename D1, typename T2, typename D2>
	constexpr bool operator<=( copiable_unique_ptr<T1, D1> const &lhs,
	                           copiable_unique_ptr<T2, D2> const &rhs ) noexcept {
		return !( rhs < lhs );
	}

	template<typename T1, typename D1, typename T2, typename D2>
	constexpr bool operator<=( copiable_unique_ptr<T1, D1> const &lhs,
	                           std::nullptr_t ) noexcept {

		return !( nullptr < lhs );
	}

	template<typename T1, typename D1, typename T2, typename D2>
	constexpr bool operator<=( std::nullptr_t,
	                           copiable_unique_ptr<T2, D2> const &rhs ) noexcept {

		return !( rhs < nullptr );
	}

	template<typename T1, typename D1, typename T2, typename D2>
	constexpr bool operator>( copiable_unique_ptr<T1, D1> const &lhs,
	                          copiable_unique_ptr<T2, D2> const &rhs ) noexcept {
		return rhs < lhs;
	}

	template<typename T1, typename D1, typename T2, typename D2>
	constexpr bool operator>( copiable_unique_ptr<T1, D1> const &lhs,
	                          std::nullptr_t ) noexcept {

		return nullptr < lhs;
	}

	template<typename T1, typename D1, typename T2, typename D2>
	constexpr bool operator>( std::nullptr_t,
	                          copiable_unique_ptr<T2, D2> const &rhs ) noexcept {

		return rhs < nullptr;
	}

	template<typename T1, typename D1, typename T2, typename D2>
	constexpr bool operator>=( copiable_unique_ptr<T1, D1> const &lhs,
	                           copiable_unique_ptr<T2, D2> const &rhs ) noexcept {
		return !( lhs < rhs );
	}

	template<typename T1, typename D1, typename T2, typename D2>
	constexpr bool operator>=( copiable_unique_ptr<T1, D1> const &lhs,
	                           std::nullptr_t ) noexcept {

		return !( lhs < nullptr );
	}

	template<typename T1, typename D1, typename T2, typename D2>
	constexpr bool operator>=( std::nullptr_t,
	                           copiable_unique_ptr<T2, D2> const &rhs ) noexcept {

		return !( nullptr < rhs );
	}
} // namespace daw

namespace std {
	template<typename T, typename D>
	struct hash<daw::copiable_unique_ptr<T, D>> {
		size_t operator( )( daw::copiable_unique_ptr<T, D> const &p ) const
		  noexcept {
			return std::hash<T const *>{}( p.get( ) );
		}
		size_t operator( )( daw::copiable_unique_ptr<T, D> &&p ) const noexcept {
			return std::hash<T const *>{}( p.get( ) );
		}
	};
} // namespace std
