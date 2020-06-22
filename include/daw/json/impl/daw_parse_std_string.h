#pragma once

#include "daw_json_assert.h"
#include "daw_json_parse_common.h"

#include <string>
#include <type_traits>

namespace daw::json::json_details {
	template<bool IsUncheckedInput>
	struct RangeHelper {
		static constexpr bool is_unchecked_input = IsUncheckedInput;
	};

	template<bool IsUncheckedInput>
	[[nodiscard]] inline constexpr unsigned to_nibble( unsigned chr ) {
		using Range = RangeHelper<IsUncheckedInput>;
		auto const b = static_cast<int>( chr );
		int const maskLetter = ( ( '9' - b ) >> 31 );
		int const maskSmall = ( ( 'Z' - b ) >> 31 );
		int const offset = '0' + ( maskLetter & int( 'A' - '0' - 10 ) ) +
		                   ( maskSmall & int( 'a' - 'A' ) );
		auto const result = static_cast<unsigned>( b - offset );
		daw_json_assert_weak( result < 16U, "Expected a hex nibble" );
		return result;
	}

	static_assert( to_nibble<true>( static_cast<unsigned>( '0' ) ) == 0U );
	static_assert( to_nibble<true>( static_cast<unsigned>( '9' ) ) == 9U );
	static_assert( to_nibble<true>( static_cast<unsigned>( 'a' ) ) == 10U );
	static_assert( to_nibble<true>( static_cast<unsigned>( 'A' ) ) == 10U );
	static_assert( to_nibble<true>( static_cast<unsigned>( 'f' ) ) == 15U );
	static_assert( to_nibble<true>( static_cast<unsigned>( 'F' ) ) == 15U );

	template<typename Range>
	[[nodiscard]] inline constexpr std::uint16_t byte_from_nibbles( Range &rng ) {
		auto const n0 = to_nibble<Range::is_unchecked_input>(
		  static_cast<unsigned>( rng.front( ) ) );
		rng.remove_prefix( );
		auto const n1 = to_nibble<Range::is_unchecked_input>(
		  static_cast<unsigned>( rng.front( ) ) );
		rng.remove_prefix( );
		return static_cast<std::uint16_t>( ( n0 << 4U ) | n1 );
	}

