// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/daw_json_link
//
//  This test will test the parsing canada_test.json file
//  in the test_data folder.  This is one of the many common JSON parsing
//  benchmark files and was included in the nativejson benchmark and this file
//  provides a cleaner way to profile without the benchmarking harness
//

#include "geojson.h"

#include <daw/daw_do_not_optimize.h>
#include <daw/daw_read_file.h>
#include <daw/daw_string_view.h>
//#include <daw/json/daw_json_find_path.h>
#include <daw/json/daw_json_link.h>

#include <cstdio>
#include <iostream>

int main( int argc, char **argv ) {
	if( argc < 2 ) {
		puts( "Must supply a file name\n" );
		exit( 1 );
	}
	using namespace daw::json;
	auto json_data = std::string( *daw::read_file( argv[1] ) );
#ifdef DAW_USE_JSON_EXCEPTIONS
	try {
#endif
		auto const canada_result =
		  daw::json::from_json<daw::geojson::FeatureCollection>( json_data );
		daw::do_not_optimize( canada_result );

		auto new_json_result = std::string( );
		new_json_result.resize( ( json_data.size( ) * 15U ) / 10U );
		auto last = daw::json::to_json( canada_result, new_json_result.data( ) );
		(void)last;
		// new_json_result.resize( std::distance( new_json_result.data( ), last ) );
		daw::do_not_optimize( canada_result );
#ifdef DAW_USE_JSON_EXCEPTIONS
	} catch( daw::json::json_exception const &jex ) {
		std::cerr << "Exception thrown by parser: "
		          << to_formatted_string( jex, json_data.data( ) );
		std::cerr << "The error is about "
		          << ( jex.parse_location( ) - json_data.data( ) ) << " bytes in\n"
		          << '\n';
		exit( 1 );
	}
#endif
}
