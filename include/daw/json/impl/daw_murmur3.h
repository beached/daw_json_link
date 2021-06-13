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

#include <ciso646>
#include <cstddef>
#include <cstdint>
#include <iterator>

namespace daw {
	namespace murmur3_details {
		[[nodiscard]] DAW_ATTRIB_FLATTEN inline constexpr UInt32
		murmur3_32_scramble( UInt32 k ) {
			k *= 0xcc9e'2d51_u32;
			k = rotate_left<15>( k );
			k *= 0x1b87'3593_u32;
			return k;
		}
	} // namespace murmur3_details

	template<typename StringView>
	[[nodiscard]] DAW_ATTRIB_FLATTEN constexpr UInt32
	fnv1a_32( StringView &&key ) {
		std::size_t const len = std::size( key );
		char const *ptr = std::data( key );
		UInt32 hash = 0x811c'9dc5_u32;
		for( std::size_t n = 0; n < len; ++n ) {
			hash ^= static_cast<unsigned char>( ptr[n] );
			hash *= 0x0100'0193_u32;
		}
		return hash;
	}

	template<typename StringView>
	[[nodiscard]] constexpr UInt32 name_hash( StringView &&key,
	                                          std::uint32_t seed = 0 ) {
		(void)seed;
		auto const Sz = std::size( key );
		if( Sz <= sizeof( UInt32 ) ) {
			auto result = 0_u32;
			for( std::size_t n = 0; n < Sz; ++n ) {
				result <<= 8U;
				result |= static_cast<unsigned char>( key[n] );
			}
			return result;
		}
		return fnv1a_32( key );
	}

	template<typename StringView>
	[[nodiscard]] DAW_ATTRIB_FLATINLINE inline constexpr UInt32
	murmur3_32( StringView &&key, std::uint32_t seed = 0 ) {
		UInt32 h = to_uint32( seed );
		UInt32 k = 0_u32;
		char const *first = std::data( key );
		char const *const last = daw::data_end( key );
		while( ( last - first ) >= 4 ) {
			// Here is a source of differing results across endianness.
			// A swap here has no effects on hash properties though.
			k = daw::to_uint32_buffer( first );
			first += 4;
			h ^= murmur3_details::murmur3_32_scramble( k );
			h = rotate_left<13>( h );
			h = h * 5 + 0xe654'6b64_u32;
		}

		// Anything left over
		k = 0_u32;
		for( auto i = ( last - first ); i > 0; --i ) {
			k <<= 8U;
			k |= static_cast<unsigned char>( first[i - 1] );
		}

		h ^= murmur3_details::murmur3_32_scramble( k );

		h ^= to_uint32( std::size( key ) );
		h ^= h >> 16U;
		h *= 0x85eb'ca6b_u32;
		h ^= h >> 13U;
		h *= 0xc2b2'ae35_u32;
		h ^= h >> 16U;
		return h;
	}
} // namespace daw
