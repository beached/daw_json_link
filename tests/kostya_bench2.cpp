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

#include <daw/json/daw_json_iterator.h>
#include <daw/json/daw_json_link.h>
#include <daw/json/impl/daw_stateful_json_value.h>

#include <fstream>
#include <iostream>
#include <sstream>
#include <string_view>
#include <unistd.h>

struct coordinate_t {
	double x;
	double y;
	double z;
	// ignore string name
	// ignore object opts
};

std::string read_file( std::string const &filename ) {
	std::ifstream f( filename );
	if( !f ) {
		return { };
	}
	return std::string( std::istreambuf_iterator<char>( f ),
	                    std::istreambuf_iterator<char>( ) );
}

int main( int argc, char *argv[] ) {
	std::string const text = read_file( "/tmp/1.json" );

	auto const json_sv = std::string_view( text.data( ), text.size( ) );
	double x = 0, y = 0, z = 0;
	int len = 0;

	using namespace daw::json;
	auto rng =
	  from_json_unchecked<json_delayed<no_name>>( json_sv, "coordinates" );

	auto val = json_value_state( );
	for( auto item : rng ) {
		val.reset( item.value );
		x += from_json_unchecked<double>( val["x"] );
		y += from_json_unchecked<double>( val["y"] );
		z += from_json_unchecked<double>( val["z"] );
		++len;
	}
	std::cout << x / len << '\n';
	std::cout << y / len << '\n';
	std::cout << z / len << '\n';

	return EXIT_SUCCESS;
}
