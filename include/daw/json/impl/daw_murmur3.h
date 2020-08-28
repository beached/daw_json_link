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
#include <daw/daw_uint_buffer.h>

#include <cstdint>

namespace daw {
	namespace murmur3_details {
		[[nodiscard]] inline constexpr daw::UInt32
		murmur3_32_scramble( daw::UInt32 k ) noexcept {
			k *= 0xcc9e'2d51ULL;
			k = rotate_left<15>( k );
			k *= 0x1b87'3593ULL;
			return k;
		}
	} // namespace murmur3_details

	template<typename StringView>
	[[nodiscard]] constexpr daw::UInt32
	murmur3_32( StringView key, std::uint32_t seed = 0 ) noexcept {

		daw::UInt32 h = static_cast<daw::UInt32>( seed );
		daw::UInt32 k = daw::UInt32( );
		char const *first = std::data( key );
		char const *const last = std::data( key ) + std::size( key );
		while( ( last - first ) >= 4U ) {
			// Here is a source of differing results across endiannesses.
			// A swap here has no effects on hash properties though.
			k = daw::to_uint32_buffer( first );
			first += 4;
			h ^= murmur3_details::murmur3_32_scramble( k );
			h = rotate_left<13>( h );
			h = h * 5U + 0xe654'6b64ULL;
		}

		// Anything left over
		k = static_cast<daw::UInt32>( 0 );
		for( auto i = ( last - first ); i > 0; --i ) {
			k <<= 8U;
			k |= static_cast<unsigned char>( first[i - 1] );
		}

		h ^= murmur3_details::murmur3_32_scramble( k );

		h ^= static_cast<daw::UInt32>( std::size( key ) );
		h ^= h >> 16U;
		h *= 0x85eb'ca6bULL;
		h ^= h >> 13U;
		h *= 0xc2b2'ae35ULL;
		h ^= h >> 16U;
		return h;
	}

} // namespace daw
