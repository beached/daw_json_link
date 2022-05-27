// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/daw_json_link
//

#include "defines.h"

#include <daw/json/daw_json_link.h>
#include <daw/json/impl/daw_json_parse_common.h>

#include <daw/daw_benchmark.h>

#include <iostream>
#include <optional>
#include <string_view>

bool test_null_literal_untrusted( ) {
	using namespace daw::json;
	using namespace daw::json::json_details;

	using my_number = json_number_null_no_name<std::optional<int>>;
	DAW_CONSTEXPR std::string_view sv = "null,";
	auto rng = BasicParsePolicy( sv.data( ), sv.data( ) + sv.size( ) );
	auto v = parse_value<my_number>( rng, ParseTag<JsonParseTypes::Null>{ } );
	return not v;
}

bool test_null_literal_known( ) {
	using namespace daw::json;
	using namespace daw::json::json_details;

	using my_number = json_number_null_no_name<std::optional<int>>;
	auto rng = BasicParsePolicy( );
	auto v =
	  parse_value<my_number, true>( rng, ParseTag<JsonParseTypes::Null>{ } );
	return not v;
}

bool test_null_number_untrusted( ) {
	using namespace daw::json;
	using namespace daw::json::json_details;

	using my_number = json_number_null_no_name<std::optional<int>>;
	DAW_CONSTEXPR std::string_view sv = "5,";
	auto rng = BasicParsePolicy( sv.data( ), sv.data( ) + sv.size( ) );
	auto v = parse_value<my_number>( rng, ParseTag<JsonParseTypes::Null>{ } );
	return v and *v == 5;
}

bool test_null_number_trusted( ) {
	using namespace daw::json;
	using namespace daw::json::json_details;

	using my_number = json_number_null_no_name<std::optional<int>>;
	DAW_CONSTEXPR std::string_view sv = "5,";
	auto rng = BasicParsePolicy( sv.data( ), sv.data( ) + sv.size( ) );
	auto v = parse_value<my_number>( rng, ParseTag<JsonParseTypes::Null>{ } );
	return v and *v == 5;
}

bool test_null_number_untrusted_known( ) {
	using namespace daw::json;
	using namespace daw::json::json_details;

	using my_number = json_number_null_no_name<std::optional<int>>;
	DAW_CONSTEXPR std::string_view sv = "5,";
	auto rng = BasicParsePolicy( sv.data( ), sv.data( ) + sv.size( ) - 1 );
	auto v =
	  parse_value<my_number, true>( rng, ParseTag<JsonParseTypes::Null>{ } );
	return v and *v == 5;
}

#ifdef DAW_USE_EXCEPTIONS
#define do_test( ... )                                                   \
	try {                                                                  \
		daw::expecting_message( __VA_ARGS__, "" #__VA_ARGS__ );              \
	} catch( daw::json::json_exception const &jex ) {                      \
		std::cerr << "Unexpected exception thrown by parser in test '"       \
		          << "" #__VA_ARGS__ << "': " << jex.reason( ) << std::endl; \
	}                                                                      \
	do {                                                                   \
	} while( false )
#else
#define do_test( ... )                                    \
	daw::expecting_message( __VA_ARGS__, "" #__VA_ARGS__ ); \
	do {                                                    \
	} while( false )
#endif
/*
#define do_fail_test( ... )                                   \
  do {                                                        \
    try {                                                     \
      daw::expecting_message( __VA_ARGS__, "" #__VA_ARGS__ ); \
    } catch( daw::json::json_exception const & ) { break; }   \
    std::cerr << "Expected exception, but none thrown in '"   \
              << "" #__VA_ARGS__ << "'\n";                    \
  } while( false )
*/

int main( )
#ifdef DAW_USE_EXCEPTIONS
  try
#endif
{
	do_test( test_null_literal_untrusted( ) );
	do_test( test_null_literal_known( ) );
	do_test( test_null_number_untrusted( ) );
	do_test( test_null_number_trusted( ) );
	do_test( test_null_number_untrusted_known( ) );
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