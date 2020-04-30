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

using namespace daw::json;
using namespace daw::json::json_details;

bool test_empty( ) {
	constexpr std::string_view sv = "[]";
	auto rng = daw::json::json_details::IteratorRange( sv.data( ),
	                                                   sv.data( ) + sv.size( ) );
	auto v = skip_array( rng );
	return std::string_view( v.begin( ), v.size( ) ) == sv;
}

bool test_extra_slash( ) {
	constexpr std::string_view sv = "[\\]";
	auto rng = daw::json::json_details::IteratorRange( sv.data( ),
	                                                   sv.data( ) + sv.size( ) );
	try {
		auto v = skip_array( rng );
		daw::do_not_optimize( v );
	} catch( json_exception const & ) { return true; }
	return false;
}

bool test_end_of_stream( ) {
	constexpr std::string_view sv = "[";
	auto rng = daw::json::json_details::IteratorRange( sv.data( ),
	                                                   sv.data( ) + sv.size( ) );
	try {
		auto v = skip_array( rng );
		daw::do_not_optimize( v );
	} catch( json_exception const & ) { return true; }
	return false;
}

bool test_trailing_comma( ) {
	constexpr std::string_view sv = "[1,2,3,4,]";
	auto rng = daw::json::json_details::IteratorRange( sv.data( ),
	                                                   sv.data( ) + sv.size( ) );
	auto v = skip_array( rng );
	return std::string_view( v.begin( ), v.size( ) ) == sv;
}

bool test_strings( ) {
	constexpr std::string_view sv = "[\"1\",\"2\",\"3\",\"4\"]";
	auto rng = daw::json::json_details::IteratorRange( sv.data( ),
	                                                   sv.data( ) + sv.size( ) );
	auto v = skip_array( rng );
	return std::string_view( v.begin( ), v.size( ) ) == sv;
}

bool test_bad_strings_001( ) {
	constexpr std::string_view sv = "[\"1\",\"2\",\"3\",\"4]";
	auto rng = daw::json::json_details::IteratorRange( sv.data( ),
	                                                   sv.data( ) + sv.size( ) );
	try {
		auto v = skip_array( rng );
		daw::do_not_optimize( v );
	} catch( json_exception const & ) { return true; }
	return false;
}

bool test_bad_strings_002( ) {
	constexpr std::string_view sv = "[\"1\",\"2\",\"3\",\"4\\\"]";
	auto rng = daw::json::json_details::IteratorRange( sv.data( ),
	                                                   sv.data( ) + sv.size( ) );
	try {
		auto v = skip_array( rng );
		daw::do_not_optimize( v );
	} catch( json_exception const & ) { return true; }
	return false;
}

bool test_classes_001( ) {
	constexpr std::string_view sv = "[{},{},{},{},{},{}]";
	auto rng = daw::json::json_details::IteratorRange( sv.data( ),
	                                                   sv.data( ) + sv.size( ) );
	auto v = skip_array( rng );
	return std::string_view( v.begin( ), v.size( ) ) == sv;
}

bool test_classes_002( ) {
	constexpr std::string_view sv =
	  "[{\"a\":\"\"},{\"a\":\"\"},{\"a\":\"\"},{\"a\":\"\"},{\"a\":\"\"},{\"a\":"
	  "\"\"}]";
	auto rng = daw::json::json_details::IteratorRange( sv.data( ),
	                                                   sv.data( ) + sv.size( ) );
	auto v = skip_array( rng );
	return std::string_view( v.begin( ), v.size( ) ) == sv;
}

bool test_embedded_arrays( ) {
	constexpr std::string_view sv = "[[[[[[{ },{ }],[]]]]]]";
	auto rng = daw::json::json_details::IteratorRange( sv.data( ),
	                                                   sv.data( ) + sv.size( ) );
	auto v = skip_array( rng );
	return std::string_view( v.begin( ), v.size( ) ) == sv;
}

bool test_embedded_arrays_broken_001( ) {
	constexpr std::string_view sv = "[[[[[[[{ },{ }],[]]]]]]";
	auto rng = daw::json::json_details::IteratorRange( sv.data( ),
	                                                   sv.data( ) + sv.size( ) );
	try {
		auto v = skip_array( rng );
		daw::do_not_optimize( v );
	} catch( json_exception const & ) { return true; }
	return false;
}

#define do_test( ... ) daw::expecting_message( __VA_ARGS__, "" #__VA_ARGS__ )

int main( int, char ** ) try {
	do_test( test_empty( ) );
	do_test( test_end_of_stream( ) );
	do_test( test_extra_slash( ) );
	do_test( test_trailing_comma( ) );
	do_test( test_strings( ) );
	do_test( test_bad_strings_001( ) );
	do_test( test_bad_strings_002( ) );
	do_test( test_classes_001( ) );
	do_test( test_classes_002( ) );
	do_test( test_embedded_arrays( ) );
	do_test( test_embedded_arrays_broken_001( ) );
} catch( json_exception const &jex ) {
	std::cerr << "Exception thrown by parser: " << jex.reason( ) << std::endl;
	exit( 1 );
}
