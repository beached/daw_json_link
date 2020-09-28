// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/daw_json_link
//

#include "defines.h"

#include "daw/json/daw_json_iterator.h"
#include "daw/json/daw_json_link.h"
#include "daw/json/daw_json_assert.h"
#include "daw/json/impl/daw_json_exec_modes.h"

#include <daw/daw_arith_traits.h>
#include <daw/daw_benchmark.h>
#include <daw/daw_bounded_vector.h>

#include <cassert>
#include <chrono>
#include <cstdint>
#include <iostream>
#include <optional>
#include <sstream>
#include <vector>

static_assert( daw::is_arithmetic_v<int> );

struct NumberX {
	int x;
};

namespace daw::json {
	template<>
	struct json_data_contract<NumberX> {
#ifdef __cpp_nontype_template_parameter_class
		using type = json_member_list<json_number<"x", int>>;
#else
		static constexpr char const x[] = "x";
		using type = json_member_list<json_number<x, int>>;
#endif
	};
} // namespace daw::json

template<typename Real, bool Trusted = false, std::size_t N>
DAW_CONSTEXPR Real parse_real( char const ( &str )[N] ) {
	auto rng = daw::json::NoCommentSkippingPolicyChecked( str, str + N );
	return daw::json::json_details::parse_real<Real, false>( rng );
}

template<typename Real, bool Trusted = false, size_t N>
DAW_CONSTEXPR bool parse_real_test( char const ( &str )[N], Real expected ) {
	auto res = parse_real<Real, Trusted>( str );
	return not( res < expected or res > expected );
}

template<typename Unsigned, bool Trusted = false, size_t N>
DAW_CONSTEXPR bool parse_unsigned_test( char const ( &str )[N],
                                        Unsigned expected ) {
	auto tmp = daw::json::NoCommentSkippingPolicyUnchecked( str, str + N );
	return daw::json::json_details::unsigned_parser<
	         Unsigned, daw::json::JsonRangeCheck::CheckForNarrowing, false>(
	         daw::json::constexpr_exec_tag{ }, tmp ) == expected;
}

struct test_001_t {
	int i = 0;
	double d = 0.0;
	bool b = false;
	std::string_view s{ };
	std::string_view s2{ };
	daw::bounded_vector_t<int, 10> y{ };
	std::optional<int> o{ };
	std::optional<int> o2{ };
	std::chrono::time_point<std::chrono::system_clock, std::chrono::milliseconds>
	  dte{ };

	DAW_CONSTEXPR test_001_t( ) = default;

	DAW_CONSTEXPR test_001_t( int Int, double Double, bool Bool,
	                          std::string_view S, std::string_view S2,
	                          daw::bounded_vector_t<int, 10> Y,
	                          std::optional<int> O, std::optional<int> O2,
	                          std::chrono::time_point<std::chrono::system_clock,
	                                                  std::chrono::milliseconds>
	                            D ) noexcept
	  : i( Int )
	  , d( Double )
	  , b( Bool )
	  , s( S )
	  , s2( S2 )
	  , y( Y )
	  , o( O )
	  , o2( O2 )
	  , dte( D ) {}
};

struct test_002_t {
	test_001_t a{ };
};

struct test_003_t {
	std::optional<test_001_t> a;
};

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
	std::abort( );
}

DAW_CONSTEXPR blah_t from_string( daw::tag_t<blah_t>,
                                  std::string_view sv ) noexcept {
	if( sv.empty( ) ) {
		daw_json_error( "Unexpected empty string_view" );
	}
	switch( sv.front( ) ) {
	case 'a':
		return blah_t::a;
	case 'b':
		return blah_t::b;
	case 'c':
		return blah_t::c;
	default:
		std::abort( );
	}
}

struct e_test_001_t {
	blah_t a = blah_t::a;
};

namespace daw::json {
	template<>
	struct json_data_contract<test_001_t> {
#ifdef __cpp_nontype_template_parameter_class
		using type =
		  json_member_list<json_number<"i", int>, json_number<"d">, json_bool<"b">,
		                   json_string_raw<"s", std::string_view>,
		                   json_string_raw<"s2", std::string_view>,
		                   json_array<"y", int, daw::bounded_vector_t<int, 10>>,
		                   json_number_null<"o", std::optional<int>>,
		                   json_number_null<"o2", std::optional<int>>,
		                   json_date<"dte">>;

#else
		static constexpr char const i[] = "i";
		static constexpr char const d[] = "d";
		static constexpr char const b[] = "b";
		static constexpr char const s[] = "s";
		static constexpr char const s2[] = "s2";
		static constexpr char const y[] = "y";
		static constexpr char const o[] = "o";
		static constexpr char const o2[] = "o2";
		static constexpr char const dte[] = "dte";
		using type =
		  json_member_list<json_number<i, int>, json_number<d>, json_bool<b>,
		                   json_string_raw<s, std::string_view>,
		                   json_string_raw<s2, std::string_view>,
		                   json_array<y, int, daw::bounded_vector_t<int, 10>>,
		                   json_number_null<o, std::optional<int>>,
		                   json_number_null<o2, std::optional<int>>,
		                   json_date<dte>>;
#endif
		static DAW_CONSTEXPR auto to_json_data( test_001_t const &v ) {
			return std::forward_as_tuple( v.i, v.d, v.b, v.s, v.s2, v.y, v.o, v.o2,
			                              v.dte );
		}
	};

