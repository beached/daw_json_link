// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/daw_json_link
//

#include "defines.h"

#include <daw/daw_read_file.h>
#include <daw/json/daw_json_link.h>
#include <daw/json/daw_json_value_state.h>

#include <fstream>
#include <iostream>
#include <sstream>
#include <string_view>
#include <unistd.h>

int main( ) {
	std::string const json_text = daw::read_file( "/tmp/1.json" ).value( );

	double x = 0;
	double y = 0;
	double z = 0;
	int len = 0;

	using namespace daw::json;
	auto rng = from_json<json_raw_no_name<>>(
	  json_text,
	  "coordinates",
	  options::parse_flags<options::CheckedParseMode::no> );

	auto val = json_value_state( );
	for( json_pair item : rng ) {
		val.reset( item.value );
		x += from_json<double>(
		  val["x"], options::parse_flags<options::CheckedParseMode::no> );
		y += from_json<double>(
		  val["y"], options::parse_flags<options::CheckedParseMode::no> );
		z += from_json<double>(
		  val["z"], options::parse_flags<options::CheckedParseMode::no> );
		++len;
	}
	std::cout << x / len << '\n';
	std::cout << y / len << '\n';
	std::cout << z / len << '\n';

	return EXIT_SUCCESS;
}
