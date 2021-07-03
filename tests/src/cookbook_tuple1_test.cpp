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

#include <string_view>
#include <tuple>

struct Foo {
	std::tuple<int, std::string> a;

	inline bool operator==( Foo const & rhs ) const {
		return a == rhs.a;
	}
};

namespace daw::json {
	template<>
	struct json_data_contract<Foo> {
		static constexpr char const a[] = "a";
		using type = json_member_list<json_tuple<a, std::tuple<int, std::string>>>;

		static constexpr auto to_json_data( Foo const &f ) {
			return std::forward_as_tuple( f.a );
		}
	};
} // namespace daw::json

int main( int argc, char **argv )
#ifdef DAW_USE_EXCEPTIONS
  try
#endif
{
	if( argc <= 1 ) {
		puts( "Must supply path to cookbook_numbers3.json file\n" );
		exit( EXIT_FAILURE );
	}
	auto data = daw::read_file( argv[1] );
	assert( data and not data->empty( ) );
	Foo foo1 = daw::json::from_json<Foo>( *data );
	std::string data2 = daw::json::to_json( foo1 );
	Foo foo2 = daw::json::from_json<Foo>( data2 );
	assert( foo1 == foo2 );
}
#ifdef DAW_USE_EXCEPTIONS
catch( daw::json::json_exception const &jex ) {
	std::cerr << "Exception thrown by parser: " << jex.reason( ) << std::endl;
	exit( 1 );
}
#endif
