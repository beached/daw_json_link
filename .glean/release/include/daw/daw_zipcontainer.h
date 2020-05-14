// The MIT License ( MIT )
//
// Copyright ( c ) 2013-2020 Darrell Wright
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

#include "iterator/daw_zipiter.h"

namespace daw {
	template<typename... Containers>
	struct zip_container {
		using iterator =
		  zip_iterator<decltype( std::declval<Containers...>( ).begin( ) )>;
		using const_iterator = zip_iterator<decltype(
		  std::declval<std::add_const_t<Containers>>( ).begin( ) )...>;

	private:
		iterator m_begin;
		iterator m_end;

	public:
		zip_container( Containers... containers )
		  : m_begin( make_zipiter( std::begin( containers )... ) )
		  , m_end( make_zipiter( std::end( containers )... ) ) {}

		iterator begin( ) {
			return m_begin;
		}

		const_iterator begin( ) const {
			return m_begin;
		}

		const_iterator cbegin( ) const {
			return m_begin;
		}

		iterator end( ) {
			return m_end;
		}

		const_iterator end( ) const {
			return m_end;
		}

		const_iterator cend( ) const {
			return m_end;
		}
	}; // struct zip_container

	template<typename... Containers>
	zip_container<Containers...> make_zipcontainer( Containers... args ) {
		return zip_container<Containers...>( args... );
	}
} // namespace daw
