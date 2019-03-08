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
#include <sstream>
#include <string>
#include <string_view>
#include <vector>

#include <daw/daw_benchmark.h>
#include <daw/daw_bounded_vector.h>

#include "daw/json/daw_json_link.h"

static_assert( daw::json::impl::parse_unsigned_integer<uintmax_t>( "12345" ) ==
               12345 );
static_assert(
  daw::json::impl::parse_unsigned_integer2<uintmax_t>( "12345" ).count == 5 );
static_assert( daw::json::impl::parse_integer<uintmax_t>( "12345" ) == 12345 );
static_assert( daw::json::impl::parse_integer<intmax_t>( "-12345" ) == -12345 );

static_assert( daw::json::impl::parse_real<double>( "5" ) == 5.0 );
static_assert( daw::json::impl::parse_real<double>( "5.5" ) == 5.5 );
static_assert( daw::json::impl::parse_real<double>( "5.5e2" ) == 550.0 );
static_assert( daw::json::impl::parse_real<double>( "5.5e+2" ) == 550.0 );
static_assert( daw::json::impl::parse_real<double>( "5e2" ) == 500.0 );

struct test_001_t {
	int i = 0;
	double d = 0.0;
	bool b = false;
	std::string_view s{};
	daw::bounded_vector_t<int, 10> y{};
	std::optional<int> o{};
	std::optional<int> o2{};
	std::chrono::time_point<std::chrono::system_clock, std::chrono::milliseconds>
	  dte{};

	constexpr test_001_t( ) = default;

	constexpr test_001_t( int Int, double Double, bool Bool, std::string_view S,
	                      daw::bounded_vector_t<int, 10> Y, std::optional<int> O,
	                      std::optional<int> O2,
	                      std::chrono::time_point<std::chrono::system_clock,
	                                              std::chrono::milliseconds>
	                        D ) noexcept
	  : i( Int )
	  , d( Double )
	  , b( Bool )
	  , s( S )
	  , y( Y )
	  , o( O )
	  , o2( O2 )
	  , dte( D ) {}
};

auto describe_json_class( test_001_t ) noexcept {
	using namespace daw::json;
#if __cplusplus > 201703L or ( defined( __GNUC__ ) and __GNUC__ >= 9 )
	return class_description_t<
	  json_number<"i", int>, json_number<"d">, json_bool<"b">,
	  json_string<"s", std::string_view>,
	  json_array<"y", daw::bounded_vector_t<int, 10>, json_number<no_name, int>>,
	  json_nullable<json_number<"o", int>>, json_nullable<json_number<"o2", int>>,
	  json_date<"dte">>{};
#else
	constexpr static char const i[] = "i";
	constexpr static char const d[] = "d";
	constexpr static char const b[] = "b";
	constexpr static char const s[] = "s";
	constexpr static char const y[] = "y";
	constexpr static char const o[] = "o";
	constexpr static char const o2[] = "o2";
	constexpr static char const dte[] = "dte";
	return class_description_t<
	  json_number<i, int>, json_number<d>, json_bool<b>,
	  json_string<s, std::string_view>,
	  json_array<y, daw::bounded_vector_t<int, 10>, json_number<no_name, int>>,
	  json_nullable<json_number<o, int>>, json_nullable<json_number<o2, int>>,
	  json_date<dte>>{};
#endif
}

auto to_json_data( test_001_t const &v ) {
	return std::forward_as_tuple( v.i, v.d, v.b, v.s, v.y, v.o, v.o2, v.dte );
}

struct test_002_t {
	test_001_t a{};
};
auto describe_json_class( test_002_t ) noexcept {
	using namespace daw::json;
#if __cplusplus > 201703L or ( defined( __GNUC__ ) and __GNUC__ >= 9 )
	return class_description_t<json_class<"a", test_001_t>>{};
#else
	constexpr static char const a[] = "a";
	return class_description_t<json_class<a, test_001_t>>{};
#endif
}

auto to_json_data( test_002_t const &v ) {
	return std::forward_as_tuple( v.a );
}

struct test_003_t {
	std::optional<test_001_t> a;
};
auto describe_json_class( test_003_t ) noexcept {
	using namespace daw::json;
#if __cplusplus > 201703L or ( defined( __GNUC__ ) and __GNUC__ >= 9 )
	return class_description_t<json_nullable<json_class<"a", test_001_t>>>{};
#else
	constexpr static char const a[] = "a";
	return class_description_t<json_nullable<json_class<a, test_001_t>>>{};
#endif
}

auto to_json_data( test_003_t const &v ) {
	return std::forward_as_tuple( v.a );
}

enum class blah_t { a, b, c };

