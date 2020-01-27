// The MIT License (MIT)
//
// Copyright (c) 2019 Darrell Wright
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files( the "Software" ), to
// deal in the Software without restriction, including without limitation the
// rights to use, copy, modify, merge, publish, distribute, sublicense, and / or
// sell copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#include "daw/json/impl/daw_murmur3.h"

#include <daw/daw_benchmark.h>

static void test( daw::string_view key, std::uint32_t seed,
                  std::uint32_t expected ) {
	std::uint32_t answer = daw::murmur3_32( key, seed );
	daw::expecting( answer == expected );
}

int main( ) {
	constexpr char const t0_a[] = {0x01, 0x02, 0x03, 0x04, 0};
	constexpr std::uint32_t t0_b = daw::murmur3_details::to_u32( t0_a );
	daw::expecting( t0_b == 0x04030201 );
	test( "", 0, 0 );
	test( "", 1, 0x514E'28B7ULL );
	test( "", 0xFFFF'FFFF, 0x81F1'6F39ULL );
	test( "\xFF\xFF\xFF\xFF", 0, 0x7629'3B50ULL );

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
