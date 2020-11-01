// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/daw_json_link
//
// See cookbook/array.md for the 1st example
//

#include "defines.h"

#include "daw/json/daw_json_link.h"

#include <daw/daw_read_file.h>

#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <string>
#include <vector>

int main( int argc, char **argv )
#ifdef DAW_USE_JSON_EXCEPTIONS
  try
#endif
{
	if( argc <= 1 ) {
		puts( "Must supply path to cookbook_array1.json file\n" );
		exit( EXIT_FAILURE );
	}
	auto const data = *daw::read_file( argv[1] );

	using namespace daw::json;

	std::vector<int> const ve =
	  from_json_array<int>( { data.data( ), data.size( ) } );

	int count = 1;
	(void)count;
	for( auto val : ve ) {
		(void)val;
		test_assert( count++ == val, "Unexpected value" );
	}
}
#ifdef DAW_USE_JSON_EXCEPTIONS
catch( daw::json::json_exception const &jex ) {
	std::cerr << "Exception thrown by parser: " << jex.reason( ) << std::endl;
	exit( 1 );
}
#endif
