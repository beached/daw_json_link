// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/daw_json_link
//

#pragma once

#include "version.h"

#include "daw_json_assert.h"
#include "daw_json_parse_iso8601_utils.h"
#include "daw_json_value.h"

#include <daw/daw_algorithm.h>
#include <daw/daw_arith_traits.h>
#include <daw/daw_cpp_feature_check.h>
#include <daw/daw_cxmath.h>
#include <daw/daw_likely.h>
#include <daw/daw_move.h>
#include <daw/daw_traits.h>
#include <daw/daw_visit.h>
#include <utf8/unchecked.h>

#include <array>
#include <ciso646>
#include <optional>
#include <sstream>
#include <string>

#ifndef DAW_JSON_CUSTOM_D2S
#include <third_party/dragonbox/dragonbox.h>
#elif DAW_HAS_INCLUDE( "custom_d2s.h" )
#include "custom_d2s.h"
#else
#error Request for local d2s, but no custom_d2s.h supplied with char * d2s( Real const & value, char * ); declaration/definition in namespace daw::json
#endif
#include <type_traits>
#include <variant>

namespace daw::json {
	inline namespace DAW_JSON_VER {
		template<typename Real, typename OutputIterator>
		constexpr OutputIterator real2string( Real const &value,
		                                      OutputIterator out_it ) {
			// TODO: Customization point, add to readme
#ifndef DAW_JSON_CUSTOM_D2S
			if constexpr( std::is_same<Real, float>::value ) {
				return jkj::dragonbox::to_chars_n( value, out_it );
			} else {
				return jkj::dragonbox::to_chars_n( static_cast<double>( value ),
				                                   out_it );
			}
#else
			if constexpr( std::is_same<Real, float>::value ) {
				return d2s( value, out_it );
			} else {
				return d2s( static_cast<double>( value ), out_it );
			}
#endif
		}

		namespace json_details::to_strings {
			using std::to_string;
			// Need to use ADL to_string in unevaluated contexts.  Limiting to it's
			// own namespace
			template<typename T>
			[[nodiscard, maybe_unused]] constexpr auto
			to_string( std::optional<T> const &v ) -> decltype( to_string( *v ) ) {
				if( not has_value( v ) ) {
					return { "null" };
				}
				return to_string( *v );
			}

			namespace to_string_test {
				template<typename T>
				[[maybe_unused]] auto to_string_test( T &&v )
				  -> decltype( to_string( DAW_FWD2( T, v ) ) );

				template<typename T>
				using to_string_result =
				  decltype( to_string_test( std::declval<T>( ) ) );
			} // namespace to_string_test

			template<typename T>
			using has_to_string =
			  daw::is_detected<to_string_test::to_string_result, T>;

			template<typename T>
			inline constexpr bool has_to_string_v = has_to_string<T>::value;

		} // namespace json_details::to_strings
		namespace json_details {
			template<typename T>
			using from_string_test = decltype( from_string(
			  std::declval<daw::tag_t<T>>( ), std::declval<std::string_view>( ) ) );

			template<typename T>
			inline constexpr bool has_from_string_v =
			  daw::is_detected_v<from_string_test, T>;

			template<typename T, typename U>
			using has_lshift_test = decltype( operator<<(
			  std::declval<T &>( ), std::declval<U const &>( ) ) );

			template<typename T, typename U>
			using has_rshift_test = decltype( operator>>(
			  std::declval<T &>( ), std::declval<U const &>( ) ) );

			template<typename T>
			inline constexpr bool has_ostream_op_v =
			  daw::is_detected_v<has_lshift_test, std::stringstream, T>;

			template<typename T>
			inline constexpr bool has_istream_op_v =
			  daw::is_detected_v<has_rshift_test, std::stringstream, T>;

		} // namespace json_details

		/***
		 * This is the default ToJsonConverter for json_raw. By default is will
		 * return the stringified version of the value if, to_string( T ) exists.
		 * Otherwise it will fallback to an std::ostream converter for T if it
		 * exists.
		 * @tparam T type of value to convert to a string
		 */
		template<typename T>
		struct default_to_json_converter_t {
			template<typename U>
			[[nodiscard]] static inline auto use_stream( U const &v ) {
				std::stringstream ss{ };
				ss << v;
				return DAW_MOVE( ss ).str( );
			}

			template<typename U>
			[[nodiscard]] inline constexpr auto operator( )( U const &value ) const {
				if constexpr( json_details::is_string_view_like_v<U> ) {
					return std::string_view( std::data( value ), std::size( value ) );
				} else if constexpr( json_details::to_strings::has_to_string_v<U> ) {
					using json_details::to_strings::to_string;
					return to_string( value );
				} else if constexpr( json_details::has_ostream_op_v<U> ) {
					return use_stream( value );
				} else if constexpr( std::is_convertible_v<U, std::string_view> ) {
					return static_cast<std::string_view>( value );
				} else if constexpr( std::is_convertible_v<U, std::string> ) {
					return static_cast<std::string>( value );
				} else if constexpr( daw::is_arithmetic_v<U> ) {
					return to_string( value );
				} else if constexpr( std::is_enum_v<U> ) {
					return to_string( static_cast<std::underlying_type_t<U>>( value ) );
				} else if constexpr( json_details::can_deref_v<U> ) {
					static_assert( json_details::has_op_bool_v<U>,
					               "default_to_converter cannot work with type" );
					using deref_t = daw::remove_cvref_t<decltype( *value )>;
					if constexpr( json_details::is_string_view_like_v<deref_t> ) {
						if( value ) {
							auto const &v = *value;
							return std::string_view( std::data( v ), std::size( v ) );
						}
						return std::string_view( "null", 4 );
					} else if constexpr( json_details::to_strings::has_to_string_v<
					                       deref_t> ) {
						if( value ) {
							using json_details::to_strings::to_string;
							return to_string( *value );
						} else {
							using result_t =
							  daw::remove_cvref_t<decltype( to_string( *value ) )>;
							return result_t{ "null" };
						}
					} else if constexpr( std::is_convertible_v<deref_t,
					                                           std::string_view> ) {
						if( value ) {
							return static_cast<std::string_view>( value );
						}
						return std::string_view{ "null" };
					} else if constexpr( std::is_convertible_v<deref_t, std::string> ) {
						if( value ) {
							return static_cast<std::string>( value );
						}
						return std::string( "null" );
					} else {
						if( value ) {
							return use_stream( *value );
						} else {
							return std::string( "null" );
						}
					}
				}
			}
		};

		namespace from_json_conv_details {
			template<typename T>
			[[nodiscard]] inline auto use_stream( std::string_view sv ) {
				std::stringstream ss{ };
				ss << sv;
				T result;
				ss >> result;
				return result;
			}
		} // namespace from_json_conv_details

		template<typename T>
		struct default_from_json_converter_t {
			[[nodiscard]] inline constexpr decltype( auto )
			operator( )( std::string_view sv ) const {
				if constexpr( std::disjunction<
				                std::is_same<T, std::string_view>,
				                std::is_same<T, std::optional<std::string_view>>>::
				                value ) {
					return sv;
				} else if constexpr( json_details::has_from_string_v<T> ) {
					return from_string( daw::tag<T>, sv );
				} else if constexpr( std::is_convertible_v<std::string_view, T> ) {
					return static_cast<T>( sv );
				} else if constexpr( std::is_convertible_v<std::string, T> ) {
					return static_cast<T>( static_cast<std::string>( sv ) );
				} else {
					static_assert( json_details::has_istream_op_v<T>,
					               "Unsupported type in default to converter.  Must "
					               "supply a custom one" );
					static_assert( std::is_default_constructible_v<T>,
					               "Unsupported type in default to converter.  Must "
					               "supply a custom one" );
					return from_json_conv_details::use_stream<T>( sv );
				}
			}
		};

		namespace json_details {
			template<typename JsonMember, typename OutputIterator,
			         typename parse_to_t>
			[[nodiscard]] inline OutputIterator constexpr to_daw_json_string(
			  ParseTag<JsonParseTypes::Null>, OutputIterator it,
			  parse_to_t const &value );

			template<typename JsonMember, typename OutputIterator,
			         typename parse_to_t>
			[[nodiscard]] constexpr OutputIterator
			to_daw_json_string( ParseTag<JsonParseTypes::Array>, OutputIterator it,
			                    parse_to_t const &value );

			template<typename JsonMember, typename OutputIterator,
			         typename parse_to_t>
			[[nodiscard]] constexpr OutputIterator
			to_daw_json_string( ParseTag<JsonParseTypes::KeyValueArray>,
			                    OutputIterator it, parse_to_t const &value );

			template<typename JsonMember, typename OutputIterator,
			         typename parse_to_t>
			[[nodiscard]] constexpr OutputIterator
			to_daw_json_string( ParseTag<JsonParseTypes::KeyValue>, OutputIterator it,
			                    parse_to_t const &value );

			template<typename JsonMember, typename OutputIterator,
			         typename parse_to_t>
			[[nodiscard]] constexpr OutputIterator
			to_daw_json_string( ParseTag<JsonParseTypes::Custom>, OutputIterator it,
			                    parse_to_t const &value );

			template<typename JsonMember, typename OutputIterator,
			         typename parse_to_t>
			[[nodiscard]] constexpr OutputIterator
			to_daw_json_string( ParseTag<JsonParseTypes::Variant>, OutputIterator it,
			                    parse_to_t const &value );

			template<typename JsonMember, typename OutputIterator,
			         typename parse_to_t>
			[[nodiscard]] constexpr OutputIterator
			to_daw_json_string( ParseTag<JsonParseTypes::Class>, OutputIterator it,
			                    parse_to_t const &value );

			template<typename JsonMember, typename OutputIterator,
			         typename parse_to_t>
			[[nodiscard]] inline OutputIterator
			to_daw_json_string( ParseTag<JsonParseTypes::Real>, OutputIterator it,
			                    parse_to_t const &value );

			template<typename JsonMember, typename OutputIterator,
			         typename parse_to_t>
			[[nodiscard]] constexpr OutputIterator
			to_daw_json_string( ParseTag<JsonParseTypes::Signed>, OutputIterator it,
			                    parse_to_t const &value );

			template<typename JsonMember, typename OutputIterator,
			         typename parse_to_t>
			[[nodiscard]] constexpr OutputIterator
			to_daw_json_string( ParseTag<JsonParseTypes::Unsigned>, OutputIterator it,
			                    parse_to_t const &value );

			template<typename JsonMember, typename OutputIterator,
			         typename parse_to_t>
			[[nodiscard]] constexpr OutputIterator
			to_daw_json_string( ParseTag<JsonParseTypes::StringRaw>,
			                    OutputIterator it, parse_to_t const &value );

			template<typename JsonMember, typename OutputIterator,
			         typename parse_to_t>
			[[nodiscard]] constexpr OutputIterator
			to_daw_json_string( ParseTag<JsonParseTypes::StringEscaped>,
			                    OutputIterator it, parse_to_t const &value );

			template<typename JsonMember, typename OutputIterator,
			         typename parse_to_t>
			[[nodiscard]] constexpr OutputIterator
			to_daw_json_string( ParseTag<JsonParseTypes::Date>, OutputIterator it,
			                    parse_to_t const &value );

			template<typename JsonMember, typename OutputIterator,
			         typename parse_to_t>
			[[nodiscard]] constexpr OutputIterator
			to_daw_json_string( ParseTag<JsonParseTypes::Bool>, OutputIterator it,
			                    parse_to_t const &value );

			//************************************************
			template<typename Char>
			constexpr char to_nibble_char( Char c ) {
				auto const u = static_cast<unsigned>( static_cast<unsigned char>( c ) );
				daw_json_assert( u < 16, ErrorReason::InvalidUTFEscape );
				if( u < 10 ) {
					return static_cast<char>( u + static_cast<unsigned char>( '0' ) );
				} else {
					return static_cast<char>( ( u - 10U ) +
					                          static_cast<unsigned char>( 'A' ) );
				}
			}

			template<typename OutputIterator>
			constexpr OutputIterator output_hex( std::uint16_t c,
			                                     OutputIterator it ) {
				char const nibbles[] = { '\\',
				                         'u',
				                         to_nibble_char( ( c >> 12U ) & 0xFU ),
				                         to_nibble_char( ( c >> 8U ) & 0xFU ),
				                         to_nibble_char( ( c >> 4U ) & 0xFU ),
				                         to_nibble_char( c & 0xFU ) };

				*it++ = nibbles[0];
				*it++ = nibbles[1];
				*it++ = nibbles[2];
				*it++ = nibbles[3];
				*it++ = nibbles[4];
				*it++ = nibbles[5];
				return it;
			}

			template<typename OutputIterator>
			constexpr void utf32_to_utf8( std::uint32_t cp, OutputIterator &it ) {
				if( cp <= 0x7FU ) {
					*it++ = static_cast<char>( cp );
					return;
				}
				if( cp <= 0x7FFU ) {
					*it++ = static_cast<char>( ( cp >> 6U ) | 0b11000000U );
					*it++ = static_cast<char>( ( cp & 0b00111111U ) | 0b10000000U );
					return;
				}
				if( cp <= 0xFFFFU ) {
					*it++ = static_cast<char>( ( cp >> 12U ) | 0b11100000U );
					*it++ =
					  static_cast<char>( ( ( cp >> 6U ) & 0b00111111U ) | 0b10000000U );
					*it++ = static_cast<char>( ( cp & 0b00111111U ) | 0b10000000U );
					return;
				}
				if( cp <= 0x10FFFFU ) {
					*it++ = static_cast<char>( ( cp >> 18U ) | 0b11110000U );
					*it++ =
					  static_cast<char>( ( ( cp >> 12U ) & 0b00111111U ) | 0b10000000U );
					*it++ =
					  static_cast<char>( ( ( cp >> 6U ) & 0b00111111U ) | 0b10000000U );
					*it++ = static_cast<char>( ( cp & 0b00111111U ) | 0b10000000U );
					return;
				}
				daw_json_error( ErrorReason::InvalidUTFCodepoint );
			}
		} // namespace json_details

		namespace utils {
			template<bool do_escape = false,
			         EightBitModes EightBitMode = EightBitModes::AllowFull,
			         typename OutputIterator, typename Container,
			         std::enable_if_t<
			           traits::is_container_like_v<daw::remove_cvref_t<Container>>,
			           std::nullptr_t> = nullptr>
			[[nodiscard]] constexpr OutputIterator
			copy_to_iterator( OutputIterator it, Container const &container ) {
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
							if( cp < 0x20U ) {
								it = json_details::output_hex( static_cast<std::uint16_t>( cp ),
								                               it );
								break;
							}
							if constexpr( EightBitMode == EightBitModes::DisallowHigh ) {
								if( cp >= 0x7FU and cp <= 0xFFFFU ) {
									it = json_details::output_hex(
									  static_cast<std::uint16_t>( cp ), it );
									break;
								}
								if( cp > 0xFFFFU ) {
									it = json_details::output_hex(
									  static_cast<std::uint16_t>( 0xD7C0U + ( cp >> 10U ) ), it );
									it = json_details::output_hex(
									  static_cast<std::uint16_t>( 0xDC00U + ( cp & 0x3FFU ) ),
									  it );
									break;
								}
							}
							json_details::utf32_to_utf8( cp, it );
							break;
						}
					}
				} else {
					for( auto c : container ) {
						if constexpr( EightBitMode == EightBitModes::DisallowHigh ) {
							daw_json_assert( ( static_cast<unsigned char>( c ) >= 0x20U and
							                   static_cast<unsigned char>( c ) <= 0x7FU ),
							                 ErrorReason::InvalidStringHighASCII );
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
			copy_to_iterator( OutputIterator it, char const *ptr ) {
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
							if( cp < 0x20U ) {
								it = json_details::output_hex( static_cast<std::uint16_t>( cp ),
								                               it );
								break;
							}
							if constexpr( EightBitMode == EightBitModes::DisallowHigh ) {
								if( cp >= 0x7FU and cp <= 0xFFFFU ) {
									it = json_details::output_hex(
									  static_cast<std::uint16_t>( cp ), it );
									break;
								}
								if( cp > 0xFFFFU ) {
									it = json_details::output_hex(
									  static_cast<std::uint16_t>( 0xD7C0U + ( cp >> 10U ) ), it );
									it = output_hex(
									  static_cast<std::uint16_t>( 0xDC00U + ( cp & 0x3FFU ) ),
									  it );
									break;
								}
							}
							json_details::utf32_to_utf8( cp, it );
							break;
						}
					}
				} else {
					while( *ptr != '\0' ) {
						if constexpr( EightBitMode == EightBitModes::DisallowHigh ) {
							daw_json_assert( ( static_cast<unsigned>( *ptr ) >= 0x20U and
							                   static_cast<unsigned>( *ptr ) <= 0x7FU ),
							                 ErrorReason::InvalidStringHighASCII );
						}
						*it++ = *ptr++;
					}
				}
				return it;
			}

