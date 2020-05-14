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
#include <cstdio>
#include <utility>

#include "daw_exchange.h"

namespace daw {
	class CFile {
		FILE *ptr = nullptr;

	public:
		constexpr CFile( ) noexcept = default;
		explicit constexpr CFile( FILE *f ) noexcept
		  : ptr( f ) {}

		constexpr CFile( CFile &&other ) noexcept
		  : ptr( exchange( other.ptr, nullptr ) ) {}

		constexpr CFile &operator=( CFile &&rhs ) noexcept {
			if( this != &rhs ) {
				ptr = exchange( rhs.ptr, nullptr );
			}
			return *this;
		}

		constexpr FILE *get( ) const noexcept {
			return ptr;
		}
		constexpr FILE *release( ) noexcept {
			return exchange( ptr, nullptr );
		}

		constexpr void close( ) {
			if( auto tmp = exchange( ptr, nullptr ); tmp ) {
				fclose( tmp );
			}
		}
		~CFile( ) {
			close( );
		}
	};
} // namespace daw
