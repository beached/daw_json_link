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
#include "daw/json/impl/daw_json_parse_policy_policy_details.h"

#include <daw/daw_attributes.h>
#include <daw/daw_is_constant_evaluated.h>

#include <cstddef>
#include <cstdint>
#include <type_traits>

#if __has_include( <simd> )
#include <simd>
#endif

#if defined( __cpp_lib_simd ) or defined( __glibcxx_simd )
#include <bit>
#include <span>

namespace daw::json {
	inline namespace DAW_JSON_VER {
		namespace json_details {
			namespace skip_bracketed_item_simd_details {
				using simd_type = std::simd::vec<char, 64>;
				static constexpr std::size_t block_size =
				  static_cast<std::size_t>( simd_type::size( ) );
				static_assert( block_size <= 64 );

				[[nodiscard]] constexpr std::uint64_t
				low_bits( std::size_t count ) noexcept {
					return count == 64 ? ~std::uint64_t{ 0 }
					                   : ( std::uint64_t{ 1 } << count ) - 1;
				}

				[[nodiscard]] constexpr std::uint64_t
				prefix_xor( std::uint64_t bits ) noexcept {
					bits ^= bits << 1U;
					bits ^= bits << 2U;
					bits ^= bits << 4U;
					bits ^= bits << 8U;
					bits ^= bits << 16U;
					bits ^= bits << 32U;
					return bits;
				}

				[[nodiscard]] constexpr bool last_bit( std::uint64_t bits,
				                                       std::size_t count ) noexcept {
					return count != 0 and
					       ( ( bits >> ( count - 1U ) ) & std::uint64_t{ 1 } ) != 0;
				}

				[[nodiscard]] consteval simd_type splat( char value ) noexcept {
					return simd_type( value );
				}

				template<char... Values>
				[[nodiscard]] constexpr auto one_of( simd_type value ) {
					return ( ( value == splat( Values ) ) | ... );
				}

				[[nodiscard]] constexpr simd_type load( char const *first,
				                                        std::size_t count ) {
					if( count == block_size ) {
						return std::simd::unchecked_load<simd_type>(
						  std::span( first, block_size ) );
					}
					return std::simd::partial_load<simd_type>(
					  std::span( first, count ) );
				}
			} // namespace skip_bracketed_item_simd_details

			/// Skip the JSON class or array at parse_state.first using SIMD to
			/// classify each block.
			template<SkipBracketedType BracketedType, typename ParseState>
			[[nodiscard]] DAW_ATTRIB_FLATTEN constexpr ParseState
			skip_bracketed_item_simd( ParseState &parse_state ) {
				constexpr char primary_left =
				  BracketedType == SkipBracketedType::Class ? '{' : '[';
				constexpr char primary_right =
				  BracketedType == SkipBracketedType::Class ? '}' : ']';
				constexpr char secondary_left =
				  BracketedType == SkipBracketedType::Class ? '[' : '{';
				constexpr char secondary_right =
				  BracketedType == SkipBracketedType::Class ? ']' : '}';

				using namespace skip_bracketed_item_simd_details;
				auto ptr_first = parse_state.first;
				auto const ptr_last = parse_state.last;
				if( DAW_UNLIKELY( ptr_first >= ptr_last ) ) {
					return parse_state;
				}

				auto result = parse_state;
				std::size_t count = 0;
				std::uint32_t primary_depth = 1;
				std::uint32_t secondary_depth = 0;
				bool in_string = false;
				bool escaped = false;

				if( *ptr_first == primary_left ) {
					++ptr_first;
				}

				while( ptr_first < ptr_last ) {
					auto const remaining =
					  static_cast<std::size_t>( ptr_last - ptr_first );
					auto const lane_count =
					  remaining < block_size ? remaining : block_size;
					auto const input = load( ptr_first, lane_count );
					auto const valid_bits = low_bits( lane_count );
					auto const backslash_bits =
					  ( input == splat( '\\' ) ).to_ullong( ) & valid_bits;

					constexpr std::uint64_t odd_bits = 0xAAAAAAAAAAAAAAAAULL;
					auto const previous_escaped = escaped ? std::uint64_t{ 1 } : 0;
					auto const potential_escape = backslash_bits & ~previous_escaped;
					auto const maybe_escaped = potential_escape << 1U;
					auto const escape_and_terminal =
					  ( ( maybe_escaped | odd_bits ) - potential_escape ) ^ odd_bits;
					auto const escaped_bits =
					  ( escape_and_terminal ^ ( backslash_bits | previous_escaped ) ) &
					  valid_bits;
					auto const escape_bits = escape_and_terminal & backslash_bits;
					escaped = last_bit( escape_bits, lane_count );

					auto const quote_bits =
					  ( input == splat( '"' ) ).to_ullong( ) & ~escaped_bits & valid_bits;
					auto const in_string_bits =
					  ( prefix_xor( quote_bits ) ^
					    ( in_string ? valid_bits : std::uint64_t{ 0 } ) ) &
					  valid_bits;
					in_string = last_bit( in_string_bits, lane_count );

					auto events = one_of<'{', '}', '[', ']', ','>( input ).to_ullong( ) &
					              ~( in_string_bits ^ quote_bits ) & ~escaped_bits &
					              valid_bits;
					while( events != 0 ) {
						auto const lane =
						  static_cast<std::size_t>( std::countr_zero( events ) );
						switch( ptr_first[lane] ) {
						case ',':
							if( ( primary_depth == 1 ) & ( secondary_depth == 0 ) ) {
								++count;
							}
							break;
						case primary_left:
							++primary_depth;
							break;
						case primary_right:
							--primary_depth;
							if( primary_depth == 0 ) {
								auto const end = ptr_first + lane + 1;
								if constexpr( not ParseState::is_unchecked_input ) {
									daw_json_ensure( secondary_depth == 0,
									                 ErrorReason::InvalidBracketing,
									                 parse_state );
								}
								result.last = end;
								result.counter = count;
								parse_state.first = end;
								return result;
							}
							break;
						case secondary_left:
							++secondary_depth;
							break;
						case secondary_right:
							--secondary_depth;
							break;
						default:
							DAW_UNREACHABLE( );
						}
						events &= events - 1U;
					}
					ptr_first += lane_count;
				}

				if constexpr( not ParseState::is_unchecked_input ) {
					daw_json_ensure(
					  not in_string, ErrorReason::UnexpectedEndOfData, parse_state );
					daw_json_ensure( ( primary_depth == 0 ) & ( secondary_depth == 0 ),
					                 ErrorReason::InvalidBracketing,
					                 parse_state );
				}
				result.last = ptr_first;
				result.counter = count;
				parse_state.first = ptr_first;
				return result;
			}
		} // namespace json_details
	} // namespace DAW_JSON_VER
} // namespace daw::json
#endif
