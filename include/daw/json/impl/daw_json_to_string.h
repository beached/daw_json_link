// The MIT License (MIT)
//
// Copyright (c) 2019-2020 Darrell Wright
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

#include "daw_json_parse_common.h"

#include <daw/daw_traits.h>
#include <daw/iso8601/daw_date_formatting.h>
#include <utf8/unchecked.h>

#include <optional>
#include <sstream>
#include <string>
#include <tuple>
#include <type_traits>
#include <variant>

namespace daw::json::impl::to_strings {

	namespace {
		using std::to_string;
		namespace to_string_test {
			template<typename T>
			[[maybe_unused]] auto to_string_test( T &&v )
			  -> decltype( to_string( std::forward<T>( v ) ) );

			template<typename T>
			using to_string_result = decltype( to_string_test( std::declval<T>( ) ) );
		} // namespace to_string_test

		template<typename T>
		using has_to_string = daw::is_detected<to_string_test::to_string_result, T>;

		template<typename T>
		inline constexpr bool has_to_string_v = has_to_string<T>::value;

		template<typename T>
		[[nodiscard, maybe_unused]] auto to_string( std::optional<T> const &v )
		  -> decltype( to_string( *v ) ) {
			if( not v ) {
				return {"null"};
			}
			return to_string( *v );
		}
	} // namespace
} // namespace daw::json::impl::to_strings

namespace daw::json {
	template<typename T>
	struct custom_to_converter_t {
		template<typename U, std::enable_if_t<impl::to_strings::has_to_string_v<U>,
		                                      std::nullptr_t> = nullptr>
		[[nodiscard]] constexpr decltype( auto ) operator( )( U &&value ) const {
			using std::to_string;
			return to_string( std::forward<U>( value ) );
		}

		template<typename U,
		         std::enable_if_t<not impl::to_strings::has_to_string_v<U>,
		                          std::nullptr_t> = nullptr>
		[[nodiscard]] std::string operator( )( U &&value ) const {
			std::stringstream ss;
			ss << std::forward<U>( value );
			return ss.str( );
		}
	};
} // namespace daw::json

namespace daw::json::impl {
	namespace {
		template<typename JsonMember, typename OutputIterator, typename parse_to_t>
		[[nodiscard]] constexpr OutputIterator
		to_string( ParseTag<JsonParseTypes::Null>, OutputIterator it,
		           parse_to_t const &container );

		template<typename JsonMember, typename OutputIterator, typename parse_to_t>
		[[nodiscard]] constexpr OutputIterator
		to_string( ParseTag<JsonParseTypes::Array>, OutputIterator it,
		           parse_to_t const &value );

		template<typename JsonMember, typename OutputIterator, typename parse_to_t>
		[[nodiscard]] constexpr OutputIterator
		to_string( ParseTag<JsonParseTypes::KeyValueArray>, OutputIterator it,
		           parse_to_t const &value );

		template<typename JsonMember, typename OutputIterator, typename parse_to_t>
		[[nodiscard]] constexpr OutputIterator
		to_string( ParseTag<JsonParseTypes::KeyValue>, OutputIterator it,
		           parse_to_t const &value );

		template<typename JsonMember, typename OutputIterator, typename parse_to_t>
		[[nodiscard]] constexpr OutputIterator
		to_string( ParseTag<JsonParseTypes::Custom>, OutputIterator it,
		           parse_to_t const &value );

		template<typename JsonMember, typename OutputIterator, typename parse_to_t>
		[[nodiscard]] constexpr OutputIterator
		to_string( ParseTag<JsonParseTypes::Variant>, OutputIterator it,
		           parse_to_t const &value );

		template<typename JsonMember, typename OutputIterator, typename parse_to_t>
		[[nodiscard]] constexpr OutputIterator
		to_string( ParseTag<JsonParseTypes::Class>, OutputIterator it,
		           parse_to_t const &value );

		template<typename JsonMember, typename OutputIterator, typename parse_to_t>
		[[nodiscard]] constexpr OutputIterator
		to_string( ParseTag<JsonParseTypes::Real>, OutputIterator it,
		           parse_to_t const &value );

