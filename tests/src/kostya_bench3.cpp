// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/daw_json_link
//

#include "defines.h"

#include <daw/json/daw_json_iterator.h>
#include <daw/json/daw_json_link.h>

#include <daw/daw_read_file.h>

#include <iostream>
#include <sstream>
#include <string_view>
#include <unistd.h>
#include <vector>

struct coordinate_t {
	double x;
	double y;
	double z;

	constexpr bool operator!=( coordinate_t const &rhs ) const noexcept {
		return x != rhs.x and y != rhs.y and z != rhs.z;
	}

	friend std::ostream &operator<<( std::ostream &out,
	                                 coordinate_t const &point ) {
		out << "coordinate_t {x: " << point.x << ", y: " << point.y
		    << ", z: " << point.z << "}";
		return out;
	}
};

struct coordinates_t {
	std::vector<coordinate_t> coordinates;
};

namespace daw::json {
	template<>
	struct json_data_contract<coordinate_t> {
		constexpr inline static char const x[] = "x";
		constexpr inline static char const y[] = "y";
		constexpr inline static char const z[] = "z";
		using type =
		  json_member_list<json_number<x>, json_number<y>, json_number<z>>;
	};
} // namespace daw::json

coordinate_t calc( std::string const &text ) {
	double x = 0.0;
	double y = 0.0;
	double z = 0.0;
	std::size_t len = 0;

	using namespace daw::json;
	using range_t = json_array_range<coordinate_t, options::CheckedParseMode::no>;
	auto rng = range_t( text, "coordinates" );

	for( auto c : rng ) {
		++len;
		x += c.x;
		y += c.y;
		z += c.z;
	}

	double const dlen = static_cast<double>( len );
	return coordinate_t{ x / dlen, y / dlen, z / dlen };
}

int main( ) {
	auto left = calc( R"({"coordinates":[{"x":1.1,"y":2.2,"z":3.3}]})" );
	auto right = coordinate_t{ 1.1, 2.2, 3.3 };
	if( left != right ) {
		std::cerr << left << " != " << right << '\n';
		exit( EXIT_FAILURE );
	}

	std::string const text =
	  daw::read_file( "/tmp/1.json", daw::terminate_on_read_file_error );

	std::cout << calc( text ) << '\n';
}
