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
		static constexpr char const uris[] = "uris";
		using type = json_member_list<json_array<uris, std::string>>;
#endif
		static inline auto
		to_json_data( daw::cookbook_escaped_strings1::WebData const &value ) {
			return std::forward_as_tuple( value.uris );
		}
	};
} // namespace daw::json

int main( int argc, char **argv )
#ifdef DAW_USE_JSON_EXCEPTIONS
  try
#endif
{
	if( argc <= 1 ) {
		puts( "Must supply path to cookbook_escaped_strings1.json file\n" );
		exit( EXIT_FAILURE );
	}
	auto const expected_wd = daw::cookbook_escaped_strings1::WebData{
	  { "example.com", "Bücher.ch", "happy.cn", "happy快乐.cn", "快乐.cn",
	    "happy.中国", "快乐.中国", "www.ハンドボールサムズ.com", "🦄.com" } };

	auto data = *daw::read_file( argv[1] );

	auto const cls =
	  daw::json::from_json<daw::cookbook_escaped_strings1::WebData>(
	    std::string_view( data.data( ), data.size( ) ) );

	test_assert( cls == expected_wd, "Unexpected value" );
	auto const str = daw::json::to_json( cls );
	puts( str.c_str( ) );

	auto const cls2 =
	  daw::json::from_json<daw::cookbook_escaped_strings1::WebData>(
	    std::string_view( str.data( ), str.size( ) ) );

	test_assert( cls == cls2, "Unexpected round trip error" );
}
#ifdef DAW_USE_JSON_EXCEPTIONS
catch( daw::json::json_exception const &jex ) {
	std::cerr << "Exception thrown by parser: " << jex.reason( ) << std::endl;
	exit( 1 );
}
#endif
