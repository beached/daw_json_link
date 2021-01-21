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

#include <iostream>

namespace daw {
	struct Data {
		int a;
	};

	bool operator==( Data const &lhs, Data const &rhs ) {
		return lhs.a == rhs.a;
	}
} // namespace daw

namespace daw::json {
	template<>
	struct json_data_contract<daw::Data> {
#ifdef __cpp_nontype_template_parameter_class
		using type = json_member_list<json_number<"a", int>>;
#else
		static constexpr char const a[] = "a";
		using type = json_member_list<json_number<a, int>>;
#endif
		static inline auto to_json_data( daw::Data const &value ) {
			return std::forward_as_tuple( value.a );
		}
	}; // namespace daw::json
} // namespace daw::json

int main( int argc, char **argv )
#ifdef DAW_USE_JSON_EXCEPTIONS
  try
#endif
{
	if( argc <= 1 ) {
		puts( "Must supply path to small_test.json file\n" );
		exit( EXIT_FAILURE );
	}
	auto data = *daw::read_file( argv[1] );

	auto const cls = daw::json::from_json<daw::Data>(
	  std::string_view( data.data( ), data.size( ) ) );

	test_assert( cls.a == 12345, "Unexpected value" );
}
catch( daw::json::json_exception const &jex ) {
	std::cerr << "Exception thrown by parser: " << jex.reason( ) << std::endl;
	exit( 1 );
}
