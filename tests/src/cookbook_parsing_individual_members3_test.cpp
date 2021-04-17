// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/daw_json_link
//

#include "defines.h"

#include "daw/json/daw_json_link.h"

#include <daw/daw_read_file.h>

#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <string>
#include <unordered_map>

int main( int argc, char **argv )
#ifdef DAW_USE_JSON_EXCEPTIONS
  try
#endif
{
	if( argc <= 1 ) {
		puts(
		  "Must supply path to cookbook_parsing_individual_members1.json file\n" );
		exit( EXIT_FAILURE );
	}

	auto const file_data = *daw::read_file( argv[1] );
	auto const json_data =
	  std::string_view( file_data.data( ), file_data.size( ) );

	using namespace daw::json;
	auto const value = from_json<std::string_view>( json_data, "member1[1]" );

	test_assert( value == "is", "Unexpected value" );

	std::string_view opt_value =
	  from_json<json_string_raw_null<no_name, std::string_view>>(
	    json_data, "member1[1000]" );

	test_assert( opt_value.empty( ), "Unexpected result" );
}
catch( daw::json::json_exception const &jex ) {
	std::cerr << "Exception thrown by parser: " << jex.reason( ) << std::endl;
	exit( 1 );
}
