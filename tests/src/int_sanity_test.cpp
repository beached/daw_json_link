// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/daw_json_link
//

#include "defines.h"

#include "daw/json/daw_json_link.h"

#include <daw/daw_benchmark.h>
#include <daw/daw_random.h>

#include <algorithm>
#include <cstdint>
#include <iostream>
#include <vector>

template<typename T, size_t N>
std::vector<T> make_random_data( ) {
	std::vector<T> result{ };
	result.reserve( N );
	for( size_t n = 0; n < N; ++n ) {
		result.push_back( daw::randint<T>( daw::numeric_limits<T>::min( ),
		                                   daw::numeric_limits<T>::max( ) ) );
	}
	return result;
}
using namespace daw::json::options;
template<ExecModeTypes ExecMode, size_t N>
void test( ) {
	std::cout << "Using " << to_string( ExecMode )
	          << " exec model\n********************************************\n";
	{
		std::cout << "unsigned test\n";
		using namespace daw::json;
		std::vector<unsigned> const data = make_random_data<unsigned, N>( );

		std::string json_data = to_json_array( data );

		json_data += "                "; // Ensure SIMD has enough rooom to go full
		std::vector<unsigned> const parsed_1 =
		  from_json_array<json_number_no_name<unsigned>, std::vector<unsigned>>(
		    json_data, parse_flags<ExecMode> );
		test_assert( parsed_1 == data, "Failure to parse unsigned" );
	}

	{
		std::cout << "signed test\n";
		using namespace daw::json;
		std::vector<signed> const data = make_random_data<signed, N>( );

		std::string json_data = to_json_array( data );

		json_data += "        "; // so that SSE has enough room to safely parse
		std::vector<signed> const parsed_1 =
		  from_json_array<json_number_no_name<signed>, std::vector<signed>>(
		    json_data, parse_flags<ExecMode> );
		test_assert( parsed_1 == data, "Failure to parse signed" );
	}
}

int main( int, char ** )
#ifdef DAW_USE_EXCEPTIONS
  try
#endif
{
	test<ExecModeTypes::compile_time, 1000>( );
	test<ExecModeTypes::runtime, 1000>( );
	if constexpr( not std::is_same_v<daw::json::simd_exec_tag,
	                                 daw::json::runtime_exec_tag> ) {
		test<ExecModeTypes::simd, 1000>( );
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