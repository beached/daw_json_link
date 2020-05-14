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

#include <cstddef>
#include <new>
#include <type_traits>
#include <utility>

#include "daw_traits.h"

namespace daw {
	template<typename T>
	struct alignas( T ) uninitialized_storage : enable_copy_assignment<T>,
	                                            enable_copy_constructor<T>,
	                                            enable_move_assignment<T>,
	                                            enable_move_constructor<T> {
		using value_type = T;
		using pointer = value_type *;
		using const_pointer = value_type const *;
		using reference = value_type &;
		using const_reference = value_type const &;

	private:
		std::byte m_data[sizeof( value_type )];

	public:
		constexpr uninitialized_storage( ) noexcept = default;

		template<typename... Args>
		void construct( Args &&... args ) noexcept(
		  std::is_nothrow_constructible_v<T, Args...> ) {

			if constexpr( std::is_aggregate_v<T> ) {
				new( m_data ) T{std::forward<Args>( args )...};
			} else {
				new( m_data ) T( std ::forward<Args>( args )... );
			}
		}

		pointer ptr( ) noexcept {
			return std::launder( reinterpret_cast<pointer>( m_data ) );
		}

		const_pointer ptr( ) const noexcept {
			return std::launder( reinterpret_cast<const_pointer>( m_data ) );
		}

		const_pointer cptr( ) const noexcept {
			return std::launder( reinterpret_cast<const_pointer>( m_data ) );
		}

		reference ref( ) noexcept {
			return *ptr( );
		}

		const_reference cref( ) const noexcept {
			return *cptr( );
		}

		value_type rref( ) noexcept {
			return std::move( *ptr( ) );
		}

		reference operator*( ) noexcept {
			return ref( );
		}

		const_reference operator*( ) const noexcept {
			return cref( );
		}

		pointer operator->( ) noexcept {
			return ptr( );
		}

		const_pointer operator->( ) const noexcept {
			return cptr( );
		}

		void destruct( ) noexcept( std::is_nothrow_destructible_v<T> ) {
			ptr( )->~T( );
		}

		constexpr std::byte *raw_data( ) const noexcept {
			return m_data;
		}
	};
} // namespace daw
