// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/daw_json_link
//

#pragma once

#include "version.h"

#include "../../third_party/dragonbox/dragonbox.h"
#include "../daw_json_data_contract.h"
#include "daw_json_assert.h"
#include "daw_json_parse_iso8601_utils.h"
#include "daw_json_serialize_options_impl.h"
#include "daw_json_serialize_policy.h"
#include "daw_json_value.h"

#include <daw/daw_algorithm.h>
#include <daw/daw_arith_traits.h>
#include <daw/daw_cpp_feature_check.h>
#include <daw/daw_cxmath.h>
#include <daw/daw_likely.h>
#include <daw/daw_move.h>
#include <daw/daw_traits.h>
#include <daw/daw_utility.h>
#include <daw/daw_visit.h>
#include <daw/utf8/unchecked.h>

#include <array>
#include <ciso646>
#include <optional>
#include <sstream>
#include <string>
#include <tuple>
#include <type_traits>
#include <variant>

namespace daw::json {
	inline namespace DAW_JSON_VER {
		namespace json_details {
			template<options::FPOutputFormat fp_output_format, typename WriteableType,
			         typename Real>
			constexpr WriteableType to_chars( Real const &value,
			                                  WriteableType out_it );
		} // namespace json_details

		namespace json_details::to_strings {
			using std::to_string;
			// Need to use ADL to_string in unevaluated contexts.  Limiting to it's
			// own namespace
			template<typename T>
			[[nodiscard]] constexpr auto to_string( std::optional<T> const &v )
			  -> decltype( to_string( *v ) ) {
				if( not has_value( v ) ) {
					return { "null" };
				}
				return to_string( *v );
			}

			template<typename T>
			inline constexpr bool has_to_string_v =
			  requires( T && v ) { to_string( DAW_FWD( v ) ); };

			template<typename T>
			using has_to_string = std::bool_constant<has_to_string_v<T>>;
		} // namespace json_details::to_strings

		namespace json_details {
			template<typename T>
			inline constexpr bool has_from_string_v = requires(
			  daw::tag_t<T> tag, std::string_view sv ) { from_string( tag, sv ); };

			template<typename T>
			inline constexpr bool has_ostream_op_v =
			  requires( std::ostream & os, T const &value ) { os << value; };

			template<typename T>
			inline constexpr bool has_istream_op_v =
			  requires( std::istream & is, T &value ) { is >> value; };
		} // namespace json_details

		/***
		 * This is the default ToJsonConverter for json_custom. By default is will
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
				} else if constexpr( concepts::is_nullable_value_v<U> ) {
					using value_type = concepts::nullable_value_type_t<U>;
					if constexpr( json_details::is_string_view_like_v<value_type> ) {
						if constexpr( std::is_reference_v<DAW_TYPEOF(
						                concepts::nullable_value_read( value ) )> ) {
							if( concepts::nullable_value_has_value( value ) ) {
								return daw::string_view(
								  concepts::nullable_value_read( value ) );
							}
							return daw::string_view( "null" );
						} else {
							if( concepts::nullable_value_has_value( value ) ) {
								auto const &v = concepts::nullable_value_read( value );
								return std::string( std::data( v ), std::size( v ) );
							}
							return std::string( "null", 4 );
						}
					} else if constexpr( json_details::to_strings::has_to_string_v<
					                       value_type> ) {
						if( concepts::nullable_value_has_value( value ) ) {
							using json_details::to_strings::to_string;
							return to_string( concepts::nullable_value_read( value ) );
						} else {
							using result_t = DAW_TYPEOF(
							  to_string( concepts::nullable_value_read( value ) ) );
							return result_t{ "null" };
						}
					} else if constexpr( std::is_convertible_v<value_type,
					                                           std::string_view> ) {
						if( concepts::nullable_value_has_value( value ) ) {
							return static_cast<std::string_view>(
							  concepts::nullable_value_read( value ) );
						}
						return std::string_view{ "null" };
					} else if constexpr( std::is_convertible_v<value_type,
					                                           std::string> ) {
						if( concepts::nullable_value_has_value( value ) ) {
							return static_cast<std::string>(
							  concepts::nullable_value_read( value ) );
						}
						return std::string( "null" );
					} else {
						if( concepts::nullable_value_has_value( value ) ) {
							return use_stream( concepts::nullable_value_has_value( value ) );
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
			template<typename JsonMember, typename WriteableType, typename parse_to_t>
			[[nodiscard]] inline WriteableType constexpr to_daw_json_string(
			  ParseTag<JsonParseTypes::Null>, WriteableType it,
			  parse_to_t const &value );

			template<typename JsonMember, typename WriteableType,
			         json_options_t SerializationOptions, typename parse_to_t>
			[[nodiscard]] constexpr serialization_policy<WriteableType,
			                                             SerializationOptions>
			to_daw_json_string(
			  ParseTag<JsonParseTypes::Array>,
			  serialization_policy<WriteableType, SerializationOptions> it,
			  parse_to_t const &value );

			template<typename JsonMember, typename WriteableType, typename parse_to_t>
			[[nodiscard]] constexpr WriteableType
			to_daw_json_string( ParseTag<JsonParseTypes::SizedArray>,
			                    WriteableType it, parse_to_t const &value );

			template<typename JsonMember, typename WriteableType,
			         json_options_t SerializationOptions, typename parse_to_t>
			[[nodiscard]] constexpr serialization_policy<WriteableType,
			                                             SerializationOptions>
			to_daw_json_string(
			  ParseTag<JsonParseTypes::KeyValueArray>,
			  serialization_policy<WriteableType, SerializationOptions> it,
			  parse_to_t const &value );

			template<typename JsonMember, typename WriteableType,
			         json_options_t SerializationOptions, typename parse_to_t>
			[[nodiscard]] constexpr serialization_policy<WriteableType,
			                                             SerializationOptions>
			to_daw_json_string(
			  ParseTag<JsonParseTypes::KeyValue>,
			  serialization_policy<WriteableType, SerializationOptions> it,
			  parse_to_t const &value );

			template<typename JsonMember, typename WriteableType, typename parse_to_t>
			[[nodiscard]] constexpr WriteableType
			to_daw_json_string( ParseTag<JsonParseTypes::Custom>, WriteableType it,
			                    parse_to_t const &value );

			template<typename JsonMember, typename WriteableType, typename parse_to_t>
			[[nodiscard]] constexpr WriteableType
			to_daw_json_string( ParseTag<JsonParseTypes::Variant>, WriteableType it,
			                    parse_to_t const &value );

			template<typename JsonMember, typename WriteableType, typename parse_to_t>
			[[nodiscard]] constexpr WriteableType
			to_daw_json_string( ParseTag<JsonParseTypes::Class>, WriteableType it,
			                    parse_to_t const &value );

			template<typename JsonMember, typename WriteableType, typename parse_to_t>
			[[nodiscard]] constexpr WriteableType
			to_daw_json_string( ParseTag<JsonParseTypes::Real>, WriteableType it,
			                    parse_to_t const &value );

			template<typename JsonMember, typename WriteableType, typename parse_to_t>
			[[nodiscard]] constexpr WriteableType
			to_daw_json_string( ParseTag<JsonParseTypes::Signed>, WriteableType it,
			                    parse_to_t const &value );

			template<typename JsonMember, typename WriteableType, typename parse_to_t>
			[[nodiscard]] constexpr WriteableType
			to_daw_json_string( ParseTag<JsonParseTypes::Unsigned>, WriteableType it,
			                    parse_to_t const &value );

			template<typename JsonMember, typename WriteableType, typename parse_to_t>
			[[nodiscard]] constexpr WriteableType
			to_daw_json_string( ParseTag<JsonParseTypes::StringRaw>, WriteableType it,
			                    parse_to_t const &value );

			template<typename JsonMember, typename WriteableType, typename parse_to_t>
			[[nodiscard]] constexpr WriteableType
			to_daw_json_string( ParseTag<JsonParseTypes::StringEscaped>,
			                    WriteableType it, parse_to_t const &value );

			template<typename JsonMember, typename WriteableType, typename parse_to_t>
			[[nodiscard]] constexpr WriteableType
			to_daw_json_string( ParseTag<JsonParseTypes::Date>, WriteableType it,
			                    parse_to_t const &value );

			template<typename JsonMember, typename WriteableType, typename parse_to_t>
			[[nodiscard]] constexpr WriteableType
			to_daw_json_string( ParseTag<JsonParseTypes::Bool>, WriteableType it,
			                    parse_to_t const &value );

			template<typename JsonMember, typename WriteableType, typename parse_to_t>
			[[nodiscard]] inline constexpr WriteableType
			to_daw_json_string( ParseTag<JsonParseTypes::Unknown>, WriteableType it,
			                    parse_to_t const &value );

			template<typename Char>
			constexpr char to_nibble_char( Char c ) {
				auto const u = static_cast<unsigned>( static_cast<unsigned char>( c ) );
				daw_json_ensure( u < 16, ErrorReason::InvalidUTFEscape );
				if( u < 10 ) {
					return static_cast<char>( u + static_cast<unsigned char>( '0' ) );
				} else {
					return static_cast<char>( ( u - 10U ) +
					                          static_cast<unsigned char>( 'A' ) );
				}
			}

			template<typename WritableType>
			constexpr WritableType output_hex( std::uint16_t c, WritableType it ) {
				char const nibbles[] = { '\\',
				                         'u',
				                         to_nibble_char( ( c >> 12U ) & 0xFU ),
				                         to_nibble_char( ( c >> 8U ) & 0xFU ),
				                         to_nibble_char( ( c >> 4U ) & 0xFU ),
				                         to_nibble_char( c & 0xFU ) };

				it.write( nibbles );
				return it;
			}

			template<typename WritableType>
			constexpr void utf32_to_utf8( std::uint32_t cp, WritableType &it ) {
				if( cp <= 0x7FU ) {
					it.put( static_cast<char>( cp ) );
					return;
				}
				if( cp <= 0x7FFU ) {
					char const tmp[] = {
					  static_cast<char>( ( cp >> 6U ) | 0b11000000U ),
					  static_cast<char>( ( cp & 0b00111111U ) | 0b10000000U ) };
					it.write( tmp );
					return;
				}
				if( cp <= 0xFFFFU ) {
					char const tmp[]{
					  static_cast<char>( ( cp >> 12U ) | 0b11100000U ),
					  static_cast<char>( ( ( cp >> 6U ) & 0b00111111U ) | 0b10000000U ),
					  static_cast<char>( ( cp & 0b00111111U ) | 0b10000000U ) };
					it.write( tmp );
					return;
				}
				if( cp <= 0x10FFFFU ) {
					char const tmp[]{
					  static_cast<char>( ( cp >> 18U ) | 0b11110000U ),
					  static_cast<char>( ( ( cp >> 12U ) & 0b00111111U ) | 0b10000000U ),
					  static_cast<char>( ( ( cp >> 6U ) & 0b00111111U ) | 0b10000000U ),
					  static_cast<char>( ( cp & 0b00111111U ) | 0b10000000U ) };
					it.write( tmp );
					return;
				}
				daw_json_error( ErrorReason::InvalidUTFCodepoint );
			}
		} // namespace json_details

		namespace utils {
			template<
			  bool do_escape = false,
			  options::EightBitModes EightBitMode = options::EightBitModes::AllowFull,
			  typename WritableType, typename Container>
			requires( traits::is_container_like_v<daw::remove_cvref_t<Container>> )
			[[nodiscard]] constexpr WritableType
			copy_to_iterator( WritableType it, Container const &container ) {
				constexpr bool restrict_high =
				  EightBitMode != options::EightBitModes::AllowFull or
				  ( WritableType::restricted_string_output ==
				    options::RestrictedStringOutput::OnlyAllow7bitsStrings );
				if constexpr( do_escape ) {
					using iter = DAW_TYPEOF( std::begin( container ) );
					using it_t = utf8::unchecked::iterator<iter>;
					auto first = it_t( std::begin( container ) );
					auto const last = it_t( std::end( container ) );
					while( first != last ) {
						auto const cp = *first++;
						switch( cp ) {
						case '"':
							it.write( "\\\"" );
							break;
						case '\\':
							it.write( "\\\\" );
							break;
						case '\b':
							it.write( "\\b" );
							break;
						case '\f':
							it.write( "\\f" );
							break;
						case '\n':
							it.write( "\\n" );
							break;
						case '\r':
							it.write( "\\r" );
							break;
						case '\t':
							it.write( "\\t" );
							break;
						default:
							if( cp < 0x20U ) {
								it = json_details::output_hex( static_cast<std::uint16_t>( cp ),
								                               it );
								break;
							}
							if constexpr( restrict_high ) {
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
						if constexpr( restrict_high ) {
							daw_json_ensure( ( static_cast<unsigned char>( c ) >= 0x20U and
							                   static_cast<unsigned char>( c ) <= 0x7FU ),
							                 ErrorReason::InvalidStringHighASCII );
						}
						it.put( c );
					}
				}
				return it;
			}

			template<
			  bool do_escape = false,
			  options::EightBitModes EightBitMode = options::EightBitModes::AllowFull,
			  typename WriteableType>
			[[nodiscard]] constexpr WriteableType
			copy_to_iterator( WriteableType it, char const *ptr ) {
				if( ptr == nullptr ) {
					return it;
				}
				constexpr bool restrict_high =
				  EightBitMode != options::EightBitModes::AllowFull or
				  ( WriteableType::restricted_string_output ==
				    options::RestrictedStringOutput::OnlyAllow7bitsStrings );

				if constexpr( do_escape ) {
					auto chr_it = utf8::unchecked::iterator<char const *>( ptr );
					while( *chr_it.base( ) != '\0' ) {
						auto const cp = *chr_it++;
						switch( cp ) {
						case '"':
							it.write( "\\\"" );
							break;
						case '\\':
							it.write( "\\\\" );
							break;
						case '\b':
							it.write( "\\b" );
							break;
						case '\f':
							it.write( "\\f" );
							break;
						case '\n':
							it.write( "\\n" );
							break;
						case '\r':
							it.write( "\\r" );
							break;
						case '\t':
							it.write( "\\t" );
							break;
						default:
							if( cp < 0x20U ) {
								it = json_details::output_hex( static_cast<std::uint16_t>( cp ),
								                               it );
								break;
							}
							if constexpr( restrict_high ) {
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
						if constexpr( restrict_high ) {
							daw_json_ensure( ( static_cast<unsigned>( *ptr ) >= 0x20U and
							                   static_cast<unsigned>( *ptr ) <= 0x7FU ),
							                 ErrorReason::InvalidStringHighASCII );
						}
						it.put( *ptr );
						++ptr;
					}
				}
				return it;
			}

			template<
			  bool do_escape = false,
			  options::EightBitModes EightBitMode = options::EightBitModes::AllowFull,
			  typename WriteableType, json_options_t P, typename A>
			[[nodiscard]] constexpr WriteableType
			copy_to_iterator( WriteableType it, basic_json_value<P, A> const &jv ) {
				if( jv.is_null( ) ) {
					return copy_to_iterator<do_escape, EightBitMode>( it, "null" );
				} else {
					return copy_to_iterator<do_escape, EightBitMode>(
					  it, jv.get_string_view( ) );
				}
			}
		} // namespace utils

		namespace json_details {
			template<typename JsonMember, typename WriteableType, typename parse_to_t>
			[[nodiscard]] constexpr WriteableType
			to_daw_json_string( ParseTag<JsonParseTypes::Bool>, WriteableType it,
			                    parse_to_t const &value ) {

				if constexpr( JsonMember::literal_as_string ==
				              options::LiteralAsStringOpt::Always ) {
					if( value ) {
						it.write( "\"true\"" );
					} else {
						it.write( "\"false\"" );
					}
				} else {
					if( value ) {
						it.write( "true" );
					} else {
						it.write( "false" );
					}
				}
				return it;
			}

			template<std::size_t idx, typename JsonMembers, typename WriteableType,
			         typename parse_to_t>
			constexpr void to_variant_string( WriteableType &it,
			                                  parse_to_t const &value ) {
				if constexpr( idx < std::variant_size_v<parse_to_t> ) {
					if( value.index( ) != idx ) {
						to_variant_string<idx + 1, JsonMembers>( it, value );
						return;
					}
					using element_t = typename JsonMembers::json_elements;
					using JsonMember =
					  typename pack_element<idx, typename element_t::element_map_t>::type;
					it = to_daw_json_string<JsonMember>(
					  ParseTag<JsonMember::expected_type>{ }, it,
					  daw::get_nt<idx>( value ) );
				}
			}

			template<typename JsonMember, typename WriteableType, typename parse_to_t>
			[[nodiscard]] inline constexpr WriteableType
			to_daw_json_string( ParseTag<JsonParseTypes::Variant>, WriteableType it,
			                    parse_to_t const &value ) {

				assert( value.index( ) >= 0 );
				to_variant_string<0, JsonMember>( it, value );
				return it;
			}

			template<typename JsonMember, typename WriteableType, typename parse_to_t>
			[[nodiscard]] inline constexpr WriteableType
			to_daw_json_string( ParseTag<JsonParseTypes::VariantTagged>,
			                    WriteableType it, parse_to_t const &value ) {

				to_variant_string<0, JsonMember>( it, value );
				return it;
			}

			template<typename JsonMember, typename WriteableType, typename parse_to_t>
			[[nodiscard]] inline constexpr WriteableType
			to_daw_json_string( ParseTag<JsonParseTypes::VariantIntrusive>,
			                    WriteableType it, parse_to_t const &value ) {

				to_variant_string<0, JsonMember>( it, value );
				return it;
			}

			template<typename JsonMember, typename WriteableType, typename Optional>
			[[nodiscard]] inline constexpr WriteableType
			to_daw_json_string( ParseTag<JsonParseTypes::Null>, WriteableType it,
			                    Optional const &value ) {

				if constexpr( has_op_bool_v<Optional> ) {
					if( not static_cast<bool>( value ) ) {
						it.write( "null" );
						return it;
					}
				} else {
					if( not concepts::nullable_value_has_value( value ) ) {
						it.write( "null" );
						return it;
					}
				}
				using member_type = typename JsonMember::member_type;
				using tag_type = ParseTag<member_type::expected_type>;
				return to_daw_json_string<member_type>( tag_type{ }, it, [&] {
					if constexpr( concepts::is_nullable_value_v<Optional> ) {
						return concepts::nullable_value_traits<Optional>::read( value );
					} else if constexpr( json_details::has_op_star_v<Optional> ) {
						return *value;
					} else {
						return value;
					}
				}( ) );
			}

			template<typename JsonMember, typename WriteableType, typename parse_to_t>
			[[nodiscard]] constexpr WriteableType
			to_daw_json_string( ParseTag<JsonParseTypes::Real>, WriteableType it,
			                    parse_to_t const &value ) {

				static_assert(
				  std::is_convertible_v<parse_to_t, typename JsonMember::parse_to_t>,
				  "value must be convertible to specified type in class contract" );

				if constexpr( std::is_floating_point_v<
				                typename JsonMember::parse_to_t> ) {
					if( daw::cxmath::is_nan( value ) ) {
						if constexpr( JsonMember::literal_as_string ==
						                options::LiteralAsStringOpt::Never or
						              JsonMember::allow_number_errors ==
						                options::JsonNumberErrors::None or
						              JsonMember::allow_number_errors ==
						                options::JsonNumberErrors::AllowInf ) {
							daw_json_error( ErrorReason::NumberIsNaN );
						} else {
							it.write( "\"NaN\"" );
							return it;
						}
					} else if( daw::cxmath::is_inf( value ) ) {
						if constexpr( JsonMember::literal_as_string ==
						                options::LiteralAsStringOpt::Never or
						              JsonMember::allow_number_errors ==
						                options::JsonNumberErrors::None or
						              JsonMember::allow_number_errors ==
						                options::JsonNumberErrors::AllowNaN ) {
							daw_json_error( ErrorReason::NumberIsInf );
						} else {
							if( value < 0 ) {
								it.write( "\"-Infinity\"" );
							} else {
								it.write( "\"Infinity\"" );
							}
							return it;
						}
					}
				}

				if constexpr( JsonMember::literal_as_string ==
				              options::LiteralAsStringOpt::Always ) {
					it.put( '"' );
				}
				if constexpr( daw::is_floating_point_v<parse_to_t> ) {
					static_assert( sizeof( parse_to_t ) <= sizeof( double ) );
					it = to_chars<JsonMember::fp_output_format>( value, it );
				} else {
					using std::to_string;
					using to_strings::to_string;
					it = utils::copy_to_iterator( it, to_string( value ) );
				}
				if constexpr( JsonMember::literal_as_string ==
				              options::LiteralAsStringOpt::Always ) {
					it.put( '"' );
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

			template<typename JsonMember, typename WriteableType, typename parse_to_t>
			[[nodiscard]] constexpr WriteableType
			to_daw_json_string( ParseTag<JsonParseTypes::Signed>, WriteableType it,
			                    parse_to_t const &value ) {

				static_assert(
				  std::is_convertible_v<parse_to_t, typename JsonMember::base_type>,
				  "value must be convertible to specified type in class contract" );

				using std::to_string;
				using to_strings::to_string;
				using under_type = base_int_type_t<parse_to_t>;

				if constexpr( std::disjunction_v<std::is_enum<parse_to_t>,
				                                 daw::is_integral<parse_to_t>> ) {
					auto v = static_cast<under_type>( value );

					char buff[daw::numeric_limits<under_type>::digits10 + 10]{ };
					char *num_start = buff;
					char *ptr = buff;
					if constexpr( JsonMember::literal_as_string ==
					              options::LiteralAsStringOpt::Always ) {
						*ptr++ = '"';
						++num_start;
					}
					if( v < 0 ) {
						it.put( '-' );
						// Do 1 round here just in case we are
						// daw::numeric_limits<intmax_t>::min( ) and cannot negate
						// This is a subtraction because v < 0 % 10 is negative
						*ptr++ = static_cast<char>( '0' - static_cast<char>( v % 10 ) );
						v /= -10;
						if( v == 0 ) {
							if constexpr( JsonMember::literal_as_string ==
							              options::LiteralAsStringOpt::Always ) {
								*ptr++ = '"';
							}
							it.copy_buffer( buff, ptr );
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
					daw::algorithm::reverse( num_start, ptr );
					if constexpr( JsonMember::literal_as_string ==
					              options::LiteralAsStringOpt::Always ) {
						*ptr++ = '"';
					}
					it.copy_buffer( buff, ptr );
					return it;
				} else {
					if constexpr( JsonMember::literal_as_string ==
					              options::LiteralAsStringOpt::Always ) {
						it.put( '"' );
					}
					// Fallback to ADL
					it = utils::copy_to_iterator( it, to_string( value ) );
					if constexpr( JsonMember::literal_as_string ==
					              options::LiteralAsStringOpt::Always ) {
						it.put( '"' );
					}
					return it;
				}
			}

			template<typename JsonMember, typename WriteableType, typename parse_to_t>
			[[nodiscard]] constexpr WriteableType
			to_daw_json_string( ParseTag<JsonParseTypes::Unsigned>, WriteableType it,
			                    parse_to_t const &value ) {

				static_assert(
				  std::is_convertible_v<parse_to_t, typename JsonMember::parse_to_t>,
				  "value must be convertible to specified type in class contract" );

				using std::to_string;
				using to_strings::to_string;
				using under_type = base_int_type_t<parse_to_t>;

				if constexpr( JsonMember::literal_as_string ==
				              options::LiteralAsStringOpt::Always ) {
					it.put( '"' );
				}
				if constexpr( std::disjunction_v<std::is_enum<parse_to_t>,
				                                 daw::is_integral<parse_to_t>> ) {
					auto v = static_cast<under_type>( value );

					if( DAW_UNLIKELY( v == 0 ) ) {
						it.put( '0' );
					} else {
						daw_json_ensure( v > 0, ErrorReason::NumberOutOfRange );
						char buff[daw::numeric_limits<under_type>::digits10 + 10]{ };
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
						daw::algorithm::reverse( buff, ptr );
						it.copy_buffer( buff, ptr );
					}
				} else {
					// Fallback to ADL
					it = utils::copy_to_iterator( it, to_string( value ) );
				}
				if constexpr( JsonMember::literal_as_string ==
				              options::LiteralAsStringOpt::Always ) {
					it.put( '"' );
				}
				return it;
			}
		} // namespace json_details

		namespace utils {
			namespace utils_details {
				template<typename Integer>
				struct number {
					using parse_to_t = Integer;
					using base_type = Integer;
					static constexpr options::LiteralAsStringOpt literal_as_string =
					  options::LiteralAsStringOpt::Never;
				};
			} // namespace utils_details

			template<typename Integer, typename WriteableType>
			inline constexpr WriteableType integer_to_string( WriteableType it,
			                                                  Integer const &value ) {
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
			template<typename JsonMember, typename WriteableType, typename parse_to_t>
			[[nodiscard]] inline constexpr WriteableType
			to_daw_json_string( ParseTag<JsonParseTypes::StringRaw>, WriteableType it,
			                    parse_to_t const &value ) {

				static_assert(
				  std::is_convertible_v<parse_to_t, typename JsonMember::parse_to_t>,
				  "Value must be convertible to specialized type in "
				  "json_data_contract" );

				constexpr options::EightBitModes eight_bit_mode =
				  JsonMember::eight_bit_mode;
				it.put( '"' );
				if( std::size( value ) > 0U ) {
					it = utils::copy_to_iterator<false, eight_bit_mode>( it, value );
				}
				it.put( '"' );
				return it;
			}

			template<typename JsonMember, typename WriteableType, typename parse_to_t>
			[[nodiscard]] inline constexpr WriteableType
			to_daw_json_string( ParseTag<JsonParseTypes::StringEscaped>,
			                    WriteableType it, parse_to_t const &value ) {

				constexpr options::EightBitModes eight_bit_mode =
				  JsonMember::eight_bit_mode;
				it.put( '"' );
				it = utils::copy_to_iterator<true, eight_bit_mode>( it, value );
				it.put( '"' );
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

			template<typename JsonMember, typename WriteableType, typename parse_to_t>
			[[nodiscard]] constexpr WriteableType
			to_daw_json_string( ParseTag<JsonParseTypes::Date>, WriteableType it,
			                    parse_to_t const &value ) {

				static_assert(
				  std::is_convertible_v<parse_to_t, typename JsonMember::parse_to_t>,
				  "value must be convertible to specified type in class contract" );

				using json_details::is_null;
				// TODO: document customization point
				if( is_null( value ) ) {
					it.write( "null" );
					return it;
				}
				it.put( '"' );
				datetime::ymdhms const civil = datetime::time_point_to_civil( value );
				it = utils::integer_to_string( it, civil.year );
				it.put( '-' );
				if( civil.month < 10 ) {
					it.put( '0' );
				}
				it = utils::integer_to_string( it, civil.month );
				it.put( '-' );
				if( civil.day < 10 ) {
					it.put( '0' );
				}
				it = utils::integer_to_string( it, civil.day );
				it.put( 'T' );
				if( civil.hour < 10 ) {
					it.put( '0' );
				}
				it = utils::integer_to_string( it, civil.hour );
				it.put( ':' );
				if( civil.minute < 10 ) {
					it.put( '0' );
				}
				it = utils::integer_to_string( it, civil.minute );
				it.put( ':' );
				if( civil.second < 10 ) {
					it.put( '0' );
				}
				it = utils::integer_to_string( it, civil.second );
				if( civil.millisecond > 0 ) {
					it.put( '.' );
					it = utils::integer_to_string( it, civil.millisecond );
				}
				it.write( "Z\"" );
				return it;
			}

			template<typename JsonMember, typename WriteableType, typename parse_to_t>
			[[nodiscard]] inline constexpr WriteableType
			to_daw_json_string( ParseTag<JsonParseTypes::Unknown>, WriteableType it,
			                    parse_to_t const &value ) {

				return utils::copy_to_iterator( it, value );
			}

			template<typename JsonMember, typename WriteableType, typename parse_to_t>
			[[nodiscard]] inline constexpr WriteableType
			to_daw_json_string( ParseTag<JsonParseTypes::Class>, WriteableType it,
			                    parse_to_t const &value ) {

				static_assert(
				  std::is_convertible_v<parse_to_t, typename JsonMember::parse_to_t>,
				  "value must be convertible to specified type in class contract" );

				if constexpr( has_json_to_json_data_v<parse_to_t> ) {
					return json_data_contract_trait_t<typename JsonMember::wrapped_type>::
					  serialize(
					    it,
					    json_data_contract<
					      typename JsonMember::wrapped_type>::to_json_data( value ),
					    value );
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

			template<typename JsonMember, typename WriteableType, typename parse_to_t>
			[[nodiscard]] inline constexpr WriteableType
			to_daw_json_string( ParseTag<JsonParseTypes::Custom>, WriteableType it,
			                    parse_to_t const &value ) {

				static_assert(
				  std::is_convertible_v<parse_to_t, typename JsonMember::parse_to_t>,
				  "value must be convertible to specified type in class contract" );

				if constexpr( JsonMember::custom_json_type !=
				              options::JsonCustomTypes::Literal ) {
					it.put( '"' );
					if constexpr( std::is_invocable_r_v<
					                WriteableType, typename JsonMember::to_converter_t,
					                WriteableType, parse_to_t> ) {

						it = typename JsonMember::to_converter_t{ }( it, value );
					} else {
						it = utils::copy_to_iterator(
						  it, typename JsonMember::to_converter_t{ }( value ) );
					}
					it.put( '"' );
					return it;
				} else {
					return utils::copy_to_iterator(
					  it, typename JsonMember::to_converter_t{ }( value ) );
				}
			}

			template<typename JsonMember, typename WriteableType,
			         json_options_t SerializationOptions, typename parse_to_t,
			         std::size_t... Is>
			serialization_policy<WriteableType, SerializationOptions>
			to_daw_json_string_tuple(
			  serialization_policy<WriteableType, SerializationOptions> it,
			  parse_to_t const &value, std::index_sequence<Is...> ) {

				auto const to_daw_json_string_help = [&]( auto Idx ) {
					using index = daw::remove_cvref_t<decltype( Idx )>;
					using pack_element = tuple_elements_pack<parse_to_t>;
					using T = std::tuple_element_t<index::value,
					                               typename JsonMember::sub_member_list>;

					it = to_daw_json_string<T>(
					  ParseTag<T::expected_type>{ }, it,
					  pack_element::template get<index::value>( value ) );
					if constexpr( index::value + 1 < sizeof...( Is ) ) {
						it.put( ',' );
						it.next_member( );
					}
				};
				(void)to_daw_json_string_help;

				daw::Empty const expander[]{
				  ( to_daw_json_string_help( daw::constant<Is>{ } ), daw::Empty{ } )...,
				  daw::Empty{} };
				(void)expander;

				return it;
			}

			template<typename JsonMember, typename WriteableType,
			         json_options_t SerializationOptions, typename parse_to_t>
			[[nodiscard]] constexpr serialization_policy<WriteableType,
			                                             SerializationOptions>
			to_daw_json_string(
			  ParseTag<JsonParseTypes::Tuple>,
			  serialization_policy<WriteableType, SerializationOptions> it,
			  parse_to_t const &value ) {

				using tuple_t = typename JsonMember::parse_to_t;
				using element_pack = tuple_elements_pack<tuple_t>;

				static_assert( is_tuple_v<tuple_t>, "Expected tuple like type" );
				static_assert(
				  std::is_convertible_v<parse_to_t, tuple_t>,
				  "value must be convertible to specified type in class contract" );

				it.put( '[' );
				it.add_indent( );
				it.next_member( );
				it = to_daw_json_string_tuple<JsonMember>(
				  it, value, std::make_index_sequence<element_pack::size>{ } );
				it.del_indent( );
				if constexpr( element_pack::size > 0 ) {
					if constexpr( element_pack::size > 0 and
					              it.output_trailing_comma ==
					                options::OutputTrailingComma::Yes ) {
						it.put( ',' );
					}
					it.next_member( );
				}
				it.put( ']' );
				return it;
			}

			template<typename T>
			inline constexpr bool is_view_like_v = requires( T & value ) {
				                                       std::begin( value );
				                                       std::end( value );
				                                       typename T::value_type;
			                                       };

			template<typename JsonMember, typename WriteableType,
			         json_options_t SerializationOptions, typename parse_to_t>
			[[nodiscard]] constexpr serialization_policy<WriteableType,
			                                             SerializationOptions>
			to_daw_json_string(
			  ParseTag<JsonParseTypes::Array>,
			  serialization_policy<WriteableType, SerializationOptions> it,
			  parse_to_t const &value ) {

				using array_t = typename JsonMember::parse_to_t;
				if constexpr( is_view_like_v<array_t> ) {
					static_assert(
					  std::is_convertible_v<parse_to_t, array_t>,
					  "value must be convertible to specified type in class contract" );
				} else {
					static_assert(
					  is_pointer_like_v<array_t>,
					  "This is a special case for pointer like(T*, unique_ptr<T>, "
					  "shared_ptr<T>) arrays.  In the to_json_data it is required to "
					  "encode the size of the data with the pointer.  Will take any "
					  "Container like type, but std::span like types work too" );
					static_assert(
					  is_view_like_v<parse_to_t>,
					  "This is a special case for pointer like(T*, unique_ptr<T>, "
					  "shared_ptr<T>) arrays.  In the to_json_data it is required to "
					  "encode the size of the data with the pointer.  Will take any "
					  "Container like type, but std::span like types work too" );
				}

				it.put( '[' );
				it.add_indent( );
				auto first = std::begin( value );
				auto last = std::end( value );
				bool const has_elements = first != last;
				while( first != last ) {
					it.next_member( );
					it = to_daw_json_string<typename JsonMember::json_element_t>(
					  ParseTag<JsonMember::json_element_t::expected_type>{ }, it,
					  *first );
					++first;
					if( first != last ) {
						it.put( ',' );
					}
				}
				it.del_indent( );
				if constexpr( it.output_trailing_comma ==
				              options::OutputTrailingComma::Yes ) {
					if( has_elements ) {
						it.put( ',' );
					}
				}
				if( has_elements ) {
					it.next_member( );
				}
				it.put( ']' );
				return it;
			}

			template<typename JsonMember, typename WriteableType, typename parse_to_t>
			[[nodiscard]] constexpr WriteableType
			to_daw_json_string( ParseTag<JsonParseTypes::SizedArray>,
			                    WriteableType it, parse_to_t const &value ) {
				return to_daw_json_string<JsonMember>(
				  ParseTag<JsonParseTypes::Array>{ }, it, value );
			}

			template<typename Key, typename Value>
			inline constexpr Key const &
			json_get_key( std::pair<Key, Value> const &kv ) {
				return kv.first;
			}

			template<typename Key, typename Value>
			inline constexpr Value const &
			json_get_value( std::pair<Key, Value> const &kv ) {
				return kv.second;
			}

			template<typename JsonMember, typename WriteableType,
			         json_options_t SerializeOptions, typename parse_to_t>
			[[nodiscard]] constexpr serialization_policy<WriteableType,
			                                             SerializeOptions>
			to_daw_json_string(
			  ParseTag<JsonParseTypes::KeyValueArray>,
			  serialization_policy<WriteableType, SerializeOptions> it,
			  parse_to_t const &value ) {

				static_assert(
				  std::is_convertible_v<parse_to_t, typename JsonMember::parse_to_t>,
				  "value must be convertible to specified type in class contract" );
				using key_t = typename JsonMember::json_key_t;
				using value_t = typename JsonMember::json_value_t;
				it.put( '[' );
				it.add_indent( );
				auto first = std::begin( value );
				auto last = std::end( value );
				bool const has_elements = first != last;
				while( first != last ) {
					it.next_member( );
					it.put( '{' );
					it.add_indent( );
					it.next_member( );
					// Append Key Name
					it.write( "\"", key_t::name, "\":", it.space );
					// Append Key Value
					it = to_daw_json_string<key_t>( ParseTag<key_t::expected_type>{ }, it,
					                                json_get_key( *first ) );

					it.put( ',' );
					// Append Value Name
					it.next_member( );
					it.write( "\"", value_t::name, "\":", it.space );
					// Append Value Value
					it = to_daw_json_string<value_t>( ParseTag<value_t::expected_type>{ },
					                                  it, json_get_value( *first ) );

					it.del_indent( );
					if constexpr( it.output_trailing_comma ==
					              options::OutputTrailingComma::Yes ) {
						if( has_elements ) {
							it.put( ',' );
						}
					}
					it.next_member( );
					it.put( '}' );
					++first;
					if( first != last ) {
						it.put( ',' );
					}
				}
				it.del_indent( );
				if constexpr( it.output_trailing_comma ==
				              options::OutputTrailingComma::Yes ) {
					if( has_elements ) {
						it.put( ',' );
					}
				}
				if( has_elements ) {
					it.next_member( );
				}
				it.put( ']' );
				return it;
			}

			template<typename JsonMember, typename WriteableType,
			         json_options_t SerializationOptions, typename parse_to_t>
			[[nodiscard]] constexpr serialization_policy<WriteableType,
			                                             SerializationOptions>
			to_daw_json_string(
			  ParseTag<JsonParseTypes::KeyValue>,
			  serialization_policy<WriteableType, SerializationOptions> it,
			  parse_to_t const &value ) {

				it.put( '{' );
				it.add_indent( );
				auto first = std::begin( value );
				auto last = std::end( value );
				bool const has_elements = first != last;
				while( first != last ) {
					auto const &v = *first;
					it.next_member( );
					it = to_daw_json_string<typename JsonMember::json_key_t>(
					  ParseTag<JsonMember::json_key_t::expected_type>{ }, it,
					  json_get_key( v ) );
					it.write( ":", it.space );
					it = to_daw_json_string<typename JsonMember::json_element_t>(
					  ParseTag<JsonMember::json_element_t::expected_type>{ }, it,
					  json_get_value( v ) );
					++first;
					if( first != last ) {
						it.put( ',' );
					}
				}
				it.del_indent( );
				if constexpr( it.output_trailing_comma ==
				              options::OutputTrailingComma::Yes ) {
					if( has_elements ) {
						it.put( ',' );
					}
				}
				if( has_elements ) {
					it.next_member( );
				}
				it.put( '}' );
				return it;
			}

			template<typename JsonMember, typename WriteableType, typename T>
			[[nodiscard]] inline constexpr WriteableType
			member_to_string( template_param<JsonMember>, WriteableType it,
			                  T const &value ) {
				return to_daw_json_string<JsonMember>(
				  ParseTag<JsonMember::expected_type>{ }, DAW_MOVE( it ), value );
			}

			template<typename>
			struct missing_required_mapping_for {};

			// This is only ever called in a constant expression. But will not
			// compile if exceptions are disabled, and it tries to throw
			template<typename Name>
			DAW_ATTRIB_NOINLINE static void missing_required_mapping_error( ) {
#ifdef DAW_USE_EXCEPTIONS
				throw missing_required_mapping_for<Name>{ };
#else
				std::terminate( );
#endif
			}

			template<typename, typename...>
			struct find_names_in_pack;

			template<typename Needle, typename... Haystack>
			struct find_names_in_pack<Needle, daw::fwd_pack<Haystack...>> {

				static DAW_CONSTEVAL std::size_t find_position( ) {
					static_assert( ( ( Haystack::name == Needle::name ) or ... ),
					               "Name must exist" );
					constexpr std::array const names = { Haystack::name... };
					std::size_t n = 0;
					for( ; n < sizeof...( Haystack ); ++n ) {
						if( Needle::name == names[n] ) {
							return n;
						}
					}
					if( n >= sizeof...( Haystack ) ) {
						missing_required_mapping_error<Needle>( );
					}
					DAW_UNREACHABLE( );
				}

				static constexpr std::size_t value = find_position( );
			};

			template<typename Needle, typename... Haystack>
			inline static constexpr std::size_t find_names_in_pack_v =
			  find_names_in_pack<Needle, Haystack...>::value;

			template<std::size_t, typename JsonMember, typename /*NamePack*/,
			         typename WriteableType, typename TpArgs, typename Value,
			         typename VisitedMembers>
			requires( not has_dependent_member_v<JsonMember> )
			DAW_ATTRIB_INLINE constexpr void
			dependent_member_to_json_str( bool &, WriteableType const &,
			                              TpArgs const &, Value const &,
			                              VisitedMembers const & ) noexcept {

				// This is empty so that the call is able to be put into a pack
			}

			template<std::size_t pos, typename JsonMember, typename NamePack,
			         typename WriteableType, json_options_t SerializationOptions,
			         typename TpArgs, typename Value, typename VisitedMembers>
			requires( has_dependent_member_v<JsonMember> )
			constexpr void dependent_member_to_json_str(
			  bool &is_first,
			  serialization_policy<WriteableType, SerializationOptions> it,
			  TpArgs const &args, Value const &v, VisitedMembers &visited_members ) {
				using base_member_t = typename std::conditional_t<
				  is_json_nullable_v<JsonMember>,
				  ident_trait<json_nullable_member_type_t, JsonMember>,
				  traits::identity<JsonMember>>::type;

				using dependent_member = dependent_member_t<base_member_t>;

				using daw::get;
				using std::get;
				static_assert( is_a_json_type_v<JsonMember>, "Unsupported data type" );
				if constexpr( is_json_nullable_v<JsonMember> ) {
					if constexpr( JsonMember::nullable == JsonNullable::Nullable ) {
						// We have no requirement to output this member when it's null
						if( not get<pos>( args ) ) {
							return;
						}
					}
				}
				if( daw::algorithm::contains( std::data( visited_members ),
				                              daw::data_end( visited_members ),
				                              dependent_member::name ) ) {
					// Already outputted this member
					return;
				}
				visited_members.push_back( dependent_member::name );
				if( not is_first ) {
					it.put( ',' );
				}
				it.next_member( );
				is_first = false;
				it.put( '"' );
				it = utils::copy_to_iterator<false, options::EightBitModes::AllowFull>(
				  it, dependent_member::name );
				it.write( "\":", it.space );

				if constexpr( has_switcher_v<base_member_t> ) {
					it = member_to_string( template_arg<dependent_member>, it,
					                       typename base_member_t::switcher{ }( v ) );
				} else {
					constexpr auto idx = find_names_in_pack_v<dependent_member, NamePack>;
					it = member_to_string( template_arg<dependent_member>, it,
					                       get<idx>( args ) );
				}
				(void)it;
			}

			template<std::size_t pos, typename JsonMember, typename WriteableType,
			         json_options_t SerializationOptions, typename Tuple,
			         typename Value, typename Visited>
			inline constexpr void to_json_str(
			  bool &is_first,
			  serialization_policy<WriteableType, SerializationOptions> &it,
			  Tuple const &tp, Value const &, Visited &visited_members ) {
				constexpr auto json_member_name = daw::string_view(
				  std::data( JsonMember::name ), std::size( JsonMember::name ) );
				if( daw::algorithm::contains( std::data( visited_members ),
				                              daw::data_end( visited_members ),
				                              json_member_name ) ) {
					return;
				}
				visited_members.push_back( json_member_name );
				static_assert( is_a_json_type_v<JsonMember>, "Unsupported data type" );
				if constexpr( is_json_nullable_v<JsonMember> ) {
					if( not concepts::nullable_value_has_value( get<pos>( tp ) ) ) {
						return;
					}
				}
				if( not is_first ) {
					it.put( ',' );
				}
				it.next_member( );
				is_first = false;
				it.put( '"' );
				it = utils::copy_to_iterator<false, options::EightBitModes::AllowFull>(
				  it, JsonMember::name );
				it.write( "\":", it.space );
				it = member_to_string( template_arg<JsonMember>, DAW_MOVE( it ),
				                       get<pos>( tp ) );
			}

			template<size_t TupleIdx, typename JsonMember, typename WriteableType,
			         json_options_t SerializerOptions, template<class...> class Tuple,
			         typename... Args>
			constexpr void to_json_ordered_str(
			  std::size_t &array_idx, std::size_t array_size,
			  serialization_policy<WriteableType, SerializerOptions> &it,
			  Tuple<Args...> const &tp ) {

				using json_member_type = ordered_member_subtype_t<JsonMember>;
				static_assert( is_a_json_type_v<json_member_type>,
				               "Unsupported data type" );
				// json_tagged_variant like members cannot work as we have no member
				// names to work with
				static_assert(
				  not is_a_json_tagged_variant_v<json_member_type>,
				  "JSON tagged variant types are not supported when inside an array "
				  "as an ordered structure" );

				if constexpr( is_an_ordered_member_v<JsonMember> ) {
					for( ; array_idx < JsonMember::member_index; ++array_idx ) {
						it.next_member( );
						it.write( "null," );
						it.next_member( );
					}
				}
				it = member_to_string( template_arg<json_member_type>, it,
				                       get<TupleIdx>( tp ) );
				++array_idx;
				if( array_idx < array_size ) {
					it.put( ',' );
					it.next_member( );
				}
			}

			template<options::FPOutputFormat fp_output_fmt, typename WriteableType,
			         typename Real>
			constexpr WriteableType to_chars( Real const &value,
			                                  WriteableType out_it ) {
				daw::jkj::dragonbox::unsigned_fp_t<Real> dec =
				  daw::jkj::dragonbox::to_decimal(
				    value, daw::jkj::dragonbox::policy::sign::ignore );

				auto const digits =
				  daw::jkj::dragonbox::to_chars_detail::decimal_length(
				    dec.significand );

				auto whole_dig = static_cast<std::int32_t>( digits ) + dec.exponent;

				auto const br = [&] {
					if constexpr( std::is_same_v<Real, float> ) {
						return daw::jkj::dragonbox::ieee754_bits( value );
					} else {
						return daw::jkj::dragonbox::ieee754_bits(
						  static_cast<double>( value ) );
					}
				}( );
				if( dec.significand == 0 ) {
					if( br.is_negative( ) ) {
						out_it.put( '-' );
					}
					out_it.put( '0' );
					return out_it;
				}
				if( fp_output_fmt == options::FPOutputFormat::Scientific ) {
					char buff[50]{ };
					char *ptr = buff;
					ptr =
					  daw::jkj::dragonbox::to_chars_detail::to_chars( dec, ptr, digits );
					out_it.copy_buffer( buff, ptr );
					return out_it;
				} else if( fp_output_fmt == options::FPOutputFormat::Auto ) {
					if( ( whole_dig < -4 ) | ( whole_dig > 6 ) ) {
						char buff[50]{ };
						char *ptr = buff;
						ptr = daw::jkj::dragonbox::to_chars_detail::to_chars( dec, ptr,
						                                                      digits );
						out_it.copy_buffer( buff, ptr );
						return out_it;
					}
				}
				if( br.is_negative( ) ) {
					out_it.put( '-' );
				}
				if( dec.significand == 0 ) {
					out_it.put( '0' );
					return out_it;
				}
				if( dec.exponent < 0 ) {
					if( whole_dig < 0 ) {
						out_it.write( "0." );
						do {
							out_it.put( '0' );
							++whole_dig;
						} while( whole_dig < 0 );
						out_it = utils::integer_to_string( out_it, dec.significand );
						return out_it;
					}
					// TODO allow for decimal output for all
					auto const p1pow =
					  daw::cxmath::pow10( static_cast<std::size_t>( -dec.exponent ) );
					auto const p1val = dec.significand / p1pow;
					out_it = utils::integer_to_string( out_it, p1val );
					if( p1pow == 1 ) {
						return out_it;
					}
					out_it.put( '.' );
					auto const p2val = dec.significand - ( p1val * p1pow );
					out_it = utils::integer_to_string( out_it, p2val );
					return out_it;
				}
				out_it = utils::integer_to_string( out_it, dec.significand );
				while( dec.exponent > 0 ) {
					out_it.put( '0' );
					--dec.exponent;
				}
				return out_it;
			}
		} // namespace json_details
	}   // namespace DAW_JSON_VER
} // namespace daw::json
