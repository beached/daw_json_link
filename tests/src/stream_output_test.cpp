// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/daw_json_link
//

#include "defines.h"

#include "daw/json/daw_json_link.h"

#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <string>

namespace daw::cookbook_class1 {
	struct MyClass1 {
		std::string member_0;
		int member_1;
		bool member_2;
	};

	bool operator==( MyClass1 const &lhs, MyClass1 const &rhs ) {
		return std::tie( lhs.member_0, lhs.member_1, lhs.member_2 ) ==
		       std::tie( rhs.member_0, rhs.member_1, rhs.member_2 );
	}
} // namespace daw::cookbook_class1

namespace daw::json {
	template<>
	struct json_data_contract<daw::cookbook_class1::MyClass1> {
#if defined( __cpp_nontype_template_parameter_class )
		using type = json_member_list<json_string<"member0">,
		                              json_number<"member1", int>,
		                              json_bool<"member2">>;
#else
		static constexpr char const member0[] = "member0";
		static constexpr char const member1[] = "member1";
		static constexpr char const member2[] = "member2";
		using type = json_member_list<json_string<member0>,
		                              json_number<member1, int>,
		                              json_bool<member2>>;
#endif
		static inline auto
		to_json_data( daw::cookbook_class1::MyClass1 const &value ) {
			return std::forward_as_tuple( value.member_0,
			                              value.member_1,
			                              value.member_2 );
		}
	};
} // namespace daw::json

int main( int, char ** )
#ifdef DAW_USE_EXCEPTIONS
  try
#endif
{
	std::string_view data = R"(
{
  "member0": "this is a test",
  "member1": 314159,
  "member2": true
}
)";

	auto const cls = daw::json::from_json<daw::cookbook_class1::MyClass1>(
	  std::string_view( data.data( ), data.size( ) ) );

	test_assert( cls.member_0 == "this is a test", "Unexpected value" );
	test_assert( cls.member_1 == 314159, "Unexpected value" );
	test_assert( cls.member_2, "Unexpected value" );

	std::stringstream ss{ };
	auto it = std::ostreambuf_iterator<char>( ss );

	(void)daw::json::to_json( cls, it );
	std::string const str = ss.str( );
	puts( str.c_str( ) );
}
#ifdef DAW_USE_EXCEPTIONS
catch( daw::json::json_exception const &jex ) {
	std::cerr << "Exception thrown by parser: " << jex.reason( ) << '\n';
	exit( 1 );
} catch( std::exception const &ex ) {
	std::cerr << "Unknown exception thrown during testing: " << ex.what( )
	          << '\n';
	exit( 1 );
} catch( ... ) {
	std::cerr << "Unknown exception thrown during testing\n";
	throw;
}
#endif
