// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/daw_json_link
//
//  This test will test the parsing citm_catalog.json file
//  in the test_data folder.  This is one of the many common JSON parsing
//  benchmark files and was included in the nativejson benchmark and this file
//  provides a cleaner way to profile without the benchmarking harness
//

#include "defines.h"

#include "citm_test_json.h"

#include <daw/daw_benchmark.h>
#include <daw/daw_read_file.h>
#include <daw/daw_string_view.h>
#include <daw/json/daw_from_json.h>

#include <fstream>
#include <iostream>
#include <streambuf>

int main( int argc, char **argv )
#ifdef DAW_USE_JSON_EXCEPTIONS
  try
#endif
{
	if( argc < 2 ) {
		std::cerr << "Must supply a filenames to open\n";
		exit( 1 );
	}
	using namespace daw::json;
	auto const json_data1 = *daw::read_file( argv[1] );
	auto const json_sv1 =
	  std::string_view( json_data1.data( ), json_data1.size( ) );

	auto citm_result = daw::json::from_json<daw::citm::citm_object_t>( json_sv1 );
	daw::do_not_optimize( citm_result );
	test_assert( not citm_result.areaNames.empty( ), "Expected values" );
	test_assert( citm_result.areaNames.count( 205706005 ) == 1,
	             "Expected value" );
	test_assert( citm_result.areaNames[205706005] == "1er balcon jardin",
	             "Incorrect value" );
}
#ifdef DAW_USE_JSON_EXCEPTIONS
catch( daw::json::json_exception const &jex ) {
	std::cerr << "Exception thrown by parser: " << jex.reason( ) << std::endl;
	exit( 1 );
}
#endif
