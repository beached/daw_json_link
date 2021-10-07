// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/daw_json_link
//

#include <daw/json/daw_json_link.h>

#include <daw/daw_read_file.h>

#include <cstdio>
#include <iostream>

struct Foo {
	std::string bar;
	std::string raw_json;
};

namespace daw::json {
	template<>
	struct json_data_contract<Foo> {
		static constexpr char const bar[] = "bar";
		static constexpr char const raw_json[] = "raw_json";
		using type = json_member_list<json_link<bar, std::string>,
		                              json_raw<raw_json, std::string>>;

		static auto to_json_data( Foo const &v ) {
			return std::forward_as_tuple( v.bar, v.raw_json );
		}
	};
} // namespace daw::json

int main( int argc, char **argv )
#ifdef DAW_USE_EXCEPTIONS
  try
#endif
{
	if( argc <= 1 ) {
		puts(
		  "Must supply path to cookbook_inserting_extracting_raw_json1.json "
		  "file\n" );
		exit( EXIT_FAILURE );
	}
	auto data = *daw::read_file( argv[1] );

	puts( data.c_str( ) );

	auto my_foo = daw::json::from_json<Foo>( data );
	my_foo.raw_json = R"({"a":55})";

	auto data2 = daw::json::to_json( my_foo );

	puts( data2.c_str( ) );
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