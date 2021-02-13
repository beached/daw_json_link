// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/daw_json_link
//
// See cookbook/class.md for the 3rd example
//

#include "defines.h"

#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <string>
#include <unordered_map>

#include <daw/daw_read_file.h>

#include "daw/json/daw_json_link.h"

namespace daw::cookbook_class3 {
	struct MyClass1 {
		std::string member_0;
		int member_1;
		bool member_2;
	};

	bool operator==( MyClass1 const &lhs, MyClass1 const &rhs ) {
		return std::tie( lhs.member_0, lhs.member_1, lhs.member_2 ) ==
		       std::tie( rhs.member_0, rhs.member_1, rhs.member_2 );
	}

	struct MyClass3 {
		MyClass1 a;
	};

	bool operator==( MyClass3 const &lhs, MyClass3 const &rhs ) {
		return lhs.a == rhs.a;
	}
} // namespace daw::cookbook_class3

namespace daw::json {
	template<>
	struct json_data_contract<daw::cookbook_class3::MyClass1> {
#if defined( __cpp_nontype_template_parameter_class )
		using type =
		  json_member_list<json_string<"member0">, json_number<"member1", int>,
		                   json_bool<"member2">>;
#else
		static constexpr char const member0[] = "member0";
		static constexpr char const member1[] = "member1";
		static constexpr char const member2[] = "member2";
		using type =
		  json_member_list<json_string<member0>, json_number<member1, int>,
		                   json_bool<member2>>;
#endif
		static inline auto
		to_json_data( daw::cookbook_class3::MyClass1 const &value ) {
			return std::forward_as_tuple( value.member_0, value.member_1,
			                              value.member_2 );
		}
	};

	template<>
	struct json_data_contract<daw::cookbook_class3::MyClass3> {
#if defined( __cpp_nontype_template_parameter_class )
		using type =
		  json_member_list<json_class<"a", daw::cookbook_class3::MyClass1>>;
#else
		static constexpr char const a[] = "a";
		using type =
		  json_member_list<json_class<a, daw::cookbook_class3::MyClass1>>;

#endif
		static inline auto
		to_json_data( daw::cookbook_class3::MyClass3 const &value ) {
			return std::forward_as_tuple( value.a );
		}
	};
} // namespace daw::json

int main( int argc, char **argv )
#ifdef DAW_USE_JSON_EXCEPTIONS
  try
#endif
{
	if( argc <= 1 ) {
		puts( "Must supply path to cookbook_class2.json file\n" );
		exit( EXIT_FAILURE );
	}
	auto data = *daw::read_file( argv[1] );

	auto const cls = daw::json::from_json<daw::cookbook_class3::MyClass3>(
	  std::string_view( data.data( ), data.size( ) ) );

	test_assert( cls.a.member_0 == "this is a test", "Unexpected value" );
	test_assert( cls.a.member_1 == 314159, "Unexpected value" );
	test_assert( cls.a.member_2 == true, "Unexpected value" );
	auto const str = daw::json::to_json( cls );
	puts( str.c_str( ) );

	auto const cls2 = daw::json::from_json<daw::cookbook_class3::MyClass3>(
	  std::string_view( str.data( ), str.size( ) ) );

	test_assert( cls == cls2, "Unexpected round trip error" );
}
#ifdef DAW_USE_JSON_EXCEPTIONS
catch( daw::json::json_exception const &jex ) {
	std::cerr << "Exception thrown by parser: " << jex.reason( ) << std::endl;
	exit( 1 );
}
#endif
