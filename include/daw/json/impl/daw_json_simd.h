// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/daw_json_link
//

#pragma once

#include <daw/daw_attributes.h>
#include <daw/daw_cpp_feature_check.h>
#include <daw/daw_is_constant_evaluated.h>
#include <daw/daw_span.h>

#include "daw/json/impl/version.h"

#include <array>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <type_traits>

#if defined( __x86_64__ ) or defined( _M_X64 ) or defined( __i386__ ) or \
  defined( _M_IX86 )
#include <immintrin.h>
#elif defined( __aarch64__ ) and defined( __ARM_FEATURE_AES )
#include <arm_neon.h>
#endif

#if ( defined( __ARM_NEON ) or defined( __ARM_NEON__ ) )
#include "daw/json/impl/daw_json_simd_neon.h"
#endif

#if ( defined( __AVX512F__ ) and defined( __AVX512BW__ ) ) or \
  defined( _M_AVX512 )
#include "daw/json/impl/daw_json_simd_avx512.h"
#elif defined( __AVX2__ ) or defined( _M_AVX2 )
#include "daw/json/impl/daw_json_simd_avx2.h"
#elif defined( __SSE4_2__ ) or defined( __AVX__ ) or defined( _M_X64 ) or \
  ( defined( _M_IX86_FP ) and _M_IX86_FP >= 2 )
#include "daw/json/impl/daw_json_simd_sse42.h"
#endif

#if defined( DAW_JSON_HAS_NEON_SIMD )

#define DAW_JSON_HAS_SIMD 1
#define DAW_JSON_SIMD_CONSTEXPR inline

namespace daw {
	namespace simd = simd_impl::neon;
}

#elif defined( DAW_JSON_HAS_AVX512_SIMD )

#define DAW_JSON_HAS_SIMD 1
#define DAW_JSON_SIMD_CONSTEXPR inline

namespace daw {
	namespace simd = simd_impl::avx512;
}

#elif defined( DAW_JSON_HAS_AVX2_SIMD )

#define DAW_JSON_HAS_SIMD 1
#define DAW_JSON_SIMD_CONSTEXPR inline

namespace daw {
	namespace simd = simd_impl::avx2;
}

#elif defined( DAW_JSON_HAS_SSE42_SIMD )

#define DAW_JSON_HAS_SIMD 1
#define DAW_JSON_SIMD_CONSTEXPR inline

namespace daw {
	namespace simd = simd_impl::sse42;
}

#endif

#if defined( DAW_JSON_HAS_SIMD )

namespace daw::simd_impl {
	/**
	 * Pack the bits selected by selection into the low bits of the result while
	 * preserving their lane order.  This is the scalar interface used after a
	 * SIMD comparison mask has been materialized.
	 */
	[[nodiscard]] constexpr std::uint64_t
	compress_bits( std::uint64_t values, std::uint64_t selection ) noexcept {
#if defined( __BMI2__ ) and ( defined( __x86_64__ ) or defined( _M_X64 ) )
		if( not DAW_IS_CONSTANT_EVALUATED_COMPAT( ) ) {
			return _pext_u64( values, selection );
		}
#endif
		auto result = std::uint64_t{ 0 };
		auto output_bit = std::uint64_t{ 1 };
		while( selection != 0 ) {
			auto const selected_bit = selection & ( ~selection + 1U );
			if( ( values & selected_bit ) != 0 ) {
				result |= output_bit;
			}
			selection = daw::cxmath::blsr( selection );
			output_bit <<= 1U;
		}
		return result;
	}

	/**
	 * Accept the SIMD mask itself.  Keeping this overload in simd_impl lets the
	 * DAW backends add native-mask overloads without exposing their register
	 * types to JSON code.
	 */
	template<typename Mask,
	         std::enable_if_t<not std::is_integral_v<Mask>, int> = 0>
	[[nodiscard]] DAW_JSON_SIMD_CONSTEXPR std::uint64_t
	compress_bits( Mask values, std::uint64_t selection ) noexcept {
		return compress_bits( values.to_ullong( ), selection );
	}

