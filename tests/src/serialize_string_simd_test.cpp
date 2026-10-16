// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/daw_json_link
//
// Checks the SIMD escape scan used when serializing strings against a scalar
// reference for every start alignment and with escapes and non-ASCII
// sequences placed around block boundaries and at the end of the string.

#include "daw/json/daw_json_link.h"

#include <daw/daw_ensure.h>

#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <string>
#include <string_view>
#include <vector>

namespace {
	using namespace daw::json;

	constexpr std::size_t max_block_size = 64;

	char hex_digit( unsigned v ) {
		return static_cast<char>( v < 10 ? '0' + v : 'A' + ( v - 10 ) );
	}

	void append_u16( std::string &out, std::uint32_t v ) {
		out += "\\u";
		out += hex_digit( ( v >> 12U ) & 0xFU );
		out += hex_digit( ( v >> 8U ) & 0xFU );
		out += hex_digit( ( v >> 4U ) & 0xFU );
		out += hex_digit( v & 0xFU );
	}

	std::uint32_t decode_utf8( std::string_view s, std::size_t &pos ) {
		auto const c = static_cast<unsigned char>( s[pos] );
		auto const cont = [&]( std::size_t n ) {
			return static_cast<std::uint32_t>(
			  static_cast<unsigned char>( s[pos + n] ) & 0x3FU );
		};
		std::uint32_t cp;
		if( c < 0xE0U ) {
			cp = ( ( c & 0x1FU ) << 6U ) | cont( 1 );
			pos += 2;
		} else if( c < 0xF0U ) {
			cp = ( ( c & 0x0FU ) << 12U ) | ( cont( 1 ) << 6U ) | cont( 2 );
			pos += 3;
		} else {
			cp = ( ( c & 0x07U ) << 18U ) | ( cont( 1 ) << 12U ) |
			     ( cont( 2 ) << 6U ) | cont( 3 );
			pos += 4;
		}
		return cp;
	}

	// Scalar reference of the escaped string output for valid UTF-8 input
	std::string reference_escape( std::string_view s, bool only_7bit ) {
		std::string out = "\"";
		std::size_t pos = 0;
		while( pos < s.size( ) ) {
			auto const c = static_cast<unsigned char>( s[pos] );
			if( c >= 0x80U ) {
				auto const start = pos;
				auto const cp = decode_utf8( s, pos );
				if( not only_7bit ) {
					out.append( s.substr( start, pos - start ) );
				} else if( cp <= 0xFFFFU ) {
					append_u16( out, cp );
				} else {
					append_u16( out, 0xD7C0U + ( cp >> 10U ) );
					append_u16( out, 0xDC00U + ( cp & 0x3FFU ) );
				}
				continue;
			}
			++pos;
			switch( c ) {
			case '"':
				out += "\\\"";
				break;
			case '\\':
				out += "\\\\";
				break;
			case '\b':
				out += "\\b";
				break;
			case '\f':
				out += "\\f";
				break;
			case '\n':
				out += "\\n";
				break;
			case '\r':
				out += "\\r";
				break;
			case '\t':
				out += "\\t";
				break;
			default:
				if( c < 0x20U ) {
					append_u16( out, c );
				} else {
					out += static_cast<char>( c );
				}
			}
		}
		out += '"';
		return out;
	}

	std::size_t failures = 0;

