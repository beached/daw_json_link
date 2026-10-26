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
#include "daw/json/impl/daw_json_lexer_cursor.h"
#include "daw/json/impl/daw_json_parse_policy_policy_details.h"
#include "daw/simd/daw_simd.h"

#include <daw/daw_attributes.h>
#include <daw/daw_cxmath.h>
#include <daw/daw_is_constant_evaluated.h>
#include <daw/daw_span.h>

#include <cstddef>
#include <cstdint>
#include <type_traits>

#if defined( DAW_HAS_SIMD )
namespace daw::json {
	inline namespace DAW_JSON_VER {
		namespace json_details {
			namespace skip_bracketed_item_simd_details {
				using lexer_cursor_details::last_bit;
				using lexer_cursor_details::low_bits;
				using lexer_cursor_details::one_of;
				using lexer_cursor_details::prefix_xor;
				using lexer_cursor_details::simd_type;
				using lexer_cursor_details::splat;
				static constexpr std::size_t block_size =
				  lexer_cursor_details::simd_size;
				[[nodiscard]] DAW_SIMD_CONSTEXPR simd_type load( char const *first,
				                                                 std::size_t count ) {
					if( count == block_size )
						return daw::simd::unchecked_load<simd_type>(
						  daw::span( first, block_size ) );
					return daw::simd::partial_load<simd_type>(
					  daw::span( first, count ) );
				}
			} // namespace skip_bracketed_item_simd_details

			/// Skip the JSON class or array at parse_state.first using SIMD to
			/// classify each block.
			template<SkipBracketedType BracketedType, typename ParseState>
			[[nodiscard]] DAW_ATTRIB_FLATTEN DAW_SIMD_CONSTEXPR ParseState
			skip_bracketed_item_simd( ParseState &parse_state ) {
				DAW_CPP23_STATIC_LOCAL constexpr char primary_left =
				  BracketedType == SkipBracketedType::Class ? '{' : '[';
				DAW_CPP23_STATIC_LOCAL constexpr char primary_right =
				  BracketedType == SkipBracketedType::Class ? '}' : ']';
				DAW_CPP23_STATIC_LOCAL constexpr char secondary_left =
				  BracketedType == SkipBracketedType::Class ? '[' : '{';
				DAW_CPP23_STATIC_LOCAL constexpr char secondary_right =
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

					DAW_CPP23_STATIC_LOCAL constexpr std::uint64_t odd_bits =
					  0xAAAAAAAAAAAAAAAAULL;
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
						  static_cast<std::size_t>( daw::cxmath::count_trailing_zeros(
						    static_cast<std::uint64_t>( events ) ) );
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
						events = daw::cxmath::blsr( events );
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

			/// Skip the JSON class or array at parse_state.first like
			/// skip_bracketed_item_simd, and also check that it is structurally
			/// valid: brackets close in order, members are a name, a colon and a
			/// value, and values are separated by commas.  Trailing commas are
			/// allowed.  Literals and numbers are checked by
			/// skip_scalar_token_checked
			template<SkipBracketedType BracketedType, typename ParseState>
			[[nodiscard]] DAW_ATTRIB_FLATTEN DAW_SIMD_CONSTEXPR ParseState
			skip_bracketed_item_simd_validated( ParseState &parse_state ) {
				DAW_CPP23_STATIC_LOCAL constexpr bool is_class =
				  BracketedType == SkipBracketedType::Class;
				// What may come next at the current depth
				enum : unsigned char {
					name_or_end,
					colon,
					value,
					value_or_end,
					comma_or_end,
					// Started part way through the item, after a value or a comma
					resume
				};

				using namespace skip_bracketed_item_simd_details;
				auto ptr_first = parse_state.first;
				auto const ptr_last = parse_state.last;
				if( DAW_UNLIKELY( ptr_first >= ptr_last ) ) {
					return parse_state;
				}

				auto result = parse_state;
				std::size_t count = 0;
				// Bit n is set when depth n + 1 is a class
				std::uint64_t kinds = is_class ? 1U : 0U;
				std::size_t depth = 1;
				unsigned char state = resume;
				bool in_string = false;
				bool escaped = false;
				bool in_token = false;

				if( *ptr_first == ( is_class ? '{' : '[' ) ) {
					++ptr_first;
					state = is_class ? name_or_end : value_or_end;
				}

				while( ptr_first < ptr_last ) {
					auto const remaining =
					  static_cast<std::size_t>( ptr_last - ptr_first );
					auto const lane_count =
					  remaining < block_size ? remaining : block_size;
					auto const input =
					  lexer_cursor_details::load( ptr_first, lane_count );
					auto const valid_bits = low_bits( lane_count );
					auto const backslash_bits =
					  ( input == splat( '\\' ) ).to_ullong( ) & valid_bits;

					DAW_CPP23_STATIC_LOCAL constexpr std::uint64_t odd_bits =
					  0xAAAAAAAAAAAAAAAAULL;
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

					auto const outside = ~( in_string_bits | quote_bits ) & valid_bits;
					auto const whitespace =
					  ( ( input >= splat( '\x01' ) ) & ( input <= splat( '\x20' ) ) )
					    .to_ullong( );
					auto const structural =
					  lexer_cursor_details::structural_candidates( input ).to_ullong( ) &
					  outside & ~escaped_bits & ~whitespace;
					// Bytes of literals and numbers, an event is raised at the start of
					// each run
					auto const token = outside & ~structural & ~whitespace;
					auto const token_start =
					  token &
					  ~( ( token << 1U ) | ( in_token ? std::uint64_t{ 1 } : 0 ) );
					in_token = last_bit( token, lane_count );
					auto const open_quotes = quote_bits & in_string_bits;

					auto events = structural | open_quotes | token_start;
					while( events != 0 ) {
						auto const lane =
						  static_cast<std::size_t>( daw::cxmath::count_trailing_zeros(
						    static_cast<std::uint64_t>( events ) ) );
						auto const at_class = ( ( kinds >> ( depth - 1U ) ) & 1U ) != 0;
						// Where a value may start: after a colon, at the start of an array
						// or after its commas, or when resuming in an array
						auto const value_allowed = ( state == value ) |
						                           ( state == value_or_end ) |
						                           ( ( state == resume ) & not at_class );
						switch( ptr_first[lane] ) {
						case '"':
							if( ( state == name_or_end ) |
							    ( ( state == resume ) & at_class ) ) {
								state = colon;
							} else {
								daw_json_ensure( value_allowed,
								                 ErrorReason::ExpectedTokenNotFound,
								                 parse_state );
								state = comma_or_end;
							}
							break;
						case ':':
							daw_json_ensure( state == colon,
							                 ErrorReason::ExpectedTokenNotFound,
							                 parse_state );
							state = value;
							break;
						case ',':
							daw_json_ensure( ( state == comma_or_end ) | ( state == resume ),
							                 ErrorReason::InvalidStartOfValue,
							                 parse_state );
							if( depth == 1 ) {
								++count;
							}
							state = at_class ? name_or_end : value_or_end;
							break;
						case '{':
						case '[':
							daw_json_ensure( value_allowed,
							                 state == colon
							                   ? ErrorReason::ExpectedTokenNotFound
							                   : ErrorReason::MissingMemberName,
							                 parse_state );
							daw_json_ensure(
							  depth < 64, ErrorReason::MaxDepthExceeded, parse_state );
							if( ptr_first[lane] == '{' ) {
								kinds |= std::uint64_t{ 1 } << depth;
								state = name_or_end;
							} else {
								kinds &= ~( std::uint64_t{ 1 } << depth );
								state = value_or_end;
							}
							++depth;
							break;
						case '}':
						case ']':
							daw_json_ensure( at_class == ( ptr_first[lane] == '}' ),
							                 ErrorReason::InvalidBracketing,
							                 parse_state );
							daw_json_ensure(
							  ( state == comma_or_end ) | ( state == resume ) |
							    ( state == ( at_class ? name_or_end : value_or_end ) ),
							  state == colon ? ErrorReason::ExpectedTokenNotFound
							                 : ErrorReason::InvalidStartOfValue,
							  parse_state );
							--depth;
							if( depth == 0 ) {
								auto const end = ptr_first + lane + 1;
								result.last = end;
								result.counter = count;
								parse_state.first = end;
								return result;
							}
							state = comma_or_end;
							break;
						default:
							// The start of a literal or number
							daw_json_ensure( value_allowed,
							                 state == name_or_end
							                   ? ErrorReason::MissingMemberName
							                   : ErrorReason::ExpectedTokenNotFound,
							                 parse_state );
							state = comma_or_end;
							(void)parse_policy_details::skip_scalar_token_checked(
							  ptr_first + lane, ptr_last, parse_state );
							break;
						}
						events = daw::cxmath::blsr( events );
					}
					ptr_first += lane_count;
				}
				daw_json_ensure(
				  not in_string, ErrorReason::UnexpectedEndOfData, parse_state );
				// Data ended before the item was closed
				daw_json_error( true, ErrorReason::InvalidBracketing, parse_state );
			}
		} // namespace json_details
	} // namespace DAW_JSON_VER
} // namespace daw::json
#endif
