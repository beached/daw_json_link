// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/daw_json_link
//

#include "defines.h"

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
	std::unordered_map<std::string, int> kv{ };
};

struct kv2_t {
	daw::bounded_hash_map<daw::string_view, int, 5, daw::fnv1a_hash_t> kv{ };
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
		using type = json_member_list<json_key_value<
		  "kv", daw::bounded_hash_map<daw::string_view, int, 5, daw::fnv1a_hash_t>,
		  int, daw::string_view>>;
#else
		constexpr inline static char const kv[] = "kv";
		using type = json_member_list<json_key_value<
		  kv, daw::bounded_hash_map<daw::string_view, int, 5, daw::fnv1a_hash_t>,
		  int, daw::string_view>>;
#endif
	};
} // namespace daw::json

int main( int, char ** )
#ifdef DAW_USE_JSON_EXCEPTIONS
  try
#endif
{
	DAW_CONSTEXPR std::string_view const json_data3 =
	  R"( {"kv": {
				"key0": 0,
				"key1": 1,
				"key2": 2
	}})";

	kv_t kv_test = daw::json::from_json<kv_t>( json_data3 );
	daw::do_not_optimize( kv_test );

	DAW_CONSTEXPR kv2_t kv2_test = daw::json::from_json<kv2_t>( json_data3 );
	test_assert( kv2_test.kv.size( ) == 3, "Unexpected size" );
	test_assert( kv2_test.kv["key0"] == 0, "Unexpected value" );
	test_assert( kv2_test.kv["key1"] == 1, "Unexpected value" );
	test_assert( kv2_test.kv["key2"] == 2, "Unexpected value" );
}
catch( daw::json::json_exception const &jex ) {
	std::cerr << "Exception thrown by parser: " << jex.reason( ) << std::endl;
	exit( 1 );
}
