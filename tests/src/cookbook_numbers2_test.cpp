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

namespace daw::cookbook_numbers2 {
	struct MyClass2 {
		unsigned member_unsigned0;
		unsigned member_unsigned1;
		signed member_signed;

		DAW_CONSTEXPR bool operator==( MyClass2 const &rhs ) const {
			return member_unsigned0 == rhs.member_unsigned0 and
			       member_unsigned1 == rhs.member_unsigned1 and
			       member_signed == rhs.member_signed;
		}
	};
} // namespace daw::cookbook_numbers2

namespace daw::json {
	template<>
	struct json_data_contract<daw::cookbook_numbers2::MyClass2> {
#ifdef __cpp_nontype_template_parameter_class
		using type = json_member_list<json_number<"member_unsigned0", unsigned>,
		                              json_number<"member_unsigned1", unsigned>,
		                              json_number<"member_signed", signed>>;
#else
		static constexpr char const member_unsigned0[] = "member_unsigned0";
		static constexpr char const member_unsigned1[] = "member_unsigned1";
		static constexpr char const member_signed[] = "member_signed";
		using type = json_member_list<json_number<member_unsigned0, unsigned>,
		                              json_number<member_unsigned1, unsigned>,
		                              json_number<member_signed, signed>>;
#endif
		static inline auto
		to_json_data( daw::cookbook_numbers2::MyClass2 const &value ) {
			return std::forward_as_tuple(
			  value.member_unsigned0, value.member_unsigned1, value.member_signed );
		}
	};
} // namespace daw::json

int main( int argc, char **argv )
#ifdef DAW_USE_JSON_EXCEPTIONS
  try
#endif
{
	if( argc <= 1 ) {
		puts( "Must supply path to cookbook_numbers2.json file\n" );
		exit( EXIT_FAILURE );
	}
	auto data = *daw::read_file( argv[1] );

	auto const cls = daw::json::from_json<daw::cookbook_numbers2::MyClass2>(
	  std::string_view( data.data( ), data.size( ) ) );

	test_assert( cls.member_unsigned0 == 12345, "Unexpected value" );
	test_assert( cls.member_signed == -12345, "Unexpected value" );
	std::string const str = daw::json::to_json( cls );
	puts( str.c_str( ) );

	auto const cls2 = daw::json::from_json<daw::cookbook_numbers2::MyClass2>(
	  std::string_view( str.data( ), str.size( ) ) );

	test_assert( cls == cls2, "Unexpected round trip error" );
}
catch( daw::json::json_exception const &jex ) {
	std::cerr << "Exception thrown by parser: " << jex.reason( ) << std::endl;
	exit( 1 );
}
