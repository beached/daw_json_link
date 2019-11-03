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
#include <string_view>

#include <daw/daw_benchmark.h>

#include "canada_test.h"
#include "citm_test.h"
#include "daw/json/daw_json_link.h"
#include "daw/json/impl/daw_memory_mapped.h"
#include "twitter_test.h"

int main( int argc, char **argv ) {
	using namespace daw::json;
	if( argc < 4 ) {
		std::cerr << "Must supply a filenames to open\n";
		std::cerr << "twitter citm canada\n";
		exit( 1 );
	}

	auto const mm_twitter = daw::memory_mapped_file<>( argv[1] );
	auto const mm_citm = daw::memory_mapped_file<>( argv[2] );
	auto const mm_canada = daw::memory_mapped_file<>( argv[3] );
	auto const sv_twitter =
	  std::string_view( mm_twitter.data( ), mm_twitter.size( ) );
	auto const sv_citm =
	  std::string_view( mm_citm.data( ), mm_citm.size( ) );
	auto const sv_canada =
	  std::string_view( mm_canada.data( ), mm_canada.size( ) );

#ifdef NDEBUG
	std::cout << "non-debug run\n";
	auto const sz = sv_twitter.size( ) + sv_citm.size( ) + sv_canada.size( );
	daw::bench_n_test_mbs<250>(
	  "nativejson bench", sz,
	  [&]( auto f1, auto f2, auto f3 ) {
		  daw::do_not_optimize( daw::json::from_json<twitter_object_t>( f1 ) );
		  daw::do_not_optimize( daw::json::from_json<citm_object_t>( f2 ) );
		  daw::do_not_optimize( daw::json::from_json<canada_object_t>( f3 ) );
	  },
	  sv_twitter, sv_citm, sv_canada );
#else
	for( size_t n = 0; n < 25; ++n ) {
		daw::do_not_optimize( sv_twitter );
		daw::do_not_optimize( sv_citm );
		daw::do_not_optimize( sv_canada );
		daw::do_not_optimize( daw::json::from_json_trusted<twitter_object_t>( sv_twitter ) );
		daw::do_not_optimize( daw::json::from_json_trusted<citm_object_t>( sv_citm ) );
		daw::do_not_optimize( daw::json::from_json_trusted<canada_object_t>( sv_canada ) );
	}
#endif
}
