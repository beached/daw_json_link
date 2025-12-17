// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/daw_json_link
//

#pragma once

#include "daw/json/impl/version.h"

#include "daw/json/impl/daw_json_assert.h"
#include "daw/json/impl/daw_json_exec_modes.h"

#include <daw/daw_attributes.h>
#include <daw/daw_cpp_feature_check.h>
#include <daw/daw_cxmath.h>
#include <daw/daw_do_n.h>
#include <daw/daw_likely.h>
#include <daw/daw_logic.h>
#include <daw/daw_not_null.h>
#include <daw/daw_uint_buffer.h>
#include <daw/daw_unreachable.h>

#if defined( DAW_ALLOW_SSE42 )
#include <emmintrin.h>
#include <nmmintrin.h>
#include <smmintrin.h>
#include <tmmintrin.h>
#include <wmmintrin.h>
#include <xmmintrin.h>
#if defined( DAW_HAS_MSVC_LIKE )
#include <intrin.h>
#endif
#endif

#include <cstddef>
#include <cstring>
#include <type_traits>

namespace daw::json {
	inline namespace DAW_JSON_VER {
		namespace json_details {
			DAW_ATTRIB_INLINE
			constexpr bool is_escaped( daw::not_null<char const *> ptr,
			                           daw::not_null<char const *> min_ptr ) {
				if( *( ptr - 1 ) != '\\' ) {
					return false;
				}
				if( ( ptr - min_ptr ) < 2 ) {
					return false;
				}
				return *( ptr - 2 ) != '\\';
			}

#if defined( DAW_ALLOW_SSE42 )
			struct key_table_t {
				alignas( 16 ) bool values[256] = { };

				constexpr bool operator[]( char idx ) const {
					return values[static_cast<unsigned char>( idx )];
				}
			};

			template<char... keys>
			static constexpr inline key_table_t key_table =
			  [] DAW_CPP23_STATIC_CALL_OP {
				  auto result = key_table_t{ };
				  (void)( daw::nsc_or(
				    ( result.values[static_cast<unsigned char>( keys )] = true )... ) );
				  return result;
			  }( );

			constexpr std::ptrdiff_t find_lsb_set( UInt32 value ) {
#if DAW_HAS_BUILTIN( __builtin_ffs )
				return __builtin_ffs( static_cast<int>( value ) ) - 1;
#elif defined( DAW_HAS_MSVC_LIKE )
				if( not DAW_IS_CONSTANT_EVALUATED( ) ) {
					unsigned long index;
					_BitScanForward( &index, static_cast<int>( value ) );
					return static_cast<std::ptrdiff_t>( index );
				}
#else
				std::ptrdiff_t result = 0;
				if( value == 0 ) {
					return -1;
				}
				while( ( value & 1 ) == 0 ) {
					value >>= 1;
					++result;
				}
				return result;
#endif
			}

			DAW_ATTRIB_INLINE
			__m128i set_reverse( char c0, char c1 = 0, char c2 = 0, char c3 = 0,
			                     char c4 = 0, char c5 = 0, char c6 = 0, char c7 = 0,
			                     char c8 = 0, char c9 = 0, char c10 = 0, char c11 = 0,
			                     char c12 = 0, char c13 = 0, char c14 = 0,
			                     char c15 = 0 ) {
				return _mm_set_epi8( c15,
				                     c14,
				                     c13,
				                     c12,
				                     c11,
				                     c10,
				                     c9,
				                     c8,
				                     c7,
				                     c6,
				                     c5,
				                     c4,
				                     c3,
				                     c2,
				                     c1,
				                     c0 );
			}

			DAW_ATTRIB_INLINE __m128i
			uload16_char_data_simd( daw::not_null<char const *> ptr ) {
				return _mm_loadu_si128(
				  reinterpret_cast<__m128i const *>( ptr.get( ) ) );
			}

			DAW_ATTRIB_INLINE __m128i
			load16_char_data_simd( daw::not_null<char const *> ptr ) {
				return _mm_load_si128(
				  reinterpret_cast<__m128i const *>( ptr.get( ) ) );
			}

			template<char k>
			DAW_ATTRIB_INLINE UInt32 mem_find_eq_simd( __m128i block ) {
				__m128i const keys = _mm_set1_epi8( k );
				__m128i const found = _mm_cmpeq_epi8( block, keys );
				return to_uint32( _mm_movemask_epi8( found ) );
			}

			template<bool is_unchecked_input, char... keys>
			DAW_ATTRIB_INLINE daw::not_null<char const *>
			mem_move_to_next_of_sse42( daw::not_null<char const *> first,
			                           daw::not_null<char const *> const last ) {

				while( last - first >= 16 ) {
					auto const val0 = uload16_char_data_simd( first );
					auto const key_positions = ( mem_find_eq_simd<keys>( val0 ) | ... );
					if( key_positions != 0 ) {
						return first + find_lsb_set( key_positions );
					}
					first += 16;
				}
				auto val1 = __m128i{ };
				auto const max_pos = last - first;
				std::memcpy( &val1, first, static_cast<std::size_t>( max_pos ) );
				auto const key_positions = ( mem_find_eq_simd<keys>( val1 ) | ... );
				if( key_positions != 0 ) {
					auto const offset = find_lsb_set( key_positions );
					if( offset >= max_pos ) {
						return last;
					}
					return first + offset;
				}
				return last;
			}

			// Adapted from
			// https://github.com/simdjson/simdjson/blob/master/src/generic/stage1/json_string_scanner.h#L79
			DAW_ATTRIB_INLINE constexpr UInt32
			find_escaped_branchless( UInt32 &prev_escaped, UInt32 backslashes ) {
				backslashes &= ~prev_escaped;
				UInt32 follow_escape = ( backslashes << 1 ) | prev_escaped;
				using even_bits = daw::constant<0x5555'5555_u32>;

				UInt32 const odd_seq_start =
				  backslashes & ( ~even_bits::value ) & ( ~follow_escape );
				UInt32 seq_start_on_even_bits = 0_u32;
				prev_escaped = [&] {
					auto r = odd_seq_start + backslashes;
					seq_start_on_even_bits = 0x0000'FFFF_u32 & r;
					r >>= 16U;
					return r;
				}( );
				UInt32 invert_mask = seq_start_on_even_bits << 1U;

				return ( even_bits::value ^ invert_mask ) & follow_escape;
			}

			DAW_ATTRIB_INLINE UInt32 prefix_xor_simd( UInt32 bitmask ) {
				__m128i const all_ones = _mm_set1_epi8( '\xFF' );
				__m128i const result = _mm_clmulepi64_si128(
				  _mm_set_epi32( 0, 0, 0, static_cast<std::int32_t>( bitmask ) ),
				  all_ones,
				  0 );
				return to_uint32( _mm_cvtsi128_si32( result ) );
			}

			template<bool is_unchecked_input>
			inline daw::not_null<char const *> mem_skip_until_end_of_string_simd(
			  daw::not_null<char const *> first,
			  daw::not_null<char const *> const last ) {
				UInt32 prev_escapes = 0_u32;
				while( last - first >= 16 ) {
					auto const val0 = uload16_char_data_simd( first );
					UInt32 const backslashes = mem_find_eq_simd<'\\'>( val0 );
					UInt32 const escaped =
					  find_escaped_branchless( prev_escapes, backslashes );
					UInt32 const quotes = mem_find_eq_simd<'"'>( val0 ) & ( ~escaped );
					UInt32 const in_string = prefix_xor_simd( quotes );
					if( in_string != 0 ) {
						first += find_lsb_set( in_string );
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
					while( DAW_LIKELY( first < last ) and *first != '"' ) {
						while( DAW_LIKELY( first < last ) and
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
				return ( is_unchecked_input or DAW_LIKELY( first < last ) ) ? first
				                                                            : last;
			}

			template<bool is_unchecked_input>
			inline daw::not_null<char const *>
			mem_skip_until_end_of_string_simd( daw::not_null<char const *> first,
			                                   daw::not_null<char const *> const last,
			                                   std::ptrdiff_t &first_escape ) {
				auto const first_first = first;
				UInt32 prev_escapes = 0_u32;
				while( last - first >= 16 ) {
					auto const val0 = uload16_char_data_simd( first );
					UInt32 const backslashes = mem_find_eq_simd<'\\'>( val0 );
					if( ( backslashes != 0 ) & ( first_escape < 0 ) ) {
						first_escape = find_lsb_set( backslashes );
					}
					UInt32 const escaped =
					  find_escaped_branchless( prev_escapes, backslashes );
					UInt32 const quotes = mem_find_eq_simd<'"'>( val0 ) & ( ~escaped );
					UInt32 const in_string = prefix_xor_simd( quotes );
					if( in_string != 0 ) {
						first += find_lsb_set( in_string );
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
							first_escape = first - first_first;
						}
						first += 2;
					}
				} else {
					while( DAW_LIKELY( first < last ) and *first != '"' ) {
						while( DAW_LIKELY( first < last ) and
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
							first_escape = first - first_first;
						}
						first += 2;
					}
				}
				return ( is_unchecked_input or DAW_LIKELY( first < last ) ) ? first
				                                                            : last;
			}

#endif
			template<bool is_unchecked_input, char... keys>
			DAW_ATTRIB_INLINE daw::not_null<char const *>
			mem_move_to_next_of_runtime( daw::not_null<char const *> first,
			                             daw::not_null<char const *> last ) {
				if constexpr( sizeof...( keys ) == 1 ) {
					char const key[]{ keys... };
					char const *ptr = static_cast<char const *>( std::memchr(
					  first, key[0], static_cast<std::size_t>( last - first ) ) );
					if( ptr == nullptr ) {
						ptr = last;
					}
					return ptr;
				} else {
					DAW_CPP23_STATIC_LOCAL constexpr auto eq =
					  []( char l, char r ) DAW_JSON_CPP23_STATIC_CALL_OP {
						  return l == r;
					  };
					while( is_unchecked_input or first < last ) {
						char const c = *first;
						if( nsc_or( eq( c, keys )... ) ) {
							return first;
						}
						++first;
					}
					return first;
				}
			}
			template<bool is_unchecked_input, typename ExecTag, char... keys>
			DAW_ATTRIB_FLATTEN daw::not_null<char const *>
			mem_move_to_next_of( daw::not_null<char const *> first,
			                     daw::not_null<char const *> last ) {

#if defined( DAW_ALLOW_SSE42 )
				if( not std::is_same_v<runtime_exec_tag, ExecTag> ) {
					return mem_move_to_next_of_sse42<is_unchecked_input, keys...>( first,
					                                                               last );
				}
#endif
				return mem_move_to_next_of_runtime<is_unchecked_input, keys...>( first,
				                                                                 last );
			}

			template<bool is_unchecked_input>
			DAW_ATTRIB_INLINE daw::not_null<char const *>
			mem_skip_until_end_of_string_runtime(
			  daw::not_null<char const *> first,
			  daw::not_null<char const *> const last ) {
				if constexpr( not is_unchecked_input ) {
					daw_json_ensure( first < last, ErrorReason::UnexpectedEndOfData );
				}
				first =
				  mem_move_to_next_of<is_unchecked_input, runtime_exec_tag, '\\', '"'>(
				    first, last );
				while( is_unchecked_input or first < last ) {
					switch( *first ) {
					case '"':
						return first;
					case '\\':
						++first;
						break;
					}
					++first;
					first = mem_move_to_next_of<is_unchecked_input,
					                            runtime_exec_tag,
					                            '\\',
					                            '"'>( first, last );
				}
				return first;
			}

			template<bool is_unchecked_input>
			DAW_ATTRIB_INLINE constexpr daw::not_null<char const *>
			mem_skip_until_end_of_string_constexpr(
			  daw::not_null<char const *> first,
			  daw::not_null<char const *> const last ) {
				if( first == last ) {
					return first;
				}
				using char_t = std::remove_const_t<char const>;
				// Check if the last valid char is a '\'.  If not we can skip a check
				// in the loop on escaped things
				if( is_unchecked_input or
				    DAW_LIKELY( *( last - 1 ) != char_t{ '\\' } ) ) {
					while( is_unchecked_input or DAW_UNLIKELY( first < last ) ) {
						char const c = *first;
						if( c == char_t{ '"' } ) {
							break;
						}
						if( c == char_t{ '\\' } ) {
							// We know that the last \ character is not the last character
							// in range
							first += 2;
						} else {
							++first;
						}
					}
				} else {
					while( is_unchecked_input or DAW_UNLIKELY( first < last ) ) {
						char const c = *first;
						if( c == char_t{ '"' } ) {
							break;
						}
						if( c == char_t{ '\\' } ) {
							if( DAW_LIKELY( first + 1 < last ) ) {
								first += 2;
							} else {
								first = last;
								break;
							}
						} else {
							++first;
						}
					}
				}
				return first;
			}

			template<bool is_unchecked_input, typename ExecTag>
			DAW_ATTRIB_INLINE constexpr daw::not_null<char const *>
			mem_skip_until_end_of_string( daw::not_null<char const *> first,
			                              daw::not_null<char const *> const last ) {
				if( use_constexpr_exec_mode<ExecTag>( ) ) {
					return mem_skip_until_end_of_string_constexpr<is_unchecked_input>(
					  first, last );
				}
#if defined( DAW_ALLOW_SSE42 )
				if( not std::is_same_v<runtime_exec_tag, ExecTag> ) {
					return mem_skip_until_end_of_string_simd<is_unchecked_input>( first,
					                                                              last );
				}
#endif
				return mem_skip_until_end_of_string_runtime<is_unchecked_input>( first,
				                                                                 last );
			}

			template<bool is_unchecked_input>
			DAW_ATTRIB_INLINE daw::not_null<char const *>
			mem_skip_until_end_of_string_runtime(
			  daw::not_null<char const *> first,
			  daw::not_null<char const *> const last, std::ptrdiff_t &first_escape ) {
				auto first_first = first;
				if constexpr( not is_unchecked_input ) {
					daw_json_ensure( first < last, ErrorReason::UnexpectedEndOfData );
				}
				first =
				  mem_move_to_next_of<is_unchecked_input, runtime_exec_tag, '\\', '"'>(
				    first, last );
				while( is_unchecked_input or first < last ) {
					switch( *first ) {
					case '"':
						return first;
					case '\\':
						if( first_escape < 0 ) {
							first_escape = first - first_first;
						}
						if constexpr( is_unchecked_input ) {
							++first;
						} else {
							first += static_cast<int>( static_cast<bool>( last - first ) );
						}
						break;
					}
					++first;
					first = mem_move_to_next_of<is_unchecked_input,
					                            runtime_exec_tag,
					                            '\\',
					                            '"'>( first, last );
				}
				return first;
			}

			template<bool is_unchecked_input, typename ExecTag>
			DAW_ATTRIB_FLATINLINE daw::not_null<char const *>
			mem_skip_until_end_of_string( daw::not_null<char const *> first,
			                              daw::not_null<char const *> const last,
			                              std::ptrdiff_t &first_escape ) {
#if defined( DAW_ALLOW_SSE42 )
				if( not std::is_same_v<runtime_exec_tag, ExecTag> ) {
					return mem_skip_until_end_of_string_simd<is_unchecked_input>(
					  first, last, first_escape );
				}
#endif
				return mem_skip_until_end_of_string_runtime<is_unchecked_input>(
				  first, last, first_escape );
			}
		} // namespace json_details
	} // namespace DAW_JSON_VER
} // namespace daw::json
