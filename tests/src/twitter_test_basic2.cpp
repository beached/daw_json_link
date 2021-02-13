// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/daw_json_link
//

#include "defines.h"

#include "twitter_test2_json.h"

#include <daw/daw_benchmark.h>
#include <daw/daw_read_file.h>
#include <daw/json/daw_from_json.h>

#include <iostream>
#include <string_view>

int main( int argc, char **argv ) {
	if( argc < 2 ) {
		std::cerr << "Must supply a file name\n";
		exit( 1 );
	}
	using namespace daw::json;
	auto const json_data1 = *daw::read_file( argv[1] );
	auto const json_sv1 =
	  std::string_view( json_data1.data( ), json_data1.size( ) );
	try {
		auto const twitter_result =
		  daw::json::from_json<daw::twitter2::twitter_object_t>( json_sv1 );
		daw::do_not_optimize( twitter_result );
		test_assert( not twitter_result.statuses.empty( ), "Expected values" );
		test_assert( twitter_result.statuses.front( ).user.id == "1186275104",
		             "Missing value" );
	} catch( daw::json::json_exception const &jex ) {
		std::cerr << "Exception thrown by parser: "
		          << to_formatted_string( jex, json_data1.data( ) ) << '\n';
		if( jex.parse_location( ) ) {
			std::cerr << "Error happened around "
			          << ( jex.parse_location( ) - json_sv1.data( ) )
			          << " bytes into file\n";
		}
		exit( 1 );
	}
}
