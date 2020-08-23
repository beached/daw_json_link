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

template<size_t N>
bool unsigned_test( ) {
	using namespace daw::json;
	std::vector<unsigned> const data = make_random_data<unsigned, N>( );

	std::string json_data = to_json_array( data );

	std::vector<unsigned> const parsed_1 = from_json_array<unsigned>( json_data );

#ifdef DAW_ALLOW_SSE42
	json_data += "        "; // so that SSE has enough room to safely parse
	std::vector<unsigned> const parsed_2 =
	  from_json_array<json_number<no_name, unsigned>, std::vector<unsigned>,
	                  SIMDNoCommentSkippingPolicyChecked<SIMDModes::SSE42>>(
	    json_data );
	return parsed_1 == data and parsed_1 == parsed_2;
#else
	return parsed_1 == data;
#endif
}

template<size_t N>
bool signed_test( ) {
	using namespace daw::json;
	std::vector<signed> const data = make_random_data<signed, N>( );

	std::string json_data = to_json_array( data );

	std::vector<signed> const parsed_1 = from_json_array<signed>( json_data );

#ifdef DAW_ALLOW_SSE42
	json_data += "        "; // so that SSE has enough room to safely parse
	std::vector<signed> const parsed_2 =
	  from_json_array<json_number<no_name, signed>, std::vector<signed>,
	                  SIMDNoCommentSkippingPolicyChecked<SIMDModes::SSE42>>(
	    json_data );
	return parsed_1 == data and parsed_1 == parsed_2;
#else
	return parsed_1 == data;
#endif
}

int main( int, char ** ) try {
	daw::expecting( unsigned_test<1000>( ) );
	daw::expecting( signed_test<1000>( ) );
} catch( daw::json::json_exception const &jex ) {
	std::cerr << "Exception thrown by parser: " << jex.reason( ) << std::endl;
	exit( 1 );
}
