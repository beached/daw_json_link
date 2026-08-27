// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/daw_json_link
//

#pragma once

#include "daw/json/daw_json_switches.h"
#include "daw/json/impl/version.h"

#if defined( DAW_ALLOW_NEON )

#include "daw/json/impl/daw_json_assert.h"
#include "daw/json/impl/daw_json_parse_policy_policy_details.h"

#include <daw/daw_attributes.h>

#include <arm_neon.h>

#include <cstddef>
#include <cstdint>
#include <cstring>

namespace daw::json {
	inline namespace DAW_JSON_VER {
		namespace json_details {
			namespace skip_bracketed_item_neon_details {
				inline constexpr std::size_t block_size = 16;

				[[nodiscard]] DAW_ATTRIB_INLINE std::uint32_t
				movemask( uint8x16_t comparison ) noexcept {
					alignas( 16 ) static constexpr std::uint8_t bit_values[16] = {
					  0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80,
					  0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80 };
					auto const bits = vandq_u8( comparison, vld1q_u8( bit_values ) );
					auto const pairs = vpaddlq_u8( bits );
					auto const quads = vpaddlq_u16( pairs );
					auto const octets = vpaddlq_u32( quads );
					return static_cast<std::uint32_t>( vgetq_lane_u64( octets, 0 ) ) |
					       ( static_cast<std::uint32_t>(
					           vgetq_lane_u64( octets, 1 ) )
					         << 8U );
				}

				[[nodiscard]] DAW_ATTRIB_INLINE std::uint32_t
				find_char( uint8x16_t input, char value ) noexcept {
					return movemask( vceqq_u8(
					  input, vdupq_n_u8( static_cast<std::uint8_t>( value ) ) ) );
				}

				[[nodiscard]] DAW_ATTRIB_INLINE std::size_t
				first_bit( std::uint32_t bits ) noexcept {
#if defined( __GNUC__ ) or defined( __clang__ )
					return static_cast<std::size_t>( __builtin_ctz( bits ) );
#else
					std::size_t result = 0;
					while( ( bits & 1U ) == 0 ) {
						bits >>= 1U;
						++result;
					}
					return result;
#endif
				}

				[[nodiscard]] DAW_ATTRIB_INLINE uint8x16_t
				load( char const *first, std::size_t count ) noexcept {
					if( count == block_size ) {
						return vld1q_u8(
						  reinterpret_cast<std::uint8_t const *>( first ) );
					}
					auto result = vdupq_n_u8( 0 );
					std::memcpy( &result, first, count );
					return result;
				}
			} // namespace skip_bracketed_item_neon_details

			/// Skip the JSON class or array at parse_state.first using ARM NEON.
			/// This is a runtime-only implementation; callers must not invoke it
			/// during constant evaluation.
			template<SkipBracketedType BracketedType, typename ParseState>
			[[nodiscard]] DAW_ATTRIB_FLATTEN ParseState
			skip_bracketed_item_neon( ParseState &parse_state ) {
				constexpr char primary_left =
				  BracketedType == SkipBracketedType::Class ? '{' : '[';
				constexpr char primary_right =
				  BracketedType == SkipBracketedType::Class ? '}' : ']';
				constexpr char secondary_left =
				  BracketedType == SkipBracketedType::Class ? '[' : '{';
				constexpr char secondary_right =
				  BracketedType == SkipBracketedType::Class ? ']' : '}';

				using namespace skip_bracketed_item_neon_details;
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
					auto const valid_bits =
					  ( std::uint32_t{ 1 } << lane_count ) - std::uint32_t{ 1 };
					auto const backslash_bits = find_char( input, '\\' ) & valid_bits;

					constexpr std::uint32_t odd_bits = 0xAAAAU;
					auto const previous_escaped = escaped ? std::uint32_t{ 1 } : 0U;
					auto const potential_escape = backslash_bits & ~previous_escaped;
					auto const maybe_escaped = potential_escape << 1U;
					auto const escape_and_terminal =
					  ( ( maybe_escaped | odd_bits ) - potential_escape ) ^ odd_bits;
					auto const escaped_bits =
					  ( escape_and_terminal ^ ( backslash_bits | previous_escaped ) ) &
					  valid_bits;
					auto const escape_bits = escape_and_terminal & backslash_bits;
					escaped = ( ( escape_bits >> ( lane_count - 1U ) ) &
					            std::uint32_t{ 1 } ) != 0;

					auto const quote_bits =
					  find_char( input, '"' ) & ~escaped_bits & valid_bits;
					auto in_string_bits = quote_bits;
					in_string_bits ^= in_string_bits << 1U;
					in_string_bits ^= in_string_bits << 2U;
					in_string_bits ^= in_string_bits << 4U;
					in_string_bits ^= in_string_bits << 8U;
					in_string_bits =
					  ( in_string_bits ^ ( in_string ? valid_bits : 0U ) ) & valid_bits;
					in_string =
					  ( ( in_string_bits >> ( lane_count - 1U ) ) & 1U ) != 0;

					auto events = ( find_char( input, '{' ) | find_char( input, '}' ) |
					                find_char( input, '[' ) | find_char( input, ']' ) |
					                find_char( input, ',' ) ) &
					              ~( in_string_bits ^ quote_bits ) & ~escaped_bits &
					              valid_bits;
					while( events != 0 ) {
						auto const lane = first_bit( events );
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
