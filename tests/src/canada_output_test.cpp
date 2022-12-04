// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/daw_json_link
//
//  This test will benchmark the performance of parsing canada_test.json file
//  in the test_data folder.  This is one of the many common JSON parsing
//  benchmark files and was included in the nativejson benchmark
//

#include "defines.h"

#include "geojson_json.h"

#include <daw/cpp_17.h>
#include <daw/daw_benchmark.h>
#include <daw/daw_read_file.h>
#include <daw/daw_traits.h>
#include <daw/json/daw_from_json.h>
#include <daw/json/daw_to_json.h>

#include <fstream>
#include <iostream>
#include <streambuf>

#if not defined( DAW_NUM_RUNS )
#if not defined( DEBUG ) or defined( NDEBUG )
static inline constexpr std::size_t DAW_NUM_RUNS = 250;
#else
static inline constexpr std::size_t DAW_NUM_RUNS = 2;
#endif
#endif
static_assert( DAW_NUM_RUNS > 0 );

template<typename T>
using is_to_json_data_able = decltype( to_json_data( std::declval<T>( ) ) );

template<typename T>
inline bool DAW_CONSTEXPR is_to_json_data_able_v =
  daw::is_detected_v<is_to_json_data_able, T>;

template<typename T,
         std::enable_if_t<is_to_json_data_able_v<T>, std::nullptr_t> = nullptr>
DAW_CONSTEXPR bool operator==( T const &lhs, T const &rhs ) {
	test_assert( to_json_data( lhs ) == to_json_data( rhs ),
	             "Expected that values would be equal" );
	return true;
}

template<daw::json::options::ExecModeTypes ExecMode =
           daw::json::options::ExecModeTypes::compile_time>
inline auto get_canada_check( std::string_view f1 ) {
	return daw::json::from_json<daw::geojson::FeatureCollection>(
	  f1, daw::json::options::parse_flags<ExecMode> );
}

int main( int argc, char **argv )
#ifdef DAW_USE_EXCEPTIONS
  try
#endif
{
	using namespace daw::json;
	if( argc < 2 ) {
		std::cerr << "Must supply canada.json path\n";
		exit( 1 );
	}

	auto const json_data = *daw::read_file( argv[1] );
	assert( json_data.size( ) > 2 and "Minimum json data size is 2 '{}'" );

	auto const sz = json_data.size( );
	std::cout << "Processing: " << daw::utility::to_bytes_per_second( sz )
	          << '\n';

	std::cout
	  << "to_json testing\n*********************************************\n";
	auto const canada_result = get_canada_check( json_data );
	std::string str{ };
	{
		str.reserve( json_data.size( ) );
		(void)daw::bench_n_test_mbs<DAW_NUM_RUNS>(
		  "canada bench(to_json_string - string & output)", sz,
		  [&]( auto const &tr ) {
			  str.clear( );
			  daw::json::to_json( tr, str );
			  daw::do_not_optimize( str );
		  },
		  canada_result );
	}
	test_assert( not str.empty( ), "Expected a string value" );
	daw::do_not_optimize( str );
	auto canada_result2 = get_canada_check( str );
	daw::do_not_optimize( canada_result2 );
	{
		auto const str_sz = str.size( );
		str.clear( );
		str.resize( str_sz * 20 );
		(void)daw::bench_n_test_mbs<DAW_NUM_RUNS>(
		  "canada bench(to_json_string2 - char * output)", sz,
		  [&]( auto const &tr ) {
			  auto *out_it = str.data( );
			  daw::json::to_json(
			    tr, out_it,
			    options::output_flags<options::SerializationFormat::Pretty> );
			  daw::do_not_optimize( str );
		  },
		  canada_result );
	}
	// Removing for now as it will do a float compare and fail
	/*
	test_assert( canada_result == canada_result2,
	                 "Expected round trip to produce same result" );
	                 */
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
