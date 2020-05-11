// The MIT License (MIT)
//
// Copyright (c) Darrell Wright
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
#include "twitter_test2.h"

#include "daw/json/daw_json_link.h"

#include <daw/daw_benchmark.h>
#include <daw/daw_memory_mapped_file.h>

#include <fstream>
#include <iostream>
#include <streambuf>
#include <string_view>

int main( int argc, char **argv ) try {
	using namespace daw::json;
	if( argc < 4 ) {
		std::cerr << "Must supply a filenames to open\n";
		std::cerr << "twitter citm canada\n";
		exit( 1 );
	}

	auto const mm_twitter = daw::filesystem::memory_mapped_file_t<>( argv[1] );
	auto const mm_citm = daw::filesystem::memory_mapped_file_t<>( argv[2] );
	auto const mm_canada = daw::filesystem::memory_mapped_file_t<>( argv[3] );
	auto const sv_twitter =
	  std::string_view( mm_twitter.data( ), mm_twitter.size( ) );
	auto const sv_citm = std::string_view( mm_citm.data( ), mm_citm.size( ) );
	auto const sv_canada =
	  std::string_view( mm_canada.data( ), mm_canada.size( ) );

	std::optional<daw::twitter::twitter_object_t> j1{};
	std::optional<daw::citm::citm_object_t> j2{};
	std::optional<daw::geojson::FeatureCollection> j3{};
#ifdef NDEBUG
	std::cout << "non-debug run\n";
	auto const sz = sv_twitter.size( ) + sv_citm.size( ) + sv_canada.size( );
	daw::bench_n_test_mbs<250>(
	  "nativejson bench", sz,
	  [&]( auto f1, auto f2, auto f3 ) {
		  j1 = daw::json::from_json_unchecked<daw::twitter::twitter_object_t>( f1 );
		  j2 = daw::json::from_json_unchecked<daw::citm::citm_object_t>( f2 );
		  j3 =
		    daw::json::from_json_unchecked<daw::geojson::FeatureCollection>( f3 );
		  daw::do_not_optimize( sv_twitter );
		  daw::do_not_optimize( sv_citm );
		  daw::do_not_optimize( sv_canada );
		  daw::do_not_optimize( j1 );
		  daw::do_not_optimize( j2 );
		  daw::do_not_optimize( j3 );
	  },
	  sv_twitter, sv_citm, sv_canada );
#else
	for( size_t n = 0; n < 25; ++n ) {
		j1 = daw::json::from_json_unchecked<daw::twitter::twitter_object_t>(
		  sv_twitter );
		j2 = daw::json::from_json_unchecked<daw::citm::citm_object_t>( sv_citm );
		j3 = daw::json::from_json_unchecked<daw::geojson::FeatureCollection>(
		  sv_canada );
		daw::do_not_optimize( sv_twitter );
		daw::do_not_optimize( sv_citm );
		daw::do_not_optimize( sv_canada );
		daw::do_not_optimize( j1 );
		daw::do_not_optimize( j2 );
		daw::do_not_optimize( j3 );
	}
#endif
	if( not j1 ) {
		daw_json_error( "Missing value" );
	}
	if( not j2 ) {
		daw_json_error( "Missing value" );
	}
	if( not j3 ) {
		daw_json_error( "Missing value" );
	}
} catch( daw::json::json_exception const &jex ) {
	std::cerr << "Exception thrown by parser: " << jex.reason( ) << std::endl;
	exit( 1 );
}
