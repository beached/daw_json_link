// The MIT License (MIT)
//
// Copyright (c) 2020 Darrell Wright
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files( the "Software" ), to
// deal in the Software without restriction, including without limitation the
// rights to use, copy, modify, merge, publish, distribute, sublicense, and / or
// sell copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#include "citm_test.h"
#include "geojson.h"
#include "twitter_test.h"

#include <daw/json/daw_json_iterator.h>
#include <daw/json/daw_json_link.h>

#include <fstream>
#include <iostream>
#include <sstream>
#include <string_view>

std::string read_file( std::string const &filename ) {
	std::ifstream f( filename );
	if( !f ) {
		return {};
	}
	return std::string( std::istreambuf_iterator<char>( f ),
	                    std::istreambuf_iterator<char>( ) );
}

int main( int argc, char *argv[] ) try {
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
	auto const canada_obj =
	  daw::json::from_json<daw::geojson::FeatureCollection>( canada_data );
	auto const twitter_obj =
	  daw::json::from_json<daw::twitter::twitter_object_t>( twitter_data );
	auto const citm_obj =
	  daw::json::from_json<daw::citm::citm_object_t>( citm_data );

	auto const canada_out = daw::json::to_json( canada_obj );
	auto const twitter_out = daw::json::to_json( twitter_obj );
	auto const citm_out = daw::json::to_json( citm_obj );

	auto const canada_obj2 =
	  daw::json::from_json<daw::geojson::FeatureCollection>( canada_out );
	auto const twitter_obj2 =
	  daw::json::from_json<daw::twitter::twitter_object_t>( twitter_out );
	auto const citm_obj2 =
	  daw::json::from_json<daw::citm::citm_object_t>( citm_out );

	auto const canada_out2 = daw::json::to_json( canada_obj2 );
	auto const twitter_out2 = daw::json::to_json( twitter_obj2 );
	auto const citm_out2 = daw::json::to_json( citm_obj2 );

	std::cout << "canada roundtrip: "
	          << ( canada_out == canada_out2 ? "good" : "bad" ) << '\n';
	std::cout << "twitter roundtrip: "
	          << ( twitter_out == twitter_out2 ? "good" : "bad" ) << '\n';
	std::cout << "citm roundtrip: " << ( citm_out == citm_out2 ? "good" : "bad" )
	          << '\n';

	std::cout << "stop";

	return EXIT_SUCCESS;
} catch( daw::json::json_exception const &jex ) {
	std::cerr << "Exception thrown by parser: " << jex.reason( ) << std::endl;
	exit( 1 );
}
