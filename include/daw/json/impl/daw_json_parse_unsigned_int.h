// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/daw_json_link
//

#pragma once

#include "daw_arith_traits.h"
#include "daw_json_assert.h"

#include <daw/daw_cxmath.h>

#include <cstddef>
#include <utility>

#ifdef DAW_ALLOW_SSE3
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
		std::byte buff[8]{};
		for( std::size_t n = 0; n < 8; ++n ) {
			buff[n] = static_cast<std::byte>( ptr[n] );
		}
		std::uint64_t val = 0;
		for( std::size_t n = 0; n < 8; ++n ) {
			val |= static_cast<std::uint64_t>( buff[n] ) << ( 8 * n );
		}
		return ( ( ( val & 0xF0F0F0F0F0F0F0F0U ) |
		           ( ( ( val + 0x0606060606060606U ) & 0xF0F0F0F0F0F0F0F0U ) >>
		             4U ) ) == 0x3333333333333333U );
	}

	template<JsonRangeCheck RangeCheck, typename Unsigned,
	         typename MaxArithUnsigned>
	using max_unsigned_t =
	  std::conditional_t<(not static_cast<bool>( RangeCheck ) and
	                      not std::is_enum_v<Unsigned>),
	                     Unsigned,
	                     std::conditional_t<std::is_integral_v<Unsigned>,
	                                        MaxArithUnsigned, Unsigned>>;

	inline constexpr std::uint32_t
	parse_eight_digits_unrolled_cx( const char *first ) {
		constexpr std::uint8_t ascii0[16] = {'0', '0', '0', '0', '0', '0',
		                                     '0', '0', '0', '0', '0', '0',
		                                     '0', '0', '0', '0'};
		constexpr std::uint8_t mul_1_10[16] = {10, 1, 10, 1, 10, 1, 10, 1,
		                                       10, 1, 10, 1, 10, 1, 10, 1};

		constexpr std::uint16_t mul_1_100[8] = {100, 1, 100, 1, 100, 1, 100, 1};

		constexpr std::uint16_t mul_1_10000[8] = {10000, 1, 10000, 1,
		                                          10000, 1, 10000, 1};

		alignas( unsigned long long ) std::uint8_t const input[16] = {
		  static_cast<std::uint8_t>( first[0] - ascii0[0] ),
		  static_cast<std::uint8_t>( first[1] - ascii0[1] ),
		  static_cast<std::uint8_t>( first[2] - ascii0[2] ),
		  static_cast<std::uint8_t>( first[3] - ascii0[3] ),
		  static_cast<std::uint8_t>( first[4] - ascii0[4] ),
		  static_cast<std::uint8_t>( first[5] - ascii0[5] ),
		  static_cast<std::uint8_t>( first[6] - ascii0[6] ),
		  static_cast<std::uint8_t>( first[7] - ascii0[7] )};

		alignas( unsigned long long ) std::uint16_t const t1[8] = {
		  static_cast<std::uint16_t>( ( input[0] * mul_1_10[0] ) +
		                              ( input[1] * mul_1_10[1] ) ),
		  static_cast<std::uint16_t>( ( input[2] * mul_1_10[2] ) +
		                              ( input[3] * mul_1_10[3] ) ),
		  static_cast<std::uint16_t>( ( input[4] * mul_1_10[4] ) +
		                              ( input[5] * mul_1_10[5] ) ),
		  static_cast<std::uint16_t>( ( input[6] * mul_1_10[6] ) +
		                              ( input[7] * mul_1_10[7] ) )};

		alignas( unsigned long long ) std::uint32_t const t3[4] = {
		  static_cast<std::uint32_t>( ( t1[0] * mul_1_100[0] ) +
		                              ( t1[1] * mul_1_100[1] ) ),
		  static_cast<std::uint32_t>( ( t1[2] * mul_1_100[2] ) +
		                              ( t1[3] * mul_1_100[3] ) ),
		  static_cast<std::uint32_t>( ( t1[4] * mul_1_100[4] ) +
		                              ( t1[5] * mul_1_100[5] ) ),
		  static_cast<std::uint32_t>( ( t1[6] * mul_1_100[6] ) +
		                              ( t1[7] * mul_1_100[7] ) )};

		return ( t3[0] * mul_1_10000[0] ) + ( t3[1] * mul_1_10000[1] );
	}

	template<typename Unsigned, JsonRangeCheck RangeChecked, bool KnownBounds,
	         typename Range,
	         std::enable_if_t<KnownBounds, std::nullptr_t> = nullptr>
	[[nodiscard]] static constexpr Unsigned
	unsigned_parser( SIMDConst<SIMDModes::None>, Range &rng ) {
		// If a uint128 is passed, it will be larger than a uintmax_t
		using result_t = max_unsigned_t<RangeChecked, Unsigned, uintmax_t>;
		static_assert( not static_cast<bool>( RangeChecked ) or
		                 std::is_same_v<result_t, uintmax_t>,
		               "Range checking is only supported for std integral types" );

		char const *first = rng.first;
		char const *const last = rng.last;
		result_t result = 0;

		while( last - first >= 8 ) {
			result *= 100'000'000ULL;
			result +=
			  static_cast<result_t>( parse_eight_digits_unrolled_cx( first ) );
			first += 8;
		}
		while( first < last ) {
			auto dig = static_cast<unsigned>( *first ) - static_cast<unsigned>( '0' );
			result *= 10U;
			result += dig;
			++first;
		}
		if constexpr( RangeChecked != JsonRangeCheck::Never ) {
			auto const count =
			  ( std::numeric_limits<result_t>::digits10 + 1U ) - rng.size( );
			daw_json_assert( result <= std::numeric_limits<result_t>::max( ) and
			                   count >= 0,
			                 "Unsigned number outside of range of unsigned numbers" );
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
	unsigned_parser( SIMDConst<SIMDModes::None>, Range &rng ) {
		// If a uint128 is passed, it will be larger than a uintmax_t
		using result_t = max_unsigned_t<RangeChecked, Unsigned, uintmax_t>;
		static_assert( not static_cast<bool>( RangeChecked ) or
		                 std::is_same_v<result_t, uintmax_t>,
		               "Range checking is only supported for std integral types" );
		daw_json_assert_weak( rng.size( ) > 0, "Unexpected empty range" );
		char const *first = rng.first;
		result_t result = 0;

		auto dig = static_cast<unsigned>( *first ) - static_cast<unsigned>( '0' );
		char const *const orig_first = first;
		while( dig < 10U ) {
			result *= 10U;
			result += static_cast<result_t>( dig );
			++first;
			dig = static_cast<unsigned>( *first ) - static_cast<unsigned>( '0' );
		}
		if constexpr( RangeChecked != JsonRangeCheck::Never ) {
			auto const count = ( std::numeric_limits<result_t>::digits10 + 1U ) -
			                   ( first - orig_first );
			daw_json_assert( result <= std::numeric_limits<result_t>::max( ) and
			                   count >= 0,
			                 "Unsigned number outside of range of unsigned numbers" );
		}
		rng.first = first;
		if constexpr( RangeChecked == JsonRangeCheck::Never ) {
			return daw::construct_a<Unsigned>( static_cast<Unsigned>( result ) );
		} else {
			return daw::construct_a<Unsigned>( daw::narrow_cast<Unsigned>( result ) );
		}
	}

#ifdef DAW_ALLOW_SSE3
	// Adapted from
	// https://github.com/lemire/simdjson/blob/102262c7abe64b517a36a6049b39d95f58bf4aea/src/haswell/numberparsing.h
	static inline std::uint32_t parse_eight_digits_unrolled( const char *ptr ) {
		// this actually computes *16* values so we are being wasteful.
		__m128i const ascii0 = _mm_set1_epi8( '0' );

		__m128i const mul_1_10 =
		  _mm_setr_epi8( 10, 1, 10, 1, 10, 1, 10, 1, 10, 1, 10, 1, 10, 1, 10, 1 );

		__m128i const mul_1_100 = _mm_setr_epi16( 100, 1, 100, 1, 100, 1, 100, 1 );

		__m128i const mul_1_10000 =
		  _mm_setr_epi16( 10000, 1, 10000, 1, 10000, 1, 10000, 1 );

		__m128i const input = _mm_sub_epi8(
		  _mm_loadu_si128( reinterpret_cast<__m128i const *>( ptr ) ), ascii0 );
		__m128i const t1 = _mm_maddubs_epi16( input, mul_1_10 );
		__m128i const t2 = _mm_madd_epi16( t1, mul_1_100 );
		__m128i const t3 = _mm_packus_epi32( t2, t2 );
		__m128i const t4 = _mm_madd_epi16( t3, mul_1_10000 );
		return static_cast<std::uint32_t>( _mm_cvtsi128_si32(
		  t4 ) ); // only captures the sum of the first 8 digits, drop the rest
	}

	static inline std::uint64_t parse_sixteen_digits_unrolled( const char *ptr ) {
		__m128i const ascii0 = _mm_set1_epi8( '0' );

		__m128i const mul_1_10 =
		  _mm_setr_epi8( 10, 1, 10, 1, 10, 1, 10, 1, 10, 1, 10, 1, 10, 1, 10, 1 );

		__m128i const mul_1_100 = _mm_setr_epi16( 100, 1, 100, 1, 100, 1, 100, 1 );

		__m128i const mul_1_10000 =
		  _mm_setr_epi16( 10000, 1, 10000, 1, 10000, 1, 10000, 1 );

		__m128i const input = _mm_sub_epi8(
		  _mm_loadu_si128( reinterpret_cast<__m128i const *>( ptr ) ), ascii0 );
		__m128i const t1 = _mm_maddubs_epi16( input, mul_1_10 );
		__m128i const t2 = _mm_madd_epi16( t1, mul_1_100 );
		__m128i const t3 = _mm_packus_epi32( t2, t2 );
		__m128i const t4 = _mm_madd_epi16( t3, mul_1_10000 );
		return static_cast<std::uint64_t>( _mm_cvtsi128_si64( t4 ) );
	}

	[[nodiscard]] static inline bool
	is_made_of_eight_digits_fast( const char *ptr ) {
		std::uint64_t val;
		memcpy( &val, ptr, sizeof( std::uint64_t ) );
		return ( ( ( val & 0xF0F0F0F0F0F0F0F0U ) |
		           ( ( ( val + 0x0606060606060606U ) & 0xF0F0F0F0F0F0F0F0U ) >>
		             4U ) ) == 0x3333333333333333U );
	}

	template<typename Unsigned, JsonRangeCheck RangeChecked, typename Range>
	[[nodiscard]] static inline Unsigned
	unsigned_parser( SIMDConst<SIMDModes::SSE3>, Range &rng ) {
		daw_json_assert_weak( rng.size( ) > 0, "Unexpected empty range" );
		uintmax_t result = 0;
		char const *first = rng.first;
		char const *const last = rng.last;
		char const *const orig_first = first;
		while( ( last - first ) >= 8 and is_made_of_eight_digits_fast( first ) ) {
			if( ( last - first ) < 16 or
			    not is_made_of_eight_digits_fast( first + 8 ) ) {
				result *= 100'000'000ULL;
				result += parse_eight_digits_unrolled( first );
				first += 8;
				break;
			}
			result *= 10'000'000'000'000'000ULL;
			result += parse_sixteen_digits_unrolled( first );
			first += 16;
		}

		auto dig = static_cast<unsigned>( *first - '0' );
		while( dig < 10U ) {
			result *= 10U;
			result += dig;
			++first;
			dig = static_cast<unsigned>( *first - '0' );
		}
		if constexpr( RangeChecked != JsonRangeCheck::Never ) {
			auto const count = ( std::numeric_limits<Unsigned>::digits10 + 1 ) -
			                   ( first - orig_first );
			daw_json_assert( count >= 0 or
			                   result > static_cast<uintmax_t>(
			                              std::numeric_limits<Unsigned>::max( ) ),
			                 "Parsed number is out of range" );
		}
		rng.first = first;
		if constexpr( RangeChecked == JsonRangeCheck::Never ) {
			return daw::construct_a<Unsigned>( static_cast<Unsigned>( result ) );
		} else {
			return daw::construct_a<Unsigned>( daw::narrow_cast<Unsigned>( result ) );
		}
	}
#else
	template<JsonRangeCheck, typename Range>
	[[maybe_unused]] static void parse( SIMDConst<SIMDModes::SSE3>,
	                                    Range const & ) {
		static_assert( std::is_same_v<Range, void>, "DAW_ALLOW_SSE3 not defined" );
	}
#endif

} // namespace daw::json::json_details
