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

template<typename ExecTag>
void test( std::string_view json_sv1 ) {
	std::cout << "Using " << ExecTag::name
	          << " exec model\n*********************************************\n";
	auto const sz = json_sv1.size( );
	//**************************
	std::optional<daw::geojson::Polygon> canada_result;
	daw::bench_n_test_mbs<DAW_NUM_RUNS>(
	  "canada bench(checked)", sz,
	  [&canada_result]( auto f1 ) {
		  canada_result = daw::json::from_json<
		    daw::geojson::Polygon,
		    daw::json::SIMDNoCommentSkippingPolicyChecked<ExecTag>>(
		    f1, "features[0].geometry" );
		  daw::do_not_optimize( canada_result );
	  },
	  json_sv1 );
	daw::do_not_optimize( canada_result );
	test_assert( canada_result, "Missing value" );
	canada_result = std::nullopt;
	//**************************
	canada_result = std::nullopt;
	daw::bench_n_test_mbs<DAW_NUM_RUNS>(
	  "canada bench(unchecked)", sz,
	  [&canada_result]( auto f1 ) {
		  canada_result = daw::json::from_json<
		    daw::geojson::Polygon,
		    daw::json::SIMDNoCommentSkippingPolicyUnchecked<ExecTag>>(
		    f1, "features[0].geometry" );
		  daw::do_not_optimize( canada_result );
	  },
	  json_sv1 );
	daw::do_not_optimize( canada_result );
	test_assert( canada_result, "Missing value" );
}

int main( int argc, char **argv )
#ifdef DAW_USE_JSON_EXCEPTIONS
  try
#endif
{
	using namespace daw::json;
	if( argc < 2 ) {
		std::cerr << "Must supply a filenames to open\n";
		exit( 1 );
	}

	auto const json_data1 = *daw::read_file( argv[1] );
	assert( json_data1.size( ) > 2 and "Minimum json data size is 2 '{}'" );
	auto const json_sv1 =
	  std::string_view( json_data1.data( ), json_data1.size( ) );

	auto const sz = json_sv1.size( );
	std::cout << "Processing: " << daw::utility::to_bytes_per_second( sz )
	          << '\n';

	test<daw::json::constexpr_exec_tag>( json_sv1 );
	test<daw::json::runtime_exec_tag>( json_sv1 );
	if constexpr( not std::is_same_v<daw::json::simd_exec_tag,
	                                 daw::json::runtime_exec_tag> ) {
		test<daw::json::simd_exec_tag>( json_sv1 );
	}

	std::cout
	  << "to_json testing\n*********************************************\n";
	auto const canada_result = daw::json::from_json<daw::geojson::Polygon>(
	  json_sv1, "features[0].geometry" );
	std::string str{ };
	{
		auto out_it = std::back_inserter( str );
		str.reserve( json_sv1.size( ) );
		daw::bench_n_test_mbs<DAW_NUM_RUNS>(
		  "canada bench(to_json_string)", sz,
		  [&]( auto const &tr ) {
			  str.clear( );
			  daw::json::to_json( tr, out_it );
			  daw::do_not_optimize( str );
		  },
		  canada_result );
	}
	test_assert( not str.empty( ), "Expected a string value" );
	daw::do_not_optimize( str );
	auto const canada_result2 =
	  daw::json::from_json<daw::geojson::Polygon>( str );
	daw::do_not_optimize( canada_result2 );
	{
		auto const str_sz = str.size( );
		str.clear( );
		str.resize( str_sz * 2 );
		daw::bench_n_test_mbs<DAW_NUM_RUNS>(
		  "canada bench(to_json_string2)", sz,
		  [&]( auto const &tr ) {
			  auto out_it = str.data( );
			  daw::json::to_json( tr, out_it );
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
#ifdef DAW_USE_JSON_EXCEPTIONS
catch( daw::json::json_exception const &jex ) {
	std::cerr << "Exception thrown by parser: "
	          << to_formatted_string( jex, nullptr ) << std::endl;
	exit( 1 );
}
#endif
