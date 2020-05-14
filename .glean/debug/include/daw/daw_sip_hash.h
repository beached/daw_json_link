// The MIT License (MIT)
//
// Copyright (c) 2018-2020 Darrell Wright
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
// Solution inspired by code from:
// 		Marek Majkowski <marek@popcount.org>
//    Samuel Neves (supercop/crypto_auth/siphash24/little)
//    djb (supercop/crypto_auth/siphash24/little2)
//    Jean-Philippe Aumasson (https://131002.net/siphash/siphash24.c)
//

#pragma once
#include <cstddef>
#include <cstdint>

#include "daw_endian.h"
#include "daw_span.h"

namespace daw {
	namespace sip_impl {
		constexpr uint64_t rotate( uint64_t value, size_t bits ) noexcept {
			return static_cast<uint64_t>(
			  static_cast<uint64_t>( value << bits ) |
			  static_cast<uint64_t>( value >> ( 64ULL - bits ) ) );
		}

		template<typename A, typename B, typename C, typename D>
		constexpr void half_round( A &a, B &b, C &c, D &d, size_t s,
		                           size_t t ) noexcept {
			a += b;
			c += d;
			b = rotate( b, s ) ^ a;
			d = rotate( d, t ) ^ c;
			a = rotate( a, 32 );
		}

		template<typename V0, typename V1, typename V2, typename V3>
		constexpr void double_round( V0 &v0, V1 &v1, V2 &v2, V3 &v3 ) noexcept {
			half_round( v0, v1, v2, v3, 13, 16 );
			half_round( v2, v1, v0, v3, 17, 21 );
			half_round( v0, v1, v2, v3, 13, 16 );
			half_round( v2, v1, v0, v3, 17, 21 );
		}

		template<typename Byte>
		constexpr uint64_t to_u64( Byte const *const ptr ) noexcept {
			static_assert( sizeof( Byte ) == 1U );
			return static_cast<uint64_t>( ptr[0] ) |
			       static_cast<uint64_t>( ptr[1] ) << 8U |
			       static_cast<uint64_t>( ptr[2] ) << 16U |
			       static_cast<uint64_t>( ptr[3] ) << 24U |
			       static_cast<uint64_t>( ptr[4] ) << 32U |
			       static_cast<uint64_t>( ptr[5] ) << 40U |
			       static_cast<uint64_t>( ptr[6] ) << 48U |
			       static_cast<uint64_t>( ptr[7] ) << 56U;
		}

		template<typename Byte>
		constexpr std::array<uint64_t, 2>
		key_to_u64( Byte const *const key ) noexcept {
			return {to_little_endian( to_u64( &key[0] ) ),
			        to_little_endian( to_u64( &key[8] ) )};
		}

		template<typename Left, typename Right>
		constexpr void set_pt( Left &pt, Right const &m ) noexcept {
			static_assert( sizeof( pt[0] ) == 1U );
			static_assert( sizeof( m[0] ) == 1U );
			using T = daw::remove_cvref_t<decltype( pt[0] )>;
			pt[0] = static_cast<T>( m[0] );
			pt[1] = static_cast<T>( m[1] );
			pt[2] = static_cast<T>( m[2] );
			pt[3] = static_cast<T>( m[3] );
		}
	} // namespace sip_impl

	template<typename Byte>
	constexpr uint64_t siphash24( Byte const *first, size_t sz,
	                              Byte const *const key ) {
		static_assert( sizeof( Byte ) == 1U );
		auto const k = sip_impl::key_to_u64( key );

		uint64_t v0 = k[0] ^ 0x736f6d6570736575ULL;
		uint64_t v1 = k[1] ^ 0x646f72616e646f6dULL;
		uint64_t v2 = k[0] ^ 0x6c7967656e657261ULL;
		uint64_t v3 = k[1] ^ 0x7465646279746573ULL;

		daw::span<char const> data_in( first, sz );
		while( data_in.size( ) >= 8 ) {
			auto mi = sip_impl::to_u64( data_in.data( ) );
			data_in.remove_prefix( 8 );
			v3 ^= mi;
			sip_impl::double_round( v0, v1, v2, v3 );
			v0 ^= mi;
		}

		std::array<uint8_t, 8> pt{};
		switch( data_in.size( ) ) {
		case 7:
			pt[6] = static_cast<uint8_t>( data_in[6] );
			[[fallthrough]];
		case 6:
			pt[5] = static_cast<uint8_t>( data_in[5] );
			[[fallthrough]];
		case 5:
			pt[4] = static_cast<uint8_t>( data_in[4] );
			[[fallthrough]];
		case 4:
			sip_impl::set_pt( pt, data_in );
			break;
		case 3:
			pt[2] = static_cast<uint8_t>( data_in[2] );
			[[fallthrough]];
		case 2:
			pt[1] = static_cast<uint8_t>( data_in[1] );
			[[fallthrough]];
		case 1:
			pt[0] = static_cast<uint8_t>( data_in[0] );
		}
		uint64_t b = static_cast<uint64_t>( sz ) << 56ULL;
		b |= to_little_endian( sip_impl::to_u64( pt.data( ) ) );

		v3 ^= b;
		sip_impl::double_round( v0, v1, v2, v3 );
		v0 ^= b;
		v2 ^= 0x0000'0000'0000'00FF;
		sip_impl::double_round( v0, v1, v2, v3 );
		sip_impl::double_round( v0, v1, v2, v3 );
		return ( v0 ^ v1 ) ^ ( v2 ^ v3 );
	}
} // namespace daw
