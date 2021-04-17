// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/daw_json_link
//

#include "defines.h"

#include "citm_test_json.h"
#include "geojson_json.h"
#include "twitter_test_json.h"

#include <daw/daw_benchmark.h>
#include <daw/daw_read_file.h>
#include <daw/json/daw_from_json.h>

#include <cstdlib>
#include <iostream>
#include <string>

#if not defined( DAW_NUM_RUNS )
#if not defined( DEBUG ) or defined( NDEBUG )
static inline constexpr std::size_t DAW_NUM_RUNS = 250;
#else
static inline constexpr std::size_t DAW_NUM_RUNS = 2;
#endif
#endif
static_assert( DAW_NUM_RUNS > 0 );

template<typename ExecTag>
void test( char **argv ) {
	auto const json_data1 = *daw::read_file( argv[1] );
	auto const json_data2 = *daw::read_file( argv[2] );
	auto const json_data3 = *daw::read_file( argv[3] );
	auto json_sv1 = std::string_view( json_data1.data( ), json_data1.size( ) );
	auto json_sv2 = std::string_view( json_data2.data( ), json_data2.size( ) );
	auto json_sv3 = std::string_view( json_data3.data( ), json_data3.size( ) );

	std::cout << "Using " << ExecTag::name
	          << " exec model\n*********************************************\n";
	auto const sz = json_sv1.size( ) + json_sv2.size( ) + json_sv3.size( );
	std::cout << "Processing: " << daw::utility::to_bytes_per_second( sz )
	          << '\n';

	std::cout << std::flush;

	std::optional<daw::twitter::twitter_object_t> twitter_result{ };
	std::optional<daw::citm::citm_object_t> citm_result{ };
	std::optional<daw::geojson::Polygon> canada_result{ };
	try {
		daw::bench_n_test_mbs<DAW_NUM_RUNS>(
		  "nativejson_twitter bench", json_sv1.size( ),
		  [&twitter_result]( auto f1 ) {
			  twitter_result = daw::json::from_json<
			    daw::twitter::twitter_object_t,
			    daw::json::SIMDNoCommentSkippingPolicyChecked<ExecTag>>( f1 );
		  },
		  json_sv1 );
		daw::do_not_optimize( twitter_result );
	} catch( daw::json::json_exception const &jex ) {
		std::cerr << "Error while testing twitter.json\n";
		std::cerr << to_formatted_string( jex ) << '\n';
	}
	test_assert( twitter_result, "Missing value -> twitter_result" );
	test_assert( not twitter_result->statuses.empty( ),
	             "Expected values: twitter_result is empty" );
	test_assert( twitter_result->statuses.front( ).user.id == 1186275104,
	             std::string( "Expected values: user_id had wrong value, "
	                          "expected 1186275104.  Got " ) +
	               std::to_string( twitter_result->statuses.front( ).user.id ) );
	twitter_result.reset( );

	std::cout << std::flush;

	daw::bench_n_test_mbs<DAW_NUM_RUNS>(
	  "nativejson_twitter bench trusted", json_sv1.size( ),
	  [&twitter_result]( auto f1 ) {
		  {
			  twitter_result = daw::json::from_json<
			    daw::twitter::twitter_object_t,
			    daw::json::SIMDNoCommentSkippingPolicyUnchecked<ExecTag>>( f1 );
		  }
	  },
	  json_sv1 );
	daw::do_not_optimize( twitter_result );
	test_assert( twitter_result, "Missing value" );
	test_assert( not twitter_result->statuses.empty( ),
	             "Expected values: twitter_result is empty" );
	test_assert( twitter_result->statuses.front( ).user.id == 1186275104,
	             "Expected values" );
	twitter_result.reset( );

	std::cout << std::flush;

	daw::bench_n_test_mbs<DAW_NUM_RUNS>(
	  "nativejson_citm bench", json_sv2.size( ),
	  [&citm_result]( auto f2 ) {
		  citm_result = daw::json::from_json<
		    daw::citm::citm_object_t,
		    daw::json::SIMDNoCommentSkippingPolicyChecked<ExecTag>>( f2 );
	  },
	  json_sv2 );
	daw::do_not_optimize( citm_result );
	test_assert( citm_result, "Missing value" );
	test_assert( not citm_result->areaNames.empty( ), "Expected values" );
	test_assert( citm_result->areaNames.count( 205706005 ) == 1,
	             "Expected value" );
	test_assert( citm_result->areaNames[205706005] == "1er balcon jardin",
	             "Incorrect value" );
	citm_result.reset( );

	std::cout << std::flush;

	daw::bench_n_test_mbs<DAW_NUM_RUNS>(
	  "nativejson_citm bench trusted", json_sv2.size( ),
	  [&citm_result]( auto f2 ) {
		  citm_result = daw::json::from_json<
		    daw::citm::citm_object_t,
		    daw::json::SIMDNoCommentSkippingPolicyUnchecked<ExecTag>>( f2 );
	  },
	  json_sv2 );
	test_assert( citm_result, "Missing value" );
	test_assert( not citm_result->areaNames.empty( ), "Expected values" );
	test_assert( citm_result->areaNames.count( 205706005 ) == 1,
	             "Expected value" );
	test_assert( citm_result->areaNames[205706005] == "1er balcon jardin",
	             "Incorrect value" );
	citm_result.reset( );

	std::cout << std::flush;

	daw::bench_n_test_mbs<DAW_NUM_RUNS>(
	  "nativejson_canada bench", json_sv3.size( ),
	  [&canada_result]( auto f3 ) {
		  canada_result = daw::json::from_json<
		    daw::geojson::Polygon,
		    daw::json::SIMDNoCommentSkippingPolicyChecked<ExecTag>>(
		    f3, "features[0].geometry" );
	  },
	  json_sv3 );
	daw::do_not_optimize( canada_result );
	test_assert( canada_result, "Missing value" );
	canada_result.reset( );

	std::cout << std::flush;

	daw::bench_n_test_mbs<DAW_NUM_RUNS>(
	  "nativejson_canada bench trusted", json_sv3.size( ),
	  [&canada_result]( auto f3 ) {
		  canada_result = daw::json::from_json<
		    daw::geojson::Polygon,
		    daw::json::SIMDNoCommentSkippingPolicyUnchecked<ExecTag>>(
		    f3, "features[0].geometry" );
	  },
	  json_sv3 );
	daw::do_not_optimize( canada_result );
	test_assert( canada_result, "Missing value" );
	canada_result.reset( );

	std::cout << std::flush;

	daw::bench_n_test_mbs<DAW_NUM_RUNS>(
	  "nativejson bench", sz,
	  [&]( auto f1, auto f2, auto f3 ) {
		  twitter_result = daw::json::from_json<
		    daw::twitter::twitter_object_t,
		    daw::json::SIMDNoCommentSkippingPolicyChecked<ExecTag>>( f1 );
		  citm_result = daw::json::from_json<
		    daw::citm::citm_object_t,
		    daw::json::SIMDNoCommentSkippingPolicyChecked<ExecTag>>( f2 );
		  canada_result = daw::json::from_json<
		    daw::geojson::Polygon,
		    daw::json::SIMDNoCommentSkippingPolicyChecked<ExecTag>>(
		    f3, "features[0].geometry" );
	  },
	  json_sv1, json_sv2, json_sv3 );

	std::cout << std::flush;

	daw::do_not_optimize( twitter_result );
	daw::do_not_optimize( citm_result );
	daw::do_not_optimize( canada_result );
	test_assert( twitter_result, "Missing value" );
	test_assert( not twitter_result->statuses.empty( ),
	             "Expected values: twitter_result is empty" );
	test_assert( twitter_result->statuses.front( ).user.id == 1186275104,
	             "Missing value" );
	test_assert( citm_result, "Missing value" );
	test_assert( not citm_result->areaNames.empty( ), "Expected values" );
	test_assert( citm_result->areaNames.count( 205706005 ) == 1,
	             "Expected value" );
	test_assert( citm_result->areaNames[205706005] == "1er balcon jardin",
	             "Incorrect value" );
	test_assert( canada_result, "Missing value" );
	twitter_result.reset( );
	citm_result.reset( );
	canada_result.reset( );

	daw::bench_n_test_mbs<DAW_NUM_RUNS>(
	  "nativejson bench trusted", sz,
	  [&]( auto f1, auto f2, auto f3 ) {
		  twitter_result = daw::json::from_json<
		    daw::twitter::twitter_object_t,
		    daw::json::SIMDNoCommentSkippingPolicyUnchecked<ExecTag>>( f1 );
		  citm_result = daw::json::from_json<
		    daw::citm::citm_object_t,
		    daw::json::SIMDNoCommentSkippingPolicyUnchecked<ExecTag>>( f2 );
		  canada_result = daw::json::from_json<
		    daw::geojson::Polygon,
		    daw::json::SIMDNoCommentSkippingPolicyUnchecked<ExecTag>>(
		    f3, "features[0].geometry" );
	  },
	  json_sv1, json_sv2, json_sv3 );

	std::cout << std::flush;

	daw::do_not_optimize( twitter_result );
	daw::do_not_optimize( citm_result );
	daw::do_not_optimize( canada_result );

	test_assert( twitter_result, "Missing value" );
	test_assert( not twitter_result->statuses.empty( ), "Expected values" );
	test_assert( twitter_result->statuses.front( ).user.id == 1186275104,
	             "Missing value" );
	test_assert( citm_result, "Missing value" );
	test_assert( not citm_result->areaNames.empty( ), "Expected values" );
	test_assert( citm_result->areaNames.count( 205706005 ) == 1,
	             "Expected value" );
	test_assert( citm_result->areaNames[205706005] == "1er balcon jardin",
	             "Incorrect value" );
	test_assert( canada_result, "Missing value" );
}

int main( int argc, char **argv )
#ifdef DAW_USE_JSON_EXCEPTIONS
  try
#endif
{
	try {
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
		test<daw::json::constexpr_exec_tag>( argv );

		if constexpr( not std::is_same_v<daw::json::simd_exec_tag,
		                                 daw::json::runtime_exec_tag> ) {
			test<daw::json::runtime_exec_tag>( argv );
		}
		test<daw::json::simd_exec_tag>( argv );

	} catch( daw::json::json_exception const &je ) {
		std::cerr << "Unexpected error while testing: " << je.reason( ) << '\n';
		exit( EXIT_FAILURE );
	}
}
catch( daw::json::json_exception const &jex ) {
	std::cerr << "Exception thrown by parser: " << jex.reason( ) << std::endl;
	exit( 1 );
}
