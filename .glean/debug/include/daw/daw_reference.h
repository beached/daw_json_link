// The MIT License (MIT)
//
// Copyright (c) 2014-2020 Darrell Wright
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

#include <algorithm>
#include <cstddef>
#include <iterator>
#include <numeric>
#include <vector>

#include "daw_algorithm.h"
#include "daw_traits.h"

namespace daw {
	template<typename T>
	struct Reference {
		using ptr_type = daw::traits::root_type_t<T>;
		ptr_type const *ptr;
		Reference( ) = delete;
		Reference( T const &value )
		  : ptr( &value ) {}
		~Reference( ) {}

		Reference( Reference const & ) = default;
		Reference( Reference && ) noexcept = default;
		Reference &operator=( Reference const & ) = default;
		Reference &operator=( Reference && ) noexcept = default;

		T const &operator*( ) const {
			return *ptr;
		}

		T const &get( ) const {
			return *ptr;
		}
	}; // struct Reference

	template<typename T>
	bool operator==( Reference<T> const &lhs, Reference<T> const &rhs ) {
		return *lhs == *rhs;
	}

	template<typename T>
	bool operator!=( Reference<T> const &lhs, Reference<T> const &rhs ) {
		return *lhs != *rhs;
	}

	template<typename T>
	bool operator<( Reference<T> const &lhs, Reference<T> const &rhs ) {
		return *lhs < *rhs;
	}

	template<typename T>
	bool operator<=( Reference<T> const &lhs, Reference<T> const &rhs ) {
		return *lhs <= *rhs;
	}

	template<typename T>
	bool operator>( Reference<T> const &lhs, Reference<T> const &rhs ) {
		return *lhs > *rhs;
	}

	template<typename T>
	bool operator>=( Reference<T> const &lhs, Reference<T> const &rhs ) {
		return *lhs >= *rhs;
	}

	template<typename T>
	auto ref( T const *value ) {
		return Reference<T>( *value );
	}

	template<typename T, typename = void>
	auto ref( T const &value ) {
		return Reference<T>( value );
	}

	template<typename Iterator>
	auto to_reference_vector( Iterator first, Iterator last ) {
		using value_type = daw::traits::root_type_t<decltype( *first )>;
		using values_type = std::vector<daw::Reference<value_type>>;

		values_type result{};
		while( first != last ) {
			result.push_back( daw::ref<value_type>( *first ) );
		}
		return result;
	}

	template<typename OStream, typename T,
	         std::enable_if_t<daw::traits::is_ostream_like_lite_v<OStream>,
	                          std::nullptr_t> = nullptr>
	OStream &operator<<( OStream &os, Reference<T> const &ref ) {
		os << *ref;
		return os;
	}
} // namespace daw
