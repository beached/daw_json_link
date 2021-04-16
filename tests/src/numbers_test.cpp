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

#include <daw/daw_benchmark.h>
#include <daw/daw_read_file.h>

#include <iostream>
#include <string_view>

#if not defined( DAW_NUM_RUNS )
#if not defined( DEBUG ) or defined( NDEBUG )
static inline constexpr std::size_t DAW_NUM_RUNS = 250;
#else
static inline constexpr std::size_t DAW_NUM_RUNS = 2;
#endif
#endif
static_assert( DAW_NUM_RUNS > 0 );

template<typename ExecTag>
void test( std::string_view data ) {
	std::cout << "Using " << ExecTag::name
	          << " exec model\n*********************************************\n";
	using range_t = daw::json::json_array_range<
	  double, daw::json::SIMDNoCommentSkippingPolicyChecked<ExecTag>>;

	std::vector<double> results = [&] {
		auto rng = range_t( data );
		return std::vector<double>( rng.begin( ), rng.end( ) );
	}( );
	daw::bench_n_test_mbs<DAW_NUM_RUNS>(
	  "numbers bench (checked)", data.size( ),
	  [&]( auto rng ) {
		  double *ptr = results.data( );
		  for( double d : rng ) {
			  *ptr++ += d;
		  }
	  },
	  range_t( data ) );
	daw::do_not_optimize( results );
	using range2_t = daw::json::json_array_range<
	  double, daw::json::SIMDNoCommentSkippingPolicyUnchecked<ExecTag>>;
	daw::bench_n_test_mbs<DAW_NUM_RUNS>(
	  "numbers bench (unchecked)", data.size( ),
	  [&]( auto rng ) {
		  double *ptr = results.data( );
		  for( double d : rng ) {
			  *ptr++ += d;
		  }
	  },
	  range2_t( data ) );

	daw::do_not_optimize( results );
}

int main( int argc, char **argv )
#ifdef DAW_USE_JSON_EXCEPTIONS
  try
#endif
{
	using namespace daw::json;
	if( argc < 2 ) {
		std::cerr << "Must supply a filenames to open\n";
		std::cerr << "numbers.json\n";
		exit( 1 );
	}

	auto const mm_numbers = *daw::read_file( argv[1] );
	auto const sv_numbers =
	  std::string_view( mm_numbers.data( ), mm_numbers.size( ) );

#ifndef NDEBUG
	std::cout << "non-debug run\n";
#endif
	test<daw::json::constexpr_exec_tag>( sv_numbers );
	test<daw::json::runtime_exec_tag>( sv_numbers );
#if defined( DAW_ALLOW_SSE42 )
	test<daw::json::sse42_exec_tag>( sv_numbers );
#endif
}
#ifdef DAW_USE_JSON_EXCEPTIONS
catch( daw::json::json_exception const &jex ) {
	std::cerr << "Exception thrown by parser: " << jex.reason( ) << std::endl;
	exit( 1 );
}
#endif
