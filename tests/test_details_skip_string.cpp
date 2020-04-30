// The MIT License (MIT)
//
// Copyright (c) Darrell Wright
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files( the "Software" ), to
// deal in the Software without restriction, including without limitation the
// rights to use, copy, modify, merge, publish, distribute, sublicense, and / or
// sell copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#include <daw/json/impl/daw_iterator_range.h>
#include <daw/json/impl/daw_json_parse_common.h>

#include <daw/daw_benchmark.h>

#include <iostream>
#include <string_view>

bool test_empty( ) {
	constexpr std::string_view sv = "  ";
	constexpr std::string_view sv2 = sv.substr( 2 );
	auto rng = daw::json::json_details::IteratorRange(
		sv2.data( ), sv2.data( ) + sv2.size( ) );
	using namespace daw::json::json_details;
	auto v = skip_string( rng );
	return v.empty( );
}

bool test_empty_quoted( ) {
	constexpr std::string_view sv = R"( "")";
	constexpr std::string_view sv2 = sv.substr( 2 );
	auto rng = daw::json::json_details::IteratorRange(
	  sv2.data( ), sv2.data( ) + sv2.size( ) );
	using namespace daw::json::json_details;
	auto v = skip_string( rng );
	return v.empty( );
}

bool test_embeded_quotes( ) {
	constexpr std::string_view sv =  R"( "\"  \\ ")";
	constexpr std::string_view sv2 = sv.substr( 2 );
	auto rng = daw::json::json_details::IteratorRange(
	  sv2.data( ), sv2.data( ) + sv2.size( ) );
	using namespace daw::json::json_details;
	auto v = skip_string( rng );
	constexpr std::string_view ans = R"(\"  \\ )";
	return std::string_view( v.data( ), v.size( ) ) == ans;
}

bool test_missing_quotes_001( ) {
	constexpr std::string_view sv = R"( ")";
	constexpr std::string_view sv2 = sv.substr( 1 );
	auto rng = daw::json::json_details::IteratorRange(
	  sv2.data( ), sv2.data( ) + sv2.size( ) );
	using namespace daw::json::json_details;
	try {
		auto v = skip_string( rng );
		daw::do_not_optimize( v );
	} catch( daw::json::json_exception const & ) { return true; }
	return false;
}

bool test_missing_quotes_002( ) {
	constexpr std::string_view sv = R"( "\")";
	constexpr std::string_view sv2 = sv.substr( 2 );
	auto rng = daw::json::json_details::IteratorRange(
	  sv2.data( ), sv2.data( ) + sv2.size( ) );
	using namespace daw::json::json_details;
	try {
		auto v = skip_string( rng );
		daw::do_not_optimize( v );
	} catch( daw::json::json_exception const & ) { return true; }
	return false;
}

bool test_missing_quotes_003( ) {
	constexpr std::string_view sv = R"( "\"]})";
	constexpr std::string_view sv2 = sv.substr( 2 );
	auto rng = daw::json::json_details::IteratorRange(
	  sv2.data( ), sv2.data( ) + sv2.size( ) );
	using namespace daw::json::json_details;
	try {
		auto v = skip_string( rng );
		daw::do_not_optimize( v );
	} catch( daw::json::json_exception const & ) { return true; }
	return false;
}

#define do_test( ... ) daw::expecting_message( __VA_ARGS__, "" #__VA_ARGS__ )

int main( int, char ** ) try {
	do_test( test_empty( ) );
	do_test( test_empty_quoted( ) );
	do_test( test_embeded_quotes( ) );
	do_test( test_missing_quotes_001( ) );
	do_test( test_missing_quotes_002( ) );
	do_test( test_missing_quotes_003( ) );
} catch( daw::json::json_exception const &jex ) {
	std::cerr << "Exception thrown by parser: " << jex.reason( ) << std::endl;
	exit( 1 );
}
