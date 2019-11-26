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

struct Number {
	float a{};
};
#ifdef __cpp_nontype_template_parameter_class
[[maybe_unused]] static constexpr auto describe_json_class( Number ) noexcept {
	using namespace daw::json;
	return class_description_t<json_number<"a", float>>{};
}
#else
namespace symbols_Number {
	static inline constexpr char const a[] = "a";
}

[[maybe_unused]] static constexpr auto describe_json_class( Number ) noexcept {
	using namespace daw::json;
	return class_description_t<json_number<symbols_Number::a, float>>{};
}
#endif

#ifndef NDEBUG
static constexpr size_t const NUMVALUES = 10'000ULL;
#else
// static constexpr size_t const NUMVALUES = 1'000'000ULL;
static constexpr size_t const NUMVALUES = 10'000ULL;
#endif

template<typename Float>
Float rand_float( ) {
	auto f0 = daw::randint<intmax_t>( std::numeric_limits<intmax_t>::min( ),
	                                  std::numeric_limits<intmax_t>::max( ) );
	if( f0 == 0 ) {
		f0 = 1;
	}
	auto const f1 =
	  daw::randint<intmax_t>( std::numeric_limits<intmax_t>::min( ),
	                          std::numeric_limits<intmax_t>::max( ) );

	return static_cast<Float>( f1 ) / static_cast<Float>( f0 );
}

int main( ) {
	using namespace daw::json;
	using iterator_t = daw::json::json_array_iterator<float>;

	std::string json_data3 = [] {
		std::string result = "[";
		result.reserve( NUMVALUES * 23 + 8 );
		daw::algorithm::do_n( NUMVALUES, [&result] {
			result += std::to_string( rand_float<float>( ) ) + ',';
		} );
		result.back( ) = ']';
		return result;
	}( );

	daw::string_view json_sv{json_data3.data( ), json_data3.size( )};
	auto data2 = std::unique_ptr<float[]>( new float[NUMVALUES] );
	{
		auto const count3 = *daw::bench_n_test_mbs<100>(
		  "float", json_sv.size( ),
		  [&]( auto &&sv ) noexcept {
			  auto ptr = std::copy( iterator_t( sv ), iterator_t( ), data2.get( ) );
			  daw::do_not_optimize( data2 );
			  return ptr - data2.get( );
		  },
		  json_sv );

		std::cout << "float parse count: " << count3 << '\n';
	}
}
