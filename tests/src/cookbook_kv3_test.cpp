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
#include <map>
#include <string>

namespace daw::cookbook_kv3 {
	struct MyKeyValue3 {
		std::multimap<std::string, std::string> kv;

		friend bool operator==( MyKeyValue3 const &lhs, MyKeyValue3 const &rhs ) {
			return lhs.kv == rhs.kv;
		}
	};
} // namespace daw::cookbook_kv3

namespace daw::json {
	template<>
	struct json_data_contract<cookbook_kv3::MyKeyValue3> {
#ifdef __cpp_nontype_template_parameter_class
		using type = json_member_list<json_key_value<
		  "kv", std::multimap<std::string, std::string>, std::string>>;
#else
		static constexpr char const kv[] = "kv";
		using type = json_member_list<
		  json_key_value<kv, std::multimap<std::string, std::string>, std::string>>;
#endif
		static inline auto to_json_data( cookbook_kv3::MyKeyValue3 const &v ) {
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
		puts( "Must supply path to cookbook_kv3.json file\n" );
		exit( EXIT_FAILURE );
	}
	auto data = *daw::read_file( argv[1] );

	auto kv = daw::json::from_json<daw::cookbook_kv3::MyKeyValue3>(
	  std::string_view( data.data( ), data.size( ) ) );

	test_assert( kv.kv.size( ) == 2, "Expected data to have 2 items" );
	test_assert( kv.kv.begin( )->first == std::prev( kv.kv.end( ) )->first,
	             "Unexpected value" );
	test_assert( kv.kv.begin( )->second != std::prev( kv.kv.end( ) )->second,
	             "Unexpected value" );
	auto const str = daw::json::to_json( kv );
	puts( str.c_str( ) );

	auto const kv3 = daw::json::from_json<daw::cookbook_kv3::MyKeyValue3>(
	  std::string_view( str.data( ), str.size( ) ) );
	test_assert( kv == kv3, "Unexpected round trip error" );
}
catch( daw::json::json_exception const &jex ) {
	std::cerr << "Exception thrown by parser: " << jex.reason( ) << std::endl;
	exit( 1 );
}
