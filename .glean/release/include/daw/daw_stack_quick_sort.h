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

#include <algorithm>
#include <utility>

namespace daw {
	namespace quick_sort_impl {
		template<typename T, size_t N, typename Idx = uint16_t>
		struct bounded_stack {
			T values[N];
			Idx idx = 0;

			template<typename... Args>
			constexpr void push_back( Args &&... args ) {
				values[idx++] = T{std::forward<Args>( args )...};
			}

			constexpr T &back( ) {
				return values[idx - 1];
			}

			constexpr void pop_back( ) {
				--idx;
			}

			constexpr bool has_more( ) const {
				return idx > 0;
			}
		};

		template<typename T, typename Compare>
		constexpr T median( T const &a, T const &b, T const &c, Compare cmp ) {
			if( cmp( a, b ) != cmp( a, c ) ) {
				return a;
			} else if( cmp( a, b ) != cmp( c, b ) ) {
				return b;
			}
			return c;
		}

	} // namespace quick_sort_impl

	template<typename RandomIterator, typename Compare = std::less<>>
	constexpr void quick_sort( RandomIterator f, RandomIterator l,
	                           Compare cmp = Compare{} ) {

		constexpr size_t tree_size = sizeof( size_t ) * 8U * 2U;

		quick_sort_impl::bounded_stack<std::pair<RandomIterator, RandomIterator>,
		                               tree_size * 2U>
		  sort_stack;
		sort_stack.push_back( f, l );

		while( sort_stack.has_more( ) ) {
			auto [first, last] = sort_stack.back( );
			sort_stack.pop_back( );

			auto const dist = std::distance( first, last );
			if( dist <= 1 ) {
				return;
			}
			auto const mid = std::next( first, dist / 2 );
			auto const pivot =
			  quick_sort_impl::median( *first, *mid, *std::prev( last ), cmp );

			auto const less_pivot = std::partition(
			  first, last, [=]( auto const &v ) { return cmp( v, pivot ); } );

			auto const split2 =
			  std::partition( less_pivot, last,
			                  [=]( auto const &v ) { return not cmp( pivot, v ); } );

			sort_stack.push_back( split2, last );
			sort_stack.push_back( first, less_pivot );
		}
	}
} // namespace daw
