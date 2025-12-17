// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/daw_json_link
//

#pragma once

#include "version.h"

#include <daw/daw_do_n.h>
#include <daw/daw_endian.h>
#include <daw/daw_string_view.h>
#include <daw/daw_uint_buffer.h>

#include <cstddef>
#include <cstdint>
#include <iterator>

namespace daw {
	using json_name_hash_t = daw::UInt64;

	namespace murmur3_details {
		[[nodiscard]] DAW_ATTRIB_FLATINLINE static constexpr daw::UInt32
		murmur3_32_scramble( daw::UInt32 k ) {
			using prime1 = daw::constant<0xcc9e'2d51_u32>;
			using prime2 = daw::constant<0x1b87'3593_u32>;
			k *= prime1::value;
			k = rotate_left<15>( k );
			k *= prime2::value;
			return k;
		}
	} // namespace murmur3_details

	template<typename T>
	inline constexpr T fnv1a_basis_v = daw::undefined_v<T>;
	template<typename T>
	inline constexpr T fnv1a_prime_v = daw::undefined_v<T>;

	template<>
	inline constexpr auto fnv1a_basis_v<daw::UInt32> = 0x811c'9dc5_u32;
	template<>
	inline constexpr auto fnv1a_prime_v<daw::UInt32> = 0x0100'0193_u32;

	template<>
	inline constexpr auto fnv1a_basis_v<daw::UInt64> = 0xcbf2'9ce4'8422'2325_u64;
	template<>
	inline constexpr auto fnv1a_prime_v<daw::UInt64> = 0x0000'0100'0000'01b3_u64;

	// Unrolled fnv1a to N elements
	template<std::size_t N, typename Hash>
	[[nodiscard]] DAW_ATTRIB_INLINE static constexpr auto
	fnv1a_N( daw::not_null<char const *> first,
	         Hash hash = fnv1a_basis_v<Hash> ) {
		daw::algorithm::do_n_arg<N>( [&]( std::size_t n ) {
			hash ^= static_cast<Hash>( static_cast<unsigned char>( first[n] ) );
			hash *= fnv1a_prime_v<Hash>;
		} );
		return hash;
	}

	template<typename Hash, bool expect_long_strings, typename StringView>
	[[nodiscard]] static constexpr Hash fnv1a( StringView key ) {
		static_assert( daw::traits::is_string_view_like_v<StringView>,
		               "Can only pass contiguous character ranges to fnv1a" );
		std::size_t len = std::size( key );
		daw::not_null ptr = std::data( key );
		auto hash = fnv1a_basis_v<Hash>;
		if constexpr( expect_long_strings ) {
			while( DAW_UNLIKELY( len >= 8 ) ) {
				hash = fnv1a_N<8>( ptr, hash );
				len -= 8;
				ptr += 8;
			}
			if( len >= 4 ) {
				hash = fnv1a_N<4>( ptr, hash );
				len -= 4;
				ptr += 4;
			}
		}
		for( std::size_t n = 0; n < len; ++n ) {
			hash ^= static_cast<unsigned char>( ptr[n] );
			hash *= fnv1a_prime_v<Hash>;
		}
		return hash;
	}

	template<bool expect_long_strings>
	[[nodiscard]] DAW_ATTRIB_INLINE static constexpr json_name_hash_t
	name_hash( daw::string_view key ) {
		if( auto const Sz = std::size( key );
		    DAW_LIKELY( Sz <= sizeof( json_name_hash_t ) ) ) {
			auto result = json_name_hash_t{ };
			auto const *ptr = std::data( key );
			for( std::size_t n = 0; n < Sz; ++n ) {
				result <<= 8U;
				result |= static_cast<unsigned char>( ptr[n] );
			}
			return result; // * fnv1a_prime_v<json_name_hash_t>; // mix it up with an
			               // fnv1a prime
		}
		return fnv1a<json_name_hash_t, expect_long_strings>( key );
	}

	template<typename StringView>
	[[nodiscard]] static constexpr daw::UInt32
	murmur3_32( StringView key, std::uint32_t seed = 0 ) {
		static_assert( daw::traits::is_string_view_like_v<StringView>,
		               "Can only pass contiguous character ranges to fnv1a" );
		daw::UInt32 h = to_uint32( seed );
		daw::UInt32 k = 0_u32;
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