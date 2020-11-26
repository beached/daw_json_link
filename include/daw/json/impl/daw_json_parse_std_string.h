#pragma once

#include "daw_json_assert.h"
#include "daw_json_parse_common.h"

#include <ciso646>
#include <string>
#include <type_traits>

namespace daw::json::json_details {
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

	template<typename Range>
	[[nodiscard]] static constexpr char *decode_utf16( Range &rng, char *it ) {
		constexpr bool is_unchecked_input = Range::is_unchecked_input;
		char const *first = rng.first;
		++first;
		UInt32 cp = to_uint32( byte_from_nibbles<is_unchecked_input>( first ) )
		            << 8U;
		cp |= byte_from_nibbles<is_unchecked_input>( first );
		if( cp <= 0x7FU ) {
			*it++ = static_cast<char>( static_cast<unsigned char>( cp ) );
			rng.first = first;
			return it;
		}

		//******************************
		if( 0xD800U <= cp and cp <= 0xDBFFU ) {
			cp = ( cp - 0xD800U ) * 0x400U;
			++first;
			daw_json_assert_weak( *first == 'u', ErrorReason::InvalidUTFEscape,
			                      rng ); // Expected rng to start with a \\u
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
			rng.first = first;
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
			rng.first = first;
			return it;
		}
		//******************************
		// cp >= 0x80U
		// 2 bytes
		char const enc1 = u32toC( ( cp & 0b0011'1111U ) | 0b1000'0000U );
		char const enc0 = u32toC( ( cp >> 6U ) | 0b1100'0000U );
		*it++ = enc0;
		*it++ = enc1;
		rng.first = first;
		return it;
	}

	template<typename Range, typename Appender>
	static constexpr void decode_utf16( Range &rng, Appender &app ) {
		constexpr bool is_unchecked_input = Range::is_unchecked_input;
		char const *first = rng.first;
		++first;
		UInt32 cp = to_uint32( byte_from_nibbles<is_unchecked_input>( first ) )
		            << 8U;
		cp |= byte_from_nibbles<is_unchecked_input>( first );
		if( cp <= 0x7FU ) {
			app( u32toC( cp ) );
			rng.first = first;
			return;
		}
		if( 0xD800U <= cp and cp <= 0xDBFFU ) {
			cp = ( cp - 0xD800U ) * 0x400U;
			++first;
			daw_json_assert_weak( *first == 'u', ErrorReason::InvalidUTFEscape, rng );
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
			rng.first = first;
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
			rng.first = first;
			return;
		}
		// cp >= 0x80U
		// 2 bytes
		char const enc1 = u32toC( ( cp & 0b0011'1111U ) | 0b1000'0000U );
		char const enc0 = u32toC( ( cp >> 6U ) | 0b1100'0000U );
		app( enc0 );
		app( enc1 );
		rng.first = first;
	}

	namespace parse_tokens {
		inline constexpr char const escape_quotes[] = "\\\"";
	}

	// Fast path for parsing escaped strings to a std::string with the default
	// appender
	template<bool AllowHighEight, typename JsonMember, bool KnownBounds,
	         typename Range>
	[[nodiscard, maybe_unused]] constexpr json_result<JsonMember>
	parse_string_known_stdstring( Range &rng ) {
		using string_type =
		  std::basic_string<char, std::char_traits<char>,
		                    typename Range::template allocator_type_as<char>>;
		string_type result =
		  string_type( rng.size( ), '\0', rng.template get_allocator_for<char>( ) );

		char *it = result.data( );

		bool const has_quote = rng.front( ) == '"';
		if( has_quote ) {
			rng.remove_prefix( );
		}

		if( auto const first_slash = static_cast<std::ptrdiff_t>( rng.counter ) - 1;
		    first_slash > 1 ) {
			it = std::copy_n( rng.first, first_slash, it );
			rng.first += first_slash;
		}
		while(
		  ( Range::is_unchecked_input or DAW_JSON_LIKELY( rng.has_more( ) ) ) and
		  rng.front( ) != '"' ) {
			{
				char const *first = rng.first;
				char const *const last = rng.last;
				if constexpr( std::is_same_v<typename Range::exec_tag_t,
				                             constexpr_exec_tag> ) {
					while( not key_table<'"', '\\'>[*first] ) {
						++first;
						daw_json_assert_weak( KnownBounds or first < last,
						                      ErrorReason::UnexpectedEndOfData, rng );
					}
				} else {
					first = mem_move_to_next_of<Range::is_unchecked_input, '"', '\\'>(
					  Range::exec_tag, first, last );
				}
				it = std::copy( rng.first, first, it );
				rng.first = first;
			}
			if( rng.front( ) == '\\' ) {
				daw_json_assert_weak( not rng.is_space_unchecked( ),
				                      ErrorReason::InvalidUTFCodepoint, rng );
				rng.remove_prefix( );
				switch( rng.front( ) ) {
				case 'b':
					*it++ = '\b';
					rng.remove_prefix( );
					break;
				case 'f':
					*it++ = '\f';
					rng.remove_prefix( );
					break;
				case 'n':
					*it++ = '\n';
					rng.remove_prefix( );
					break;
				case 'r':
					*it++ = '\r';
					rng.remove_prefix( );
					break;
				case 't':
					*it++ = '\t';
					rng.remove_prefix( );
					break;
				case 'u':
					it = decode_utf16( rng, it );
					break;
				case '/':
				case '\\':
				case '"':
					*it++ = rng.front( );
					rng.remove_prefix( );
					break;
				default:
					if constexpr( not AllowHighEight ) {
						daw_json_assert_weak(
						  ( not rng.is_space_unchecked( ) ) &
						    ( static_cast<unsigned char>( rng.front( ) ) <= 0x7FU ),
						  ErrorReason::InvalidStringHighASCII, rng );
					}
					*it++ = rng.front( );
					rng.remove_prefix( );
				}
			} else {
				daw_json_assert_weak( not has_quote or rng.is_quotes_checked( ),
				                      ErrorReason::InvalidString, rng );
			}
			daw_json_assert_weak( not has_quote or rng.has_more( ),
			                      ErrorReason::UnexpectedEndOfData, rng );
		}
		auto const sz =
		  static_cast<std::size_t>( std::distance( result.data( ), it ) );
		daw_json_assert_weak( result.size( ) >= sz, ErrorReason::InvalidString,
		                      rng );
		result.resize( sz );
		if constexpr( std::is_convertible_v<string_type,
		                                    json_result<JsonMember>> ) {
			return result;
		} else {
			using constructor_t = typename JsonMember::constructor_t;
			construct_value<json_result<JsonMember>>(
			  constructor_t{ }, rng, result.data( ),
			  result.data( ) + static_cast<std::ptrdiff_t>( result.size( ) ) );
		}
	}
} // namespace daw::json::json_details
