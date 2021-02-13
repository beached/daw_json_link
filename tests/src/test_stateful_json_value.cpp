// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/daw_json_link
//

#include "defines.h"

#include <daw/daw_do_not_optimize.h>
#include <daw/daw_read_file.h>

#include <daw/json/daw_json_link.h>
#include <daw/json/daw_json_value_state.h>

#include <cstdlib>
#include <iostream>

struct coordinate_t {
	double x;
	double y;
	double z;

	DAW_CONSTEXPR bool operator!=( const coordinate_t &r ) const {
		return x != r.x and y != r.y and z != r.z;
	}
};

coordinate_t calc( std::string_view text ) {
	using namespace daw::json;
	double x = 0.0;
	double y = 0.0;
	double z = 0.0;
	int len = 0;

	auto jv = basic_json_value<NoCommentSkippingPolicyChecked>( text );
	auto state = basic_stateful_json_value<NoCommentSkippingPolicyChecked>( jv );
	jv = state["coordinates"];
	for( auto c : jv ) {
		state.reset( c.value );
		++len;
		static DAW_CONSTEXPR auto mem_x = json_member_name( "x" );
		static DAW_CONSTEXPR auto mem_y = json_member_name( "y" );
		static DAW_CONSTEXPR auto mem_z = json_member_name( "z" );
		x += from_json<double>( state[mem_x] );
		y += from_json<double>( state[mem_y] );
		z += from_json<double>( state[mem_z] );
	}

	return coordinate_t{ x / len, y / len, z / len };
}

int main( int argc, char **argv ) {
	if( argc <= 1 ) {
		std::cout << "Must supply path to test_stateful_json_value.json file\n";
		exit( EXIT_FAILURE );
	}
	auto const json_data = *daw::read_file( argv[1] );
	try {
		auto coords = calc( json_data );
		daw::do_not_optimize( coords );
		std::cout << "x: " << coords.x << " y: " << coords.y << " z: " << coords.z
		          << '\n';
	} catch( daw::json::json_exception const &jex ) {
		std::cerr << "Exception while parsing: " << to_formatted_string( jex )
		          << '\n';
		return EXIT_FAILURE;
	}
}
