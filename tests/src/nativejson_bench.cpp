// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/daw_json_link
//

#include "defines.h"

#include "citm_test_json.h"
#include "daw_json_benchmark.h"
#include "geojson_json.h"
#include "twitter_test_json.h"

#include <daw/daw_read_file.h>
#include <daw/json/daw_json_link.h>

#include <cstdlib>
#include <iostream>
#include <string>

#if not defined( DAW_NUM_RUNS )
#if not defined( DEBUG ) or defined( NDEBUG )
static inline constexpr std::size_t DAW_NUM_RUNS = 1000;
#else
static inline constexpr std::size_t DAW_NUM_RUNS = 2;
#endif
#endif
static_assert( DAW_NUM_RUNS > 0 );

using namespace daw::json;

// GCC-8 bug workaround
template<options::ExecModeTypes ExecMode, bool expect_long_strings>
inline constexpr auto unchecked_policy_v =
  options::parse_flags<options::CheckedParseMode::no, ExecMode,
                       ( expect_long_strings ? options::ExpectLongNames::yes
                                             : options::ExpectLongNames::no )>;
template<options::ExecModeTypes ExecMode, bool expect_long_strings>
inline constexpr auto checked_policy_v =
  options::parse_flags<ExecMode,
                       ( expect_long_strings ? options::ExpectLongNames::yes
                                             : options::ExpectLongNames::no )>;

