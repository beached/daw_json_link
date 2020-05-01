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

bool test_bool_true( ) {
	constexpr std::string_view sv = R"({"a":true})";
	constexpr std::string_view sv2 = sv.substr( 5 );
	auto rng = daw::json::json_details::IteratorRange(
	  sv2.data( ), sv2.data( ) + sv2.size( ) );
	using namespace daw::json::json_details;
	auto v = skip_literal( rng );
	return std::string_view( v.data( ), v.size( ) ) == "true";
}

bool test_number( ) {
	constexpr std::string_view sv = "12345,";
	auto rng = daw::json::json_details::IteratorRange(
	  sv.data( ), sv.data( ) + sv.size( ) );
	using namespace daw::json::json_details;
	auto v = skip_literal( rng );
	return std::string_view( v.data( ), v.size( ) ) == "12345";
}

bool test_number_space( ) {
	constexpr std::string_view sv = "12345         ,";
	auto rng = daw::json::json_details::IteratorRange(
		sv.data( ), sv.data( ) + sv.size( ) );
	using namespace daw::json::json_details;
	auto v = skip_literal( rng );
	return std::string_view( v.data( ), v.size( ) ) == "12345";
}


#define do_test( ... ) daw::expecting_message( __VA_ARGS__, "" #__VA_ARGS__ )

int main( int, char ** ) try {
//	do_test( test_bool_true( ) );
//	do_test( test_number( ) );
	do_test( test_number_space( ) );
} catch( daw::json::json_exception const &jex ) {
	std::cerr << "Exception thrown by parser: " << jex.reason( ) << std::endl;
	exit( 1 );
}
