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

#include "cpp_17.h"
#include "daw_move.h"

#pragma once

namespace daw {
	template<typename T>
	struct read_only {
		static_assert( not std::is_reference_v<T>,
		               "Reference types are not supported" );
		using value_type = T;
		using reference = value_type &;
		using const_reference = value_type const &;
		using rvalue_reference = value_type &&;
		using const_pointer = value_type const *;

	private:
		value_type m_value;

	public:
		constexpr read_only( ) noexcept( std::is_nothrow_constructible_v<T> )
		  : m_value( ) {}

		constexpr read_only( rvalue_reference value ) noexcept(
		  std::is_nothrow_move_constructible_v<T> )
		  : m_value( daw::move( value ) ) {}

		constexpr read_only( const_reference value ) noexcept(
		  std::is_nothrow_copy_constructible_v<T> )
		  : m_value( value ) {}

		read_only &operator=( const_reference ) = delete;
		read_only &operator=( rvalue_reference ) = delete;

		constexpr operator const_reference( ) const noexcept {
			return m_value;
		}

		constexpr const_reference get( ) const noexcept {
			return m_value;
		}

		constexpr reference read_write( ) noexcept {
			return m_value;
		}

		constexpr const_pointer operator->( ) const noexcept {
			return &m_value;
		}

		constexpr value_type move_out( ) noexcept {
			return daw::move( m_value );
		}
	}; // read_only
} // namespace daw