	template<>
	struct json_data_contract<test_002_t> {
#ifdef __cpp_nontype_template_parameter_class
		using type = json_member_list<json_class<"a", test_001_t>>;
#else
		static constexpr char const a[] = "a";
		using type = json_member_list<json_class<a, test_001_t>>;
#endif
		static DAW_CONSTEXPR auto to_json_data( test_002_t const &v ) {
			return std::forward_as_tuple( v.a );
		}
	};

	template<>
	struct json_data_contract<test_003_t> {
#ifdef __cpp_nontype_template_parameter_class
		using type =
		  json_member_list<json_class_null<"a", std::optional<test_001_t>>>;
#else
		static constexpr char const a[] = "a";
		using type =
		  json_member_list<json_class_null<a, std::optional<test_001_t>>>;
#endif
		static DAW_CONSTEXPR auto to_json_data( test_003_t const &v ) {
			return std::forward_as_tuple( v.a );
		}
	};

	template<>
	struct json_data_contract<e_test_001_t> {
#ifdef __cpp_nontype_template_parameter_class
		using type = json_member_list<json_custom<"a", blah_t>>;
#else
		static constexpr char const a[] = "a";
		using type = json_member_list<json_custom<a, blah_t>>;
#endif
		static DAW_CONSTEXPR auto to_json_data( e_test_001_t const &v ) {
			return std::forward_as_tuple( v.a );
		}
	}; // namespace daw::json
} // namespace daw::json

DAW_CONSTEXPR auto const test_001_t_json_data =
  R"({
	    "d": -1.234e+3,
	    "i": 55,
			"b": true,
			"x": { "b": false, "c": [1,2,3] },
			"y": [1,2,3,4],
			"z": { "a": 1 },
	    "tp": "2018-06-22T15:05:37Z",
			"s": "yo yo yo",
			"s2": "ho ho ho",
			"o": 1344,
			"dte": "2016-12-31T01:02:03.343Z"
	  })";

DAW_CONSTEXPR bool test_004( ) {
	return daw::json::from_json<int, daw::json::NoCommentSkippingPolicyUnchecked>(
	         test_001_t_json_data, "i" ) == 55;
}

DAW_CONSTEXPR bool test_005( ) {
	return daw::json::from_json<int, daw::json::NoCommentSkippingPolicyUnchecked>(
	         test_001_t_json_data, "i" ) == 55;
}

DAW_CONSTEXPR bool test_006( ) {
	return daw::json::from_json<int>( test_001_t_json_data, "y[2]" ) == 3;
}

#if not defined( DAW_JSON_NO_CONST_EXPR ) and                                  \
  ( ( defined( __GNUC__ ) and __GNUC__ > 8 ) or defined( __clang__ ) or        \
    defined( _MSC_VER ) )
static_assert( test_004( ), "Unexpected value" );
static_assert( test_005( ), "Unexpected value" );
static_assert( test_006( ), "Unexpected value" );

#endif

DAW_CONSTEXPR char const json_data_array[] =
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
			"s2": "ho ho ho",
			"o": 1344,
			"dte": "2016-12-31T01:02:03.343Z"
	  },
	  {
	    "i": 55,
	    "d": -2.2,
			"b": true,
			"x": { "b": false, "c": [] },
			"y": [1,2,3,4],
			"z": { "a": 1 },
	    "tp": "2018-06-22T15:05:37Z",
			"s": "",
			"s2": "ho ho ho",
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
			"s2": "ho ho ho",
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
			"s2": "ho ho ho",
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
			"s2": "ho ho ho",
			"o": 1322,
			"dte": "2010-06-31T01:02:03.343Z"
	  }])";

struct EmptyClassTest {
	int a = 0;
};
DAW_CONSTEXPR bool operator==( EmptyClassTest const &l,
                               EmptyClassTest const &r ) {
	return l.a == r.a;
}

namespace daw::json {
	template<>
	struct json_data_contract<EmptyClassTest> {
		using type = json_member_list<>;

