// The MIT License (MIT)
//
// Copyright (c) 2019 Darrell Wright
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files( the "Software" ), to
// deal in the Software without restriction, including without limitation the
// rights to use, copy, modify, merge, publish, distribute, sublicense, and / or
// sell copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#pragma once

#include <cstddef>
#include <cstdint>

#include "daw_iterator_range.h"
#include "daw_json_assert.h"
#include "daw_json_link_impl.h"
#include "daw_json_parse_array_iterator.h"
#include "daw_json_parse_common.h"
#include "daw_json_parse_name.h"
#include "daw_json_parse_real.h"
#include "daw_json_parse_signed_int.h"
#include "daw_json_parse_string_quote.h"
#include "daw_json_parse_unsigned_int.h"
#include "daw_json_parse_value_fwd.h"

namespace daw::json::impl {
	template<typename JsonMember, typename First, typename Last,
	         bool TrustedInput>
	constexpr void skip_quote_when_literal_as_string(
	  IteratorRange<First, Last, TrustedInput> &rng ) {
		if constexpr( JsonMember::literal_as_string == LiteralAsStringOpt::Never ) {
			return;
			// Temporary fix as right now we are only sometimes having strings to skip
	/*	else if constexpr( JsonMember::literal_as_string ==
		                     LiteralAsStringOpt::Always ) {
			rng.remove_prefix( );
			return;
	*/	} else {
		if( rng.front( ) == '"' ) {
			rng.remove_prefix( );
		}
		return;
	}
	}

	template<typename JsonMember, typename First, typename Last,
	         bool TrustedInput>
	[[nodiscard]] static constexpr json_result<JsonMember>
	parse_value( ParseTag<JsonParseTypes::Real>,
	             IteratorRange<First, Last, TrustedInput> &rng ) {
		using constructor_t = typename JsonMember::constructor_t;
		using element_t = typename JsonMember::parse_to_t;

		skip_quote_when_literal_as_string<JsonMember>( rng );
		daw_json_assert_untrusted(
		  rng.is_real_number_part( ),
		  "Expected number to start with on of \"0123456789eE+-\"" );

		auto result = constructor_t{}( parse_real<element_t>( rng ) );
		skip_quote_when_literal_as_string<JsonMember>( rng );
		daw_json_assert_untrusted(
		  rng.at_end_of_item( ),
		  "Expected whitespace or one of \",}]\" at end of number" );
		return result;
	}

	template<typename JsonMember, typename First, typename Last,
	         bool TrustedInput>
	[[nodiscard]] static constexpr json_result<JsonMember>
	parse_value( ParseTag<JsonParseTypes::Signed>,
	             IteratorRange<First, Last, TrustedInput> &rng ) {
		using constructor_t = typename JsonMember::constructor_t;
		using element_t = typename JsonMember::parse_to_t;

		skip_quote_when_literal_as_string<JsonMember>( rng );
		daw_json_assert_untrusted(
		  rng.is_real_number_part( ),
		  "Expected number to start with on of \"0123456789eE+-\"" );

		auto result = constructor_t{}(
		  parse_integer<element_t, JsonMember::range_check>( rng ) );
		skip_quote_when_literal_as_string<JsonMember>( rng );
		daw_json_assert_untrusted(
		  rng.at_end_of_item( ),
		  "Expected whitespace or one of \",}]\" at end of number" );
		return result;
	}

	template<typename JsonMember, typename First, typename Last,
	         bool TrustedInput>
	[[nodiscard]] static constexpr json_result<JsonMember>
	parse_value( ParseTag<JsonParseTypes::Unsigned>,
	             IteratorRange<First, Last, TrustedInput> &rng ) {
		using constructor_t = typename JsonMember::constructor_t;
		using element_t = typename JsonMember::parse_to_t;

		skip_quote_when_literal_as_string<JsonMember>( rng );
		daw_json_assert_untrusted(
		  rng.is_real_number_part( ),
		  "Expected number to start with on of \"0123456789eE+-\"" );
		auto result = constructor_t{}(
		  parse_unsigned_integer<element_t, JsonMember::range_check>( rng ) );
		skip_quote_when_literal_as_string<JsonMember>( rng );
		daw_json_assert_untrusted(
		  rng.at_end_of_item( ),
		  "Expected whitespace or one of \",}]\" at end of number" );
		return result;
	}

