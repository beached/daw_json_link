// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/daw_json_link
//

#include "defines.h"

#include "twitter_test2.h"

#include <daw/daw_benchmark.h>
#include <daw/daw_memory_mapped_file.h>
#include <daw/daw_string_view.h>
#include <daw/json/daw_json_link.h>

#include <fstream>
#include <iostream>
#include <streambuf>

int main( int argc, char **argv ) {
	if( argc < 2 ) {
		std::cerr << "Must supply a file name\n";
		exit( 1 );
	}
	using namespace daw::json;
	auto const json_data1 = daw::filesystem::memory_mapped_file_t<>( argv[1] );
	auto const json_sv1 =
	  std::string_view( json_data1.data( ), json_data1.size( ) );
	try {
		auto const twitter_result =
		  daw::json::from_json<daw::twitter::twitter_object_t>( json_sv1 );
		daw::do_not_optimize( twitter_result );
		daw_json_assert( twitter_result.statuses.size( ) > 0, "Expected values" );
		daw_json_assert( twitter_result.statuses.front( ).user.id == "1186275104",
		                 "Missing value" );
	} catch( daw::json::json_exception const &jex ) {
		std::cerr << "Exception thrown by parser: " << to_formatted_string( jex )
		          << '\n';
		if( jex.parse_location( ) ) {
			std::cerr << "Error happened around "
			          << ( jex.parse_location( ) - json_sv1.data( ) )
			          << " bytes into file\n";
		}
		exit( 1 );
	}
}
