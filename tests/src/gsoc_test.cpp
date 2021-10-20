// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/daw_json_link
//

#include "gsoc.h"

#include <daw/json/daw_json_link.h>

#include <daw/daw_benchmark.h>
#include <daw/daw_read_file.h>

#include <cassert>
#include <iostream>

#if not defined( DAW_NUM_RUNS )
#if not defined( DEBUG ) or defined( NDEBUG )
static inline constexpr std::size_t DAW_NUM_RUNS = 250;
#else
static inline constexpr std::size_t DAW_NUM_RUNS = 2;
#endif
#endif
static_assert( DAW_NUM_RUNS > 0 );

int main( int argc, char **argv )
#ifdef DAW_USE_EXCEPTIONS
  try
#endif
{
	using namespace daw::json;
	if( argc < 2 ) {
		std::cerr << "Must supply a filenames to open\n";
		exit( 1 );
	}

	std::string const json_data1 = *daw::read_file( argv[1] );
	assert( json_data1.size( ) > 2 and "Minimum json data size is 2 '{}'" );

	auto const sz = json_data1.size( );
	std::cout << "Processing: " << daw::utility::to_bytes_per_second( sz )
	          << '\n';
	using namespace daw::json;

	std::optional<daw::gsoc::gsoc_object_t> gsoc_result;
	using nc_checked_policy_t = NoCommentSkippingPolicyChecked;
	daw::bench_n_test_mbs<DAW_NUM_RUNS>(
	  "gsoc bench(checked)",
	  sz,
	  [&]( std::string const &jd ) {
		  gsoc_result =
		    from_json<daw::gsoc::gsoc_object_t, nc_checked_policy_t>( jd );
		  daw::do_not_optimize( gsoc_result );
	  },
	  json_data1 );

	daw::do_not_optimize( json_data1 );

	daw::bench_n_test_mbs<DAW_NUM_RUNS>(
	  "gsoc bench(unchecked)",
	  sz,
	  [&gsoc_result]( std::string const &jd ) {
		  gsoc_result =
		    from_json<daw::gsoc::gsoc_object_t, NoCommentSkippingPolicyUnchecked>(
		      jd );
		  daw::do_not_optimize( gsoc_result );
	  },
	  json_data1 );

	std::cout
	  << "to_json testing\n*********************************************\n";
	std::string str{ };
	str.reserve( 4U * 1024U * 1024U );
	{
		auto const gsoc_result2 = from_json<daw::gsoc::gsoc_object_t>( json_data1 );
		auto out_it = std::back_inserter( str );
		str.reserve( json_data1.size( ) );
		daw::bench_n_test_mbs<DAW_NUM_RUNS>(
		  "gsoc bench(to_json_string)",
		  sz,
		  [&]( auto const &value ) {
			  str.clear( );
			  to_json( value, out_it );
			  daw::do_not_optimize( str );
		  },
		  gsoc_result2 );
	}
	daw::do_not_optimize( str );
	auto const gsoc_result2 = from_json<daw::gsoc::gsoc_object_t>( str );
	daw::do_not_optimize( gsoc_result2 );
	{
		auto const str_sz = str.size( );
		str.clear( );
		str.resize( str_sz * 2 );
		char const *out_ptr = nullptr;
		daw::bench_n_test_mbs<DAW_NUM_RUNS>(
		  "gsoc bench(to_json_string2)",
		  sz,
		  [&]( auto const &tr ) {
			  auto *out_it = str.data( );
			  out_ptr = to_json( tr, out_it );
			  daw::do_not_optimize( str );
		  },
		  *gsoc_result );
		daw_json_assert( out_ptr != nullptr, ErrorReason::NullOutputIterator );
		daw_json_assert( out_ptr - str.data( ) > 0, ErrorReason::NumberOutOfRange );
		daw_json_assert( static_cast<std::size_t>( out_ptr - str.data( ) ) <=
		                   str.size( ),
		                 ErrorReason::NumberOutOfRange );
	}
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