	template<typename Mask,
	         std::enable_if_t<not std::is_integral_v<Mask>, int> = 0>
	[[nodiscard]] DAW_JSON_SIMD_CONSTEXPR std::uint64_t
	compress_bits( Mask values, Mask selection ) noexcept {
		return compress_bits( values.to_ullong( ), selection.to_ullong( ) );
	}

	/**
	 * Bit n of the result is the xor of bits 0 through n.  With the quotes of
	 * a block it marks the bytes inside of strings.  A carry-less multiply by
	 * all ones where the target has one, a shift ladder otherwise.
	 */
	[[nodiscard]] constexpr std::uint64_t
	prefix_xor( std::uint64_t bits ) noexcept {
#if defined( __PCLMUL__ ) and ( defined( __x86_64__ ) or defined( _M_X64 ) )
		if( not DAW_IS_CONSTANT_EVALUATED_COMPAT( ) ) {
			auto const product = _mm_clmulepi64_si128(
			  _mm_set_epi64x( 0, static_cast<long long>( bits ) ),
			  _mm_set1_epi8( static_cast<char>( 0xFF ) ),
			  0 );
			return static_cast<std::uint64_t>( _mm_cvtsi128_si64( product ) );
		}
#elif defined( __aarch64__ ) and defined( __ARM_FEATURE_AES )
		if( not DAW_IS_CONSTANT_EVALUATED_COMPAT( ) ) {
			return vgetq_lane_u64(
			  vreinterpretq_u64_p128( vmull_p64( bits, ~std::uint64_t{ 0 } ) ), 0 );
		}
#endif
		bits ^= bits << 1U;
		bits ^= bits << 2U;
		bits ^= bits << 4U;
		bits ^= bits << 8U;
		bits ^= bits << 16U;
		bits ^= bits << 32U;
		return bits;
	}

#if defined( __AVX512VBMI2__ )
	namespace flatten_bits_details {
		alignas( 64 ) inline constexpr std::uint8_t lane_numbers[64] = {
		  0,  1,  2,  3,  4,  5,  6,  7,  8,  9,  10, 11, 12, 13, 14, 15,
		  16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31,
		  32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47,
		  48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63 };
	} // namespace flatten_bits_details
#endif

	/// flatten_bits writes within this many entries of output
	inline constexpr std::size_t flatten_bits_capacity = 64U;

	/**
	 * Writes base + n for each set bit n of bits, lowest bit first, and returns
	 * their count.  output must have room for flatten_bits_capacity entries,
	 * those past the count have unspecified values.  Not stopping at the count
	 * keeps the stores free of branches on it.  With the event bits of a block
	 * and its offset this lists the event positions.  A byte compress of the
	 * lane numbers where the target has one, groups of eight set bits
	 * otherwise.
	 */
	DAW_ATTRIB_INLINE std::size_t flatten_bits( std::uint64_t bits,
	                                            std::uint32_t base,
	                                            std::uint32_t *output ) noexcept {
		auto const count =
		  static_cast<std::size_t>( daw::cxmath::popcount( bits ) );
#if defined( __AVX512VBMI2__ )
		auto const packed = _mm512_maskz_compress_epi8(
		  static_cast<__mmask64>( bits ),
		  _mm512_load_si512( flatten_bits_details::lane_numbers ) );
		auto const offset = _mm512_set1_epi32( static_cast<int>( base ) );
		auto const store = [&]( std::size_t quarter, __m128i lanes ) {
			_mm512_storeu_si512(
			  output + 16U * quarter,
			  _mm512_add_epi32( _mm512_cvtepu8_epi32( lanes ), offset ) );
		};
		store( 0, _mm512_castsi512_si128( packed ) );
		// Blocks rarely have more than 16 events, sparse ones should not pay
		// for the other stores
		if( count > 16U ) {
			store( 1, _mm512_extracti32x4_epi32( packed, 1 ) );
			store( 2, _mm512_extracti32x4_epi32( packed, 2 ) );
			store( 3, _mm512_extracti32x4_epi32( packed, 3 ) );
		}
#else
		// The top bit keeps the trailing zero count defined once bits runs out
		for( std::size_t entry = 0; entry < count; entry += 8U ) {
			for( std::size_t n = 0; n < 8U; ++n ) {
				auto lane =
				  static_cast<std::uint32_t>( daw::cxmath::count_trailing_zeros(
				    bits | ( std::uint64_t{ 1 } << 63U ) ) );
#if defined( __clang__ )
				// Otherwise clang combines the stores into vector code emulating the
				// trailing zero count, which is slower than the scalar count.  It
				// emits no instructions.
				__asm__( "" : "+r"( lane ) );
#endif
				output[entry + n] = base + lane;
				bits = daw::cxmath::blsr( bits );
			}
		}
#endif
		return count;
	}

