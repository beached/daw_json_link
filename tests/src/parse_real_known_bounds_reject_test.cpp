// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/daw_json_link
//
// parse_real_known_bounds relies on validate_signed_first to reject a bad
// number start and leading zeros.  Check that those, and the other malformed
// numbers it checks itself, are rejected and valid numbers are not

#include "defines.h"

#include <daw/json/daw_json_link.h>

#include <cstdio>
#include <cstring>

namespace {
	/// Parse s as a known bounds number, finding the decimal point and
	/// exponent marker the way skip_number would
	double parse_known( char const *s ) {
		auto const last = s + std::strlen( s );
		char const *decimal_point = nullptr;
		char const *exponent_marker = nullptr;
		for( auto p = s; p < last; ++p ) {
			if( *p == '.' ) {
				decimal_point = p;
			} else if( *p == 'e' or *p == 'E' ) {
				exponent_marker = p;
			}
		}
		auto state = daw::json::BasicParsePolicy<>( s, last, decimal_point,
		                                            exponent_marker );
		return daw::json::json_details::parse_real_known_bounds<double>( state );
	}

#if defined( DAW_USE_EXCEPTIONS )
	bool rejects( char const *s ) {
		try {
			(void)parse_known( s );
		} catch( daw::json::json_exception const & ) { return true; }
		return false;
	}
#endif
} // namespace

int main( ) {
#if defined( DAW_USE_EXCEPTIONS )
	for( char const *s : { "", "-", "+1", "a1", "01", "01.5", "-01", "00",
	                       "-00.1", "007e3", ".5", "-.5", "1.", "1.e5", "1e",
	                       "1e+" } ) {
		test_assert( rejects( s ), s );
	}
#endif
	test_assert( parse_known( "0" ) == 0.0, "0" );
	test_assert( parse_known( "-0" ) == 0.0, "-0" );
	test_assert( parse_known( "0.5" ) == 0.5, "0.5" );
	test_assert( parse_known( "-0.5" ) == -0.5, "-0.5" );
	test_assert( parse_known( "10" ) == 10.0, "10" );
	test_assert( parse_known( "1.5e3" ) == 1500.0, "1.5e3" );
	test_assert( parse_known( "0e1" ) == 0.0, "0e1" );
	test_assert( parse_known( "123456789.123456789" ) == 123456789.123456789,
	             "123456789.123456789" );
	std::puts( "parse_real_known_bounds_reject_test passed" );
}
