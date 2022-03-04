// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/daw_json_link
//

#include "defines.h"

#include <daw/daw_read_file.h>

#include "daw/json/daw_json_link.h"

#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <string>

int main( int argc, char **argv )
#ifdef DAW_USE_JSON_EXCEPTIONS
  try
#endif
{
	if( argc <= 1 ) {
		puts( "Must supply path to cookbook_kv4.json file\n" );
		exit( EXIT_FAILURE );
	}
	auto json_data = *daw::read_file( argv[1] );

	using namespace daw::json;
	auto val = json_value( json_data );
	std::multimap<std::string, std::string> kv =
	  from_json<json_key_value<no_name, std::multimap<std::string, std::string>,
	                           std::string>>( val );
	test_assert( kv.size( ) == 2, "Expected data to have 2 items" );
	test_assert( kv.begin( )->first == std::prev( kv.end( ) )->first,
	             "Unexpected value" );
	test_assert( kv.begin( )->second != std::prev( kv.end( ) )->second,
	             "Unexpected value" );
}
catch( daw::json::json_exception const &jex ) {
	std::cerr << "Exception thrown by parser: " << jex.reason( ) << std::endl;
	exit( 1 );
}