			template<bool do_escape = false,
			         EightBitModes EightBitMode = EightBitModes::AllowFull,
			         typename OutputIterator, typename ParseState>
			[[nodiscard]] constexpr OutputIterator
			copy_to_iterator( OutputIterator it,
			                  basic_json_value<ParseState> const &jv ) {
				if( jv.is_null( ) ) {
					return copy_to_iterator<do_escape, EightBitMode>( it, "null" );
				} else {
					return copy_to_iterator<do_escape, EightBitMode>(
					  it, jv.get_string_view( ) );
				}
			}
		} // namespace utils

		namespace json_details {
			template<typename JsonMember, typename OutputIterator,
			         typename parse_to_t>
			[[nodiscard]] constexpr OutputIterator
			to_daw_json_string( ParseTag<JsonParseTypes::Bool>, OutputIterator it,
			                    parse_to_t const &value ) {

				if constexpr( JsonMember::literal_as_string ==
				              LiteralAsStringOpt::Always ) {
					*it++ = '"';
				}
				if( value ) {
					it = utils::copy_to_iterator( it, "true" );
				} else {
					it = utils::copy_to_iterator( it, "false" );
				}
				if constexpr( JsonMember::literal_as_string ==
				              LiteralAsStringOpt::Always ) {
					*it++ = '"';
				}
				return it;
			}

			template<std::size_t idx, typename JsonMembers, typename OutputIterator,
			         typename parse_to_t>
			constexpr void to_variant_string( OutputIterator &it,
			                                  parse_to_t const &value ) {
				if constexpr( idx < std::variant_size<parse_to_t>::value ) {
					if( value.index( ) != idx ) {
						to_variant_string<idx + 1, JsonMembers>( it, value );
						return;
					}
					using element_t = typename JsonMembers::json_elements;
					using JsonMember =
					  typename pack_element<idx, typename element_t::element_map_t>::type;
					it = to_daw_json_string<JsonMember>(
					  ParseTag<JsonMember::base_expected_type>{ }, it,
					  daw::get_nt<idx>( value ) );
				}
			}

			template<typename JsonMember, typename OutputIterator,
			         typename parse_to_t>
			[[nodiscard]] inline constexpr OutputIterator
			to_daw_json_string( ParseTag<JsonParseTypes::Variant>, OutputIterator it,
			                    parse_to_t const &value ) {

				assert( value.index( ) >= 0 );
				to_variant_string<0, JsonMember>( it, value );
				return it;
			}

			template<typename JsonMember, typename OutputIterator,
			         typename parse_to_t>
			[[nodiscard]] inline constexpr OutputIterator
			to_daw_json_string( ParseTag<JsonParseTypes::VariantTagged>,
			                    OutputIterator it, parse_to_t const &value ) {

				to_variant_string<0, JsonMember>( it, value );
				return it;
			}

			template<typename T>
			[[maybe_unused]] constexpr auto deref_detect( T &&value )
			  -> decltype( *value );

			[[maybe_unused]] inline constexpr void deref_detect( ... ) {}

			template<typename T>
			using deref_t =
			  daw::remove_cvref_t<decltype( deref_detect( std::declval<T>( ) ) )>;

			template<typename Optional>
			inline constexpr bool is_valid_optional_v =
			  daw::is_detected<deref_t, Optional>::value;

			template<typename JsonMember, typename OutputIterator, typename Optional>
			[[nodiscard]] inline constexpr OutputIterator
			to_daw_json_string( ParseTag<JsonParseTypes::Null>, OutputIterator it,
			                    Optional const &value ) {
				static_assert( is_valid_optional_v<Optional> );

				if( not json_details::has_value( value ) ) {
					return utils::copy_to_iterator( it, "null" );
				}
				using tag_type = ParseTag<JsonMember::base_expected_type>;
				if constexpr( json_details::has_op_star_v<Optional> ) {
					return to_daw_json_string<JsonMember>( tag_type{ }, it, *value );
				} else {
					return to_daw_json_string<JsonMember>( tag_type{ }, it, value );
				}
			}

			template<typename JsonMember, typename OutputIterator,
			         typename parse_to_t>
			[[nodiscard]] inline OutputIterator
			to_daw_json_string( ParseTag<JsonParseTypes::Real>, OutputIterator it,
			                    parse_to_t const &value ) {

				static_assert(
				  std::is_convertible<parse_to_t,
				                      typename JsonMember::parse_to_t>::value,
				  "value must be convertible to specified type in class contract" );

				if constexpr( std::is_floating_point<
				                typename JsonMember::parse_to_t>::value ) {
					if( daw::cxmath::is_nan( value ) ) {
						if constexpr( JsonMember::literal_as_string ==
						              LiteralAsStringOpt::Never ) {
							daw_json_error( ErrorReason::NumberIsNaN );
						} else {
							*it++ = '"';
							it = utils::copy_to_iterator( it, "NaN" );
							*it++ = '"';
							return it;
						}
					} else if( daw::cxmath::is_inf( value ) ) {
						if constexpr( JsonMember::literal_as_string ==
						              LiteralAsStringOpt::Never ) {
							daw_json_error( ErrorReason::NumberIsInf );
						} else {
							*it++ = '"';
							it = utils::copy_to_iterator( it, "Infinity" );
							*it++ = '"';
							return it;
						}
					}
				}

				if constexpr( JsonMember::literal_as_string ==
				              LiteralAsStringOpt::Always ) {
					*it++ = '"';
				}
				if constexpr( daw::is_floating_point_v<parse_to_t> ) {
					static_assert( sizeof( parse_to_t ) <= sizeof( double ) );
					if constexpr( std::is_same<OutputIterator, char *>::value ) {
						it = real2string( value, it );
					} else {
						char buff[50]{ };
						buff[49] = 0;
						char *ptr = buff;
						ptr = real2string( value, ptr );
						std::copy( buff, ptr, it );
					}
				} else {
					using std::to_string;
					using to_strings::to_string;
					it = utils::copy_to_iterator( it, to_string( value ) );
				}
				if constexpr( JsonMember::literal_as_string ==
				              LiteralAsStringOpt::Always ) {
					*it++ = '"';
				}
				return it;
			}

			template<typename T>
			using base_int_type_impl = std::underlying_type<T>;

			template<typename T>
			using base_int_type_t =
			  typename std::conditional_t<std::is_enum_v<T>, base_int_type_impl<T>,
			                              daw::traits::identity<T>>::type;

			inline constexpr auto digits100 = [] {
				std::array<char[2], 100> result{ };
				for( size_t n = 0; n < 100; ++n ) {
					result[n][0] =
					  static_cast<char>( ( n % 10 ) + static_cast<unsigned char>( '0' ) );
					result[n][1] =
					  static_cast<char>( ( n / 10 ) + static_cast<unsigned char>( '0' ) );
				}
				return result;
			}( );

			template<typename JsonMember, typename OutputIterator,
			         typename parse_to_t>
			[[nodiscard]] constexpr OutputIterator
			to_daw_json_string( ParseTag<JsonParseTypes::Signed>, OutputIterator it,
			                    parse_to_t const &value ) {

				static_assert(
				  std::is_convertible<parse_to_t,
				                      typename JsonMember::parse_to_t>::value,
				  "value must be convertible to specified type in class contract" );

				using std::to_string;
				using to_strings::to_string;
				using under_type = base_int_type_t<parse_to_t>;

				if constexpr( JsonMember::literal_as_string ==
				              LiteralAsStringOpt::Always ) {
					*it++ = '"';
				}
				if constexpr( std::disjunction<std::is_enum<parse_to_t>,
				                               daw::is_integral<parse_to_t>>::value ) {
					auto v = static_cast<under_type>( value );

					char buff[daw::numeric_limits<under_type>::digits10 + 1]{ };
					char *ptr = buff;
					if( v < 0 ) {
						*it++ = '-';
						// Do 1 round here just in case we are
						// daw::numeric_limits<intmax_t>::min( ) and cannot negate
						// This is a subtraction because v < 0 % 10 is negative
						*ptr++ = static_cast<char>( '0' - static_cast<char>( v % 10 ) );
						v /= -10;
						if( v == 0 ) {
							*it++ = buff[0];
							if constexpr( JsonMember::literal_as_string ==
							              LiteralAsStringOpt::Always ) {
								*it++ = '"';
							}
							return it;
						}
					}
					if( v == 0 ) {
						*ptr++ = '0';
					}
					while( v >= 10 ) {
						auto const tmp = static_cast<std::size_t>( v % 100 );
						v /= 100;
						ptr[0] = digits100[tmp][0];
						ptr[1] = digits100[tmp][1];
						ptr += 2;
					}
					if( v > 0 ) {
						*ptr++ = static_cast<char>( '0' + static_cast<char>( v ) );
					}
					--ptr;
					*it++ = *ptr;
					while( ptr != buff ) {
						--ptr;
						*it++ = *ptr;
					}
				} else {
					// Fallback to ADL
					it = utils::copy_to_iterator( it, to_string( value ) );
				}
				if constexpr( JsonMember::literal_as_string ==
				              LiteralAsStringOpt::Always ) {
					*it++ = '"';
				}
				return it;
			}

			template<typename JsonMember, typename OutputIterator,
			         typename parse_to_t>
			[[nodiscard]] constexpr OutputIterator
			to_daw_json_string( ParseTag<JsonParseTypes::Unsigned>, OutputIterator it,
			                    parse_to_t const &value ) {

				static_assert(
				  std::is_convertible<parse_to_t,
				                      typename JsonMember::parse_to_t>::value,
				  "value must be convertible to specified type in class contract" );

				using std::to_string;
				using to_strings::to_string;
				using under_type = base_int_type_t<parse_to_t>;

				if constexpr( JsonMember::literal_as_string ==
				              LiteralAsStringOpt::Always ) {
					*it++ = '"';
				}
				if constexpr( std::disjunction<std::is_enum<parse_to_t>,
				                               daw::is_integral<parse_to_t>>::value ) {
					auto v = static_cast<under_type>( value );

					if( DAW_UNLIKELY( v == 0 ) ) {
						*it++ = '0';
					} else {
						daw_json_assert( v > 0, ErrorReason::NumberOutOfRange );
						char buff[daw::numeric_limits<under_type>::digits10 + 1]{ };
						char *ptr = buff;
						while( v >= 10 ) {
							auto const tmp = static_cast<std::size_t>( v % 100 );
							v /= 100;
							ptr[0] = digits100[tmp][0];
							ptr[1] = digits100[tmp][1];
							ptr += 2;
						}
						if( v > 0 ) {
							*ptr++ = static_cast<char>( '0' + static_cast<char>( v ) );
						}
						--ptr;
						*it++ = *ptr;
						while( ptr != buff ) {
							--ptr;
							*it++ = *ptr;
						}
					}
				} else {
					// Fallback to ADL
					it = utils::copy_to_iterator( it, to_string( value ) );
				}
				if constexpr( JsonMember::literal_as_string ==
				              LiteralAsStringOpt::Always ) {
					*it++ = '"';
				}
				return it;
			}
		} // namespace json_details

		namespace utils {
			namespace utils_details {
				template<typename Integer>
				struct number {
					using parse_to_t = Integer;
					static constexpr LiteralAsStringOpt literal_as_string =
					  LiteralAsStringOpt::Never;
				};
			} // namespace utils_details

			template<typename Integer, typename OutputIterator>
			inline constexpr OutputIterator
			integer_to_string( OutputIterator it, Integer const &value ) {
				static_assert( daw::is_integral_v<Integer> );

				if constexpr( daw::is_unsigned_v<Integer> ) {
					return json_details::to_daw_json_string<
					  utils_details::number<Integer>>(
					  ParseTag<JsonParseTypes::Unsigned>{ }, it, value );
				} else {
					return json_details::to_daw_json_string<
					  utils_details::number<Integer>>(
					  ParseTag<JsonParseTypes::Signed>{ }, it, value );
				}
			}
		} // namespace utils

		namespace json_details {
			template<typename JsonMember, typename OutputIterator,
			         typename parse_to_t>
			[[nodiscard]] inline constexpr OutputIterator
			to_daw_json_string( ParseTag<JsonParseTypes::StringRaw>,
			                    OutputIterator it, parse_to_t const &value ) {

				static_assert(
				  std::is_convertible<parse_to_t,
				                      typename JsonMember::parse_to_t>::value,
				  "Value must be convertible to specialized type in "
				  "json_data_contract" );

				constexpr EightBitModes eight_bit_mode = JsonMember::eight_bit_mode;
				*it++ = '"';
				if( std::size( value ) > 0U ) {
					it = utils::copy_to_iterator<false, eight_bit_mode>( it, value );
				}
				*it++ = '"';
				return it;
			}

			template<typename JsonMember, typename OutputIterator,
			         typename parse_to_t>
			[[nodiscard]] inline constexpr OutputIterator
			to_daw_json_string( ParseTag<JsonParseTypes::StringEscaped>,
			                    OutputIterator it, parse_to_t const &value ) {

				/* TODO is something like this necessary
				 static_assert(
				  std::is_convertible<parse_to_t, typename
				 JsonMember::parse_to_t>::value, "value must be convertible to
				 specified type in class contract" );
				  */

				constexpr EightBitModes eight_bit_mode = JsonMember::eight_bit_mode;
				*it++ = '"';
				it = utils::copy_to_iterator<true, eight_bit_mode>( it, value );
				*it++ = '"';
				return it;
			}

			template<typename T>
			[[nodiscard]] inline constexpr bool is_null( std::optional<T> const &v ) {
				return not static_cast<bool>( v );
			}

			template<typename T>
			[[nodiscard]] inline constexpr bool is_null( T const & ) {
				return false;
			}

			template<typename JsonMember, typename OutputIterator,
			         typename parse_to_t>
			[[nodiscard]] constexpr OutputIterator
			to_daw_json_string( ParseTag<JsonParseTypes::Date>, OutputIterator it,
			                    parse_to_t const &value ) {

				static_assert(
				  std::is_convertible<parse_to_t,
				                      typename JsonMember::parse_to_t>::value,
				  "value must be convertible to specified type in class contract" );

				using json_details::is_null;
				// TODO: document customization point
				if( is_null( value ) ) {
					return utils::copy_to_iterator( it, "null" );
				}
				*it++ = '"';
				datetime::ymdhms const civil = datetime::time_point_to_civil( value );
				it = utils::integer_to_string( it, civil.year );
				*it++ = '-';
				if( civil.month < 10 ) {
					*it++ = '0';
				}
				it = utils::integer_to_string( it, civil.month );
				*it++ = '-';
				if( civil.day < 10 ) {
					*it++ = '0';
				}
				it = utils::integer_to_string( it, civil.day );
				*it++ = 'T';
				if( civil.hour < 10 ) {
					*it++ = '0';
				}
				it = utils::integer_to_string( it, civil.hour );
				*it++ = ':';
				if( civil.minute < 10 ) {
					*it++ = '0';
				}
				it = utils::integer_to_string( it, civil.minute );
				*it++ = ':';
				if( civil.second < 10 ) {
					*it++ = '0';
				}
				it = utils::integer_to_string( it, civil.second );
				if( civil.millisecond > 0 ) {
					*it++ = '.';
					it = utils::integer_to_string( it, civil.millisecond );
				}
				*it++ = 'Z';
				*it++ = '"';
				return it;
			}

			template<typename JsonMember, typename OutputIterator,
			         typename parse_to_t>
			[[nodiscard]] inline constexpr OutputIterator
			to_daw_json_string( ParseTag<JsonParseTypes::Unknown>, OutputIterator it,
			                    parse_to_t const &value ) {

				return utils::copy_to_iterator( it, value );
			}

			template<typename JsonMember, typename OutputIterator,
			         typename parse_to_t>
			[[nodiscard]] inline constexpr OutputIterator
			to_daw_json_string( ParseTag<JsonParseTypes::Class>, OutputIterator it,
			                    parse_to_t const &value ) {

				static_assert(
				  std::is_convertible<parse_to_t,
				                      typename JsonMember::parse_to_t>::value,
				  "value must be convertible to specified type in class contract" );

				if constexpr( has_json_to_json_data_v<parse_to_t> ) {
					return json_data_contract_trait_t<parse_to_t>::serialize(
					  it, json_data_contract<parse_to_t>::to_json_data( value ), value );
				} else if constexpr( is_json_map_alias_v<parse_to_t> ) {
					return json_data_contract_trait_t<parse_to_t>::serialize( it, value,
					                                                          value );
				} else {
					static_assert( is_submember_tagged_variant_v<parse_to_t>,
					               "Could not find appropriate mapping or to_json_data "
					               "member of json_data_contract" );
					return json_data_contract_trait_t<parse_to_t>::serialize( it, value );
				}
			}

			template<typename JsonMember, typename OutputIterator,
			         typename parse_to_t>
			[[nodiscard]] inline constexpr OutputIterator
			to_daw_json_string( ParseTag<JsonParseTypes::Custom>, OutputIterator it,
			                    parse_to_t const &value ) {

				static_assert(
				  std::is_convertible<parse_to_t,
				                      typename JsonMember::parse_to_t>::value,
				  "value must be convertible to specified type in class contract" );

				if constexpr( JsonMember::custom_json_type != JsonRawTypes::Literal ) {
					*it++ = '"';
					if constexpr( std::is_invocable_r<
					                OutputIterator, typename JsonMember::to_converter_t,
					                OutputIterator, parse_to_t>::value ) {

						it = typename JsonMember::to_converter_t{ }( it, value );
					} else {
						it = utils::copy_to_iterator(
						  it, typename JsonMember::to_converter_t{ }( value ) );
					}
					*it++ = '"';
					return it;
				} else {
					return utils::copy_to_iterator(
					  it, typename JsonMember::to_converter_t{ }( value ) );
				}
			}

			template<typename JsonMember, typename OutputIterator,
			         typename parse_to_t>
			[[nodiscard]] constexpr OutputIterator
			to_daw_json_string( ParseTag<JsonParseTypes::Array>, OutputIterator it,
			                    parse_to_t const &value ) {

				using array_t = typename JsonMember::parse_to_t;
				if constexpr( is_container_v<array_t> ) {
					static_assert(
					  std::is_convertible<parse_to_t, array_t>::value,
					  "value must be convertible to specified type in class contract" );
				} else {
					static_assert(
					  is_pointer_like<array_t>::value,
					  "This is a special case for pointer like(T*, unique_ptr<T>, "
					  "shared_ptr<T>) arrays.  In the to_json_data it is required to "
					  "encode the size of the data with the pointer.  Will take any "
					  "Container like type, but std::span like types work too" );
					static_assert(
					  is_container_v<parse_to_t>,
					  "This is a special case for pointer like(T*, unique_ptr<T>, "
					  "shared_ptr<T>) arrays.  In the to_json_data it is required to "
					  "encode the size of the data with the pointer.  Will take any "
					  "Container like type, but std::span like types work too" );
				}

				*it++ = '[';
				if( not std::empty( value ) ) {
					auto count = std::size( value ) - 1U;
					for( auto const &v : value ) {
						it = to_daw_json_string<typename JsonMember::json_element_t>(
						  ParseTag<JsonMember::json_element_t::expected_type>{ }, it, v );
						if( count-- > 0 ) {
							*it++ = ',';
						}
					}
				}
				*it++ = ']';
				return it;
			}

			template<typename Key, typename Value>
			[[maybe_unused]] inline constexpr Key const &
			json_get_key( std::pair<Key, Value> const &kv ) {
				return kv.first;
			}

			template<typename Key, typename Value>
			[[maybe_unused]] inline constexpr Value const &
			json_get_value( std::pair<Key, Value> const &kv ) {
				return kv.second;
			}

			template<typename JsonMember, typename OutputIterator,
			         typename parse_to_t>
			[[nodiscard]] constexpr OutputIterator
			to_daw_json_string( ParseTag<JsonParseTypes::KeyValueArray>,
			                    OutputIterator it, parse_to_t const &value ) {

				static_assert(
				  std::is_convertible<parse_to_t,
				                      typename JsonMember::parse_to_t>::value,
				  "value must be convertible to specified type in class contract" );
				using key_t = typename JsonMember::json_key_t;
				using value_t = typename JsonMember::json_value_t;
				*it++ = '[';
				if( not std::empty( value ) ) {
					auto count = std::size( value ) - 1U;
					for( auto const &v : value ) {
						*it++ = '{';
						// Append Key Name
						*it++ = '"';
						it = utils::copy_to_iterator( it, key_t::name );
						*it++ = '"';
						*it++ = ':';
						// Append Key Value
						it = to_daw_json_string<key_t>( ParseTag<key_t::expected_type>{ },
						                                it, json_get_key( v ) );

						*it++ = ',';
						// Append Value Name
						*it++ = '"';
						it = utils::copy_to_iterator( it, value_t::name );
						*it++ = '"';
						*it++ = ':';
						// Append Value Value
						it = to_daw_json_string<value_t>(
						  ParseTag<value_t::expected_type>{ }, it, json_get_value( v ) );

						*it++ = '}';
						if( count-- > 0 ) {
							*it++ = ',';
						}
					}
				}
				*it++ = ']';
				return it;
			}

			template<typename JsonMember, typename OutputIterator,
			         typename parse_to_t>
			[[nodiscard]] constexpr OutputIterator
			to_daw_json_string( ParseTag<JsonParseTypes::KeyValue>, OutputIterator it,
			                    parse_to_t const &value ) {

				static_assert(
				  std::is_convertible<parse_to_t,
				                      typename JsonMember::parse_to_t>::value,
				  "value must be convertible to specified type in class contract" );

				*it++ = '{';
				if( not std::empty( value ) ) {
					auto count = std::size( value ) - 1U;
					for( auto const &v : value ) {
						it = to_daw_json_string<typename JsonMember::json_key_t>(
						  ParseTag<JsonMember::json_key_t::expected_type>{ }, it,
						  json_get_key( v ) );
						*it++ = ':';
						it = to_daw_json_string<typename JsonMember::json_element_t>(
						  ParseTag<JsonMember::json_element_t::expected_type>{ }, it,
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
			[[nodiscard]] inline constexpr OutputIterator
			member_to_string( template_param<JsonMember>, OutputIterator it,
			                  T const &value ) {
				return to_daw_json_string<JsonMember>(
				  ParseTag<JsonMember::expected_type>{ }, DAW_MOVE( it ), value );
			}

			template<std::size_t, typename JsonMember, typename /*NamePack*/,
			         typename OutputIterator, typename TpArgs, typename Value,
			         typename VisitedMembers,
			         std::enable_if_t<not has_dependent_member_v<JsonMember>,
			                          std::nullptr_t> = nullptr>
			inline constexpr void
			dependent_member_to_json_str( bool &, OutputIterator const &,
			                              TpArgs const &, Value const &,
			                              VisitedMembers const & ) {

				// This is empty so that the call is able to be put into a pack
			}

			template<typename>
			struct missing_required_mapping_for {};

			template<typename Needle, typename... Haystack>
			constexpr std::size_t find_names_in_pack( daw::fwd_pack<Haystack...> ) {
				constexpr auto const names = std::array{ Haystack::name... };
				for( size_t n = 0; n < sizeof...( Haystack ); ++n ) {
					if( Needle::name == names[n] ) {
						return n;
					}
				}
				throw missing_required_mapping_for<Needle>{ };
			}

			template<std::size_t pos, typename JsonMember, typename NamePack,
			         typename OutputIterator, typename TpArgs, typename Value,
			         typename VisitedMembers,
			         std::enable_if_t<has_dependent_member_v<JsonMember>,
			                          std::nullptr_t> = nullptr>
			constexpr void
			dependent_member_to_json_str( bool &is_first, OutputIterator it,
			                              TpArgs const &args, Value const &v,
			                              VisitedMembers &visited_members ) {
				using dependent_member = dependent_member_t<JsonMember>;
				static_assert( is_a_json_type<JsonMember>::value,
				               "Unsupported data type" );
				if constexpr( JsonMember::nullable == JsonNullable::Nullable ) {
					// We have no requirement to output this member when it's null
					if( not get<pos>( args ) ) {
						return;
					}
				}
				constexpr auto dependent_member_name =
				  daw::string_view( std::data( dependent_member::name ),
				                    std::size( dependent_member::name ) );
				if( daw::algorithm::contains( std::data( visited_members ),
				                              daw::data_end( visited_members ),
				                              dependent_member_name ) ) {
					// Already outputted this member
					return;
				}
				visited_members.push_back( dependent_member_name );
				if( not is_first ) {
					*it++ = ',';
				}
				is_first = false;
				*it++ = '"';
				it = utils::copy_to_iterator<false, EightBitModes::AllowFull>(
				  it, dependent_member_name );
				it =
				  utils::copy_to_iterator<false, EightBitModes::AllowFull>( it, "\":" );
				if constexpr( has_switcher_v<JsonMember> ) {
					it = member_to_string( template_arg<dependent_member>, it,
					                       typename JsonMember::switcher{ }( v ) );
				} else {
					constexpr std::size_t dependent_member_pos =
					  find_names_in_pack<dependent_member>( NamePack{ } );
					it = member_to_string( template_arg<dependent_member>, it,
					                       get<dependent_member_pos>( args ) );
				}
			}

			template<std::size_t pos, typename JsonMember, typename OutputIterator,
			         typename Tuple, typename Value, typename Visited>
			inline constexpr void to_json_str( bool &is_first, OutputIterator &it,
			                                   Tuple const &tp, Value const &,
			                                   Visited &visited_members ) {
				constexpr auto json_member_name = daw::string_view(
				  std::data( JsonMember::name ), std::size( JsonMember::name ) );
				if( daw::algorithm::contains( std::data( visited_members ),
				                              daw::data_end( visited_members ),
				                              json_member_name ) ) {
					return;
				}
				visited_members.push_back( json_member_name );
				static_assert( json_details::is_a_json_type<JsonMember>::value,
				               "Unsupported data type" );
				if constexpr( json_details::is_json_nullable<JsonMember>::value and
				              JsonMember::nullable == JsonNullable::Nullable ) {
					if( not json_details::has_value( get<pos>( tp ) ) ) {
						return;
					}
				}
				if( not is_first ) {
					*it++ = ',';
				}
				is_first = false;
				*it++ = '"';
				it = utils::copy_to_iterator<false, EightBitModes::AllowFull>(
				  it, JsonMember::name );

				it =
				  utils::copy_to_iterator<false, EightBitModes::AllowFull>( it, "\":" );
				it = member_to_string( template_arg<JsonMember>, DAW_MOVE( it ),
				                       get<pos>( tp ) );
			}

			template<size_t TupleIdx, typename JsonMember, typename OutputIterator,
			         template<class...> class Tuple, typename... Args>
			constexpr void to_json_ordered_str( std::size_t &array_idx,
			                                    OutputIterator &it,
			                                    Tuple<Args...> const &tp ) {

				using json_member_type = ordered_member_subtype_t<JsonMember>;
				static_assert( is_a_json_type<json_member_type>::value,
				               "Unsupported data type" );
				// json_tagged_variant like members cannot work as we have no member
				// names to work with
				static_assert(
				  not is_a_json_tagged_variant<json_member_type>::value,
				  "JSON tagged variant types are not supported when inside an array "
				  "as an ordered structure" );

				if constexpr( is_an_ordered_member_v<JsonMember> ) {
					for( ; array_idx < JsonMember::member_index; ++array_idx ) {
						if( array_idx > 0 ) {
							*it++ = ',';
						}
						*it++ = 'n';
						*it++ = 'u';
						*it++ = 'l';
						*it++ = 'l';
					}
				}
				if( array_idx > 0 ) {
					*it++ = ',';
				}
				it = member_to_string( template_arg<json_member_type>, it,
				                       get<TupleIdx>( tp ) );
				++array_idx;
			}
		} // namespace json_details
	}   // namespace DAW_JSON_VER
} // namespace daw::json
