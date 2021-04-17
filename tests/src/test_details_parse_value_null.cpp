// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/daw_json_link
//

#include "defines.h"

#include <daw/json/daw_json_link.h>
#include <daw/json/impl/daw_json_iterator_range.h>
#include <daw/json/impl/daw_json_parse_common.h>

#include <daw/daw_benchmark.h>

#include <iostream>
#include <optional>
#include <string_view>

bool test_null_literal_untrusted( ) {
	using namespace daw::json;
	using namespace daw::json::json_details;

	using my_number = json_number_null<no_name, std::optional<int>>;
	DAW_CONSTEXPR std::string_view sv = "null,";
	auto rng = DefaultParsePolicy( sv.data( ), sv.data( ) + sv.size( ) );
	auto v = parse_value<my_number>( ParseTag<JsonParseTypes::Null>{ }, rng );
	return not v;
}

bool test_null_literal_known( ) {
	using namespace daw::json;
	using namespace daw::json::json_details;

	using my_number = json_number_null<no_name, std::optional<int>>;
	auto rng = DefaultParsePolicy( );
	auto v =
	  parse_value<my_number, true>( ParseTag<JsonParseTypes::Null>{ }, rng );
	return not v;
}

bool test_null_number_untrusted( ) {
	using namespace daw::json;
	using namespace daw::json::json_details;

	using my_number = json_number_null<no_name, std::optional<int>>;
	DAW_CONSTEXPR std::string_view sv = "5,";
	auto rng = DefaultParsePolicy( sv.data( ), sv.data( ) + sv.size( ) );
	auto v = parse_value<my_number>( ParseTag<JsonParseTypes::Null>{ }, rng );
	return v and *v == 5;
}

bool test_null_number_trusted( ) {
	using namespace daw::json;
	using namespace daw::json::json_details;

	using my_number = json_number_null<no_name, std::optional<int>>;
	DAW_CONSTEXPR std::string_view sv = "5,";
	auto rng = DefaultParsePolicy( sv.data( ), sv.data( ) + sv.size( ) );
	auto v = parse_value<my_number>( ParseTag<JsonParseTypes::Null>{ }, rng );
	return v and *v == 5;
}

bool test_null_number_untrusted_known( ) {
	using namespace daw::json;
	using namespace daw::json::json_details;

	using my_number = json_number_null<no_name, std::optional<int>>;
	DAW_CONSTEXPR std::string_view sv = "5,";
	auto rng = DefaultParsePolicy( sv.data( ), sv.data( ) + sv.size( ) - 1 );
	auto v =
	  parse_value<my_number, true>( ParseTag<JsonParseTypes::Null>{ }, rng );
	return v and *v == 5;
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

int main( )
#ifdef DAW_USE_JSON_EXCEPTIONS
  try
#endif
{
	do_test( test_null_literal_untrusted( ) );
	do_test( test_null_literal_known( ) );
	do_test( test_null_number_untrusted( ) );
	do_test( test_null_number_trusted( ) );
	do_test( test_null_number_untrusted_known( ) );
}
catch( daw::json::json_exception const &jex ) {
	std::cerr << "Exception thrown by parser: " << jex.reason( ) << std::endl;
	exit( 1 );
}
