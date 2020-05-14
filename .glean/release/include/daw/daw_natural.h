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

#include "daw_exception.h"
#include "daw_traits.h"

namespace daw {
	struct invalid_natural_number : public daw::exception::arithmetic_exception {
	};
	///
	// A natural number type
	// An operation that causes overflow is undefined
	template<typename T, required<std::is_integral_v<T>> = nullptr>
	struct natural_t {
		using value_type = std::decay_t<T>;

	private:
		T m_value;

		template<typename Value>
		static constexpr decltype( auto ) validate( Value &&val ) {
			daw::exception::precondition_check<invalid_natural_number>( val >= 1 );

			return std::forward<Value>( val );
		}

	public:
		constexpr natural_t( ) noexcept
		  : m_value{1} {}

		constexpr natural_t( natural_t const & ) noexcept = default;
		constexpr natural_t( natural_t && ) noexcept = default;
		constexpr natural_t &operator=( natural_t const & ) noexcept = default;
		constexpr natural_t &operator=( natural_t && ) noexcept = default;
		~natural_t( ) noexcept = default;

		template<typename Value,
		         required<not std::is_same_v<natural_t, Value>> = nullptr>
		constexpr natural_t( Value &&v )
		  : m_value( validate( std::forward<Value>( v ) ) ) {}

		template<typename Value,
		         required<not std::is_same_v<natural_t, Value>> = nullptr>
		constexpr natural_t &operator=( Value &&v ) {
			m_value = validate( std::forward<Value>( v ) );
			return *this;
		}

		constexpr operator T( ) const noexcept {
			return m_value;
		}

		template<typename U>
		constexpr natural_t &operator+=( natural_t<U> const &rhs ) noexcept {
			static_assert( daw::traits::has_addition_operator_v<T>,
			               "Addition operator is not valid" );
			m_value += rhs.m_value;
			return *this;
		}

		template<typename U>
		constexpr natural_t &operator-=( natural_t<U> const &rhs ) {
			static_assert( daw::traits::has_subtraction_operator_v<T>,
			               "Subtraction operator is not valid" );
			m_value = validate( m_value - rhs.m_value );
			return *this;
		}

		template<typename U>
		constexpr natural_t &operator*=( natural_t<U> const &rhs ) noexcept {
			static_assert( daw::traits::has_multiplication_operator_v<T>,
			               "Multiplication operator is not valid" );
			m_value *= rhs.m_value;
			return *this;
		}

		template<typename U>
		constexpr natural_t &operator/=( natural_t<U> const &rhs ) noexcept {
			static_assert( daw::traits::has_division_operator_v<T>,
			               "Division operator is not valid" );
			m_value /= rhs.m_value;
			return *this;
		}

		template<typename U>
		constexpr natural_t &operator%=( natural_t<U> const &rhs ) noexcept {
			static_assert( daw::traits::has_modulus_operator_v<T>,
			               "Modulus operator is not valid" );
			m_value %= rhs.m_value;
			return *this;
		}

		constexpr natural_t &operator++( ) noexcept {
			static_assert( daw::traits::has_increment_operator_v<T>,
			               "Increment operator is not valid" );
			++m_value;
			return *this;
		}

		constexpr natural_t operator++( int ) noexcept {
			static_assert( daw::traits::has_increment_operator_v<T>,
			               "Increment operator is not valid" );
			auto tmp = *this;
			++m_value;
			return tmp;
		}

		constexpr natural_t &operator--( ) {
			static_assert( daw::traits::has_decrement_operator_v<T>,
			               "Decrement operator is not valid" );
			m_value = validate( m_value - 1 );
			return *this;
		}

		constexpr natural_t operator--( int ) {
			static_assert( daw::traits::has_decrement_operator_v<T>,
			               "Decrement operator is not valid" );
			auto tmp = *this;
			m_value = validate( m_value - 1 );
			return tmp;
		}

