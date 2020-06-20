// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/daw_json_link
//

#pragma once

#include <daw/daw_endian.h>
#include <daw/daw_string_view.h>

#include <cstdint>

namespace daw {
	namespace murmur3_details {
		static constexpr std::uint32_t to_u32( char const *const ptr ) {
			std::uint32_t const c0 = static_cast<unsigned char>( ptr[3] );
			std::uint32_t const c1 = static_cast<unsigned char>( ptr[2] );
			std::uint32_t const c2 = static_cast<unsigned char>( ptr[1] );
			std::uint32_t const c3 = static_cast<unsigned char>( ptr[0] );
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
		auto const len = static_cast<uint32_t>( key.size( ) );
		char const *first = key.data( );
		char const *const last = key.data( ) + key.size( );
		while( ( last - first ) >= 4U ) {
			// Here is a source of differing results across endiannesses.
			// A swap here has no effects on hash properties though.
			k = murmur3_details::to_u32( first );
			h ^= murmur3_details::murmur3_32_scramble( k );
			h = ( h << 13U ) | ( h >> 19U );
			h = h * 5U + 0xe654'6b64ULL;
			first += 4;
		}

		// Anything left over
		k = 0U;
		while( first != last ) {
			k <<= 8U;
			k |= static_cast<unsigned char>( *first++ );
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
