// Copyright (c) Darrell Wright
// Distributed under the Boost Software License, Version 1.0.

#include "twitter_test_json.h"

#include <daw/daw_do_not_optimize.h>
#include <daw/daw_read_file.h>
#include <daw/json/daw_from_json_simd.h>

#include <cstdlib>

int main( int argc, char **argv ) {
	if( argc < 2 or argc > 3 ) {
		return 1;
	}

#if defined( DAW_JSON_HAS_FROM_JSON_SIMD )
	auto const document = daw::read_file( argv[1] ).value( );
	auto const repetitions =
	  argc == 3
	    ? static_cast<std::size_t>( std::strtoull( argv[2], nullptr, 10 ) )
	    : std::size_t{ 1 };
	for( auto n = std::size_t{ 0 }; n < repetitions; ++n ) {
		auto result =
		  daw::json::from_json_simd<daw::twitter::twitter_object_t>( document );
		daw::do_not_optimize( result );
	}
#else
	(void)argv;
#endif
}
