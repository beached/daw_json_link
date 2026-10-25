// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/daw_json_link
//

#pragma once

#include <daw/daw_cpp_feature_check.h>
#include <daw/daw_cxmath.h>

#define DAW_JSON_HAS_AVX512_SIMD 1

#include <immintrin.h>

#include <cstddef>
#include <cstdint>
#include <cstring>
#include <type_traits>

namespace daw::simd_impl::avx512 {
	class mask {
		__mmask64 m_value;

	public:
		explicit mask( __mmask64 value ) noexcept
		  : m_value( value ) {}

		[[nodiscard]] std::uint64_t to_ullong( ) const noexcept {
			return static_cast<std::uint64_t>( m_value );
		}

		[[nodiscard]] friend mask operator|( mask lhs, mask rhs ) noexcept {
			return mask( lhs.m_value | rhs.m_value );
		}

		[[nodiscard]] friend mask operator&( mask lhs, mask rhs ) noexcept {
			return mask( lhs.m_value & rhs.m_value );
		}

		[[nodiscard]] friend mask operator!( mask value ) noexcept {
			return mask( static_cast<__mmask64>( ~value.m_value ) );
		}
	};

	[[nodiscard]] inline std::uint64_t compress_mask( mask value ) noexcept {
		return value.to_ullong( );
	}

	[[nodiscard]] inline mask decompress_mask( std::uint64_t bits ) noexcept {
		return mask( static_cast<__mmask64>( bits ) );
	}

	/// value must contain at least one set bit.
	[[nodiscard]] inline std::size_t
	count_trailing_zeros( std::uint64_t value ) noexcept {
		return static_cast<std::size_t>(
		  daw::cxmath::count_trailing_zeros( value ) );
	}

	/// value must contain at least one set lane.
	[[nodiscard]] inline std::size_t count_trailing_zeros( mask value ) noexcept {
		return count_trailing_zeros( compress_mask( value ) );
	}

	/// Writes the set lane numbers in ascending order and returns their count.
	[[nodiscard]] inline std::size_t
	flatten_mask( std::uint64_t bits, std::uint8_t *output ) noexcept {
#if defined( __AVX512VBMI2__ )
		auto const lanes = _mm512_set_epi8( 63,
		                                    62,
		                                    61,
		                                    60,
		                                    59,
		                                    58,
		                                    57,
		                                    56,
		                                    55,
		                                    54,
		                                    53,
		                                    52,
		                                    51,
		                                    50,
		                                    49,
		                                    48,
		                                    47,
		                                    46,
		                                    45,
		                                    44,
		                                    43,
		                                    42,
		                                    41,
		                                    40,
		                                    39,
		                                    38,
		                                    37,
		                                    36,
		                                    35,
		                                    34,
		                                    33,
		                                    32,
		                                    31,
		                                    30,
		                                    29,
		                                    28,
		                                    27,
		                                    26,
		                                    25,
		                                    24,
		                                    23,
		                                    22,
		                                    21,
		                                    20,
		                                    19,
		                                    18,
		                                    17,
		                                    16,
		                                    15,
		                                    14,
		                                    13,
		                                    12,
		                                    11,
		                                    10,
		                                    9,
		                                    8,
		                                    7,
		                                    6,
		                                    5,
		                                    4,
		                                    3,
		                                    2,
		                                    1,
		                                    0 );
		_mm512_mask_compressstoreu_epi8(
		  output, static_cast<__mmask64>( bits ), lanes );
		return static_cast<std::size_t>( daw::cxmath::popcount( bits ) );
#else
		auto count = std::size_t{ 0 };
		while( bits != 0 ) {
			output[count++] =
			  static_cast<std::uint8_t>( count_trailing_zeros( bits ) );
			bits = daw::cxmath::blsr( bits );
		}
		return count;
#endif
	}

	template<typename T, std::size_t /*MaximumSize*/ = 64U>
	class vec {
		static_assert( sizeof( T ) == 1,
		               "The AVX-512 JSON SIMD implementation supports byte types" );
		static_assert(
		  std::is_integral_v<T>,
		  "The AVX-512 JSON SIMD implementation requires an integral type" );

		__m512i m_value;

		template<typename U, std::size_t MaximumSize>
		friend class vec;

		template<typename Simd, typename Range, typename Flag>
		friend Simd unchecked_load( Range values, Flag );

		template<typename Simd, typename Range, typename Flag>
		friend Simd partial_load( Range values, Flag );

		explicit vec( __m512i value ) noexcept
		  : m_value( value ) {}

	public:
		using value_type = T;
		using mask_type = mask;

		explicit vec( value_type value ) noexcept
		  : m_value( _mm512_set1_epi8( static_cast<char>( value ) ) ) {}

		[[nodiscard]] static constexpr std::size_t size( ) noexcept {
			return 64;
		}

		[[nodiscard]] friend mask operator==( vec lhs, vec rhs ) noexcept {
			return mask( _mm512_cmpeq_epi8_mask( lhs.m_value, rhs.m_value ) );
		}

		[[nodiscard]] friend mask operator>=( vec lhs, vec rhs ) noexcept {
			return mask(
			  _mm512_cmp_epu8_mask( lhs.m_value, rhs.m_value, _MM_CMPINT_GE ) );
		}

		[[nodiscard]] friend mask operator<=( vec lhs, vec rhs ) noexcept {
			return mask(
			  _mm512_cmp_epu8_mask( lhs.m_value, rhs.m_value, _MM_CMPINT_LE ) );
		}

		[[nodiscard]] friend vec operator|( vec lhs, vec rhs ) noexcept {
			return vec( _mm512_or_si512( lhs.m_value, rhs.m_value ) );
		}

		[[nodiscard]] friend vec operator&( vec lhs, vec rhs ) noexcept {
			return vec( _mm512_and_si512( lhs.m_value, rhs.m_value ) );
		}

		/// Lane n is table[lane n & 0x0F], see daw::simd_impl::lookup16
		[[nodiscard]] vec lookup16( value_type const *table ) const noexcept {
			auto const indices =
			  _mm512_and_si512( m_value, _mm512_set1_epi8( 0x0F ) );
			return vec(
			  _mm512_shuffle_epi8( _mm512_broadcast_i32x4( _mm_loadu_si128(
			                         reinterpret_cast<__m128i const *>( table ) ) ),
			                       indices ) );
		}
	};

	struct flag_default_t {};
	struct flag_convert_t {};
	struct flag_aligned_t {};

	inline constexpr flag_default_t flag_default = { };
	inline constexpr flag_convert_t flag_convert = { };
	inline constexpr flag_aligned_t flag_aligned = { };

	template<typename Simd, typename Range, typename Flag = flag_default_t>
	[[nodiscard]] inline Simd unchecked_load( Range values,
	                                          Flag = flag_default ) {
		static_assert( Simd::size( ) == 64 );
		auto const *ptr = reinterpret_cast<void const *>( values.data( ) );
		if constexpr( std::is_same_v<Flag, flag_aligned_t> ) {
			return Simd( _mm512_load_si512( ptr ) );
		} else {
			return Simd( _mm512_loadu_si512( ptr ) );
		}
	}

	template<typename Simd, typename Range, typename Flag = flag_default_t>
	[[nodiscard]] inline Simd partial_load( Range values, Flag = flag_default ) {
		static_assert( Simd::size( ) == 64 );
		auto result = _mm512_setzero_si512( );
		std::memcpy( &result, values.data( ), values.size( ) );
		return Simd( result );
	}
} // namespace daw::simd_impl::avx512
