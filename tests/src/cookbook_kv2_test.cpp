// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/daw_json_link
//

#include "defines.h"

#include "daw/json/daw_json_link.h"

#include <daw/daw_read_file.h>

#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <string>
#include <unordered_map>

namespace daw::cookbook_kv2 {
	struct MyKeyValue2 {
		std::unordered_map<intmax_t, std::string> kv;
	};

	bool operator==( MyKeyValue2 const &lhs, MyKeyValue2 const &rhs ) {
		return lhs.kv == rhs.kv;
	}
} // namespace daw::cookbook_kv2

namespace daw::json {
	template<>
	struct json_data_contract<daw::cookbook_kv2::MyKeyValue2> {
#ifdef __cpp_nontype_template_parameter_class
		using type = json_member_list<
		  json_key_value_array<"kv", std::unordered_map<intmax_t, std::string>,
		                       json_string_raw<"value">, intmax_t>>;
#else
		static constexpr char const kv[] = "kv";
		static constexpr char const value[] = "value";
		using type = json_member_list<
		  json_key_value_array<kv, std::unordered_map<intmax_t, std::string>,
		                       json_string_raw<value>, intmax_t>>;
#endif
		static inline auto to_json_data( daw::cookbook_kv2::MyKeyValue2 const &v ) {
			return std::forward_as_tuple( v.kv );
		}
	};
} // namespace daw::json

int main( int argc, char **argv )
#ifdef DAW_USE_JSON_EXCEPTIONS
  try
#endif
{
	if( argc <= 1 ) {
		puts( "Must supply path to cookbook_kv2.json file\n" );
		exit( EXIT_FAILURE );
	}
	auto data = *daw::read_file( argv[1] );

	auto kv = daw::json::from_json<daw::cookbook_kv2::MyKeyValue2>(
	  std::string_view( data.data( ), data.size( ) ) );

	test_assert( kv.kv.size( ) == 2, "Expected data to have 2 items" );
	test_assert( kv.kv[0] == "test_001", "Unexpected value" );
	test_assert( kv.kv[1] == "test_002", "Unexpected value" );
	auto const str = daw::json::to_json( kv );
	puts( str.c_str( ) );

	auto const kv2 = daw::json::from_json<daw::cookbook_kv2::MyKeyValue2>(
	  std::string_view( str.data( ), str.size( ) ) );
	test_assert( kv == kv2, "Unexpected round trip error" );
}
catch( daw::json::json_exception const &jex ) {
	std::cerr << "Exception thrown by parser: " << jex.reason( ) << std::endl;
	exit( 1 );
}
