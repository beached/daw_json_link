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

#include "geojson_json.h"

#include <daw/daw_do_not_optimize.h>
#include <daw/daw_read_file.h>
#include <daw/daw_string_view.h>
#include <daw/json/daw_from_json.h>
#include <daw/json/daw_to_json.h>

#include <cstdio>

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
		for( int n = 0; n < 100; ++n ) {
			auto const canada_result = daw::json::from_json<daw::geojson::Polygon>(
			  json_data, "features[0].geometry" );
			daw::do_not_optimize( canada_result );

			auto new_json_result = std::string( );
			new_json_result.resize( ( json_data.size( ) * 15U ) / 10U );
			auto last = daw::json::to_json( canada_result, new_json_result.data( ) );
			(void)last;
			// new_json_result.resize( std::distance( new_json_result.data( ), last )
			// );
			daw::do_not_optimize( canada_result );
		}
#ifdef DAW_USE_JSON_EXCEPTIONS
	} catch( daw::json::json_exception const &jex ) {
		display_exception( jex, json_data.data( ) );
		exit( 1 );
	}
#endif
}