	void check( std::string_view value, std::string_view what ) {
		auto const expected = reference_escape( value, false );
		auto const actual = to_json( std::string( value ) );
		if( actual != expected ) {
			++failures;
			std::printf( "FAIL %.*s\n  expected: %s\n  actual:   %s\n",
			             static_cast<int>( what.size( ) ), what.data( ),
			             expected.c_str( ), actual.c_str( ) );
		}
		auto const expected7 = reference_escape( value, true );
		auto const actual7 =
		  to_json( std::string( value ),
		           options::output_flags<
		             options::RestrictedStringOutput::OnlyAllow7bitStrings> );
		if( actual7 != expected7 ) {
			++failures;
			std::printf( "FAIL 7bit %.*s\n  expected: %s\n  actual:   %s\n",
			             static_cast<int>( what.size( ) ), what.data( ),
			             expected7.c_str( ), actual7.c_str( ) );
		}
	}

#if defined( DAW_JSON_HAS_SERIALIZE_STRING_SIMD )
	// The scan itself, compared to a byte loop.  Runs on unpadded heap
	// buffers of the exact size so reads outside the string would show up as
	// wrong results if the masking were off.
	void check_find_next_escape( std::string_view value ) {
		auto const buff = std::vector<char>( value.begin( ), value.end( ) );
		char const *const first = buff.data( );
		char const *const last = first + buff.size( );
		for( char const *p = first; p < last; ++p ) {
			char const *expected = p;
			while( expected < last ) {
				auto const c = static_cast<unsigned char>( *expected );
				if( c == '"' or c == '\\' or c < 0x20U or c >= 0x80U ) {
					break;
				}
				++expected;
			}
			auto const actual =
			  json_details::serialize_string_simd::find_next_escape( p, last );
			if( actual != expected ) {
				++failures;
				std::printf( "FAIL find_next_escape size=%zu start=%td expected=%td "
				             "actual=%td\n",
				             value.size( ), p - first, expected - first,
				             actual - first );
				return;
			}
		}
	}
#endif

	std::string_view const specials[] = {
	  "\"", "\\", "\n", "\x01", "\x1F", "\t", "/", "\x7F",
	  "\xC3\xA9",         // U+00E9, 2 bytes
	  "\xE2\x82\xAC",     // U+20AC, 3 bytes
	  "\xF0\x9F\x98\x80", // U+1F600, 4 bytes
	};
} // namespace

int main( ) {
	// A buffer with room to place the string at every alignment
	std::string backing( 4 * max_block_size + 2 * max_block_size, 'x' );
	for( std::size_t offset = 0; offset < max_block_size; ++offset ) {
		for( std::size_t len : { std::size_t{ 0 }, std::size_t{ 1 },
		                         std::size_t{ 15 }, std::size_t{ 16 },
		                         std::size_t{ 17 }, std::size_t{ 31 },
		                         std::size_t{ 32 }, std::size_t{ 33 },
		                         std::size_t{ 63 }, std::size_t{ 64 },
		                         std::size_t{ 65 }, std::size_t{ 130 } } ) {
			auto const plain = std::string_view( backing ).substr( offset, len );
			check( plain, "plain" );
		}
	}

	// One special at each position of a string, for string lengths around
	// block sizes, and the same strings at each start alignment
	for( auto special : specials ) {
		for( std::size_t len : { std::size_t{ 16 }, std::size_t{ 32 },
		                         std::size_t{ 64 }, std::size_t{ 100 } } ) {
			for( std::size_t pos = 0; pos + special.size( ) <= len; ++pos ) {
				std::string s( len, 'a' );
				s.replace( pos, special.size( ), special );
				check( s, s );
#if defined( DAW_JSON_HAS_SERIALIZE_STRING_SIMD )
				check_find_next_escape( s );
#endif
			}
		}
	}

	// Dense escapes and mixed content
	{
		std::string s;
		for( int n = 0; n < 200; ++n ) {
			s += specials[static_cast<std::size_t>( n ) % std::size( specials )];
			s.append( static_cast<std::size_t>( n % 7 ), 'b' );
		}
		for( std::size_t offset = 0; offset < max_block_size; ++offset ) {
			std::string padded( offset, 'p' );
			padded += s;
			check( std::string_view( padded ).substr( offset ), "dense" );
		}
#if defined( DAW_JSON_HAS_SERIALIZE_STRING_SIMD )
		check_find_next_escape( s );
#endif
	}

	if( failures != 0 ) {
		std::printf( "%zu failures\n", failures );
		return 1;
	}
	std::puts( "serialize_string_simd_test passed" );
	return 0;
}