		template<typename JsonMember, typename OutputIterator, typename parse_to_t>
		[[nodiscard]] constexpr OutputIterator
		to_string( ParseTag<JsonParseTypes::Signed>, OutputIterator it,
		           parse_to_t const &value );

		template<typename JsonMember, typename OutputIterator, typename parse_to_t>
		[[nodiscard]] constexpr OutputIterator
		to_string( ParseTag<JsonParseTypes::Unsigned>, OutputIterator it,
		           parse_to_t const &value );

		template<typename JsonMember, typename OutputIterator, typename parse_to_t>
		[[nodiscard]] constexpr OutputIterator
		to_string( ParseTag<JsonParseTypes::StringRaw>, OutputIterator it,
		           parse_to_t const &value );

		template<typename JsonMember, typename OutputIterator, typename parse_to_t>
		[[nodiscard]] constexpr OutputIterator
		to_string( ParseTag<JsonParseTypes::StringEscaped>, OutputIterator it,
		           parse_to_t const &value );

		template<typename JsonMember, typename OutputIterator, typename parse_to_t>
		[[nodiscard]] constexpr OutputIterator
		to_string( ParseTag<JsonParseTypes::Date>, OutputIterator it,
		           parse_to_t const &value );

		template<typename JsonMember, typename OutputIterator, typename parse_to_t>
		[[nodiscard]] constexpr OutputIterator
		to_string( ParseTag<JsonParseTypes::Bool>, OutputIterator it,
		           parse_to_t const &value );

		//************************************************
		template<typename Char>
		constexpr char to_nibble_char( Char c ) noexcept {
			daw_json_assert( c < 16, "Unexpected hex nibble" );
			if( c < 10 ) {
				return static_cast<char>( c + '0' );
			} else {
				return static_cast<char>( ( c - 10U ) + 'A' );
			}
		}

		template<typename OutputIterator>
		constexpr OutputIterator output_hex( uint16_t c, OutputIterator it ) {
			char const n0 = to_nibble_char( ( c >> 12U ) & 0xFU );
			char const n1 = to_nibble_char( ( c >> 8U ) & 0xFU );
			char const n2 = to_nibble_char( ( c >> 4U ) & 0xFU );
			char const n3 = to_nibble_char( c & 0xFU );

			*it++ = '\\';
			*it++ = 'u';
			*it++ = n0;
			*it++ = n1;
			*it++ = n2;
			*it++ = n3;
			return it;
		}

		template<typename OutputIterator>
		constexpr void utf32_to_utf8( uint32_t cp, OutputIterator &it ) {
			if( cp <= 0x7FU ) {
				*it++ = cp;
				return;
			}
			if( cp <= 0x7FFU ) {
				*it++ = ( cp >> 6U ) | 0b11000000;
				*it++ = ( cp & 0b00111111 ) | 0b10000000;
				return;
			}
			if( cp <= 0xFFFFU ) {
				*it++ = ( cp >> 12U ) | 0b11100000;
				*it++ = ( ( cp >> 6U ) & 0b00111111 ) | 0b10000000;
				*it++ = ( cp & 0b00111111 ) | 0b10000000;
				return;
			}
			if( cp <= 0x10FFFF ) {
				*it++ = ( cp >> 18 ) | 0b11110000;
				*it++ = ( ( cp >> 12 ) & 0b00111111 ) | 0b10000000;
				*it++ = ( ( cp >> 6 ) & 0b00111111 ) | 0b10000000;
				*it++ = ( cp & 0b00111111 ) | 0b10000000;
				return;
			}
			daw_json_error( "Invalid code point" );
		}

