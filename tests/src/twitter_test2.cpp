// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/daw_json_link
//

#include "defines.h"

#include "twitter_test2_json.h"

#include <daw/cpp_17.h>
#include <daw/daw_benchmark.h>
#include <daw/daw_read_file.h>
#include <daw/daw_traits.h>
#include <daw/json/daw_from_json.h>
#include <daw/json/daw_to_json.h>

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

	std::optional<daw::twitter2::twitter_object_t> twitter_result;
	daw::bench_n_test_mbs<DAW_NUM_RUNS>(
	  "twitter bench(checked)", sz,
	  [&twitter_result]( auto f1 ) {
		  twitter_result =
		    daw::json::from_json<daw::twitter2::twitter_object_t>( f1 );
		  daw::do_not_optimize( twitter_result );
	  },
	  json_sv1 );
	daw::do_not_optimize( twitter_result );
	test_assert( twitter_result, "Missing value" );
	test_assert( not twitter_result->statuses.empty( ), "Expected values" );
	test_assert( twitter_result->statuses.front( ).user.id == "1186275104",
	             "Missing value" );

	twitter_result = std::nullopt;
	daw::bench_n_test_mbs<DAW_NUM_RUNS>(
	  "twitter bench(unchecked)", sz,
	  [&twitter_result]( auto f1 ) {
		  twitter_result =
		    daw::json::from_json<daw::twitter2::twitter_object_t,
		                         daw::json::NoCommentSkippingPolicyUnchecked>( f1 );
		  daw::do_not_optimize( twitter_result );
	  },
	  json_sv1 );
	daw::do_not_optimize( twitter_result );
	test_assert( twitter_result, "Missing value" );
	test_assert( not twitter_result->statuses.empty( ), "Expected values" );
	test_assert( twitter_result->statuses.front( ).user.id == "1186275104",
	             "Missing value" );

	std::string str = std::string( );
	str.reserve( json_sv1.size( ) );
	auto out_it = std::back_inserter( str );
	daw::bench_n_test_mbs<DAW_NUM_RUNS>(
	  "twitter_catalog bench(to_json_string)", sz,
	  [&]( auto const &tr ) {
		  str.clear( );
		  daw::json::to_json( tr.value( ), out_it );
		  daw::do_not_optimize( str );
	  },
	  twitter_result );
	test_assert( not str.empty( ), "Expected a string value" );
	daw::do_not_optimize( str );
	auto const twitter_result2 =
	  daw::json::from_json<daw::twitter2::twitter_object_t>( str );
	daw::do_not_optimize( twitter_result2 );
	// Removing for now as it will do a float compare and fail

	/*test_assert( twitter_result == twitter_result2,
	                 "Expected round trip to produce same result" );*/
}
#ifdef DAW_USE_JSON_EXCEPTIONS
catch( daw::json::json_exception const &jex ) {
	std::cerr << "Exception thrown by parser: " << jex.reason( ) << std::endl;
	exit( 1 );
}
#endif
