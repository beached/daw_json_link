// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/daw_json_link
//
// parse_real_known_bounds reads the digit tail of a number with an 8 byte load
// that can extend past the number.  Check it against parse_real_single_pass
// for numbers that end exactly at the end of a heap allocation, so sanitizers
// see any read past it, and right before a page that cannot be read

#include "defines.h"

#include <daw/json/daw_json_link.h>

#include <cstdio>
#include <cstring>
#include <memory>
#include <random>
#include <string>

#if not defined( _WIN32 )
#include <sys/mman.h>
#endif

namespace {
	double parse_known( char const *first, char const *last ) {
		char const *decimal_point = nullptr;
		char const *exponent_marker = nullptr;
		for( auto p = first; p < last; ++p ) {
			if( *p == '.' ) {
				decimal_point = p;
			} else if( *p == 'e' or *p == 'E' ) {
				exponent_marker = p;
			}
		}
		auto state = daw::json::BasicParsePolicy<>( first, last, decimal_point,
		                                            exponent_marker );
		return daw::json::json_details::parse_real_known_bounds<double>( state );
	}

	double parse_single( char const *first, char const *last ) {
		auto state = daw::json::BasicParsePolicy<>( first, last );
		return daw::json::json_details::parse_real_single_pass<double>( state );
	}

	std::string digits( std::mt19937_64 &rng, int count, bool no_leading_zero ) {
		auto result = std::string( );
		for( int n = 0; n < count; ++n ) {
			auto digit = static_cast<char>( '0' + rng( ) % 10U );
			if( n == 0 and no_leading_zero and count > 1 and digit == '0' ) {
				digit = '1';
			}
			result += digit;
		}
		return result;
	}

	/// A random JSON number with 1-18 whole digits, an optional 1-18 digit
	/// fraction and an optional exponent
	std::string random_number( std::mt19937_64 &rng ) {
		auto result = std::string( );
		if( rng( ) % 2U == 0 ) {
			result += '-';
		}
		result += digits( rng, 1 + static_cast<int>( rng( ) % 18U ), true );
		if( rng( ) % 4U != 0 ) {
			result += '.';
			result += digits( rng, 1 + static_cast<int>( rng( ) % 18U ), false );
		}
		if( rng( ) % 4U == 0 ) {
			result += ( rng( ) % 2U == 0 ) ? 'e' : 'E';
			if( rng( ) % 2U == 0 ) {
				result += ( rng( ) % 2U == 0 ) ? '-' : '+';
			}
			result += digits( rng, 1 + static_cast<int>( rng( ) % 3U ), false );
		}
		return result;
	}

	void test_end_of_allocation( std::mt19937_64 &rng ) {
		for( int n = 0; n < 100'000; ++n ) {
			auto const number = random_number( rng );
			auto const buff = std::make_unique<char[]>( number.size( ) );
			std::memcpy( buff.get( ), number.data( ), number.size( ) );
			auto const last = buff.get( ) + number.size( );
			test_assert( parse_known( buff.get( ), last ) ==
			               parse_single( buff.get( ), last ),
			             number.c_str( ) );
		}
	}

#if not defined( _WIN32 )
	/// Every whole and fraction digit count ending right before a page that
	/// cannot be read
	void test_end_of_page( std::mt19937_64 &rng ) {
		auto *const pages = static_cast<char *>(
		  mmap( nullptr, 8192, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS,
		        -1, 0 ) );
		daw_ensure( pages != MAP_FAILED );
		daw_ensure( mprotect( pages + 4096, 4096, PROT_NONE ) == 0 );
		auto *const last = pages + 4096;
		for( int whole = 1; whole <= 20; ++whole ) {
			for( int fraction = 0; fraction <= 20; ++fraction ) {
				for( int n = 0; n < 20; ++n ) {
					auto number = digits( rng, whole, true );
					if( fraction > 0 ) {
						number += '.';
						number += digits( rng, fraction, false );
					}
					auto *const first = last - number.size( );
					std::memcpy( first, number.data( ), number.size( ) );
					test_assert( parse_known( first, last ) ==
					               parse_single( first, last ),
					             number.c_str( ) );
				}
			}
		}
		munmap( pages, 8192 );
	}
#endif
} // namespace

int main( ) {
	auto rng = std::mt19937_64( 42 );
	test_end_of_allocation( rng );
#if not defined( _WIN32 )
	test_end_of_page( rng );
#endif
	std::puts( "parse_real_known_bounds_tail_test passed" );
}
