// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/daw_json_link
//

#pragma once

#include "daw/json/impl/version.h"

#include "daw/json/impl/daw_json_simd.h"

#include <daw/daw_cpp_feature_check.h>
#include <daw/daw_span.h>

#if defined( DAW_HAS_MSVC_LIKE )
#include <intrin.h>
#endif

#include <cstddef>
#include <cstdint>

#if defined( DAW_JSON_HAS_SIMD )

#define DAW_JSON_HAS_SERIALIZE_STRING_SIMD 1

namespace daw::json {
	inline namespace DAW_JSON_VER {
		namespace json_details::serialize_string_simd {
			using byte_simd_t = daw::simd::vec<unsigned char>;
			inline constexpr std::size_t block_size = byte_simd_t::size( );
			static_assert( block_size <= 64 and
			                 ( block_size & ( block_size - 1U ) ) == 0,
			               "Block size must be a power of two no larger than 64" );

			/// Index of the lowest set bit.  value must not be 0
			[[nodiscard]] inline std::size_t lowest_set_bit( std::uint64_t value ) {
#if DAW_HAS_BUILTIN( __builtin_ctzll )
				return static_cast<std::size_t>( __builtin_ctzll( value ) );
#elif defined( DAW_HAS_MSVC_LIKE ) and ( defined( _M_X64 ) or defined( _M_ARM64 ) )
				unsigned long index;
				_BitScanForward64( &index, value );
				return static_cast<std::size_t>( index );
#else
				auto result = std::size_t{ 0 };
				while( ( value & 1U ) == 0 ) {
					value >>= 1U;
					++result;
				}
				return result;
#endif
			}

			/// Bit n is set when byte n of the aligned block must be escaped or
			/// is the start of a non-ASCII sequence: '"', '\\', < 0x20, >= 0x80.
			/// [first_addr, last_addr) is the part of the string, the block holds
			/// at least one byte of it.
			[[nodiscard]] inline std::uint64_t
			escape_mask( std::uintptr_t block_addr,
			             std::uintptr_t first_addr, std::uintptr_t last_addr ) {
				auto const block =
				  load_aligned_block<byte_simd_t>( block_addr, first_addr, last_addr );
				auto const needs_escape =
				  ( block == byte_simd_t( static_cast<unsigned char>( '"' ) ) ) |
				  ( block == byte_simd_t( static_cast<unsigned char>( '\\' ) ) ) |
				  ( block <= byte_simd_t( static_cast<unsigned char>( 0x1FU ) ) ) |
				  ( block >= byte_simd_t( static_cast<unsigned char>( 0x80U ) ) );
				return needs_escape.to_ullong( );
			}

			/// Finds the bytes in [first, last) that must be escaped or start a
			/// non-ASCII sequence.  Every load is of an aligned block holding at
			/// least one byte of [first, last).  The mask of the current block is
			/// kept between calls so a block with many hits is only loaded once.
			class escape_scanner {
				std::uintptr_t m_first;
				std::uintptr_t m_last;
				std::uintptr_t m_block;
				std::uint64_t m_mask;

			public:
				/// first must be less than last
				escape_scanner( char const *first, char const *last )
				  : m_first( reinterpret_cast<std::uintptr_t>( first ) )
				  , m_last( reinterpret_cast<std::uintptr_t>( last ) )
				  , m_block( m_first & ~std::uintptr_t{ block_size - 1U } )
				  , m_mask( escape_mask( m_block, m_first, m_last ) ) {}

				/// The first hit at or after pos, or last if there are none.  pos
				/// must be less than last and not before the previous pos.
				[[nodiscard]] char const *next( char const *pos ) {
					auto const pos_addr = reinterpret_cast<std::uintptr_t>( pos );
					if( pos_addr - m_block >= block_size ) {
						m_block = pos_addr & ~std::uintptr_t{ block_size - 1U };
						m_mask = escape_mask( m_block, m_first, m_last );
					}
					m_mask &= ~std::uint64_t{ 0 } << ( pos_addr - m_block );
					while( m_mask == 0 ) {
						m_block += block_size;
						if( m_block >= m_last ) {
							return reinterpret_cast<char const *>( m_last );
						}
						m_mask = escape_mask( m_block, m_first, m_last );
					}
					auto const hit_addr = m_block + lowest_set_bit( m_mask );
					if( hit_addr >= m_last ) {
						return reinterpret_cast<char const *>( m_last );
					}
					return pos + ( hit_addr - pos_addr );
				}
			};

			/// The first byte in [first, last) that must be escaped or starts a
			/// non-ASCII sequence, or last.  first must be less than last.
			[[nodiscard]] inline char const *find_next_escape( char const *first,
			                                                   char const *last ) {
				return escape_scanner( first, last ).next( first );
			}
		} // namespace json_details::serialize_string_simd
	} // namespace DAW_JSON_VER
} // namespace daw::json


#endif
