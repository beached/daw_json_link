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
#include <daw/daw_do_n.h>
#include <daw/daw_random.h>
#include <daw/iterator/daw_back_inserter.h>

#include "daw/json/daw_json_iterator.h"
#include "daw/json/daw_json_link.h"

#ifndef NDEBUG
static constexpr size_t const NUMVALUES = 1'000ULL;
#else
static constexpr size_t const NUMVALUES = 1'000'000ULL;
#endif

int main( ) {
	using namespace daw::json;
	using int_type = uintmax_t;
	using iterator_t =
	  daw::json::json_array_iterator<json_number<no_name, int_type>>;

	std::string json_data3 = [] {
		std::string result = "[";
		result.reserve( NUMVALUES * 23 + 8 );
		daw::algorithm::do_n( NUMVALUES, [&result] {
			result += std::to_string( daw::randint<int_type>(
			            std::numeric_limits<int_type>::min( ),
			            std::numeric_limits<int_type>::max( ) ) ) +
			          ',';
		} );
		result.back( ) = ']';
		return result;
	}( );

	daw::string_view json_sv{json_data3.data( ), json_data3.size( )};
	auto data2 = std::unique_ptr<int_type[]>( new int_type[NUMVALUES] );

	daw::do_not_optimize( data2 );
	auto const count3 = *daw::bench_n_test_mbs<100>(
	  "uint parsing", json_sv.size( ),
	  [&]( auto &&sv ) noexcept {
		  auto ptr = std::copy( iterator_t( sv ), iterator_t( ), data2.get( ) );
		  daw::do_not_optimize( data2 );
		  return ptr - data2.get( );
	  },
	  json_sv );

	std::cout << "unsigned parse count: " << count3 << '\n';
}
