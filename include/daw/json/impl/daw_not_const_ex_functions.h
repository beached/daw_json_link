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
#include "daw/json/impl/daw_json_simd.h"

#include <daw/daw_attributes.h>
#include <daw/daw_cpp_feature_check.h>
#include <daw/daw_cxmath.h>
#include <daw/daw_do_n.h>
#include <daw/daw_likely.h>
#include <daw/daw_logic.h>
#include <daw/daw_not_null.h>
#include <daw/daw_span.h>
#include <daw/daw_uint_buffer.h>
#include <daw/daw_unreachable.h>

#if defined( DAW_HAS_MSVC_LIKE )
#include <intrin.h>
#endif

#include <cstddef>
#include <cstring>
#include <type_traits>

#if defined( DAW_JSON_HAS_SIMD ) and                                      \
  ( defined( __i386__ ) or defined( __x86_64__ ) or defined( _M_IX86 ) or \
    defined( _M_X64 ) )
#define DAW_JSON_HAS_INTEL_STRING_SIMD 1
#endif

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

#if defined( DAW_JSON_HAS_INTEL_STRING_SIMD )
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

#if not defined( DAW_HAS_MSVC_LIKE )
			constexpr
#else
			inline
#endif
			  std::ptrdiff_t find_lsb_set( std::uint64_t value ) {
#if DAW_HAS_BUILTIN( __builtin_ffsll )
				return __builtin_ffsll( static_cast<long long>( value ) ) - 1;
#elif defined( DAW_HAS_MSVC_LIKE )
				if( not DAW_IS_CONSTANT_EVALUATED( ) ) {
					unsigned long index;
#if defined( _M_X64 ) or defined( _M_ARM64 )
					if( _BitScanForward64( &index, value ) != 0 ) {
						return static_cast<std::ptrdiff_t>( index );
					}
#else
					auto const low = static_cast<unsigned long>( value );
					if( _BitScanForward( &index, low ) != 0 ) {
						return static_cast<std::ptrdiff_t>( index );
					}
					auto const high = static_cast<unsigned long>( value >> 32U );
					if( _BitScanForward( &index, high ) != 0 ) {
						return static_cast<std::ptrdiff_t>( index ) + 32;
					}
#endif
					return -1;
				}
#endif
				std::ptrdiff_t result = 0;
				if( value == 0 ) {
					return -1;
				}
				while( ( value & 1 ) == 0 ) {
					value >>= 1;
					++result;
				}
				return result;
			}

			using char_simd_t = daw::simd::vec<char>;
			static constexpr std::size_t char_simd_size = char_simd_t::size( );
			static_assert( char_simd_size <= 64 );

			DAW_ATTRIB_INLINE char_simd_t
			load_char_data_simd( daw::not_null<char const *> ptr ) {
				return daw::simd::unchecked_load<char_simd_t>(
				  daw::span( ptr.get( ), char_simd_size ) );
			}

			template<char k>
			DAW_ATTRIB_INLINE std::uint64_t mem_find_eq_simd( char_simd_t block ) {
				return ( block == char_simd_t( k ) ).to_ullong( );
			}

			template<bool is_unchecked_input, char... keys>
			DAW_ATTRIB_INLINE daw::not_null<char const *>
			mem_move_to_next_of_simd( daw::not_null<char const *> first,
			                          daw::not_null<char const *> const last ) {

				while( last - first >= static_cast<std::ptrdiff_t>( char_simd_size ) ) {
					auto const val0 = load_char_data_simd( first );
					auto const key_positions = ( mem_find_eq_simd<keys>( val0 ) | ... );
					if( key_positions != 0 ) {
						return first + find_lsb_set( key_positions );
					}
					first += char_simd_size;
				}
				auto const max_pos = last - first;
				if( max_pos == 0 ) {
					return last;
				}
				auto const val1 = daw::simd::partial_load<char_simd_t>(
				  daw::span( first.get( ), static_cast<std::size_t>( max_pos ) ) );
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
			DAW_ATTRIB_INLINE constexpr std::uint64_t
			find_escaped_branchless( std::uint64_t &prev_escaped,
			                         std::uint64_t backslashes ) {
				constexpr std::uint64_t odd_bits = 0xAAAA'AAAA'AAAA'AAAAULL;
				constexpr auto valid_bits = [] {
					if constexpr( char_simd_size == 64 ) {
						return ~std::uint64_t{ 0 };
					} else {
						return ( std::uint64_t{ 1 } << char_simd_size ) - 1;
					}
				}( );
				auto const potential_escape = backslashes & ~prev_escaped;
				auto const maybe_escaped = potential_escape << 1U;
				auto const escape_and_terminal =
				  ( ( maybe_escaped | odd_bits ) - potential_escape ) ^ odd_bits;
				auto const escaped =
				  ( escape_and_terminal ^ ( backslashes | prev_escaped ) ) & valid_bits;
				auto const escape_bits = escape_and_terminal & backslashes;
				prev_escaped = ( escape_bits >> ( char_simd_size - 1U ) ) & 1U;
				return escaped;
			}

			DAW_ATTRIB_INLINE constexpr std::uint64_t
			prefix_xor_simd( std::uint64_t bitmask ) {
				bitmask ^= bitmask << 1U;
				bitmask ^= bitmask << 2U;
				bitmask ^= bitmask << 4U;
				bitmask ^= bitmask << 8U;
				bitmask ^= bitmask << 16U;
				bitmask ^= bitmask << 32U;
				return bitmask;
			}

			template<bool is_unchecked_input>
			inline daw::not_null<char const *> mem_skip_until_end_of_string_simd(
			  daw::not_null<char const *> first,
			  daw::not_null<char const *> const last ) {
				std::uint64_t prev_escapes = 0;
				while( last - first >= static_cast<std::ptrdiff_t>( char_simd_size ) ) {
					auto const val0 = load_char_data_simd( first );
					std::uint64_t const backslashes = mem_find_eq_simd<'\\'>( val0 );
					std::uint64_t const escaped =
					  find_escaped_branchless( prev_escapes, backslashes );
					std::uint64_t const quotes =
					  mem_find_eq_simd<'"'>( val0 ) & ( ~escaped );
					std::uint64_t const in_string = prefix_xor_simd( quotes );
					if( in_string != 0 ) {
						first += find_lsb_set( in_string );
						return first;
					}
					first += char_simd_size;
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
				std::uint64_t prev_escapes = 0;
				while( last - first >= static_cast<std::ptrdiff_t>( char_simd_size ) ) {
					auto const val0 = load_char_data_simd( first );
					std::uint64_t const backslashes = mem_find_eq_simd<'\\'>( val0 );
					std::uint64_t const escaped =
					  find_escaped_branchless( prev_escapes, backslashes );
					std::uint64_t const quotes =
					  mem_find_eq_simd<'"'>( val0 ) & ( ~escaped );
					std::uint64_t const in_string = prefix_xor_simd( quotes );
					auto relevant_backslashes = backslashes;
					if( in_string != 0 ) {
						auto const quote_pos = find_lsb_set( in_string );
						relevant_backslashes &=
						  ( std::uint64_t{ 1 } << static_cast<unsigned>( quote_pos ) ) - 1U;
						if( ( relevant_backslashes != 0 ) & ( first_escape < 0 ) ) {
							first_escape =
							  ( first - first_first ) + find_lsb_set( relevant_backslashes );
						}
						first += quote_pos;
						return first;
					}
					if( ( relevant_backslashes != 0 ) & ( first_escape < 0 ) ) {
						first_escape =
						  ( first - first_first ) + find_lsb_set( relevant_backslashes );
					}
					first += char_simd_size;
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

#if defined( DAW_JSON_HAS_INTEL_STRING_SIMD )
				if( not std::is_same_v<runtime_exec_tag, ExecTag> ) {
					return mem_move_to_next_of_simd<is_unchecked_input, keys...>( first,
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
			DAW_ATTRIB_INLINE
			  daw::not_null<char const *> constexpr mem_skip_until_end_of_string(
			    daw::not_null<char const *> first,
			    daw::not_null<char const *> const last ) {
				if( use_constexpr_exec_mode<ExecTag>( ) ) {
					return mem_skip_until_end_of_string_constexpr<is_unchecked_input>(
					  first, last );
				}
#if defined( DAW_JSON_HAS_INTEL_STRING_SIMD )
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
			DAW_ATTRIB_FLATINLINE inline daw::not_null<char const *>
			mem_skip_until_end_of_string( daw::not_null<char const *> first,
			                              daw::not_null<char const *> const last,
			                              std::ptrdiff_t &first_escape ) {
#if defined( DAW_JSON_HAS_INTEL_STRING_SIMD )
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

#if defined( DAW_JSON_HAS_INTEL_STRING_SIMD )
#undef DAW_JSON_HAS_INTEL_STRING_SIMD
#endif
