// The MIT License (MIT)
//
// Copyright (c) 2019-2020 Darrell Wright
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

#include "daw/json/daw_json_link.h"

#include <daw/daw_memory_mapped_file.h>

#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <string>
#include <unordered_map>

namespace daw::cookbook_escaped_strings1 {
	struct WebData {
		std::vector<std::string> uris;
	};

	bool operator==( WebData const &lhs, WebData const &rhs ) {
		return lhs.uris == rhs.uris;
	}
} // namespace daw::cookbook_escaped_strings1

namespace daw::json {
	template<>
	struct json_data_contract<daw::cookbook_escaped_strings1::WebData> {
#if defined( __cpp_nontype_template_parameter_class )
		using type = json_member_list<json_array<"uris", std::string>>;
#else
		static inline constexpr char const uris[] = "uris";
		using type = json_member_list<json_array<uris, std::string>>;
#endif
		static inline auto
		to_json_data( daw::cookbook_escaped_strings1::WebData const &value ) {
			return std::forward_as_tuple( value.uris );
		}
	};
} // namespace daw::json

int main( int argc, char **argv ) try {
	if( argc <= 1 ) {
		puts( "Must supply path to cookbook_escaped_strings1.json file\n" );
		exit( EXIT_FAILURE );
	}
	auto const expected_wd = daw::cookbook_escaped_strings1::WebData{
	  { "example.com", "Bücher.ch", "happy.cn", "happy快乐.cn", "快乐.cn",
	    "happy.中国", "快乐.中国", "www.ハンドボールサムズ.com", "🦄.com" } };

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
} catch( daw::json::json_exception const &jex ) {
	std::cerr << "Exception thrown by parser: " << jex.reason( ) << std::endl;
	exit( 1 );
}
