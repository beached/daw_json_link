// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/daw_json_link
//

#include "defines.h"

#include "daw/json/daw_json_link.h"

#include <daw/daw_benchmark.h>
#include <daw/daw_read_file.h>

#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <memory>
#include <optional>
#include <string>

namespace daw::cookbook_variant1 {
	struct MyVariantStuff1 {
		std::variant<int, std::string> member0;
		std::optional<std::variant<std::string, bool>> member1;
	};

	bool operator==( MyVariantStuff1 const &lhs, MyVariantStuff1 const &rhs ) {
		return lhs.member0 == rhs.member0 and lhs.member1 == rhs.member1;
	}
} // namespace daw::cookbook_variant1

namespace daw::json {
	template<>
	struct json_data_contract<daw::cookbook_variant1::MyVariantStuff1> {
#ifdef __cpp_nontype_template_parameter_class
		using type = json_member_list<
		  json_variant<"member0", std::variant<int, std::string>,
		               json_variant_type_list<int, std::string>>,
		  json_variant_null<"member1",
		                    std::optional<std::variant<std::string, bool>>,
		                    json_variant_type_list<std::string, bool>>>;
#else
		static constexpr char const member0[] = "member0";
		static constexpr char const member1[] = "member1";
		using type = json_member_list<
		  json_variant<member0, std::variant<int, std::string>,
		               json_variant_type_list<int, std::string>>,
		  json_variant_null<member1, std::optional<std::variant<std::string, bool>>,
		                    json_variant_type_list<std::string, bool>>>;
#endif
		static inline auto
		to_json_data( daw::cookbook_variant1::MyVariantStuff1 const &value ) {
			return std::forward_as_tuple( value.member0, value.member1 );
		}
	};
} // namespace daw::json

int main( int argc, char **argv )
#ifdef DAW_USE_JSON_EXCEPTIONS
  try
#endif
{
	if( argc <= 1 ) {
		puts( "Must supply path to cookbook_variant1.json file\n" );
		exit( EXIT_FAILURE );
	}
	auto data = *daw::read_file( argv[1] );
	puts( "Original" );
	puts( std::string( data.data( ), data.size( ) ).c_str( ) );
	auto stuff =
	  daw::json::from_json_array<daw::cookbook_variant1::MyVariantStuff1>( data );
	test_assert( stuff.size( ) == 3, "Unexpected size" );
	test_assert( stuff.front( ).member0.index( ) == 0, "Unexpected value" );
	test_assert( ( std::get<0>( stuff.front( ).member0 ) == 5 ),
	             "Unexpected value" );
	test_assert( stuff.front( ).member1->index( ) == 0, "Unexpected value" );
	test_assert( ( std::get<0>( stuff.front( ).member1.value( ) ) == "hello" ),
	             "Unexpected value" );

	auto const str = daw::json::to_json_array( stuff );
	puts( "After" );
	puts( str.c_str( ) );
	auto stuff2 =
	  daw::json::from_json_array<daw::cookbook_variant1::MyVariantStuff1>( str );

	test_assert( stuff == stuff2, "Unexpected round trip error" );
	return 0;
}
catch( daw::json::json_exception const &jex ) {
	std::cerr << "Exception thrown by parser: " << jex.reason( ) << std::endl;
	exit( 1 );
}
