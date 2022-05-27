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

struct NoOp {
	DAW_CONSTEXPR std::string_view operator( )( std::string_view rng ) const {
		return rng;
	}
};

bool empty_array_empty_json_array( ) {
	using namespace daw::json;
	using namespace daw::json::json_details;

	DAW_CONSTEXPR std::string_view sv = R"({ "a": "20200130" })";
	auto rng = BasicParsePolicy( sv.data( ), sv.data( ) + sv.size( ) );
	rng.remove_prefix( 7 );
	auto v = parse_value<json_custom_no_name<std::string_view, NoOp>>(
	  rng, ParseTag<JsonParseTypes::Custom>{ } );
	return v.size( ) == 8;
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

int main( int, char ** )
#ifdef DAW_USE_EXCEPTIONS
  try
#endif
{
	do_test( empty_array_empty_json_array( ) );
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