// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/daw_json_link
//

#pragma once

#include <daw/daw_cpp_feature_check.h>

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
			return mask( _mm_xor_si128( value.m_value,
			                             _mm_set1_epi8( static_cast<char>( 0xFF ) ) ) );
		}
	};

	template<typename T, std::size_t /*MaximumSize*/>
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
			return _mm_xor_si128( value,
			                        _mm_set1_epi8( static_cast<char>( 0x80 ) ) );
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
	};

	struct flag_default_t {};
	struct flag_convert_t {};

	inline constexpr flag_default_t flag_default = { };
	inline constexpr flag_convert_t flag_convert = { };

	template<typename Simd, typename Range, typename Flag = flag_default_t>
	[[nodiscard]] inline Simd unchecked_load( Range values,
	                                          Flag = flag_default ) {
		static_assert( Simd::size( ) == 16 );
		return Simd( _mm_loadu_si128(
		  reinterpret_cast<__m128i const *>( values.data( ) ) ) );
	}

	template<typename Simd, typename Range, typename Flag = flag_default_t>
	[[nodiscard]] inline Simd partial_load( Range values, Flag = flag_default ) {
		static_assert( Simd::size( ) == 16 );
		auto result = _mm_setzero_si128( );
		std::memcpy( &result, values.data( ), values.size( ) );
		return Simd( result );
	}
} // namespace daw::simd_impl::sse42
