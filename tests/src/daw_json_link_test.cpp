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

#include <cassert>
#include <chrono>
#include <cstdint>
#include <deque>
#include <iostream>
#include <list>
#include <optional>
#include <random>
#include <string_view>
#include <vector>

/***
Temporarily disable the constexpr tests in MSVC when C++20
*/
#if defined( _MSC_VER ) and not defined( __clang__ ) and \
  __cplusplus > 201703L
#define DAW_JSON_NO_CONST_EXPR
#endif

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

template<typename Real, bool Trusted = false>
DAW_CONSTEXPR Real parse_real( std::string_view str ) {
	auto rng = daw::json::NoCommentSkippingPolicyChecked(
	  str.data( ), str.data( ) + str.size( ) );
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
	test_assert( not sv.empty( ), "Unexpected empty string_view" );
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
		  json_ordered_member_list<int,
		                           json_number_null_no_name<std::optional<int>>>;

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
		auto rng = BasicParsePolicy<parse_options( Precise ? IEEE754Precise::yes
		                                                   : IEEE754Precise::no )>(
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
		std::cout << std::dec << "unsigned diff: " << diff << '\n';
		std::terminate( );
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
			auto rng = daw::json::NoCommentSkippingPolicyChecked(
			  num.data( ), num.data( ) + num.size( ) );
			rng = daw::json::json_details::skip_number( rng );
			using json_member = daw::json::json_details::json_deduced_type<double>;
			return daw::json::json_details::parse_value<json_member, KnownBounds>(
			  rng, daw::json::ParseTag<json_member::expected_type>{ } );
		} else {
			return daw::json::from_json<
			  double, daw::json::NoCommentSkippingPolicyChecked, KnownBounds>( num );
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
				auto rng = daw::json::NoCommentSkippingPolicyChecked(
				  num.data( ), num.data( ) + num.size( ) );
				rng = daw::json::json_details::skip_number( rng );
				using json_member = daw::json::json_details::json_deduced_type<double>;
				return daw::json::json_details::parse_value<json_member, KnownBounds>(
				  rng, daw::json::ParseTag<json_member::expected_type>{ } );
			} else {
				return daw::json::from_json<
				  double, daw::json::NoCommentSkippingPolicyChecked, KnownBounds>(
				  num );
			}
		}( );
		std::cout.precision( std::numeric_limits<double>::max_digits10 );
		std::cout << "orig: " << num << '\n';
		std::cout << "daw_json_link: " << lib_parse_dbl << '\n'
		          << "strtod: " << strod_parse_dbl << '\n';
		std::cout << "diff: " << ( lib_parse_dbl - strod_parse_dbl ) << '\n';

		std::cout.precision( old_precision );
		std::cout << std::dec << "unsigned diff: " << diff << '\n';
		std::terminate( );
	}
#else
	(void)orig;
#endif
	return diff;
}

template<bool KnownBounds = false, bool Precise = false,
         int NUM_VALS = 1'000'000>
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
		int x3 = std::uniform_int_distribution<>( -308, +308 )( rng );
		char buffer[128];
		std::sprintf( buffer, "%llu.%llue%d", x1, x2, x3 );

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

bool test_optional_array( ) {
	std::string_view const json_data = "[null,5]";
	using namespace daw::json;
	auto result = from_json_array<std::optional<int>>( json_data );
	daw_json_assert( result.size( ) == 2 and not result[0] and result[1] == 5,
	                 ErrorReason::Unknown );
	std::string str{ };
	to_json_array<json_number_null_no_name<std::optional<int>>>(
	  result, std::back_inserter( str ) );
	auto result2 =
	  from_json_array<json_number_null_no_name<std::optional<int>>>( str );
	return result == result2;
}

bool test_key_value( ) {
	std::string_view const json_data = R"({"a":0,"b":1})";
	using namespace daw::json;
	auto const result = from_json<std::map<std::string, int>>( json_data );
	daw_json_assert( result.size( ) == 2 and result.at( "a" ) == 0 and
	                   result.at( "b" ) == 1,
	                 ErrorReason::Unknown );
	std::string str{ };
	to_json( result, std::back_inserter( str ) );
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
		using type = json_ordered_member_list<>;

		static constexpr std::tuple<> to_json_data( empty_ordered ) {
			return { };
		}
	};
} // namespace daw::json

int main( int, char ** )
#ifdef DAW_USE_EXCEPTIONS
  try
