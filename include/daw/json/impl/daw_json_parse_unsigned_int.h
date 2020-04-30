// The MIT License (MIT)
//
// Copyright (c) Darrell Wright
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files( the "Software" ), to
// deal in the Software without restriction, including without limitation the
// rights to use, copy, modify, merge, publish, distribute, sublicense, and / or
// sell copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#pragma once

#include "daw_iterator_range.h"
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

	template<typename Unsigned>
	struct unsigned_parser {
		template<JsonRangeCheck RangeChecked>
		[[nodiscard]] static constexpr std::pair<Unsigned, char const *>
		parse( char const *ptr ) {
			uintmax_t n = 0;
			auto dig = static_cast<unsigned>( *ptr ) - static_cast<unsigned>( '0' );
			int count = std::numeric_limits<Unsigned>::digits10 + 1U;
			while( dig < 10U ) {
				if constexpr( RangeChecked != JsonRangeCheck::Never ) {
					--count;
				}
				n *= 10U;
				n += static_cast<uintmax_t>( dig );
				++ptr;
				dig = static_cast<unsigned>( *ptr ) - static_cast<unsigned>( '0' );
			}
			if constexpr( RangeChecked != JsonRangeCheck::Never ) {
				daw_json_assert(
				  n <= std::numeric_limits<Unsigned>::max( ) and count >= 0,
				  "Unsigned number outside of range of unsigned numbers" );
			}
			return { daw::construct_a<Unsigned>( n ), ptr };
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
		parse_sse3( char const *ptr ) {
			uintmax_t result = 0;
			int count = std::numeric_limits<Unsigned>::digits10 + 1;
			while( is_made_of_eight_digits_fast( ptr ) ) {
				if constexpr( RangeChecked != JsonRangeCheck::Never ) {
					count -= 8;
				}
				if( not is_made_of_eight_digits_fast( ptr + 8 ) ) {
					result *= 100'000'000ULL;
					result += parse_eight_digits_unrolled( ptr );
					ptr += 8;
					break;
				}
				result *= 10'000'000'000'000'000ULL;
				result += parse_sixteen_digits_unrolled( ptr );
				ptr += 16;
			}

			auto dig = static_cast<unsigned>( *ptr - '0' );
			while( dig < 10U ) {
				result *= 10U;
				result += dig;
				++ptr;
				dig = static_cast<unsigned>( *ptr - '0' );
			}
			if constexpr( RangeChecked != JsonRangeCheck::Never ) {
				daw_json_assert( count >= 0 or
				                   result > static_cast<uintmax_t>(
				                              std::numeric_limits<Unsigned>::max( ) ),
				                 "Parsed number is out of range" );
			}
			return { daw::construct_a<Unsigned>( result ), ptr };
		}
#endif
	};
	static_assert( unsigned_parser<unsigned>::template parse<
	                 JsonRangeCheck::CheckForNarrowing>( "12345" )
	                 .first == 12345 );

} // namespace daw::json::json_details::unsignedint

namespace daw::json::json_details {

	template<typename Result, SIMDModes SIMDMode,
	         JsonRangeCheck RangeCheck = JsonRangeCheck::Never, typename First,
	         typename Last, bool IsUnCheckedInput>
	[[nodiscard]] constexpr auto
	parse_unsigned_integer2( IteratorRange<First, Last, IsUnCheckedInput> &rng ) {
		daw_json_assert_weak( rng.is_number( ), "Expecting a digit as first item" );

		using namespace daw::json::json_details::unsignedint;
		using iresult_t =
		  std::conditional_t<RangeCheck == JsonRangeCheck::CheckForNarrowing,
		                     std::uintmax_t, Result>;
		auto [v, new_p] = [rng] {
#ifdef DAW_ALLOW_SSE3
			if constexpr( SIMDMode == SIMDModes::SSE3 ) {
				return unsigned_parser<iresult_t>::template parse_sse3<RangeCheck>(
				  rng.first );
			} else {
#endif
				return unsigned_parser<iresult_t>::template parse<RangeCheck>(
				  rng.first );
#ifdef DAW_ALLOW_SSE3
			}
#endif
		}( );
		std::uint_fast8_t c = static_cast<std::uint_fast8_t>( new_p - rng.first );
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
	         SIMDModes SimdMode = SIMDModes::None, typename First, typename Last,
	         bool IsUnCheckedInput>
	[[nodiscard]] constexpr Result
	parse_unsigned_integer( IteratorRange<First, Last, IsUnCheckedInput> &rng ) {
		daw_json_assert_weak( rng.is_number( ), "Expecting a digit as first item" );

		using namespace daw::json::json_details::unsignedint;
		using result_t =
		  std::conditional_t<RangeCheck == JsonRangeCheck::CheckForNarrowing or
		                       std::is_enum_v<Result>,
		                     std::uintmax_t, Result>;
		auto [result, ptr] = [&] {
#ifdef DAW_ALLOW_SSE3
			if constexpr( SimdMode == SIMDModes::SSE3 ) {
				daw_json_assert_weak(
				  rng.size( ) >= parse_space_needed_v<SimdMode>,
				  "Insufficient space to parse number in SSE3 Mode" );
				return unsigned_parser<result_t>::template parse_sse3<RangeCheck>(
				  rng.first );
			} else {
#endif
				return unsigned_parser<result_t>::template parse<RangeCheck>(
				  rng.first );
#ifdef DAW_ALLOW_SSE3
			}
#endif
		}( );
		rng.first = ptr;

		if constexpr( RangeCheck == JsonRangeCheck::CheckForNarrowing ) {
			return daw::narrow_cast<Result>( result );
		} else {
			return static_cast<Result>( result );
		}
	}

} // namespace daw::json::json_details
