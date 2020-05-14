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

#include <deque>
#include <functional>
#include <list>
#include <vector>

#include "../daw_move.h"
#include "daw_function_iterator.h"

namespace daw {
	namespace si_impl {
		// Workaround
		// MSVC had issues with lambdas
		template<typename Compare, typename... VecArgs>
		struct sorted_iterator_vector_callable {
			std::vector<VecArgs...> *ptr;
			Compare compare;

			template<typename T>
			constexpr void operator( )( T &&value ) const {
				auto pos = std::lower_bound( std::begin( *ptr ), std::end( *ptr ),
				                             value, compare );
				ptr->emplace( std::move( pos ),
				              std::forward<decltype( value )>( value ) );
			}
		};

		template<typename Compare, typename... VecArgs>
		struct sorted_iterator_deque_callable {
			std::deque<VecArgs...> *ptr;
			Compare compare;

			template<typename T>
			constexpr void operator( )( T &&value ) const {
				auto pos = std::lower_bound( std::begin( *ptr ), std::end( *ptr ),
				                             value, compare );
				ptr->emplace( std::move( pos ),
				              std::forward<decltype( value )>( value ) );
			}
		};

		template<typename Compare, typename... VecArgs>
		struct sorted_iterator_list_callable {
			std::list<VecArgs...> *ptr;
			Compare compare;

			template<typename T>
			constexpr void operator( )( T &&value ) const {
				auto const pos = std::lower_bound( std::begin( *ptr ), std::end( *ptr ),
				                                   value, compare );
				ptr->emplace( std::move( pos ),
				              std::forward<decltype( value )>( value ) );
			}
		};

	} // namespace si_impl

	template<typename... Args, typename Compare = std::less<>>
	auto make_sorted_insert_iterator( std::vector<Args...> &c,
	                                  Compare compare = Compare{} ) {
		return daw::make_function_iterator(
		  si_impl::sorted_iterator_vector_callable<Compare, Args...>{
		    &c, std::move( compare )} );
	}

	template<typename... Args, typename Compare = std::less<>>
	auto make_sorted_insert_iterator( std::deque<Args...> &c,
	                                  Compare compare = Compare{} ) {
		return daw::make_function_iterator(
		  si_impl::sorted_iterator_deque_callable<Compare, Args...>{
		    &c, std::move( compare )} );
	}

	template<typename... Args, typename Compare = std::less<>>
	decltype( auto ) make_sorted_insert_iterator( std::list<Args...> &c,
	                                              Compare compare = Compare{} ) {
		return daw::make_function_iterator(
		  si_impl::sorted_iterator_list_callable<Compare, Args...>{
		    &c, std::move( compare )} );
	}
} // namespace daw
