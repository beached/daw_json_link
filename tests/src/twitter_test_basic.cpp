// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/daw_json_link
//

#include "defines.h"

#include "twitter_test_json.h"

#include <daw/daw_do_not_optimize.h>
#include <daw/daw_read_file.h>
#include <daw/json/daw_from_json.h>

#include <iostream>
#include <streambuf>

int main( int argc, char **argv )
#ifdef DAW_USE_JSON_EXCEPTIONS
  try
#endif
{
	if( argc < 2 ) {
		std::cerr << "Must supply a file name\n";
		exit( 1 );
	}
	using namespace daw::json;
	std::string const json_data = [argv] {
		auto const mmf = *daw::read_file( argv[1] );
		return std::string( mmf.data( ), mmf.size( ) );
	}( );

	daw::twitter::twitter_object_t twitter_result;
	for( std::size_t n = 0; n < 1000; ++n ) {
		daw::do_not_optimize( json_data );
		twitter_result =
		  daw::json::from_json<daw::twitter::twitter_object_t>( json_data );
		daw::do_not_optimize( twitter_result );
	}
	test_assert( not twitter_result.statuses.empty( ), "Expected values" );
	test_assert( twitter_result.statuses.front( ).user.id == 1186275104,
	             "Missing value" );
}
#ifdef DAW_USE_JSON_EXCEPTIONS
catch( daw::json::json_exception const &jex ) {
	std::cerr << "Exception thrown by parser: " << jex.reason( ) << std::endl;
	exit( 1 );
}
#endif
