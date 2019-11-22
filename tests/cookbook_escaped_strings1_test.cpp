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

#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <string>
#include <unordered_map>

#include <daw/daw_memory_mapped_file.h>

#include "daw/json/daw_json_link.h"

namespace daw::cookbook_escaped_strings1 {
	struct WebData {
		std::vector<std::string> uris;
	};

#if defined( __cpp_nontype_template_parameter_class )
	auto describe_json_class( WebData const & ) {
		using namespace daw::json;
		return class_description_t<
		  json_array<"uris", std::vector<std::string>, json_string<no_name>>>{};
	}
#else
	namespace symbols_WebData {
		static constexpr char const uris[] = "uris";
	}

	auto describe_json_class( WebData const & ) {
		using namespace daw::json;
		return class_description_t<json_array<
		  symbols_WebData::uris, std::vector<std::string>, json_string<no_name>>>{};
	}
#endif

	auto to_json_data( WebData const &value ) {
		return std::forward_as_tuple( value.uris );
	}

	bool operator==( WebData const &lhs, WebData const &rhs ) {
		return to_json_data( lhs ) == to_json_data( rhs );
	}
} // namespace daw::cookbook_escaped_strings1

int main( int argc, char **argv ) {
	if( argc <= 1 ) {
		puts( "Must supply path to cookbook_escaped_strings1.json file\n" );
		exit( EXIT_FAILURE );
	}
	auto const expected_wd = daw::cookbook_escaped_strings1::WebData{
	  {"example.com", "Bücher.ch", "happy.cn", "happy快乐.cn", "快乐.cn",
	   "happy.中国", "快乐.中国", "www.ハンドボールサムズ.com", "🦄.com"}};

	auto data = daw::filesystem::memory_mapped_file_t<>( argv[1] );

	auto const cls =
	  daw::json::from_json<daw::cookbook_escaped_strings1::WebData>(
	    std::string_view( data.data( ), data.size( ) ) );

	daw_json_assert( cls == expected_wd, "Unexpected value" );
	auto const str = daw::json::to_json( cls );
	puts( str.c_str( ) );

	auto const cls2 =
	  daw::json::from_json<daw::cookbook_escaped_strings1::WebData>(
	    std::string_view( str.data( ), str.size( ) ) );

	daw_json_assert( cls == cls2, "Unexpected round trip error" );
}
