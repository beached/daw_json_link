// The MIT License (MIT)
//
// Copyright (c) 2018-2019 Darrell Wright
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
#include <string_view>
#include <vector>

#include <daw/daw_benchmark.h>
#include <daw/daw_do_n.h>
#include <daw/daw_random.h>

#include "daw/json/daw_json_link.h"

struct Number {
	intmax_t a{};
};
#if __cplusplus > 201703L
auto describe_json_class( Number ) noexcept {
	using namespace daw::json;
	return class_description_t<json_number<"a", intmax_t>>{};
}
#else
auto describe_json_class( Number ) noexcept {
	using namespace daw::json;
	static constexpr char const a[] = "a";
	return class_description_t<json_number<a, intmax_t>>{};
}
#endif

int main( ) {
	using namespace daw::json;
	constexpr size_t const NUMVALUES = 1'000'000ULL;
	std::string const json_data = [] {
		std::string result = "[";

		// 23 is what I calculated as the string size of the serialized class.
		// It may be incorrect but that is ok, it is close and should reduce
		// allocations
		result.reserve( NUMVALUES * ( 23 ) + 8 );
		daw::algorithm::do_n( NUMVALUES, [&result] {
			result += "{\"a\":" +
			          std::to_string( daw::randint<intmax_t>(
			            std::numeric_limits<intmax_t>::min( ),
			            std::numeric_limits<intmax_t>::max( ) ) ) +
			          "},";
		} );
		result.back( ) = ']';
		return result;
	}( );
	auto json_sv = std::string_view( json_data );
	std::cout << "Processing " << json_sv.size( ) << " bytes "
	          << daw::utility::to_bytes_per_second( json_sv.size( ) ) << '\n';
	auto const count =
	  *daw::bench_n_test<10>( "int parsing 1", []( auto &&sv ) noexcept {
		  auto const data = from_json_array<json_class<no_name, Number>>( sv );
		  return data.size( );
	  },
	                         json_sv );

	std::cout << "element count: " << count << '\n';

	using iterator_t =
	  daw::json::json_array_iterator<json_class<no_name, Number>>;

	auto data = std::vector<Number>( );

	auto const count2 = *daw::bench_n_test<10>(
	  "int parsing 2",
	  [&]( auto &&sv ) noexcept {
		  data.clear( );
		  std::copy( iterator_t( sv ), iterator_t( ), std::back_inserter( data ) );
		  return data.size( );
	  },
	  json_sv );

	std::cout << "element count 2: " << count2 << '\n';

	using iterator2_t = daw::json::json_array_iterator<json_number<no_name, int>>;
	std::string json_data2 = "[1,2,3,4]";
	auto values = std::vector<int>( iterator2_t( json_data2 ), iterator2_t( ) );
	for( auto i : values ) {
		std::cout << i << ",\n";
	}
}
