#pragma once

#include "version.h"

#include "daw_json_assert.h"
#include "daw_json_parse_common.h"
#include "daw_not_const_ex_functions.h"

#include <daw/daw_likely.h>

#include <ciso646>
#include <string>
#include <type_traits>

namespace daw::json {
	inline namespace DAW_JSON_VER {
		namespace json_details {
			[[nodiscard]] inline constexpr UInt8 to_nibble( unsigned char chr ) {
				int const b = static_cast<int>( chr );
				int const maskLetter = ( ( '9' - b ) >> 31 );
				int const maskSmall = ( ( 'Z' - b ) >> 31 );
				int const offset = '0' + ( maskLetter & int( 'A' - '0' - 10 ) ) +
				                   ( maskSmall & int( 'a' - 'A' ) );
				auto const result = static_cast<unsigned>( b - offset );
				return to_uint8( result );
			}

			template<bool is_unchecked_input>
			[[nodiscard]] inline constexpr UInt16
			byte_from_nibbles( char const *&first ) {
				auto const n0 = to_nibble( static_cast<unsigned char>( *first++ ) );
				auto const n1 = to_nibble( static_cast<unsigned char>( *first++ ) );
				if constexpr( is_unchecked_input ) {
					daw_json_assert( n0 < 16 and n1 < 16, ErrorReason::InvalidUTFEscape );
				}
				return to_uint16( ( n0 << 4U ) | n1 );
			}

			constexpr char u32toC( UInt32 value ) {
				return static_cast<char>( static_cast<unsigned char>( value ) );
			}

			template<typename ParseState>
			[[nodiscard]] static constexpr char *
			decode_utf16( ParseState &parse_state, char *it ) {
				constexpr bool is_unchecked_input = ParseState::is_unchecked_input;
				char const *first = parse_state.first;
				++first;
				UInt32 cp = to_uint32( byte_from_nibbles<is_unchecked_input>( first ) )
				            << 8U;
				cp |= byte_from_nibbles<is_unchecked_input>( first );
				if( cp <= 0x7FU ) {
					*it++ = static_cast<char>( static_cast<unsigned char>( cp ) );
					parse_state.first = first;
					return it;
				}

				//******************************
				if( 0xD800U <= cp and cp <= 0xDBFFU ) {
					cp = ( cp - 0xD800U ) * 0x400U;
					++first;
					daw_json_assert_weak(
					  *first == 'u', ErrorReason::InvalidUTFEscape,
					  parse_state ); // Expected parse_state to start with a \\u
					++first;
					auto trailing =
					  to_uint32( byte_from_nibbles<is_unchecked_input>( first ) ) << 8U;
					trailing |= byte_from_nibbles<is_unchecked_input>( first );
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
					parse_state.first = first;
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
					parse_state.first = first;
					return it;
				}
				//******************************
				// cp >= 0x80U
				// 2 bytes
				char const enc1 = u32toC( ( cp & 0b0011'1111U ) | 0b1000'0000U );
				char const enc0 = u32toC( ( cp >> 6U ) | 0b1100'0000U );
				*it++ = enc0;
				*it++ = enc1;
				parse_state.first = first;
				return it;
			}

			template<typename ParseState, typename Appender>
			static constexpr void decode_utf16( ParseState &parse_state,
			                                    Appender &app ) {
				constexpr bool is_unchecked_input = ParseState::is_unchecked_input;
				char const *first = parse_state.first;
				++first;
				UInt32 cp = to_uint32( byte_from_nibbles<is_unchecked_input>( first ) )
				            << 8U;
				cp |= byte_from_nibbles<is_unchecked_input>( first );
				if( cp <= 0x7FU ) {
					app( u32toC( cp ) );
					parse_state.first = first;
					return;
				}
				if( 0xD800U <= cp and cp <= 0xDBFFU ) {
					cp = ( cp - 0xD800U ) * 0x400U;
					++first;
					daw_json_assert_weak( *first == 'u', ErrorReason::InvalidUTFEscape,
					                      parse_state );
					++first;
					auto trailing =
					  to_uint32( byte_from_nibbles<is_unchecked_input>( first ) ) << 8U;
					trailing |= byte_from_nibbles<is_unchecked_input>( first );
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
					parse_state.first = first;
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
					parse_state.first = first;
					return;
				}
				// cp >= 0x80U
				// 2 bytes
				char const enc1 = u32toC( ( cp & 0b0011'1111U ) | 0b1000'0000U );
				char const enc0 = u32toC( ( cp >> 6U ) | 0b1100'0000U );
				app( enc0 );
				app( enc1 );
				parse_state.first = first;
			}

			namespace parse_tokens {
				inline constexpr char const escape_quotes[] = "\\\"";
			}

			// Fast path for parsing escaped strings to a std::string with the default
			// appender
			template<bool AllowHighEight, typename JsonMember, bool KnownBounds,
			         typename ParseState>
			[[nodiscard, maybe_unused]] constexpr auto // json_result<JsonMember>
			parse_string_known_stdstring( ParseState &parse_state ) {
				using string_type = json_base_type<JsonMember>;
				string_type result =
				  string_type( std::size( parse_state ), '\0',
				               parse_state.get_allocator_for( template_arg<char> ) );

				char *it = std::data( result );

				bool const has_quote = parse_state.front( ) == '"';
				if( has_quote ) {
					parse_state.remove_prefix( );
				}

				if( auto const first_slash =
				      static_cast<std::ptrdiff_t>( parse_state.counter ) - 1;
				    first_slash > 1 ) {
					it = std::copy_n( parse_state.first, first_slash, it );
					parse_state.first += first_slash;
				}
				constexpr auto pred = []( auto const &r ) {
					if constexpr( ParseState::is_unchecked_input ) {
						return DAW_LIKELY( r.front( ) != '"' );
					} else {
						return DAW_LIKELY( r.has_more( ) ) & ( r.front( ) != '"' );
					}
				};

				while( pred( parse_state ) ) {
					{
						char const *first = parse_state.first;
						char const *const last = parse_state.last;
						if constexpr( std::is_same<typename ParseState::exec_tag_t,
						                           constexpr_exec_tag>::value ) {
							while( not key_table<'"', '\\'>[*first] ) {
								++first;
								daw_json_assert_weak( KnownBounds or first < last,
								                      ErrorReason::UnexpectedEndOfData,
								                      parse_state );
							}
						} else {
							first =
							  mem_move_to_next_of<( ParseState::is_unchecked_input or
							                        ParseState::is_zero_terminated_string ),
							                      '"', '\\'>( ParseState::exec_tag, first,
							                                  last );
						}
						it = daw::algorithm::copy( parse_state.first, first, it );
						parse_state.first = first;
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
							if constexpr( not AllowHighEight ) {
								daw_json_assert_weak(
								  ( not parse_state.is_space_unchecked( ) ) &
								    ( static_cast<unsigned char>( parse_state.front( ) ) <=
								      0x7FU ),
								  ErrorReason::InvalidStringHighASCII, parse_state );
							}
							*it++ = parse_state.front( );
							parse_state.remove_prefix( );
						}
					} else {
						daw_json_assert_weak( not has_quote or
						                        parse_state.is_quotes_checked( ),
						                      ErrorReason::InvalidString, parse_state );
					}
					daw_json_assert_weak( not has_quote or parse_state.has_more( ),
					                      ErrorReason::UnexpectedEndOfData, parse_state );
				}
				auto const sz =
				  static_cast<std::size_t>( std::distance( std::data( result ), it ) );
				daw_json_assert_weak( std::size( result ) >= sz,
				                      ErrorReason::InvalidString, parse_state );
				result.resize( sz );
				if constexpr( std::is_convertible<string_type,
				                                  json_result<JsonMember>>::value ) {
					return result;
				} else {
					using constructor_t = typename JsonMember::constructor_t;
					construct_value( template_arg<json_result<JsonMember>>,
					                 constructor_t{ }, parse_state, std::data( result ),
					                 daw::data_end( result ) );
				}
			}
		} // namespace json_details
	}   // namespace DAW_JSON_VER
} // namespace daw::json
