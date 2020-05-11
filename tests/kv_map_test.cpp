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

#include "daw/json/daw_json_iterator.h"
#include "daw/json/daw_json_link.h"

#include <daw/daw_benchmark.h>
#include <daw/daw_bounded_hash_map.h>
#include <daw/daw_fnv1a_hash.h>
#include <daw/daw_string_view.h>

#include <fstream>
#include <iostream>
#include <string>

struct kv_t {
	std::unordered_map<std::string, int> kv{};
};

struct kv2_t {
	daw::bounded_hash_map<std::string, int, 5> kv{};
};

namespace daw::json {
	template<>
	struct json_data_contract<kv_t> {
#ifdef __cpp_nontype_template_parameter_class
		using type = json_member_list<
		  json_key_value<"kv", std::unordered_map<std::string, int>, int>>;
#else
		constexpr inline static char const kv[] = "kv";
		using type = json_member_list<
		  json_key_value<kv, std::unordered_map<std::string, int>, int>>;
#endif
	};

	template<>
	struct json_data_contract<kv2_t> {
#ifdef __cpp_nontype_template_parameter_class
		using type = json_member_list<
		  json_key_value<"kv", daw::bounded_hash_map<std::string, int, 5>, int>>;
#else
		constexpr inline static char const kv[] = "kv";
		using type = json_member_list<
		  json_key_value<kv, daw::bounded_hash_map<std::string, int, 5>, int>>;
#endif
	};
} // namespace daw::json

int main( int, char ** ) try {
	constexpr std::string_view const json_data3 =
	  R"( {"kv": {
				"key0": 0,
				"key1": 1,
				"key2": 2
	}})";

	kv_t kv_test = daw::json::from_json<kv_t>( json_data3 );
	daw::do_not_optimize( kv_test );

	kv2_t const kv2_test = daw::json::from_json<kv2_t>( json_data3 );

	daw_json_assert( kv2_test.kv.size( ) == 3, "Unexpected size" );
	daw_json_assert( kv2_test.kv["key0"] == 0, "Unexpected value" );
	daw_json_assert( kv2_test.kv["key1"] == 1, "Unexpected value" );
	daw_json_assert( kv2_test.kv["key2"] == 2, "Unexpected value" );
} catch( daw::json::json_exception const &jex ) {
	std::cerr << "Exception thrown by parser: " << jex.reason( ) << std::endl;
	exit( 1 );
}
