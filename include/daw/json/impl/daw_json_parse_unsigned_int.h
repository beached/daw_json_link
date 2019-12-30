// The MIT License (MIT)
//
// Copyright (c) 2019 Darrell Wright
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

#ifdef DAW_ALLOW_SSE2
#include <emmintrin.h>
#include <smmintrin.h>
#include <tmmintrin.h>
#include <xmmintrin.h>
#ifdef _MSC_VER
#include <intrin.h>
#endif
#endif

namespace daw::json::impl::unsignedint {
	namespace {
		template<typename Unsigned>
		struct unsigned_parser {
			[[nodiscard]] static constexpr std::pair<Unsigned, char const *>
			parse( char const *ptr ) {
				uintmax_t n = 0;
				auto dig = static_cast<unsigned>( *ptr ) - static_cast<unsigned>( '0' );
				while( dig < 10U ) {
					n *= 10U;
					n += static_cast<uintmax_t>( dig );
					++ptr;
					dig = static_cast<unsigned>( *ptr ) - static_cast<unsigned>( '0' );
				}
				return {daw::construct_a<Unsigned>( n ), ptr};
			}

#ifdef DAW_ALLOW_SSE2
			// Adapted from
			// https://github.com/lemire/simdjson/blob/102262c7abe64b517a36a6049b39d95f58bf4aea/src/haswell/numberparsing.h
			static inline Unsigned parse_eight_digits_unrolled( const char *ptr ) {
				// this actually computes *16* values so we are being wasteful.
				__m128i const ascii0 = _mm_set1_epi8( '0' );
				__m128i const mul_1_10 = _mm_setr_epi8( 10, 1, 10, 1, 10, 1, 10, 1, 10,
				                                        1, 10, 1, 10, 1, 10, 1 );
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
				return static_cast<Unsigned>( _mm_cvtsi128_si32(
				  t4 ) ); // only captures the sum of the first 8 digits, drop the rest
			}

			static inline bool is_made_of_eight_digits_fast( const char *ptr ) {
				uint64_t val;
				memcpy( &val, ptr, sizeof( uint64_t ) );
				// this can read up to 7 bytes beyond the buffer size, but we require
				// SIMDJSON_PADDING of padding
				// a branchy method might be faster:
				/*
				 return (( val & 0xF0F0F0F0F0F0F0F0 ) == 0x3030303030303030)
				  && (( (val + 0x0606060606060606) & 0xF0F0F0F0F0F0F0F0 ) ==
				  0x3030303030303030);
				  */
				return ( ( ( val & 0xF0F0F0F0F0F0F0F0U ) |
				           ( ( ( val + 0x0606060606060606U ) & 0xF0F0F0F0F0F0F0F0U ) >>
				             4U ) ) == 0x3333333333333333U );
			}

			[[nodiscard]] static inline std::pair<Unsigned, char const *>
			parse_sse2( char const *ptr ) {
				uintmax_t result = 0;
				while( is_made_of_eight_digits_fast( ptr ) ) {
					result *= 100'000'000ULL;
					result += parse_eight_digits_unrolled( ptr );
					ptr += 8;
				}
				auto dig = static_cast<unsigned>( *ptr - '0' );
				while( dig < 10U ) {
					result *= 10U;
					result += dig;
					++ptr;
					dig = static_cast<unsigned>( *ptr - '0' );
				}
				return {daw::construct_a<Unsigned>( result ), ptr};
			}
#endif
		};
		static_assert( unsigned_parser<unsigned>::parse( "12345" ).first == 12345 );
	} // namespace
} // namespace daw::json::impl::unsignedint

namespace daw::json::impl {
	namespace {
		template<typename Result, SIMDModes SIMDMode,
		         JsonRangeCheck RangeCheck = JsonRangeCheck::Never, typename First,
		         typename Last, bool IsTrustedInput>
		[[nodiscard]] constexpr auto parse_unsigned_integer2(
		  IteratorRange<First, Last, IsTrustedInput> &rng ) noexcept {
			daw_json_assert_untrusted( rng.is_number( ),
			                           "Expecting a digit as first item" );

			using namespace daw::json::impl::unsignedint;
			using iresult_t =
			  std::conditional_t<RangeCheck == JsonRangeCheck::CheckForNarrowing,
			                     uintmax_t, Result>;
			auto [v, new_p] = [rng] {
				if constexpr( SIMDMode == SIMDModes::SSE2 ) {
					return unsigned_parser<iresult_t>::parse_sse2( rng.first );
				}
				return unsigned_parser<iresult_t>::parse( rng.first );
			}( );
			uint_fast8_t c = static_cast<uint_fast8_t>( new_p - rng.first );
			rng.first = new_p;

			struct result_t {
				Result value;
				uint_fast8_t count;
			};

			if constexpr( RangeCheck == JsonRangeCheck::CheckForNarrowing ) {
				return result_t{daw::narrow_cast<Result>( v ), c};
			} else {
				return result_t{v, c};
			}
		}

		template<typename Result, JsonRangeCheck RangeCheck, SIMDModes SimdMode,
		         typename First, typename Last, bool IsTrustedInput>
		[[nodiscard]] constexpr Result parse_unsigned_integer(
		  IteratorRange<First, Last, IsTrustedInput> &rng ) noexcept {
			daw_json_assert_untrusted( rng.is_number( ),
			                           "Expecting a digit as first item" );

			using namespace daw::json::impl::unsignedint;
			using result_t =
			  std::conditional_t<RangeCheck == JsonRangeCheck::CheckForNarrowing or
			                       std::is_enum_v<Result>,
			                     uintmax_t, Result>;
			auto [result, ptr] = [&] {
				if constexpr( SimdMode == SIMDModes::SSE2 ) {
					return unsigned_parser<result_t>::parse_sse2( rng.first );
				} else {
					return unsigned_parser<result_t>::parse( rng.first );
				}
			}( );
			rng.first = ptr;

			if constexpr( RangeCheck == JsonRangeCheck::CheckForNarrowing ) {
				return daw::narrow_cast<Result>( result );
			} else {
				return static_cast<Result>( result );
			}
		}
	} // namespace
} // namespace daw::json::impl
