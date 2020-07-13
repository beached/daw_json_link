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
	std::string const json_data = [argv] {
		auto const data = daw::filesystem::memory_mapped_file_t<>( argv[1] );
		return std::string( data.data( ), data.size( ) );
	}( );
	auto json_sv = static_cast<std::string_view>( json_data );
	using namespace daw::json;
	using String = std::string;
	{
		std::vector<String> ve;
		daw::bench_n_test_mbs<250>(
		  "strings.json checked", json_sv.size( ),
		  [&ve]( auto sv ) {
			  ve.clear( );
			  auto range = json_array_range<String>( sv );
			  ve.insert( ve.end( ), range.begin( ), range.end( ) );
		  },
		  json_sv );
		daw::do_not_optimize( ve );
	}

	{
		std::vector<std::string> ve;
		daw::bench_n_test_mbs<250>(
		  "strings.json unchecked", json_sv.size( ),
		  [&ve]( auto sv ) {
			  ve.clear( );
			  auto range =
			    json_array_range<String, NoCommentSkippingPolicyUnchecked>( sv );
			  ve.insert( ve.end( ), range.begin( ), range.end( ) );
		  },
		  json_sv );

		daw::do_not_optimize( ve );
	}
} catch( daw::json::json_exception const &jex ) {
	std::cerr << "Exception thrown by parser: " << jex.reason( ) << std::endl;
	exit( 1 );
}
