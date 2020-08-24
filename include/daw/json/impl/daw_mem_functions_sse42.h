// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/daw_json_link
//

#pragma once

#include "daw_exec_modes.h"

#include <daw/daw_hide.h>

#if defined( DAW_ALLOW_SSE42 )
#include <emmintrin.h>
#include <nmmintrin.h>
#include <smmintrin.h>
#include <tmmintrin.h>
#include <xmmintrin.h>
#ifdef _MSC_VER
#include <intrin.h>
#endif
#include <cstring>

namespace daw::json::json_details {
	DAW_ATTRIBUTE_FLATTEN static inline __m128i
	set_epi8_reverse( char c0, char c1 = 0, char c2 = 0, char c3 = 0, char c4 = 0,
	                  char c5 = 0, char c6 = 0, char c7 = 0, char c8 = 0,
	                  char c9 = 0, char c10 = 0, char c11 = 0, char c12 = 0,
	                  char c13 = 0, char c14 = 0, char c15 = 0 ) {
		return _mm_set_epi8( c15, c14, c13, c12, c11, c10, c9, c8, c7, c6, c5, c4,
		                     c3, c2, c1, c0 );
	}

	template<bool is_unchecked_input, char... keys>
	DAW_ATTRIBUTE_FLATTEN static inline char const *
	mem_move_to_next_of( sse42_exec_tag const &, char const *first,
	                     char const *last ) {
		static constexpr int keys_len = static_cast<int>( sizeof...( keys ) );
		static_assert( keys_len <= 16 );
		__m128i const a = set_epi8_reverse( keys... );
		static constexpr int compare_mode = _SIDD_SBYTE_OPS | _SIDD_CMP_EQUAL_ANY;

		while( last - first >= 16 ) {
			__m128i const b =
			  _mm_loadu_si128( reinterpret_cast<__m128i const *>( first ) );
			int const b_len =
			  last - first >= 16 ? 16 : static_cast<int>( last - first );
			int result = _mm_cmpestri( a, keys_len, b, b_len, compare_mode );
			first += result;
			if( result < 16 ) {
				return first;
			}
		}
		static constexpr auto is_eq = []( char c ) {
			return ( ( c == keys ) | ... );
		};
		if constexpr( is_unchecked_input ) {
			while( not is_eq( *first ) ) {
				++first;
			}
		} else {
			while( first < last and not is_eq( *first ) ) {
				++first;
			}
		}
		return first;
	}

	template<bool is_unchecked_input, char... keys>
	DAW_ATTRIBUTE_FLATTEN static inline char const *
	mem_find_substr( sse42_exec_tag const &, char const *first,
	                 char const *last ) {
		static constexpr int keys_len = static_cast<int>( sizeof...( keys ) );
		static_assert( keys_len <= 16 );
		__m128i const a = set_epi8_reverse( keys... );
		static constexpr int compare_mode =
		  _SIDD_SBYTE_OPS | _SIDD_CMP_EQUAL_ORDERED;

		while( last - first >= 16 ) {
			__m128i const b =
			  _mm_loadu_si128( reinterpret_cast<__m128i const *>( first ) );
			int const b_len =
			  last - first >= 16 ? 16 : static_cast<int>( last - first );
			int result = _mm_cmpestri( a, keys_len, b, b_len, compare_mode );
			first += result;
			if( result < 16 ) {
				return first;
			}
		}
		static constexpr auto is_eq = []( char const *ptr ) {
			static constexpr auto check = []( char const *&p, char c ) {
				return *p++ == c;
			};
			return ( check( ptr, keys ) and ... );
		};
		// TODO look into std::search and perf, this is a tail end of document when
		// there are <16 characters left.  This is also primarily for end of comment
		// finding
		if constexpr( is_unchecked_input ) {
			while( not is_eq( first ) ) {
				++first;
			}
		} else {
			while(
			  ( last - first > static_cast<std::ptrdiff_t>( sizeof...( keys ) ) ) and
			  not is_eq( first ) ) {
				++first;
			}
		}
		return first + sizeof...( keys );
	}

	template<bool is_unchecked_input, char... keys>
	DAW_ATTRIBUTE_FLATTEN static inline char const *
	mem_move_to_next_not_of( sse42_exec_tag const &, char const *first,
	                         char const *last ) {
		static constexpr int keys_len = static_cast<int>( sizeof...( keys ) );
		static_assert( keys_len <= 16 );
		__m128i const a = set_epi8_reverse( keys... );
		static constexpr int compare_mode =
		  _SIDD_SBYTE_OPS | _SIDD_CMP_EQUAL_ANY | _SIDD_NEGATIVE_POLARITY;

		while( last - first >= 16 ) {
			__m128i const b =
			  _mm_loadu_si128( reinterpret_cast<__m128i const *>( first ) );
			int const b_len =
			  last - first >= 16 ? 16 : static_cast<int>( last - first );
			int result = _mm_cmpestri( a, keys_len, b, b_len, compare_mode );
			first += result;
			if( result < 16 ) {
				return first;
			}
		}
		static constexpr auto is_eq = []( char c ) {
			return ( ( c == keys ) | ... );
		};
		if constexpr( is_unchecked_input ) {
			while( is_eq( *first ) ) {
				++first;
			}
		} else {
			while( first < last and is_eq( *first ) ) {
				++first;
			}
		}
		return first;
	}
} // namespace daw::json::json_details
#endif