		static DAW_CONSTEXPR auto to_json_data( EmptyClassTest const & ) {
			return std::tuple<>{ };
		}
	};
} // namespace daw::json
#if not defined( DAW_JSON_NO_CONST_EXPR )
static constexpr std::string_view empty_class_data = R"(
{
	"b": { "a":[1,2,3,4], "b": true, "dfd": null },
	"c": 5
}
)";
static_assert( daw::json::from_json<EmptyClassTest>( test_001_t_json_data ) ==
               EmptyClassTest{ } );
#endif

struct Empty2 {
	EmptyClassTest b = EmptyClassTest{ };
	int c = 0;
};

namespace daw::json {
	template<>
	struct json_data_contract<Empty2> {
#ifdef __cpp_nontype_template_parameter_class
		using type =
		  json_member_list<json_class<"b", EmptyClassTest>, json_number<"c", int>>;
#else
		static constexpr char const b[] = "b";
		static constexpr char const c[] = "c";
		using type =
		  json_member_list<json_class<b, EmptyClassTest>, json_number<c, int>>;
#endif
		static DAW_CONSTEXPR auto to_json_data( Empty2 const &v ) {
			return std::forward_as_tuple( v.b );
		}
	};
} // namespace daw::json
#if not defined( DAW_JSON_NO_CONST_EXPR ) and                                  \
  ( not defined( _MSC_VER ) or defined( __clang__ ) )
static_assert( daw::json::from_json<Empty2>( empty_class_data ).c == 5 );
#endif

struct OptionalOrdered {
	int a = 0;
	std::optional<int> b{ };
};

namespace daw::json {
	template<>
	struct json_data_contract<OptionalOrdered> {
		using type =
		  json_ordered_member_list<int,
		                           json_number_null<no_name, std::optional<int>>>;

		static DAW_CONSTEXPR auto to_json_data( OptionalOrdered const &v ) {
			return std::forward_as_tuple( v.a, v.b );
		}
	};
} // namespace daw::json
constexpr std::string_view optional_ordered1_data = "[1]";
/*
static_assert(
  static_cast<bool>(
   not daw::json::from_json<OptionalOrdered>( optional_ordered1_data ).b ) );
*/

#if not defined( DAW_JSON_NO_INT128 ) and defined( __SIZEOF_INT128__ ) and     \
  ( not defined( _MSC_VER ) )
