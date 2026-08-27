// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/daw_json_link
//

#pragma once

#include <daw/daw_cpp_feature_check.h>

#if __has_include( <simd> )
#include <simd>
#endif

#if defined( __cpp_lib_simd ) or defined( __glibcxx_simd )

#define DAW_JSON_HAS_STD_SIMD 1

#endif

#if ( defined( __ARM_NEON ) or defined( __ARM_NEON__ ) ) and \
  DAW_CPP_VERSION >= 202002L

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
			alignas( 16 ) static constexpr std::uint8_t bit_values[16] = {
			  0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80,
			  0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80 };
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

	template<typename T, std::size_t /*MaximumSize*/>
	class vec {
		static_assert( sizeof( T ) == 1,
		               "The NEON JSON SIMD implementation supports byte types" );
		static_assert( std::is_integral_v<T>,
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
		  : m_value(
		      vdupq_n_u8( static_cast<std::uint8_t>( value ) ) ) {}

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
	};

	struct flag_default_t {};
	struct flag_convert_t {};

	inline constexpr flag_default_t flag_default = {};
	inline constexpr flag_convert_t flag_convert = {};

	template<typename Simd, typename Range, typename Flag = flag_default_t>
	[[nodiscard]] inline Simd unchecked_load(
	  Range values, Flag = flag_default ) {
		static_assert( Simd::size( ) == 16 );
		return Simd( vld1q_u8(
		  reinterpret_cast<std::uint8_t const *>( values.data( ) ) ) );
	}

	template<typename Simd, typename Range, typename Flag = flag_default_t>
	[[nodiscard]] inline Simd partial_load(
	  Range values, Flag = flag_default ) {
		static_assert( Simd::size( ) == 16 );
		auto result = vdupq_n_u8( 0 );
		std::memcpy( &result, values.data( ), values.size( ) );
		return Simd( result );
	}
} // namespace daw::simd_impl::neon

#endif

#if defined( DAW_JSON_HAS_STD_SIMD )

#define DAW_JSON_HAS_SIMD 1
#define DAW_JSON_SIMD_CONSTEXPR constexpr

namespace daw {
	namespace simd = std::simd;
}

#elif defined( DAW_JSON_HAS_NEON_SIMD )

#define DAW_JSON_HAS_SIMD 1
#define DAW_JSON_SIMD_CONSTEXPR

namespace daw {
	namespace simd = simd_impl::neon;
}

#endif
