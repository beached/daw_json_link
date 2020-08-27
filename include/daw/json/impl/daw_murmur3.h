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
		inline constexpr std::uint32_t to_u32( char const *const ptr ) noexcept {
			std::uint32_t const c0 = static_cast<unsigned char>( ptr[3] );
			std::uint32_t const c1 = static_cast<unsigned char>( ptr[2] );
			std::uint32_t const c2 = static_cast<unsigned char>( ptr[1] );
			std::uint32_t const c3 = static_cast<unsigned char>( ptr[0] );
			std::uint32_t result = ( c0 << 24U ) | ( c1 << 16U ) | ( c2 << 8U ) | c3;
			return result;
		}

		static inline constexpr uint32_t rotl( std::uint32_t n, unsigned c ) {
			constexpr unsigned mask = ( CHAR_BIT * sizeof( n ) - 1 );
			c &= mask;
			return ( n << c ) | ( n >> ( ( -c ) & mask ) );
		}

		inline constexpr std::uint32_t
		murmur3_32_scramble( std::uint32_t k ) noexcept {
			k *= 0xcc9e'2d51ULL;
			rotl( k, 15 );
			k *= 0x1b87'3593ULL;
			return k;
		}
	} // namespace murmur3_details

	template<typename String>
	constexpr std::uint32_t murmur3_32( String const &key ) noexcept {
		std::uint32_t h = 0; // seed
		auto const len = static_cast<uint32_t>( key.size( ) );
		char const *first = key.data( );
		char const *const last = key.data( ) + len;
		while( ( last - first ) >= 4U ) {
			// Here is a source of differing results across endiannesses.
			// A swap here has no effects on hash properties though.
			std::uint32_t k = murmur3_details::to_u32( first );
			h ^= murmur3_details::murmur3_32_scramble( k );
			h = murmur3_details::rotl( h, 13 );
			h = h * 5U + 0xe654'6b64ULL;
			first += 4;
		}

		// Anything left over
		std::uint32_t k = 0U;
		for( auto d = ( last - first ); d > 0; --d ) {
			k <<= 8U;
			char const c = *( first + ( d - 1 ) );
			k |= static_cast<unsigned char>( c );
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