#endif
{
	using namespace std::string_literals;
	std::cout << ( sizeof( std::size_t ) * 8U ) << "bit architecture\n";
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

#if defined( __GNUC__ ) and __GNUC__ <= 9
#define CX
#elif defined( DAW_JSON_NO_CONST_EXPR )
#define CX
#else
#define CX DAW_CONSTEXPR
#endif

#if not defined( DAW_JSON_NO_INT128 ) and defined( DAW_HAS_INT128 )
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
	std::cout << daw::json::to_json_array( a ) << '\n';

	using namespace daw::json;
	using num_t =
	  json_number_no_name<double, number_opt( LiteralAsStringOpt::Always )>;
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

	test_dblparse2( "5792711765526609591.9963073925412025509e-82",
	                5792711765526609591.9963073925412025509e-82 );
	test_dblparse( "4891559871276714924261e222", true );
	test_dblparse(
	  "11111111111111111111111111111111111111111111111111111111111111111111111111"
	  "11111111111111111111111111111111111111111111111111111111111111111111111111"
	  "11111111111111111111111111111111111111111111111111111111111111111111111111"
	  "11111111111111111111111111111111111111111111111111111111111111111111111111"
	  "11111111111111111111111111111111111111111111111111111111111111111111111111"
	  "111111111111111111111111111111.0e-100",
	  true );
	test_dblparse(
	  "11111111111111111111111111111111111111111111111111111111111111111111111111"
	  "11111111111111111111111111111111111111111111111111111111111111111111111111"
	  "11111111111111111111111111111111111111111111111111111111111111111111111111"
	  "11111111111111111111111111111111111111111111111111111111111111111111111111"
	  "11111111111111111111111111111111111111111111111111111111111111111111111111"
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
	std::cout << "result: " << from_json<long double>( "1e-10000" ) << '\n';
	test_dblparse( "1e-214748364", true );
	test_dblparse( "0.89", true );
	test_dblparse(
	  "2."
	  "22507385850720113605740979670913197593481954635164564802342610972482222202"
	  "10769455165295239081350879141491589130396211068700864386945946455276572074"
	  "07820621743379988141063267329253552286881372149012981122451451889849057222"
	  "30728525513315575501591439747639798341180199932396254828901710708185069063"
	  "06666559949382757725720157630626906633326475653000092458883164330377797918"
	  "69612049497390377829704905051080609940730262937128958950003583799967207254"
	  "30436028407889577179615094551674824347103070260914462157228988025818254518"
	  "03257070188608721131280795122334262883686223215037756666225039825343359745"
	  "68884423900265498198385487948292206894721689831099698365846814022854243330"
	  "66033985088644580400103493397042756718644338377048603786162277173854562306"
	  "5874679014086723327636718751234567890123456789012345678901e-308",
	  true );
	test_dblparse( "0.9868011474609375", true );
	std::cout.precision( std::numeric_limits<double>::max_digits10 );
	std::cout << "result: " << from_json<long double>( "0.9868011474609375" )
	          << '\n';
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
		auto const d0 =
		  from_json<long double,
		            BasicParsePolicy<parse_options( ExecModeTypes::runtime )>>(
		    two63e100 );
		std::cout << d0 << '\n';
		std::cout << "using strtold\n";
		char *end = nullptr;
		long double const d1 = strtold( two63e100.data( ), &end );
		std::cout << d1 << '\n';
		double d2 = 0.89;
		std::cout << to_json( d2 ) << '\n';
	}

	test_optional_array( );
	test_key_value( );
	test_vector_of_bool( );
	static_assert( from_json<bool>( "true" ) );
	static_assert( not from_json<bool>( "false" ) );
	static_assert( not *from_json<std::optional<bool>>( "false" ) );
	static_assert( not from_json<std::optional<bool>>( "null" ) );
	static_assert( from_json<signed char>( "-1" ) == -1 );
	static_assert( from_json<short>( "-1" ) == -1 );
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
	assert( ( from_json<json_array_no_name<char, std::string>>( "[97,98,99]"s ) ==
	          "abc" ) );
	static_assert( from_json<std::array<int, 4>>( "[1,2,3]"sv )[1] == 2 );

	auto const test_bad_float = []( ) -> bool {
		try {
			(void)from_json<double>( "0e "sv );
		} catch( daw::json::json_exception const & ) { return true; }
		return false;
	};
	daw_json_assert( test_bad_float( ), ErrorReason::Unknown );

	auto const test_empty_map = []( ) -> bool {
		try {
			auto m = from_json<std::map<std::string, std::string>>( "{}"sv );
			if( not m.empty( ) ) {
				return false;
			}
			auto const s = to_json( m );

			return s == "{}";
		} catch( daw::json::json_exception const &jex ) {
			std::cerr << "Exception thrown by parser: " << jex.reason( ) << std::endl;
			throw;
		}
	};
	daw_json_assert( test_empty_map( ), ErrorReason::Unknown );

	auto const test_leading_zero = []( auto i ) {
		using test_t = daw::remove_cvref_t<decltype( i )>;
		try {
			auto l0 = from_json<test_t>( "01.0"sv );
			(void)l0;
		} catch( daw::json::json_exception const & ) { return true; }
		return false;
	};
	daw_json_assert( test_leading_zero( 0.0 ), ErrorReason::Unknown );
	daw_json_assert( test_leading_zero( 0 ), ErrorReason::Unknown );
	daw_json_assert( test_leading_zero( 0U ), ErrorReason::Unknown );

	static_assert(
	  from_json<json_key_value_no_name<
	    std::array<std::pair<std::string_view, int>, 2>, int, std::string_view>>(
	    R"({"a":0,"b":1})" )[1]
	    .second == 1 );

	auto v = from_json<tuple_json_mapping<>>( std::string_view( "{}" ) );
	auto vstr = to_json( v );
	(void)vstr;
	auto v1 = from_json<empty_ordered>( std::string_view( "[]" ) );
	auto v1str = to_json( v1 );
	(void)v1str;
	std::cout << "done";
}
#ifdef DAW_USE_EXCEPTIONS
catch( daw::json::json_exception const &jex ) {
	std::cerr << "Exception thrown by parser: " << jex.reason( ) << std::endl;
	exit( 1 );
}
#endif