		template<bool do_escape = false,
		         EightBitModes EightBitMode = EightBitModes::AllowFull,
		         typename Container, typename OutputIterator,
		         daw::enable_when_t<daw::traits::is_container_like_v<
		           daw::remove_cvref_t<Container>>> = nullptr>
		[[nodiscard]] constexpr OutputIterator
		copy_to_iterator( Container const &container, OutputIterator it ) {
			if constexpr( do_escape ) {
				using iter = daw::remove_cvref_t<decltype( std::begin( container ) )>;
				using it_t = utf8::unchecked::iterator<iter>;
				auto first = it_t( std::begin( container ) );
				auto const last = it_t( std::end( container ) );
				while( first != last ) {
					auto const cp = *first++;
					switch( cp ) {
					case '"':
						*it++ = '\\';
						*it++ = '"';
						break;
					case '\\':
						*it++ = '\\';
						*it++ = '\\';
						break;
					case '/':
						*it++ = '\\';
						*it++ = '/';
						break;
					case '\b':
						*it++ = '\\';
						*it++ = 'b';
						break;
					case '\f':
						*it++ = '\\';
						*it++ = 'f';
						break;
					case '\n':
						*it++ = '\\';
						*it++ = 'n';
						break;
					case '\r':
						*it++ = '\\';
						*it++ = 'r';
						break;
					case '\t':
						*it++ = '\\';
						*it++ = 't';
						break;
					default:
						if constexpr( EightBitMode == EightBitModes::DisallowHigh ) {
							if( cp < 0x20U ) {
								it = output_hex( static_cast<uint16_t>( cp ), it );
								break;
							}
							if( cp >= 0x7FU and cp <= 0xFFFFU ) {
								it = output_hex( static_cast<uint16_t>( cp ), it );
								break;
							}
							if( cp > 0xFFFFU ) {
								it = output_hex(
								  static_cast<uint16_t>( 0xD7C0U + ( cp >> 10U ) ), it );
								it = output_hex(
								  static_cast<uint16_t>( 0xDC00U + ( cp & 0x3FFU ) ), it );
								break;
							}
						}
						utf32_to_utf8( cp, it );
						break;
					}
				}
			} else {
				for( auto c : container ) {
					if constexpr( EightBitMode == EightBitModes::DisallowHigh ) {
						daw_json_assert(
						  static_cast<unsigned>( c ) >= 0x20U and
						    static_cast<unsigned>( c ) <= 0x7FU,
						  "string support limited to 0x20 < chr <= 0x7F when "
						  "DisallowHighEightBit is true" );
					}
					*it++ = c;
				}
			}
			return it;
		}

		template<bool do_escape = false,
		         EightBitModes EightBitMode = EightBitModes::AllowFull,
		         typename OutputIterator>
		[[nodiscard]] constexpr OutputIterator
		copy_to_iterator( char const *ptr, OutputIterator it ) {
			if( ptr == nullptr ) {
				return it;
			}
			if constexpr( do_escape ) {

				auto chr_it = utf8::unchecked::iterator<char const *>( ptr );
				while( *chr_it.base( ) != '\0' ) {
					auto const cp = *chr_it++;
					switch( cp ) {
					case '"':
						*it++ = '\\';
						*it++ = '"';
						break;
					case '\\':
						*it++ = '\\';
						*it++ = '\\';
						break;
					case '/':
						*it++ = '\\';
						*it++ = '/';
						break;
					case '\b':
						*it++ = '\\';
						*it++ = 'b';
						break;
					case '\f':
						*it++ = '\\';
						*it++ = 'f';
						break;
					case '\n':
						*it++ = '\\';
						*it++ = 'n';
						break;
					case '\r':
						*it++ = '\\';
						*it++ = 'r';
						break;
					case '\t':
						*it++ = '\\';
						*it++ = 't';
						break;
					default:
						if constexpr( EightBitMode == EightBitModes::DisallowHigh ) {
							if( cp < 0x20U ) {
								it = output_hex( static_cast<uint16_t>( cp ), it );
								break;
							}
							if( cp >= 0x7FU and cp <= 0xFFFFU ) {
								it = output_hex( static_cast<uint16_t>( cp ), it );
								break;
							}
							if( cp > 0xFFFFU ) {
								it = output_hex(
								  static_cast<uint16_t>( 0xD7C0U + ( cp >> 10U ) ), it );
								it = output_hex(
								  static_cast<uint16_t>( 0xDC00U + ( cp & 0x3FFU ) ), it );
								break;
							}
						}
						utf32_to_utf8( cp, it );
						break;
					}
				}
			} else {
				while( *ptr != '\0' ) {
					if constexpr( EightBitMode == EightBitModes::DisallowHigh ) {
						daw_json_assert(
						  static_cast<unsigned>( *ptr ) >= 0x20U and
						    static_cast<unsigned>( *ptr ) <= 0x7FU,
						  "string support limited to 0x20 < chr <= 0x7F when "
						  "DisallowHighEightBit is true" );
					}
					*it++ = *ptr++;
				}
			}
			return it;
		}

