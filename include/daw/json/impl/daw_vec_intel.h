// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/daw_json_link
//

#pragma once

#include <emmintrin.h>
#include <immintrin.h>

namespace daw::vec {
	struct set_char_t {};
	inline constexpr set_char_t set_char = set_char_t{ };

	struct mask128 {
		__m128i data;
		explicit inline mask128( ) = default;

		explicit inline mask128( char const *ptr )
		  : data( _mm_loadu_si128( reinterpret_cast<__m128i_u const *>( ptr ) ) ) {}

		explicit inline mask128( __m128i d )
		  : data( d ) {}

		explicit inline mask128( set_char_t, char v )
		  : data( _mm_set1_epi8( v ) ) {}

		explicit inline mask128( char e15,
		                         char e14,
		                         char e13,
		                         char e12,
		                         char e11,
		                         char e10,
		                         char e9,
		                         char e8,
		                         char e7,
		                         char e6,
		                         char e5,
		                         char e4,
		                         char e3,
		                         char e2,
		                         char e1,
		                         char e0 )
		  : data(
		      _mm_set_epi8( e15, e14, e13, e12, e11, e10, e9, e8, e7, e6, e5, e4, e3, e2, e1, e0 ) ) {}

		[[nodiscard]] inline int to_mask( ) const {
			return _mm_movemask_epi8( data );
		}

		friend inline mask128 operator&( mask128 const &lhs, mask128 const &rhs ) {
			return mask128( _mm_and_si128( lhs.data, rhs.data ) );
		}

		friend inline mask128 operator|( mask128 const &lhs, mask128 const &rhs ) {
			return mask128( _mm_or_si128( lhs.data, rhs.data ) );
		}

		friend inline mask128 operator~( mask128 const &lhs ) {
			return mask128(
			  _mm_xor_si128( lhs.data, _mm_cmpeq_epi32( _mm_setzero_si128( ), _mm_setzero_si128( ) ) ) );
		}
	};

	struct vec128 {
		__m128i data{ };

		explicit inline vec128( ) = default;

		explicit inline vec128( char const *ptr )
		  : data( _mm_loadu_si128( reinterpret_cast<__m128i_u const *>( ptr ) ) ) {}

		explicit inline vec128( __m128i d )
		  : data( d ) {}

		explicit inline vec128( set_char_t, char v )
		  : data( _mm_set1_epi8( v ) ) {}

		explicit inline vec128( char e15,
		                        char e14,
		                        char e13,
		                        char e12,
		                        char e11,
		                        char e10,
		                        char e9,
		                        char e8,
		                        char e7,
		                        char e6,
		                        char e5,
		                        char e4,
		                        char e3,
		                        char e2,
		                        char e1,
		                        char e0 )
		  : data(
		      _mm_set_epi8( e15, e14, e13, e12, e11, e10, e9, e8, e7, e6, e5, e4, e3, e2, e1, e0 ) ) {}

		friend inline mask128 operator<( vec128 const &lhs, vec128 const &rhs ) {
			return mask128( _mm_cmplt_epi8( lhs.data, rhs.data ) );
		}

		friend inline mask128 operator<=( vec128 const &lhs, vec128 const &rhs ) {
			return ( lhs < rhs ) | ( lhs == rhs );
		}

		friend inline mask128 operator>( vec128 const &lhs, vec128 const &rhs ) {
			return mask128( _mm_cmpgt_epi8( lhs.data, rhs.data ) );
		}

		friend inline mask128 operator>=( vec128 const &lhs, vec128 const &rhs ) {
			return ( lhs > rhs ) | ( lhs == rhs );
		}

		friend inline mask128 operator==( vec128 const &lhs, vec128 const &rhs ) {
			return mask128( _mm_cmpeq_epi8( lhs.data, rhs.data ) );
		}

		friend inline vec128 operator-( vec128 const &lhs, vec128 const &rhs ) {
			return vec128( _mm_sub_epi8( lhs.data, rhs.data ) );
		}

		friend inline vec128 operator~( vec128 const &lhs ) {
			return vec128(
			  _mm_xor_si128( lhs.data, _mm_cmpeq_epi32( _mm_setzero_si128( ), _mm_setzero_si128( ) ) ) );
		}
	};
} // namespace daw::vec
