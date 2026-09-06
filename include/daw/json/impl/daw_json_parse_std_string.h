// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/daw_json_link
//

#pragma once

#include "daw/json/impl/version.h"

#include "daw/json/daw_json_switches.h"
#include "daw/json/impl/daw_json_assert.h"
#include "daw/json/impl/daw_json_parse_common.h"
#include "daw/json/impl/daw_not_const_ex_functions.h"

#include <daw/algorithms/daw_algorithm_copy.h>
#include <daw/algorithms/daw_algorithm_copy_n.h>
#include <daw/daw_data_end.h>
#include <daw/daw_likely.h>
#include <daw/daw_not_null.h>
#include <daw/daw_span.h>

#include <cstddef>
#include <daw/stdinc/data_access.h>
#include <daw/stdinc/range_access.h>
#include <type_traits>

namespace daw::json {
	inline namespace DAW_JSON_VER {
		namespace json_details {
			[[nodiscard]] static constexpr UInt8 to_nibble( unsigned char chr ) {
				int const b = static_cast<int>( chr );
				int const maskLetter = ( ( '9' - b ) >> 31 );
				int const maskSmall = ( ( 'Z' - b ) >> 31 );
				int const offset = '0' + ( maskLetter & int( 'A' - '0' - 10 ) ) +
				                   ( maskSmall & int( 'a' - 'A' ) );
				auto const result = static_cast<unsigned>( b - offset );
				return to_uint8( result );
			}

			template<bool is_unchecked_input>
			[[nodiscard]] static constexpr UInt16
			byte_from_nibbles( daw::not_null<char const *> &first ) {
				auto const n0 = to_nibble( static_cast<unsigned char>( *first++ ) );
				auto const n1 = to_nibble( static_cast<unsigned char>( *first++ ) );
				if constexpr( not is_unchecked_input ) {
					daw_json_ensure( n0 < 16 and n1 < 16, ErrorReason::InvalidUTFEscape );
				}
				return to_uint16( ( n0 << 4U ) | n1 );
			}

			static constexpr char u32toC( UInt32 value ) {
				return static_cast<char>( static_cast<unsigned char>( value ) );
			}

			template<typename ParseState>
			[[nodiscard]] static constexpr daw::not_null<char *>
			decode_utf16( ParseState &parse_state, daw::not_null<char *> it ) {
				daw_json_assert_weak( parse_state.size( ) >= 5,
				                      ErrorReason::UnexpectedEndOfData,
				                      parse_state );
				auto first = daw::not_null<char const *>( parse_state.first );
				++first;
				UInt32 cp =
				  to_uint32(
				    byte_from_nibbles<ParseState::is_unchecked_input>( first ) )
				  << 8U;
				cp |= byte_from_nibbles<ParseState::is_unchecked_input>( first );
				if( cp <= 0x7FU ) {
					*it++ = static_cast<char>( static_cast<unsigned char>( cp ) );
					parse_state.first = input_pointer( parse_state.first, first.get( ) );
					return it;
				}

				//******************************
				daw_json_assert_weak(
				  cp < 0xDC00U or cp > 0xDFFFU, ErrorReason::InvalidUTFEscape,
				  parse_state ); // Lone/unpaired low surrogate
				if( 0xD800U <= cp and cp <= 0xDBFFU ) {
					cp = ( cp - 0xD800U ) * 0x400U;
					daw_json_assert_weak(
					  *first == '\\', ErrorReason::InvalidUTFEscape,
					  parse_state ); // High surrogate must be followed by \\u
					++first;
					daw_json_assert_weak(
					  ( parse_state.last - first >= 5 ) and *first == 'u',
					  ErrorReason::InvalidUTFEscape,
					  parse_state ); // Expected parse_state to start with a \\u
					++first;
					auto trailing =
					  to_uint32(
					    byte_from_nibbles<ParseState::is_unchecked_input>( first ) )
					  << 8U;
					trailing |=
					  byte_from_nibbles<ParseState::is_unchecked_input>( first );
					daw_json_assert_weak(
					  0xDC00U <= trailing and trailing <= 0xDFFFU,
					  ErrorReason::InvalidUTFEscape,
					  parse_state ); // Expected a low surrogate to complete the pair
					trailing -= 0xDC00U;
					cp += trailing;
					cp += 0x10000;
				}
				// UTF32-> UTF8
				if( cp >= 0x10000U ) {
					// 4 bytes
					char const enc3 = u32toC( ( cp & 0b0011'1111U ) | 0b1000'0000U );
					char const enc2 =
					  u32toC( ( ( cp >> 6U ) & 0b0011'1111U ) | 0b1000'0000U );
					char const enc1 =
					  u32toC( ( ( cp >> 12U ) & 0b0011'1111U ) | 0b1000'0000U );
					char const enc0 = u32toC( ( cp >> 18U ) | 0b1111'0000U );
					*it++ = enc0;
					*it++ = enc1;
					*it++ = enc2;
					*it++ = enc3;
					parse_state.first = input_pointer( parse_state.first, first.get( ) );
					return it;
				}
				//******************************
				if( cp >= 0x800U ) {
					// 3 bytes
					char const enc2 = u32toC( ( cp & 0b0011'1111U ) | 0b1000'0000U );
					char const enc1 =
					  u32toC( ( ( cp >> 6U ) & 0b0011'1111U ) | 0b1000'0000U );
					char const enc0 = u32toC( ( cp >> 12U ) | 0b1110'0000U );
					*it++ = enc0;
					*it++ = enc1;
					*it++ = enc2;
					parse_state.first = input_pointer( parse_state.first, first.get( ) );
					return it;
				}
				//******************************
				// cp >= 0x80U
				// 2 bytes
				char const enc1 = u32toC( ( cp & 0b0011'1111U ) | 0b1000'0000U );
				char const enc0 = u32toC( ( cp >> 6U ) | 0b1100'0000U );
				*it++ = enc0;
				*it++ = enc1;
				parse_state.first = input_pointer( parse_state.first, first.get( ) );
				return it;
			}

			template<typename ParseState, typename Appender>
			static constexpr void decode_utf16( ParseState &parse_state,
			                                    Appender &app ) {
				auto first = daw::not_null<char const *>( parse_state.first );
				++first;
				UInt32 cp =
				  to_uint32(
				    byte_from_nibbles<ParseState::is_unchecked_input>( first ) )
				  << 8U;
				cp |= byte_from_nibbles<ParseState::is_unchecked_input>( first );
				if( cp <= 0x7FU ) {
					app( u32toC( cp ) );
					parse_state.first = input_pointer( parse_state.first, first.get( ) );
					return;
				}
				daw_json_assert_weak(
				  cp < 0xDC00U or cp > 0xDFFFU, ErrorReason::InvalidUTFEscape,
				  parse_state ); // Lone/unpaired low surrogate
				if( 0xD800U <= cp and cp <= 0xDBFFU ) {
					cp = ( cp - 0xD800U ) * 0x400U;
					daw_json_assert_weak(
					  *first == '\\', ErrorReason::InvalidUTFEscape,
					  parse_state ); // High surrogate must be followed by \\u
					++first;
					daw_json_assert_weak(
					  *first == 'u', ErrorReason::InvalidUTFEscape, parse_state );
					++first;
					auto trailing =
					  to_uint32(
					    byte_from_nibbles<ParseState::is_unchecked_input>( first ) )
					  << 8U;
					trailing |=
					  byte_from_nibbles<ParseState::is_unchecked_input>( first );
					daw_json_assert_weak(
					  0xDC00U <= trailing and trailing <= 0xDFFFU,
					  ErrorReason::InvalidUTFEscape,
					  parse_state ); // Expected a low surrogate to complete the pair
					trailing -= 0xDC00U;
					cp += trailing;
					cp += 0x10000;
				}
				// UTF32-> UTF8
				if( cp >= 0x10000U ) {
					// 4 bytes
					char const enc3 = u32toC( ( cp & 0b0011'1111U ) | 0b1000'0000U );
					char const enc2 =
					  u32toC( ( ( cp >> 6U ) & 0b0011'1111U ) | 0b1000'0000U );
					char const enc1 =
					  u32toC( ( ( cp >> 12U ) & 0b0011'1111U ) | 0b1000'0000U );
					char const enc0 = u32toC( ( cp >> 18U ) | 0b1111'0000U );
					app( enc0 );
					app( enc1 );
					app( enc2 );
					app( enc3 );
					parse_state.first = input_pointer( parse_state.first, first.get( ) );
					return;
				}
				if( cp >= 0x800U ) {
					// 3 bytes
					char const enc2 = u32toC( ( cp & 0b0011'1111U ) | 0b1000'0000U );
					char const enc1 =
					  u32toC( ( ( cp >> 6U ) & 0b0011'1111U ) | 0b1000'0000U );
					char const enc0 = u32toC( ( cp >> 12U ) | 0b1110'0000U );
					app( enc0 );
					app( enc1 );
					app( enc2 );
					parse_state.first = input_pointer( parse_state.first, first.get( ) );
					return;
				}
				// cp >= 0x80U
				// 2 bytes
				char const enc1 = u32toC( ( cp & 0b0011'1111U ) | 0b1000'0000U );
				char const enc0 = u32toC( ( cp >> 6U ) | 0b1100'0000U );
				app( enc0 );
				app( enc1 );
				parse_state.first = input_pointer( parse_state.first, first.get( ) );
			}

			namespace parse_tokens {
				inline constexpr char escape_quotes[] = "\\\"";
			}

			// Fast path for parsing escaped strings to a std::string with the default
			// appender
			template<bool AllowHighEight, typename JsonMember, bool KnownBounds,
			         typename ParseState>
			[[nodiscard]] constexpr json_result_t<JsonMember>
			parse_string_known_stdstring( ParseState &parse_state ) {
				DAW_CPP23_STATIC_LOCAL constexpr bool is_insitu =
				  JsonMember::expected_type == JsonParseTypes::StringInsitu;

				static_assert(
				  not is_insitu or ParseState::allow_string_mutation,
				  "In-situ strings can only be parsed from writable input documents" );
				static_assert( not is_insitu or
				                 std::is_same_v<typename ParseState::iterator, char *>,
				               "In-situ strings require a mutable character buffer" );

				bool const has_quote = parse_state.front( ) == '"';
				if( has_quote ) {
					parse_state.remove_prefix( );
				}

				using string_type = json_base_type_t<JsonMember>;
				using result_t =
				  std::conditional_t<is_insitu, daw::span<char>, string_type>;
				result_t result = [&] {
					if constexpr( is_insitu ) {
						return daw::span( std::data( parse_state ),
						                  std::size( parse_state ) );
					} else {
						return string_type(
						  std::size( parse_state ) + 1U +
						    static_cast<unsigned>( has_quote ),
						  '\0',
						  parse_state.template get_allocator_for<char>( ) );
					}
				}( );
				daw::not_null<char *> it = std::data( result );

				if( auto const first_slash =
				      static_cast<std::ptrdiff_t>( parse_state.counter ) - 1;
				    first_slash > 1 ) {
					if constexpr( is_insitu ) {
						it += first_slash;
					} else {
						it =
						  daw::algorithm::copy_n( parse_state.first,
						                          it.get( ),
						                          static_cast<std::size_t>( first_slash ) )
						    .output;
					}
					parse_state.first += first_slash;
				}

				DAW_CPP23_STATIC_LOCAL constexpr auto in_json_string =
				  []( auto const &r ) DAW_JSON_CPP23_STATIC_CALL_OP -> bool {
					if constexpr( KnownBounds or not ParseState::is_unchecked_input ) {
						if( not DAW_LIKELY( r.has_more( ) ) ) {
							return false;
						}
					}
					return DAW_LIKELY( r.front( ) != '"' );
				};

				while( in_json_string( parse_state ) ) {
					{
						daw::not_null<char const *> first = parse_state.first;
						daw::not_null<char const *> const last = parse_state.last;

						if( not json_details::use_constexpr_exec_mode<
						      typename ParseState::exec_tag_t>( ) ) {
							first = mem_move_to_next_of<
							  ( not KnownBounds and
							    ( ParseState::is_unchecked_input or
							      ParseState::is_zero_terminated_string ) ),
							  typename ParseState::exec_tag_t,
							  '"',
							  '\\'>( first, last );
						} else {
							while( first < last and *first != '"' and *first != '\\' ) {
								++first;
							}
							daw_json_assert_weak( KnownBounds or first < last,
							                      ErrorReason::UnexpectedEndOfData,
							                      parse_state );
						}

						auto const run_size = first.get( ) - parse_state.first;
						daw_json_assert_weak(
						  static_cast<std::ptrdiff_t>( result.size( ) ) -
						      ( it.get( ) - result.data( ) ) >=
						    run_size,
						  ErrorReason::UnexpectedEndOfData );

						if( it.get( ) == parse_state.first ) {
							it += run_size;
						} else {
							it = daw::algorithm::copy(
							  parse_state.first, first.get( ), it.get( ) );
						}
						parse_state.first =
						  input_pointer( parse_state.first, first.get( ) );
					}
					if( parse_state.front( ) == '\\' ) {
						parse_state.remove_prefix( );
						daw_json_assert_weak( not parse_state.is_space_unchecked( ),
						                      ErrorReason::InvalidUTFCodepoint,
						                      parse_state );
						switch( parse_state.front( ) ) {
						case 'b':
							*it++ = '\b';
							parse_state.remove_prefix( );
							break;
						case 'f':
							*it++ = '\f';
							parse_state.remove_prefix( );
							break;
						case 'n':
							*it++ = '\n';
							parse_state.remove_prefix( );
							break;
						case 'r':
							*it++ = '\r';
							parse_state.remove_prefix( );
							break;
						case 't':
							*it++ = '\t';
							parse_state.remove_prefix( );
							break;
						case 'u':
							it = decode_utf16( parse_state, it );
							break;
						case '/':
						case '\\':
						case '"':
							*it++ = parse_state.front( );
							parse_state.remove_prefix( );
							break;
						default:
							// Every legal escape character (", \, /, b, f, n, r, t, u) is
							// handled by an explicit case above; anything else following a
							// backslash is not a valid JSON escape.
							daw_json_assert_weak( false, ErrorReason::InvalidString,
							                      parse_state );
							*it++ = parse_state.front( );
							parse_state.remove_prefix( );
						}
					} else {
						daw_json_assert_weak( not has_quote or
						                        parse_state.is_quotes_checked( ),
						                      ErrorReason::InvalidString,
						                      parse_state );
					}
					daw_json_assert_weak( not has_quote or parse_state.has_more( ),
					                      ErrorReason::UnexpectedEndOfData,
					                      parse_state );
				}
				auto const sz = static_cast<std::size_t>(
				  std::distance( std::data( result ), it.get( ) ) );
				daw_json_assert_weak(
				  std::size( result ) >= sz, ErrorReason::InvalidString, parse_state );

				if constexpr( is_insitu ) {
					*it = '"';
					++it;
					while( it != ( parse_state.last + 1 ) ) {
						*it = ' ';
						++it;
					}
					result = result.subspan( 0, sz );
				} else {
					result.resize( sz );
				}

				if constexpr( not is_insitu and
				              std::is_convertible_v<result_t,
				                                    json_result_t<JsonMember>> ) {
					return result;
				} else {
					using constructor_t = json_constructor_t<JsonMember>;
					return construct_value<json_result_t<JsonMember>, constructor_t>(
					  parse_state, std::data( result ), std::size( result ) );
				}
			}
		} // namespace json_details
	} // namespace DAW_JSON_VER
} // namespace daw::json
