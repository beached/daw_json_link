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
#include <daw/daw_memory_mapped_file.h>

#include <iostream>
#include <string_view>

#if not defined( DAW_NUM_RUNS )
#if not defined( DEBUG ) or defined( NDEBUG )
static inline constexpr std::size_t DAW_NUM_RUNS = 250;
#else
static inline constexpr std::size_t DAW_NUM_RUNS = 1;
#endif
#endif
static_assert( DAW_NUM_RUNS > 0 );

int main( int argc, char **argv ) try {
	using namespace daw::json;
	if( argc < 2 ) {
		std::cerr << "Must supply a filenames to open\n";
		std::cerr << "numbers.json\n";
		exit( 1 );
	}

	auto const mm_numbers = daw::filesystem::memory_mapped_file_t<>( argv[1] );
	auto const sv_numbers =
	  std::string_view( mm_numbers.data( ), mm_numbers.size( ) );

	std::cout << "non-debug run\n";
	using range_t = daw::json::json_array_range<
	  double,
	  daw::json::SIMDNoCommentSkippingPolicyChecked<daw::json::simd_exec_tag>>;

	std::vector<double> results = [&] {
		auto rng = range_t( sv_numbers );
		return std::vector<double>( rng.begin( ), rng.end( ) );
	}( );
	daw::bench_n_test_mbs<DAW_NUM_RUNS>(
	  "numbers bench (checked)", sv_numbers.size( ),
	  [&]( auto rng ) {
		  double *ptr = results.data( );
		  for( double d : rng ) {
			  *ptr++ += d;
		  }
	  },
	  range_t( sv_numbers ) );
	daw::do_not_optimize( results );
	using range2_t = daw::json::json_array_range<
	  double,
	  daw::json::SIMDNoCommentSkippingPolicyUnchecked<daw::json::simd_exec_tag>>;
	daw::bench_n_test_mbs<DAW_NUM_RUNS>(
	  "numbers bench (unchecked)", sv_numbers.size( ),
	  [&]( auto rng ) {
		  double *ptr = results.data( );
		  for( double d : rng ) {
			  *ptr++ += d;
		  }
	  },
	  range2_t( sv_numbers ) );

	daw::do_not_optimize( results );
} catch( daw::json::json_exception const &jex ) {
	std::cerr << "Exception thrown by parser: " << jex.reason( ) << std::endl;
	exit( 1 );
}
