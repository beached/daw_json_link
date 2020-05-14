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
#include <utility>

#include "../cpp_17.h"
#include "../daw_enable_if.h"
#include "daw_function_iterator.h"

namespace daw {
	template<typename Container>
	struct end_inserter_iterator final {
		using iterator_category = std::output_iterator_tag;
		using value_type = void;
		using difference_type = void;
		using pointer = void;
		using reference = void;

	private:
		Container *m_container;

	public:
		constexpr end_inserter_iterator( Container &c ) noexcept
		  : m_container( &c ) {}

		template<typename T,
		         daw::enable_when_t<!std::is_same_v<
		           daw::remove_cvref_t<T>, end_inserter_iterator>> = nullptr>
		constexpr end_inserter_iterator &operator=( T &&val ) {
			m_container->insert( std::end( *m_container ), std::forward<T>( val ) );
			return *this;
		}

		constexpr end_inserter_iterator &operator*( ) noexcept {
			return *this;
		}

		constexpr end_inserter_iterator &operator++( ) noexcept {
			return *this;
		}

		constexpr end_inserter_iterator &operator++( int ) noexcept {
			return *this;
		}
	};
	template<typename Container>
	end_inserter_iterator( Container )->end_inserter_iterator<Container>;

	template<typename Container>
	constexpr auto end_inserter( Container &c ) {
		return end_inserter_iterator<Container>( c );
	}
} // namespace daw
