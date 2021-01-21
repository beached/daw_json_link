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

bool test_zero_untrusted( ) {
	using namespace daw::json;
	using namespace daw::json::json_details;

	using my_number = json_number<no_name>;
	DAW_CONSTEXPR std::string_view sv = "0,";
	auto rng = DefaultParsePolicy( sv.data( ), sv.data( ) + sv.size( ) );
	auto v = parse_value<my_number>( ParseTag<JsonParseTypes::Real>{ }, rng );
	return v == 0;
}

bool test_positive_zero_untrusted( ) {
	using namespace daw::json;
	using namespace daw::json::json_details;

	using my_number = json_number<no_name>;
	DAW_CONSTEXPR std::string_view sv = "+0,";
	auto rng = DefaultParsePolicy( sv.data( ), sv.data( ) + sv.size( ) );
	auto v = parse_value<my_number>( ParseTag<JsonParseTypes::Real>{ }, rng );
	return v == 0;
}

bool test_negative_zero_untrusted( ) {
	using namespace daw::json;
	using namespace daw::json::json_details;

	using my_number = json_number<no_name>;
	DAW_CONSTEXPR std::string_view sv = "-0,";
	auto rng = DefaultParsePolicy( sv.data( ), sv.data( ) + sv.size( ) );
	auto v = parse_value<my_number>( ParseTag<JsonParseTypes::Real>{ }, rng );
	return v == 0;
}

bool test_missing_untrusted( ) {
	using namespace daw::json;
	using namespace daw::json::json_details;

	using my_number = json_number<no_name>;
	DAW_CONSTEXPR std::string_view sv = " ,";
	auto rng = DefaultParsePolicy( sv.data( ), sv.data( ) + sv.size( ) );
	auto v = parse_value<my_number>( ParseTag<JsonParseTypes::Real>{ }, rng );
	daw::do_not_optimize( v );
	return false;
}

bool test_real_untrusted( ) {
	using namespace daw::json;
	using namespace daw::json::json_details;

	using my_number = json_number<no_name>;
	DAW_CONSTEXPR std::string_view sv = "1.23,";
	auto rng = DefaultParsePolicy( sv.data( ), sv.data( ) + sv.size( ) );
	auto v = parse_value<my_number>( ParseTag<JsonParseTypes::Real>{ }, rng );
	return v >= ( 1.23 - daw::numeric_limits<double>::epsilon( ) ) and
	       v <= ( 1.23 + daw::numeric_limits<double>::epsilon( ) );
}

bool test_bad_real_untrusted( ) {
	using namespace daw::json;
	using namespace daw::json::json_details;

	using my_number = json_number<no_name>;
	DAW_CONSTEXPR std::string_view sv = "1.0fsdf3,";
	auto rng = DefaultParsePolicy( sv.data( ), sv.data( ) + sv.size( ) );
	auto v = parse_value<my_number>( ParseTag<JsonParseTypes::Real>{ }, rng );
	Unused( v );
	return false;
}

bool test_bad_real_untrusted2( ) {
	using namespace daw::json;
	using namespace daw::json::json_details;

	using my_number = json_number<no_name, double, LiteralAsStringOpt::Always>;
	DAW_CONSTEXPR std::string_view sv = R"("1.0fsdf3",)";
	auto rng = DefaultParsePolicy( sv.data( ), sv.data( ) + sv.size( ) );
	auto v = parse_value<my_number>( ParseTag<JsonParseTypes::Real>{ }, rng );
	Unused( v );
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
	/*
	do_test( test_zero_untrusted( ) );
	do_fail_test( test_positive_zero_untrusted( ) );
	do_test( test_negative_zero_untrusted( ) );
	do_fail_test( test_missing_untrusted( ) );
	do_test( test_real_untrusted( ) );
	do_fail_test( test_bad_real_untrusted( ) );*/
	do_fail_test( test_bad_real_untrusted2( ) );
}
catch( daw::json::json_exception const &jex ) {
	std::cerr << "Exception thrown by parser: " << jex.reason( ) << std::endl;
	exit( 1 );
}