		template<typename JsonMember, typename OutputIterator, typename parse_to_t>
		[[nodiscard]] constexpr OutputIterator
		to_string( ParseTag<JsonParseTypes::Bool>, OutputIterator it,
		           parse_to_t const &value ) {

			/*static_assert(
			  std::is_convertible_v<parse_to_t, typename JsonMember::parse_to_t>,
			  "value must be convertible to specified type in class contract" );
			*/
			if( value ) {
				return copy_to_iterator( "true", it );
			}
			return copy_to_iterator( "false", it );
		}

		template<size_t idx, typename JsonMembers, typename OutputIterator,
		         typename parse_to_t>
		constexpr void to_variant_string( OutputIterator &it,
		                                  parse_to_t const &value ) {
			if constexpr( idx < std::variant_size_v<parse_to_t> ) {
				if( value.index( ) != idx ) {
					to_variant_string<idx + 1, JsonMembers>( it, value );
					return;
				}
				using element_t = typename JsonMembers::json_elements;
				using JsonMember =
				  typename std::tuple_element<idx,
				                              typename element_t::element_map_t>::type;
				it = to_string<JsonMember>( ParseTag<JsonMember::base_expected_type>{},
				                            it, std::get<idx>( value ) );
			}
		}

		template<typename JsonMember, typename OutputIterator, typename parse_to_t>
		[[nodiscard]] constexpr OutputIterator
		to_string( ParseTag<JsonParseTypes::Variant>, OutputIterator it,
		           parse_to_t const &value ) {

			to_variant_string<0, JsonMember>( it, value );
			return it;
		}

		template<typename JsonMember, typename OutputIterator, typename parse_to_t>
		[[nodiscard]] constexpr OutputIterator
		to_string( ParseTag<JsonParseTypes::VariantTagged>, OutputIterator it,
		           parse_to_t const &value ) {

			to_variant_string<0, JsonMember>( it, value );
			return it;
		}

		template<typename T>
		constexpr auto deref_detect( T &&value ) noexcept -> decltype( *value );

		constexpr void deref_detect( ... ) noexcept {}

		template<typename T>
		using deref_t =
		  daw::remove_cvref_t<decltype( deref_detect( std::declval<T>( ) ) )>;

		template<typename Optional>
		inline constexpr bool is_valid_optional_v =
		  daw::is_detected_v<deref_t, Optional>;

		template<typename JsonMember, typename OutputIterator, typename Optional>
		[[nodiscard]] constexpr OutputIterator
		to_string( ParseTag<JsonParseTypes::Null>, OutputIterator it,
		           Optional const &value ) {
			static_assert( is_valid_optional_v<Optional> );
			daw_json_assert( value, "Should Never get here without a value" );
			using tag_type = ParseTag<JsonMember::base_expected_type>;
			return to_string<JsonMember>( tag_type{}, it, *value );
		}

