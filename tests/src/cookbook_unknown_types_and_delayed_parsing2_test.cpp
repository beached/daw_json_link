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

struct MyClass2 {
	int member0;
	daw::json::json_value member_later;
	std::string member1;
};

namespace daw::json {
	template<>
	struct json_data_contract<MyClass2> {
#if defined( __cpp_nontype_template_parameter_class )
		using type =
		  json_member_list<json_number<"member0", int>,
		                   json_delayed<"member_later">, json_string<"member1">>;
#else
		static constexpr char const member0[] = "member0";
		static constexpr char const member_later[] = "member_later";
		static constexpr char const member1[] = "member1";
		using type =
		  json_member_list<json_number<member0, int>, json_delayed<member_later>,
		                   json_string<member1>>;
#endif
		static inline auto to_json_data( MyClass2 const &value ) {
			return std::forward_as_tuple( value.member0, value.member_later,
			                              value.member1 );
		}
	};
} // namespace daw::json

// This isn't necessary to parse the values as but allows for direcly
// constructing the object
struct MyDelayedClass {
	int a;
	bool b;

	DAW_CONSTEXPR bool operator==( MyDelayedClass const &rhs ) const {
		return a == rhs.a and b == rhs.b;
	}
};

namespace daw::json {
	template<>
	struct json_data_contract<MyDelayedClass> {
#if defined( __cpp_nontype_template_parameter_class )
		using type = json_member_list<json_number<"a", int>, json_bool<"b">>;
#else
		static constexpr char const a[] = "a";
		static constexpr char const b[] = "b";
		using type = json_member_list<json_number<a, int>, json_bool<b>>;
#endif

		static inline auto to_json_data( MyDelayedClass const &value ) {
			return std::forward_as_tuple( value.a, value.b );
		}
	};
} // namespace daw::json

bool operator==( MyClass2 const &lhs, MyClass2 const &rhs ) {
	if( lhs.member0 != rhs.member0 or lhs.member1 != lhs.member1 ) {
		return false;
	}
	using namespace daw::json;
	return from_json<MyDelayedClass>( rhs.member_later ) ==
	       from_json<MyDelayedClass>( rhs.member_later );
}

int main( int argc, char **argv )
#ifdef DAW_USE_JSON_EXCEPTIONS
  try
#endif
{
	if( argc <= 1 ) {
		puts(
		  "Must supply path to cookbook_unknown_types_and_delayed_parsing2.json "
		  "file\n" );
		exit( EXIT_FAILURE );
	}
	auto data = *daw::read_file( argv[1] );

	auto const val = daw::json::from_json<MyClass2>(
	  std::string_view( data.data( ), data.size( ) ) );

	auto const delayed_val =
	  daw::json::from_json<MyDelayedClass>( val.member_later );

	test_assert( delayed_val.a == 1, "Unexpected value" );
	test_assert( delayed_val.b, "Unexpected value" );

	std::string json_str2 = daw::json::to_json( val );
	puts( json_str2.c_str( ) );
	auto const val2 = daw::json::from_json<MyClass2>( json_str2 );
	test_assert( val == val2, "Broken round trip" );
}
catch( daw::json::json_exception const &jex ) {
	std::cerr << "Exception thrown by parser: " << jex.reason( ) << std::endl;
	exit( 1 );
}
