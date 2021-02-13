// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/daw_json_link
//
// See cookbook/array.md for the 2nd example
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

namespace daw::cookbook_array2 {
	struct MyClass4 {
		std::string a;
		unsigned b;
		float c;
		bool d;
	};

	bool operator==( MyClass4 const &lhs, MyClass4 const &rhs ) {
		return std::tie( lhs.a, lhs.b, lhs.c, lhs.d ) ==
		       std::tie( rhs.a, rhs.b, rhs.c, rhs.d );
	}
} // namespace daw::cookbook_array2

namespace daw::json {
	template<>
	struct json_data_contract<daw::cookbook_array2::MyClass4> {
#if defined( __cpp_nontype_template_parameter_class )
		using type = json_member_list<json_string<"a">, json_number<"b", unsigned>,
		                              json_number<"c", float>, json_bool<"d">>;
#else
		static constexpr char const a[] = "a";
		static constexpr char const b[] = "b";
		static constexpr char const c[] = "c";
		static constexpr char const d[] = "d";
		using type = json_member_list<json_string<a>, json_number<b, unsigned>,
		                              json_number<c, float>, json_bool<d>>;
#endif
		static inline auto
		to_json_data( daw::cookbook_array2::MyClass4 const &value ) {
			return std::forward_as_tuple( value.a, value.b, value.c, value.d );
		}
	};
} // namespace daw::json

int main( int argc, char **argv )
#ifdef DAW_USE_JSON_EXCEPTIONS
  try
#endif
{
	if( argc <= 1 ) {
		puts( "Must supply path to cookbook_array2.json file\n" );
		exit( EXIT_FAILURE );
	}
	auto const data = *daw::read_file( argv[1] );

	using namespace daw::json;

	auto const ve = from_json_array<daw::cookbook_array2::MyClass4>(
	  { data.data( ), data.size( ) } );

	test_assert( ve.size( ) == 2, "Expected 2 items" );
	auto const str = to_json_array( ve );
	puts( str.c_str( ) );
	auto const ve2 = from_json_array<daw::cookbook_array2::MyClass4>(
	  { str.data( ), str.size( ) } );

	test_assert( ve == ve2, "Roundtrip failed" );
}
#ifdef DAW_USE_JSON_EXCEPTIONS
catch( daw::json::json_exception const &jex ) {
	std::cerr << "Exception thrown by parser: " << jex.reason( ) << std::endl;
	exit( 1 );
}
#endif