		template<typename JsonMember, typename OutputIterator, typename parse_to_t>
		[[nodiscard]] constexpr OutputIterator
		to_string( ParseTag<JsonParseTypes::Real>, OutputIterator it,
		           parse_to_t const &value ) {

			static_assert(
			  std::is_convertible_v<parse_to_t, typename JsonMember::parse_to_t>,
			  "value must be convertible to specified type in class contract" );

			using std::to_string;
			using to_strings::to_string;
			return copy_to_iterator( to_string( value ), it );
		}
		template<typename JsonMember, typename OutputIterator, typename parse_to_t>
		[[nodiscard]] constexpr OutputIterator
		to_string( ParseTag<JsonParseTypes::Signed>, OutputIterator it,
		           parse_to_t const &value ) {

			static_assert(
			  std::is_convertible_v<parse_to_t, typename JsonMember::parse_to_t>,
			  "value must be convertible to specified type in class contract" );

			using std::to_string;
			using to_strings::to_string;
			if constexpr( std::is_enum_v<parse_to_t> ) {
				return copy_to_iterator(
				  to_string( static_cast<std::underlying_type_t<parse_to_t>>( value ) ),
				  it );
			} else {
				return copy_to_iterator( to_string( value ), it );
			}
		}

		template<typename JsonMember, typename OutputIterator, typename parse_to_t>
		[[nodiscard]] constexpr OutputIterator
		to_string( ParseTag<JsonParseTypes::Unsigned>, OutputIterator it,
		           parse_to_t const &value ) {

			static_assert(
			  std::is_convertible_v<parse_to_t, typename JsonMember::parse_to_t>,
			  "value must be convertible to specified type in class contract" );

			using std::to_string;
			using to_strings::to_string;
			if constexpr( std::is_enum_v<parse_to_t> ) {
				return copy_to_iterator(
				  to_string( static_cast<std::underlying_type_t<parse_to_t>>( value ) ),
				  it );
			} else {
				return copy_to_iterator( to_string( value ), it );
			}
		}

		template<typename JsonMember, typename OutputIterator, typename parse_to_t>
		[[nodiscard]] constexpr OutputIterator
		to_string( ParseTag<JsonParseTypes::StringRaw>, OutputIterator it,
		           parse_to_t const &value ) {

			static_assert(
			  std::is_convertible_v<parse_to_t, typename JsonMember::parse_to_t>,
			  "value must be convertible to specified type in class contract" );

			constexpr EightBitModes eight_bit_mode = JsonMember::eight_bit_mode;
			*it++ = '"';
			it = copy_to_iterator<false, eight_bit_mode>( value, it );
			*it++ = '"';
			return it;
		}

		template<typename JsonMember, typename OutputIterator, typename parse_to_t>
		[[nodiscard]] constexpr OutputIterator
		to_string( ParseTag<JsonParseTypes::StringEscaped>, OutputIterator it,
		           parse_to_t const &value ) {

			static_assert(
			  std::is_convertible_v<parse_to_t, typename JsonMember::parse_to_t>,
			  "value must be convertible to specified type in class contract" );

			constexpr EightBitModes eight_bit_mode = JsonMember::eight_bit_mode;
			*it++ = '"';
			it = copy_to_iterator<true, eight_bit_mode>( value, it );
			*it++ = '"';
			return it;
		}

		template<typename T>
		[[nodiscard]] constexpr bool is_null( std::optional<T> const &v ) {
			return not static_cast<bool>( v );
		}

		template<typename T>
		[[nodiscard]] constexpr bool is_null( T const & ) {
			return false;
		}

		template<typename JsonMember, typename OutputIterator, typename parse_to_t>
		[[nodiscard]] constexpr OutputIterator
		to_string( ParseTag<JsonParseTypes::Date>, OutputIterator it,
		           parse_to_t const &value ) {

			static_assert(
			  std::is_convertible_v<parse_to_t, typename JsonMember::parse_to_t>,
			  "value must be convertible to specified type in class contract" );

			using daw::json::impl::is_null;
			if( is_null( value ) ) {
				it = copy_to_iterator( "null", it );
			} else {
				*it++ = '"';
				using namespace daw::date_formatting::formats;
				it = copy_to_iterator( daw::date_formatting::fmt_string(
				                         "{0}T{1}:{2}:{3}Z", value, YearMonthDay<>{},
				                         Hour<>{}, Minute<>{}, Second<>{} ),
				                       it );
				*it++ = '"';
			}
			return it;
		}

