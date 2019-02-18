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

#include <cassert>
#include <chrono>
#include <iostream>
#include <optional>
#include <vector>

#include <daw/daw_bounded_vector.h>

#include "daw/json/daw_json_link.h"

/*
static_assert( daw::json::impl::parse_real( "5" ) == 5.0 );
static_assert( daw::json::impl::parse_real( "5.5" ) == 5.5 );
static_assert( daw::json::impl::parse_real( "5.5e2" ) == 550.0 );
*/

struct test_001_t {
	int i = 0;
	double d = 0.0;
	bool b = false;
	daw::string_view s{};
	daw::bounded_vector_t<int, 10> y{};
	std::optional<int> o{};
	std::optional<int> o2{};

	constexpr test_001_t( ) noexcept = default;

	constexpr test_001_t( int Int, double Double, bool Bool, daw::string_view S,
	                      daw::bounded_vector_t<int, 10> Y, std::optional<int> O,
	                      std::optional<int> O2 ) noexcept
	  : i( Int )
	  , d( Double )
	  , b( Bool )
	  , s( S )
	  , y( Y )
	  , o( O )
	  , o2( O2 ) {}
};

constexpr auto describe_json_class( test_001_t ) noexcept {
	using namespace daw::json;
	return json_parser_t<
	  json_number<"i", int>, json_number<"d">, json_bool<"b">,
	  json_string<"s", daw::string_view>,
	  json_array<"y", daw::bounded_vector_t<int, 10>, json_number<"", int>>,
	  json_number<"o", std::optional<int>, true>,
	  json_number<"o2", std::optional<int>, true>>{};
}

constexpr auto const json_data =
  R"({
	    "i": 55,
	    "d": 2.2,
			"b": true,
			"x": { "b": false, "c": [1,2,3] },
			"y": [1,2,3,4],
			"z": { "a": 1 },
	    "tp": "2018-06-22T15:05:37Z",
			"s": "yo yo yo",
			"o": 1344
	  })";

constexpr auto const json_data_array =
  R"([
			{
	    "i": 55,
	    "d": 2.2,
			"b": true,
			"x": { "b": false, "c": [1,2,3] },
			"y": [1,2,3,4],
			"z": { "a": 1 },
	    "tp": "2018-06-22T15:05:37Z",
			"s": "yo yo yo",
			"o": 1344
	  },
	  {
	    "i": 55,
	    "d": 2.2,
			"b": true,
			"x": { "b": false, "c": [1,2,3] },
			"y": [1,2,3,4],
			"z": { "a": 1 },
	    "tp": "2018-06-22T15:05:37Z",
			"s": "yo yo yo",
			"o": 1344
	  },{
	    "i": 55,
	    "d": 2.2,
			"b": true,
			"x": { "b": false, "c": [1,2,3] },
			"y": [1,2,3,4],
			"z": { "a": 1 },
	    "tp": "2018-06-22T15:05:37Z",
			"s": "yo yo yo",
			"o": 1344
	  },{
	    "i": 55,
	    "d": 2.2,
			"b": true,
			"x": { "b": false, "c": [1,2,3] },
			"y": [1,2,3,4],
			"z": { "a": 1 },
	    "tp": "2018-06-22T15:05:37Z",
			"s": "yo yo yo",
			"o": 1344
	  },{
	    "i": 55,
	    "d": 2.2,
			"b": true,
			"x": { "b": false, "c": [1,2,3] },
			"y": [1,2,3,4],
			"z": { "a": 1 },
	    "tp": "2018-06-22T15:05:37Z",
			"s": "yo yo yo",
			"o": 1344
	  }])";
int main( ) {
	constexpr auto data = daw::json::from_json<test_001_t>( json_data );
	std::clog << "result: i->" << data.i << '\n';
	std::clog << "result: d->" << data.d << '\n';
	std::clog << "result: b->" << data.b << '\n';
	std::clog << "result: s->" << data.s << '\n';
	std::clog << "result: y->[ ";
	for( auto const &v : data.y ) {
		std::clog << v << ", ";
	}
	std::clog << "]\n";
	std::clog << "result o->";
	if( !data.o ) {
		std::clog << "empty\n";
	} else {
		std::clog << *data.o << '\n';
	}
	std::clog << "result o2->";
	if( !data.o2 ) {
		std::clog << "empty\n";
	} else {
		std::clog << *data.o2 << '\n';
	}

	constexpr auto ary =
	  daw::json::from_json_array<daw::json::json_class<"", test_001_t>,
	                             daw::bounded_vector_t<test_001_t, 10>>(
	    json_data_array );
	std::cout << "read in " << ary.size( ) << " items\n";
}
