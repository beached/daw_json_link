// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/daw_json_link
//

#include "defines.h"

#include <daw/json/impl/daw_json_parse_common.h>

#include <daw/daw_benchmark.h>

#include <iostream>
#include <string_view>

bool test_empty( ) {
	DAW_CONSTEXPR std::string_view sv = "";
	auto rng =
	  daw::json::BasicParsePolicy( std::data( sv ), daw::data_end( sv ) );
	using namespace daw::json::json_details;
	auto v = skip_string( rng );
	return v.empty( );
}

bool test_quoted_number( ) {
	DAW_CONSTEXPR std::string_view sv = R"( "1234")";
	auto rng =
	  daw::json::BasicParsePolicy( std::data( sv ) + 1, daw::data_end( sv ) );
	using namespace daw::json::json_details;
	auto v = skip_string( rng );
	return std::string_view( v.first, v.size( ) ) == "1234";
}

bool test_empty_quoted( ) {
	DAW_CONSTEXPR std::string_view sv = R"( "")";
	DAW_CONSTEXPR std::string_view sv2 = sv.substr( 1 );
	auto rng =
	  daw::json::BasicParsePolicy( std::data( sv2 ), daw::data_end( sv2 ) );
	using namespace daw::json::json_details;
	auto v = skip_string( rng );
	return v.empty( );
}

bool test_embedded_quotes( ) {
	DAW_CONSTEXPR std::string_view sv = R"( "\"  \\ ")";
	DAW_CONSTEXPR std::string_view sv2 = sv.substr( 1 );
	auto rng =
	  daw::json::BasicParsePolicy( std::data( sv2 ), daw::data_end( sv2 ) );
	using namespace daw::json::json_details;
	auto v = skip_string( rng );
	DAW_CONSTEXPR std::string_view ans = R"(\"  \\ )";
	return std::string_view( v.first, v.size( ) ) == ans;
}

bool test_missing_quotes_001( ) {
	DAW_CONSTEXPR std::string_view sv = R"( ")";
	DAW_CONSTEXPR std::string_view sv2 = sv.substr( 1 );
	auto rng =
	  daw::json::BasicParsePolicy( std::data( sv2 ), daw::data_end( sv2 ) );
	using namespace daw::json::json_details;
	auto v = skip_string( rng );
	(void)v;
	return false;
}

bool test_missing_quotes_002( ) {
	DAW_CONSTEXPR std::string_view sv = R"( "\")";
	DAW_CONSTEXPR std::string_view sv2 = sv.substr( 1 );
	auto rng =
	  daw::json::BasicParsePolicy( std::data( sv2 ), daw::data_end( sv2 ) );
	using namespace daw::json::json_details;
	auto v = skip_string( rng );
	(void)v;
	return false;
}

bool test_missing_quotes_003( ) {
	DAW_CONSTEXPR std::string_view sv = R"( "\"]})";
	DAW_CONSTEXPR std::string_view sv2 = sv.substr( 1 );
	auto rng =
	  daw::json::BasicParsePolicy( std::data( sv2 ), daw::data_end( sv2 ) );
	using namespace daw::json::json_details;
	auto v = skip_string( rng );
	(void)v;
	return false;
}

template<daw::json::options::ExecModeTypes ExecMode>
bool test_escaped_quote_001( ) {
	std::string_view sv =
	  R"( "abcdefghijklmnop\"qrstuvwxyz"                                        )";
	std::string_view sv2 = sv.substr( 1 );
	using namespace daw::json;
	using namespace daw::json::json_details;
	auto rng = BasicParsePolicy<parse_options( ExecMode )>(
	  std::data( sv2 ), daw::data_end( sv2 ) );
	auto v = skip_string( rng );
	auto const sz = v.size( );
	return sz == 28;
}

