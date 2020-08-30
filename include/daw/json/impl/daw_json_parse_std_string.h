#pragma once

#include "daw_json_assert.h"
#include "daw_json_parse_common.h"

#include <string>
#include <type_traits>

namespace daw::json::json_details {
	template<typename Range>
	[[nodiscard]] inline constexpr UInt8 to_nibble( unsigned char chr ) {
		int const b = static_cast<int>( chr );
		int const maskLetter = ( ( '9' - b ) >> 31 );
		int const maskSmall = ( ( 'Z' - b ) >> 31 );
		int const offset = '0' + ( maskLetter & int( 'A' - '0' - 10 ) ) +
		                   ( maskSmall & int( 'a' - 'A' ) );
		auto const result = static_cast<unsigned>( b - offset );
		daw_json_assert_weak( result < 16U, "Expected a hex nibble" );
		return to_uint8( result );
	}

	template<typename Range>
	[[nodiscard]] inline constexpr UInt16 byte_from_nibbles( Range &rng ) {
		auto const n0 =
		  to_nibble<Range>( static_cast<unsigned char>( rng.front( ) ) );
		rng.remove_prefix( );
		auto const n1 =
		  to_nibble<Range>( static_cast<unsigned char>( rng.front( ) ) );
		rng.remove_prefix( );
		return to_uint16( ( n0 << 4U ) | n1 );
	}

	constexpr char u32toC( UInt32 value ) {
		return static_cast<char>( static_cast<unsigned char>( value ) );
	};

	template<typename Range>
	[[nodiscard]] static constexpr char *decode_utf16( Range &rng, char *it ) {
		daw_json_assert_weak( rng.front( ) == 'u',
		                      "Expected rng to start with a u" );
		rng.remove_prefix( );
		UInt32 cp = to_uint32( byte_from_nibbles( rng ) ) << 8U;
		cp |= to_uint32( byte_from_nibbles( rng ) );
		if( cp <= 0x7FU ) {
			*it++ = static_cast<char>( static_cast<unsigned char>( cp ) );
			return it;
		}

		//******************************
		if( 0xD800U <= cp and cp <= 0xDBFFU ) {
			cp = ( cp - 0xD800U ) * 0x400U;
			rng.remove_prefix( );
			daw_json_assert_weak( rng.front( ) == 'u',
			                      "Expected rng to start with a \\u" );
			rng.remove_prefix( );
			auto trailing = to_uint32( byte_from_nibbles( rng ) ) << 8U;
			trailing |= to_uint32( byte_from_nibbles( rng ) );
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
			return it;
		}
		//******************************
		// cp >= 0x80U
		// 2 bytes
		char const enc1 = u32toC( ( cp & 0b0011'1111U ) | 0b1000'0000U );
		char const enc0 = u32toC( ( cp >> 6U ) | 0b1100'0000U );
		*it++ = enc0;
		*it++ = enc1;
		return it;
	}

	template<typename Range, typename Appender>
	static constexpr void decode_utf16( Range &rng, Appender &app ) {
		daw_json_assert_weak( rng.front( ) == 'u',
		                      "Expected rng to start with a \\u" );
		rng.remove_prefix( );
		UInt32 cp = to_uint32( byte_from_nibbles( rng ) ) << 8U;
		cp |= to_uint32( byte_from_nibbles( rng ) );
		if( cp <= 0x7FU ) {
			app( u32toC( cp ) );
			return;
		}
		if( 0xD800U <= cp and cp <= 0xDBFFU ) {
			cp = ( cp - 0xD800U ) * 0x400U;
			rng.remove_prefix( );
			daw_json_assert_weak( rng.front( ) == 'u',
			                      "Expected rng to start with a \\u" );
			rng.remove_prefix( );
			auto trailing = to_uint32( byte_from_nibbles( rng ) ) << 8U;
			trailing |= to_uint32( byte_from_nibbles( rng ) );
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
			return;
		}
		// cp >= 0x80U
		// 2 bytes
		char const enc1 = u32toC( ( cp & 0b0011'1111U ) | 0b1000'0000U );
		char const enc0 = u32toC( ( cp >> 6U ) | 0b1100'0000U );
		app( enc0 );
		app( enc1 );
	}

	namespace parse_tokens {
		inline constexpr char const escape_quotes[] = "\\\"";
	}

	// Fast path for parsing escaped strings to a std::string with the default
	// appender
	template<bool AllowHighEight, typename Result, typename Range>
	[[nodiscard, maybe_unused]] constexpr Result
	parse_string_known_stdstring( Range &rng ) {
		Result result2 = std::string( rng.size( ), '\0' );

		auto &result = [&result2]( ) -> std::string & {
			if constexpr( std::is_same_v<Result, std::string> ) {
				return result2;
			} else {
				return *result2;
			}
		}( );

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
		daw_json_assert(
		  rng.last < rng.class_last,
		  "Unabled to parse lone string, need space after to see \"" );
		while( rng.front( ) != '"' ) {
			{
				char const *first = rng.first;
				char const *const last = rng.last;
				while( *first != '"' and *first != '\\' ) {
					daw_json_assert_weak( first < last, "Unexpected end of data" );
					++first;
				}
				it = std::copy( rng.first, first, it );
				rng.first = first;
			}
			if( rng.front( ) == '\\' ) {
				daw_json_assert_weak( not rng.is_space_unchecked( ),
				                      "Invalid codepoint" );
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
				case '\\':
				case '/':
				case '"':
					*it++ = rng.front( );
					rng.remove_prefix( );
					break;
				default:
					if constexpr( not AllowHighEight ) {
						daw_json_assert_weak(
						  ( not rng.is_space_unchecked( ) ) &
						    ( static_cast<unsigned char>( rng.front( ) ) <= 0x7FU ),
						  "string support limited to 0x20 < chr <= 0x7F when "
						  "DisallowHighEightBit is true" );
					}
					*it++ = rng.front( );
					rng.remove_prefix( );
				}
			} else {
				daw_json_assert_weak( not has_quote or rng.is_quotes_checked( ),
				                      "Unexpected end of string" );
			}
			daw_json_assert_weak( not has_quote or rng.has_more( ),
			                      "Unexpected end of data" );
		}
		auto const sz =
		  static_cast<std::size_t>( std::distance( result.data( ), it ) );
		daw_json_assert_weak( result.size( ) >= sz, "Unexpected string state" );
		result.resize( sz );
		return result2;
	} // namespace daw::json::json_details
} // namespace daw::json::json_details
