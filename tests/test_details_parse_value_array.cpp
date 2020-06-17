// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/daw_json_link
//

#include <daw/json/daw_json_link.h>
#include <daw/json/impl/daw_iterator_range.h>
#include <daw/json/impl/daw_json_parse_common.h>

#include <daw/daw_benchmark.h>

#include <iostream>
#include <optional>
#include <string_view>

bool empty_array_empty_json_array( ) {
	using namespace daw::json;
	using namespace daw::json::json_details;

	constexpr std::string_view sv = "[]";
	auto rng = IteratorRange( sv.data( ), sv.data( ) + sv.size( ) );
	auto v = parse_value<json_array<no_name, int>>(
	  ParseTag<JsonParseTypes::Array>{}, rng );
	return v.empty( );
}

bool int_array_json_string_array_fail( ) {
	using namespace daw::json;
	using namespace daw::json::json_details;

	std::string_view sv = R"([ "this is strange" ])";
	daw::do_not_optimize( sv );
	auto rng = IteratorRange( sv.data( ), sv.data( ) + sv.size( ) );
	auto v = parse_value<json_array<no_name, int>>(
	  ParseTag<JsonParseTypes::Array>{}, rng );
	daw::do_not_optimize( v );
	return true;
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
	do_test( empty_array_empty_json_array( ) );
	do_fail_test( int_array_json_string_array_fail( ) );
} catch( daw::json::json_exception const &jex ) {
	std::cerr << "Exception thrown by parser: " << jex.reason( ) << std::endl;
	exit( 1 );
}
