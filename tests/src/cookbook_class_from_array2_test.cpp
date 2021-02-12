// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/daw_json_link
//
// See cookbook/class_from_array.md for the 2nd example
//

#include "defines.h"

#include <daw/daw_read_file.h>

#include "daw/json/daw_json_link.h"

#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <string>
#include <unordered_map>

namespace daw::cookbook_class_from_array2 {
	struct Point {
		double x;
		double y;
	};

	bool operator!=( Point const &lhs, Point const &rhs ) {
		return lhs.x != rhs.x and lhs.y != rhs.y;
	}
} // namespace daw::cookbook_class_from_array2

namespace daw::json {
	template<>
	struct json_data_contract<daw::cookbook_class_from_array2::Point> {
		using type =
		  json_ordered_member_list<double,
		                           ordered_json_member<5, json_number<no_name>>>;

		static inline auto
		to_json_data( daw::cookbook_class_from_array2::Point const &p ) {
			return std::forward_as_tuple( p.x, p.y );
		}
	};
} // namespace daw::json

int main( int argc, char **argv )
#ifdef DAW_USE_JSON_EXCEPTIONS
  try
#endif
{
	if( argc <= 1 ) {
		puts( "Must supply path to cookbook_class_from_array2.json file\n" );
		exit( EXIT_FAILURE );
	}
	auto data = *daw::read_file( argv[1] );
	puts( "Original\n" );
	puts( data.data( ) );

	auto const cls = daw::json::from_json<daw::cookbook_class_from_array2::Point>(
	  std::string_view( data.data( ), data.size( ) ) );

	std::string const str = daw::json::to_json( cls );
	puts( "Round trip\n" );
	puts( str.c_str( ) );
	auto const cls2 =
	  daw::json::from_json<daw::cookbook_class_from_array2::Point>(
	    std::string_view( str.data( ), str.size( ) ) );

	if( cls != cls2 ) {
		puts( "not exact same\n" );
	}
}
#ifdef DAW_USE_JSON_EXCEPTIONS
catch( daw::json::json_exception const &jex ) {
	std::cerr << "Exception thrown by parser: " << jex.reason( ) << std::endl;
	exit( 1 );
}
#endif