std::string to_string( blah_t e ) noexcept {
	switch( e ) {
	case blah_t::a:
		return "a";
	case blah_t::b:
		return "b";
	case blah_t::c:
		return "c";
	}
	std::terminate( );
}

constexpr blah_t from_string( daw::tag_t<blah_t>,
                              std::string_view sv ) noexcept {
	if( sv.empty( ) ) {
		std::terminate( );
	}
	switch( sv.front( ) ) {
	case 'a':
		return blah_t::a;
	case 'b':
		return blah_t::b;
	case 'c':
		return blah_t::c;
	default:
		std::terminate( );
	}
}

struct e_test_001_t {
	blah_t a = blah_t::a;
};

auto describe_json_class( e_test_001_t ) noexcept {
	using namespace daw::json;
#if __cplusplus > 201703L or ( defined( __GNUC__ ) and __GNUC__ >= 9 )
	return class_description_t<json_custom<"a", blah_t>>{};
#else
	constexpr static char const a[] = "a";
	return class_description_t<json_custom<a, blah_t>>{};
#endif
}

auto to_json_data( e_test_001_t const &v ) {
	return std::forward_as_tuple( v.a );
}

constexpr auto const json_data =
  R"({
	    "d": -1.234e+3,
	    "i": 55,
			"b": true,
			"x": { "b": false, "c": [1,2,3] },
			"y": [1,2,3,4],
			"z": { "a": 1 },
	    "tp": "2018-06-22T15:05:37Z",
			"s": "yo yo yo",
			"o": 1344,
			"dte": "2016-12-31T01:02:03.343Z"
	  })";

constexpr char const json_data_array[] =
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
			"o": 1344,
			"dte": "2016-12-31T01:02:03.343Z"
	  },
	  {
	    "i": 55,
	    "d": -2.2,
			"b": true,
			"x": { "b": false, "c": [1,2,3] },
			"y": [1,2,3,4],
			"z": { "a": 1 },
	    "tp": "2018-06-22T15:05:37Z",
			"s": "yo yo yo",
			"o": 1344,
			"o2": 80085,
			"dte": "2017-12-31T01:02:03.343Z"
	  },{
	    "i": 55,
	    "d": -2.2e4,
			"b": true,
			"x": { "b": false, "c": [1,2,3] },
			"y": [1,2,3,4],
			"z": { "a": 1 },
	    "tp": "2018-06-22T15:05:37Z",
			"s": "yo yo yo",
			"o": 1344,
			"dte": "2018-12-31T01:02:03.343Z"
	  },{
	    "i": 55,
	    "d": 2.2e-5,
			"b": true,
			"x": { "b": false, "c": [1,2,3] },
			"y": [1,2,3,4],
			"z": { "a": 1 },
	    "tp": "2018-06-22T15:05:37Z",
			"s": "yo yo yo",
			"o": 1344,
			"dte": "2019-11-31T01:02:03.343Z"
	  },{
	    "i": 55,
	    "d": 2.2,
			"b": true,
			"x": { "b": false, "c": [1,2,3] },
			"y": [1,2,3,4],
			"z": { "a": 1 },
	    "tp": "2018-06-22T15:05:37Z",
			"s": "yo yo yo",
			"o": 1322,
			"dte": "2010-06-31T01:02:03.343Z"
	  }])";

int main( ) {
	using namespace daw::json;

	constexpr auto data = daw::json::from_json<test_001_t>( json_data );
	std::clog << to_json( data ) << '\n';

	constexpr auto ary =
	  from_json_array<json_class<no_name, test_001_t>,
	                  daw::bounded_vector_t<test_001_t, 10>>( json_data_array );
	std::cout << "read in " << ary.size( ) << " items\n";
	for( auto const &v : ary ) {
		std::clog << to_json( v ) << "\n\n";
	}
	std::cout << "as array\n";
	std::cout << to_json_array( ary ) << "\n\n";

	test_002_t t2{data};
	t2.a.o2 = std::nullopt;
	std::cout << to_json( t2 ) << '\n';

	test_003_t t3{data};
	std::cout << to_json( t3 ) << '\n';

	e_test_001_t t4{};
	auto e_test_001_str = to_json( t4 );
	std::cout << e_test_001_str << '\n';
	auto e_test_001_back = from_json<e_test_001_t>( e_test_001_str );
	daw::do_not_optimize( e_test_001_back );

	constexpr std::string_view const json_data2 =
	  R"({
	"a": {
		"b": {
			"c": [1,2,3] }}})";

	using iterator2_t = daw::json::json_array_iterator<json_number<no_name, int>>;

	auto first = iterator2_t( json_data2, "a/b/c" );
	auto sum = 0;
	while( first ) {
		sum += *first;
		++first;
	}
	std::cout << "sum: " << sum << '\n';
}
