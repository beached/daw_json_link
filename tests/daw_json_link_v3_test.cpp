// The MIT License (MIT)
//
// Copyright (c) 2018 Darrell Wright
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

#include <cassert>
#include <chrono>

#include "daw/json/daw_json_link_v3.h"

struct test_001_t {
	int i = 0;
	double d = 0.0;
	bool b = false;

	constexpr test_001_t( int Int, double Double, bool Bool ) noexcept
	  : i( Int )
	  , d( Double )
	  , b( Bool ) {}
};

constexpr auto get_json_link( test_001_t ) noexcept {
	using namespace daw::json;
	return json_link<json_number<"i", int>, json_number<"d">, json_bool<"b">>{};
}

struct test_002_t {
	int i = 0;
	double d = 0.0;
	bool b = false;
	std::chrono::system_clock::time_point tp{};

	constexpr test_002_t( ) noexcept = default; // delete;
	constexpr test_002_t(
	  int Int, double Double, bool Bool,
	  std::chrono::system_clock::time_point TimePoint ) noexcept
	  : i( Int )
	  , d( Double )
	  , b( Bool )
	  , tp( TimePoint ) {}
};

auto get_json_link( test_002_t ) {
	using namespace daw::json;
	return json_link<json_number<"i", int>, json_number<"d", double>,
	                 json_bool<"b">, json_date<"tp">>{};
}

constexpr auto const json_data =
  R"({
	    "i": 55,
	    "d": 2.2,
			"b": true,
	    "tp": "2018-06-22T15:05:37Z"
	  })";

auto func( daw::string_view sv ) {
	using link_t = decltype( get_json_link( std::declval<test_001_t>( ) ) );

	auto data = daw::json::from_json_t<test_001_t>( sv );
	std::clog << "result: i->" << data.i << '\n';
	std::clog << "result: d->" << data.d << '\n';
	std::clog << "result: b->" << data.b << '\n';
	return data.i;
}

int main( ) {
	auto result = func( json_data );
	return result;

	// assert( data.i == 1 );
	// assert( data.d == 2.2 );

	// constexpr auto data2 = daw::json::from_json_t<test_002_t>( json_data );
	// assert( data.i == 1 );
	// assert( data.d == 2.2 );
}
