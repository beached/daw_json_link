// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/daw_json_link
//

#include "defines.h"

#include "twitter_test_json.h"

#include <daw/daw_parse_args.h>
#include <daw/json/daw_json_link.h>

#include <cstdlib>
#include <fstream>
#include <iostream>

extern "C" int LLVMFuzzerTestOneInput( std::uint8_t const *data,
                                       std::size_t size ) {
	if( data == nullptr or size == 0 ) {
		return 0;
	}
	auto json_doc =
	  std::string_view( reinterpret_cast<char const *>( data ), size );
#ifdef DAW_USE_EXCEPTIONS
	try {
#endif
		auto t = daw::json::from_json<daw::twitter::twitter_object_t>(
		  json_doc, daw::json::options::parse_flags<DAW_JSON_CONFORMANCE_FLAGS> );
		(void)t;
#ifdef DAW_USE_EXCEPTIONS
	} catch( ... ) {}
#endif
	return 0;
}
