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

#include <fstream>
#include <iostream>
#include <streambuf>

#include <daw/daw_benchmark.h>
#include <daw/daw_string_view.h>

#include "canada_test.h"
#include "citm_test.h"
#include "daw/json/daw_json_link.h"
#include "daw/json/impl/daw_memory_mapped.h"
#include "twitter_test.h"

#if defined( NDEBUG ) and not defined( DEBUG )
#define NUMRUNS 250
#else
#define NUMRUNS 50
#endif

int main( int argc, char **argv ) {
	using namespace daw::json;
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

	auto const json_data1 = daw::memory_mapped_file<>( argv[1] );
	auto const json_data2 = daw::memory_mapped_file<>( argv[2] );
	auto const json_data3 = daw::memory_mapped_file<>( argv[3] );
	auto json_sv1 = std::string_view( json_data1.data( ), json_data1.size( ) );
	auto json_sv2 = std::string_view( json_data2.data( ), json_data2.size( ) );
	auto json_sv3 = std::string_view( json_data3.data( ), json_data3.size( ) );

	auto const sz = json_sv1.size( ) + json_sv2.size( ) + json_sv3.size( );
	std::cout << "Processing: " << daw::utility::to_bytes_per_second( sz )
	          << '\n';

	std::cout << std::flush;

	std::optional<twitter_object_t> j1{};
	std::optional<citm_object_t> j2{};
	std::optional<canada_object_t> j3{};
	daw::bench_n_test_mbs<NUMRUNS>(
	  "nativejson_twitter bench", json_sv1.size( ),
	  [&j1]( auto f1 ) { j1 = daw::json::from_json<twitter_object_t>( f1 ); },
	  json_sv1 );
	daw::do_not_optimize( j1 );

	std::cout << std::flush;

	daw::bench_n_test_mbs<NUMRUNS>(
	  "nativejson_twitter bench trusted", json_sv1.size( ),
	  [&j1]( auto f1 ) {
		  j1 = daw::json::from_json_trusted<twitter_object_t>( f1 );
	  },
	  json_sv1 );
	daw::do_not_optimize( j1 );

	std::cout << std::flush;

	daw::bench_n_test_mbs<NUMRUNS>(
	  "nativejson_citm bench", json_sv2.size( ),
	  [&j2]( auto f2 ) { j2 = daw::json::from_json<citm_object_t>( f2 ); },
	  json_sv2 );
	daw::do_not_optimize( j2 );

	std::cout << std::flush;

	daw::bench_n_test_mbs<NUMRUNS>(
	  "nativejson_citm bench trusted", json_sv2.size( ),
	  [&j2]( auto f2 ) {
		  j2 = daw::json::from_json_trusted<citm_object_t>( f2 );
	  },
	  json_sv2 );
	daw::do_not_optimize( j2 );

	std::cout << std::flush;

	daw::bench_n_test_mbs<NUMRUNS>(
	  "nativejson_canada bench", json_sv3.size( ),
	  [&j3]( auto f3 ) { j3 = daw::json::from_json<canada_object_t>( f3 ); },
	  json_sv3 );
	daw::do_not_optimize( j3 );

	std::cout << std::flush;

	daw::bench_n_test_mbs<NUMRUNS>(
	  "nativejson_canada bench trusted", json_sv3.size( ),
	  [&j3]( auto f3 ) {
		  j3 = daw::json::from_json_trusted<canada_object_t>( f3 );
	  },
	  json_sv3 );
	daw::do_not_optimize( j3 );

	std::cout << std::flush;

	daw::bench_n_test_mbs<NUMRUNS>(
	  "nativejson bench", sz,
	  [&]( auto f1, auto f2, auto f3 ) {
		  j1 = daw::json::from_json<twitter_object_t>( f1 );
		  j2 = daw::json::from_json<citm_object_t>( f2 );
		  j3 = daw::json::from_json<canada_object_t>( f3 );
	  },
	  json_sv1, json_sv2, json_sv3 );

	std::cout << std::flush;

	daw::do_not_optimize( j1 );
	daw::do_not_optimize( j2 );
	daw::do_not_optimize( j3 );

	daw::bench_n_test_mbs<NUMRUNS>(
	  "nativejson bench trusted", sz,
	  [&]( auto f1, auto f2, auto f3 ) {
		  j1 = daw::json::from_json_trusted<twitter_object_t>( f1 );
		  j2 = daw::json::from_json_trusted<citm_object_t>( f2 );
		  j3 = daw::json::from_json_trusted<canada_object_t>( f3 );
	  },
	  json_sv1, json_sv2, json_sv3 );

	std::cout << std::flush;

	daw::do_not_optimize( j1 );
	daw::do_not_optimize( j2 );
	daw::do_not_optimize( j3 );
}
