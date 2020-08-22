// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/daw_json_link
//

#include "defines.h"

#include "daw/json/daw_json_iterator.h"
#include "daw/json/daw_json_link.h"
#include "tweet/twitter_timeline.h"

#include <daw/cpp_17.h>
#include <daw/daw_benchmark.h>
#include <daw/daw_memory_mapped_file.h>
#include <daw/daw_traits.h>

#include <fstream>
#include <iostream>
#include <streambuf>

#if not defined( DAW_NUM_RUNS ) and                                            \
  ( not defined( DEBUG ) or defined( NDEBUG ) )
static inline constexpr std::size_t DAW_NUM_RUNS = 2500;
#else
static inline constexpr std::size_t DAW_NUM_RUNS = 1;
#endif
static_assert( DAW_NUM_RUNS > 0 );

int main( int argc, char **argv ) try {

	using namespace daw::json;
	if( argc < 2 ) {
		std::cerr << "Must supply a filenames to open(twitter_timeline.json)\n";
		exit( 1 );
	}

	auto const json_data1 = daw::filesystem::memory_mapped_file_t<>( argv[1] );
	auto const json_sv1 =
	  std::string_view( json_data1.data( ), json_data1.size( ) );
	assert( json_sv1.size( ) > 2 and "Minimum json data size is 2 '{}'" );

	auto const sz = json_sv1.size( );
	std::cout << "Processing: " << daw::utility::to_bytes_per_second( sz )
	          << '\n';

	auto twitter_result = std::vector<daw::twitter::tweet>( );
	{
		using range_t = daw::json::json_array_range<daw::twitter::tweet>;
		auto rng = range_t( json_sv1 );
		twitter_result.insert( twitter_result.end( ), rng.begin( ), rng.end( ) );
	}
	{
		using range_t = daw::json::json_array_range<daw::twitter::tweet>;
		auto res = daw::bench_n_test_mbs<DAW_NUM_RUNS>(
		  "twitter timeline bench(checked)", sz,
		  [&]( auto rng ) {
			  std::copy( rng.begin( ), rng.end( ), twitter_result.data( ) );
		  },
		  range_t( json_sv1 ) );
		if( not res.has_value( ) ) {
			daw_json_error( "Exception while parsing: res.get_exception_message()" );
		}
	}
#if defined( DAW_ALLOW_SSE3 )
	{
		using range_t =
		  daw::json::json_array_range<daw::twitter::tweet,
		                              daw::json::SIMDNoCommentSkippingPolicyChecked<
		                                daw::json::SIMDModes::SSE3>>;
		auto res = daw::bench_n_test_mbs<DAW_NUM_RUNS>(
		  "twitter timeline bench(checked, sse3)", sz,
		  [&]( auto rng ) {
			  std::copy( rng.begin( ), rng.end( ), twitter_result.data( ) );
		  },
		  range_t( json_sv1 ) );
		if( not res.has_value( ) ) {
			daw_json_error( "Exception while parsing: res.get_exception_message()" );
		}
	}
	{
		using range_t =
		  daw::json::json_array_range<daw::twitter::tweet,
		                              daw::json::SIMDNoCommentSkippingPolicyChecked<
		                                daw::json::SIMDModes::SSE3>>;
		auto res = daw::bench_n_test_mbs<DAW_NUM_RUNS>(
		  "twitter timeline bench(checked, sse3, nostore)", sz,
		  [&]( auto rng ) {
			  for( auto v : rng ) {
				  daw::do_not_optimize( v );
			  }
		  },
		  range_t( json_sv1 ) );
	}

#endif
	daw::do_not_optimize( twitter_result );
	daw_json_assert( not twitter_result.empty( ), "Unexpected empty array" );

	{
		using range_t =
		  daw::json::json_array_range<daw::twitter::tweet,
		                              daw::json::NoCommentSkippingPolicyUnchecked>;
		auto res = daw::bench_n_test_mbs<DAW_NUM_RUNS>(
		  "twitter timeline bench(unchecked)", sz,
		  [&]( auto rng ) {
			  std::copy( rng.begin( ), rng.end( ), twitter_result.data( ) );
		  },
		  range_t( json_sv1 ) );
		if( not res.has_value( ) ) {
			daw_json_error( "Exception while parsing: res.get_exception_message()" );
		}
	}
	daw::do_not_optimize( twitter_result );
	daw_json_assert( not twitter_result.empty( ), "Unexpected empty array" );
	{
		using range_t = daw::json::json_array_range<
		  daw::twitter::tweet, daw::json::SIMDNoCommentSkippingPolicyUnchecked<
		                         daw::json::SIMDModes::SSE3>>;
		auto res = daw::bench_n_test_mbs<DAW_NUM_RUNS>(
		  "twitter timeline bench(unchecked, sse3)", sz,
		  [&]( auto rng ) {
			  std::copy( rng.begin( ), rng.end( ), twitter_result.data( ) );
		  },
		  range_t( json_sv1 ) );
		if( not res.has_value( ) ) {
			daw_json_error( "Exception while parsing: res.get_exception_message()" );
		}
		auto res2 = daw::bench_n_test_mbs<DAW_NUM_RUNS>(
		  "twitter timeline bench(unchecked, sse3, nostore)", sz,
		  [&]( auto rng ) {
			  for( auto v : rng ) {
				  daw::do_not_optimize( v );
			  }
		  },
		  range_t( json_sv1 ) );
		if( not res.has_value( ) ) {
			daw_json_error( "Exception while parsing: res.get_exception_message()" );
		}
	}
} catch( daw::json::json_exception const &jex ) {
	std::cerr << "Exception thrown by parser: " << jex.reason( ) << std::endl;
	exit( 1 );
}
