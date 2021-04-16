// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/daw_json_link
//

#include "defines.h"

#include "citm_test_json.h"
#include "geojson_json.h"
#include "twitter_test_json.h"

#include <daw/json/daw_from_json.h>
#include <daw/json/daw_to_json.h>

#include <fstream>
#include <iostream>
#include <sstream>
#include <string_view>

std::string read_file( std::string const &filename ) {
	std::ifstream f( filename );
	if( !f ) {
		return { };
	}
	return std::string( std::istreambuf_iterator<char>( f ),
	                    std::istreambuf_iterator<char>( ) );
}

int main( int argc, char *argv[] )
#ifdef DAW_USE_JSON_EXCEPTIONS
  try
#endif
{
#if defined( NDEBUG ) and not defined( DEBUG )
	std::cout << "release run\n";
#else
	std::cout << "debug run\n";
#endif
	if( argc < 4 ) {
		std::cerr << "Must supply a filenames to open\n";
		std::cerr << "twitter citm canada\n";
		exit( 1 );
	}

	std::string const twitter_data = read_file( argv[1] );
	std::string const citm_data = read_file( argv[2] );
	std::string const canada_data = read_file( argv[3] );

	std::cout << "C++ DAW JSON Link\n";
	auto const canada_obj = daw::json::from_json<daw::geojson::Polygon>(
	  canada_data, "features[0].geometry" );
	auto const twitter_obj =
	  daw::json::from_json<daw::twitter::twitter_object_t>( twitter_data );
	auto const citm_obj =
	  daw::json::from_json<daw::citm::citm_object_t>( citm_data );

	auto const canada_out = daw::json::to_json( canada_obj );
	auto const twitter_out = daw::json::to_json( twitter_obj );
	auto const citm_out = daw::json::to_json( citm_obj );

	auto const canada_obj2 =
	  daw::json::from_json<daw::geojson::Polygon>( canada_out );
	auto const twitter_obj2 =
	  daw::json::from_json<daw::twitter::twitter_object_t>( twitter_out );
	auto const citm_obj2 =
	  daw::json::from_json<daw::citm::citm_object_t>( citm_out );

	auto const canada_out2 = daw::json::to_json( canada_obj2 );
	auto const twitter_out2 = daw::json::to_json( twitter_obj2 );
	auto const citm_out2 = daw::json::to_json( citm_obj2 );

	std::cout << "Maybe say bad due to FP equality\n";
	std::cout << "canada roundtrip: "
	          << ( canada_out == canada_out2 ? "good" : "bad" ) << '\n';
	std::cout << "twitter roundtrip: "
	          << ( twitter_out == twitter_out2 ? "good" : "bad" ) << '\n';
	std::cout << "citm roundtrip: " << ( citm_out == citm_out2 ? "good" : "bad" )
	          << '\n';

	std::cout << "stop";

	return EXIT_SUCCESS;
}
catch( daw::json::json_exception const &jex ) {
	std::cerr << "Exception thrown by parser: " << jex.reason( ) << std::endl;
	exit( 1 );
}
