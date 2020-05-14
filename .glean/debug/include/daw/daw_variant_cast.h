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
#include <exception>
#include <type_traits>
#include <variant>

#include "cpp_17.h"
#include "daw_move.h"
#include "daw_traits.h"

namespace daw {
	namespace impl {
		template<typename T>
		struct variant_visitor_t {
			template<typename U,
			         std::enable_if_t<std::is_convertible_v<remove_cvref_t<U>, T>,
			                          std::nullptr_t> = nullptr>
			constexpr T operator( )( U &&result ) const noexcept {
				return static_cast<T>( std::forward<U>( result ) );
			}

			template<typename U,
			         std::enable_if_t<not std::is_convertible_v<remove_cvref_t<U>, T>,
			                          std::nullptr_t> = nullptr>
			T operator( )( U && ) const {
				throw std::bad_variant_access( );
			}
		};

		template<typename T>
		struct can_variant_visitor_t {
			constexpr bool operator( )( T const & ) noexcept {
				return true;
			}
			constexpr bool operator( )( T && ) noexcept {
				return true;
			}
			template<typename U>
			constexpr bool operator( )( U const & ) noexcept {
				return false;
			}
		};
	} // namespace impl

	template<typename T, typename... Args>
	constexpr T variant_cast( std::variant<Args...> &var ) {
		static_assert( daw::traits::can_convert_from_v<T, Args...>,
		               "T must be a convertible from type inside of variant" );
		return std::visit( impl::variant_visitor_t<T>{}, var );
	}

	template<typename T, typename... Args>
	constexpr T variant_cast( std::variant<Args...> const &var ) {
		static_assert( daw::traits::can_convert_from_v<T, Args...>,
		               "T must be a convertible from type inside of variant" );
		return std::visit( impl::variant_visitor_t<T>{}, var );
	}

	template<typename T, typename... Args>
	constexpr T variant_cast( std::variant<Args...> &&var ) {
		static_assert( daw::traits::can_convert_from_v<T, Args...>,
		               "T must be a convertible from type inside of variant" );
		return std::visit( impl::variant_visitor_t<T>{}, daw::move( var ) );
	}

	template<typename T, typename... Args>
	constexpr bool can_extract( std::variant<Args...> const &var ) noexcept {
		return std::visit( impl::can_variant_visitor_t<T>{}, var );
	}

	template<typename T, typename... Args>
	constexpr bool can_extract( std::variant<Args...> &&var ) noexcept {
		return std::visit( impl::can_variant_visitor_t<T>{}, daw::move( var ) );
	}
} // namespace daw
