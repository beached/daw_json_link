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
#include <daw/daw_read_file.h>
#include <daw/daw_traits.h>

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

template<typename ExecTag>
void test( std::string_view json_sv1 ) {
	std::cout << "Using " << ExecTag::name
	          << " exec model\n*********************************************\n";

	auto const sz = json_sv1.size( );
	auto twitter_result = std::vector<daw::twitter::tweet>( );
	{
		using range_t = daw::json::json_array_range<
		  daw::twitter::tweet,
		  daw::json::SIMDNoCommentSkippingPolicyChecked<ExecTag>>;
		auto rng = range_t( json_sv1 );
		twitter_result.insert( twitter_result.end( ), rng.begin( ), rng.end( ) );
	}
	{
		using range_t = daw::json::json_array_range<
		  daw::twitter::tweet,
		  daw::json::SIMDNoCommentSkippingPolicyChecked<ExecTag>>;
		auto res = daw::bench_n_test_mbs<DAW_NUM_RUNS>(
		  "twitter timeline bench(checked)", sz,
		  [&]( auto rng ) {
			  std::copy( rng.begin( ), rng.end( ), twitter_result.data( ) );
		  },
		  range_t( json_sv1 ) );
		test_assert( res.has_value( ),
		             "Exception while parsing: res.get_exception_message()" );
	}
	{
		using range_t = daw::json::json_array_range<
		  daw::twitter::tweet,
		  daw::json::SIMDNoCommentSkippingPolicyChecked<ExecTag>>;
		auto res = daw::bench_n_test_mbs<DAW_NUM_RUNS>(
		  "twitter timeline bench(checked, nostore)", sz,
		  [&]( auto rng ) {
			  for( auto v : rng ) {
				  daw::do_not_optimize( v );
			  }
		  },
		  range_t( json_sv1 ) );
	}
	{
		using range_t = daw::json::json_array_range<
		  daw::twitter::tweet,
		  daw::json::SIMDNoCommentSkippingPolicyUnchecked<ExecTag>>;
		auto res = daw::bench_n_test_mbs<DAW_NUM_RUNS>(
		  "twitter timeline bench(unchecked)", sz,
		  [&]( auto rng ) {
			  std::copy( rng.begin( ), rng.end( ), twitter_result.data( ) );
		  },
		  range_t( json_sv1 ) );
		test_assert( res.has_value( ),
		             "Exception while parsing: res.get_exception_message()" );
	}
	{
		using range_t = daw::json::json_array_range<
		  daw::twitter::tweet,
		  daw::json::SIMDNoCommentSkippingPolicyUnchecked<ExecTag>>;
		auto res = daw::bench_n_test_mbs<DAW_NUM_RUNS>(
		  "twitter timeline bench(unchecked, nostore)", sz,
		  [&]( auto rng ) {
			  for( auto v : rng ) {
				  daw::do_not_optimize( v );
			  }
		  },
		  range_t( json_sv1 ) );
	}
	daw::do_not_optimize( twitter_result );
	test_assert( not twitter_result.empty( ), "Unexpected empty array" );
}

int main( int argc, char **argv )
#ifdef DAW_USE_JSON_EXCEPTIONS
  try
#endif
{

	using namespace daw::json;
	if( argc < 2 ) {
		std::cerr << "Must supply a filenames to open(twitter_timeline.json)\n";
		exit( 1 );
	}

	auto const json_data1 = *daw::read_file( argv[1] );
	auto const json_sv1 =
	  std::string_view( json_data1.data( ), json_data1.size( ) );
	assert( json_sv1.size( ) > 2 and "Minimum json data size is 2 '{}'" );

	auto const sz = json_sv1.size( );
	std::cout << "Processing: " << daw::utility::to_bytes_per_second( sz )
	          << '\n';

	test<daw::json::constexpr_exec_tag>( json_sv1 );
	test<daw::json::runtime_exec_tag>( json_sv1 );
	if constexpr( not std::is_same_v<daw::json::simd_exec_tag,
	                                 daw::json::runtime_exec_tag> ) {
		test<daw::json::simd_exec_tag>( json_sv1 );
	}
}
catch( daw::json::json_exception const &jex ) {
	std::cerr << "Exception thrown by parser: " << jex.reason( ) << std::endl;
	exit( 1 );
}
