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

#include "cpp_17.h"
#include "daw_hide.h"

namespace daw::algorithm {
	namespace do_n_details DAW_ATTRIBUTE_HIDDEN {
		template<typename Function, typename... Args, std::size_t... Is>
		DAW_ATTRIBUTE_FLATTEN static inline constexpr void
		do_n( Function &&func, std::integer_sequence<std::size_t, Is...>,
		      Args &&... args ) noexcept( noexcept( func( args... ) ) ) {
			if constexpr( sizeof...( Is ) > 0 ) {
				(void)( ( func( args... ), Is ) + ... );
			}
		}

		template<std::size_t by_n, typename Function, std::size_t... Is>
		DAW_ATTRIBUTE_FLATTEN static inline constexpr void
		do_n_arg( Function &&func, std::integer_sequence<std::size_t, Is...> ) noexcept(
		  noexcept( func( 0ULL ) ) ) {
			if constexpr( sizeof...( Is ) > 0 ) {
				(void)( ( func( Is*by_n ), 0 ) + ... );
			}
		}
	} // namespace do_n_detailsDAW_ATTRIBUTE_HIDDEN

	template<std::size_t count, typename Function, typename... Args>
	DAW_ATTRIBUTE_FLATTEN static inline constexpr void do_n( Function &&func,
	                                                         Args &&... args ) {
		do_n_details::do_n( std::forward<Function>( func ),
		                    std::make_integer_sequence<std::size_t, count>{},
		                    std::forward<Args>( args )... );
	}

	template<typename Function, typename... Args>
	DAW_ATTRIBUTE_FLATTEN static inline constexpr void
	do_n( std::size_t count, Function &&func, Args &&... args ) noexcept(
	  std::is_nothrow_invocable_v<Function, Args...> ) {
		while( count-- > 0 ) {
			func( args... );
		}
	}

	template<typename Function, typename... Args>
	DAW_ATTRIBUTE_FLATTEN static inline constexpr void
	do_n_arg( std::size_t count, Function &&func ) noexcept(
	  std::is_nothrow_invocable_v<Function, std::size_t> ) {
		std::size_t n = 0;
		while( n < count ) {
			func( n++ );
		}
	}

	template<std::size_t count, std::size_t by_n = 1, typename Function, typename... Args>
	DAW_ATTRIBUTE_FLATTEN static inline constexpr void
	do_n_arg( Function &&func ) noexcept(
	  std::is_nothrow_invocable_v<Function, std::size_t> ) {
		static_assert( by_n > 0 );
		do_n_details::do_n_arg<by_n>( std::forward<Function>( func ),
		                        std::make_index_sequence<count/by_n>{} );
	}
} // namespace daw::algorithm
