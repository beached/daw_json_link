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
	try {
		auto value = json_value{ "name:, 4d2}" };
		if( value ) {}
		if( value.is_null( ) ) {}
	} catch( json_exception const & ) { return 0; }
	return 1;
}