		template<typename JsonMember, typename OutputIterator, typename parse_to_t>
		[[nodiscard]] constexpr OutputIterator
		to_string( ParseTag<JsonParseTypes::Class>, OutputIterator it,
		           parse_to_t const &value ) {

			static_assert(
			  std::is_convertible_v<parse_to_t, typename JsonMember::parse_to_t>,
			  "value must be convertible to specified type in class contract" );

			return json_data_contract_trait_t<parse_to_t>::serialize(
			  it, daw::json::json_data_contract<parse_to_t>::to_json_data( value ),
			  value );
		}

		template<typename JsonMember, typename OutputIterator, typename parse_to_t>
		[[nodiscard]] constexpr OutputIterator
		to_string( ParseTag<JsonParseTypes::Custom>, OutputIterator it,
		           parse_to_t const &value ) {

			static_assert(
			  std::is_convertible_v<parse_to_t, typename JsonMember::parse_to_t>,
			  "value must be convertible to specified type in class contract" );

			if constexpr( JsonMember::custom_json_type == CustomJsonTypes::String ) {
				*it++ = '"';
				it = copy_to_iterator( typename JsonMember::to_converter_t{}( value ),
				                       it );
				*it++ = '"';
				return it;
			} else {
				return copy_to_iterator( typename JsonMember::to_converter_t{}( value ),
				                         it );
			}
		}

		template<typename JsonMember, typename OutputIterator, typename parse_to_t>
		[[nodiscard]] constexpr OutputIterator
		to_string( ParseTag<JsonParseTypes::Array>, OutputIterator it,
		           parse_to_t const &container ) {

			static_assert(
			  std::is_convertible_v<parse_to_t, typename JsonMember::parse_to_t>,
			  "value must be convertible to specified type in class contract" );

			*it++ = '[';
			if( not std::empty( container ) ) {
				auto count = std::size( container ) - 1U;
				for( auto const &v : container ) {
					it = to_string<typename JsonMember::json_element_t>(
					  ParseTag<JsonMember::json_element_t::expected_type>{}, it, v );
					if( count-- > 0 ) {
						*it++ = ',';
					}
				}
			}
			*it++ = ']';
			return it;
		}

		template<typename Key, typename Value>
		[[maybe_unused]] auto const &
		json_get_key( std::pair<Key, Value> const &kv ) {
			return kv.first;
		}

		template<typename Key, typename Value>
		[[maybe_unused]] auto const &
		json_get_value( std::pair<Key, Value> const &kv ) {
			return kv.second;
		}

		template<typename JsonMember, typename OutputIterator, typename parse_to_t>
		[[nodiscard]] constexpr OutputIterator
		to_string( ParseTag<JsonParseTypes::KeyValueArray>, OutputIterator it,
		           parse_to_t const &container ) {

			static_assert(
			  std::is_convertible_v<parse_to_t, typename JsonMember::parse_to_t>,
			  "value must be convertible to specified type in class contract" );
			using key_t = typename JsonMember::json_key_t;
			using value_t = typename JsonMember::json_value_t;
			*it++ = '[';
			if( not std::empty( container ) ) {
				auto count = std::size( container ) - 1U;
				for( auto const &v : container ) {
					*it++ = '{';
					// Append Key Name
					*it++ = '"';
					it = copy_to_iterator( key_t::name, it );
					*it++ = '"';
					*it++ = ':';
					// Append Key Value
					it = to_string<key_t>( ParseTag<key_t::expected_type>{}, it,
					                       json_get_key( v ) );

					*it++ = ',';
					// Append Value Name
					*it++ = '"';
					it = copy_to_iterator( value_t::name, it );
					*it++ = '"';
					*it++ = ':';
					// Append Value Value
					it = to_string<value_t>( ParseTag<value_t::expected_type>{}, it,
					                         json_get_value( v ) );

					*it++ = '}';
					if( count-- > 0 ) {
						*it++ = ',';
					}
				}
			}
			*it++ = ']';
			return it;
		}

