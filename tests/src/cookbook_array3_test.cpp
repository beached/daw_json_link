// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/daw_json_link
//
// See cookbook/array.md for the 3rd example
//

#include "defines.h"

#include "daw/json/daw_json_link.h"

#include <daw/daw_read_file.h>

#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <string>
#include <vector>

namespace daw::cookbook_array3 {
	struct MyArrayClass1 {
		int member0;
		std::vector<int> member1;
		std::vector<std::string> member2;
	};

	bool operator==( MyArrayClass1 const &lhs, MyArrayClass1 const &rhs ) {
		return std::tie( lhs.member0, lhs.member1, lhs.member2 ) ==
		       std::tie( rhs.member0, rhs.member1, rhs.member2 );
	}
} // namespace daw::cookbook_array3

namespace daw::json {
	template<>
	struct json_data_contract<daw::cookbook_array3::MyArrayClass1> {
#ifdef __cpp_nontype_template_parameter_class
		using type =
		  json_member_list<json_number<"member0", int>, json_array<"member1", int>,
		                   json_array<"member2", std::string>>;
#else
		static constexpr char const member0[] = "member0";
		static constexpr char const member1[] = "member1";
		static constexpr char const member2[] = "member2";
		using type =
		  json_member_list<json_number<member0, int>, json_array<member1, int>,
		                   json_array<member2, std::string>>;
#endif
		static inline auto
		to_json_data( daw::cookbook_array3::MyArrayClass1 const &value ) {
			return std::forward_as_tuple( value.member0, value.member1,
			                              value.member2 );
		}
	};
} // namespace daw::json

int main( int argc, char **argv )
#ifdef DAW_USE_JSON_EXCEPTIONS
  try
#endif
{
	if( argc <= 1 ) {
		puts( "Must supply path to cookbook_array3.json file\n" );
		exit( EXIT_FAILURE );
	}
	auto const data = *daw::read_file( argv[1] );

	using namespace daw::json;

	auto const my_array_class = from_json<daw::cookbook_array3::MyArrayClass1>(
	  { data.data( ), data.size( ) } );

	test_assert( my_array_class.member1.size( ) == 5, "Expected 5 items" );
	test_assert( my_array_class.member2.size( ) == 2, "Expected 2 items" );
	auto const str = to_json( my_array_class );
	puts( str.c_str( ) );
	auto const my_array_class2 = from_json<daw::cookbook_array3::MyArrayClass1>(
	  { str.data( ), str.size( ) } );

	test_assert( my_array_class == my_array_class2, "Round trip failed" );
}
#ifdef DAW_USE_JSON_EXCEPTIONS
catch( daw::json::json_exception const &jex ) {
	std::cerr << "Exception thrown by parser: " << jex.reason( ) << std::endl;
	exit( 1 );
}
#endif
