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

#include <fstream>
#include <iostream>
#include <string>

#include <daw/daw_benchmark.h>
#include <daw/daw_bounded_hash_map.h>
#include <daw/daw_fnv1a_hash.h>
#include <daw/daw_string_view.h>

#include "daw/json/daw_json_iterator.h"
#include "daw/json/daw_json_link.h"

struct kv_t {
	std::unordered_map<std::string, int> kv{};
};

auto describe_json_class( kv_t ) noexcept {
	using namespace daw::json;
#ifdef __cpp_nontype_template_parameter/_class
	return class_description_t<json_key_value<
	  "kv", std::unordered_map<std::string, int>, json_number<no_name, int>>>{};
#else
	constexpr static char const kv[] = "kv";
	return class_description_t<json_key_value<
	  kv, std::unordered_map<std::string, int>, json_number<no_name, int>>>{};
#endif
}

struct kv2_t {
	daw::bounded_hash_map<daw::string_view, int, 5> kv{};
};

auto describe_json_class( kv2_t ) noexcept {
	using namespace daw::json;
#ifdef __cpp_nontype_template_parameter_class
	return class_description_t<json_key_value<
	  "kv", daw::bounded_hash_map<daw::string_view, int, 5>,
	  json_number<no_name, int>, json_string<no_name, daw::string_view>>>{};
#else
	constexpr static char const kv[] = "kv";
	return class_description_t<json_key_value<
	  kv, daw::bounded_hash_map<daw::string_view, int, 5>,
	  json_number<no_name, int>, json_string<no_name, daw::string_view>>>{};
#endif
}

int main( int argc, char **argv ) {
	using namespace daw::json;
	constexpr std::string_view const json_data3 =
	  R"( {"kv": {
				"key0": 0,
				"key1": 1,
				"key2": 2
	}})";

	auto kv_test = from_json<kv_t>( json_data3 );
	daw::do_not_optimize( kv_test );

	constexpr auto kv2_test = from_json<kv2_t>( json_data3 );
	daw::do_not_optimize( kv2_test );
}
