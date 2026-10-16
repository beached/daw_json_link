// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/daw_json_link
//
// Checks the SIMD string scans used when parsing against a scalar reference
// for every start alignment, with backslash runs and quotes crossing block
// boundaries and with bounds that end part way through a block.

#include "daw/json/daw_json_link.h"

#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <string>
#include <vector>

namespace {
	using namespace daw::json;
	using daw::json::json_details::mem_move_to_next_of;
	using daw::json::json_details::mem_skip_until_end_of_string;

	constexpr std::size_t max_block_size = 64;
	std::size_t failures = 0;

	struct skip_result {
		char const *pos;
		std::ptrdiff_t first_escape;
	};

	skip_result reference_skip( char const *first, char const *last ) {
		auto result = skip_result{ last, -1 };
		char const *p = first;
		while( p < last ) {
			if( *p == '"' ) {
				result.pos = p;
				return result;
			}
			if( *p == '\\' ) {
				if( result.first_escape < 0 ) {
					result.first_escape = p - first;
				}
				p += 2;
				continue;
			}
			++p;
		}
		return result;
	}

	char const *reference_find( char const *first, char const *last ) {
		while( first < last and *first != '"' and *first != '\\' ) {
			++first;
		}
		return first;
	}

	// Scans [first, last) in a heap buffer of exactly that size so reads
	// outside of it are visible to sanitizers
	void check( std::string const &content, std::size_t offset ) {
		// At least one byte so data( ) is never null
		auto buff = std::vector<char>( offset + content.size( ) + ( offset + content.size( ) == 0 ? 1U : 0U ) );
		std::copy( content.begin( ), content.end( ),
		           buff.begin( ) + static_cast<std::ptrdiff_t>( offset ) );
		char const *const first = buff.data( ) + offset;
		char const *const last = first + content.size( );
		auto const expected = reference_skip( first, last );

		auto first_escape = std::ptrdiff_t{ -1 };
		auto const actual = mem_skip_until_end_of_string<false, simd_exec_tag>(
		  first, last, first_escape );
		auto const actual_no_escape =
		  mem_skip_until_end_of_string<false, simd_exec_tag>( first, last );
		bool ok = actual.get( ) == expected.pos and
		          actual_no_escape.get( ) == expected.pos;
		// The first escape is only meaningful when the closing quote was found
		if( expected.pos != last ) {
			ok = ok and first_escape == expected.first_escape;
			first_escape = -1;
			auto const unchecked = mem_skip_until_end_of_string<true, simd_exec_tag>(
			  first, last, first_escape );
			ok = ok and unchecked.get( ) == expected.pos and
			     first_escape == expected.first_escape;
		}
		auto const found =
		  mem_move_to_next_of<false, simd_exec_tag, '"', '\\'>( first, last );
		ok = ok and found.get( ) == reference_find( first, last );
		if( not ok ) {
			++failures;
			std::printf( "FAIL offset=%zu size=%zu expected_pos=%td actual_pos=%td "
			             "expected_escape=%td actual_escape=%td content=%s\n",
			             offset, content.size( ), expected.pos - first,
			             actual.get( ) - first, expected.first_escape, first_escape,
			             content.c_str( ) );
		}
	}

	void check_all_offsets( std::string const &content ) {
		for( std::size_t offset = 0; offset < max_block_size; ++offset ) {
			check( content, offset );
		}
	}

#if defined( DAW_USE_EXCEPTIONS )
	// Strict parsing must reject a raw control character in every exec mode,
	// the SIMD string scan does not validate so strict parsing must not use it
	template<options::ExecModeTypes ExecMode>
	void check_strict_rejects_raw_newline( char const *mode_name ) {
		auto const input = std::string( "\"raw\nnewline\"" );
		bool rejected = false;
		try {
			(void)from_json<std::string>(
			  input, options::parse_flags<DAW_JSON_CONFORMANCE_FLAGS, ExecMode> );
		} catch( json_exception const & ) {
			rejected = true;
		}
		if( not rejected ) {
			++failures;
			std::printf( "FAIL strict parse accepted a raw newline in %s mode\n",
			             mode_name );
		}
	}
#endif

	std::uint32_t rng_state = 0x1234'5678U;
	std::uint32_t next_random( ) {
		rng_state ^= rng_state << 13U;
		rng_state ^= rng_state >> 17U;
		rng_state ^= rng_state << 5U;
		return rng_state;
	}
} // namespace

int main( ) {
	check_all_offsets( "" );
	check_all_offsets( "\"" );
	check_all_offsets( "\\" );
	check_all_offsets( "\\\"" );

	// A run of n backslashes followed by a quote, placed so the run ends at
	// and around each block boundary.  An even run leaves the quote unescaped.
	for( std::size_t run = 1; run <= 5; ++run ) {
		for( std::size_t prefix = 0; prefix <= 2 * max_block_size + 2; ++prefix ) {
			auto s = std::string( prefix, 'a' );
			s.append( run, '\\' );
			s += "\"bc\"";
			check( s, 0 );
			check( s, 1 );
			check( s, 7 );
		}
	}

	// Random content over a small alphabet at every alignment
	char const alphabet[] = { 'a', 'a', 'a', 'a', 'a', 'a', '\\', '\\', '"' };
	for( int n = 0; n < 400; ++n ) {
		auto const len = next_random( ) % 150U;
		auto s = std::string( );
		for( std::uint32_t i = 0; i < len; ++i ) {
			s += alphabet[next_random( ) % sizeof( alphabet )];
		}
		check_all_offsets( s );
	}

#if defined( DAW_USE_EXCEPTIONS )
	check_strict_rejects_raw_newline<options::ExecModeTypes::default_mode>(
	  "default" );
	check_strict_rejects_raw_newline<options::ExecModeTypes::compile_time>(
	  "compile_time" );
	check_strict_rejects_raw_newline<options::ExecModeTypes::runtime>(
	  "runtime" );
	check_strict_rejects_raw_newline<options::ExecModeTypes::simd>( "simd" );
#endif

	if( failures != 0 ) {
		std::printf( "%zu failures\n", failures );
		return 1;
	}
	std::puts( "parse_string_simd_test passed" );
	return 0;
}
