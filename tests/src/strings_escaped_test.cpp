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

#include <cstddef>
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <string>
#include <vector>

#if not defined( DAW_NUM_RUNS )
#if not defined( DEBUG ) or defined( NDEBUG )
static inline constexpr std::size_t DAW_NUM_RUNS = 250;
#else
static inline constexpr std::size_t DAW_NUM_RUNS = 2;
#endif
#endif
static_assert( DAW_NUM_RUNS > 0 );

template<typename Container>
constexpr void clear( Container &c ) {
	for( auto &v : c ) {
		v = { };
	}
}

template<typename ExecTag>
std::size_t test( std::string_view json_data ) {
	std::cout << "Using " << ExecTag::name
	          << " exec model\n*********************************************\n";
	using namespace daw::json;
	std::vector<std::string> values =
	  from_json_array<std::string, std::vector<std::string>,
	                  daw::json::SIMDNoCommentSkippingPolicyChecked<ExecTag>>(
	    json_data );
	daw::do_not_optimize( values );
	auto const v2 = values;
	clear( values );
	daw::do_not_optimize( values );
	daw::bench_n_test_mbs<DAW_NUM_RUNS>(
	  "strings.json checked", json_data.size( ),
	  []( auto sv, auto ptr ) {
		  auto range = json_array_range<
		    std::string, daw::json::SIMDNoCommentSkippingPolicyChecked<ExecTag>>(
		    sv );
		  for( auto v : range ) {
			  daw::do_not_optimize( v );
			  *ptr++ = v;
		  }
	  },
	  json_data, values.data( ) );
	daw::do_not_optimize( values );
	test_assert( v2 == values, "Expected them to parse the same" );
	auto const h0 = std::accumulate(
	  values.begin( ), values.end( ), 0ULL, []( auto old, auto current ) {
		  return old +=
		         std::hash<std::string>{ }( static_cast<std::string>( current ) );
	  } );
	daw::do_not_optimize( json_data );
	std::vector<std::string> values2 =
	  from_json_array<std::string, std::vector<std::string>,
	                  daw::json::SIMDNoCommentSkippingPolicyUnchecked<ExecTag>>(
	    json_data );

	daw::bench_n_test_mbs<DAW_NUM_RUNS>(
	  "strings.json unchecked", json_data.size( ),
	  []( auto sv, auto ptr ) mutable {
		  auto range = json_array_range<
		    std::string, daw::json::SIMDNoCommentSkippingPolicyUnchecked<ExecTag>>(
		    sv );
		  for( auto v : range ) {
			  daw::do_not_optimize( v );
			  *ptr++ = v;
		  }
	  },
	  json_data, values2.data( ) );
	daw::do_not_optimize( json_data );
	daw::do_not_optimize( values2 );
	auto const h1 = std::accumulate(
	  values2.begin( ), values2.end( ), 0ULL, []( auto old, auto current ) {
		  return old +=
		         std::hash<std::string>{ }( static_cast<std::string>( current ) );
	  } );
	test_assert( values == values2, "Parses don't match" );
	test_assert( h0 == h1, "Hashes don't match" );
	return h1;
}

int main( int argc, char **argv )
#ifdef DAW_USE_JSON_EXCEPTIONS
  try
#endif
{
	if( argc <= 1 ) {
		puts( "Must supply path to strings.json file\n" );
		exit( EXIT_FAILURE );
	}
	std::string const json_string = [argv] {
		auto const data = *daw::read_file( argv[1] );
		return std::string( data.data( ), data.size( ) );
	}( );
	auto const h0 = test<daw::json::constexpr_exec_tag>( json_string );
	auto const h1 = test<daw::json::runtime_exec_tag>( json_string );
	test_assert( h0 == h1, "constexpr/runtime exec model hashes do not match" );
	if constexpr( not std::is_same_v<daw::json::simd_exec_tag,
	                                 daw::json::runtime_exec_tag> ) {
		auto const h2 = test<daw::json::simd_exec_tag>( json_string );
		test_assert( h0 == h2, "constexpr/fast exec model hashes do not match" );
	}
}
catch( daw::json::json_exception const &jex ) {
	std::cerr << "Exception thrown by parser: " << jex.reason( ) << std::endl;
	exit( 1 );
}