#ifdef __GNUC__
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpedantic"
#endif
void test128( ) {
	using namespace daw::json;
	DAW_CONSTEXPR std::string_view very_big_int =
	  "[170141183460469231731687303715884105727]";
	std::cout << "Trying to parse large int '" << very_big_int << "'\n";
	auto vec = from_json_array<json_number<no_name, __int128>>( very_big_int );
	__int128 val = vec[0];
	std::cout << "really big: " << std::hex
	          << static_cast<std::uint64_t>( val >> 64U ) << ' '
	          << static_cast<std::uint64_t>( val & 0xFFFF'FFFF'FFFF'FFFFULL )
	          << '\n';
	DAW_CONSTEXPR std::string_view very_negative_int =
	  "[-170141183460469231731687303715884105728]";
	std::cout << "Trying to parse large negative int '" << very_negative_int
	          << "'\n";
	vec = from_json_array<json_number<no_name, __int128>>( very_negative_int );
	val = vec[0];
	std::cout << "really negative: " << std::hex
	          << static_cast<std::uint64_t>( val >> 64U ) << ' '
	          << static_cast<std::uint64_t>( val & 0xFFFF'FFFF'FFFF'FFFFULL )
	          << '\n';
}
#ifdef __GNUC__
#pragma GCC diagnostic pop
#endif
#endif

int main( int, char ** ) try {
	std::cout << ( sizeof( std::size_t ) * 8U ) << "bit architecture\n";
	test_004( );
	test_005( );
	test_006( );
	{
		DAW_CONSTEXPR auto const v =
		  daw::json::from_json<OptionalOrdered>( optional_ordered1_data );
		daw::expecting( not v.b );
	}
	using namespace daw::json;
	daw::expecting(
	  not daw::json::from_json<OptionalOrdered>( optional_ordered1_data ).b );
	daw::expecting( parse_unsigned_test<uintmax_t>( "12345", 12345 ) );
	daw::expecting( parse_real_test<double>( "5", 5.0 ) );
	daw::expecting( parse_real_test<double>( "5.5", 5.5 ) );
	daw::expecting( parse_real_test<double>( "5.5e2", 550.0 ) );
	daw::expecting( parse_real_test<double>( "5.5e+2", 550.0 ) );
	daw::expecting( parse_real_test<double>( "5e2", 500.0 ) );
	daw::expecting( parse_real_test<double>( "5.5e+2", 550.0 ) );

#if defined( __GNUC__ ) and __GNUC__ <= 9
#define CX
#elif defined( _MSC_VER ) and not defined( __clang__ )
#define CX
#else
#define CX DAW_CONSTEXPR
#endif

#if not defined( DAW_JSON_NO_INT128 ) and defined( DAW_HAS_INT128 )
	test128( );
#else
	std::cout << "No 128bit int support detected\n";
#endif

	daw::do_not_optimize( test_001_t_json_data );
	CX auto data = daw::json::from_json<test_001_t>( test_001_t_json_data );
	{
		std::string tmp = to_json( data );
		auto data2 = daw::json::from_json<test_001_t>( tmp );
		daw::do_not_optimize( data2 );
	}
	{
		auto dtmp = to_json( data );
		std::cout << dtmp << '\n';
	}
	CX auto ary =
	  from_json_array<test_001_t, daw::bounded_vector_t<test_001_t, 10>>(
	    json_data_array );
	std::cout << "read in ";
	std::cout << ary.size( ) << " items\n";
	for( auto const &v : ary ) {
		std::cout << to_json( v ) << "\n\n";
	}
	std::cout << "as array\n";
	std::cout << to_json_array( ary ) << "\n\n";

	auto t2 = test_002_t{ data };
	t2.a.o2 = std::nullopt;
	std::cout << to_json( t2 ) << '\n';

	test_003_t t3{ data };
	std::cout << to_json( t3 ) << '\n';

	e_test_001_t t4{ };
	auto e_test_001_str = to_json( t4 );
	std::cout << e_test_001_str << '\n';
	auto e_test_001_back = from_json<e_test_001_t>( e_test_001_str );
	daw::do_not_optimize( e_test_001_back );

	DAW_CONSTEXPR std::string_view const json_data2 =
	  R"({
	"a": {
		"b.hi": {
			"c": [1,2,3] }}})";

	using iterator2_t = daw::json::json_array_iterator<int>;
	using namespace std::string_literals;

	auto first = iterator2_t( json_data2, "a.b\\.hi.c" );
	auto sum = 0;
	while( first ) {
		sum += *first;
		++first;
	}
	std::cout << "sum: " << sum << '\n';

	constexpr std::string_view const json_data3 =
	  R"({
	"a": {
		"b.hi": {
			"c": [{"x":1},{"x":2},{"x":3}] }}})";

	using iterator3_t = daw::json::json_array_iterator<NumberX>;

	auto first3 = iterator3_t( json_data3, "a.b\\.hi.c" );
	sum = 0;
	while( first3 ) {
		sum += first3->x;
		++first3;
	}
	std::cout << "sum2: " << sum << '\n';

	std::vector<double> a = { 1.1, 11.1 };
	std::cout << daw::json::to_json_array( a ) << '\n';

	using namespace daw::json;
	using num_t = json_number<no_name, double, LiteralAsStringOpt::Always>;
	std::cout << "Inf double: "
	          << to_json<std::string, double, num_t>(
	               std::numeric_limits<double>::infinity( ) )
	          << '\n';

	std::cout << "NaN double: "
	          << to_json<std::string, double, num_t>(
	               std::numeric_limits<double>::quiet_NaN( ) )
	          << '\n';

	std::cout << "denormal - DOUBLE_MIN/2 double: "
	          << to_json( std::numeric_limits<double>::min( ) / 2.0 ) << '\n';

	std::cout << "denormal min double: "
	          << to_json( std::numeric_limits<double>::denorm_min( ) ) << '\n';

	std::cout << "5E-324 -> " << from_json<double>( "5E-324" ) << '\n';
	std::cout << "1.1125369292536007E-308 -> "
	          << from_json<double>( "1.1125369292536007E-308" ) << '\n';

	std::cout << "min double: " << to_json( std::numeric_limits<double>::min( ) )
	          << '\n';
	std::cout << "2.2250738585072014E-308 -> "
	          << from_json<double>( "2.2250738585072014E-308" ) << '\n';
	std::cout << "2.2250738585072014E-307 -> "
	          << from_json<double>( "2.2250738585072014E-307" ) << '\n';
	std::cout << "0.22250738585072014E-307 -> "
	          << from_json<double>( "0.22250738585072014E-307" ) << '\n';

	std::cout << from_json<double>( "5E-324" ) << '\n';
	std::cout << "max double: " << to_json( std::numeric_limits<double>::max( ) )
	          << '\n';
	std::cout << "1.7976931348623157E308 -> "
	          << from_json<double>( "1.7976931348623157E308" ) << '\n';

	std::cout << "1.7976931348623157E307 -> "
	          << from_json<double>( "1.7976931348623157E307" ) << '\n';
	std::cout << "10.7976931348623157E307 -> "
	          << from_json<double>( "10.7976931348623157E307" ) << '\n';

} catch( daw::json::json_exception const &jex ) {
	std::cerr << "Exception thrown by parser: " << jex.reason( ) << std::endl;
	exit( 1 );
}