template<options::ExecModeTypes ExecMode>
void test( char **argv, bool do_asserts ) {
	auto const json_opt1 = daw::read_file( argv[1] );
	if( not json_opt1 or json_opt1->empty( ) ) {
		std::cerr << "Could not open " << argv[1] << " or it is empty\n";
		std::terminate( );
	}
	auto const json_sv1 = *json_opt1;
	auto const json_opt2 = daw::read_file( argv[2] );
	if( not json_opt2 or json_opt2->empty( ) ) {
		std::cerr << "Could not open " << argv[2] << " or it is empty\n";
		std::terminate( );
	}
	auto const json_sv2 = *json_opt2;
	auto const json_opt3 = daw::read_file( argv[3] );
	if( not json_opt3 or json_opt3->empty( ) ) {
		std::cerr << "Could not open " << argv[3] << " or it is empty\n";
		std::terminate( );
	}
	auto const json_sv3 = *json_opt3;

	std::cout << "Using " << to_string( ExecMode )
	          << " exec model\n*********************************************\n";
	auto const sz =
	  std::size( json_sv1 ) + std::size( json_sv2 ) + std::size( json_sv3 );

	std::cout << std::flush;

	std::optional<daw::twitter::twitter_object_t> twitter_result{ };
	std::optional<daw::citm::citm_object_t> citm_result{ };
	std::optional<daw::geojson::Polygon> canada_result{ };

#ifdef DAW_USE_EXCEPTIONS
	try
#endif
	{
		auto ret = daw::json::benchmark::benchmark(
		  DAW_NUM_RUNS, json_sv1.size( ), "twitter bench(checked)",
		  []( auto const &f1 ) {
			  return from_json<daw::twitter::twitter_object_t>(
			    f1, checked_policy_v<ExecMode, true> );
		  },
		  json_sv1 );
		twitter_result = ret.get( );
	}
#ifdef DAW_USE_EXCEPTIONS
	catch( json_exception const &jex ) {
		std::cerr << "Error while testing twitter.json\n";
		std::cerr << to_formatted_string( jex ) << '\n';
	}
#endif
	if( do_asserts ) {
		test_assert( twitter_result, "Missing value -> twitter_result" );
		test_assert( not twitter_result->statuses.empty( ),
		             "Expected values: twitter_result is empty" );
		test_assert(
		  twitter_result->statuses.front( ).user.id == 1186275104,
		  std::string( "Expected values: user_id had wrong value, "
		               "expected 1186275104.  Got " ) +
		    std::to_string( twitter_result->statuses.front( ).user.id ) );
	}
	twitter_result.reset( );

	std::cout << std::flush;

	{
		auto ret = daw::json::benchmark::benchmark(
		  DAW_NUM_RUNS, json_sv1.size( ), "twitter bench(unchecked)",
		  []( auto f1 ) {
			  return from_json<daw::twitter::twitter_object_t>(
			    f1, checked_policy_v<ExecMode, true> );
		  },
		  json_sv1 );
		twitter_result = ret.get( );
	}
	if( do_asserts ) {
		test_assert( not twitter_result->statuses.empty( ),
		             "Expected values: twitter_result is empty" );
		test_assert( twitter_result->statuses.front( ).user.id == 1186275104,
		             "Expected values" );
	}
	twitter_result.reset( );

	std::cout << std::flush;

	{
		auto ret = daw::json::benchmark::benchmark(
		  DAW_NUM_RUNS, json_sv2.size( ), "citm bench(checked)",
		  []( auto const &f2 ) {
			  return from_json<daw::citm::citm_object_t>(
			    f2, checked_policy_v<ExecMode, true> );
		  },
		  json_sv2 );
		citm_result = ret.get( );
	}
	if( do_asserts ) {
		test_assert( citm_result, "Missing value" );
		test_assert( not citm_result->areaNames.empty( ), "Expected values" );
		test_assert( citm_result->areaNames.count( 205706005 ) == 1,
		             "Expected value" );
		test_assert( citm_result->areaNames[205706005] == "1er balcon jardin",
		             "Incorrect value" );
	}
	citm_result.reset( );

	std::cout << std::flush;

	{
		auto ret = daw::json::benchmark::benchmark(
		  DAW_NUM_RUNS, json_sv2.size( ), "citm bench(unchecked)",
		  []( auto const &f2 ) {
			  return from_json<daw::citm::citm_object_t>(
			    f2, checked_policy_v<ExecMode, true> );
		  },
		  json_sv2 );
		citm_result = ret.get( );
	}
	if( do_asserts ) {
		test_assert( not citm_result->areaNames.empty( ), "Expected values" );
		test_assert( citm_result->areaNames.count( 205706005 ) == 1,
		             "Expected value" );
		test_assert( citm_result->areaNames[205706005] == "1er balcon jardin",
		             "Incorrect value" );
	}
	citm_result.reset( );

	std::cout << std::flush;

	{
		auto ret = daw::json::benchmark::benchmark(
		  DAW_NUM_RUNS, json_sv3.size( ), "canada bench(checked)",
		  []( auto const &f3 ) {
			  return from_json<daw::geojson::Polygon>(
			    f3, "features[0].geometry", checked_policy_v<ExecMode, false> );
		  },
		  json_sv3 );
		canada_result = ret.get( );
	}
	if( do_asserts ) {
		test_assert( canada_result, "Missing value" );
	}
	canada_result.reset( );

	std::cout << std::flush;

	{
		auto ret = daw::json::benchmark::benchmark(
		  DAW_NUM_RUNS, json_sv3.size( ), "canada bench(unchecked)",
		  []( auto const &f3 ) {
			  return from_json<daw::geojson::Polygon>(
			    f3, "features[0].geometry", checked_policy_v<ExecMode, false> );
		  },
		  json_sv3 );
		canada_result = ret.get( );
	}
	if( do_asserts ) {
		test_assert( canada_result, "Missing value" );
	}
	canada_result.reset( );

	std::cout << std::flush;

	{
		auto ret = daw::json::benchmark::benchmark(
		  DAW_NUM_RUNS, sz, "nativejson bench(checked)",
		  []( auto const &f1, auto const &f2, auto const &f3 ) {
			  return std::tuple{
			    from_json<daw::twitter::twitter_object_t>(
			      f1, checked_policy_v<ExecMode, true> ),
			    from_json<daw::citm::citm_object_t>(
			      f2, checked_policy_v<ExecMode, true> ),
			    from_json<daw::geojson::Polygon>(
			      f3, "features[0].geometry", checked_policy_v<ExecMode, false> ) };
		  },
		  json_sv1, json_sv2, json_sv3 );
		std::tie( twitter_result, citm_result, canada_result ) = ret.get( );
	}

	std::cout << std::flush;

	daw::do_not_optimize( twitter_result );
	daw::do_not_optimize( citm_result );
	daw::do_not_optimize( canada_result );
	if( do_asserts ) {
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
	}
	twitter_result.reset( );
	citm_result.reset( );
	canada_result.reset( );

	{
		auto ret = daw::json::benchmark::benchmark(
		  DAW_NUM_RUNS, sz, "nativejson bench(unchecked)",
		  []( auto const &f1, auto const &f2, auto const &f3 ) {
			  return std::tuple{
			    from_json<daw::twitter::twitter_object_t>(
			      f1, unchecked_policy_v<ExecMode, true> ),
			    from_json<daw::citm::citm_object_t>(
			      f2, unchecked_policy_v<ExecMode, true> ),
			    from_json<daw::geojson::Polygon>(
			      f3, "features[0].geometry", unchecked_policy_v<ExecMode, false> ) };
		  },
		  json_sv1, json_sv2, json_sv3 );
		std::tie( twitter_result, citm_result, canada_result ) = ret.get( );
	}

	std::cout << std::flush;

	daw::do_not_optimize( twitter_result );
	daw::do_not_optimize( citm_result );
	daw::do_not_optimize( canada_result );

	if( do_asserts ) {
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
	bool const do_asserts = [&] {
		if( argc > 4 ) {
			std::string_view arg2 = argv[4];
			return arg2 != "noassert";
		}
		return true;
	}( );
	test<options::ExecModeTypes::compile_time>( argv, do_asserts );

	if constexpr( not std::is_same_v<simd_exec_tag, runtime_exec_tag> ) {
		test<options::ExecModeTypes::runtime>( argv, do_asserts );
	}
	test<options::ExecModeTypes::simd>( argv, do_asserts );
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
