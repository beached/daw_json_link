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

#include <algorithm>
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
#ifdef DAW_USE_EXCEPTIONS
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
	static constexpr auto policy_v =
	  daw::json::options::parse_flags<daw::json::options::IEEE754Precise::yes>;
	std::cout << "C++ DAW JSON Link\n";
	auto const canada_obj = daw::json::from_json<daw::geojson::Polygon>(
	  canada_data, "features[0].geometry", policy_v );
	auto const twitter_obj = daw::json::from_json<daw::twitter::twitter_object_t>(
	  twitter_data, policy_v );
	auto const citm_obj =
	  daw::json::from_json<daw::citm::citm_object_t>( citm_data, policy_v );

	auto const canada_out = daw::json::to_json( canada_obj );
	auto const twitter_out = daw::json::to_json( twitter_obj );
	auto const citm_out = daw::json::to_json( citm_obj );

	auto const canada_obj2 =
	  daw::json::from_json<daw::geojson::Polygon>( canada_out, policy_v );
	auto const twitter_obj2 =
	  daw::json::from_json<daw::twitter::twitter_object_t>( twitter_out,
	                                                        policy_v );
	auto const citm_obj2 =
	  daw::json::from_json<daw::citm::citm_object_t>( citm_out, policy_v );

	auto const canada_out2 = daw::json::to_json( canada_obj2 );
	auto const twitter_out2 = daw::json::to_json( twitter_obj2 );
	auto const citm_out2 = daw::json::to_json( citm_obj2 );

	std::cout << "Maybe say bad due to FP equality\n";
	std::cout << "canada roundtrip: "
	          << ( canada_out == canada_out2 ? "good" : "bad" ) << '\n';

	std::cout << "twitter roundtrip: "
	          << ( twitter_out == twitter_out2 ? "good" : "bad" ) << '\n';

	//	std::cout << twitter_out << "\n\n\n";
	//	std::cerr << twitter_out2 << "\n\n\n";
	if( twitter_out != twitter_out2 ) {
		if( twitter_out.size( ) != twitter_out2.size( ) ) {
			std::cout << "\tsize mismatch";
		}
		auto pos = std::mismatch(
		  std::data( twitter_out ),
		  std::data( twitter_out ) +
		    std::min( std::size( twitter_out ), std::size( twitter_out2 ) ),
		  std::data( twitter_out2 ) );
		std::cout << ( pos.first - std::data( twitter_out ) ) << " char's in\n";
	}

	std::cout << "citm roundtrip: " << ( citm_out == citm_out2 ? "good" : "bad" )
	          << '\n';

	std::cout << "stop";

	return EXIT_SUCCESS;
}
#ifdef DAW_USE_EXCEPTIONS
catch( daw::json::json_exception const &jex ) {
	std::cerr << "Exception thrown by parser: " << jex.reason( ) << '\n';
	exit( 1 );
} catch( std::exception const &ex ) {
	std::cerr << "Unknown exception thrown during testing: " << ex.what( )
	          << '\n';
	exit( 1 );
} catch( ... ) {
	std::cerr << "Unknown exception thrown during testing\n";
	throw;
}
#endif