	template<typename JsonMember, typename First, typename Last,
	         bool TrustedInput>
	[[nodiscard]] static constexpr json_result<JsonMember>
	parse_value( ParseTag<JsonParseTypes::Null>,
	             IteratorRange<First, Last, TrustedInput> &rng ) {

		using constructor_t = typename JsonMember::constructor_t;
		using element_t = typename JsonMember::base;
		if( rng.empty( ) or rng.is_null( ) ) {
			return constructor_t{}( );
		}
		constexpr size_t null_size = 4;
		if( ( rng.size( ) >= null_size ) and rng.front( ) == 'n' ) {
			daw_json_assert_untrusted( rng == "null", "Expected literal null" );
			rng.remove_prefix( null_size );
			rng.trim_left( );
			return constructor_t{}( );
		}
		return parse_value<element_t>( ParseTag<element_t::expected_type>{}, rng );
	}

	template<typename JsonMember, typename First, typename Last,
	         bool TrustedInput>
	[[nodiscard]] static constexpr json_result<JsonMember>
	parse_value( ParseTag<JsonParseTypes::Bool>,
	             IteratorRange<First, Last, TrustedInput> &rng ) {
		daw_json_assert_untrusted( rng.size( ) >= 4,
		                           "Range to small to be a bool" );

		using constructor_t = typename JsonMember::constructor_t;

		skip_quote_when_literal_as_string<JsonMember>( rng );
		bool result = false;
		if( rng.front( ) == 't' and rng.size( ) >= 4 ) {
			daw_json_assert_untrusted( rng == "true", "Expected a literal true" );
			rng.remove_prefix( 4 );
			result = true;
		} else {
			daw_json_assert_untrusted( rng == "false", "Expected a literal false" );
			rng.remove_prefix( 5 );
		}
		skip_quote_when_literal_as_string<JsonMember>( rng );
		return constructor_t{}( result );
	}

	template<typename JsonMember, typename First, typename Last,
	         bool TrustedInput>
	[[nodiscard]] static constexpr json_result<JsonMember>
	parse_value( ParseTag<JsonParseTypes::String>,
	             IteratorRange<First, Last, TrustedInput> &rng ) {

		auto str = skip_string( rng );
		using constructor_t = typename JsonMember::constructor_t;
		if constexpr( JsonMember::empty_is_null == JsonNullable::Nullable ) {
			if( str.empty( ) ) {
				return constructor_t{}( );
			}
		}
		return constructor_t{}( str.begin( ), str.size( ) );
	}

	template<bool TrustedInput>
	constexpr unsigned to_nibble( unsigned chr ) {
		if( auto tmp = chr - static_cast<unsigned>( '0' ); tmp < 10U ) {
			return tmp;
		}
		if( auto tmp = chr - static_cast<unsigned>( 'a' ); tmp < 6U ) {
			return tmp + 10U;
		}
		auto tmp = chr - static_cast<unsigned>( 'A' );
		daw_json_assert_untrusted( tmp < 6U, "Expected nibble" );
		return tmp + 10U;
	}

	template<typename First, typename Last, bool TrustedInput>
	constexpr uint16_t
	byte_from_nibbles( IteratorRange<First, Last, TrustedInput> &rng ) noexcept {
		auto n0 = to_nibble<TrustedInput>( static_cast<unsigned>( rng.front( ) ) );
		rng.remove_prefix( );
		auto n1 = to_nibble<TrustedInput>( static_cast<unsigned>( rng.front( ) ) );
		rng.remove_prefix( );
		return static_cast<uint16_t>( ( n0 << 4U ) | n1 );
	}

