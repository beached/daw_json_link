// The MIT License (MIT)
//
// Copyright (c) 2020 Darrell Wright
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
#include <type_traits>
#include <utility>

#if defined( _MSC_VER ) and not defined( __clang__ )
#include <intrin.h>
#endif

namespace daw {
#if not defined( _MSC_VER ) or defined( __clang__ )
	template<typename Tp>
	inline void do_not_optimize( Tp const &value ) {
		asm volatile( "" : : "r,m"( value ) : "memory" );
	}

	template<typename Tp>
	inline void do_not_optimize( Tp &value ) {
#if defined( __clang__ )
		asm volatile( "" : "+r,m"( value ) : : "memory" );
#else
		asm volatile( "" : "+m,r"( value ) : : "memory" );
#endif
	}
#else
	namespace do_not_optimize_details {
		[[maybe_unused]] constexpr void UseCharPointer( char const volatile * ) {}
	} // namespace do_not_optimize_details

#pragma optimize( "", off )
	template<class T>
	inline void do_not_optimize( T const &value ) {
		do_not_optimize_details::UseCharPointer(
		  &reinterpret_cast<char const volatile &>( value ) );
		_ReadWriteBarrier( );
	}
#pragma optimize( "", on )
#endif

	template<typename... Values, std::enable_if_t<( sizeof...( Values ) != 1 ),
	                                              std::nullptr_t> = nullptr>
	void do_not_optimize( Values &&... values ) {
		if constexpr( sizeof...( Values ) > 0 ) {
			(void)( ( do_not_optimize( std::forward<Values>( values ) ), 1 ) | ... );
		}
	}
} // namespace daw
