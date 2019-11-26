// The MIT License (MIT)
//
// Copyright (c) 2019 Darrell Wright
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

#include <iostream>

#include <daw/daw_benchmark.h>
#include <daw/daw_memory_mapped_file.h>

#include "daw/json/daw_json_link.h"

struct unicode_data {
	std::string escaped;
	std::string unicode;
};

#ifdef __cpp_nontype_template_parameter_class
static inline auto describe_json_class( unicode_data ) {
	using namespace daw::json;
	return daw::json::class_description_t<json_string<"escaped">,
	                                      json_string<"unicode">>{};
}
#else
namespace symbols_unicode_data {
	constexpr static char const escaped[] = "escaped";
	constexpr static char const unicode[] = "unicode";
} // namespace symbols_unicode_data

static inline auto describe_json_class( unicode_data ) {
	using namespace daw::json;
	return daw::json::class_description_t<
	  json_string<symbols_unicode_data::escaped>,
	  json_string<symbols_unicode_data::unicode>>{};
}
#endif

static inline auto to_json_data( unicode_data const &value ) {
	return std::forward_as_tuple( value.escaped, value.unicode );
}

bool operator==( unicode_data const &lhs, unicode_data const &rhs ) {
	return to_json_data( lhs ) == to_json_data( rhs );
}

int main( int argc, char **argv ) {
	using namespace daw::json;
#if defined( NDEBUG ) and not defined( DEBUG )
	std::cout << "release run\n";
#else
	std::cout << "debug run\n";
#endif
	if( argc < 3 ) {
		std::cerr << "Must supply filenames to open\n";
		std::cerr << "full_unicode.json full_unicode_escaped.json\n";
		exit( 1 );
	}

	auto const json_str = daw::filesystem::memory_mapped_file_t<>( argv[1] );
	auto const json_str_escaped =
	  daw::filesystem::memory_mapped_file_t<>( argv[2] );

	auto unicode_test = from_json_array<unicode_data>(
	  std::string_view( json_str.data( ), json_str.size( ) ) );
	auto unicode_test_from_escaped = from_json_array<unicode_data>(
	  std::string_view( json_str_escaped.data( ), json_str_escaped.size( ) ) );

	auto mismatch_pos = std::mismatch( unicode_test.begin( ), unicode_test.end( ),
	                                   unicode_test_from_escaped.begin( ) );
	daw_json_assert( mismatch_pos.first == unicode_test.end( ),
	                 "Should be the same after parsing" );

	auto const json_str2 = to_json_array( unicode_test );
	auto unicode_test2 = from_json_array<unicode_data>(
	  std::string_view( json_str2.data( ), json_str2.size( ) ) );

	auto mismatch_pos2 = std::mismatch(
	  unicode_test.begin( ), unicode_test.end( ), unicode_test2.begin( ) );
	daw_json_assert( mismatch_pos2.first == unicode_test.end( ),
	                 "Should be the same after parsing" );
}
