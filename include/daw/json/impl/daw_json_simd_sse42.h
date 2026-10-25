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

#define DAW_JSON_HAS_SSE42_SIMD 1

#include <nmmintrin.h>

#include <cstddef>
#include <cstdint>
#include <cstring>
#include <type_traits>

namespace daw::simd_impl::sse42 {
	class mask {
		__m128i m_value;

	public:
		explicit mask( __m128i value ) noexcept
		  : m_value( value ) {}

		[[nodiscard]] std::uint64_t to_ullong( ) const noexcept {
			return static_cast<std::uint32_t>( _mm_movemask_epi8( m_value ) );
		}

		[[nodiscard]] friend mask operator|( mask lhs, mask rhs ) noexcept {
			return mask( _mm_or_si128( lhs.m_value, rhs.m_value ) );
		}

		[[nodiscard]] friend mask operator&( mask lhs, mask rhs ) noexcept {
			return mask( _mm_and_si128( lhs.m_value, rhs.m_value ) );
		}

		[[nodiscard]] friend mask operator!( mask value ) noexcept {
			return mask( _mm_xor_si128(
			  value.m_value, _mm_set1_epi8( static_cast<char>( 0xFF ) ) ) );
		}
	};

	[[nodiscard]] inline std::uint64_t compress_mask( mask value ) noexcept {
		return value.to_ullong( );
	}

	[[nodiscard]] inline mask decompress_mask( std::uint64_t bits ) noexcept {
		auto const bytes = _mm_shuffle_epi8(
		  _mm_set1_epi32( static_cast<std::int32_t>( bits ) ),
		  _mm_set_epi64x( 0x0101010101010101LL, 0x0000000000000000LL ) );
		auto const bit_values =
		  _mm_set1_epi64x( static_cast<long long>( 0x8040201008040201ULL ) );
		return mask(
		  _mm_cmpeq_epi8( _mm_and_si128( bytes, bit_values ), bit_values ) );
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
		auto count = std::size_t{ 0 };
		bits &= 0xFFFFULL;
		while( bits != 0 ) {
			output[count++] =
			  static_cast<std::uint8_t>( count_trailing_zeros( bits ) );
			bits = daw::cxmath::blsr( bits );
		}
		return count;
	}

	template<typename T, std::size_t /*MaximumSize*/ = 16U>
	class vec {
		static_assert( sizeof( T ) == 1,
		               "The SSE4.2 JSON SIMD implementation supports byte types" );
		static_assert(
		  std::is_integral_v<T>,
		  "The SSE4.2 JSON SIMD implementation requires an integral type" );

		__m128i m_value;

		template<typename U, std::size_t MaximumSize>
		friend class vec;

		template<typename Simd, typename Range, typename Flag>
		friend Simd unchecked_load( Range values, Flag );

		template<typename Simd, typename Range, typename Flag>
		friend Simd partial_load( Range values, Flag );

		explicit vec( __m128i value ) noexcept
		  : m_value( value ) {}

		[[nodiscard]] static __m128i order_bytes( __m128i value ) noexcept {
			return _mm_xor_si128( value, _mm_set1_epi8( static_cast<char>( 0x80 ) ) );
		}

	public:
		using value_type = T;
		using mask_type = mask;

		explicit vec( value_type value ) noexcept
		  : m_value( _mm_set1_epi8( static_cast<char>( value ) ) ) {}

		[[nodiscard]] static constexpr std::size_t size( ) noexcept {
			return 16;
		}

		[[nodiscard]] friend mask operator==( vec lhs, vec rhs ) noexcept {
			return mask( _mm_cmpeq_epi8( lhs.m_value, rhs.m_value ) );
		}

		[[nodiscard]] friend mask operator>=( vec lhs, vec rhs ) noexcept {
			auto const ordered_lhs = order_bytes( lhs.m_value );
			auto const ordered_rhs = order_bytes( rhs.m_value );
			return mask( _mm_or_si128( _mm_cmpgt_epi8( ordered_lhs, ordered_rhs ),
			                           _mm_cmpeq_epi8( lhs.m_value, rhs.m_value ) ) );
		}

		[[nodiscard]] friend mask operator<=( vec lhs, vec rhs ) noexcept {
			auto const ordered_lhs = order_bytes( lhs.m_value );
			auto const ordered_rhs = order_bytes( rhs.m_value );
			return mask( _mm_or_si128( _mm_cmpgt_epi8( ordered_rhs, ordered_lhs ),
			                           _mm_cmpeq_epi8( lhs.m_value, rhs.m_value ) ) );
		}

		[[nodiscard]] friend vec operator|( vec lhs, vec rhs ) noexcept {
			return vec( _mm_or_si128( lhs.m_value, rhs.m_value ) );
		}

		[[nodiscard]] friend vec operator&( vec lhs, vec rhs ) noexcept {
			return vec( _mm_and_si128( lhs.m_value, rhs.m_value ) );
		}

		/// Lane n is table[lane n & 0x0F], see daw::simd_impl::lookup16
		[[nodiscard]] vec lookup16( value_type const *table ) const noexcept {
			auto const indices = _mm_and_si128( m_value, _mm_set1_epi8( 0x0F ) );
			return vec( _mm_shuffle_epi8(
			  _mm_loadu_si128( reinterpret_cast<__m128i const *>( table ) ),
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
		static_assert( Simd::size( ) == 16 );
		auto const *ptr = reinterpret_cast<__m128i const *>( values.data( ) );
		if constexpr( std::is_same_v<Flag, flag_aligned_t> ) {
			return Simd( _mm_load_si128( ptr ) );
		} else {
			return Simd( _mm_loadu_si128( ptr ) );
		}
	}

	template<typename Simd, typename Range, typename Flag = flag_default_t>
	[[nodiscard]] inline Simd partial_load( Range values, Flag = flag_default ) {
		static_assert( Simd::size( ) == 16 );
		auto result = _mm_setzero_si128( );
		std::memcpy( &result, values.data( ), values.size( ) );
		return Simd( result );
	}
} // namespace daw::simd_impl::sse42
