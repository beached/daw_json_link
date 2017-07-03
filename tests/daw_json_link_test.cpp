// The MIT License (MIT)
//
// Copyright (c) 2017 Darrell Wright
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

#include <cstdint>
#include <cstdlib>
#include <iostream>

#include "daw_json_link.h"

struct A: public daw::json::daw_json_link<A> {
	int a;
	double b;

	static void json_link_map( ) {
		using std::to_string;
		link_json_integer( "a", []( A &obj, int64_t value ) { obj.a = std::move( value ); },
						   []( A const &obj ) { return to_string( obj.a ); } );
		link_json_real( "b", []( A &obj, double value ) { obj.b = std::move( value ); },
		                   []( A const &obj ) { return to_string( obj.b ); } );
	}
};

int main( int argc, char **argv ) {
	boost::string_view str = "{ \"a\" : 5, \"b\" : 6.6 }";
	auto a = A::from_json_string( str.begin( ), str.end( ) ).result;
	std::cout << a.to_json_string( ) << '\n';

	return EXIT_SUCCESS;
}


