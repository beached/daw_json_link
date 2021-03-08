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

#include <boost/multiprecision/cpp_dec_float.hpp>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <string>

namespace daw::json {
	template<typename Backend, boost::multiprecision::expression_template_option
	                             ExpressionTemplates>
	struct json_data_contract<
	  boost::multiprecision::number<Backend, ExpressionTemplates>> {

		using type = json_class_map<std::string>;

		static inline auto to_json_data(
		  boost::multiprecision::number<Backend, ExpressionTemplates> const
		    &value ) {
			return value.str( );
		}
	};
} // namespace daw::json

int main( int argc, char **argv )
#ifdef DAW_USE_JSON_EXCEPTIONS
  try
#endif
{
	if( argc <= 1 ) {
		puts( "Must supply path to cookbook_numbers3.json file\n" );
		exit( EXIT_FAILURE );
	}
	auto data = daw::read_file( argv[1] );
	assert( data and not data->empty( ) );

	auto numbers =
	  daw::json::from_json_array<boost::multiprecision::cpp_dec_float_100>(
	    *data );

	assert( not numbers.empty( ) );
	auto new_json_data = daw::json::to_json( numbers );
	puts( new_json_data.c_str( ) );

	auto const numbers2 =
	  daw::json::from_json_array<boost::multiprecision::cpp_dec_float_100>(
	    new_json_data );
	assert( not numbers2.empty( ) );

	test_assert( numbers.front( ) == numbers2.front( ),
	             "Unexpected round trip error" );
	test_assert( numbers.size( ) == numbers2.size( ),
	             "Unexpected round trip error" );
} catch( daw::json::json_exception const &jex ) {
	std::cerr << "Exception thrown by parser: " << jex.reason( ) << std::endl;
	exit( 1 );
}
