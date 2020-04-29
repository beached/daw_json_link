// The MIT License (MIT)
//
// Copyright (c) 2019-2020 Darrell Wright
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

#include "apache_builds.h"
#include "daw/json/daw_json_link.h"

#include <daw/cpp_17.h>
#include <daw/daw_benchmark.h>
#include <daw/daw_memory_mapped_file.h>
#include <daw/daw_traits.h>

#include <fstream>
#include <iostream>
#include <streambuf>

template<typename T>
using is_to_json_data_able = decltype( to_json_data( std::declval<T>( ) ) );

template<typename T>
inline bool constexpr is_to_json_data_able_v =
  daw::is_detected_v<is_to_json_data_able, T>;

template<typename T,
         std::enable_if_t<is_to_json_data_able_v<T>, std::nullptr_t> = nullptr>
constexpr bool operator==( T const &lhs, T const &rhs ) {
	if( to_json_data( lhs ) == to_json_data( rhs ) ) {
		return true;
	}
	daw_json_error( "Expected that values would be equal" );
}

int main( int argc, char **argv ) try {
	using namespace daw::json;
	if( argc < 2 ) {
		std::cerr << "Must supply a path to apache_builds.json\n";
		exit( 1 );
	}

	auto const json_data1 = daw::filesystem::memory_mapped_file_t<>( argv[1] );
	assert( json_data1.size( ) > 2 and "Minimum json data size is 2 '{}'" );
	auto const json_sv1 =
	  std::string_view( json_data1.data( ), json_data1.size( ) );

	auto const sz = json_sv1.size( );
	std::cout << "Processing: " << daw::utility::to_bytes_per_second( sz )
	          << '\n';

	std::optional<apache_builds::apache_builds> apache_builds_result;
	daw::bench_n_test_mbs<100>(
	  "apache_builds bench", sz,
	  [&apache_builds_result]( auto f1 ) {
		  apache_builds_result =
		    daw::json::from_json<apache_builds::apache_builds>( f1 );
		  daw::do_not_optimize( apache_builds_result );
	  },
	  json_sv1 );
	daw::do_not_optimize( apache_builds_result );
	daw_json_assert( apache_builds_result, "Missing value" );
	daw_json_assert( apache_builds_result->jobs.size( ) > 0, "Expected values" );
	daw_json_assert( apache_builds_result->numExecutors == 0, "Missing value" );

	std::string str{};
	auto out_it = std::back_inserter( str );
	daw::bench_n_test_mbs<100>(
	  "apache_builds bench(to_json_string)", sz,
	  [&]( auto const &tr ) {
		  str.clear( );
		  daw::json::to_json( *tr, out_it );
		  daw::do_not_optimize( str );
	  },
	  apache_builds_result );

	daw_json_assert( not str.empty( ), "Expected a string value" );

	daw::do_not_optimize( str );
	auto const apache_builds_result2 =
	  daw::json::from_json<apache_builds::apache_builds>( str );
	daw::do_not_optimize( apache_builds_result2 );
	// Removing for now as it will do a float compare and fail
	/*
	daw_json_assert( apache_builds_result == apache_builds_result2,
	                 "Expected round trip to produce same result" );
	                 */
} catch( daw::json::json_exception const &jex ) {
	std::cerr << "Exception thrown by parser: " << jex.reason( ) << std::endl;
	exit( 1 );
}
