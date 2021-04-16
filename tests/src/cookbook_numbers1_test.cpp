// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/daw_json_link
//

#include "defines.h"

#include <daw/daw_read_file.h>

#include "daw/json/daw_json_link.h"

#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <string>

namespace daw::cookbook_numbers1 {
	struct MyClass1 {
		double member0;
		double member1;
		double member2;
		double member3;

		DAW_CONSTEXPR bool operator==( MyClass1 const &rhs ) const {
			return member0 == rhs.member0 and member1 == rhs.member1 and
			       member2 == rhs.member2 and member3 == rhs.member3;
		}
	};
} // namespace daw::cookbook_numbers1

namespace daw::json {
	template<>
	struct json_data_contract<daw::cookbook_numbers1::MyClass1> {
#ifdef __cpp_nontype_template_parameter_class
		using type =
		  json_member_list<json_number<"member0">, json_number<"member1">,
		                   json_number<"member2">, json_number<"member3">>;
#else
		static constexpr char const member0[] = "member0";
		static constexpr char const member1[] = "member1";
		static constexpr char const member2[] = "member2";
		static constexpr char const member3[] = "member3";
		using type = json_member_list<json_number<member0>, json_number<member1>,
		                              json_number<member2>, json_number<member3>>;
#endif
		static inline auto
		to_json_data( cookbook_numbers1::MyClass1 const &value ) {
			return std::forward_as_tuple( value.member0, value.member1, value.member2,
			                              value.member3 );
		}
	};
} // namespace daw::json

int main( int argc, char **argv )
#ifdef DAW_USE_JSON_EXCEPTIONS
  try
#endif
{
	if( argc <= 1 ) {
		puts( "Must supply path to cookbook_numbers1.json file\n" );
		exit( EXIT_FAILURE );
	}
	auto data = *daw::read_file( argv[1] );

	auto const cls = daw::json::from_json<daw::cookbook_numbers1::MyClass1>(
	  std::string_view( data.data( ), data.size( ) ) );

	test_assert( cls.member0 == 1.23, "Unexpected value" );
	test_assert( cls.member1 == 1, "Unexpected value" );
	test_assert( cls.member2 == 1200, "Unexpected value" );
	test_assert( cls.member3 == 3000, "Unexpected value" );
	std::string const str = daw::json::to_json( cls );
	puts( str.c_str( ) );

	auto const cls2 = daw::json::from_json<daw::cookbook_numbers1::MyClass1>(
	  std::string_view( str.data( ), str.size( ) ) );

	test_assert( cls == cls2, "Unexpected round trip error" );
}
catch( daw::json::json_exception const &jex ) {
	std::cerr << "Exception thrown by parser: " << jex.reason( ) << std::endl;
	exit( 1 );
}
