// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/daw_json_link
//

#include "defines.h"
#include "twitter_test_json.h"

#include <daw/json/daw_json_link.h>
#include <daw/json/impl/daw_json_parse_common.h>

#include <daw/daw_benchmark.h>

#include <iostream>
#include <optional>
#include <string_view>

int main( )
#ifdef DAW_USE_EXCEPTIONS
  try
#endif
{
	DAW_CONSTEXPR std::string_view sv =
	  R"json(Sun Dec 4 12:51:18 +0000 2011")json";
	//	mo = 12, dy = 4, hr = 12, mn = 51, se=18, yr = 2011
	auto result = daw::twitter::TimestampConverter{ }( sv );
	(void)result;
}
#ifdef DAW_USE_EXCEPTIONS
catch( daw::json::json_exception const &jex ) {
	std::cerr << "Exception thrown by parser: " << jex.reason( ) << '\n';
	exit( 1 );
} catch( std::exception const &ex ) {
	std::cerr << "Unknown exception thrown during testing: " << ex.what( )
	          << '\n';
	exit( 1 );
} catch( ... ) {
	std::cerr << "Unknown exception thrown during testing\n";
	throw;
}
#endif