// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/daw_json_link
//

#include "citm_test.h"

#include <daw/daw_benchmark.h>
#include <daw/daw_memory_mapped_file.h>
#include <daw/daw_string_view.h>
#include <daw/json/daw_json_link.h>

#include <fstream>
#include <iostream>
#include <streambuf>

#if defined( NDEBUG ) and not defined( DEBUG )
#define NUMRUNS 250
#else
#define NUMRUNS 10
#endif

int main( int argc, char **argv ) try {
	using namespace daw::json;
	if( argc < 2 ) {
		std::cerr << "Must supply a filenames to open\n";
		exit( 1 );
	}

	auto const json_data1 = daw::filesystem::memory_mapped_file_t<>( argv[1] );
	auto const json_sv1 =
	  std::string_view( json_data1.data( ), json_data1.size( ) );

	auto const sz = json_sv1.size( );
	std::cout << "Processing: " << daw::utility::to_bytes_per_second( sz )
	          << '\n';

	auto citm_result = daw::bench_n_test_mbs<NUMRUNS>(
	  "citm_catalog bench", sz,
	  []( auto f1 ) {
		  return daw::json::from_json<daw::citm::citm_object_t>( f1 );
	  },
	  json_sv1 );
	daw::do_not_optimize( citm_result );
	daw_json_assert( citm_result, "Missing value" );
	daw_json_assert( not citm_result->areaNames.empty( ), "Expected values" );
	daw_json_assert( citm_result->areaNames.count( 205706005 ) == 1,
	                 "Expected value" );
	daw_json_assert( citm_result->areaNames[205706005] == "1er balcon jardin",
	                 "Incorrect value" );

	std::string str{ };
	auto out_it = std::back_inserter( str );
	str.reserve( json_sv1.size( ) );
	daw::bench_n_test_mbs<100>(
	  "citm bench(to_json_string)", sz,

	  [&]( daw::citm::citm_object_t const &tr ) {
		  str.clear( );
		  daw::json::to_json( tr, out_it );
		  daw::do_not_optimize( str );
	  },
	  *citm_result );
	daw_json_assert( not str.empty( ), "Expected a string value" );
	daw::do_not_optimize( str );
} catch( daw::json::json_exception const &jex ) {
	std::cerr << "Exception thrown by parser: " << jex.reason( ) << std::endl;
	exit( 1 );
}
