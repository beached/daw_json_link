// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/daw_json_link
//

#pragma once

#include "daw_json_exec_modes.h"
#include "version.h"

#include <daw/daw_hide.h>
#include <daw/daw_uint_buffer.h>

#if defined( DAW_ALLOW_SSE42 )
#include <emmintrin.h>
#include <nmmintrin.h>
#include <smmintrin.h>
#include <tmmintrin.h>
#include <wmmintrin.h>
#include <xmmintrin.h>
#ifdef _MSC_VER
#include <intrin.h>
#endif
#endif

#include <ciso646>
#include <cstddef>
#include <cstring>

namespace daw::json {
	inline namespace DAW_JSON_VER {
		namespace json_details {
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

			struct key_table_t {
				alignas( 16 ) bool values[256] = { };

				constexpr bool operator[]( char idx ) const {
					return values[static_cast<unsigned char>( idx )];
				}
			};

			template<char... keys>
			static constexpr inline key_table_t key_table = [] {
				auto result = key_table_t{ };
				(void)( ( result.values[static_cast<unsigned char>( keys )] = true ) |
				        ... );
				return result;
			}( );

			static inline std::ptrdiff_t find_lsb_set( runtime_exec_tag,
			                                           UInt32 value ) {
#if defined( __GNUC__ ) or defined( __clang__ )
				return __builtin_ffs( static_cast<int>( value ) ) - 1;
#else
				unsigned long index;
				_BitScanForward( &index, static_cast<int>( value ) );
				return static_cast<std::ptrdiff_t>( index );
#endif
			}

#if defined( DAW_ALLOW_SSE42 )
			DAW_ATTRIBUTE_FLATTEN static inline __m128i
			set_reverse( char c0, char c1 = 0, char c2 = 0, char c3 = 0, char c4 = 0,
			             char c5 = 0, char c6 = 0, char c7 = 0, char c8 = 0,
			             char c9 = 0, char c10 = 0, char c11 = 0, char c12 = 0,
			             char c13 = 0, char c14 = 0, char c15 = 0 ) {
				return _mm_set_epi8( c15, c14, c13, c12, c11, c10, c9, c8, c7, c6, c5,
				                     c4, c3, c2, c1, c0 );
			}

			DAW_ATTRIBUTE_FLATTEN static inline __m128i
			uload16_char_data( sse42_exec_tag, char const *ptr ) {
				return _mm_loadu_si128( reinterpret_cast<__m128i const *>( ptr ) );
			}

			DAW_ATTRIBUTE_FLATTEN static inline __m128i
			load16_char_data( sse42_exec_tag, char const *ptr ) {
				return _mm_load_si128( reinterpret_cast<__m128i const *>( ptr ) );
			}

			template<char k>
			DAW_ATTRIBUTE_FLATTEN static inline UInt32 mem_find_eq( sse42_exec_tag,
			                                                        __m128i block ) {
				__m128i const keys = _mm_set1_epi8( k );
				__m128i const found = _mm_cmpeq_epi8( block, keys );
				return to_uint32( _mm_movemask_epi8( found ) );
			}

			template<unsigned char k>
			DAW_ATTRIBUTE_FLATTEN static inline UInt32 mem_find_gt( sse42_exec_tag,
			                                                        __m128i block ) {
				static __m128i const keys = _mm_set1_epi8( k );
				__m128i const found = _mm_cmpgt_epi8( block, keys );
				return to_uint32( _mm_movemask_epi8( found ) );
			}

			template<bool is_unchecked_input, char... keys>
			DAW_ATTRIBUTE_FLATTEN static inline char const *
			mem_move_to_next_of( sse42_exec_tag tag, char const *first,
			                     char const *const last ) {

				while( last - first >= 16 ) {
					auto const val0 = uload16_char_data( tag, first );
					auto const key_positions = ( mem_find_eq<keys>( tag, val0 ) | ... );
					if( key_positions != 0 ) {
						return first + find_lsb_set( tag, key_positions );
					}
					first += 16;
				}
				__m128i val1{ };
				auto const max_pos = last - first;
				memcpy( &val1, first, static_cast<std::size_t>( max_pos ) );
				auto const key_positions = ( mem_find_eq<keys>( tag, val1 ) | ... );
				if( key_positions != 0 ) {
					auto const offset = find_lsb_set( tag, key_positions );
					if( offset >= max_pos ) {
						return last;
					}
					return first + offset;
				}
				return last;
			}

			template<bool is_unchecked_input, char... keys>
			DAW_ATTRIBUTE_FLATTEN static inline char const *
			mem_move_to_next_not_of( sse42_exec_tag tag, char const *first,
			                         char const *last ) {
				static constexpr int keys_len = static_cast<int>( sizeof...( keys ) );
				static_assert( keys_len <= 16 );
				__m128i const a = set_reverse( keys... );
				static constexpr int compare_mode =
				  _SIDD_SBYTE_OPS | _SIDD_CMP_EQUAL_ANY | _SIDD_NEGATIVE_POLARITY;

				while( last - first >= 16 ) {
					auto const b = uload16_char_data( tag, first );
					int const result = _mm_cmpestri( a, keys_len, b, 16, compare_mode );
					first += result;
					if( result < 16 ) {
						return first;
					}
				}
				__m128i b{ };
				auto const max_pos = last - first;
				int const result = _mm_cmpestri( a, keys_len, b, 16, compare_mode );
				if( result < max_pos ) {
					return first + result;
				}
				return last;
			}

			template<typename U32>
			DAW_ATTRIBUTE_FLATTEN static inline bool
			add_overflow( U32 value1, U32 value2, U32 &result ) {
				static_assert( sizeof( U32 ) <= sizeof( unsigned long long ) );
				static_assert( sizeof( U32 ) == 4 );
#if defined( __clang__ ) or defined( __GNUC__ )
				if constexpr( sizeof( unsigned ) == sizeof( U32 ) ) {
					return __builtin_uadd_overflow(
					  static_cast<unsigned>( value1 ), static_cast<unsigned>( value2 ),
					  reinterpret_cast<unsigned *>( &result ) );
				} else if constexpr( sizeof( unsigned long ) == sizeof( U32 ) ) {
					return __builtin_uaddl_overflow(
					  static_cast<unsigned long>( value1 ),
					  static_cast<unsigned long>( value2 ),
					  reinterpret_cast<unsigned long *>( &result ) );
				} else {
					return __builtin_uaddll_overflow(
					  static_cast<unsigned long long>( value1 ),
					  static_cast<unsigned long long>( value2 ),
					  reinterpret_cast<unsigned long long *>( &result ) );
				}
#else
				return _addcarry_u32( 0, static_cast<std::uint32_t>( value1 ),
				                      static_cast<std::uint32_t>( value2 ),
				                      reinterpret_cast<std::uint32_t *>( &result ) );
#endif
			}

			// Adapted from
			// https://github.com/simdjson/simdjson/blob/master/src/generic/stage1/json_string_scanner.h#L79
			DAW_ATTRIBUTE_FLATTEN static inline constexpr UInt32
			find_escaped_branchless( constexpr_exec_tag, UInt32 &prev_escaped,
			                         UInt32 backslashes ) {
				backslashes &= ~prev_escaped;
				UInt32 follow_escape = ( backslashes << 1 ) | prev_escaped;
				constexpr UInt32 even_bits = 0x5555'5555_u32;

				UInt32 const odd_seq_start =
				  backslashes & ( ~even_bits ) & ( ~follow_escape );
				UInt32 seq_start_on_even_bits = 0_u32;
				prev_escaped = [&] {
					auto r = odd_seq_start + backslashes;
					seq_start_on_even_bits = 0x0000'FFFF_u32 & r;
					r >>= 16U;
					return r;
				}( );
				UInt32 invert_mask = seq_start_on_even_bits << 1U;

				return ( even_bits ^ invert_mask ) & follow_escape;
			}

			DAW_ATTRIBUTE_FLATTEN static inline UInt32 prefix_xor( sse42_exec_tag,
			                                                       UInt32 bitmask ) {
				__m128i const all_ones = _mm_set1_epi8( '\xFF' );
				__m128i const result = _mm_clmulepi64_si128(
				  _mm_set_epi32( 0, 0, 0, static_cast<std::int32_t>( bitmask ) ),
				  all_ones, 0 );
				return to_uint32( _mm_cvtsi128_si32( result ) );
			}

			template<bool is_unchecked_input>
			static inline char const *
			mem_skip_until_end_of_string( simd_exec_tag tag, char const *first,
			                              char const *const last ) {
				UInt32 prev_escapes = 0_u32;
				while( last - first >= 16 ) {
					auto const val0 = uload16_char_data( tag, first );
					UInt32 const backslashes = mem_find_eq<'\\'>( tag, val0 );
					UInt32 const escaped =
					  find_escaped_branchless( tag, prev_escapes, backslashes );
					UInt32 const quotes = mem_find_eq<'"'>( tag, val0 ) & ( ~escaped );
					UInt32 const in_string = prefix_xor( tag, quotes );
					if( in_string != 0 ) {
						first += find_lsb_set( tag, in_string );
						return first;
					}
					first += 16;
				}
				if constexpr( is_unchecked_input ) {
					while( *first != '"' ) {
						while( not key_table<'"', '\\'>[*first] ) {
							++first;
						}
						if( *first == '"' ) {
							return first;
						}
						first += 2;
					}
				} else {
					while( DAW_JSON_LIKELY( first < last ) and *first != '"' ) {
						while( DAW_JSON_LIKELY( first < last ) and
						       not key_table<'"', '\\'>[*first] ) {
							++first;
						}
						if( first >= last ) {
							return last;
						}
						if( *first == '"' ) {
							return first;
						}
						first += 2;
					}
				}
				return ( is_unchecked_input or DAW_JSON_LIKELY( first < last ) ) ? first
				                                                                 : last;
			}

			template<bool is_unchecked_input>
			static inline char const *
			mem_skip_until_end_of_string( simd_exec_tag tag, char const *first,
			                              char const *const last,
			                              std::ptrdiff_t &first_escape ) {
				char const *const first_first = first;
				UInt32 prev_escapes = 0_u32;
				while( last - first >= 16 ) {
					auto const val0 = uload16_char_data( tag, first );
					UInt32 const backslashes = mem_find_eq<'\\'>( tag, val0 );
					if( ( backslashes != 0 ) & ( first_escape < 0 ) ) {
						first_escape = find_lsb_set( tag, backslashes );
					}
					UInt32 const escaped =
					  find_escaped_branchless( tag, prev_escapes, backslashes );
					UInt32 const quotes = mem_find_eq<'"'>( tag, val0 ) & ( ~escaped );
					UInt32 const in_string = prefix_xor( tag, quotes );
					if( in_string != 0 ) {
						first += find_lsb_set( tag, in_string );
						return first;
					}
					first += 16;
				}
				if constexpr( is_unchecked_input ) {
					while( *first != '"' ) {
						while( not key_table<'"', '\\'>[*first] ) {
							++first;
						}
						if( *first == '"' ) {
							return first;
						}
						if( first_escape < 0 ) {
							first_escape = first_first - first;
						}
						first += 2;
					}
				} else {
					while( DAW_JSON_LIKELY( first < last ) and *first != '"' ) {
						while( DAW_JSON_LIKELY( first < last ) and
						       not key_table<'"', '\\'>[*first] ) {
							++first;
						}
						if( first >= last ) {
							return last;
						}
						if( *first == '"' ) {
							return first;
						}
						if( first_escape < 0 ) {
							first_escape = first_first - first;
						}
						first += 2;
					}
				}
				return ( is_unchecked_input or DAW_JSON_LIKELY( first < last ) ) ? first
				                                                                 : last;
			}

#endif
			template<bool is_unchecked_input, char... keys>
			DAW_ATTRIBUTE_FLATTEN static inline char const *
			mem_move_to_next_of( runtime_exec_tag, char const *first,
			                     char const *last ) {

				if( sizeof...( keys ) == 1 ) {
					char const key[]{ keys... };
					char const *ptr = reinterpret_cast<char const *>( std::memchr(
					  first, key[0], static_cast<std::size_t>( last - first ) ) );
					if( ptr == nullptr ) {
						ptr = last;
					}
					return ptr;
				} else {
					constexpr auto eq = []( char l, char r ) { return l == r; };
					while( is_unchecked_input or first < last ) {
						char const c = *first;
						if( ( eq( c, keys ) | ... ) ) {
							return first;
						}
						++first;
					}
					return first;
				}
			}

			template<bool is_unchecked_input, typename ExecTag,
			         std::enable_if_t<std::is_base_of_v<runtime_exec_tag, ExecTag>,
			                          std::nullptr_t> = nullptr>
			DAW_ATTRIBUTE_FLATTEN static inline char const *
			mem_skip_string( ExecTag const &tag, char const *first,
			                 char const *const last ) {
				return mem_move_to_next_of<is_unchecked_input, '"', '\\'>( tag, first,
				                                                           last );
			}

			template<bool is_unchecked_input, typename ExecTag,
			         std::enable_if_t<std::is_base_of_v<runtime_exec_tag, ExecTag>,
			                          std::nullptr_t> = nullptr>
			DAW_ATTRIBUTE_FLATTEN static inline char const *
			mem_skip_until_end_of_string( ExecTag const &tag, char const *first,
			                              char const *const last ) {
				if constexpr( not is_unchecked_input ) {
					daw_json_assert( first < last, ErrorReason::UnexpectedEndOfData );
				}
				first = mem_move_to_next_of<is_unchecked_input, '\\', '"'>( tag, first,
				                                                            last );
				while( is_unchecked_input or first < last ) {
					switch( *first ) {
					case '"':
						return first;
					case '\\':
						if constexpr( is_unchecked_input ) {
							++first;
						} else {
							first += static_cast<int>( static_cast<bool>( last - first ) );
						}
						break;
					}
					++first;
					first = mem_move_to_next_of<is_unchecked_input, '\\', '"'>(
					  tag, first, last );
				}
				return first;
			}

			template<bool is_unchecked_input>
			DAW_ATTRIBUTE_FLATTEN static inline char const *
			mem_skip_until_end_of_string( runtime_exec_tag tag, char const *first,
			                              char const *const last,
			                              std::ptrdiff_t &first_escape ) {
				char const *const first_first = first;
				if constexpr( not is_unchecked_input ) {
					daw_json_assert( first < last, ErrorReason::UnexpectedEndOfData );
				}
				first = mem_move_to_next_of<is_unchecked_input, '\\', '"'>( tag, first,
				                                                            last );
				while( is_unchecked_input or first < last ) {
					switch( *first ) {
					case '"':
						return first;
					case '\\':
						if( first_escape < 0 ) {
							first_escape = first_first - first;
						}
						if constexpr( is_unchecked_input ) {
							++first;
						} else {
							first += static_cast<int>( static_cast<bool>( last - first ) );
						}
						break;
					}
					++first;
					first = mem_move_to_next_of<is_unchecked_input, '\\', '"'>(
					  tag, first, last );
				}
				return first;
			}
		} // namespace json_details
	}   // namespace DAW_JSON_VER
} // namespace daw::json
