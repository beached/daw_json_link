// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/daw_json_link
//

#include "daw/json/daw_json_iterator.h"
#include "daw/json/daw_json_link.h"

#include <daw/daw_benchmark.h>
#include <daw/daw_memory_mapped_file.h>

#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <string>
#include <vector>

int main( int argc, char **argv ) try {
	if( argc <= 1 ) {
		puts( "Must supply path to strings.json file\n" );
		exit( EXIT_FAILURE );
	}
	std::string const json_string = [argv] {
		auto const data = daw::filesystem::memory_mapped_file_t<>( argv[1] );
		return std::string( data.data( ), data.size( ) );
	}( );

	auto const json_data = static_cast<std::string_view>( json_string );
	using namespace daw::json;
	using String = std::string_view;
	daw::bench_n_test_mbs<1000>(
	  "strings.json checked", json_data.size( ),
	  []( auto sv ) {
		  auto range = json_array_range<String>( sv );
		  for( auto v : range ) {
			  daw::do_not_optimize( v );
		  }
	  },
	  json_data );

	daw::bench_n_test_mbs<1000>(
	  "strings.json unchecked", json_data.size( ),
	  []( auto sv ) {
		  auto range =
		    json_array_range<String, NoCommentSkippingPolicyUnchecked>( sv );
		  for( auto v : range ) {
			  daw::do_not_optimize( v );
		  }
	  },
	  json_data );
} catch( daw::json::json_exception const &jex ) {
	std::cerr << "Exception thrown by parser: " << jex.reason( ) << std::endl;
	exit( 1 );
}