template<daw::json::options::ExecModeTypes ExecMode>
bool test_escaped_quote_002( ) {
	DAW_CONSTEXPR std::string_view sv =
	  R"( "abcdefghijklmnop\"qrstuvwxy\\"                                        )";
	DAW_CONSTEXPR std::string_view sv2 = sv.substr( 1 );
	using namespace daw::json;
	using namespace daw::json::json_details;
	auto rng = BasicParsePolicy<parse_options( ExecMode )>(
	  std::data( sv2 ), daw::data_end( sv2 ) );
	auto v = skip_string( rng );
	return v.size( ) == 29;
}

template<daw::json::options::ExecModeTypes ExecMode>
bool test_escaped_quote_003( ) {
	DAW_CONSTEXPR std::string_view sv =
	  R"( "abcdefghijklmn\\\"qrstuvwxy\\"                                        )";
	DAW_CONSTEXPR std::string_view sv2 = sv.substr( 1 );
	using namespace daw::json;
	using namespace daw::json::json_details;
	auto rng = BasicParsePolicy<parse_options( ExecMode )>(
	  std::data( sv2 ), daw::data_end( sv2 ) );
	auto v = skip_string( rng );
	return v.size( ) == 29;
}

template<daw::json::options::ExecModeTypes ExecMode>
bool test_escaped_quote_004( ) {
	DAW_CONSTEXPR std::string_view sv =
	  R"( "<a href=\"http://twittbot.net/\" rel=\"nofollow\">twittbot.net</a>"                                 )";
	DAW_CONSTEXPR std::string_view sv2 = sv.substr( 1 );
	using namespace daw::json;
	using namespace daw::json::json_details;
	auto rng = BasicParsePolicy<parse_options( ExecMode )>(
	  std::data( sv2 ), daw::data_end( sv2 ) );
	auto v = skip_string( rng );
	return v.size( ) == 66;
}

#define do_test( ... )                                                 \
	try {                                                                \
		if( not( __VA_ARGS__ ) ) {                                         \
			std::cerr << "Error while testing: " #__VA_ARGS__ << '\n';       \
			exit( 1 );                                                       \
		}                                                                  \
	} catch( daw::json::json_exception const &jex ) {                    \
		std::cerr << "Unexpected exception thrown by parser in test '"     \
		          << "" #__VA_ARGS__                                       \
		          << "': " << to_formatted_string( jex, nullptr ) << '\n'; \
		exit( 1 );                                                         \
	}                                                                    \
	do {                                                                 \
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
#ifdef DAW_USE_EXCEPTIONS
  try
#endif
{
	do_test( test_empty( ) );
	do_test( test_quoted_number( ) );
	do_test( test_empty_quoted( ) );
	do_test( test_embedded_quotes( ) );
	do_test( test_escaped_quote_001<
	         daw::json::options::ExecModeTypes::compile_time>( ) );
	do_test( test_escaped_quote_002<
	         daw::json::options::ExecModeTypes::compile_time>( ) );
	do_test( test_escaped_quote_003<
	         daw::json::options::ExecModeTypes::compile_time>( ) );
	do_test( test_escaped_quote_004<
	         daw::json::options::ExecModeTypes::compile_time>( ) );
	do_test(
	  test_escaped_quote_001<daw::json::options::ExecModeTypes::runtime>( ) );
	do_test(
	  test_escaped_quote_002<daw::json::options::ExecModeTypes::runtime>( ) );
	do_test(
	  test_escaped_quote_003<daw::json::options::ExecModeTypes::runtime>( ) );
	do_test(
	  test_escaped_quote_004<daw::json::options::ExecModeTypes::runtime>( ) );
#if defined( DAW_ALLOW_SSE42 )
	do_test( test_escaped_quote_001<daw::json::options::ExecModeTypes::simd>( ) );
	do_test( test_escaped_quote_002<daw::json::options::ExecModeTypes::simd>( ) );
	do_test( test_escaped_quote_003<daw::json::options::ExecModeTypes::simd>( ) );
	do_test( test_escaped_quote_004<daw::json::options::ExecModeTypes::simd>( ) );
#endif
	do_fail_test( test_missing_quotes_001( ) );
	do_fail_test( test_missing_quotes_002( ) );
	do_fail_test( test_missing_quotes_003( ) );
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
