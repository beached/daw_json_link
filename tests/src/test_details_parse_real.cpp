// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/daw_json_link
//

#include "defines.h"

#include "daw/json/daw_json_exception.h"
#include "daw/json/daw_json_iterator.h"
#include "daw/json/daw_json_link.h"
#include "daw/json/impl/daw_json_exec_modes.h"

#include <daw/daw_arith_traits.h>
#include <daw/daw_benchmark.h>
#include <daw/daw_bounded_vector.h>

#include <cassert>
#include <chrono>
#include <cstdint>
#include <iostream>
#include <optional>
#include <random>
#include <sstream>
#include <vector>

#if not defined( DAW_NUM_RUNS )
#if not defined( DEBUG ) or defined( NDEBUG )
static inline constexpr std::size_t DAW_NUM_RUNS = 200;
#else
static inline constexpr std::size_t DAW_NUM_RUNS = 2;
#endif
#else
#if not defined( DAW_NUM_RUNS ) or DAW_NUM_RUNS < 1
#error "DAW_NUM_RUNS must be an integer > 0"
#endif
#endif
#if not defined( DAW_NUM_RUNS2 )
#if not defined( DEBUG ) or defined( NDEBUG )
static inline constexpr std::size_t DAW_NUM_RUNS2 = 10;
#else
static inline constexpr std::size_t DAW_NUM_RUNS2 = 1;
#endif
#else
#if not defined( DAW_NUM_RUNS2 ) or DAW_NUM_RUNS2 < 1
#error "DAW_NUM_RUNS2 must be an integer > 0"2
#endif
#endif

template<std::size_t NUM_VALS = 100'000>
void test_lots_of_doubles( ) {
	auto rd = std::random_device( );
	auto rnd = std::mt19937_64( rd( ) );

	using num_t = daw::json::BasicParsePolicy<>;
	auto numbers_str = std::vector<std::string>( NUM_VALS );
	auto numbers = std::vector<num_t>( NUM_VALS );
	std::size_t bytes = 0;
	for( std::size_t i = 0; i < NUM_VALS; ++i ) {
		unsigned long long x1 = rnd( );
		unsigned long long x2 = rnd( );
		int x3 = std::uniform_int_distribution<>( -308, +308 )( rnd );
		char buffer[128];
		std::snprintf( buffer, 128, "%llu.%llue%d", x1, x2, x3 );
		numbers_str[i] = std::string( buffer );
		bytes += numbers_str[i].size( );
		auto rng = num_t( numbers_str[i].data( ),
		                  numbers_str[i].data( ) + numbers_str[i].size( ) );
		rng = daw::json::json_details::skip_number( rng );
		numbers[i] = rng;
	}
	(void)daw::bench_n_test_mbs<DAW_NUM_RUNS>(
	  "float parsing(unknown bounds)", bytes,
	  []( std::vector<num_t> const &nums ) {
		  for( std::size_t n = 0; n < NUM_VALS; ++n ) {
			  auto rng = nums[n];
			  using json_member = daw::json::json_details::json_deduced_type<float>;
			  auto const r = daw::json::json_details::parse_value<json_member, false>(
			    rng, daw::json::ParseTag<json_member::expected_type>{ } );
			  daw::do_not_optimize( r );
		  }
	  },
	  numbers );

	(void)daw::bench_n_test_mbs<DAW_NUM_RUNS>(
	  "double parsing(unknown bounds)", bytes,
	  []( std::vector<num_t> const &nums ) {
		  for( std::size_t n = 0; n < NUM_VALS; ++n ) {
			  auto rng = nums[n];
			  using json_member = daw::json::json_details::json_deduced_type<double>;
			  auto const r = daw::json::json_details::parse_value<json_member, false>(
			    rng, daw::json::ParseTag<json_member::expected_type>{ } );
			  daw::do_not_optimize( r );
		  }
	  },
	  numbers );

	(void)daw::bench_n_test_mbs<DAW_NUM_RUNS>(
	  "long double parsing(unknown bounds)", bytes,
	  []( std::vector<num_t> const &nums ) {
		  for( std::size_t n = 0; n < NUM_VALS; ++n ) {
			  auto rng = nums[n];
			  using json_member =
			    daw::json::json_details::json_deduced_type<long double>;
			  auto const r = daw::json::json_details::parse_value<json_member, false>(
			    rng, daw::json::ParseTag<json_member::expected_type>{ } );
			  daw::do_not_optimize( r );
		  }
	  },
	  numbers );

	(void)daw::bench_n_test_mbs<DAW_NUM_RUNS>(
	  "float parsing(known bounds)", bytes,
	  []( std::vector<num_t> const &nums ) {
		  for( std::size_t n = 0; n < NUM_VALS; ++n ) {
			  auto rng = nums[n];
			  using json_member = daw::json::json_details::json_deduced_type<float>;
			  auto const r = daw::json::json_details::parse_value<json_member, true>(
			    rng, daw::json::ParseTag<json_member::expected_type>{ } );
			  daw::do_not_optimize( r );
		  }
	  },
	  numbers );

	(void)daw::bench_n_test_mbs<DAW_NUM_RUNS>(
	  "double parsing(known bounds)", bytes,
	  []( std::vector<num_t> const &nums ) {
		  for( std::size_t n = 0; n < NUM_VALS; ++n ) {
			  auto rng = nums[n];
			  using json_member = daw::json::json_details::json_deduced_type<double>;
			  auto const r = daw::json::json_details::parse_value<json_member, true>(
			    rng, daw::json::ParseTag<json_member::expected_type>{ } );
			  daw::do_not_optimize( r );
		  }
	  },
	  numbers );

	(void)daw::bench_n_test_mbs<DAW_NUM_RUNS>(
	  "long double parsing(known bounds)", bytes,
	  []( std::vector<num_t> const &nums ) {
		  for( std::size_t n = 0; n < NUM_VALS; ++n ) {
			  auto rng = nums[n];
			  using json_member =
			    daw::json::json_details::json_deduced_type<long double>;
			  auto const r = daw::json::json_details::parse_value<json_member, true>(
			    rng, daw::json::ParseTag<json_member::expected_type>{ } );
			  daw::do_not_optimize( r );
		  }
	  },
	  numbers );

	// Too slow to need lots of tests
	(void)daw::bench_n_test_mbs<DAW_NUM_RUNS2>(
	  "double parsing(strtod)", bytes,
	  []( std::vector<std::string> const &nums ) {
		  for( std::size_t n = 0; n < NUM_VALS; ++n ) {
			  char **nend = nullptr;
			  auto const r = strtod( nums[n].data( ), nend );
			  daw::do_not_optimize( r );
		  }
	  },
	  numbers_str );
}

int main( int, char ** )
#ifdef DAW_USE_EXCEPTIONS
  try
#endif
{
	test_lots_of_doubles( );
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