	/**
	 * Lane n of the result is table[values[n] & 0x0F], a lookup of each byte
	 * by its low four bits as pshufb and tbl do.  Byte classes are tested with
	 * a lookup and a compare instead of a compare for each character.
	 */
	template<typename Simd>
	[[nodiscard]] DAW_JSON_SIMD_CONSTEXPR Simd lookup16(
	  std::array<typename Simd::value_type, 16> const &table, Simd values ) {

		return values.lookup16( table.data( ) );
	}
} // namespace daw::simd_impl

// Scans load whole aligned blocks, which can extend past either end of the
// range being scanned.  An aligned load never crosses a page so this cannot
// fault, but sanitizers report the bytes outside the range.  Under a
// sanitizer only the bytes inside the range are copied to an aligned local
// block and loaded from there.
#if defined( __SANITIZE_ADDRESS__ ) or defined( __SANITIZE_HWADDRESS__ )
#define DAW_JSON_SIMD_SAFE_ALIGNED_LOADS 1
#elif defined( __has_feature )
#if __has_feature( address_sanitizer ) or \
  __has_feature( hwaddress_sanitizer ) or __has_feature( memory_sanitizer )
#define DAW_JSON_SIMD_SAFE_ALIGNED_LOADS 1
#endif
#endif

namespace daw::json {
	inline namespace DAW_JSON_VER {
		namespace json_details {
			/// Load the aligned block at block_addr.  The block must hold at least
			/// one byte of [first_addr, last_addr), lanes outside of it have
			/// unspecified values and must be masked off by the caller.
			template<typename Simd>
			[[nodiscard]] inline Simd
			load_aligned_block( std::uintptr_t block_addr,
			                    [[maybe_unused]] std::uintptr_t first_addr,
			                    [[maybe_unused]] std::uintptr_t last_addr ) {
				using value_t = typename Simd::value_type;
				static_assert( sizeof( value_t ) == 1 );
				constexpr std::size_t block_size = Simd::size( );
#if defined( DAW_JSON_SIMD_SAFE_ALIGNED_LOADS )
				alignas( block_size ) value_t buff[block_size] = { };
				auto const lo = block_addr < first_addr ? first_addr : block_addr;
				auto const hi = last_addr < block_addr + block_size
				                  ? last_addr
				                  : block_addr + block_size;
				std::memcpy( buff + ( lo - block_addr ),
				             reinterpret_cast<void const *>( lo ),
				             hi - lo );
				value_t const *const ptr = buff;
#else
				auto const *const ptr = reinterpret_cast<value_t const *>( block_addr );
#endif
				return daw::simd::unchecked_load<Simd>( daw::span( ptr, block_size ),
				                                        daw::simd::flag_aligned );
			}
		} // namespace json_details
	} // namespace DAW_JSON_VER
} // namespace daw::json

#if defined( DAW_JSON_SIMD_SAFE_ALIGNED_LOADS )
#undef DAW_JSON_SIMD_SAFE_ALIGNED_LOADS
#endif

namespace daw::json {
	inline namespace DAW_JSON_VER {
		namespace json_details {
			struct number_span {
				char const *first = nullptr;
				char const *last = nullptr;
				char const *decimal_point = nullptr;
				char const *exponent_marker = nullptr;
			};

			struct pending_number_span {
				char const *first = nullptr;
				char const *decimal_point = nullptr;
				char const *exponent_marker = nullptr;
			};

			struct integer_span {
				char const *first = nullptr;
				char const *last = nullptr;
			};

			struct pending_integer_span {
				char const *first = nullptr;
			};
		} // namespace json_details
	} // namespace DAW_JSON_VER
} // namespace daw::json

#endif
