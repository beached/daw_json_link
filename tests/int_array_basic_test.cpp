// The MIT License (MIT)
//
// Copyright (c) 2019 Darrell Wright
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

#include <fstream>
#include <iostream>
#include <streambuf>
#include <string_view>
#include <vector>

#include <daw/daw_benchmark.h>
#include <daw/daw_random.h>

#include "daw/json/daw_json_iterator.h"
#include "daw/json/daw_json_link.h"

#ifndef NDEBUG
static constexpr size_t const NUMVALUES = 1'000ULL;
#else
static constexpr size_t const NUMVALUES = 100'000ULL;
#endif

template<size_t N, typename T>
static std::string make_int_array_data( ) {
	return [] {
		std::string result = "[";
		result.reserve( N * 23 + 8 );
		for( size_t n = 0; n < N; ++n ) {
			result +=
			  std::to_string( daw::randint<T>( std::numeric_limits<T>::min( ),
			                                   std::numeric_limits<T>::max( ) ) ) +
			  ',';
		}
		result.back( ) = ']';
		result.shrink_to_fit( );
		return result;
	}( );
}

template<typename T>
static void test_from_json_array( std::string_view json_sv ) {
	using namespace daw::json;

	for( size_t n = 0; n < 1000; ++n ) {
		daw::do_not_optimize( json_sv );
		auto result = daw::json::from_json_array_trusted<T, daw::bounded_vector_t<T, NUMVALUES>>( json_sv );
		daw::do_not_optimize( result );
	}
}

template<typename T>
static void test_json_array_iterator( std::string_view json_sv ) {
	using namespace daw::json;
	using iterator_t = json_array_iterator_trusted<json_number<no_name, T>>;
	for( size_t n = 0; n < 1000; ++n ) {
		daw::do_not_optimize( json_sv );
		auto first = iterator_t( json_sv );
		auto const last = iterator_t( );
		T sum = 0;
		daw::do_not_optimize( sum );
		while( first != last ) {
			sum += *first++;
		}
		daw::do_not_optimize( sum );
	}
}

int main( ) {
	using int_type = uintmax_t;
	auto const json_str = make_int_array_data<NUMVALUES, int_type>( );
	auto const json_sv = std::string_view( json_str.data( ), json_str.size( ) );
	test_from_json_array<int_type>( json_sv );
	test_json_array_iterator<int_type>( json_sv );
}
