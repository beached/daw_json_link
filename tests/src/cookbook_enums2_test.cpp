// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/daw_json_link
//
// See cookbook/enums.md for the 2nd example
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

namespace daw::cookbook_enums2 {
	enum class Colours : int8_t { red, green, blue, black };

	struct MyClass1 {
		std::vector<Colours> member0;
	};

	bool operator==( MyClass1 const &lhs, MyClass1 const &rhs ) {
		return lhs.member0 == rhs.member0;
	}
} // namespace daw::cookbook_enums2

namespace daw::json {
	template<>
	struct json_data_contract<daw::cookbook_enums2::MyClass1> {
#ifdef __cpp_nontype_template_parameter_class
		using type =
		  json_member_list<json_array<"member0", daw::cookbook_enums2::Colours>>;
#else
		static constexpr char const member0[] = "member0";
		using type =
		  json_member_list<json_array<member0, daw::cookbook_enums2::Colours>>;
#endif
		static inline auto
		to_json_data( daw::cookbook_enums2::MyClass1 const &value ) {
			return std::forward_as_tuple( value.member0 );
		}
	};
} // namespace daw::json

int main( int argc, char **argv )
#ifdef DAW_USE_JSON_EXCEPTIONS
  try
#endif
{
	if( argc <= 1 ) {
		puts( "Must supply path to cookbook_enums2.json file\n" );
		exit( EXIT_FAILURE );
	}
	auto data = *daw::read_file( argv[1] );

	auto const cls = daw::json::from_json<daw::cookbook_enums2::MyClass1>(
	  std::string_view( data.data( ), data.size( ) ) );

	test_assert( cls.member0[0] == daw::cookbook_enums2::Colours::red,
	             "Unexpected value" );
	test_assert( cls.member0[1] == daw::cookbook_enums2::Colours::green,
	             "Unexpected value" );
	test_assert( cls.member0[2] == daw::cookbook_enums2::Colours::blue,
	             "Unexpected value" );
	test_assert( cls.member0[3] == daw::cookbook_enums2::Colours::black,
	             "Unexpected value" );
	auto const str = daw::json::to_json( cls );
	puts( str.c_str( ) );

	auto const cls2 = daw::json::from_json<daw::cookbook_enums2::MyClass1>(
	  std::string_view( str.data( ), str.size( ) ) );

	test_assert( cls == cls2, "Unexpected round trip error" );
}
#ifdef DAW_USE_JSON_EXCEPTIONS
catch( daw::json::json_exception const &jex ) {
	std::cerr << "Exception thrown by parser: " << jex.reason( ) << std::endl;
	exit( 1 );
}
#endif
