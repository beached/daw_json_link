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

template<typename ExecTag, size_t N>
void test( ) {
	std::cout << "Using " << ExecTag::name
	          << " exec model\n*********************************************\n";
	{
		std::cout << "unsigned test\n";
		using namespace daw::json;
		std::vector<unsigned> const data = make_random_data<unsigned, N>( );

		std::string json_data = to_json_array( data );

		json_data += "                "; // Ensure SIMD has enough rooom to go full
		std::vector<unsigned> const parsed_1 =
		  from_json_array<json_number<no_name, unsigned>, std::vector<unsigned>,
		                  SIMDNoCommentSkippingPolicyChecked<ExecTag>>( json_data );
		test_assert( parsed_1 == data, "Failure to parse unsigned" );
	}

	{
		std::cout << "signed test\n";
		using namespace daw::json;
		std::vector<signed> const data = make_random_data<signed, N>( );

		std::string json_data = to_json_array( data );

		json_data += "        "; // so that SSE has enough room to safely parse
		std::vector<signed> const parsed_1 =
		  from_json_array<json_number<no_name, signed>, std::vector<signed>,
		                  SIMDNoCommentSkippingPolicyChecked<ExecTag>>( json_data );
		test_assert( parsed_1 == data, "Failure to parse signed" );
	}
}

int main( int, char ** )
#ifdef DAW_USE_JSON_EXCEPTIONS
  try
#endif
{
	test<daw::json::constexpr_exec_tag, 1000>( );
	test<daw::json::runtime_exec_tag, 1000>( );
	if constexpr( not std::is_same_v<daw::json::simd_exec_tag,
	                                 daw::json::runtime_exec_tag> ) {
		test<daw::json::simd_exec_tag, 1000>( );
	}
}
catch( daw::json::json_exception const &jex ) {
	std::cerr << "Exception thrown by parser: " << jex.reason( ) << std::endl;
	exit( 1 );
}
