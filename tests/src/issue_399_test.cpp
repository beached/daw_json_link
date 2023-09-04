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
	auto const json = json_value{ R"({"example.com":"justanormalstring"})" };
	auto const parsed_value = as<std::string>( json["example\\.com"] );
	ensure( parsed_value == "justanormalstring" );
}
