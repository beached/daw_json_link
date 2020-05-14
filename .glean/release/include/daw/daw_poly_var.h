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

#include <type_traits>
#include <variant>

#include "daw_enable_if.h"
#include "daw_visit.h"

namespace daw {
	template<typename Base, typename... Types>
	struct poly_var {
		static_assert( ( std::is_base_of_v<Base, Types> and ... ),
		               "Types must be derived from base" );
		std::variant<Types...> m_value;

		constexpr poly_var( ) = default;

		template<typename T, daw::enable_when_t<std::is_constructible_v<
		                       std::variant<Types...>, T>> = nullptr>
		constexpr poly_var( T &&value )
		  : m_value( std::forward<T>( value ) ) {}

		template<typename T, daw::enable_when_t<std::is_constructible_v<
		                       std::variant<Types...>, T>> = nullptr>
		constexpr poly_var &operator=( T &&value ) {
			m_value = std::forward<T>( value );
			return *this;
		}

		constexpr Base const &operator*( ) const {
			return daw::visit_nt( m_value, []( auto const &value ) {
				return *static_cast<Base const *>( &value );
			} );
		}

		constexpr Base &operator*( ) {
			return daw::visit_nt(
			  m_value, []( auto &value ) { return *static_cast<Base *>( &value ); } );
		}

		constexpr Base const *operator->( ) const {
			return daw::visit_nt( m_value, []( auto const &value ) {
				return static_cast<Base const *>( &value );
			} );
		}

		constexpr Base *operator->( ) {
			return daw::visit_nt(
			  m_value, []( auto &value ) { return static_cast<Base *>( &value ); } );
		}
	};
} // namespace daw
