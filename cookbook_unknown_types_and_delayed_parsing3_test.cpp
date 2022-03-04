// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/daw_json_link
//
#include "defines.h"
// See cookbook/unknown_types_and_delayed_parsing.md for the 4th example
// This example shows how to use the json_delayed type to store/retrieve raw
// json as a string

#include "daw/json/daw_json_link.h"

#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <string>

struct Thing {
	std::string name;
	int type;
	std::string raw_json;
};

namespace daw::json {
	template<>
	struct json_data_contract<Thing> {
#if defined( __cpp_nontype_template_parameter_class )
		using type = json_member_list<json_string<"name">, json_number<"type", int>,
		                              json_delayed<"raw_json", std::string>>;
#else
		static constexpr char const name[] = "name";
		static constexpr char const type_member[] = "type";
		static constexpr char const raw_json[] = "raw_json";
		using type =
		  json_member_list<json_string<name>, json_number<type_member, int>,
		                   json_delayed<raw_json, std::string>>;
#endif
		static auto to_json_data( Thing const &value ) {
			return std::forward_as_tuple( value.name, value.type, value.raw_json );
		}
	};
} // namespace daw::json

int main( )
#ifdef DAW_USE_JSON_EXCEPTIONS
  try
#endif
{
	std::string obj = R"({ "member": 12345, "member2": "hello" })";
	auto my_thing = Thing{ "update", 1, obj };
	auto json = daw::json::to_json( my_thing );

	puts( json.c_str( ) );
	auto const val2 = daw::json::from_json<Thing>( json );
	test_assert( my_thing.raw_json == val2.raw_json,
	             "Unexpected round trip error" );
}
catch( daw::json::json_exception const &jex ) {
	std::cerr << "Exception thrown by parser: " << jex.reason( ) << std::endl;
	exit( 1 );
}
