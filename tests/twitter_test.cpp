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

#include <daw/json/daw_json_link.h>
#include <iostream>

#include <daw/daw_benchmark.h>
#include <daw/daw_memory_mapped_file.h>
#include <daw/daw_string_view.h>

#include "twitter_test.h"

int main( int argc, char **argv ) {
	using namespace daw::json;
	if( argc < 2 ) {
		std::cerr << "Must supply a filename to open\n";
		exit( 1 );
	}
	auto const json_data = daw::filesystem::memory_mapped_file_t<char>( argv[1] );
	auto json_sv = std::string_view( json_data.data( ), json_data.size( ) );

	std::cout << "Processing: " << daw::utility::to_bytes_per_second( json_sv.size( ) ) << '\n';
	auto const s =
	  *daw::bench_n_test_mbs<10>( "twitter status", json_sv.size( ),
	                              [&]( auto sv ) noexcept {
																	auto t = daw::json::from_json<root_object_t>( sv );
																	daw::do_not_optimize( t );
																	return t;
	                              },
	                              json_sv );
	daw::do_not_optimize( s );
}
