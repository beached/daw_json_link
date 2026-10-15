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

#if defined( DAW_JSON_HAS_SIMD )
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
			static_assert( char_simd_size <= 64 and
			                 ( char_simd_size & ( char_simd_size - 1U ) ) == 0,
			               "Block size must be a power of two no larger than 64" );

			/// The scans below load whole aligned blocks.  The first block starts at
			/// first rounded down with the bits before first masked off, and the
			/// scan stops at the first block that starts at or after last, so every
			/// block holds at least one byte of [first, last).  Hits at or after
			/// last are clamped to last.
			[[nodiscard]] DAW_ATTRIB_INLINE std::uintptr_t
			align_down_to_block( std::uintptr_t addr ) {
				return addr & ~std::uintptr_t{ char_simd_size - 1U };
			}

			template<char k>
			DAW_ATTRIB_INLINE std::uint64_t mem_find_eq_simd( char_simd_t block ) {
				return ( block == char_simd_t( k ) ).to_ullong( );
			}

			template<bool is_unchecked_input, char... keys>
			DAW_ATTRIB_INLINE daw::not_null<char const *>
			mem_move_to_next_of_simd( daw::not_null<char const *> first,
			                          daw::not_null<char const *> const last ) {
				if( first >= last ) {
					return last;
				}
				auto const first_addr = reinterpret_cast<std::uintptr_t>( first.get( ) );
				auto const last_addr = reinterpret_cast<std::uintptr_t>( last.get( ) );
				auto const find_keys = []( char_simd_t block ) {
					return ( mem_find_eq_simd<keys>( block ) | ... );
				};
				std::uintptr_t block_addr;
				std::uint64_t key_positions;
				if( last_addr - first_addr >= char_simd_size ) {
					// Most strings are short, one unaligned load usually covers them
					auto const key_positions0 =
					  find_keys( daw::simd::unchecked_load<char_simd_t>(
					    daw::span( first.get( ), char_simd_size ) ) );
					if( key_positions0 != 0 ) {
						return first + find_lsb_set( key_positions0 );
					}
					block_addr = align_down_to_block( first_addr + char_simd_size );
					key_positions = ~std::uint64_t{ 0 }
					                << ( first_addr + char_simd_size - block_addr );
				} else {
					block_addr = align_down_to_block( first_addr );
					key_positions = ~std::uint64_t{ 0 } << ( first_addr - block_addr );
				}
				if( block_addr >= last_addr ) {
					return last;
				}
				key_positions &= find_keys(
				  load_aligned_block<char_simd_t>( block_addr, first_addr, last_addr ) );
				while( key_positions == 0 ) {
					block_addr += char_simd_size;
					if( block_addr >= last_addr ) {
						return last;
					}
					key_positions = find_keys(
					  load_aligned_block<char_simd_t>( block_addr, first_addr, last_addr ) );
				}
				auto const hit_addr =
				  block_addr +
				  static_cast<std::uintptr_t>( find_lsb_set( key_positions ) );
				if( hit_addr >= last_addr ) {
					return last;
				}
				return first + static_cast<std::ptrdiff_t>( hit_addr - first_addr );
			}

			// Adapted from
			// https://github.com/simdjson/simdjson/blob/master/src/generic/stage1/json_string_scanner.h#L79
			DAW_ATTRIB_INLINE constexpr std::uint64_t
			find_escaped_branchless( std::uint64_t &prev_escaped,
			                         std::uint64_t backslashes ) {
				DAW_CPP23_STATIC_LOCAL constexpr std::uint64_t odd_bits =
				  0xAAAA'AAAA'AAAA'AAAAULL;
				DAW_CPP23_STATIC_LOCAL constexpr auto valid_bits = [] {
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

			/// Find the closing quote of a string whose contents start at first.
			/// first_escape, when not null and still negative, is set to the offset
			/// from first of the first backslash before the closing quote.
			/// When there is room the first block is an unaligned load at first, as
			/// most strings are short.  The aligned blocks after it may overlap it,
			/// the overlap is masked off and the escape carry is moved to the first
			/// byte that was not scanned yet.
			template<bool is_unchecked_input>
			inline daw::not_null<char const *>
			mem_skip_until_end_of_string_simd( daw::not_null<char const *> first,
			                                   daw::not_null<char const *> const last,
			                                   std::ptrdiff_t *first_escape ) {
				if( first >= last ) {
					return last;
				}
				auto const first_addr = reinterpret_cast<std::uintptr_t>( first.get( ) );
				auto const last_addr = reinterpret_cast<std::uintptr_t>( last.get( ) );
				std::uint64_t prev_escapes = 0;
				char const *result = nullptr;
				// Scan one block.  Bits below lead_shift are not part of the string
				// or were already scanned.  Returns true when the scan is done
				auto const scan = [&]( char_simd_t block, std::uintptr_t block_addr,
				                       unsigned lead_shift ) {
					auto const lead_mask = ~std::uint64_t{ 0 } << lead_shift;
					auto const backslashes = mem_find_eq_simd<'\\'>( block ) & lead_mask;
					prev_escapes <<= lead_shift;
					auto const escaped =
					  find_escaped_branchless( prev_escapes, backslashes );
					auto const quotes =
					  mem_find_eq_simd<'"'>( block ) & lead_mask & ~escaped;
					auto relevant_backslashes = backslashes;
					if( quotes != 0 ) {
						auto const quote_pos = find_lsb_set( quotes );
						relevant_backslashes &=
						  ( std::uint64_t{ 1 } << static_cast<unsigned>( quote_pos ) ) - 1U;
					}
					if( first_escape != nullptr and *first_escape < 0 and
					    relevant_backslashes != 0 ) {
						auto const escape_addr =
						  block_addr + static_cast<std::uintptr_t>(
						                 find_lsb_set( relevant_backslashes ) );
						if( escape_addr < last_addr ) {
							*first_escape =
							  static_cast<std::ptrdiff_t>( escape_addr - first_addr );
						}
					}
					if( quotes == 0 ) {
						return false;
					}
					auto const quote_addr =
					  block_addr + static_cast<std::uintptr_t>( find_lsb_set( quotes ) );
					result = quote_addr >= last_addr
					           ? last.get( )
					           : first.get( ) +
					               static_cast<std::ptrdiff_t>( quote_addr - first_addr );
					return true;
				};
				std::uintptr_t block_addr;
				unsigned lead_shift;
				if( last_addr - first_addr >= char_simd_size ) {
					if( scan( daw::simd::unchecked_load<char_simd_t>(
					            daw::span( first.get( ), char_simd_size ) ),
					          first_addr, 0 ) ) {
						return result;
					}
					block_addr = align_down_to_block( first_addr + char_simd_size );
					lead_shift = static_cast<unsigned>( first_addr + char_simd_size -
					                                    block_addr );
				} else {
					block_addr = align_down_to_block( first_addr );
					lead_shift = static_cast<unsigned>( first_addr - block_addr );
				}
				while( block_addr < last_addr ) {
					if( scan(
					      load_aligned_block<char_simd_t>( block_addr, first_addr, last_addr ),
					      block_addr, lead_shift ) ) {
						return result;
					}
					block_addr += char_simd_size;
					lead_shift = 0;
				}
				return last;
			}

			template<bool is_unchecked_input>
			inline daw::not_null<char const *>
			mem_skip_until_end_of_string_simd( daw::not_null<char const *> first,
			                                   daw::not_null<char const *> const last ) {
				return mem_skip_until_end_of_string_simd<is_unchecked_input>(
				  first, last, nullptr );
			}

			template<bool is_unchecked_input>
			inline daw::not_null<char const *>
			mem_skip_until_end_of_string_simd( daw::not_null<char const *> first,
			                                   daw::not_null<char const *> const last,
			                                   std::ptrdiff_t &first_escape ) {
				return mem_skip_until_end_of_string_simd<is_unchecked_input>(
				  first, last, &first_escape );
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

#if defined( DAW_JSON_HAS_SIMD )
				if( use_simd_exec_mode<ExecTag>( ) ) {
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
#if defined( DAW_JSON_HAS_SIMD )
				if( use_simd_exec_mode<ExecTag>( ) ) {
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
#if defined( DAW_JSON_HAS_SIMD )
				if( use_simd_exec_mode<ExecTag>( ) ) {
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
