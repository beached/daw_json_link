// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/daw_json_link
//

#include "geojson.h"

#include <daw/daw_do_not_optimize.h>
#include <daw/daw_memory_mapped_file.h>
#include <daw/daw_string_view.h>
#include <daw/json/daw_json_link.h>

#include <cstdio>
#include <iostream>

int main( int argc, char **argv ) try {
	if( argc < 2 ) {
		puts( "Must supply a file name\n" );
		exit( 1 );
	}
	using namespace daw::json;
	auto json_data = std::string(daw::filesystem::memory_mapped_file_t<>( argv[1] ) );

	auto const canada_result =
	  daw::json::from_json<daw::geojson::FeatureCollection>( json_data );
	daw::do_not_optimize( canada_result );

	auto const new_json_result = daw::json::to_json( canada_result );
	daw::do_not_optimize( canada_result );
} catch( daw::json::json_exception const &jex ) {
	std::cerr << "Exception thrown by parser: " << jex.reason( ) << std::endl;
	exit( 1 );
}
