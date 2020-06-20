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

bool test_empty( ) {
	constexpr std::string_view sv = "  ";
	constexpr std::string_view sv2 = sv.substr( 2 );
	auto rng =
	  daw::json::DefaultParsePolicy( sv2.data( ), sv2.data( ) + sv2.size( ) );
	using namespace daw::json::json_details;
	auto v = skip_string( rng );
	return v.empty( );
}

bool test_quoted_number( ) {
	constexpr std::string_view sv = R"( "1234")";
	auto rng =
	  daw::json::DefaultParsePolicy( sv.data( ) + 1, sv.data( ) + sv.size( ) );
	using namespace daw::json::json_details;
	auto v = skip_string( rng );
	return std::string_view( v.data( ), v.size( ) ) == "1234";
}

bool test_empty_quoted( ) {
	constexpr std::string_view sv = R"( "")";
	constexpr std::string_view sv2 = sv.substr( 2 );
	auto rng =
	  daw::json::DefaultParsePolicy( sv2.data( ), sv2.data( ) + sv2.size( ) );
	using namespace daw::json::json_details;
	auto v = skip_string( rng );
	return v.empty( );
}

bool test_embeded_quotes( ) {
	constexpr std::string_view sv = R"( "\"  \\ ")";
	constexpr std::string_view sv2 = sv.substr( 2 );
	auto rng =
	  daw::json::DefaultParsePolicy( sv2.data( ), sv2.data( ) + sv2.size( ) );
	using namespace daw::json::json_details;
	auto v = skip_string( rng );
	constexpr std::string_view ans = R"(\"  \\ )";
	return std::string_view( v.data( ), v.size( ) ) == ans;
}

bool test_missing_quotes_001( ) {
	constexpr std::string_view sv = R"( ")";
	constexpr std::string_view sv2 = sv.substr( 1 );
	auto rng =
	  daw::json::DefaultParsePolicy( sv2.data( ), sv2.data( ) + sv2.size( ) );
	using namespace daw::json::json_details;
	auto v = skip_string( rng );
	daw::do_not_optimize( v );
	return false;
}

bool test_missing_quotes_002( ) {
	constexpr std::string_view sv = R"( "\")";
	constexpr std::string_view sv2 = sv.substr( 2 );
	auto rng =
	  daw::json::DefaultParsePolicy( sv2.data( ), sv2.data( ) + sv2.size( ) );
	using namespace daw::json::json_details;
	auto v = skip_string( rng );
	daw::do_not_optimize( v );
	return false;
}

bool test_missing_quotes_003( ) {
	constexpr std::string_view sv = R"( "\"]})";
	constexpr std::string_view sv2 = sv.substr( 2 );
	auto rng =
	  daw::json::DefaultParsePolicy( sv2.data( ), sv2.data( ) + sv2.size( ) );
	using namespace daw::json::json_details;
	auto v = skip_string( rng );
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
	do_test( test_empty( ) );
	do_test( test_quoted_number( ) );
	do_test( test_empty_quoted( ) );
	do_test( test_embeded_quotes( ) );
	do_fail_test( test_missing_quotes_001( ) );
	do_fail_test( test_missing_quotes_002( ) );
	do_fail_test( test_missing_quotes_003( ) );
} catch( daw::json::json_exception const &jex ) {
	std::cerr << "Exception thrown by parser: " << jex.reason( ) << std::endl;
	exit( 1 );
}
