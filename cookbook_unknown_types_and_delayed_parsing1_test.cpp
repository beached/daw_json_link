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
		puts(
		  "Must supply path to cookbook_unknown_types_and_delayed_parsing1.json "
		  "file\n" );
		exit( EXIT_FAILURE );
	}
	auto data = *daw::read_file( argv[1] );

	auto val =
	  daw::json::json_value( std::string_view( data.data( ), data.size( ) ) );
	for( auto member : val ) {
		if( member.name ) {
			std::cout << *member.name << " is a ";
		} else {
			std::cout << "Array element is a ";
		}
		std::cout << member.value.is_number( );
		std::cout << to_string( member.value.type( ) ) << " with a value of "
		          << member.value.get_string_view( ) << '\n';
	}
}
catch( daw::json::json_exception const &jex ) {
	std::cerr << "Exception thrown by parser: " << jex.reason( ) << std::endl;
	exit( 1 );
}
