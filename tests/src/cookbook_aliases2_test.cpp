// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/daw_json_link
//

/***
 * This example shows using a full JSON mapping with json_type_alias
 */

#include <daw/json/daw_json_link.h>

#include <daw/daw_ensure.h>

#include <iostream>
#include <string>

struct MyExplicitClass {
	std::string value;
};

namespace daw::json {
	template<>
	struct json_data_contract<MyExplicitClass> {
		using type = json_type_alias<json_string_no_name<std::string>>;

		static auto to_json_data( MyExplicitClass const &v ) {
			return v.value;
		}
	};
} // namespace daw::json

int main( )
#if defined( DAW_USE_EXCEPTIONS )
  try
#endif
{
	MyExplicitClass const c0 =
	  daw::json::from_json<MyExplicitClass>( R"json("Hello World")json" );
	daw_ensure( c0.value == "Hello World" );
	std::string const json0 = daw::json::to_json( c0 );
	MyExplicitClass const c1 =
	  daw::json::from_json<MyExplicitClass>( json0 );
	daw_ensure( c1.value == c0.value );
	std::cout << json0 << '\n';
}
#if defined( DAW_USE_EXCEPTIONS )
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
