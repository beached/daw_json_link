// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/daw_json_link
//
// A daw::string_view knows at runtime if it is zero terminated, and checked
// parsing uses the zero terminated policy when it is.  Ensure that the results
// match those of a view that is not zero terminated, and that invalid
// documents are still rejected

#include "defines.h"

// Ensure that we are always checking
#if defined( DAW_JSON_CHECK_DEBUG_ONLY )
#undef DAW_JSON_CHECK_DEBUG_ONLY
#endif

#include <daw/json/daw_json_iterator.h>
#include <daw/json/daw_json_link.h>

#include <daw/daw_ensure.h>
#include <daw/daw_string_view.h>

#include <string>
#include <vector>

struct coordinate_t {
	double x;
	double y;
	double z;

	bool operator==( coordinate_t const &rhs ) const {
		return x == rhs.x and y == rhs.y and z == rhs.z;
	}
};

struct coordinates_t {
	std::vector<coordinate_t> coordinates;
};

namespace daw::json {
	template<>
	struct json_data_contract<coordinate_t> {
		static constexpr char const x[] = "x";
		static constexpr char const y[] = "y";
		static constexpr char const z[] = "z";
		using type =
		  json_member_list<json_number<x>, json_number<y>, json_number<z>>;
	};

	template<>
	struct json_data_contract<coordinates_t> {
		static constexpr char const coordinates[] = "coordinates";
		using type = json_member_list<json_array<coordinates, coordinate_t>>;
	};
} // namespace daw::json

static std::string const coordinates_doc = R"json({
  "coordinates": [
    {
      "x": 0.5139832434218743,
      "y": 0.2845092823913241,
      "z": 0.9937292838382324,
      "name": "abcdef 1234",
      "opts": { "1": [ 1, true ] }
    },
    {
      "x": 1e-3,
      "y": -2.5E+2,
      "z": 12345678901234567890.5,
      "name": "ghi\"jkl 42",
      "opts": { "1": [ 1, true ] }
    },
    {"x":3,"y":4,"z":5}
  ],
  "info": "some info"
}
)json";

static std::vector<coordinate_t> const expected_coordinates = {
  { 0.5139832434218743, 0.2845092823913241, 0.9937292838382324 },
  { 1e-3, -2.5E+2, 12345678901234567890.5 },
  { 3, 4, 5 } };

/// A view of the same characters that does not know it is zero terminated
daw::string_view not_zero_terminated( std::string const &str ) {
	return daw::string_view( str.data( ), str.size( ) );
}

template<typename Range>
std::vector<coordinate_t> collect( Range &&rng ) {
	auto result = std::vector<coordinate_t>( );
	for( coordinate_t c : rng ) {
		result.push_back( c );
	}
	return result;
}

template<typename Range>
std::vector<double> collect_doubles( Range &&rng ) {
	auto result = std::vector<double>( );
	for( double d : rng ) {
		result.push_back( d );
	}
	return result;
}

void test_valid( daw::string_view sv ) {
	using namespace daw::json;

	auto const doc = from_json<coordinates_t>( sv );
	daw_ensure( doc.coordinates == expected_coordinates );

	auto const unchecked = from_json<coordinates_t>(
	  sv, options::parse_flags<options::CheckedParseMode::no> );
	daw_ensure( unchecked.coordinates == expected_coordinates );

	auto const at_path = from_json<coordinate_t>( sv, "coordinates[1]" );
	daw_ensure( at_path == expected_coordinates[1] );

	auto const arr =
	  from_json_array<coordinate_t>( sv, std::string_view( "coordinates" ) );
	daw_ensure( arr == expected_coordinates );

	daw_ensure( collect( json_array_range<coordinate_t>( sv, "coordinates" ) ) ==
	            expected_coordinates );
	daw_ensure( collect( json_array_range_once<coordinate_t>(
	              sv, "coordinates" ) ) == expected_coordinates );
}

void test_root_array( daw::string_view sv ) {
	using namespace daw::json;
	auto const expected = std::vector<double>{ 1, 2.5, 300 };

	daw_ensure( from_json<std::vector<double>>( sv ) == expected );
	daw_ensure( from_json_array<double>( sv ) == expected );
	daw_ensure( collect_doubles( json_array_range<double>( sv ) ) == expected );
	daw_ensure( collect_doubles( json_array_range_once<double>( sv ) ) ==
	            expected );
}

#if defined( DAW_USE_EXCEPTIONS )
template<typename Function>
bool throws_json_exception( Function &&func ) {
	try {
		func( );
	} catch( daw::json::json_exception const & ) { return true; }
	return false;
}

void test_invalid( daw::string_view sv ) {
	using namespace daw::json;
	daw_ensure( throws_json_exception(
	  [&] { (void)from_json<std::vector<double>>( sv ); } ) );
	daw_ensure(
	  throws_json_exception( [&] { (void)from_json_array<double>( sv ); } ) );
	daw_ensure( throws_json_exception(
	  [&] { (void)collect_doubles( json_array_range<double>( sv ) ); } ) );
	daw_ensure( throws_json_exception(
	  [&] { (void)collect_doubles( json_array_range_once<double>( sv ) ); } ) );
}
#endif

int main( )
#if defined( DAW_USE_EXCEPTIONS )
  try
#endif
{
	daw_ensure( daw::string_view( coordinates_doc ).is_zero_terminated( ) ==
	            daw::ZeroTerminated::Yes );
	daw_ensure( not_zero_terminated( coordinates_doc ).is_zero_terminated( ) ==
	            daw::ZeroTerminated::No );

	test_valid( daw::string_view( coordinates_doc ) );
	test_valid( not_zero_terminated( coordinates_doc ) );

	auto const root_array = std::string( "  [1, 2.5,\n 3e2]  " );
	test_root_array( daw::string_view( root_array ) );
	test_root_array( not_zero_terminated( root_array ) );

#if defined( DAW_USE_EXCEPTIONS )
	for( std::string const doc :
	     { "[1,2", "[1,2,", "[1, 2 ", "[1.5e", "[1.]", "[-]", "[\"abc",
	       "[tru", "[1 2]" } ) {
		daw_ensure( daw::string_view( doc ).is_zero_terminated( ) ==
		            daw::ZeroTerminated::Yes );
		test_invalid( daw::string_view( doc ) );
		test_invalid( not_zero_terminated( doc ) );
	}
#endif
}
#if defined( DAW_USE_EXCEPTIONS )
catch( daw::json::json_exception const &jex ) {
	std::cerr << "Exception thrown by parser: " << jex.reason( ) << '\n';
	exit( 1 );
}
#endif
