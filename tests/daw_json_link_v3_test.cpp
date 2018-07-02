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
	std::chrono::system_clock::time_point tp =
	  std::chrono::system_clock::time_point( );

	static constexpr auto get_json_link( ) noexcept {
		using json_link_t =
		  daw::json::link<test_001_t>::json_number<int>::json_number<double>::
		    json_bool<bool>::json_string<std::chrono::system_clock::time_point>;

		return json_link_t( "i", "d", "b", "tp" );
	}

	constexpr test_001_t( ) noexcept = delete;
	constexpr test_001_t(
	  int Int, double Double, bool Bool,
	  std::chrono::system_clock::time_point TimePoint ) noexcept
	  : i( Int )
	  , d( Double )
	  , b( Bool )
	  , tp( TimePoint ) {}
};

struct test_002_t {
	int i = 0;
	double d = 0.0;
	bool b = false;
	std::chrono::system_clock::time_point tp =
	  std::chrono::system_clock::time_point( );

	constexpr test_002_t( ) noexcept = delete;
	constexpr test_002_t(
	  int Int, double Double, bool Bool,
	  std::chrono::system_clock::time_point TimePoint ) noexcept
	  : i( Int )
	  , d( Double )
	  , b( Bool )
	  , tp( TimePoint ) {}
};

constexpr auto get_json_link( daw::tag<test_002_t> ) noexcept {
	using json_link_t =
	  daw::json::link<test_002_t>::json_number<int>::json_number<double>::
	    json_bool<bool>::json_string<std::chrono::system_clock::time_point>;

	return json_link_t( "i", "d", "b", "tp" );
}

int main( ) {
	constexpr auto const json_data =
	  R"({
			"i": 1,
			"d": 2.2, 
			"tp": "2018-06-22T15:05:37Z"
		})";

	constexpr auto data = daw::json::parse_class<test_001_t>( json_data );
	// assert( data.i == 1 );
	// assert( data.d == 2.2 );

	constexpr auto data2 = daw::json::parse_class<test_002_t>( json_data );
	// assert( data.i == 1 );
	// assert( data.d == 2.2 );
	return 0;
}
