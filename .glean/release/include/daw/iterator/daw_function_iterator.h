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

#include "../cpp_17.h"
#include "../daw_algorithm.h"
#include "../daw_move.h"
#include "../daw_traits.h"

namespace daw {
	///
	///	An output iterator that calls supplied function when operator= is called
	/// This is like std::back_intsert_iterator
	///
	template<typename Function>
	struct function_iterator {
		using iterator_category = std::output_iterator_tag;
		using value_type = void;
		using difference_type = void;
		using pointer = void;
		using reference = void;

	private:
		Function m_function;

	public:
		constexpr function_iterator( Function function )
		  : m_function( daw::move( function ) ) {}

		template<typename T,
		         std::enable_if_t<
		           not std::is_same_v<daw::remove_cvref_t<T>, function_iterator>,
		           std::nullptr_t> = nullptr>
		constexpr function_iterator &operator=( T &&val ) {
			(void)m_function( std::forward<T>( val ) );
			return *this;
		}

		constexpr function_iterator &operator*( ) noexcept {
			return *this;
		}

		constexpr function_iterator &operator++( ) noexcept {
			return *this;
		}

		constexpr function_iterator &operator++( int ) noexcept {
			return *this;
		}
	};
	template<typename Function>
	function_iterator( Function )->function_iterator<Function>;

	///
	/// Create a function_iterator with supplied function
	///
	template<typename Function>
	constexpr auto make_function_iterator( Function &&func ) {
		return function_iterator<Function>( std::forward<Function>( func ) );
	}
} // namespace daw
