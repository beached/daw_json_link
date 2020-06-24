// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/daw_json_link
//

#include <daw/json/daw_json_link.h>
#include <daw/json/daw_json_value_state.h>

struct coordinate_t {
	double x;
	double y;
	double z;

	constexpr bool operator!=( const coordinate_t &r ) const {
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
		x += from_json<double>( state["x"] );
		y += from_json<double>( state["y"] );
		z += from_json<double>( state["z"] );
	}

	return coordinate_t{ x / len, y / len, z / len };
}

int main( ) {
	auto left = calc( "{\"coordinates\":[{\"x\":1.1,\"y\":2.2,\"z\":3.3}]}" );
	auto right = coordinate_t{ 1.1, 2.2, 3.3 };
	daw_json_assert( not( left != right ), "Unexpected value" );
}
