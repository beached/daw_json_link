// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/daw_json_link
//

#pragma once

#include "daw_json_assert.h"

#include <daw/daw_arith_traits.h>
#include <daw/daw_cxmath.h>
#include <daw/daw_uint_buffer.h>

#include <cstddef>
#include <utility>

#ifdef DAW_ALLOW_SSE42
#include <emmintrin.h>
#include <smmintrin.h>
#include <tmmintrin.h>
#include <xmmintrin.h>
#ifdef _MSC_VER
#include <intrin.h>
#endif
#endif

namespace daw::json::json_details {
	[[nodiscard]] static inline constexpr bool
	is_made_of_eight_digits_cx( const char *ptr ) {
		// The copy to local buffer is to get the compiler to treat it like a
		// reinterpret_cast
		std::byte buff[8]{ };
		for( std::size_t n = 0; n < 8; ++n ) {
			buff[n] = static_cast<std::byte>( ptr[n] );
		}
		UInt64 val = UInt64( );

		for( std::size_t n = 0; n < 8; ++n ) {
			val |= to_uint64( buff[n] ) << ( 8 * n );
		}
		return (
		  ( ( val & 0xF0F0'F0F0'F0F0'F0F0_u64 ) |
		    ( ( ( val + 0x0606'0606'0606'0606_u64 ) & 0xF0F0'F0F0'F0F0'F0F0_u64 ) >>
		      4U ) ) == 0x3333'3333'3333'3333_u64 );
	}

	template<JsonRangeCheck RangeCheck, typename Unsigned,
	         typename MaxArithUnsigned>
	using max_unsigned_t = std::conditional_t<
	  (daw::is_integral_v<Unsigned> or std::is_enum_v<Unsigned>),
	  std::conditional_t<( sizeof( Unsigned ) > sizeof( MaxArithUnsigned ) ),
	                     Unsigned, MaxArithUnsigned>,
	  Unsigned>;

	// Constexpr'ified version from
	// https://kholdstare.github.io/technical/2020/05/26/faster-integer-parsing.html
	inline constexpr UInt64 parse_8_digits( const char *const str ) {
		auto const chunk = daw::to_uint64_buffer( str );
		// 1-byte mask trick (works on 4 pairs of single digits)
		auto const lower_digits = ( chunk & 0x0f'00'0f'00'0f'00'0f'00_u64 ) >> 8U;
		auto const upper_digits = ( chunk & 0x00'0f'00'0f'00'0f'00'0f_u64 ) * 10U;
		auto const chunk2 = lower_digits + upper_digits;

		// 2-byte mask trick (works on 2 pairs of two digits)
		auto const lower_digits2 =
		  ( chunk2 & 0x00'ff'00'00'00'ff'00'00_u64 ) >> 16U;
		auto const upper_digits2 =
		  ( chunk2 & 0x00'00'00'ff'00'00'00'ff_u64 ) * 100U;
		auto const chunk3 = lower_digits2 + upper_digits2;

		// 4-byte mask trick (works on pair of four digits)
		auto const lower_digits3 =
		  ( chunk3 & 0x00'00'ff'ff'00'00'00'00_u64 ) >> 32U;
		auto const upper_digits3 =
		  ( chunk3 & 0x00'00'00'00'00'00'ff'ff_u64 ) * 10000U;
		auto const chunk4 = lower_digits3 + upper_digits3;

		return chunk4 & 0xFFFF'FFFF_u64;
	}
	static_assert( parse_8_digits( "12345678" ) == 1234'5678_u64,
	               "8 digit parser does not work on this platform" );
	inline constexpr UInt64 parse_16_digits( const char *const str ) {
		auto const upper = parse_8_digits( str );
		auto const lower = parse_8_digits( str + 8 );
		return upper * 100'000'000_u64 + lower;
	}
	static_assert( parse_16_digits( "1234567890123456" ) == 1234567890123456_u64,
	               "16 digit parser does not work on this platform" );

	template<typename Unsigned, JsonRangeCheck RangeChecked, bool KnownBounds,
	         typename Range,
	         std::enable_if_t<KnownBounds, std::nullptr_t> = nullptr>
	[[nodiscard]] static constexpr Unsigned
	unsigned_parser( constexpr_exec_tag const &, Range &rng ) {
		// We know how many digits are in the number
		using result_t = max_unsigned_t<RangeChecked, Unsigned, UInt64>;
		static_assert( not static_cast<bool>( RangeChecked ) or
		                 std::is_same_v<result_t, UInt64>,
		               "Range checking is only supported for std integral types" );

		char const *first = rng.first;
		char const *const last = rng.last;
		result_t result = result_t( );

		while( last - first >= 16 ) {
			result *= 10'000'000'000'000'000_u64;
			result += static_cast<result_t>( parse_16_digits( first ) );
			first += 16;
		}
		if( last - first >= 8 ) {
			result *= 100'000'000_u64;
			result += static_cast<result_t>( parse_8_digits( first ) );
			first += 8;
		}
		while( first < last ) {
			result *= 10U;
			result += static_cast<unsigned>( static_cast<unsigned char>( *first ) -
			                                 static_cast<unsigned char>( '0' ) );
			++first;
		}
		if constexpr( RangeChecked != JsonRangeCheck::Never ) {
			auto const count =
			  ( daw::numeric_limits<result_t>::digits10 + 1U ) - rng.size( );
			daw_json_assert( result <= daw::numeric_limits<result_t>::max( ) and
			                   count >= 0,
			                 "Unsigned number outside of range of unsigned numbers", rng );
		}
		rng.first = first;
		if constexpr( RangeChecked == JsonRangeCheck::Never ) {
			return daw::construct_a<Unsigned>( static_cast<Unsigned>( result ) );
		} else {
			return daw::construct_a<Unsigned>( daw::narrow_cast<Unsigned>( result ) );
		}
	}

	//**************************
	template<typename Unsigned, JsonRangeCheck RangeChecked, bool KnownBounds,
	         typename Range,
	         std::enable_if_t<not KnownBounds, std::nullptr_t> = nullptr>
	[[nodiscard]] static constexpr Unsigned
	unsigned_parser( constexpr_exec_tag const &, Range &rng ) {
		// We do not know how long the string is
		using result_t = max_unsigned_t<RangeChecked, Unsigned, UInt64>;
		static_assert( not static_cast<bool>( RangeChecked ) or
		                 std::is_same_v<result_t, UInt64>,
		               "Range checking is only supported for std integral types" );
		daw_json_assert_weak( rng.size( ) > 0, "Unexpected empty range", rng );
		char const *first = rng.first;
		result_t result = result_t( );

		auto dig = static_cast<unsigned>( static_cast<unsigned char>( *first ) -
		                                  static_cast<unsigned char>( '0' ) );
		char const *const orig_first = first;
		while( dig < 10U ) {
			result *= 10U;
			result += dig;
			++first;
			dig = static_cast<unsigned>( static_cast<unsigned char>( *first ) -
			                             static_cast<unsigned char>( '0' ) );
		}
		if constexpr( RangeChecked != JsonRangeCheck::Never ) {
			auto const count =
			  static_cast<intmax_t>( daw::numeric_limits<Unsigned>::digits10 + 1 ) -
			  ( first - orig_first );
			daw_json_assert( count >= 0 and
			                   result <= static_cast<result_t>(
			                               daw::numeric_limits<Unsigned>::max( ) ),
			                 "Parsed number is out of range", rng );
		}

		rng.first = first;
		if constexpr( RangeChecked == JsonRangeCheck::Never ) {
			return daw::construct_a<Unsigned>( static_cast<Unsigned>( result ) );
		} else {
			return daw::construct_a<Unsigned>( daw::narrow_cast<Unsigned>( result ) );
		}
	}

#ifdef DAW_ALLOW_SSE42
/*
	// Adapted from
	//
	https://github.com/lemire/simdjson/blob/102262c7abe64b517a36a6049b39d95f58bf4aea/src/haswell/numberparsing.h
	static inline UInt64 parse_eight_digits_unrolled( const char *ptr ) {
	  // this actually computes *16* values so we are being wasteful.
	  static __m128i const ascii0 = _mm_set1_epi8( '0' );

	  static __m128i const mul_1_10 =
	    _mm_setr_epi8( 10, 1, 10, 1, 10, 1, 10, 1, 10, 1, 10, 1, 10, 1, 10, 1 );

	  static __m128i const mul_1_100 =
	    _mm_setr_epi16( 100, 1, 100, 1, 100, 1, 100, 1 );

	  static __m128i const mul_1_10000 =
	    _mm_setr_epi16( 10000, 1, 10000, 1, 10000, 1, 10000, 1 );

	  __m128i const input = _mm_sub_epi8(
	    _mm_loadu_si128( reinterpret_cast<__m128i const *>( ptr ) ), ascii0 );
	  __m128i const t1 = _mm_maddubs_epi16( input, mul_1_10 );
	  __m128i const t2 = _mm_madd_epi16( t1, mul_1_100 );
	  __m128i const t3 = _mm_packus_epi32( t2, t2 );
	  __m128i const t4 = _mm_madd_epi16( t3, mul_1_10000 );
	  return to_uint64( static_cast<std::uint32_t>( _mm_cvtsi128_si32(
	    t4 ) ) ); // only captures the sum of the first 8 digits, drop the rest
	}

	static inline UInt64 parse_sixteen_digits_unrolled( const char *ptr ) {
	  static __m128i const ascii0 = _mm_set1_epi8( '0' );

	  static __m128i const mul_1_10 =
	    _mm_setr_epi8( 10, 1, 10, 1, 10, 1, 10, 1, 10, 1, 10, 1, 10, 1, 10, 1 );

	  static __m128i const mul_1_100 =
	    _mm_setr_epi16( 100, 1, 100, 1, 100, 1, 100, 1 );

	  static __m128i const mul_1_10000 =
	    _mm_setr_epi16( 10000, 1, 10000, 1, 10000, 1, 10000, 1 );

	  __m128i const input = _mm_sub_epi8(
	    _mm_loadu_si128( reinterpret_cast<__m128i const *>( ptr ) ), ascii0 );
	  __m128i const t1 = _mm_maddubs_epi16( input, mul_1_10 );
	  __m128i const t2 = _mm_madd_epi16( t1, mul_1_100 );
	  __m128i const t3 = _mm_packus_epi32( t2, t2 );
	  __m128i const t4 = _mm_madd_epi16( t3, mul_1_10000 );
	  return to_uint64( _mm_cvtsi128_si64( t4 ) );
	}

	[[nodiscard]] static inline bool
	is_made_of_eight_digits_fast( const char *ptr ) {
	  UInt64 val;
	  memcpy( &val, ptr, sizeof( std::uint64_t ) );
	  return ( ( ( val & 0xF0F0F0F0F0F0F0F0_u64 ) |
	             ( ( ( val + 0x0606060606060606_u64 ) & 0xF0F0F0F0F0F0F0F0_u64 ) >>
	               4_u64 ) ) == 0x3333333333333333_u64 );
	}

	template<typename Unsigned, JsonRangeCheck RangeChecked, bool, typename Range>
	[[nodiscard]] static inline Unsigned
	unsigned_parser( sse42_exec_tag const &, Range &rng ) {
	  daw_json_assert_weak( rng.size( ) > 0, "Unexpected empty range", rng );
	  using result_t = max_unsigned_t<RangeChecked, Unsigned, UInt64>;
	  result_t result = result_t( );
	  char const *first = rng.first;
	  char const *const last = rng.last;
	  char const *const orig_first = first;
	  {
	    auto sz = last - first;
	    while( ( sz >= 8 ) & is_made_of_eight_digits_fast( first ) ) {
	      if( ( sz < 16 ) | ( not is_made_of_eight_digits_fast( first + 8 ) ) ) {
	        result *= 100'000'000_u64;
	        result +=
	          static_cast<result_t>( parse_eight_digits_unrolled( first ) );
	        first += 8;
	        break;
	      }
	      result *= 10'000'000'000'000'000_u64;
	      result +=
	        static_cast<result_t>( parse_sixteen_digits_unrolled( first ) );
	      sz -= 16;
	      first += 16;
	    }
	  }

	  auto dig = static_cast<unsigned>( static_cast<unsigned char>( *first ) -
	                                    static_cast<unsigned char>( '0' ) );
	  while( dig < 10U ) {
	    result *= 10U;
	    result += dig;
	    ++first;
	    dig = static_cast<unsigned>( static_cast<unsigned char>( *first ) -
	                                 static_cast<unsigned char>( '0' ) );
	  }
	  if constexpr( RangeChecked != JsonRangeCheck::Never ) {
	    auto const count =
	      static_cast<intmax_t>( daw::numeric_limits<Unsigned>::digits10 + 1 ) -
	      ( first - orig_first );
	    daw_json_assert( count >= 0 and
	                       result <= static_cast<result_t>(
	                                   daw::numeric_limits<Unsigned>::max( ) ),
	                     "Parsed number is out of range", rng );
	  }
	  rng.first = first;
	  if constexpr( RangeChecked == JsonRangeCheck::Never ) {
	    return daw::construct_a<Unsigned>( static_cast<Unsigned>( result ) );
	  } else {
	    return daw::construct_a<Unsigned>( daw::narrow_cast<Unsigned>( result ) );
	  }
	}
	 */
#endif
} // namespace daw::json::json_details
