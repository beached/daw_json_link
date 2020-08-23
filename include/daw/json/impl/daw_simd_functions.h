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

#if defined( DAW_ALLOW_SSE3 )
#include <emmintrin.h>
#include <nmmintrin.h>
#include <smmintrin.h>
#include <tmmintrin.h>
#include <xmmintrin.h>
#ifdef _MSC_VER
#include <intrin.h>
#endif

namespace daw::json::json_details {
	template<bool is_unchecked_input, char... keys>
	static inline char const *sse3_skip_until( char const *first,
	                                           char const *const last ) {
		static_assert( sizeof...( keys ) > 0 );
		while( last - first >= 16 ) {
			__m128i const cur_set =
			  _mm_loadu_si128( reinterpret_cast<__m128i const *>( first ) );
			auto const found =
			  std::array{ _mm_cmpeq_epi8( cur_set, _mm_set1_epi8( keys ) )... };
			__m128i are_found = found[0];
			for( std::size_t n = 1; n < ( sizeof...( keys ) ); ++n ) {
				are_found = _mm_or_si128( are_found, found[n] );
			}
			int const any_set = _mm_movemask_epi8( are_found );
			if( any_set != 0 ) {
				// Find out which is the first character
#if defined( __GNUC__ ) or defined( __clang__ )
				first += __builtin_ffs( any_set ) - 1;
#elif defined( _MSC_VER )
				unsigned long idx;
				_BitScanForward( &idx, any_set );
				first += idx;
#endif
				break;
			}
			first += 16;
		}
		if( last - first < 16 ) {
			if constexpr( is_unchecked_input ) {
				char c = *first;
				bool test = not( ( c == keys ) | ... );
				while( test ) {
					++first;
					c = *first;
					test = not( ( c == keys ) | ... );
				}
			} else {
				char c = *first;
				bool test = first < last and not( ( c == keys ) | ... );
				while( test ) {
					++first;
					test =
					  first < last and not( ( c = *first ), ( ( c == keys ) | ... ) );
				}
			}
		}
		return first;
	}

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

	template<bool is_unchecked_input>
	DAW_ATTRIBUTE_FLATTEN static inline char const *
	find_double_quote_or_slash( char const *first, char const *last ) {
		__m128i const keys =
		  _mm_set_epi8( 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, '\\', '"' );
		static constexpr int keys_len = 2;

		static constexpr int compare_mode = _SIDD_SBYTE_OPS | _SIDD_CMP_EQUAL_ANY;

		// while( last - first >= 16 ) {
		while( last - first >= 16 ) {
			__m128i const a =
			  _mm_loadu_si128( reinterpret_cast<__m128i const *>( first ) );
			int const a_len =
			  last - first >= 16 ? 16 : static_cast<int>( last - first );
			int result = _mm_cmpestri( keys, keys_len, a, a_len, compare_mode );
			first += result;
			if( result < 16 ) {
				return first;
			}
		}

		if constexpr( is_unchecked_input ) {
			char const *const result = reinterpret_cast<char const *>(
			  memchr( first, '"', static_cast<std::size_t>( last - first ) ) );
			char const *const result2 = reinterpret_cast<char const *>( memchr(
			  first, '"',
			  static_cast<std::size_t>( result ? result - first : last - first ) ) );
			if( result2 ) {
				return result2;
			}
			return result;
		} else {
			char const *const result = reinterpret_cast<char const *>(
			  memchr( first, '"', static_cast<std::size_t>( last - first ) ) );
			char const *const result2 = reinterpret_cast<char const *>( memchr(
			  first, '"',
			  static_cast<std::size_t>( result ? result - first : last - first ) ) );
			if( result2 ) {
				return result2;
			}
			if( result ) {
				return result;
			}
			return last;
		}
	}

	template<bool is_unchecked_input>
	static inline char const *sse3_skip_string( char const *first,
	                                            char const *const last ) {
		//return sse3_skip_until<is_unchecked_input, '"', '\\'>( first, last );
		return find_double_quote_or_slash<is_unchecked_input>( first, last );
	}

	template<bool is_unchecked_input>
	static inline char const *
	sse3_skip_until_end_of_string( char const *first, char const *const last ) {
		while( last - first >= 16 ) {
			__m128i const cur_set =
			  _mm_loadu_si128( reinterpret_cast<__m128i const *>( first ) );
			auto const is_found = _mm_cmpeq_epi8( cur_set, _mm_set1_epi8( '"' ) );
			int const any_set = _mm_movemask_epi8( is_found );
			if( any_set != 0 ) {
				// Find out which is the first character
#if defined( __GNUC__ ) or defined( __clang__ )
				first += __builtin_ffs( any_set ) - 1;
#elif defined( _MSC_VER )
				unsigned long idx;
				_BitScanForward( &idx, any_set );
				first += idx;
#endif
				if( DAW_JSON_LIKELY( not is_escaped( first, first - 16 ) ) ) {
					break;
				} else {
					++first;
					continue;
				}
			}
			first += 16;
		}
		if( last - first < 16 ) {
			if constexpr( is_unchecked_input ) {
				while( *first != '"' ) {
					while( *first != '\\' and *first != '"' ) {
						++first;
					}
					if( *first == '\\' ) {
						first += 2;
					} else {
						break;
					}
				}
			} else {
				while( first < last and *first != '"' ) {
					while( first < last and *first != '\\' and *first != '"' ) {
						++first;
					}
					if( first < last and *first == '\\' ) {
						first += 2;
					} else {
						break;
					}
				}
			}
		}
		return first;
	}
} // namespace daw::json::json_details
#endif
