// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/daw_json_link
//

#include <daw/json/daw_json_link.h>

#include "defines.h"

int main( ) {
	using namespace daw::json;
	auto json =
	  basic_json_value<parse_options( options::AllowEscapedNames::yes )>{
	    R"({"example.com":"justanormalstring"})" };
	json = json["example\\.com"];
	auto const parsed_value = as<std::string>( json );
	ensure( parsed_value == "justanormalstring" );
}
