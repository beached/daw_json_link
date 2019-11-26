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

namespace daw::cookbook_kv2 {
	struct MyKeyValue2 {
		std::unordered_map<intmax_t, std::string> kv;
	};

#if defined( __cpp_nontype_template_parameter_class )
	auto describe_json_class( MyKeyValue2 const & ) {
		using namespace daw::json;
		return class_description_t<
		  json_key_value_array<"kv", std::unordered_map<intmax_t, std::string>,
		                       json_string_raw<"value">, intmax_t>>{};
	}
#else
	namespace symbols_MyKeyValue2 {
		static constexpr char const kv[] = "kv";
		static constexpr char const value[] = "value";
	} // namespace symbols_MyKeyValue2
	auto describe_json_class( MyKeyValue2 ) {
		using namespace daw::json;
		return class_description_t<json_key_value_array<
		  symbols_MyKeyValue2::kv, std::unordered_map<intmax_t, std::string>,
		  json_string_raw<symbols_MyKeyValue2::value>, intmax_t>>{};
	}

#endif
	auto to_json_data( MyKeyValue2 const &value ) {
		return std::forward_as_tuple( value.kv );
	}

	bool operator==( MyKeyValue2 const &lhs, MyKeyValue2 const &rhs ) {
		return lhs.kv == rhs.kv;
	}
} // namespace daw::cookbook_kv2

int main( int argc, char **argv ) {
	if( argc <= 1 ) {
		puts( "Must supply path to cookbook_kv2.json file\n" );
		exit( EXIT_FAILURE );
	}
	auto data = daw::filesystem::memory_mapped_file_t<>( argv[1] );

	auto kv = daw::json::from_json<daw::cookbook_kv2::MyKeyValue2>(
	  std::string_view( data.data( ), data.size( ) ) );

	daw_json_assert( kv.kv.size( ) == 2, "Expected data to have 2 items" );
	daw_json_assert( kv.kv[0] == "test_001", "Unexpected value" );
	daw_json_assert( kv.kv[1] == "test_002", "Unexpected value" );
	auto const str = daw::json::to_json( kv );
	puts( str.c_str( ) );

	auto const kv2 = daw::json::from_json<daw::cookbook_kv2::MyKeyValue2>(
	  std::string_view( str.data( ), str.size( ) ) );
	daw_json_assert( kv == kv2, "Unexpected round trip error" );
}
