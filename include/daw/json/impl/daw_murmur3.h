// The MIT License (MIT)
//
// Copyright (c) Darrell Wright
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

#pragma once

#include <daw/daw_endian.h>
#include <daw/daw_string_view.h>

#include <cstdint>

namespace daw {
	namespace murmur3_details {
		static constexpr std::uint32_t to_u32( daw::string_view sv ) {
			std::uint32_t const c0 = static_cast<unsigned char>( sv[3] );
			std::uint32_t const c1 = static_cast<unsigned char>( sv[2] );
			std::uint32_t const c2 = static_cast<unsigned char>( sv[1] );
			std::uint32_t const c3 = static_cast<unsigned char>( sv[0] );
			std::uint32_t result = ( c0 << 24U ) | ( c1 << 16U ) | ( c2 << 8U ) | c3;
			return result;
		}

		static constexpr std::uint32_t murmur3_32_scramble( std::uint32_t k ) {
			k *= 0xcc9e'2d51ULL;
			k = ( k << 15U ) | ( k >> 17U );
			k *= 0x1b87'3593ULL;
			return k;
		}
	} // namespace murmur3_details

	constexpr std::uint32_t murmur3_32( daw::string_view key,
	                                    std::uint32_t seed = 0 ) {
		std::uint32_t h = seed;
		std::uint32_t k = 0;
		std::uint32_t const len = static_cast<uint32_t>( key.size( ) );

		auto chunk = key.pop_front( 4U );
		while( chunk.size( ) >= 4U ) {
			// Here is a source of differing results across endiannesses.
			// A swap here has no effects on hash properties though.
			k = murmur3_details::to_u32( chunk );
			h ^= murmur3_details::murmur3_32_scramble( k );
			h = ( h << 13U ) | ( h >> 19U );
			h = h * 5U + 0xe654'6b64ULL;
			chunk = key.pop_front( 4U );
		}

		// Anything left over
		k = 0U;
		while( not chunk.empty( ) ) {
			k <<= 8U;
			k |= static_cast<unsigned char>( chunk.pop_back( ) );
		}

		h ^= murmur3_details::murmur3_32_scramble( k );

		h ^= len;
		h ^= h >> 16U;
		h *= 0x85eb'ca6bULL;
		h ^= h >> 13U;
		h *= 0xc2b2'ae35ULL;
		h ^= h >> 16U;
		return h;
	}

} // namespace daw
