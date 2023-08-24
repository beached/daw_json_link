// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/daw_json_link
//

#include "defines.h"

#include "daw/json/daw_json_exception.h"
#include "daw/json/daw_json_iterator.h"
#include "daw/json/daw_json_link.h"
#include "daw/json/impl/daw_json_exec_modes.h"

#include <daw/daw_arith_traits.h>
#include <daw/daw_benchmark.h>
#include <daw/daw_bounded_vector.h>
#include <daw/daw_span.h>

#include <cassert>
#include <cfloat>
#include <chrono>
#include <climits>
#include <cstdint>
#include <deque>
#include <iostream>
#include <list>
#include <map>
#include <optional>
#include <random>
#include <string_view>
#include <tuple>
#include <vector>

#define AS_CONSTEXPR( ... ) \
	[&]( ) constexpr {        \
		return __VA_ARGS__;     \
	}( )

/***
Temporarily disable the constexpr tests in MSVC when C++20
*/
#if defined( _MSC_VER ) and not defined( __clang__ ) and \
  defined( DAW_CXX_STANDARD ) and DAW_CXX_STANDARD == 20
#define DAW_JSON_NO_CONST_EXPR
#endif

static_assert( daw::is_arithmetic_v<int> );

struct NumberX {
	int x;
};

namespace daw::json {
	template<>
	struct json_data_contract<NumberX> {
#ifdef DAW_JSON_CNTTP_JSON_NAME
		using type = json_member_list<json_number<"x", int>>;
#else
		static constexpr char const x[] = "x";
		using type = json_member_list<json_number<x, int>>;
#endif
	};
} // namespace daw::json

template<typename Real, bool Trusted = false>
DAW_CONSTEXPR Real parse_real( std::string_view str ) {
	auto rng =
	  daw::json::BasicParsePolicy( str.data( ), str.data( ) + str.size( ) );
	return daw::json::json_details::parse_real<Real, false>( rng );
}

template<typename Real, bool Trusted = false>
DAW_CONSTEXPR bool parse_real_test( std::string_view str, Real expected ) {
	auto res = parse_real<Real, Trusted>( str );
	return not( res < expected or res > expected );
}

template<typename Unsigned, bool Trusted = false, size_t N>
DAW_CONSTEXPR bool parse_unsigned_test( char const ( &str )[N],
                                        Unsigned expected ) {
	using policy_t = daw::json::BasicParsePolicy<daw::json::parse_options(
	  daw::json::options::CheckedParseMode::no )>;
	auto tmp = policy_t( str, str + N );
	return daw::json::json_details::unsigned_parser<
	         Unsigned, daw::json::options::JsonRangeCheck::CheckForNarrowing,
	         false>( daw::json::constexpr_exec_tag{ }, tmp ) == expected;
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
	DAW_UNREACHABLE( );
}

DAW_CONSTEXPR blah_t from_string( daw::tag_t<blah_t>,
                                  std::string_view sv ) noexcept {
	test_assert( not sv.empty( ), "Unexpected empty string_view" );
	switch( sv.front( ) ) {
	case 'a':
		return blah_t::a;
	case 'b':
		return blah_t::b;
	case 'c':
		return blah_t::c;
	default:
		DAW_UNREACHABLE( );
	}
}

struct e_test_001_t {
	blah_t a = blah_t::a;
};

namespace daw::json {
	template<>
	struct json_data_contract<test_001_t> {
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

		static DAW_CONSTEXPR auto to_json_data( test_001_t const &v ) {
			return std::forward_as_tuple( v.i, v.d, v.b, v.s, v.s2, v.y, v.o, v.o2,
			                              v.dte );
		}
	};

	template<>
	struct json_data_contract<test_002_t> {
#ifdef DAW_JSON_CNTTP_JSON_NAME
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
#ifdef DAW_JSON_CNTTP_JSON_NAME
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
		static constexpr char const a[] = "a";
		using type = json_member_list<json_custom<a, blah_t>>;

		static DAW_CONSTEXPR auto to_json_data( e_test_001_t const &v ) {
			return std::forward_as_tuple( v.a );
		}
	}; // namespace daw::json
} // namespace daw::json

DAW_CONSTEXPR char const test_001_t_json_data[] =
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
	auto result = daw::json::from_json<int>(
	  test_001_t_json_data, "i",
	  daw::json::options::parse_flags<daw::json::options::CheckedParseMode::no> );

	if( result == 55 ) {
		return true;
	}
	throw result == 55;
}

DAW_CONSTEXPR bool test_005( ) {
	auto result = daw::json::from_json<int>(
	  test_001_t_json_data, "i",
	  daw::json::options::parse_flags<daw::json::options::CheckedParseMode::no> );
	return result == 55;
}

DAW_CONSTEXPR bool test_006( ) {
	auto result = daw::json::from_json<int>( test_001_t_json_data, "y[2]" );
	return result == 3;
}

#if not defined( DAW_JSON_NO_CONST_EXPR ) and                              \
  ( ( defined( __GNUC__ ) and __GNUC__ > 8 ) or defined( __clang__ ) ) and \
  ( not defined( DAW_JSON_NO_CONST_EXPR ) )
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
#ifdef DAW_JSON_CNTTP_JSON_NAME
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
#if not defined( DAW_JSON_NO_CONST_EXPR )
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
		  json_tuple_member_list<int, json_number_null_no_name<std::optional<int>>>;

		static DAW_CONSTEXPR auto to_json_data( OptionalOrdered const &v ) {
			return std::forward_as_tuple( v.a, v.b );
		}
	};
} // namespace daw::json
constexpr std::string_view optional_ordered1_data = "[1]";

