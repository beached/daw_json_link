// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/daw_json_link
//

#include <daw/json/daw_json_iterator.h>
#include <daw/json/daw_json_link.h>
#include <daw/json/daw_json_value_state.h>

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
		return {};
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
		x += from_json_unchecked<double>( val[0] );
		y += from_json_unchecked<double>( val[1] );
		z += from_json_unchecked<double>( val[2] );
		/*
		x += from_json_unchecked<double>( val["x"] );
		y += from_json_unchecked<double>( val["y"] );
		z += from_json_unchecked<double>( val["z"] );
		 */
		++len;
	}
	std::cout << x / len << '\n';
	std::cout << y / len << '\n';
	std::cout << z / len << '\n';

	return EXIT_SUCCESS;
}
