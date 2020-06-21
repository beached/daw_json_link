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

namespace daw::json::json_details::unsignedint {
	[[nodiscard]] static inline constexpr bool
	is_made_of_eight_digits_cx( const char *ptr ) noexcept {
		// The copy to local buffer is to get the compiler to treat it like a
		// reinterpret_cast
		std::byte buff[8]{ };
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

	template<typename Unsigned>
	struct unsigned_parser {
		template<JsonRangeCheck RangeChecked>
		[[nodiscard]] static constexpr std::pair<Unsigned, char const *>
		parse( SIMDConst<SIMDModes::None>, char const *first,
		       char const *const last ) {
			using result_t =
			  std::conditional_t<daw::is_integral_v<Unsigned> and
			                       ( sizeof( Unsigned ) > sizeof( uintmax_t ) ),
			                     Unsigned, uintmax_t>;
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
				daw_json_assert(
				  result <= std::numeric_limits<result_t>::max( ) and count >= 0,
				  "Unsigned number outside of range of unsigned numbers" );
			}
			return { daw::construct_a<Unsigned>( result ), first };
		}

#ifdef DAW_ALLOW_SSE3
		// Adapted from
		// https://github.com/lemire/simdjson/blob/102262c7abe64b517a36a6049b39d95f58bf4aea/src/haswell/numberparsing.h
		static inline std::uint32_t parse_eight_digits_unrolled( const char *ptr ) {
			// this actually computes *16* values so we are being wasteful.
			__m128i const ascii0 = _mm_set1_epi8( '0' );

			__m128i const mul_1_10 =
			  _mm_setr_epi8( 10, 1, 10, 1, 10, 1, 10, 1, 10, 1, 10, 1, 10, 1, 10, 1 );

			__m128i const mul_1_100 =
			  _mm_setr_epi16( 100, 1, 100, 1, 100, 1, 100, 1 );

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

		static inline std::uint64_t
		parse_sixteen_digits_unrolled( const char *ptr ) {
			__m128i const ascii0 = _mm_set1_epi8( '0' );

			__m128i const mul_1_10 =
			  _mm_setr_epi8( 10, 1, 10, 1, 10, 1, 10, 1, 10, 1, 10, 1, 10, 1, 10, 1 );

			__m128i const mul_1_100 =
			  _mm_setr_epi16( 100, 1, 100, 1, 100, 1, 100, 1 );

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

		template<JsonRangeCheck RangeChecked>
		[[nodiscard]] static inline std::pair<Unsigned, char const *>
		parse( SIMDConst<SIMDModes::SSE3>, char const *first, char const * const last ) {
			uintmax_t result = 0;
			int count = std::numeric_limits<Unsigned>::digits10 + 1;
			while( (last-first) >= 8 and is_made_of_eight_digits_fast( first ) ) {
				if constexpr( RangeChecked != JsonRangeCheck::Never ) {
					count -= 8;
				}
				if( (last-first) < 16 or not is_made_of_eight_digits_fast( first + 8 ) ) {
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
				daw_json_assert( count >= 0 or
				                   result > static_cast<uintmax_t>(
				                              std::numeric_limits<Unsigned>::max( ) ),
				                 "Parsed number is out of range" );
			}
			return { daw::construct_a<Unsigned>( result ), first };
		}
#else
		template<JsonRangeCheck, typename CharT>
		static void parse( SIMDConst<SIMDModes::SSE3>, CharT const * ) {
			static_assert( std::is_same_v<CharT, void>,
			               "DAW_ALLOW_SSE3 not defined" );
		}
#endif
	};
	namespace test {
		namespace {
			constexpr inline daw::string_view num_str = "12345";
		}
	} // namespace test
	static_assert( unsigned_parser<unsigned>::template parse<
	                 JsonRangeCheck::CheckForNarrowing>(
	                 SIMDConst_v<SIMDModes::None>, test::num_str.begin( ),
	                 test::num_str.end( ) )
	                 .first == 12345 );

} // namespace daw::json::json_details::unsignedint

namespace daw::json::json_details {

	template<typename Result, JsonRangeCheck RangeCheck = JsonRangeCheck::Never,
	         typename Range>
	[[nodiscard]] constexpr auto parse_unsigned_integer2( Range &rng ) {
		daw_json_assert_weak( rng.is_number( ), "Expecting a digit as first item" );

		using namespace daw::json::json_details::unsignedint;
		using iresult_t =
		  std::conditional_t<RangeCheck == JsonRangeCheck::CheckForNarrowing,
		                     std::uintmax_t, Result>;

		auto [v, new_p] = unsigned_parser<iresult_t>::template parse<RangeCheck>(
		  SIMDConst_v<Range::simd_mode>, rng.first, rng.last );

		auto c = static_cast<std::uint_fast8_t>( new_p - rng.first );
		rng.first = new_p;

		struct result_t {
			Result value;
			std::uint_fast8_t count;
		};

		if constexpr( RangeCheck == JsonRangeCheck::CheckForNarrowing ) {
			return result_t{ daw::narrow_cast<Result>( v ), c };
		} else {
			return result_t{ v, c };
		}
	}

	template<typename Result, JsonRangeCheck RangeCheck = JsonRangeCheck::Never,
	         typename Range>
	[[nodiscard]] constexpr Result parse_unsigned_integer( Range &rng ) {
		daw_json_assert_weak( rng.is_number( ), "Expecting a digit as first item" );

		using namespace daw::json::json_details::unsignedint;
		using result_t = std::conditional_t<
		  RangeCheck == JsonRangeCheck::CheckForNarrowing or std::is_enum_v<Result>,
		  std::conditional_t<daw::is_integral_v<Result> and
		                       ( sizeof( Result ) > sizeof( uintmax_t ) ),
		                     Result, std::uintmax_t>,
		  Result>;

		auto [result, ptr] = unsigned_parser<result_t>::template parse<RangeCheck>(
		  SIMDConst_v<Range::simd_mode>, rng.first, rng.last );

		rng.first = ptr;

		if constexpr( RangeCheck == JsonRangeCheck::CheckForNarrowing ) {
			return daw::narrow_cast<Result>( result );
		} else {
			return static_cast<Result>( result );
		}
	} // namespace daw::json::json_details
} // namespace daw::json::json_details
