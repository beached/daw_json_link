// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/daw_json_link
//

#pragma once

#include "daw_simd_modes.h"

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

namespace daw::json::json_details {
	DAW_ATTRIBUTE_FLATTEN static inline constexpr bool
	is_escaped( char const *ptr, char const *min_ptr ) {
		if( *( ptr - 1 ) != '\\' ) {
			return false;
		}
		if( ( ptr - min_ptr ) < 2 ) {
			return false;
		}
		return *( ptr - 2 ) != '\\';
	}

	DAW_ATTRIBUTE_FLATTEN static inline __m128i
	set_reverse( char c0, char c1 = 0, char c2 = 0, char c3 = 0, char c4 = 0,
	             char c5 = 0, char c6 = 0, char c7 = 0, char c8 = 0, char c9 = 0,
	             char c10 = 0, char c11 = 0, char c12 = 0, char c13 = 0,
	             char c14 = 0, char c15 = 0 ) {
		return _mm_set_epi8( c15, c14, c13, c12, c11, c10, c9, c8, c7, c6, c5, c4,
		                     c3, c2, c1, c0 );
	}

	template<bool is_unchecked_input, char... keys>
	DAW_ATTRIBUTE_FLATTEN static inline char const *
	sse42_move_to_next_of( char const *first, char const *last ) {
		static constexpr int keys_len = static_cast<int>( sizeof...( keys ) );
		static_assert( keys_len <= 16 );
		__m128i const a = set_reverse( keys... );
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
	sse42_find_substr( char const *first, char const *last ) {
		static constexpr int keys_len = static_cast<int>( sizeof...( keys ) );
		static_assert( keys_len <= 16 );
		__m128i const a = set_reverse( keys... );
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
		// there are <16 characters left.  This is also primarily for end of comment finding
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
	sse42_move_to_next_not_of( char const *first, char const *last ) {
		static constexpr int keys_len = static_cast<int>( sizeof...( keys ) );
		static_assert( keys_len <= 16 );
		__m128i const a = set_reverse( keys... );
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

	template<bool is_unchecked_input>
	DAW_ATTRIBUTE_FLATTEN static inline char const *
	sse42_skip_string( char const *first, char const *const last ) {
		return sse42_move_to_next_of<is_unchecked_input, '"', '\\'>( first,
		                                                                last );
	}

	template<bool is_unchecked_input>
	DAW_ATTRIBUTE_FLATTEN static inline char const *
	sse42_skip_until_end_of_string( char const *first, char const *const last ) {
		while( first < last ) {
			auto ptr = sse42_move_to_next_of<is_unchecked_input, '"'>( first, last );
			if( DAW_JSON_LIKELY( not is_escaped( ptr, first ) ) ) {
				return ptr;
			} else {
				first = ptr + 1;
				continue;
			}
		}
		return first;
	}
} // namespace daw::json::json_details
#endif
