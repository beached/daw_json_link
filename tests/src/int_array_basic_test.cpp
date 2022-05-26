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
#include <daw/daw_bounded_vector.h>
#include <daw/daw_random.h>

#include <fstream>
#include <iostream>
#include <streambuf>
#include <string_view>
#include <vector>

template<size_t N, typename T>
static std::string make_int_array_data( ) {
	return [] {
		std::string result = "[";
		result.reserve( N * 23 + 8 );
		for( size_t n = 0; n < N; ++n ) {
			result +=
			  std::to_string( daw::randint<T>( daw::numeric_limits<T>::min( ),
			                                   daw::numeric_limits<T>::max( ) ) ) +
			  ',';
		}
		result.back( ) = ']';
		result.shrink_to_fit( );
		return result;
	}( );
}

template<size_t NUMVALUES, typename T>
static void test_from_json_array( std::string_view json_sv ) {
	using namespace daw::json;

	for( size_t n = 0; n < 1000; ++n ) {
		daw::do_not_optimize( json_sv );
		auto result =
		  daw::json::from_json_array<T, daw::bounded_vector_t<T, NUMVALUES>>(
		    json_sv, options::parse_flags<options::CheckedParseMode::no> );
		daw::do_not_optimize( result );
	}
}

template<typename T>
static void test_json_array_iterator( std::string_view json_sv ) {
	using namespace daw::json;
	for( size_t n = 0; n < 1000; ++n ) {
		// daw::do_not_optimize( json_sv );
		auto rng = json_array_range<T, options::CheckedParseMode::no>( json_sv );
		// daw::do_not_optimize( rng );
		T sum = std::accumulate( rng.begin( ), rng.end( ), static_cast<T>( 0 ) );
		daw::do_not_optimize( sum );
	}
}

template<size_t NUMVALUES>
void test_func( ) {
#ifdef DAW_USE_EXCEPTIONS
	try {
#endif
		using int_type = uintmax_t;
		auto const json_str = make_int_array_data<NUMVALUES, int_type>( );
		auto const json_sv = std::string_view( json_str.data( ), json_str.size( ) );
		test_from_json_array<NUMVALUES, int_type>( json_sv );
		test_json_array_iterator<int_type>( json_sv );
#ifdef DAW_USE_EXCEPTIONS
	} catch( daw::json::json_exception const &je ) {
		std::cout << "Exception while processing: " << je.reason( ) << '\n';
		exit( 1 );
	}
#endif
}

int main( int argc, char ** )
#ifdef DAW_USE_EXCEPTIONS
  try
#endif
{
	if( argc > 1 ) {
		test_func<1'000'000ULL>( );
	} else {
		test_func<1'000ULL>( );
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