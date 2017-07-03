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
	bool c;
	std::string d;

	static void json_link_map( ) {
		link_json_integer_fn( "a", []( A & obj, int value ) { obj.a = std::move( value ); },
							  []( A const & obj ) { return obj.a; } );
		link_json_real_fn( "b", []( A & obj, double value ) { obj.b = std::move( value ); },
						   []( A const & obj ) { return obj.b; } );
		link_json_boolean_fn( "c", []( A & obj, bool value ) { obj.c = std::move( value ); },
							  []( A const & obj ) { return obj.c; } );
		link_json_string_fn( "d", []( A & obj, std::string value ) { obj.d = std::move( value ); },
							 []( A const & obj ) { return obj.d; } );
	}
};
struct A2 {
	int a;
	double b;
	bool c;
	std::string d;
};

struct B: public daw::json::daw_json_link<B> {
	A a;

	static void json_link_map( ) {
		link_json_object_fn( "a", []( B & obj, A value ) { obj.a = std::move( value ); },
							 []( B const & obj ) { return obj.a; } );
	}
};

int main( int argc, char **argv ) {
	std::cout << "Size of linked class->" << sizeof( A ) << " vs size of unlinked->" << sizeof( A2 ) << '\n';
	boost::string_view str = "{ \"a\": { \"a\" : 5, \"b\" : 6.6, \"c\" : true, \"d\": \"hello\" }}";
	auto a = B::from_json_string( str.begin( ), str.end( ) ).result;
	std::cout << a.to_json_string( ) << '\n';

	return EXIT_SUCCESS;
}


