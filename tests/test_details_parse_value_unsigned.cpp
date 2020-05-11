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

#include <daw/json/daw_json_link.h>
#include <daw/json/impl/daw_iterator_range.h>
#include <daw/json/impl/daw_json_parse_common.h>

#include <daw/daw_benchmark.h>

#include <iostream>
#include <optional>
#include <string_view>

bool test_zero_untrusted( ) {
	using namespace daw::json;
	using namespace daw::json::json_details;

	using my_number = json_number<no_name, unsigned>;
	constexpr std::string_view sv = "0,";
	auto rng = IteratorRange( sv.data( ), sv.data( ) + sv.size( ) );
	auto v = parse_value<my_number>( ParseTag<JsonParseTypes::Unsigned>{}, rng );
	return not v;
}

bool test_missing_untrusted( ) {
	using namespace daw::json;
	using namespace daw::json::json_details;

	using my_number = json_number<no_name, unsigned>;
	constexpr std::string_view sv = " ,";
	auto rng = IteratorRange( sv.data( ), sv.data( ) + sv.size( ) );
	auto v = parse_value<my_number>( ParseTag<JsonParseTypes::Unsigned>{}, rng );
	daw::do_not_optimize( v );
	return false;
}

bool test_negative_untrusted( ) {
	using namespace daw::json;
	using namespace daw::json::json_details;

	using my_number = json_number<no_name, unsigned>;
	constexpr std::string_view sv = "-1,";
	auto rng = IteratorRange( sv.data( ), sv.data( ) + sv.size( ) );
	auto v = parse_value<my_number>( ParseTag<JsonParseTypes::Unsigned>{}, rng );
	daw::do_not_optimize( v );
	return false;
}

bool test_real_untrusted( ) {
	using namespace daw::json;
	using namespace daw::json::json_details;

	using my_number = json_number<no_name, unsigned>;
	constexpr std::string_view sv = "1.23,";
	auto rng = IteratorRange( sv.data( ), sv.data( ) + sv.size( ) );
	auto v = parse_value<my_number>( ParseTag<JsonParseTypes::Unsigned>{}, rng );
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
	do_test( test_zero_untrusted( ) );
	do_fail_test( test_missing_untrusted( ) );
	do_fail_test( test_negative_untrusted( ) );
	do_fail_test( test_real_untrusted( ) );
} catch( daw::json::json_exception const &jex ) {
	std::cerr << "Exception thrown by parser: " << jex.reason( ) << std::endl;
	exit( 1 );
}