		template<typename JsonMember, typename OutputIterator, typename parse_to_t>
		[[nodiscard]] constexpr OutputIterator
		to_string( ParseTag<JsonParseTypes::KeyValue>, OutputIterator it,
		           parse_to_t const &container ) {

			static_assert(
			  std::is_convertible_v<parse_to_t, typename JsonMember::parse_to_t>,
			  "value must be convertible to specified type in class contract" );

			*it++ = '{';
			if( not std::empty( container ) ) {
				auto count = std::size( container ) - 1U;
				for( auto const &v : container ) {
					it = to_string<typename JsonMember::json_key_t>(
					  ParseTag<JsonMember::json_key_t::expected_type>{}, it,
					  json_get_key( v ) );
					*it++ = ':';
					it = to_string<typename JsonMember::json_element_t>(
					  ParseTag<JsonMember::json_element_t::expected_type>{}, it,
					  json_get_value( v ) );
					if( count-- > 0 ) {
						*it++ = ',';
					}
				}
			}
			*it++ = '}';
			return it;
		}

		template<typename JsonMember, typename OutputIterator, typename T>
		[[nodiscard]] constexpr OutputIterator member_to_string( OutputIterator it,
		                                                         T const &value ) {
			it = to_string<JsonMember>( ParseTag<JsonMember::expected_type>{},
			                            daw::move( it ), value );
			return it;
		}

		template<typename JsonMember>
		using tag_member_t = typename JsonMember::tag_member;

		template<typename JsonMember>
		inline constexpr bool has_tag_member_v =
		  daw::is_detected_v<tag_member_t, JsonMember>;

		template<size_t pos, typename JsonMember, typename OutputIterator,
		         typename Value, typename VisitedMembers>
		constexpr void tags_to_json_str( bool &is_first, OutputIterator it,
		                                 Value const &v,
		                                 VisitedMembers &visited_members ) {
			if constexpr( has_tag_member_v<JsonMember> ) {
				using tag_member = tag_member_t<JsonMember>;
				constexpr auto tag_member_name = daw::string_view( tag_member::name );
				if( daw::algorithm::contains( visited_members.begin( ),
				                              visited_members.end( ),
				                              tag_member_name ) ) {
					return;
				}
				visited_members.push_back( tag_member_name );
				if( not is_first ) {
					*it++ = ',';
				}
				is_first = false;
				*it++ = '"';
				it = copy_to_iterator<false, EightBitModes::AllowFull>( tag_member_name,
				                                                        it );
				it = copy_to_iterator<false, EightBitModes::AllowFull>( "\":", it );
				it = member_to_string<tag_member>(
				  daw::move( it ), typename JsonMember::switcher{}( v ) );
			}
		}

		template<size_t pos, typename JsonMember, typename OutputIterator,
		         typename... Args, typename Value, typename Visited>
		constexpr void to_json_str( bool &is_first, OutputIterator it,
		                            std::tuple<Args...> const &tp, Value const &v,
		                            Visited &visited_members ) {
			constexpr auto json_member_name = daw::string_view( JsonMember::name );
			if( daw::algorithm::contains( visited_members.begin( ),
			                              visited_members.end( ),
			                              json_member_name ) ) {
				return;
			}
			visited_members.push_back( json_member_name );
			static_assert( is_a_json_type_v<JsonMember>, "Unsupported data type" );
			if constexpr( is_json_nullable_v<JsonMember> ) {
				if( not std::get<pos>( tp ) ) {
					return;
				}
			}
			if( not is_first ) {
				*it++ = ',';
			}
			is_first = false;
			*it++ = '"';
			it = copy_to_iterator<false, EightBitModes::AllowFull>( JsonMember::name,
			                                                        it );
			it = copy_to_iterator<false, EightBitModes::AllowFull>( "\":", it );
			it = member_to_string<JsonMember>( daw::move( it ), std::get<pos>( tp ) );
		}
	} // namespace
} // namespace daw::json::impl
