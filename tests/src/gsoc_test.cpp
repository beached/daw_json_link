// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/daw_json_link
//

#include "gsoc.h"

#include "daw_json_benchmark.h"

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
	using namespace daw::json;

	{
		auto gsoc_result = daw::json::benchmark::benchmark(
		  DAW_NUM_RUNS, sz, "gsoc bench(checked)",
		  []( daw::string_view jd ) {
			  auto res = from_json<daw::gsoc::gsoc_object_t>( jd );
			  daw::do_not_optimize( res );
			  return res;
		  },
		  json_data1 );
		(void)gsoc_result.get( );
	}

	{
		auto gsoc_result = daw::json::benchmark::benchmark(
		  DAW_NUM_RUNS, sz, "gsoc bench(unchecked)",
		  []( daw::string_view jd ) {
			  auto res = from_json<daw::gsoc::gsoc_object_t>(
			    jd, options::parse_flags<options::CheckedParseMode::no> );
			  daw::do_not_optimize( res );
			  return res;
		  },
		  json_data1 );
		(void)gsoc_result.get( );
	}

	std::cout
	  << "to_json testing\n*********************************************\n";
	std::string str{ };
	str.reserve( 4U * 1024U * 1024U );
	{
		auto const gsoc_result2 = from_json<daw::gsoc::gsoc_object_t>( json_data1 );
		str.reserve( json_data1.size( ) );

		auto to_json_ret = daw::json::benchmark::benchmark(
		  DAW_NUM_RUNS, sz, "gsoc bench(to_json)",
		  [&str]( daw::gsoc::gsoc_object_t const &obj ) {
			  str.clear( );
			  (void)to_json( obj, str );
			  daw::do_not_optimize( str );
		  },
		  gsoc_result2 );
		(void)to_json_ret.get( );
	}
	daw::do_not_optimize( str );
	auto const gsoc_result2 = from_json<daw::gsoc::gsoc_object_t>( str );
	daw::do_not_optimize( gsoc_result2 );
	{
		auto const str_sz = str.size( );
		str.clear( );
		str.resize( str_sz * 2 );
		char const *out_ptr = nullptr;
		auto to_json_ret = daw::json::benchmark::benchmark(
		  DAW_NUM_RUNS, sz, "gsoc bench(to_json2)",
		  [&]( daw::gsoc::gsoc_object_t const &obj ) {
			  auto *out_it = str.data( );
			  out_ptr = to_json( obj, out_it );
			  daw::do_not_optimize( str );
		  },
		  gsoc_result2 );
		(void)to_json_ret.get( );
		daw_json_ensure( out_ptr != nullptr, ErrorReason::NullOutputIterator );
		daw_json_ensure( out_ptr - str.data( ) > 0, ErrorReason::NumberOutOfRange );
		daw_json_ensure( static_cast<std::size_t>( out_ptr - str.data( ) ) <=
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