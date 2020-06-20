// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/daw_json_link
//

#include <daw/json/impl/daw_iterator_range.h>
#include <daw/json/impl/daw_json_parse_common.h>

#include <daw/daw_benchmark.h>

#include <iostream>
#include <string_view>

using namespace daw::json;
using namespace daw::json::json_details;

bool test_empty_quoted( ) {
	constexpr std::string_view sv = "[]";
	auto rng =
	  daw::json::DefaultParsePolicy( sv.data( ), sv.data( ) + sv.size( ) );
	auto v = rng.skip_array( );
	return std::string_view( v.begin( ), v.size( ) ) == sv;
}

bool test_extra_slash( ) {
	constexpr std::string_view sv = "[\\]";
	auto rng =
	  daw::json::DefaultParsePolicy( sv.data( ), sv.data( ) + sv.size( ) );
	auto v = rng.skip_array( );
	daw::do_not_optimize( v );
	return false;
}

bool test_end_of_stream( ) {
	constexpr std::string_view sv = "[";
	auto rng =
	  daw::json::DefaultParsePolicy( sv.data( ), sv.data( ) + sv.size( ) );
	auto v = rng.skip_array( );
	daw::do_not_optimize( v );
	return false;
}

bool test_trailing_comma( ) {
	constexpr std::string_view sv = "[1,2,3,4,]";
	auto rng =
	  daw::json::DefaultParsePolicy( sv.data( ), sv.data( ) + sv.size( ) );
	auto v = rng.skip_array( );
	return std::string_view( v.begin( ), v.size( ) ) == sv;
}

bool test_strings( ) {
	constexpr std::string_view sv = "[\"1\",\"2\",\"3\",\"4\"]";
	auto rng =
	  daw::json::DefaultParsePolicy( sv.data( ), sv.data( ) + sv.size( ) );
	auto v = rng.skip_array( );
	return std::string_view( v.begin( ), v.size( ) ) == sv;
}

bool test_bad_strings_001( ) {
	constexpr std::string_view sv = "[\"1\",\"2\",\"3\",\"4]";
	auto rng =
	  daw::json::DefaultParsePolicy( sv.data( ), sv.data( ) + sv.size( ) );
	auto v = rng.skip_array( );
	daw::do_not_optimize( v );
	return false;
}

bool test_bad_strings_002( ) {
	constexpr std::string_view sv = "[\"1\",\"2\",\"3\",\"4\\\"]";
	auto rng =
	  daw::json::DefaultParsePolicy( sv.data( ), sv.data( ) + sv.size( ) );
	auto v = rng.skip_array( );
	daw::do_not_optimize( v );
	return false;
}

bool test_classes_001( ) {
	constexpr std::string_view sv = "[{},{},{},{},{},{}]";
	auto rng =
	  daw::json::DefaultParsePolicy( sv.data( ), sv.data( ) + sv.size( ) );
	auto v = rng.skip_array( );
	return std::string_view( v.begin( ), v.size( ) ) == sv;
}

bool test_classes_002( ) {
	constexpr std::string_view sv =
	  "[{\"a\":\"\"},{\"a\":\"\"},{\"a\":\"\"},{\"a\":\"\"},{\"a\":\"\"},{\"a\":"
	  "\"\"}]";
	auto rng =
	  daw::json::DefaultParsePolicy( sv.data( ), sv.data( ) + sv.size( ) );
	auto v = rng.skip_array( );
	return std::string_view( v.begin( ), v.size( ) ) == sv;
}

bool test_embedded_arrays( ) {
	constexpr std::string_view sv = "[[[[[[{ },{ }],[]]]]]]";
	auto rng =
	  daw::json::DefaultParsePolicy( sv.data( ), sv.data( ) + sv.size( ) );
	auto v = rng.skip_array( );
	return std::string_view( v.begin( ), v.size( ) ) == sv;
}

bool test_embedded_arrays_broken_001( ) {
	constexpr std::string_view sv = "[[[[[[[{ },{ }],[]]]]]]";
	auto rng =
	  daw::json::DefaultParsePolicy( sv.data( ), sv.data( ) + sv.size( ) );
	auto v = rng.skip_array( );
	daw::do_not_optimize( v );
	return false;
}

#define do_test( ... )                                                         \
	try {                                                                        \
		daw::expecting_message( __VA_ARGS__, "" #__VA_ARGS__ );                    \
	} catch( daw::json::json_exception const &jex ) {                            \
		std::cerr << "Unexpected exception thrown by parser in test '"             \
		          << "" #__VA_ARGS__ << "': " << jex.reason( ) << std::endl;       \
	}                                                                            \
	do {                                                                         \
	} while( false )

#define do_fail_test( ... )                                                    \
	do {                                                                         \
		try {                                                                      \
			daw::expecting_message( __VA_ARGS__, "" #__VA_ARGS__ );                  \
		} catch( daw::json::json_exception const & ) { break; }                    \
		std::cerr << "Expected exception, but none thrown in '"                    \
		          << "" #__VA_ARGS__ << "'\n";                                     \
	} while( false )

int main( int, char ** ) try {
	do_test( test_empty_quoted( ) );
	do_fail_test( test_end_of_stream( ) );
	do_fail_test( test_extra_slash( ) );
	do_test( test_trailing_comma( ) );
	do_test( test_strings( ) );
	do_fail_test( test_bad_strings_001( ) );
	do_fail_test( test_bad_strings_002( ) );
	do_test( test_classes_001( ) );
	do_test( test_classes_002( ) );
	do_test( test_embedded_arrays( ) );
	do_fail_test( test_embedded_arrays_broken_001( ) );
} catch( json_exception const &jex ) {
	std::cerr << "Exception thrown by parser: " << jex.reason( ) << std::endl;
	exit( 1 );
}
