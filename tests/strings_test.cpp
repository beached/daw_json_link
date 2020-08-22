// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/daw_json_link
//

#include "daw/json/daw_json_iterator.h"
#include "daw/json/daw_json_link.h"

#include <daw/daw_benchmark.h>
#include <daw/daw_memory_mapped_file.h>

#include <cstddef>
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <string>
#include <vector>

template<daw::json::SIMDModes simd_mode>
std::size_t test( std::string_view json_data ) {
	using namespace daw::json;
	std::vector<std::string_view> values =
	  from_json_array<std::string_view, std::vector<std::string_view>,
	                  daw::json::SIMDNoCommentSkippingPolicyChecked<simd_mode>>(
	    json_data );
	daw::do_not_optimize( values );
	daw::bench_n_test_mbs<1000>(
	  "strings.json checked", json_data.size( ),
	  []( auto sv, auto ptr ) {
		  auto range = json_array_range<
		    std::string_view,
		    daw::json::SIMDNoCommentSkippingPolicyChecked<simd_mode>>( sv );
		  for( auto v : range ) {
			  daw::do_not_optimize( v );
			  *ptr++ = v;
		  }
	  },
	  json_data, values.data( ) );
	daw::do_not_optimize( values );
	auto const h0 = std::accumulate(
	  values.begin( ), values.end( ), 0ULL, []( auto old, auto current ) {
		  return old +=
		         std::hash<std::string>{ }( static_cast<std::string>( current ) );
	  } );
	daw::do_not_optimize( json_data );
	std::vector<std::string_view> values2 =
	  from_json_array<std::string_view, std::vector<std::string_view>,
	                  daw::json::SIMDNoCommentSkippingPolicyUnchecked<simd_mode>>(
	    json_data );

	daw::bench_n_test_mbs<1000>(
	  "strings.json unchecked", json_data.size( ),
	  []( auto sv, auto ptr ) mutable {
		  auto range = json_array_range<
		    std::string_view,
		    daw::json::SIMDNoCommentSkippingPolicyUnchecked<simd_mode>>( sv );
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
	daw_json_assert( values == values2, "Parses don't match" );
	daw_json_assert( h0 == h1, "Hashes don't match" );
	return h1;
}

int main( int argc, char **argv ) try {
	if( argc <= 1 ) {
		puts( "Must supply path to strings.json file\n" );
		exit( EXIT_FAILURE );
	}
	std::string const json_string = [argv] {
		auto const data = daw::filesystem::memory_mapped_file_t<>( argv[1] );
		return std::string( data.data( ), data.size( ) );
	}( );
	auto const h0 = test<daw::json::SIMDModes::None>( json_string );
#if defined( DAW_ALLOW_SSE3 )
	std::cout << "SSE3\n**************************\n";
	auto const h1 = test<daw::json::SIMDModes::SSE3>( json_string );
	daw_json_assert( h0 == h1, "SSE/non-SSE hashes do not match" );
#else
	(void)h0;
#endif
} catch( daw::json::json_exception const &jex ) {
	std::cerr << "Exception thrown by parser: " << jex.reason( ) << std::endl;
	exit( 1 );
}
