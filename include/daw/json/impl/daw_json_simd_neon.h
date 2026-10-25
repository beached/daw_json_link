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

#define DAW_JSON_HAS_NEON_SIMD 1

#include <arm_neon.h>

#include <cstddef>
#include <cstdint>
#include <cstring>
#include <type_traits>

namespace daw::simd_impl::neon {
	namespace details {
		[[nodiscard]] inline std::uint64_t
		to_bits( uint8x16_t comparison ) noexcept {
			alignas( 16 ) static constexpr std::uint8_t bit_values[16] = { 0x01,
			                                                               0x02,
			                                                               0x04,
			                                                               0x08,
			                                                               0x10,
			                                                               0x20,
			                                                               0x40,
			                                                               0x80,
			                                                               0x01,
			                                                               0x02,
			                                                               0x04,
			                                                               0x08,
			                                                               0x10,
			                                                               0x20,
			                                                               0x40,
			                                                               0x80 };
			auto const bits = vandq_u8( comparison, vld1q_u8( bit_values ) );
			auto const pairs = vpaddlq_u8( bits );
			auto const quads = vpaddlq_u16( pairs );
			auto const octets = vpaddlq_u32( quads );
			return vgetq_lane_u64( octets, 0 ) |
			       ( vgetq_lane_u64( octets, 1 ) << 8U );
		}
	} // namespace details

	class mask {
		uint8x16_t m_value;

	public:
		explicit mask( uint8x16_t value ) noexcept
		  : m_value( value ) {}

		[[nodiscard]] std::uint64_t to_ullong( ) const noexcept {
			return details::to_bits( m_value );
		}

		[[nodiscard]] std::uint64_t to_ullong_sparse( ) const noexcept {
			auto reduced = vorr_u8( vget_low_u8( m_value ), vget_high_u8( m_value ) );
			reduced = vpmax_u8( reduced, reduced );
			reduced = vpmax_u8( reduced, reduced );
			reduced = vpmax_u8( reduced, reduced );
			if( vget_lane_u8( reduced, 0 ) == 0 ) {
				return 0;
			}
			return details::to_bits( m_value );
		}

		[[nodiscard]] friend mask operator|( mask lhs, mask rhs ) noexcept {
			return mask( vorrq_u8( lhs.m_value, rhs.m_value ) );
		}

		[[nodiscard]] friend mask operator&( mask lhs, mask rhs ) noexcept {
			return mask( vandq_u8( lhs.m_value, rhs.m_value ) );
		}

		[[nodiscard]] friend mask operator!( mask value ) noexcept {
			return mask( vmvnq_u8( value.m_value ) );
		}
	};

	[[nodiscard]] inline std::uint64_t compress_mask( mask value ) noexcept {
		return value.to_ullong( );
	}

	[[nodiscard]] inline mask decompress_mask( std::uint64_t bits ) noexcept {
		alignas( 16 ) static constexpr std::uint8_t bit_values[16] = { 0x01,
		                                                               0x02,
		                                                               0x04,
		                                                               0x08,
		                                                               0x10,
		                                                               0x20,
		                                                               0x40,
		                                                               0x80,
		                                                               0x01,
		                                                               0x02,
		                                                               0x04,
		                                                               0x08,
		                                                               0x10,
		                                                               0x20,
		                                                               0x40,
		                                                               0x80 };
		auto const bytes =
		  vcombine_u8( vdup_n_u8( static_cast<std::uint8_t>( bits ) ),
		               vdup_n_u8( static_cast<std::uint8_t>( bits >> 8U ) ) );
		auto const values = vld1q_u8( bit_values );
		return mask( vceqq_u8( vandq_u8( bytes, values ), values ) );
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
		               "The NEON JSON SIMD implementation supports byte types" );
		static_assert(
		  std::is_integral_v<T>,
		  "The NEON JSON SIMD implementation requires an integral type" );

		uint8x16_t m_value;

		template<typename U, std::size_t MaximumSize>
		friend class vec;

		template<typename Simd, typename Range, typename Flag>
		friend Simd unchecked_load( Range values, Flag );

		template<typename Simd, typename Range, typename Flag>
		friend Simd partial_load( Range values, Flag );

		explicit vec( uint8x16_t value ) noexcept
		  : m_value( value ) {}

	public:
		using value_type = T;
		using mask_type = mask;

		explicit vec( value_type value ) noexcept
		  : m_value( vdupq_n_u8( static_cast<std::uint8_t>( value ) ) ) {}

		[[nodiscard]] static constexpr std::size_t size( ) noexcept {
			return 16;
		}

		[[nodiscard]] friend mask operator==( vec lhs, vec rhs ) noexcept {
			return mask( vceqq_u8( lhs.m_value, rhs.m_value ) );
		}

		[[nodiscard]] friend mask operator>=( vec lhs, vec rhs ) noexcept {
			return mask( vcgeq_u8( lhs.m_value, rhs.m_value ) );
		}

		[[nodiscard]] friend mask operator<=( vec lhs, vec rhs ) noexcept {
			return mask( vcleq_u8( lhs.m_value, rhs.m_value ) );
		}

		[[nodiscard]] friend vec operator|( vec lhs, vec rhs ) noexcept {
			return vec( vorrq_u8( lhs.m_value, rhs.m_value ) );
		}

		[[nodiscard]] friend vec operator&( vec lhs, vec rhs ) noexcept {
			return vec( vandq_u8( lhs.m_value, rhs.m_value ) );
		}

		/// Lane n is table[lane n & 0x0F], see daw::simd_impl::lookup16
		[[nodiscard]] vec lookup16( value_type const *table ) const noexcept {
			auto const indices = vandq_u8( m_value, vdupq_n_u8( 0x0F ) );
			auto const entries =
			  vld1q_u8( reinterpret_cast<std::uint8_t const *>( table ) );
#if defined( __aarch64__ )
			return vec( vqtbl1q_u8( entries, indices ) );
#else
			auto const halves =
			  uint8x8x2_t{ { vget_low_u8( entries ), vget_high_u8( entries ) } };
			return vec( vcombine_u8( vtbl2_u8( halves, vget_low_u8( indices ) ),
			                         vtbl2_u8( halves, vget_high_u8( indices ) ) ) );
#endif
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
		auto const *ptr = reinterpret_cast<std::uint8_t const *>( values.data( ) );
#if DAW_HAS_BUILTIN( __builtin_assume_aligned )
		if constexpr( std::is_same_v<Flag, flag_aligned_t> ) {
			ptr = static_cast<std::uint8_t const *>(
			  __builtin_assume_aligned( ptr, 16 ) );
		}
#endif
		return Simd( vld1q_u8( ptr ) );
	}

	template<typename Simd, typename Range, typename Flag = flag_default_t>
	[[nodiscard]] inline Simd partial_load( Range values, Flag = flag_default ) {
		static_assert( Simd::size( ) == 16 );
		auto result = vdupq_n_u8( 0 );
		std::memcpy( &result, values.data( ), values.size( ) );
		return Simd( result );
	}
} // namespace daw::simd_impl::neon
