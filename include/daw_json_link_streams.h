// The MIT License (MIT)
//
// Copyright (c) 2017-2018 Darrell Wright
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files( the "Software" ), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and / or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#pragma once

#include <daw/daw_heap_array.h>

#include "daw_json_link.h"

template<typename Derived>
std::ostream &operator<<( std::ostream &os, daw::json::daw_json_link<Derived> const &obj ) {
	os << obj.to_json_string( );
	return os;
}

template<typename Derived>
std::istream &operator>>( std::istream &is, daw::json::daw_json_link<Derived> const &obj ) {
	// TODO find a way, if possible, to get a char const * from istream
	auto const data_size = static_cast<size_t>( is.rdbuf( )->in_avail( ) );
	daw::heap_array<char> temporary( data_size );
	is.read( temporary.data( ), static_cast<std::streamsize>( data_size ) );
	obj.from_json_string( daw::string_view{temporary.data( ), temporary.size( )} );
	return is;
}
