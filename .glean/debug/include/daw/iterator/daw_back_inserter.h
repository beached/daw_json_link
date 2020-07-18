
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

#include <type_traits>
#include <utility>

#include "../cpp_17.h"
#include "../daw_enable_if.h"
#include "daw_function_iterator.h"

namespace daw {
	template<typename Container>
	struct back_inserter final {
		using iterator_category = std::output_iterator_tag;
		using value_type = void;
		using difference_type = void;
		using pointer = void;
		using reference = void;

	private:
		Container *m_container;

	public:
		constexpr back_inserter( Container &c ) noexcept
		  : m_container( &c ) {}

		template<typename T, daw::enable_when_t<not std::is_same_v<
		                       daw::remove_cvref_t<T>, back_inserter>> = nullptr>
		constexpr back_inserter &operator=( T &&val ) {
			m_container->push_back( std::forward<T>( val ) );
			return *this;
		}

		constexpr back_inserter &operator*( ) noexcept {
			return *this;
		}

		constexpr back_inserter &operator++( ) noexcept {
			return *this;
		}

		constexpr back_inserter operator++( int ) const noexcept {
			return *this;
		}
	};

	template<typename Container>
	back_inserter( Container )->back_inserter<Container>;
} // namespace daw