	template<typename Range>
	[[nodiscard]] static constexpr char *decode_utf16( Range &rng, char *it ) {
		daw_json_assert_weak( rng.front( "uU" ), "Expected rng to start with a u" );
		rng.remove_prefix( );
		std::uint32_t cp = static_cast<std::uint32_t>( byte_from_nibbles( rng ) )
		                   << 8U;
		cp |= static_cast<std::uint32_t>( byte_from_nibbles( rng ) );
		if( cp <= 0x7FU ) {
			*it++ = static_cast<char>( cp );
			return it;
		}
		if( 0xD800U <= cp and cp <= 0xDBFFU ) {
			cp = ( cp - 0xD800U ) * 0x400U;
			rng.remove_prefix( );
			daw_json_assert_weak( rng.front( "uU" ),
			                      "Expected rng to start with a \\u" );
			rng.remove_prefix( );
			auto trailing = static_cast<std::uint32_t>( byte_from_nibbles( rng ) )
			                << 8U;
			trailing |= static_cast<std::uint32_t>( byte_from_nibbles( rng ) );
			trailing = static_cast<std::uint32_t>( trailing - 0xDC00U );
			cp += trailing;
			cp += 0x10000;
		}
		// UTF32-> UTF8
		if( cp >= 0x10000U ) {
			// 4 bytes
			char const enc3 =
			  static_cast<char>( ( cp & 0b0011'1111U ) | 0b1000'0000U );
			char const enc2 =
			  static_cast<char>( ( ( cp >> 6U ) & 0b0011'1111U ) | 0b1000'0000U );
			char const enc1 =
			  static_cast<char>( ( ( cp >> 12U ) & 0b0011'1111U ) | 0b1000'0000U );
			char const enc0 = static_cast<char>( ( cp >> 18U ) | 0b1111'0000U );
			*it++ = enc0;
			*it++ = enc1;
			*it++ = enc2;
			*it++ = enc3;
			return it;
		}
		if( cp >= 0x800U ) {
			// 3 bytes
			char const enc2 =
			  static_cast<char>( ( cp & 0b0011'1111U ) | 0b1000'0000U );
			char const enc1 =
			  static_cast<char>( ( ( cp >> 6U ) & 0b0011'1111U ) | 0b1000'0000U );
			char const enc0 = static_cast<char>( ( cp >> 12U ) | 0b1110'0000U );
			*it++ = enc0;
			*it++ = enc1;
			*it++ = enc2;
			return it;
		}
		// cp >= 0x80U
		// 2 bytes
		char const enc1 = static_cast<char>( ( cp & 0b0011'1111U ) | 0b1000'0000U );
		char const enc0 = static_cast<char>( ( cp >> 6U ) | 0b1100'0000U );
		*it++ = enc0;
		*it++ = enc1;
		return it;
	}

	template<typename Range, typename Appender>
	static constexpr void decode_utf16( Range &rng, Appender &app ) {
		daw_json_assert_weak( rng.front( "uU" ), "Expected rng to start with a u" );
		rng.remove_prefix( );
		std::uint32_t cp = static_cast<std::uint32_t>( byte_from_nibbles( rng ) )
		                   << 8U;
		cp |= static_cast<std::uint32_t>( byte_from_nibbles( rng ) );
		if( cp <= 0x7FU ) {
			app( static_cast<char>( cp ) );
			return;
		}
		if( 0xD800U <= cp and cp <= 0xDBFFU ) {
			cp = ( cp - 0xD800U ) * 0x400U;
			rng.remove_prefix( );
			daw_json_assert_weak( rng.front( "uU" ),
			                      "Expected rng to start with a \\u" );
			rng.remove_prefix( );
			auto trailing = static_cast<std::uint32_t>( byte_from_nibbles( rng ) )
			                << 8U;
			trailing |= static_cast<std::uint32_t>( byte_from_nibbles( rng ) );
			trailing = static_cast<std::uint32_t>( trailing - 0xDC00U );
			cp += trailing;
			cp += 0x10000;
		}
		// UTF32-> UTF8
		if( cp >= 0x10000U ) {
			// 4 bytes
			char const enc3 =
			  static_cast<char>( ( cp & 0b0011'1111U ) | 0b1000'0000U );
			char const enc2 =
			  static_cast<char>( ( ( cp >> 6U ) & 0b0011'1111U ) | 0b1000'0000U );
			char const enc1 =
			  static_cast<char>( ( ( cp >> 12U ) & 0b0011'1111U ) | 0b1000'0000U );
			char const enc0 = static_cast<char>( ( cp >> 18U ) | 0b1111'0000U );
			app( enc0 );
			app( enc1 );
			app( enc2 );
			app( enc3 );
			return;
		}
		if( cp >= 0x800U ) {
			// 3 bytes
			char const enc2 =
			  static_cast<char>( ( cp & 0b0011'1111U ) | 0b1000'0000U );
			char const enc1 =
			  static_cast<char>( ( ( cp >> 6U ) & 0b0011'1111U ) | 0b1000'0000U );
			char const enc0 = static_cast<char>( ( cp >> 12U ) | 0b1110'0000U );
			app( enc0 );
			app( enc1 );
			app( enc2 );
			return;
		}
		// cp >= 0x80U
		// 2 bytes
		char const enc1 = static_cast<char>( ( cp & 0b0011'1111U ) | 0b1000'0000U );
		char const enc0 = static_cast<char>( ( cp >> 6U ) | 0b1100'0000U );
		app( enc0 );
		app( enc1 );
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

		bool const has_quote = rng.front( '"' );
		if( has_quote ) {
			rng.remove_prefix( );
		}
		while( rng.front( ) != '"' ) {
			while( rng.front( ) != '"' and rng.front( ) != '\\' ) {
				daw_json_assert_weak( not rng.empty( ), "Unexpected end of data" );
				*it++ = rng.front( );
				rng.remove_prefix( );
			}
			if( rng.front( ) == '\\' ) {
				daw_json_assert_weak( rng.front( ) >= 0x20, "Invalid codepoint" );
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
				case 'U': // Sometimes people put crap
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
						  static_cast<unsigned>( rng.front( ) ) >= 0x20U and
						    static_cast<unsigned>( rng.front( ) ) <= 0x7FU,
						  "string support limited to 0x20 < chr <= 0x7F when "
						  "DisallowHighEightBit is true" );
					}
					*it++ = rng.front( );
					rng.remove_prefix( );
				}
			} else {
				daw_json_assert_weak( not has_quote or rng.front( '"' ),
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
	}
} // namespace daw::json::json_details
