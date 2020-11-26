// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/daw_json_link
//

#include "defines.h"

#include <daw/json/daw_json_iterator.h>
#include <daw/json/daw_json_link.h>

#include <fstream>
#include <iostream>
#include <libnotify.hpp>
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

struct coordinates_t {
	std::vector<coordinate_t> coordinates;
};

namespace daw::json {
	template<>
	struct json_data_contract<coordinate_t> {
#ifdef __cpp_nontype_template_parameter_class
		using type =
		  json_member_list<json_number<"x">, json_number<"y">, json_number<"z">>;
#else
		constexpr inline static char const x[] = "x";
		constexpr inline static char const y[] = "y";
		constexpr inline static char const z[] = "z";
		using type =
		  json_member_list<json_number<x>, json_number<y>, json_number<z>>;
#endif
	};

	template<>
	struct json_data_contract<coordinates_t> {
#ifdef __cpp_nontype_template_parameter_class
		using type = json_member_list<json_array<"coordinates", coordinate_t>>;
#else
		constexpr inline static char const coordinates[] = "coordinates";
		using type = json_member_list<json_array<coordinates, coordinate_t>>;
#endif
	};
} // namespace daw::json

std::string read_file( std::string const &filename ) {
	std::ifstream f( filename );
	if( !f ) {
		return { };
	}
	return std::string( std::istreambuf_iterator<char>( f ),
	                    std::istreambuf_iterator<char>( ) );
}

int main( int argc, char *argv[] )
#ifdef DAW_USE_JSON_EXCEPTIONS
  try
#endif
{
	std::string const text = read_file( "/tmp/1.json" );

	auto const json_sv = std::string_view( text.data( ), text.size( ) );
	double x = 0, y = 0, z = 0;
	int len = 0;

	{
		std::stringstream ostr;
		ostr << "C++ DAW JSON Link\t" << getpid( );
		notify( ostr.str( ) );
	}
	using range_t = daw::json::json_array_range<coordinate_t, true>;
	auto rng = range_t( json_sv, "coordinates" );

	for( auto c : rng ) {
		++len;
		x += c.x;
		y += c.y;
		z += c.z;
	}
	std::cout << x / len << '\n';
	std::cout << y / len << '\n';
	std::cout << z / len << '\n';

	notify( "stop" );

	return EXIT_SUCCESS;
}
catch( daw::json::json_exception const &jex ) {
	std::cerr << "Exception thrown by parser: " << jex.reason( ) << std::endl;
	exit( 1 );
}