#if not defined( DAW_JSON_NO_CONST_EXPR )
static_assert( static_cast<bool>(
  not daw::json::from_json<OptionalOrdered>( optional_ordered1_data ).b ) );
#endif

#if not defined( DAW_JSON_NO_INT128 ) and defined( __SIZEOF_INT128__ ) and \
  ( not defined( _MSC_VER ) ) and                                          \
  ( not defined( __clang__ ) and not defined( _LIBCPP_VERSION ) or         \
    __clang_major__ > 9 )
#ifdef __GNUC__
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpedantic"
#endif
void test128( ) {
	using namespace daw::json;
	DAW_CONSTEXPR std::string_view very_big_int =
	  "[170141183460469231731687303715884105727]";
	std::cout << "Trying to parse large int '" << very_big_int << "'\n";
	auto vec = from_json_array<json_number_no_name<__int128>>( very_big_int );
	__int128 val = vec[0];
	std::cout << "really big: " << std::hex
	          << static_cast<std::uint64_t>( val >> 64U ) << ' '
	          << static_cast<std::uint64_t>( val & 0xFFFF'FFFF'FFFF'FFFFULL )
	          << '\n';
	DAW_CONSTEXPR std::string_view very_negative_int =
	  "[-170141183460469231731687303715884105728]";
	std::cout << "Trying to parse large negative int '" << very_negative_int
	          << "'\n";
	vec = from_json_array<json_number_no_name<__int128>>( very_negative_int );
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

template<bool KnownBounds = false, bool Precise = false>
unsigned long long test_dblparse( std::string_view num,
                                  bool always_disp = false ) {
	if( always_disp ) {
		std::cout << "testing: '" << num << '\'';
	}
	char *nend = nullptr;
	double const strod_parse_dbl = std::strtod( num.data( ), &nend );

	constexpr auto dbl_lib_parser = []( std::string_view number ) {
		using namespace daw::json;
		auto rng = BasicParsePolicy<parse_options(
		  Precise ? options::IEEE754Precise::yes : options::IEEE754Precise::no )>(
		  std::data( number ), daw::data_end( number ) );
		if constexpr( KnownBounds ) {
			rng = json_details::skip_number( rng );
		}
		using json_member = json_details::json_deduced_type<double>;
		return json_details::parse_value<json_member, KnownBounds>(
		  rng, ParseTag<json_member::expected_type>{ } );
	};
	auto lib_parse_dbl = dbl_lib_parser( num );
	auto const ui0 = daw::bit_cast<std::uint64_t>( lib_parse_dbl );
	auto ui1 = daw::bit_cast<std::uint64_t>( strod_parse_dbl );
	auto const diff = std::max( ui0, ui1 ) - std::min( ui0, ui1 );
	if( always_disp ) {
		auto const old_precision = std::cout.precision( );
		std::cout.precision( std::numeric_limits<double>::max_digits10 );
		std::cout << "->ulp diff: " << std::dec << diff << '\n';
		std::cout.precision( old_precision );
	}
#ifndef NDEBUG
	if( diff > ( Precise ? 0 : 2 ) ) {
		auto const old_precision = std::cout.precision( );
		// Do again to do it from debugger

		lib_parse_dbl = dbl_lib_parser( num );

		std::cout.precision( std::numeric_limits<double>::max_digits10 );
		std::cout << "orig: " << num << '\n';
		std::cout << "daw_json_link: " << lib_parse_dbl << '\n'
		          << "strtod: " << strod_parse_dbl << '\n';
		std::cout << "diff: " << ( lib_parse_dbl - strod_parse_dbl ) << '\n';

		std::cout.precision( old_precision );
		std::cout << std::dec << "ULP diff: " << diff << '\n';
		if( diff > 3 ) {
			std::cerr << "ERROR: Number parsed out of range\n";
			exit( 1 );
		}
	}
#endif
	return diff;
}

template<bool KnownBounds = false>
unsigned long long test_dblparse2( std::string_view num, double orig,
                                   bool always_disp = false ) {
	if( always_disp ) {
		std::cout << "testing: '" << num << '\'';
	}
	double lib_parse_dbl = [&] {
		if constexpr( KnownBounds ) {
			auto rng =
			  daw::json::BasicParsePolicy( num.data( ), num.data( ) + num.size( ) );
			rng = daw::json::json_details::skip_number( rng );
			using json_member = daw::json::json_details::json_deduced_type<double>;
			return daw::json::json_details::parse_value<json_member, KnownBounds>(
			  rng, daw::json::ParseTag<json_member::expected_type>{ } );
		} else {
			return daw::json::from_json<double, KnownBounds>( num );
		}
	}( );

	char *nend = nullptr;
	double const strod_parse_dbl = std::strtod( num.data( ), &nend );

	auto const ui0 = daw::bit_cast<std::uint64_t>( lib_parse_dbl );
	auto const ui1 = daw::bit_cast<std::uint64_t>( strod_parse_dbl );
	auto const diff = std::max( ui0, ui1 ) - std::min( ui0, ui1 );
	if( always_disp ) {
		auto const old_precision = std::cout.precision( );
		std::cout.precision( std::numeric_limits<double>::max_digits10 );
		std::cout << "->ulp diff: " << std::dec << diff << '\n';
		std::cout.precision( old_precision );
	}
#ifndef NDEBUG
	if( diff > 2 ) {
		double o = orig;
		(void)o;
		double orig_ninf =
		  std::nextafter( orig, -std::numeric_limits<double>::infinity( ) );
		(void)orig_ninf;
		double orig_inf =
		  std::nextafter( orig, std::numeric_limits<double>::infinity( ) );
		(void)orig_inf;

		auto const old_precision = std::cout.precision( );
		// Do again to do it from debugger

		lib_parse_dbl = [&] {
			if constexpr( KnownBounds ) {
				auto rng =
				  daw::json::BasicParsePolicy( num.data( ), num.data( ) + num.size( ) );
				rng = daw::json::json_details::skip_number( rng );
				using json_member = daw::json::json_details::json_deduced_type<double>;
				return daw::json::json_details::parse_value<json_member, KnownBounds>(
				  rng, daw::json::ParseTag<json_member::expected_type>{ } );
			} else {
				return daw::json::from_json<double, KnownBounds>( num );
			}
		}( );
		std::cout.precision( std::numeric_limits<double>::max_digits10 );
		std::cout << "orig: " << num << '\n';
		std::cout << "daw_json_link: " << lib_parse_dbl << '\n'
		          << "strtod: " << strod_parse_dbl << '\n';
		std::cout << "diff: " << ( lib_parse_dbl - strod_parse_dbl ) << '\n';

		std::cout.precision( old_precision );
		std::cout << std::dec << "unsigned diff: " << diff << '\n';
		daw::json::daw_json_error( daw::json::ErrorReason::NumberOutOfRange );
	}
#else
	(void)orig;
#endif
	return diff;
}

template<bool KnownBounds = false, bool Precise = false,
         int NUM_VALS = 1'000'000, int exp_min = -308, int exp_max = +308>
void test_lots_of_doubles( ) {
	auto rd = std::random_device( );
	auto rng = std::mt19937_64( rd( ) );
	struct tracking_t {
		std::size_t count = 0;
		double min_value = std::numeric_limits<double>::max( );
		double max_value = std::numeric_limits<double>::min( );

		tracking_t( ) = default;

		void add( double d ) {
			++count;
			if( d < min_value ) {
				min_value = d;
			} else if( d > max_value ) {
				max_value = d;
			}
		}
	};
	auto dist = std::map<unsigned long long, tracking_t>( );
	for( int i = 0; i < NUM_VALS; ++i ) {
		unsigned long long x1 = rng( );
		unsigned long long x2 = rng( );
		int x3 = std::uniform_int_distribution<>( exp_min, exp_max )( rng );
		char buffer[128];
		std::snprintf( buffer, 128, "%llu.%llue%d", x1, x2, x3 );

		char *nend = nullptr;
		double const strod_parse_dbl = std::strtod( buffer, &nend );

		dist[test_dblparse<KnownBounds, Precise>( buffer )].add( strod_parse_dbl );
	}
	std::cout << std::dec << "distribution of diff:\n";
	for( auto const &p : dist ) {
		std::cout << "difference: " << p.first << " count: " << p.second.count
		          << " from [" << p.second.min_value << ',' << p.second.max_value
		          << "]\n";
	}
}

template<int NUM_VALS = 100, int exp_min = -5, int exp_max = 6>
void test_show_lots_of_doubles( ) {
	auto rd = std::random_device( );
	auto rng = std::mt19937_64( rd( ) );
	std::cout << "Compare output of doubles\n";
	std::cout << "*************************\n";
	for( int i = 0; i < NUM_VALS; ++i ) {
		unsigned long long x1 = rng( );
		unsigned long long x2 = rng( );
		/*
		auto x1 =
		  std::uniform_int_distribution<unsigned long long>( 1, 100 )( rng );
		auto x2 =
		  std::uniform_int_distribution<unsigned long long>( 1, 100 )( rng );
		  */
		int x3 = std::uniform_int_distribution<>( exp_min, exp_max )( rng );
		char buffer[128]{ };
		std::snprintf( buffer, 128, "%llu.%llue%d", x1, x2, x3 );

		char *nend = nullptr;
		double const strod_parse_dbl = std::strtod( buffer, &nend );
		std::cout << std::string_view( buffer ) << ": "
		          << daw::json::to_json( strod_parse_dbl ) << ": ";
		printf( "%g\n", strod_parse_dbl );
		/*
		          << std::to_string( strod_parse_dbl ) << '\n';*/
	}
	std::cout << "*************************\n";
}

bool test_optional_array( ) {
	std::string_view const json_data = "[null,5]";
	using namespace daw::json;
	auto result = from_json_array<std::optional<int>>( json_data );
	daw_json_ensure( result.size( ) == 2 and not result[0] and result[1] == 5,
	                 ErrorReason::Unknown );
	auto str = std::string{ };
	to_json_array<json_number_null_no_name<std::optional<int>>>( result, str );
	auto result2 =
	  from_json_array<json_number_null_no_name<std::optional<int>>>( str );
	return result == result2;
}

bool test_key_value( ) {
	std::string_view const json_data = R"({"a":0,"b":1})";
	using namespace daw::json;
	auto const result = from_json<std::map<std::string, int>>( json_data );
	daw_json_ensure( result.size( ) == 2 and result.at( "a" ) == 0 and
	                   result.at( "b" ) == 1,
	                 ErrorReason::Unknown );
	auto str = std::string{ };
	(void)to_json( result, str );
	auto result2 = from_json<std::map<std::string, int>>( str );
	return result == result2;
}

bool test_vector_of_bool( ) {
	std::string const json_data = "[true,false,true]";
	auto const rv0 = daw::json::from_json_array<bool>( json_data );
	assert( rv0.size( ) == 3 and rv0.at( 0 ) and not rv0.at( 1 ) and
	        rv0.at( 2 ) );
	auto const str0 = daw::json::to_json_array( rv0 );
	auto const rv1 = daw::json::from_json<std::vector<bool>>( str0 );
	return rv0 == rv1;
}

struct empty_ordered {};
namespace daw::json {
	template<>
	struct json_data_contract<empty_ordered> {
		using type = json_tuple_member_list<>;

		static constexpr std::tuple<> to_json_data( empty_ordered ) {
			return { };
		}
	};
} // namespace daw::json

#if defined( DAW_CX_BIT_CAST )
constexpr bool cxdbl_tostr1( ) {
	using namespace daw::json;
	constexpr auto dbl_half = from_json<double>( "0.5" );
	char buffer[128]{ };
	auto buff_end = to_json( dbl_half, daw::span( buffer ) ).data( );
	auto buff_sv =
	  std::string_view( buffer, static_cast<std::size_t>( buff_end - buffer ) );
	daw_json_ensure( buff_sv == "0.5", ErrorReason::InvalidString );
	(void)from_json<double>( buff_sv );
	return true;
}
static_assert( cxdbl_tostr1( ) );

constexpr bool cxdbl_tostr2( ) {
	using namespace daw::json;
	constexpr auto dbl_half = from_json<double>( "1024.5" );
	char buffer[128]{ };
	auto buff_end = to_json( dbl_half, daw::span( buffer ) ).data( );
	auto buff_sv =
	  std::string_view( buffer, static_cast<std::size_t>( buff_end - buffer ) );
	daw_json_ensure( buff_sv == "1024.5", ErrorReason::InvalidString );
	(void)from_json<double>( buff_sv );
	return true;
}
static_assert( cxdbl_tostr2( ) );
#endif

struct Foo1 {};

struct Foo2 {
	std::optional<Foo1> m1;
	std::shared_ptr<int> m2;
};

namespace daw::json {
	template<>
	struct json_data_contract<Foo1> {
		using type = json_member_list<>;

		static constexpr std::tuple<> to_json_data( Foo1 const & ) {
			return { };
		}
	};
	template<>
	struct json_data_contract<Foo2> {
		static constexpr char const m1[] = "m1";
		static constexpr char const m2[] = "m2";
		using type = json_member_list<json_class_null<m1, std::optional<Foo1>>,
		                              json_link<m2, std::shared_ptr<int>>>;

		static constexpr auto to_json_data( Foo2 const &val ) {
			return std::forward_as_tuple( val.m1, val.m2 );
		}
	};
} // namespace daw::json

int main( int, char ** ) {
#ifdef DAW_USE_EXCEPTIONS
	try {
#endif
		constexpr daw::string_view foo2_json =
		  R"json( { "m1": {}, "m2": 42  } )json";
		DAW_CONSTEXPR auto foo1_val = daw::json::from_json<Foo1>( foo2_json, "m1" );
		(void)foo1_val;
#if not defined( DAW_JSON_NO_CONST_EXPR )
		static_assert( std::is_same_v<DAW_TYPEOF( foo1_val ), Foo1> );
#else
	assert( (std::is_same_v<DAW_TYPEOF( foo1_val ), Foo1>));
#endif

		auto foo2_val = daw::json::from_json<Foo2>( foo2_json );
		ensure( foo2_val.m1 );
		auto const foo2_str = daw::json::to_json( foo2_val );
		(void)foo2_str;
		using namespace std::string_literals;
		std::cout << ( sizeof( std::size_t ) * 8U ) << "bit architecture\n";
		using namespace daw::json;
		auto oo_result =
		  daw::json::from_json<OptionalOrdered>( optional_ordered1_data );
		daw::expecting( not oo_result.b );
		daw::expecting( parse_unsigned_test<uintmax_t>( "12345", 12345 ) );
		daw::expecting( parse_real_test<double>( "5", 5.0 ) );
		daw::expecting( parse_real_test<double>( "5.5", 5.5 ) );
		daw::expecting( parse_real_test<double>( "5.5e2", 550.0 ) );
		daw::expecting( parse_real_test<double>( "5.5e+2", 550.0 ) );
		daw::expecting( parse_real_test<double>( "5e2", 500.0 ) );
		daw::expecting( parse_real_test<double>( "5.5e+2", 550.0 ) );
		daw::expecting( parse_real_test<double>(
		  std::string_view( test_001_t_json_data ).substr( 12, 9 ), -1.234e+3 ) );
		std::cout << "'" << std::string_view( test_001_t_json_data ).substr( 33, 2 )
		          << "'\n";
		daw::expecting( parse_real_test<double>(
		  std::string_view( test_001_t_json_data ).substr( 33, 2 ), 55 ) );

		test_004( );
		test_005( );
		test_006( );
		{
			DAW_CONSTEXPR auto const v =
			  daw::json::from_json<OptionalOrdered>( optional_ordered1_data );
			daw::expecting( not v.b );
		}
#if( defined( __GNUC__ ) and __GNUC__ <= 9 ) or ( defined( _MSC_VER ) )
#define CX
#elif defined( DAW_JSON_NO_CONST_EXPR )
#define CX
#else
#define CX DAW_CONSTEXPR
#endif

#if not defined( DAW_JSON_NO_INT128 ) and defined( DAW_HAS_INT128 ) and \
  ( not defined( _MSC_VER ) ) and                                       \
  ( not defined( __clang__ ) and not defined( _LIBCPP_VERSION ) or      \
    __clang_major__ > 9 )
		test128( );
#else
	std::cout << "No 128bit int support detected\n";
#endif

		// daw::do_not_optimize( test_001_t_json_data );
		CX auto data = daw::json::from_json<test_001_t>( test_001_t_json_data );
		{
			std::string tmp = to_json( data );
			auto data2 = daw::json::from_json<test_001_t>( tmp );
			daw::do_not_optimize( data2 );
		}
		to_json( data, std::cout ) << '\n';
		CX auto ary =
		  from_json_array<test_001_t, daw::bounded_vector_t<test_001_t, 10>>(
		    json_data_array );
		std::cout << "read in ";
		std::cout << ary.size( ) << " items\n";
		for( auto const &v : ary ) {
			to_json( v, std::cout ) << "\n\n";
		}
		std::cout << "as array\n";
		to_json_array( ary, std::cout ) << "\n\n";

		auto t2 = test_002_t{ data };
		t2.a.o2 = std::nullopt;
		to_json( t2, std::cout ) << '\n';

		test_003_t t3{ data };
		to_json( t3, std::cout ) << '\n';

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
		using namespace std::string_view_literals;

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
		to_json_array( a, std::cout ) << '\n';

		using namespace daw::json;
		using num_t =
		  json_number_no_name<double, options::number_opt(
		                                options::LiteralAsStringOpt::Always,
		                                options::JsonNumberErrors::AllowNanInf )>;
		std::cout << "Inf double: "
		          << "serialize: "
		          << to_json<num_t>( std::numeric_limits<double>::infinity( ) )
		          << '\n';
		std::cout << "parse: " << from_json<num_t>( R"("Infinity")" ) << '\n';
		std::cout << "-Inf double: "
		          << "serialize: "
		          << to_json<num_t>( -std::numeric_limits<double>::infinity( ) )
		          << '\n';
		std::cout << "parse: " << from_json<num_t>( R"("-Infinity")" ) << '\n';

		std::cout << "NaN double: "
		          << "serialize: "
		          << to_json<num_t>( std::numeric_limits<double>::quiet_NaN( ) )
		          << '\n';
		std::cout << "parse: " << from_json<num_t>( R"("NaN")" ) << '\n';

		std::cout << "Negative 0: "
		          << "serialize: " << to_json<num_t>( std::copysign( 0.0, -1.0 ) )
		          << '\n';

		std::cout << "parse: " << from_json<double>( "-0.0" ) << '\n';

		std::cout << "denormal - DOUBLE_MIN/2 double: "
		          << to_json( std::numeric_limits<double>::min( ) / 2.0 ) << '\n';

		std::cout << "denormal min double: "
		          << to_json( std::numeric_limits<double>::denorm_min( ) ) << '\n';

		std::cout << "5E-324 -> " << from_json<double>( "5E-324" ) << '\n';
		std::cout << "1.1125369292536007E-308 -> "
		          << AS_CONSTEXPR( from_json<double>( "1.1125369292536007E-308" ) )
		          << '\n';

		std::cout << "min double: "
		          << to_json( std::numeric_limits<double>::min( ) ) << '\n';
		std::cout << "2.2250738585072014E-308 -> "
		          << AS_CONSTEXPR( from_json<double>( "2.2250738585072014E-308" ) )
		          << '\n';
		std::cout << "2.2250738585072014E-307 -> "
		          << AS_CONSTEXPR( from_json<double>( "2.2250738585072014E-307" ) )
		          << '\n';
		std::cout << "0.22250738585072014E-307 -> "
		          << AS_CONSTEXPR( from_json<double>( "0.22250738585072014E-307" ) )
		          << '\n';

		std::cout << AS_CONSTEXPR( from_json<double>( "5E-324" ) ) << '\n';
		std::cout << "max double: "
		          << to_json( std::numeric_limits<double>::max( ) ) << '\n';
		std::cout << "1.7976931348623157E308 -> "
		          << AS_CONSTEXPR( from_json<double>( "1.7976931348623157E308" ) )
		          << '\n';

		std::cout << "1.7976931348623157E307 -> "
		          << AS_CONSTEXPR( from_json<double>( "1.7976931348623157E307" ) )
		          << '\n';
		std::cout << "10.7976931348623157E307 -> "
		          << AS_CONSTEXPR( from_json<double>( "10.7976931348623157E307" ) )
		          << '\n';
		std::cout << "9e2147483609 -> "
		          << AS_CONSTEXPR( from_json<double>( "9e2147483609" ) ) << '\n';
		std::cout << "0."
		             "0000000000000000000000000000000000000000000000000000000000000"
		             "0000000000"
		             "0000000000000000000000000000000000000000000000000000000000000"
		             "0000000000"
		             "0000000000000000000000000000000000000000000000000000000000000"
		             "0000000000"
		             "0000000000000000000000000000000000000000000000000000000000000"
		             "0000000000"
		             "0000000000000000000000000 -> "
		          << AS_CONSTEXPR(
		               from_json<double>( "0."
		                                  "0000000000000000000000000000000000000000"
		                                  "00000000000000000000000000000"
		                                  "0000000000000000000000000000000000000000"
		                                  "00000000000000000000000000000"
		                                  "0000000000000000000000000000000000000000"
		                                  "00000000000000000000000000000"
		                                  "0000000000000000000000000000000000000000"
		                                  "00000000000000000000000000000"
		                                  "000000000000000000000000000000000" ) )
		          << '\n';

		std::cout << "3e-3330000000000000000000000000000000 -> "
		          << AS_CONSTEXPR( from_json<double>(
		               "3e-3330000000000000000000000000000000" ) )
		          << '\n';

		std::cout << "DAW***********************************\n";
		test_dblparse2( "1217.2772861138403", 1217.2772861138403, true );
		test_dblparse2( "-161.68713249779881", -161.68713249779881, true );
		test_dblparse2( "267.04251495962637", 267.04251495962637, true );
		test_dblparse2( "1002.9111801605201", 1002.9111801605201, true );
		test_dblparse2( "-599.61476423470071", -599.61476423470071, true );
		test_dblparse2( "2137.0241926849581", 2137.0241926849581, true );
		test_dblparse2( "-0.0", std::copysign( 0.0, -1.0 ), true );
		std::cout << "DAW***********************************\n";
		test_dblparse2( "5792711765526609591.9963073925412025509e-82",
		                5792711765526609591.9963073925412025509e-82 );
		test_dblparse( "4891559871276714924261e222", true );
		test_dblparse(
		  "111111111111111111111111111111111111111111111111111111111111111111111111"
		  "11"
		  "111111111111111111111111111111111111111111111111111111111111111111111111"
		  "11"
		  "111111111111111111111111111111111111111111111111111111111111111111111111"
		  "11"
		  "111111111111111111111111111111111111111111111111111111111111111111111111"
		  "11"
		  "111111111111111111111111111111111111111111111111111111111111111111111111"
		  "11"
		  "111111111111111111111111111111.0e-100",
		  true );
		test_dblparse(
		  "111111111111111111111111111111111111111111111111111111111111111111111111"
		  "11"
		  "111111111111111111111111111111111111111111111111111111111111111111111111"
		  "11"
		  "111111111111111111111111111111111111111111111111111111111111111111111111"
		  "11"
		  "111111111111111111111111111111111111111111111111111111111111111111111111"
		  "11"
		  "111111111111111111111111111111111111111111111111111111111111111111111111"
		  "11"
		  "111111111111111111111111111111.0e+100",
		  true );
		test_dblparse( "14514284786278117030.4620546740167642908e-104", true );
		test_dblparse( "560449937253421.57275338353451748e-223", true );
		test_dblparse( "127987629894956.6249879371780786496e-274", true );
		test_dblparse( "19700720435664.186294290058937593e13", true );
		test_dblparse( "5.9409999999999999999996e-324", true );
		test_dblparse( "9728625633136924125.18356202983677566044e-308", true );
		test_dblparse( "9728625633136924125.18356202983677566044e-500", true );
		test_dblparse( "10199214983525025199.13135016100190689227e308", true );
		test_dblparse( "10199214983525025199.13135016100190689227e-308", true );
		test_dblparse( "0.000000000000000000000000000000000000001e-308", true );
		test_dblparse( "6372891218502368041059e64", true );
		test_dblparse( "9223372036854776000e100", true );
		test_dblparse( "9223372036854776000e2000", true );
		test_dblparse( "9223372036854776000e+20", true );
		test_dblparse( "9223372036854776000e-2000", true );
		test_dblparse( "2e-1000", true );
		test_dblparse( "42.69", true );
		test_dblparse( "1e-1000", true );
		test_dblparse( "78146521210545563.1397450998275178158e-308", true );
		test_dblparse( "8725540998407961.3743556965848965343e-308", true );
		test_dblparse( "1e-10000", true );
		test_dblparse<false, true>( "0.9868011474609375", true );
		std::cout.precision( std::numeric_limits<double>::max_digits10 );
#if defined( LDBL_MAX )
		std::cout << "result: " << from_json<long double>( "1e-10000" ) << '\n';
#endif
		test_dblparse( "1e-214748364", true );
		test_dblparse( "0.89", true );
		test_dblparse( "10070988951557009.8178168006534510403e-302", true );
		test_dblparse(
		  "2."
		  "225073858507201136057409796709131975934819546351645648023426109724822222"
		  "02"
		  "107694551652952390813508791414915891303962110687008643869459464552765720"
		  "74"
		  "078206217433799881410632673292535522868813721490129811224514518898490572"
		  "22"
		  "307285255133155755015914397476397983411801999323962548289017107081850690"
		  "63"
		  "066665599493827577257201576306269066333264756530000924588831643303777979"
		  "18"
		  "696120494973903778297049050510806099407302629371289589500035837999672072"
		  "54"
		  "304360284078895771796150945516748243471030702609144621572289880258182545"
		  "18"
		  "032570701886087211312807951223342628836862232150377566662250398253433597"
		  "45"
		  "688844239002654981983854879482922068947216898310996983658468140228542433"
		  "30"
		  "660339850886445804001034933970427567186443383770486037861622771738545623"
		  "06"
		  "5874679014086723327636718751234567890123456789012345678901e-308",
		  true );
		test_dblparse( "0.9868011474609375", true );
		std::cout.precision( std::numeric_limits<double>::max_digits10 );
#if defined( LDBL_MAX )
		std::cout << "long double result: "
		          << from_json<long double>( "0.9868011474609375" ) << '\n';
#endif
		std::cout << "Default FP Parse\n";
		std::cout << "Unknown Bounds\n";
		test_lots_of_doubles<false, false>( );
		std::cout << "Known Bounds\n";
		test_lots_of_doubles<true, false>( );
		std::cout << "Precise FP Parse\n";
		std::cout << "Unknown Bounds\n";
		test_lots_of_doubles<false, true>( );
		std::cout << "Known Bounds\n";
		test_lots_of_doubles<true, true>( );

#if defined( LDBL_MAX )
		if constexpr( sizeof( double ) < sizeof( long double ) ) {
			std::cout << "long double test\n";
			std::cout << std::setprecision(
			               std::numeric_limits<long double>::max_digits10 )
			          << from_json<long double>(
			               "11111111111111111111111111111111111111111111"
			               "11111111111111111111111111111111111111111111"
			               "11111111111111111111111111111111111111111111"
			               "11111111111111111111111111111111111111111111"
			               "11111111111111111111111111111111111111111111"
			               "11111111111111111111111111111111111111111111"
			               "11111111111111111111111111111111111111111111"
			               "11111111111111111111111111111111111111111111"
			               "111111111111111111"
			               "111111111111111111111111111111.0e+400" )
			          << '\n';

			std::cout << "testing 9223372036854776000e100\n";
			constexpr std::string_view two63e100 = "9223372036854776000e100";
			auto const d0 = from_json<long double>(
			  two63e100, options::parse_flags<options::ExecModeTypes::runtime> );
			std::cout << d0 << '\n';
			std::cout << "using strtold\n";
			char *end = nullptr;
			long double const d1 = strtold( two63e100.data( ), &end );
			std::cout << d1 << '\n';
			double d2 = 0.89;
			to_json( d2, std::cout ) << '\n';
		}
#endif
		test_show_lots_of_doubles( );
		test_optional_array( );
		test_key_value( );
		test_vector_of_bool( );
		static_assert( from_json<bool>( "true" ) );
		static_assert( not from_json<bool>( "false" ) );
		static_assert( not *from_json<std::optional<bool>>( "false" ) );
		static_assert( not from_json<std::optional<bool>>( "null" ) );
		static_assert( from_json<signed char>( "-1" ) ==
		               static_cast<signed char>( -1 ) );
		static_assert( from_json<short>( "-1" ) == static_cast<short>( -1 ) );
		static_assert( from_json<int>( "-1" ) == -1 );
		static_assert( from_json<long>( "-1" ) == -1 );
		static_assert( from_json<long long>( "-1" ) == -1 );
		static_assert( from_json<unsigned char>( "1" ) == 1 );
		static_assert( from_json<unsigned short>( "1" ) == 1 );
		static_assert( from_json<unsigned int>( "1" ) == 1 );
		static_assert( from_json<unsigned long>( "1" ) == 1 );
		static_assert( from_json<unsigned long long>( "1" ) == 1 );
		assert( from_json<std::string>( R"("hello world")" ) == "hello world" );
		assert( from_json<std::deque<int>>( "[1,2,3]"s ).at( 1 ) == 2 );
		assert( from_json<std::list<int>>( "[1,2,3]"s ).size( ) == 3 );
		assert( ( from_json<json_array_no_name<char, std::string>>(
		            "[97,98,99]"s ) == "abc" ) );
#if not defined( DAW_JSON_USE_FULL_DEBUG_ITERATORS )
		static_assert( from_json<std::array<int, 4>>( "[1,2,3]"sv )[1] == 2 );
#else
	assert( ( from_json<std::array<int, 4>>( "[1,2,3]"sv )[1] == 2 ) );
#endif

		auto const test_bad_float = []( ) -> bool {
#ifdef DAW_USE_EXCEPTIONS
			try {
#endif
				(void)from_json<double>( "0e "sv );
#ifdef DAW_USE_EXCEPTIONS
			} catch( daw::json::json_exception const &ex ) {
				(void)ex;
				return true;
			}
#endif
			return false;
		};
		daw_json_ensure( test_bad_float( ), ErrorReason::Unknown );

		auto const test_empty_map = []( ) -> bool {
#ifdef DAW_USE_EXCEPTIONS
			try {
#endif
				auto m = from_json<std::map<std::string, std::string>>( "{}"sv );
				if( not m.empty( ) ) {
					return false;
				}
				auto const s = to_json( m );

				return s == "{}";
#ifdef DAW_USE_EXCEPTIONS
			} catch( daw::json::json_exception const &jex ) {
				std::cerr << "Exception thrown by parser: " << jex.reason( )
				          << std::endl;
				throw;
			}
#endif
		};
		daw_json_ensure( test_empty_map( ), ErrorReason::Unknown );

		auto const test_leading_zero = []( auto i ) {
			using test_t = daw::remove_cvref_t<decltype( i )>;
#ifdef DAW_USE_EXCEPTIONS
			try {
#endif
				auto l0 = from_json<test_t>( "01.0"sv );
				(void)l0;
#ifdef DAW_USE_EXCEPTIONS
			} catch( daw::json::json_exception const & ) { return true; }
#endif
			return false;
		};
		daw_json_ensure( test_leading_zero( 0.0 ), ErrorReason::Unknown );
		daw_json_ensure( test_leading_zero( 0 ), ErrorReason::Unknown );
		daw_json_ensure( test_leading_zero( 0U ), ErrorReason::Unknown );

		static_assert(
		  from_json<
		    json_key_value_no_name<std::array<std::pair<std::string_view, int>, 2>,
		                           int, std::string_view>>( R"({"a":0,"b":1})" )[1]
		    .second == 1 );

		constexpr auto v =
		  from_json<tuple_json_mapping<>>( std::string_view( "{}" ) );
		auto vstr = to_json( v );
		(void)vstr;
		constexpr auto v1 = from_json<empty_ordered>( std::string_view( "[]" ) );
		auto v1str = to_json( v1 );
		(void)v1str;

		std::cout << "FP Output formating of 123456789.23456789012345:\n";
		constexpr double outfmt_dbl = 123456789.23456789012345;
		std::cout
		  << "auto: "
		  << to_json<json_base::json_number<
		       double, options::number_opt( options::FPOutputFormat::Auto )>>(
		       outfmt_dbl )
		  << '\n';
		std::cout
		  << "decimal: "
		  << to_json<json_base::json_number<
		       double, options::number_opt( options::FPOutputFormat::Decimal )>>(
		       outfmt_dbl )
		  << '\n';
		std::cout
		  << "scientific: "
		  << to_json<json_base::json_number<
		       double, options::number_opt( options::FPOutputFormat::Scientific )>>(
		       outfmt_dbl )
		  << '\n';

		auto byte_vec = to_json( 5.5, std::vector<std::byte>{ } );
		ensure( byte_vec.size( ) == 3 );
		ensure( static_cast<char>( byte_vec[0] ) == '5' );
		ensure( static_cast<char>( byte_vec[1] ) == '.' );
		ensure( static_cast<char>( byte_vec[2] ) == '5' );
		(void)byte_vec;

		daw::string_view jd_opt_jv1 = R"json({"name":5,"name2":{"foo":6}})json";
		auto jv = daw::json::json_value( jd_opt_jv1 );
		auto opt_int0 = as<std::optional<int>>( jv["name"] );
		ensure( opt_int0 );
		ensure( *opt_int0 == 5 );
		auto opt_int1 = as<std::optional<int>>( jv["name2.foo"] );
		ensure( opt_int1 );
		ensure( *opt_int1 == 6 );
		auto opt_int2 = as<std::optional<int>>( jv["name2.bar"] );
		ensure( not opt_int2 );
		auto opt_int3 =
		  daw::json::from_json<std::optional<int>>( jd_opt_jv1, "name" );
		ensure( opt_int3 );
		ensure( *opt_int3 == 5 );
		auto opt_int4 =
		  daw::json::from_json<std::optional<int>>( jd_opt_jv1, "name2.foo" );
		ensure( opt_int4 );
		ensure( *opt_int4 == 6 );
		auto opt_int5 =
		  daw::json::from_json<std::optional<int>>( jd_opt_jv1, "name2.bar" );
		ensure( not opt_int5 );

		using strsigned_t = json_number_no_name<
		  std::int64_t, options::number_opt( options::LiteralAsStringOpt::Always )>;

		std::string negnumber_str = to_json<strsigned_t>( -1234567890LL );
		ensure( negnumber_str == R"("-1234567890")" );

		constexpr std::string_view ts = "\"2016-12-31T01:02:03.343Z\"";
		using tp_t = std::chrono::time_point<std::chrono::system_clock,
		                                     std::chrono::nanoseconds>;
		constexpr auto parsed_dte =
		  daw::json::from_json<daw::json::json_date_no_name<tp_t>>( ts );
		(void)parsed_dte;
#if defined( __cpp_lib_chrono ) and __cpp_lib_chrono >= 201907
		std::cout << ts << " parsed as " << parsed_dte << '\n';
#endif
		constexpr std::string_view ts2 = "\"2016-12-31T01:02:03.123456789Z\"";
		constexpr auto parsed_dte2 =
		  daw::json::from_json<daw::json::json_date_no_name<tp_t>>( ts2 );
		(void)parsed_dte;
#if defined( __cpp_lib_chrono ) and __cpp_lib_chrono >= 201907
		std::cout << ts2 << " parsed as " << parsed_dte2 << '\n';
#endif
		auto const parsed_dte_str =
		  daw::json::to_json<daw::json::json_date_no_name<tp_t>>( parsed_dte );
		std::cout << "round trip of " << ts << " became " << parsed_dte_str << '\n';
		ensure( ts == parsed_dte_str );
		auto const parsed_dte2_str =
		  daw::json::to_json<daw::json::json_date_no_name<tp_t>>( parsed_dte2 );
		std::cout << "round trip of " << ts2 << " became " << parsed_dte2_str
		          << '\n';
		ensure( ts2 == parsed_dte2_str );
#if DAW_HAS_CLANG_VER_GTE( 9, 0 )
		static_assert( daw::json::datetime::short_day_of_week(
		                 std::chrono::time_point<std::chrono::system_clock,
		                                         std::chrono::milliseconds>( ) ) ==
		               "Thu" );
#else
	ensure( daw::json::datetime::short_day_of_week(
	          std::chrono::time_point<std::chrono::system_clock,
	                                  std::chrono::milliseconds>( ) ) == "Thu" );
#endif
		constexpr auto dbl_007 = -1.7e100;
		std::cout << "Large negative double " << dbl_007 << '\n';
		auto dbl_007_str = to_json<json_base::json_number<
		  double, options::number_opt( options::FPOutputFormat::Decimal )>>(
		  dbl_007 );
		ensure( dbl_007_str ==
		        "-17000000000000000000000000000000000000000000000000000000000000000"
		        "000000000000000000000000000000000000" );
		std::cout << dbl_007_str << '\n';
		auto dbl_007_str1 = to_json<json_base::json_number<
		  double, options::number_opt( options::FPOutputFormat::Scientific )>>(
		  dbl_007 );
		std::cout << dbl_007_str1 << '\n';
		ensure( dbl_007_str1 == "-1.7e100" );

		auto dbl_007_str2 = to_json<json_base::json_number<
		  double, options::number_opt( options::FPOutputFormat::Auto )>>( dbl_007 );
		ensure( dbl_007_str2 == "-1.7e100" );
		std::cout << dbl_007_str2 << '\n';

#if defined( __cpp_lib_char8_t )
#if __cpp_lib_char8_t >= 201907L
		static_assert(
		  daw::json::concepts::is_writable_output_type_v<std::u8string> );
		std::cout
		  << "u8string\n"
		  << reinterpret_cast<char const *>(
		       to_json<json_base::json_number<
		         double, options::number_opt( options::FPOutputFormat::Decimal )>>(
		         outfmt_dbl, std::u8string{ } )
		         .c_str( ) )
		  << '\n';
#endif
#endif

		std::cout << "\n\nJSON Link Version: " << json_link_version( ) << '\n';
		std::cout << "done\n\n";
	}
#ifdef DAW_USE_EXCEPTIONS
	catch( daw::json::json_exception const &jex ) {
		std::cerr << "Exception thrown by parser: " << jex.reason( ) << '\n';
		exit( 1 );
	} catch( std::exception const &ex ) {
		std::cerr << "Unknown exception thrown during testing: " << ex.what( )
		          << '\n';
		exit( 1 );
	} catch( ... ) {
		std::cerr << "Unknown exception thrown during testing\n";
		throw;
	}
#endif
}