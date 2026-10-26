// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/daw_json_link
//
// Checks the operations daw::simd_impl adds to the SIMD backends against
// scalar references: prefix_xor and flatten_bits, whichever instruction
// implements them, mask compression/decompression, and lookup16 on every byte
// value.

#include "defines.h"

#include <daw/json/impl/daw_json_simd.h>

#include <array>
#include <cstddef>
#include <cstdint>
#include <random>
#include <string>

#if defined( DAW_JSON_HAS_SIMD )
namespace {
	constexpr std::uint64_t reference_prefix_xor( std::uint64_t bits ) {
		std::uint64_t result = 0;
		bool running = false;
		for( unsigned n = 0; n < 64U; ++n ) {
			running ^= ( ( bits >> n ) & 1U ) != 0;
			if( running ) {
				result |= std::uint64_t{ 1 } << n;
			}
		}
		return result;
	}

	static_assert( daw::simd_impl::prefix_xor( 0 ) == 0 );
	static_assert( daw::simd_impl::prefix_xor( 0b1001 ) == 0b0111 );
	static_assert( daw::simd_impl::prefix_xor( std::uint64_t{ 1 } << 63U ) ==
	               std::uint64_t{ 1 } << 63U );

	template<typename Rng>
	void check_prefix_xor( Rng &rng ) {
		for( int n = 0; n < 100'000; ++n ) {
			auto const bits = rng( );
			if( daw::simd_impl::prefix_xor( bits ) != reference_prefix_xor( bits ) ) {
				daw_ensure_error( true,
				                  "prefix_xor differs for " + std::to_string( bits ) );
			}
		}
	}

	void check_flatten_bits( std::uint64_t bits, std::uint32_t base ) {
		// Entries past count are unspecified, past the capacity are untouched
		constexpr auto guard = std::uint32_t{ 0xDEAD'BEEFU };
		std::array<std::uint32_t, daw::simd_impl::flatten_bits_capacity + 16U>
		  output{ };
		output.fill( guard );
		auto const count =
		  daw::simd_impl::flatten_bits( bits, base, output.data( ) );
		daw_ensure( count ==
		            static_cast<std::size_t>( daw::cxmath::popcount( bits ) ) );
		auto remaining = bits;
		for( std::size_t n = 0; n < count; ++n ) {
			auto const expected =
			  base + static_cast<std::uint32_t>(
			           daw::cxmath::count_trailing_zeros( remaining ) );
			if( output[n] != expected ) {
				daw_ensure_error( true,
				                  "flatten_bits differs for " + std::to_string( bits ) +
				                    " at entry " + std::to_string( n ) );
			}
			remaining = daw::cxmath::blsr( remaining );
		}
		for( auto n = daw::simd_impl::flatten_bits_capacity; n < output.size( );
		     ++n ) {
			if( output[n] != guard ) {
				daw_ensure_error( true,
				                  "flatten_bits wrote past its capacity for " +
				                    std::to_string( bits ) );
			}
		}
	}

	template<typename Rng>
	void check_flatten_bits( Rng &rng ) {
		check_flatten_bits( 0, 5U );
		check_flatten_bits( ~std::uint64_t{ 0 }, 0U );
		check_flatten_bits( ~std::uint64_t{ 0 }, 0xFFFF'FFC0U );
		check_flatten_bits( std::uint64_t{ 1 } << 63U, 64U );
		for( unsigned n = 0; n <= 64U; ++n ) {
			// Every count, which selects how many of the stores are made
			check_flatten_bits( n == 64U ? ~std::uint64_t{ 0 }
			                             : ( std::uint64_t{ 1 } << n ) - 1U,
			                    n * 64U );
		}
		auto base = std::uniform_int_distribution<std::uint32_t>( );
		for( int n = 0; n < 100'000; ++n ) {
			// Sparse, dense and uniform masks
			auto const bits = rng( );
			check_flatten_bits( bits, base( rng ) );
			check_flatten_bits( bits & rng( ) & rng( ), base( rng ) );
			check_flatten_bits( bits | rng( ), base( rng ) );
		}
	}

	template<typename Rng>
	void check_mask_conversion( Rng &rng ) {
		using simd_type = daw::simd::vec<char>;
		constexpr auto lanes = static_cast<std::size_t>( simd_type::size( ) );
		constexpr auto valid_bits = ~std::uint64_t{ 0 } >> ( 64U - lanes );

		for( int n = 0; n < 100'000; ++n ) {
			auto const expected = rng( ) & valid_bits;
			auto const decompressed = daw::simd::decompress_mask( expected );
			if( decompressed.to_ullong( ) != expected or
			    daw::simd::compress_mask( decompressed ) != expected ) {
				daw_ensure_error(
				  true, "mask conversion differs for " + std::to_string( expected ) );
			}

			std::array<std::uint8_t, 64> lanes_out{ };
			auto const count = daw::simd::flatten_mask( expected, lanes_out.data( ) );
			daw_ensure( count == static_cast<std::size_t>(
			                       daw::cxmath::popcount( expected ) ) );
			auto remaining = expected;
			for( std::size_t lane_index = 0; lane_index < count; ++lane_index ) {
				auto const lane = static_cast<std::size_t>(
				  daw::cxmath::count_trailing_zeros( remaining ) );
				daw_ensure( lanes_out[lane_index] == lane );
				remaining = daw::cxmath::blsr( remaining );
			}
			daw_ensure( remaining == 0 );

			if( expected != 0 ) {
				auto const first = static_cast<std::size_t>(
				  daw::cxmath::count_trailing_zeros( expected ) );
				daw_ensure( daw::simd::count_trailing_zeros( expected ) == first );
				daw_ensure( daw::simd::count_trailing_zeros( decompressed ) == first );
			}
		}

		if constexpr( lanes < 64U ) {
			auto const decompressed = daw::simd::decompress_mask( ~valid_bits );
			daw_ensure( daw::simd::compress_mask( decompressed ) == 0 );
		}
	}

	template<typename Rng>
	void check_lookup16( Rng &rng ) {
		using simd_type = daw::simd::vec<char>;
		constexpr std::size_t lanes =
		  static_cast<std::size_t>( simd_type::size( ) );
		auto byte = std::uniform_int_distribution<int>( 0, 255 );
		for( int n = 0; n < 20'000; ++n ) {
			auto table = std::array<char, 16>{ };
			for( auto &entry : table ) {
				entry = static_cast<char>( byte( rng ) );
			}
			char input[lanes];
			for( auto &value : input ) {
				value = static_cast<char>( byte( rng ) );
			}
			auto const values = daw::simd::unchecked_load<simd_type>(
			  daw::span<char const>( input, lanes ) );
			auto const result = daw::simd_impl::lookup16( table, values );
			// Compare a lane at a time through the masks every backend has
			for( std::size_t lane = 0; lane < lanes; ++lane ) {
				auto const expected =
				  table[static_cast<unsigned char>( input[lane] ) & 0x0FU];
				auto const matches = ( result == simd_type( expected ) ).to_ullong( );
				if( ( ( matches >> lane ) & 1U ) == 0 ) {
					daw_ensure_error(
					  true, "lookup16 differs in lane " + std::to_string( lane ) );
				}
			}
		}
	}
} // namespace

int main( ) {
	auto rng = std::mt19937_64( 0x5eed );
	check_prefix_xor( rng );
	check_flatten_bits( rng );
	check_mask_conversion( rng );
	check_lookup16( rng );
}
#else
int main( ) {}
#endif
