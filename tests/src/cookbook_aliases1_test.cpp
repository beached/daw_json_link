// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/daw_json_link
//

/***
 * This example shows mapping types like json_string to user classes
 */

#include <daw/json/daw_json_link.h>

#include <daw/daw_read_file.h>

#include <iostream>
#include <string>

struct MyClass {
	std::string value;
};

namespace daw::json {
	template<>
	struct json_data_contract<MyClass> {
		using type = json_type_alias<std::string>;

		static inline auto to_json_data( MyClass const &v ) {
			return v.value;
		}
	};
} // namespace daw::json

int main( int argc, char **argv )
#ifdef DAW_USE_EXCEPTIONS
  try
#endif
{
	if( argc <= 1 ) {
		puts( "Must supply path to cookbook_aliases1.json file\n" );
		exit( EXIT_FAILURE );
	}
	auto data = daw::read_file( argv[1] );
	assert( data and data->size( ) > 0 );

	MyClass c0 = daw::json::from_json<MyClass>( *data );
	std::string json0 = daw::json::to_json( c0 );
	std::cout << json0 << '\n';
	(void)c0;
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