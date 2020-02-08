// The MIT License (MIT)
//
// Copyright (c) 2020 Darrell Wright
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files( the "Software" ), to
// deal in the Software without restriction, including without limitation the
// rights to use, copy, modify, merge, publish, distribute, sublicense, and / or
// sell copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#include "daw/json/daw_json_link.h"

#include <daw/daw_benchmark.h>
#include <daw/daw_random.h>

#include <algorithm>
#include <cstdint>
#include <vector>

template<typename T, size_t N>
std::vector<T> make_random_data( ) {
	std::vector<T> result{};
	result.reserve( N );
	for( size_t n = 0; n < N; ++n ) {
		result.push_back( daw::randint<T>( std::numeric_limits<T>::min( ),
		                                   std::numeric_limits<T>::max( ) ) );
	}
	return result;
}

template<size_t N>
bool unsigned_test( ) {
	using namespace daw::json;
	std::vector<unsigned> const data = make_random_data<unsigned, N>( );

	std::string json_data = to_json_array( data );

	std::vector<unsigned> const parsed_1 = from_json_array<unsigned>( json_data );

#ifdef DAW_ALLOW_SSE3
	json_data += "        "; // so that SSE has enough room to safely parse
	std::vector<unsigned> const parsed_2 =
	  from_json_array<json_number_sse3<no_name, unsigned>>( json_data );
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

#ifdef DAW_ALLOW_SSE3
	json_data += "        "; // so that SSE has enough room to safely parse
	std::vector<signed> const parsed_2 =
	  from_json_array<json_number_sse3<no_name, signed>>( json_data );
	return parsed_1 == data and parsed_1 == parsed_2;
#else
	return parsed_1 == data;
#endif
}

int main( ) {
	daw::expecting( unsigned_test<1000>( ) );
	daw::expecting( signed_test<1000>( ) );
}