	template<typename First, typename Last, bool TrustedInput, typename Appender>
	constexpr void decode_utf16( IteratorRange<First, Last, TrustedInput> &rng,
	                             Appender &app ) {
		daw_json_assert_untrusted( rng.front( ) == 'u' or rng.front( ) == 'U',
		                           "Expected rng to start with a u" );
		rng.remove_prefix( );
		uint32_t cp = static_cast<uint32_t>( byte_from_nibbles( rng ) ) << 8U;
		cp |= static_cast<uint32_t>( byte_from_nibbles( rng ) );
		if( cp <= 0x7FU ) {
			app( static_cast<char>( cp ) );
			return;
		}
		if( cp >= 0xD800U and cp <= 0xDBFFU ) {
			cp = ( cp - 0xD800U ) * 0x400U;
			rng.remove_prefix( );
			daw_json_assert_untrusted( rng.front( ) == 'u' or rng.front( ) == 'U',
			                           "Expected rng to start with a \\u" );
			rng.remove_prefix( );
			auto trailing = static_cast<uint32_t>( byte_from_nibbles( rng ) ) << 8U;
			trailing |= static_cast<uint32_t>( byte_from_nibbles( rng ) );
			trailing = static_cast<uint32_t>( trailing - 0xDC00U );
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

	template<typename JsonMember, typename First, typename Last,
	         bool TrustedInput>
	[[nodiscard]] static constexpr json_result<JsonMember>
	parse_value( ParseTag<JsonParseTypes::StringEscaped>,
	             IteratorRange<First, Last, TrustedInput> &rng ) {

		// TODO: make escape aware skip_string
		using constructor_t = typename JsonMember::constructor_t;
		using appender_t = typename JsonMember::appender_t;
		constexpr bool disallow_high8 = JsonMember::disallow_high_eight_bit;

		auto result = constructor_t{}( );
		auto app = appender_t{result};
		if( rng.front( '"' ) ) {
			rng.remove_prefix( );
		}
		while( rng.front( ) != '"' ) {
			while( rng.front( ) != '"' and rng.front( ) != '\\' ) {
				app( rng.front( ) );
				rng.remove_prefix( );
			}
			if( rng.front( ) == '\\' ) {
				rng.remove_prefix( );
				switch( rng.front( ) ) {
				case 'b':
					app( '\b' );
					rng.remove_prefix( );
					break;
				case 'f':
					app( '\f' );
					rng.remove_prefix( );
					break;
				case 'n':
					app( '\n' );
					rng.remove_prefix( );
					break;
				case 'r':
					app( '\r' );
					rng.remove_prefix( );
					break;
				case 't':
					app( '\t' );
					rng.remove_prefix( );
					break;
				case 'U': // Sometimes people put crap
				case 'u':
					decode_utf16( rng, app );
					break;
				case '\\':
				case '/':
				case '"':
					app( rng.front( ) );
					rng.remove_prefix( );
					break;
				default:
					if constexpr( disallow_high8 ) {
						daw_json_assert(
						  static_cast<unsigned>( rng.front( ) ) >= 0x20U and
						    static_cast<unsigned>( rng.front( ) ) <= 0x7FU,
						  "string support limited to 0x20 < chr <= 0x7F when "
						  "DisallowHighEightBit is true" );
					}
					app( rng.front( ) );
					rng.remove_prefix( );
				}
			} else {
				daw_json_assert_untrusted( rng.front( ) == '"',
				                           "Unexpected end of string" );
			}
		}
		daw_json_assert_untrusted( rng.front( ) == '"', "Unexpected state, no \"" );
		rng.remove_prefix( );
		return result;
	}

	template<typename JsonMember, typename First, typename Last,
	         bool TrustedInput>
	[[nodiscard]] static constexpr json_result<JsonMember>
	parse_value( ParseTag<JsonParseTypes::Date>,
	             IteratorRange<First, Last, TrustedInput> &rng ) {

		auto str = skip_string( rng );
		using constructor_t = typename JsonMember::constructor_t;
		return constructor_t{}( str.begin( ), str.size( ) );
	}

	template<typename JsonMember, typename First, typename Last,
	         bool TrustedInput>
	[[nodiscard]] static constexpr json_result<JsonMember>
	parse_value( ParseTag<JsonParseTypes::Custom>,
	             IteratorRange<First, Last, TrustedInput> &rng ) {

		daw_json_assert_untrusted(
		  rng.front( '"' ), "Custom types requite a string at the beginning" );
		rng.remove_prefix( );
		auto str = skip_string_nq( rng );
		// TODO make custom require a ptr/sz pair
		using constructor_t = typename JsonMember::from_converter_t;
		return constructor_t{}( std::string_view( str.begin( ), str.size( ) ) );
	}

	template<typename JsonMember, typename First, typename Last,
	         bool TrustedInput>
	[[nodiscard]] static constexpr json_result<JsonMember>
	parse_value( ParseTag<JsonParseTypes::Class>,
	             IteratorRange<First, Last, TrustedInput> &rng ) {

		using element_t = typename JsonMember::parse_to_t;
		return from_json<element_t>( rng );
	}
	/**
	 * Parse a key_value pair encoded as a json object where the keys are the
	 * member names
	 * @tparam JsonMember json_key_value type
	 * @tparam First Type of Iterator for beginning of stream range
	 * @tparam Last Type of Iterator for end of stream range
	 * @tparam TrustedInput Are we parsing a trusted stream
	 * @param rng Range of input to parse
	 * @return Constructed key_value container
	 */
	template<typename JsonMember, typename First, typename Last,
	         bool TrustedInput>
	[[nodiscard]] static constexpr json_result<JsonMember>
	parse_value( ParseTag<JsonParseTypes::KeyValue>,
	             IteratorRange<First, Last, TrustedInput> &rng ) {

		static_assert( JsonMember::expected_type == JsonParseTypes::KeyValue,
		               "Expected a json_key_value" );
		daw_json_assert_untrusted(
		  rng.front( '{' ),
		  "Expected keyvalue type to be of class type and beging with '{'" );

		rng.remove_prefix( );
		// We are inside a KV map, we can expected a quoted name next
		rng.move_to_next_of( '"' );

		auto array_container = typename JsonMember::constructor_t{}( );
		auto container_appender =
		  typename JsonMember::appender_t( array_container );

		using key_t = typename JsonMember::json_key_t;
		using value_t = typename JsonMember::json_element_t;
		while( rng.front( ) != '}' ) {
			auto key = parse_value<key_t>( ParseTag<key_t::expected_type>{}, rng );
			name::name_parser::trim_end_of_name( rng );
			container_appender(
			  std::move( key ),
			  parse_value<value_t>( ParseTag<value_t::expected_type>{}, rng ) );

			rng.clean_tail( );
			daw_json_assert_untrusted( rng.has_more( ), "Unexpected end of data" );
		}
		daw_json_assert_untrusted( rng.front( '}' ),
		                           "Expected keyvalue type to end with a '}'" );
		rng.remove_prefix( );
		rng.trim_left( );
		return array_container;
	}

	/**
	 * Parse a key_value pair encoded as a json object where the keys are the
	 * member names
	 * @tparam JsonMember json_key_value type
	 * @tparam First Type of Iterator for beginning of stream range
	 * @tparam Last Type of Iterator for end of stream range
	 * @tparam TrustedInput Are we parsing a trusted stream
	 * @param rng Range of input to parse
	 * @return Constructed key_value container
	 */
	template<typename JsonMember, typename First, typename Last,
	         bool TrustedInput>
	[[nodiscard]] static constexpr json_result<JsonMember>
	parse_value( ParseTag<JsonParseTypes::KeyValueArray>,
	             IteratorRange<First, Last, TrustedInput> &rng ) {

		static_assert( JsonMember::expected_type == JsonParseTypes::KeyValueArray,
		               "Expected a json_key_value" );
		daw_json_assert_untrusted(
		  rng.front( '[' ),
		  "Expected keyvalue type to be of class type and beging with '{'" );

		rng.remove_prefix( );

		auto array_container = typename JsonMember::constructor_t{}( );
		auto container_appender =
		  typename JsonMember::appender_t( array_container );

		using key_t = typename JsonMember::json_key_t;
		using value_t = typename JsonMember::json_value_t;
		while( rng.front( ) != ']' ) {
			// We are in an object find {
			rng.move_to_next_of( '{' );
			rng.remove_prefix( );
			rng.move_to_next_of( "\"}" );
			daw_json_assert( rng.front( ) == '"', "Expected name of key member" );
			rng.remove_prefix( );
			auto const key_name = daw::json::impl::name::name_parser::parse_nq( rng );
			(void)key_name;
			daw_json_assert( key_t::name == key_name,
			                 "Expected value name to match" );
			auto key = parse_value<key_t>( ParseTag<key_t::expected_type>{}, rng );
			rng.move_to_next_of( '"' ); // Next
			daw_json_assert( rng.front( ) == '"', "Expected name of value member" );
			rng.remove_prefix( );
			auto const value_name =
			  daw::json::impl::name::name_parser::parse_nq( rng );
			(void)value_name;
			daw_json_assert( value_t::name == value_name,
			                 "Expected value name to match" );
			container_appender(
			  std::move( key ),
			  parse_value<value_t>( ParseTag<value_t::expected_type>{}, rng ) );

			rng.move_to_next_of( '}' );
			rng.remove_prefix( );
			rng.trim_left( );
			daw_json_assert_untrusted( rng.has_more( ), "Unexpected end of data" );
			// End of object }
		}
		daw_json_assert_untrusted( rng.front( ']' ),
		                           "Expected keyvalue type to end with a '}'" );
		rng.remove_prefix( );
		rng.trim_left( );
		return array_container;
	}

	template<typename JsonMember, typename First, typename Last,
	         bool TrustedInput>
	[[nodiscard]] static constexpr json_result<JsonMember>
	parse_value( ParseTag<JsonParseTypes::Array>,
	             IteratorRange<First, Last, TrustedInput> &rng ) {

		using element_t = typename JsonMember::json_element_t;
		daw_json_assert_untrusted( rng.front( '[' ),
		                           "Expected array to start with a '['" );

		rng.remove_prefix( );
		rng.trim_left_no_check( );
		using container_t = typename JsonMember::parse_to_t;
		if constexpr( std::is_same_v<container_t, basic_appender<container_t>> and
		              std::is_same_v<typename JsonMember::constructor_t,
		                             daw::construct_a_t<container_t>> and
		              is_range_constructable_v<container_t> ) {
			// We are using the default constructor and appender.  This should allow
			// for
			using iterator_t =
			  json_parse_value_array_iterator<JsonMember, First, Last, TrustedInput>;
			return container_t( iterator_t( rng ), iterator_t( ) );
		} else {
			auto array_container = typename JsonMember::constructor_t{}( );
			auto container_appender =
			  typename JsonMember::appender_t( array_container );

			while( rng.front( ) != ']' ) {
				container_appender(
				  parse_value<element_t>( ParseTag<element_t::expected_type>{}, rng ) );
				rng.clean_tail( );
				daw_json_assert_untrusted( rng.has_more( ), "Unexpected end of data" );
			}
			rng.remove_prefix( );
			rng.trim_left( );
			return array_container;
		}
	}
} // namespace daw::json::impl
