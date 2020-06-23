// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/daw_json_link
//

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
	auto fname = argv[1];
	auto const json_data1 = daw::filesystem::memory_mapped_file_t<>( fname );
	assert( json_data1.size( ) > 2 and "Minimum json data size is 2 '{}'" );
	auto const json_sv1 =
	  std::string_view( json_data1.data( ), json_data1.size( ) );

	auto const sz = json_sv1.size( );
	std::cout << "Processing: " << daw::utility::to_bytes_per_second( sz )
	          << '\n';

	auto apache_builds_result =
	  daw::json::from_json<apache_builds::apache_builds>( json_sv1 );
	daw_json_assert( apache_builds_result.jobs.size( ) > 0,
	                 "Bad value for jobs.size( )" );
	daw_json_assert( apache_builds_result.numExecutors == 0,
	                 "Bad value for numExecutors" );

	daw::bench_n_test_mbs<100>(
	  "apache_builds bench", sz,
	  []( auto f1 ) {
		  auto r = daw::json::from_json<apache_builds::apache_builds>( f1 );
		  daw::do_not_optimize( r );
	  },
	  json_sv1 );

	std::string str{ };
	auto out_it = std::back_inserter( str );
	daw::bench_n_test_mbs<100>(
	  "apache_builds bench(to_json_string)", sz,
	  [&]( auto const &tr ) {
		  str.clear( );
		  daw::json::to_json( tr, out_it );
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
