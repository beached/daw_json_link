// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/daw_json_link
//

#include "defines.h"

#include "daw/json/impl/daw_murmur3.h"

#include <daw/daw_benchmark.h>

static void test( daw::string_view key, std::uint32_t seed,
                  std::uint32_t expected ) {
	daw::UInt32 answer = daw::murmur3_32( key, seed );
	daw::expecting( answer == expected );
}

int main( int, char ** ) {
	DAW_CONSTEXPR char const t0_a[] = { 0x01, 0x02, 0x03, 0x04, 0 };
	DAW_CONSTEXPR daw::UInt32 t0_b = daw::to_uint32_buffer( t0_a );
	daw::expecting( t0_b == 0x04030201 );
	test( "", 0, 0 );
	test( "", 1, 0x514E'28B7ULL );
	test( "", 0xFFFF'FFFF,
	      0x81F1'6F39ULL ); // make sure your seed uses unsigned 32-bit math
	test( "\xFF\xFF\xFF\xFF", 0,
	      0x7629'3B50ULL ); // make sure 4-byte chunks use unsigned math
	test( "\x21\x43\x65\x87", 0, 0xF55B'516BULL );
	test( "\x21\x43\x65\x87", 0x5082'EDEE, 0x2362'F9DE );

	test( "aaaa", 0x9747b28c, 0x5A97808A ); // one full chunk
	test( "aaa", 0x9747b28c, 0x283E0130 );  // three characters
	test( "aa", 0x9747b28c, 0x5D211726 );   // two characters
	test( "a", 0x9747b28c, 0x7FA09EA6 );    // one character

	test( "abcd", 0x9747b28c, 0xF0478627 );
	test( "abc", 0x9747b28c, 0xC84A62DD );
	test( "ab", 0x9747b28c, 0x74875592 );
	test( "a", 0x9747b28c, 0x7FA09EA6 );

	test( "Hello, world!", 0x9747b28c, 0x24884CBA );

	// Make sure you handle UTF-8 high characters. A bcrypt implementation messed
	// this up
	test( "ππππππππ", 0x9747b28c, 0xD58063C1 ); // U+03C0: Greek Small Letter Pi

	// String of 256 characters.
	// Make sure you don't store string lengths in a char, and overflow at 255
	// bytes (as OpenBSD's canonical BCrypt implementation did)
	std::string aas;
	aas.resize( 256, 'a' );
	test( aas, 0x9747b28c, 0x37405BDC );

	test( "abc", 0, 0xB3DD93FA );
	test( "abcdbcdecdefdefgefghfghighijhijkijkljklmklmnlmnomnopnopq", 0,
	      0xEE925B90 );
}