		template<typename U>
		friend constexpr decltype( auto )
		operator+( natural_t const &lhs, natural_t<U> const &rhs ) noexcept {
			static_assert( daw::traits::has_addition_operator_v<T, U>,
			               "Addition operator is not valid" );
			using result_t = decltype( std::declval<T>( ) + std::declval<U>( ) );
			natural_t<result_t> result{};
			result.m_value = static_cast<T>( lhs ) + static_cast<U>( rhs );
			return result;
		}

		template<typename U>
		friend constexpr auto operator-( natural_t const &lhs,
		                                 natural_t<U> const &rhs ) {
			static_assert( daw::traits::has_subtraction_operator_v<T, U>,
			               "Subtraction operator is not valid" );
			using result_t = decltype( std::declval<T>( ) - std::declval<U>( ) );
			return natural_t<result_t>{static_cast<T>( lhs ) - static_cast<U>( rhs )};
		}

		template<typename U>
		friend constexpr auto operator*( natural_t const &lhs,
		                                 natural_t<U> const &rhs ) noexcept {
			static_assert( daw::traits::has_multiplication_operator_v<T, U>,
			               "Multiplication operator is not valid" );
			using result_t = decltype( std::declval<T>( ) * std::declval<U>( ) );
			natural_t<result_t> result{};
			result.m_value = static_cast<T>( lhs ) * static_cast<U>( rhs );
			return result;
		}

		template<typename U>
		friend constexpr auto operator/( natural_t const &lhs,
		                                 natural_t<U> const &rhs ) noexcept {
			static_assert( daw::traits::has_division_operator_v<T, U>,
			               "Division operator is not valid" );
			using result_t = decltype( std::declval<T>( ) / std::declval<U>( ) );
			natural_t<result_t> result{};
			result.m_value = static_cast<T>( lhs ) / static_cast<U>( rhs );
			return result;
		}

		template<typename U>
		friend constexpr auto operator%( natural_t const &lhs,
		                                 natural_t<U> const &rhs ) noexcept {
			static_assert( daw::traits::has_modulus_operator_v<T, U>,
			               "Modulus operator is not valid" );
			using result_t = decltype( std::declval<T>( ) % std::declval<U>( ) );
			natural_t<result_t> result{};
			result.m_value = static_cast<T>( lhs ) % static_cast<U>( rhs );
			return result;
		}
	};

	template<typename T, typename U>
	constexpr bool operator==( natural_t<T> const &lhs,
	                           natural_t<U> const &rhs ) noexcept {
		return static_cast<T>( lhs ) == static_cast<U>( rhs );
	}

	template<typename T, typename U>
	constexpr bool operator!=( natural_t<T> const &lhs,
	                           natural_t<U> const &rhs ) noexcept {
		return static_cast<T>( lhs ) != static_cast<U>( rhs );
	}

	template<typename T, typename U>
	constexpr bool operator<( natural_t<T> const &lhs,
	                          natural_t<U> const &rhs ) noexcept {
		return static_cast<T>( lhs ) < static_cast<U>( rhs );
	}

	template<typename T, typename U>
	constexpr bool operator<=( natural_t<T> const &lhs,
	                           natural_t<U> const &rhs ) noexcept {
		return static_cast<T>( lhs ) <= static_cast<U>( rhs );
	}

	template<typename T, typename U>
	constexpr bool operator>( natural_t<T> const &lhs,
	                          natural_t<U> const &rhs ) noexcept {
		return static_cast<T>( lhs ) > static_cast<U>( rhs );
	}

	template<typename T, typename U>
	constexpr bool operator>=( natural_t<T> const &lhs,
	                           natural_t<U> const &rhs ) noexcept {
		return static_cast<T>( lhs ) >= static_cast<U>( rhs );
	}

	namespace literals {
		constexpr size_t operator"" _N( unsigned long long val ) noexcept {
			return daw::natural_t<unsigned long long>{val};
		}
	} // namespace literals
} // namespace daw
