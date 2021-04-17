// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/daw_json_link
//

#include "defines.h"

#include <daw/json/impl/daw_json_iterator_range.h>
#include <daw/json/impl/daw_json_parse_common.h>

#include <daw/daw_benchmark.h>

#include <iostream>
#include <string_view>

using namespace daw::json;
using namespace daw::json::json_details;

bool test_empty_quoted( ) {
	DAW_CONSTEXPR std::string_view sv = "{}";
	auto rng =
	  daw::json::DefaultParsePolicy( sv.data( ), sv.data( ) + sv.size( ) );
	auto v = rng.skip_class( );
	return std::string_view( v.first, v.size( ) ) == sv;
}

bool test_extra_slash( ) {
	DAW_CONSTEXPR std::string_view sv = "{\\}";
	auto rng =
	  daw::json::DefaultParsePolicy( sv.data( ), sv.data( ) + sv.size( ) );
	auto v = rng.skip_class( );
	daw::do_not_optimize( v );
	return false;
}

bool test_end_of_stream( ) {
	DAW_CONSTEXPR std::string_view sv = "{";
	auto rng =
	  daw::json::DefaultParsePolicy( sv.data( ), sv.data( ) + sv.size( ) );
	auto v = rng.skip_class( );
	daw::do_not_optimize( v );
	return false;
}

bool test_trailing_comma( ) {
	DAW_CONSTEXPR std::string_view sv = R"({ "a": 1, "b": 2, "c": 3,})";
	auto rng =
	  daw::json::DefaultParsePolicy( sv.data( ), sv.data( ) + sv.size( ) );
	auto v = rng.skip_class( );
	return std::string_view( v.first, v.size( ) ) == sv;
}

bool test_strings( ) {
	DAW_CONSTEXPR std::string_view sv = R"({ "a": "1", "b": "2", "c": "3"})";
	auto rng =
	  daw::json::DefaultParsePolicy( sv.data( ), sv.data( ) + sv.size( ) );
	auto v = rng.skip_class( );
	return std::string_view( v.first, v.size( ) ) == sv;
}

bool test_bad_strings_001( ) {
	DAW_CONSTEXPR std::string_view sv = R"({ "a": "1", "b": "2", "c": "3})";
	auto rng =
	  daw::json::DefaultParsePolicy( sv.data( ), sv.data( ) + sv.size( ) );
	auto v = rng.skip_class( );
	daw::do_not_optimize( v );
	return false;
}

bool test_bad_strings_002( ) {
	DAW_CONSTEXPR std::string_view sv = R"({ "a": "1", "b": "2", "c: "3"})";
	// DAW_CONSTEXPR std::string_view sv = "[\"1\",\"2\",\"3\",\"4\\\"]";
	auto rng =
	  daw::json::DefaultParsePolicy( sv.data( ), sv.data( ) + sv.size( ) );
	auto v = rng.skip_class( );
	daw::do_not_optimize( v );
	return false;
}

bool test_bad_strings_003( ) {
	DAW_CONSTEXPR std::string_view sv = R"({ "a": "1", "b": "2", "c": "3\"})";
	auto rng =
	  daw::json::DefaultParsePolicy( sv.data( ), sv.data( ) + sv.size( ) );
	auto v = rng.skip_class( );
	daw::do_not_optimize( v );
	return false;
}

bool test_classes_001( ) {
	DAW_CONSTEXPR std::string_view sv = R"({"a":{},"b":{},"c":{}})";
	auto rng =
	  daw::json::DefaultParsePolicy( sv.data( ), sv.data( ) + sv.size( ) );
	auto v = rng.skip_class( );
	return std::string_view( v.first, v.size( ) ) == sv;
}

bool test_classes_002( ) {
	DAW_CONSTEXPR std::string_view sv =
	  R"({"a":{"a":""},"b":{"a":""},"c":{"a":""},"d":{"a":"
	  """},"e":{"a":""},"f":{"a":"
	  """}})";
	auto rng =
	  daw::json::DefaultParsePolicy( sv.data( ), sv.data( ) + sv.size( ) );
	auto v = rng.skip_class( );
	return std::string_view( v.first, v.size( ) ) == sv;
}

bool test_embedded_class( ) {
	DAW_CONSTEXPR std::string_view sv =
	  R"({
	"a": {
		"a": {
			"a": {
				"a": {
					"a": {
						"a": {
							"a": {},
							"b": {}
						},
						"b": {}
					}
				}
			}
		}
	}
})";
	auto rng =
	  daw::json::DefaultParsePolicy( sv.data( ), sv.data( ) + sv.size( ) );
	auto v = rng.skip_class( );
	return std::string_view( v.first, v.size( ) ) == sv;
}

bool test_embedded_class_broken_001( ) {
	DAW_CONSTEXPR std::string_view sv =
	  "{\"a\":{\"a\":{\"a\":{\"a\":{\"a\":{\"a\":{\"a\":{ },\"b\":{ "
	  "}},\"b\":{}}}}}}";
	auto rng =
	  daw::json::DefaultParsePolicy( sv.data( ), sv.data( ) + sv.size( ) );
	auto v = rng.skip_class( );
	daw::do_not_optimize( v );
	return false;
}

bool test_class_close_mid_array_without_open( ) {
	DAW_CONSTEXPR std::string_view sv = "{ [ } ] }";
	auto rng =
	  daw::json::DefaultParsePolicy( sv.data( ), sv.data( ) + sv.size( ) );
	auto v = rng.skip_class( );
	daw::do_not_optimize( v );
	return false;
}

#define do_test( ... )                                                   \
	try {                                                                  \
		daw::expecting_message( __VA_ARGS__, "" #__VA_ARGS__ );              \
	} catch( daw::json::json_exception const &jex ) {                      \
		std::cerr << "Unexpected exception thrown by parser in test '"       \
		          << "" #__VA_ARGS__ << "': " << jex.reason( ) << std::endl; \
	}                                                                      \
	do {                                                                   \
	} while( false )

#define do_fail_test( ... )                                   \
	do {                                                        \
		try {                                                     \
			daw::expecting_message( __VA_ARGS__, "" #__VA_ARGS__ ); \
		} catch( daw::json::json_exception const & ) { break; }   \
		std::cerr << "Expected exception, but none thrown in '"   \
		          << "" #__VA_ARGS__ << "'\n";                    \
	} while( false )

int main( int, char ** )
#ifdef DAW_USE_JSON_EXCEPTIONS
  try
#endif
{
	/*do_test( test_empty_quoted( ) );
	do_fail_test( test_end_of_stream( ) );
	do_fail_test( test_extra_slash( ) );
	do_test( test_trailing_comma( ) );
	do_test( test_strings( ) );
	do_fail_test( test_bad_strings_001( ) );
	do_fail_test( test_bad_strings_002( ) );
	do_fail_test( test_bad_strings_003( ) );
	do_test( test_classes_001( ) );
	do_test( test_classes_002( ) );
	do_test( test_embedded_class( ) );
	do_fail_test( test_embedded_class_broken_001( ) );*/
	do_fail_test( test_class_close_mid_array_without_open( ) );
}
catch( json_exception const &jex ) {
	std::cerr << "Exception thrown by parser: " << jex.reason( ) << std::endl;
	exit( 1 );
}
