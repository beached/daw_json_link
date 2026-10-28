// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/daw_json_link
//
// Checks simd_parse_8_digits and simd_try_parse_8_digits, built on
// daw::simd::vec128, and parse_8_digits against a scalar reference for every 8
// digit string, and that simd_try_parse_8_digits rejects every non-digit.

#include "daw/json/daw_json_link.h"

#include <daw/daw_ensure.h>

#include <cstdint>
#include <cstdio>

int main( ) {
#if defined( DAW_HAS_SIMD )
	// One spare byte so the loads can be checked at an odd address
	char buffer[9] = { };
	char *const digits = buffer + 1;
	for( std::uint32_t value = 0; value < 100'000'000U; ++value ) {
		auto v = value;
		for( int n = 7; n >= 0; --n ) {
			digits[n] = static_cast<char>( '0' + v % 10U );
			v /= 10U;
		}
		daw_ensure( daw::json::json_details::simd_parse_8_digits( digits ) ==
		            value );
		daw_ensure( static_cast<std::uint64_t>(
		              daw::json::json_details::parse_8_digits( digits ) ) ==
		            value );
		std::uint32_t parsed = 0;
		daw_ensure( daw::json::json_details::simd_try_parse_8_digits( digits,
		                                                              parsed ) and
		            parsed == value );
	}
	std::puts( "simd_parse_8_digits matches for every 8 digit string" );

	// Every byte value at every position
	for( int pos = 0; pos < 8; ++pos ) {
		for( int c = 0; c < 256; ++c ) {
			for( int n = 0; n < 8; ++n ) {
				digits[n] = static_cast<char>( '1' + n );
			}
			digits[pos] = static_cast<char>( c );
			bool const is_digit = c >= '0' and c <= '9';
			std::uint32_t parsed = 0;
			daw_ensure( daw::json::json_details::simd_try_parse_8_digits(
			              digits, parsed ) == is_digit );
			daw_ensure( daw::json::json_details::is_made_of_eight_digits_cx(
			              digits ) == is_digit );
		}
	}
	std::puts( "simd_try_parse_8_digits rejects every non-digit" );
#else
	std::puts( "No SIMD backend, nothing to check" );
#endif
}
