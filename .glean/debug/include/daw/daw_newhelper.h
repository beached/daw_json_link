// The MIT License (MIT)
//
// Copyright (c) 2013-2020 Darrell Wright
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

#include <exception>
#include <memory>

#include "daw_exception.h"
#include "daw_string.h"

template<typename T, typename... Args>
[[nodiscard]] T *new_throw( Args &&... args ) {
	std::unique_ptr<T> result;
	try {
		result = std::make_unique<T>( std::forward<Args>( args )... );
	} catch( ... ) { std::rethrow_exception( std::current_exception( ) ); }
	daw::exception::daw_throw_on_false( result, "Error allocating" );
	return result.release( );
}

template<typename T>
[[nodiscard]] T *new_array_throw( const size_t size ) {
	T *result = nullptr;
	try {
		result = new T[size];
	} catch( ... ) { std::rethrow_exception( std::current_exception( ) ); }
	daw::exception::daw_throw_on_null(
	  result, daw::string::string_join( "Error allocating ", size, " items" ) );
	return result;
}
