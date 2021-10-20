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
#include <daw/json/daw_json_link.h>

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

using namespace daw::json;

// GCC-8 bug workaround
#if not( defined( __GNUC__ ) and __GNUC__ < 9 )
template<typename ExecTag, bool expect_long_strings>
using unchecked_policy_t = BasicParsePolicy<parse_options(
  ( expect_long_strings ? ExpectLongStrings::yes : ExpectLongStrings::no ),
  CheckedParseMode::no,
  json_details::exec_mode_from_tag<ExecTag> )>;
template<typename ExecTag, bool expect_long_strings>
using checked_policy_t = BasicParsePolicy<parse_options(
  ( expect_long_strings ? ExpectLongStrings::yes : ExpectLongStrings::no ),
  json_details::exec_mode_from_tag<ExecTag> )>;
#else
template<typename ExecTag, bool expect_long_strings>
using unchecked_policy_t =
  BasicParsePolicy<parse_options( CheckedParseMode::no,
                                  json_details::exec_mode_from_tag<ExecTag> )>;
template<typename ExecTag, bool expect_long_strings>
using checked_policy_t =
  BasicParsePolicy<parse_options( json_details::exec_mode_from_tag<ExecTag> )>;
#endif

template<typename ExecTag>
void test( char **argv ) {
	auto const json_sv1 = *daw::read_file( argv[1] );
	auto const json_sv2 = *daw::read_file( argv[2] );
	auto const json_sv3 = *daw::read_file( argv[3] );

	std::cout << "Using " << ExecTag::name
	          << " exec model\n*********************************************\n";
	auto const sz =
	  std::size( json_sv1 ) + std::size( json_sv2 ) + std::size( json_sv3 );
	std::cout << "Processing: " << daw::utility::to_bytes_per_second( sz )
	          << '\n';

	std::cout << std::flush;

	std::optional<daw::twitter::twitter_object_t> twitter_result{ };
	std::optional<daw::citm::citm_object_t> citm_result{ };
	std::optional<daw::geojson::Polygon> canada_result{ };

#ifdef DAW_USE_EXCEPTIONS
	try
#endif
	{
		daw::bench_n_test_mbs<DAW_NUM_RUNS>(
		  "nativejson_twitter bench",
		  std::size( json_sv1 ),
		  [&twitter_result]( auto const &f1 ) {
			  twitter_result = from_json<daw::twitter::twitter_object_t,
			                             checked_policy_t<ExecTag, true>>( f1 );
		  },
		  json_sv1 );
		daw::do_not_optimize( twitter_result );
	}
#ifdef DAW_USE_EXCEPTIONS
	catch( json_exception const &jex ) {
		std::cerr << "Error while testing twitter.json\n";
		std::cerr << to_formatted_string( jex ) << '\n';
	}
#endif
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
	  "nativejson_twitter bench trusted",
	  std::size( json_sv1 ),
	  [&twitter_result]( auto const &f1 ) {
		  {
			  twitter_result = from_json<daw::twitter::twitter_object_t,
			                             checked_policy_t<ExecTag, true>>( f1 );
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
	  "nativejson_citm bench",
	  std::size( json_sv2 ),
	  [&citm_result]( auto const &f2 ) {
		  citm_result =
		    from_json<daw::citm::citm_object_t, checked_policy_t<ExecTag, true>>(
		      f2 );
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
	  "nativejson_citm bench trusted",
	  std::size( json_sv2 ),
	  [&citm_result]( auto const &f2 ) {
		  citm_result =
		    from_json<daw::citm::citm_object_t, checked_policy_t<ExecTag, true>>(
		      f2 );
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
	  "nativejson_canada bench",
	  std::size( json_sv3 ),
	  [&canada_result]( auto const &f3 ) {
		  canada_result =
		    from_json<daw::geojson::Polygon, checked_policy_t<ExecTag, false>>(
		      f3,
		      "features[0].geometry" );
	  },
	  json_sv3 );
	daw::do_not_optimize( canada_result );
	test_assert( canada_result, "Missing value" );
	canada_result.reset( );

	std::cout << std::flush;

	daw::bench_n_test_mbs<DAW_NUM_RUNS>(
	  "nativejson_canada bench trusted",
	  std::size( json_sv3 ),
	  [&canada_result]( auto const &f3 ) {
		  canada_result =
		    from_json<daw::geojson::Polygon, checked_policy_t<ExecTag, false>>(
		      f3,
		      "features[0].geometry" );
	  },
	  json_sv3 );
	daw::do_not_optimize( canada_result );
	test_assert( canada_result, "Missing value" );
	canada_result.reset( );

	std::cout << std::flush;

	daw::bench_n_test_mbs<DAW_NUM_RUNS>(
	  "nativejson bench",
	  sz,
	  [&]( auto const &f1, auto const &f2, auto const &f3 ) {
		  twitter_result = from_json<daw::twitter::twitter_object_t,
		                             checked_policy_t<ExecTag, true>>( f1 );
		  citm_result =
		    from_json<daw::citm::citm_object_t, checked_policy_t<ExecTag, true>>(
		      f2 );
		  canada_result =
		    from_json<daw::geojson::Polygon, checked_policy_t<ExecTag, false>>(
		      f3,
		      "features[0].geometry" );
	  },
	  json_sv1,
	  json_sv2,
	  json_sv3 );

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
	  "nativejson bench trusted",
	  sz,
	  [&]( auto const &f1, auto const &f2, auto const &f3 ) {
		  twitter_result = from_json<daw::twitter::twitter_object_t,
		                             unchecked_policy_t<ExecTag, true>>( f1 );
		  citm_result =
		    from_json<daw::citm::citm_object_t, unchecked_policy_t<ExecTag, true>>(
		      f2 );
		  canada_result =
		    from_json<daw::geojson::Polygon, unchecked_policy_t<ExecTag, false>>(
		      f3,
		      "features[0].geometry" );
	  },
	  json_sv1,
	  json_sv2,
	  json_sv3 );

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
#ifdef DAW_USE_EXCEPTIONS
  try
#endif
{
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
	test<constexpr_exec_tag>( argv );

	if constexpr( not std::is_same_v<simd_exec_tag, runtime_exec_tag> ) {
		test<runtime_exec_tag>( argv );
	}
	test<simd_exec_tag>( argv );
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