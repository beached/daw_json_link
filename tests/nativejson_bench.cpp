// The MIT License (MIT)
//
// Copyright (c) 2019 Darrell Wright
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

#include <iostream>
#include <fstream>
#include <streambuf>

#include <daw/daw_benchmark.h>
#include <daw/daw_string_view.h>
#include <daw/json/daw_json_link.h>

#include "canada_test.h"
#include "citm_test.h"
#include "twitter_test.h"

namespace {
	std::string load_json_data( daw::string_view file_path ) {
		auto file = std::ifstream( file_path.data( ) );
		assert( file );
		return std::string( std::istreambuf_iterator<char>( file ), std::istreambuf_iterator<char>( ) );
	}
}

int main( int argc, char **argv ) {
	using namespace daw::json;
	if( argc < 4 ) {
		std::cerr << "Must supply a filenames to open\n";
		std::cerr << "twitter citm canada\n";
		exit( 1 );
	}

	auto const json_data1 = load_json_data( argv[1] );
	auto const json_data2 = load_json_data( argv[2] );
	auto const json_data3 = load_json_data( argv[3] );
	auto json_sv1 = std::string_view( json_data1.data( ), json_data1.size( ) );
	auto json_sv2 = std::string_view( json_data2.data( ), json_data2.size( ) );
	auto json_sv3 = std::string_view( json_data3.data( ), json_data3.size( ) );

	auto const sz = json_sv1.size( ) + json_sv2.size( ) + json_sv3.size( );
	std::cout << "Processing: " << daw::utility::to_bytes_per_second( sz )
	          << '\n';

	std::cout << std::flush;

	daw::bench_n_test_mbs<10>(
	  "nativejson_twitter bench", json_sv1.size( ),
	  []( auto f1 ) {
		  auto j1 = daw::json::from_json<twitter_object_t>( f1 );
		  daw::do_not_optimize( j1 );
	  },
	  json_sv1 );

	std::cout << std::flush;

	daw::bench_n_test_mbs<10>(
	  "nativejson_citm bench", json_sv2.size( ),
	  []( auto f2 ) {
		  auto j2 = daw::json::from_json<citm_object_t>( f2 );
		  daw::do_not_optimize( j2 );
	  },
	  json_sv2 );

	std::cout << std::flush;

	daw::bench_n_test_mbs<10>(
	  "nativejson_canada bench", json_sv3.size( ),
	  []( auto f3 ) {
		  auto j3 = daw::json::from_json<canada_object_t>( f3 );
		  daw::do_not_optimize( j3 );
	  },
	  json_sv3 );

	std::cout << std::flush;

		daw::bench_n_test_mbs<100>(
	  "nativejson bench", sz,
	  []( auto f1, auto f2, auto f3 ) {
		  auto j1 = daw::json::from_json<twitter_object_t>( f1 );
		  daw::do_not_optimize( j1 );
		  auto j2 = daw::json::from_json<citm_object_t>( f2 );
		  daw::do_not_optimize( j2 );
		  auto j3 = daw::json::from_json<canada_object_t>( f3 );
		  daw::do_not_optimize( j3 );
	  },
	  json_sv1, json_sv2, json_sv3 );

}
