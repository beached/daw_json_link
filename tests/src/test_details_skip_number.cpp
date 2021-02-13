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

bool test_number_in_class( ) {
	DAW_CONSTEXPR std::string_view sv = R"({"a":1234})";
	DAW_CONSTEXPR std::string_view sv2 = sv.substr( 5 );
	auto rng =
	  daw::json::DefaultParsePolicy( sv2.data( ), sv2.data( ) + sv2.size( ) );
	using namespace daw::json::json_details;
	auto v = skip_number( rng );
	return std::string_view( v.first, v.size( ) ) == "1234";
}

bool test_number( ) {
	DAW_CONSTEXPR std::string_view sv = "12345,";
	auto rng =
	  daw::json::DefaultParsePolicy( sv.data( ), sv.data( ) + sv.size( ) );
	using namespace daw::json::json_details;
	auto v = skip_number( rng );
	return std::string_view( v.first, v.size( ) ) == "12345";
}

bool test_number_space( ) {
	DAW_CONSTEXPR std::string_view sv = "12345         ,";
	auto rng =
	  daw::json::DefaultParsePolicy( sv.data( ), sv.data( ) + sv.size( ) );
	using namespace daw::json::json_details;
	auto v = skip_number( rng );
	return std::string_view( v.first, v.size( ) ) == "12345";
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
	do_test( test_number_in_class( ) );
	do_test( test_number( ) );
	do_test( test_number_space( ) );
}
catch( daw::json::json_exception const &jex ) {
	std::cerr << "Exception thrown by parser: " << jex.reason( ) << std::endl;
	exit( 1 );
}
