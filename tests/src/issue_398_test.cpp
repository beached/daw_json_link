// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/daw_json_link
//

#include <daw/json/daw_json_link.h>

int main( ) {
	auto json = daw::json::json_value{ R"({"food":"lasagna"})" };
	try {
		auto favourite_food = daw::json::as<std::string>( json["garfield"] );
	} catch( daw::json::json_exception const &je ) {
		if( je.reason_type( ) == daw::json::ErrorReason::UnexpectedNull ) {
			return 0;
		}
		throw;
	}
	return 1;
}
