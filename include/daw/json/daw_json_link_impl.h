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

#include <array>
#include <chrono>
#include <cstdint>
#include <cstdlib>
#include <iterator>
#include <limits>
#include <string>
#include <variant>

#include <daw/daw_algorithm.h>
#include <daw/daw_array.h>
#include <daw/daw_bounded_string.h>
#include <daw/daw_cxmath.h>
#include <daw/daw_exception.h>
#include <daw/daw_parser_helper_sv.h>
#include <daw/daw_string_view.h>
#include <daw/daw_traits.h>
#include <daw/daw_utility.h>
#include <daw/iso8601/daw_date_formatting.h>
#include <daw/iso8601/daw_date_parsing.h>
#include <daw/iterator/daw_back_inserter.h>

namespace daw {
	namespace json {
		namespace to_strings {
			namespace {
				using std::to_string;
			}
			template<typename T>
			auto to_string( std::optional<T> const &v )
			  -> decltype( to_string( *v ) ) {
				if( !v ) {
					return {"null"};
				}
				return to_string( *v );
			}
		} // namespace to_strings

		template<typename T>
		using is_a_json_type_detect = typename T::i_am_a_json_type;

		template<typename T>
		inline constexpr bool is_a_json_type_v =
		  daw::is_detected_v<is_a_json_type_detect, T>;

		template<typename T>
		bool is_null( std::optional<T> const &v ) {
			return !static_cast<bool>( v );
		}

		template<typename T>
		bool is_null( T const & ) {
			return false;
		}

		template<typename T>
		constexpr T from_json( daw::string_view sv );

#if __cplusplus > 201703L
		// C++ 20 Non-Type Class Template Arguments
#define JSONNAMETYPE basic_bounded_string
		template<typename T>
		constexpr size_t json_name_len( T &&str ) {
			return str.extent;
		}
#else
#define JSONNAMETYPE char const *
		constexpr size_t json_name_len( char const *str ) {
			return daw::string_view( str ).size( );
		}
#endif
		namespace impl {
			namespace {
				template<typename Container, typename OutputIterator>
				constexpr OutputIterator copy_to_iterator( Container const &c,
				                                           OutputIterator it ) {
					for( auto const &value : c ) {
						*it++ = value;
					}
					return it;
				}

				template<typename JsonMember, typename OutputIterator, typename T>
				constexpr OutputIterator member_to_string( OutputIterator &&it,
				                                           T &&value ) {
					it = JsonMember::to_string( std::forward<OutputIterator>( it ),
					                            std::forward<T>( value ) );
					return std::move( it );
				}

				template<typename JsonMember, size_t pos, typename OutputIterator,
				         typename... Args>
				void make_json_string( OutputIterator it, std::tuple<Args...> &args ) {

					static_assert( is_a_json_type_v<JsonMember>,
					               "Unsupported data type" );
					*it++ = '"';
					it = copy_to_iterator( daw::string_view( JsonMember::name ), it );
					it = copy_to_iterator( daw::string_view( "\":" ), it );
					it = member_to_string<JsonMember>( std::move( it ),
					                                   std::get<pos>( args ) );
					if constexpr( pos < ( sizeof...( Args ) - 1U ) ) {
						*it++ = ',';
					}
				}

				constexpr char to_lower( char c ) noexcept {
					return static_cast<char>( static_cast<unsigned>( c ) |
					                          static_cast<unsigned>( ' ' ) );
				}

				template<typename T>
				constexpr auto deref_detect( T &&value ) noexcept
				  -> decltype( *value ) {
					return *value;
				}

				template<typename Optional>
				using deref_t = decltype(
				  deref_detect( std::declval<daw::remove_cvref_t<Optional> &>( ) ) );

				template<typename Optional>
				inline constexpr bool is_valid_optional_v =
				  daw::is_detected_v<deref_t, Optional>;

				template<typename Result>
				constexpr Result parse_signed( daw::string_view sv ) noexcept {
					if( sv.empty( ) ) {
						return static_cast<Result>( 0 );
					}
					if constexpr( std::is_signed_v<Result> ) {
						return daw::parser::parse_int<Result>( sv );
					} else {
						return daw::parser::parse_unsigned_int<Result>( sv );
					}
				}

				template<typename Result>
				constexpr Result parse_real( daw::string_view sv ) noexcept {
					// [-]WHOLE[.FRACTION][(e|E)EXPONENT]

					auto const whole_part =
					  static_cast<double>( daw::parser::parse_int<int64_t>(
					    sv.pop_front( sv.find_first_of( ".eE" ) ) ) );

					double fract_part = 0;
					if( !sv.empty( ) and sv.front( ) == '.' ) {
						sv.remove_prefix( );
						auto fract_str = sv.pop_front( sv.find_first_of( "eE" ) );
						auto const fract_exp = static_cast<int32_t>( fract_str.size( ) );
						fract_part = static_cast<double>(
						  daw::parser::parse_unsigned_int<uint64_t>( fract_str ) );
						fract_part *= daw::cxmath::dpow10( -fract_exp );
						fract_part = daw::cxmath::copy_sign( fract_part, whole_part );
					}

					int32_t exp_part = 0;
					if( !sv.empty( ) ) {
						sv.remove_prefix( );
						exp_part = daw::parser::parse_int<int32_t>( sv );
					}
					return static_cast<Result>( ( whole_part + fract_part ) *
					                            daw::cxmath::dpow10( exp_part ) );
				} // namespace

				template<typename T>
				struct nullable_type {
					using type = T;
				};

				template<template<class> class C, class... Args>
				struct nullable_type<C<Args...>> {
					using type = daw::traits::first_type<Args...>;
				};

				template<typename T>
				struct nullable_type<T const *const> {
					using type = T;
				};

				template<typename T>
				using nullable_type_t = typename nullable_type<T>::type;

				enum class JsonParseTypes : uint_fast8_t {
					Number,
					Bool,
					String,
					Date,
					Class,
					Array,
					Null,
					Custom
				};

				template<typename T>
				using json_parser_description_t =
				  decltype( describe_json_class( std::declval<T &>( ) ) );

				template<typename T>
				inline constexpr bool has_json_parser_description_v =
				  daw::is_detected_v<json_parser_description_t, T>;

				template<typename T>
				using json_parser_to_json_data_t =
				  decltype( to_json_data( std::declval<T &>( ) ) );

				template<typename T>
				inline constexpr bool has_json_to_json_data_v =
				  daw::is_detected_v<json_parser_to_json_data_t, T>;

				template<JsonParseTypes v>
				using ParseTag = std::integral_constant<JsonParseTypes, v>;

				template<typename string_t>
				struct kv_t {
					string_t name;
					JsonParseTypes expected_type;
					bool nullable;
					size_t pos;

					constexpr kv_t( string_t Name, JsonParseTypes Expected, bool Nullable,
					                size_t Pos )
					  : name( std::move( Name ) )
					  , expected_type( Expected )
					  , nullable( Nullable )
					  , pos( Pos ) {}
				};

				struct value_pos {
					bool is_nullable;
					bool empty_is_null;
					string_view value_str{};
					std::variant<std::monostate, double, uint64_t, int64_t, bool>
					  parsed_sv{};

					explicit constexpr value_pos( bool Nullable,
					                              bool empty_null ) noexcept
					  : is_nullable( Nullable )
					  , empty_is_null( empty_null ) {}

					constexpr value_pos( bool Nullable, bool empty_null,
					                     daw::string_view sv ) noexcept
					  : is_nullable( Nullable )
					  , empty_is_null( empty_null )
					  , value_str( sv ) {}

					explicit constexpr operator bool( ) const noexcept {
						return static_cast<int>( is_nullable ) or
						       static_cast<int>( !value_str.empty( ) ) or
						       static_cast<int>( empty_is_null );
					}

					constexpr bool is_parsed( ) const noexcept {
						return parsed_sv.index( ) != 0;
					}

					template<typename T>
					constexpr T parsed_as( ) const {
						switch( parsed_sv.index( ) ) {
						case 1:
							return static_cast<T>( std::get<1>( parsed_sv ) );
						case 2:
							return static_cast<T>( std::get<2>( parsed_sv ) );
						case 3:
							return static_cast<T>( std::get<3>( parsed_sv ) );
						case 4:
							return static_cast<T>( std::get<4>( parsed_sv ) );
						default:
							std::terminate( );
						}
					}
				};

				template<typename JsonType>
				using json_parse_to = typename JsonType::parse_to_t;

				template<typename JsonType>
				inline constexpr bool is_json_nullable_v = JsonType::nullable;

				template<typename JsonType>
				inline constexpr bool is_json_empty_null_v = JsonType::empty_is_null;

				constexpr daw::string_view parse_name( daw::string_view &sv ) {
					if( sv.front( ) != '"' ) {
						sv = parser::trim_left( sv );
					}
					struct member_name_parse_error {};
					daw::exception::precondition_check( sv.front( ) == '"' );
					sv.remove_prefix( );
					auto name = sv.pop_front( "\"" );
					sv = parser::trim_left( sv );
					sv.pop_front( ":" );
					sv = parser::trim_left( sv );
					return name;
				}

				constexpr daw::string_view skip_string( daw::string_view &sv ) {
					auto result = daw::string_view{};
					if( sv.empty( ) ) {
						return {};
					}
					if( sv.front( ) == '"' ) {
						sv.remove_prefix( );
					}
					auto pos = sv.find_first_of( "\"" );
					exception::precondition_check( pos != daw::string_view::npos,
					                               "Invalid class" );
					while( pos != daw::string_view::npos ) {
						if( pos == 0 or
						    ( pos != daw::string_view::npos and sv[pos - 1] != '\\' ) ) {
							result = sv.pop_front( pos );
							break;
						}
						++pos;
						pos = sv.find_first_of( "\"", pos );
					}
					exception::precondition_check( pos != daw::string_view::npos,
					                               "Invalid class" );
					pos = sv.find_first_of( ",}]\n" );
					exception::precondition_check( pos != daw::string_view::npos,
					                               "Invalid class" );
					sv.remove_prefix( pos + 1 );
					sv = parser::trim_left( sv );
					return result;
				}

				constexpr daw::string_view skip_other( daw::string_view &sv ) {
					auto pos = sv.find_first_of( ",}]\n" );
					exception::precondition_check( pos != daw::string_view::npos,
					                               "Invalid class" );
					auto result = sv.pop_front( pos );
					sv.remove_prefix( );
					sv = parser::trim_left( sv );
					return result;
				}

				template<char Left, char Right>
				constexpr daw::string_view skip_bracketed_item( daw::string_view &sv ) {

					size_t bracket_count = 1;
					bool is_escaped = false;
					bool in_quotes = false;
					auto tmp_sv = sv;
					sv.remove_prefix( );
					while( !sv.empty( ) and bracket_count > 0 ) {
						switch( sv.front( ) ) {
						case '\\':
							if( !in_quotes and !is_escaped ) {
								is_escaped = true;
								sv.remove_prefix( );
								continue;
							}
							break;
						case '"':
							if( !is_escaped ) {
								in_quotes = !in_quotes;
								sv.remove_prefix( );
								continue;
							}
							break;
						case Left:
							if( !in_quotes and !is_escaped ) {
								++bracket_count;
							}
							break;
						case Right:
							if( !in_quotes and !is_escaped ) {
								--bracket_count;
							}
						}
						is_escaped = false;
						sv.remove_prefix( );
					}
					tmp_sv = tmp_sv.pop_front( tmp_sv.size( ) - sv.size( ) );
					auto pos = sv.find_first_of( ",}]\n" );
					struct bracketed_item_parse_exception {};
					exception::precondition_check<bracketed_item_parse_exception>(
					  pos != sv.npos );
					sv.remove_prefix( );
					sv = parser::trim_left( sv );
					return tmp_sv;
				}

				constexpr daw::string_view skip_class( daw::string_view &sv ) {
					return skip_bracketed_item<'{', '}'>( sv );
				}

				constexpr daw::string_view skip_array( daw::string_view &sv ) {
					return skip_bracketed_item<'[', ']'>( sv );
				}

				struct numeric_value {};

				struct skip_value_result_t {
					daw::string_view sv{};
					std::variant<std::monostate, double, uint64_t, int64_t, bool>
					  parsed_sv{};

					bool is_null = false;

					constexpr skip_value_result_t( ) noexcept = default;

					constexpr skip_value_result_t( daw::string_view s ) noexcept
					  : sv( s ) {}

					constexpr skip_value_result_t( daw::string_view s, bool n ) noexcept
					  : sv( s )
					  , is_null( n ) {}

					template<typename T>
					constexpr skip_value_result_t( numeric_value, daw::string_view s,
					                               T value ) noexcept
					  : sv( s )
					  , parsed_sv( value ) {}

					constexpr bool is_parsed( ) const noexcept {
						return parsed_sv.index( ) != 0;
					}

					template<typename T>
					constexpr T parsed_as( ) const {
						switch( parsed_sv.index( ) ) {
						case 1:
							return static_cast<T>( std::get<1>( parsed_sv ) );
						case 2:
							return static_cast<T>( std::get<2>( parsed_sv ) );
						case 3:
							return static_cast<T>( std::get<3>( parsed_sv ) );
						case 4:
							return static_cast<T>( std::get<4>( parsed_sv ) );
						default:
							std::terminate( );
						}
					}
				};

				constexpr skip_value_result_t skip_value( daw::string_view &sv ) {
					sv = parser::trim_left( sv );
					daw::exception::precondition_check( !sv.empty( ) );
					switch( sv.front( ) ) {
					case '"':
						return skip_string( sv );
					case '[':
						return skip_array( sv );
					case '{':
						return skip_class( sv );
					default: {
						auto tmp = skip_other( sv );
						if( to_lower( tmp.front( ) ) == 't' ) {
							return {numeric_value{}, tmp, true};
						} else if( to_lower( tmp.front( ) ) == 'f' ) {
							return {numeric_value{}, tmp, false};
						} else if( to_lower( sv.front( ) ) == 'n' ) {
							return {skip_other( sv ), true};
						} else {
							if( tmp.find_first_of( ".eE" ) != daw::string_view::npos ) {
								return {numeric_value{}, tmp, parse_real<double>( tmp )};
							}
							if( tmp.front( ) == '-' ) {
								return {numeric_value{}, tmp, parse_signed<int64_t>( tmp )};
							}
							return {numeric_value{}, tmp,
							        daw::parser::parse_unsigned_int<uint64_t>( tmp )};
						}
					}
					}
				}

				template<JsonParseTypes>
				struct missing_nonnullable_value_expection {};

				template<typename ParseInfo>
				constexpr auto parse_value( ParseTag<JsonParseTypes::Number>,
				                            value_pos pos ) {
					// assert !pos.value_str.empty( );
					using constructor_t = typename ParseInfo::constructor_t;
					using element_t = nullable_type_t<typename ParseInfo::parse_to_t>;

					if( pos.is_parsed( ) ) {
						return constructor_t{}( pos.template parsed_as<element_t>( ) );
					}
					if constexpr( is_floating_point_v<element_t> ) {
						return constructor_t{}( parse_real<element_t>( pos.value_str ) );
					}
					return constructor_t{}( parse_signed<element_t>( pos.value_str ) );
				}

				template<typename ParseInfo>
				constexpr auto parse_value( ParseTag<JsonParseTypes::Custom>,
				                            value_pos pos ) {
					return typename ParseInfo::from_converter_t{}( pos.value_str );
				}

				template<typename ParseInfo>
				constexpr auto parse_value( ParseTag<JsonParseTypes::Bool>,
				                            value_pos pos ) {
					// assert !pos.value_str.empty( );
					using constructor_t = typename ParseInfo::constructor_t;
					if( pos.is_parsed( ) ) {
						return constructor_t{}( pos.template parsed_as<bool>( ) );
					}
					return constructor_t{}( to_lower( pos.value_str.front( ) ) == 't' );
				}

				template<typename ParseInfo>
				constexpr auto parse_value( ParseTag<JsonParseTypes::String>,
				                            value_pos pos ) {

					using constructor_t = typename ParseInfo::constructor_t;
					return constructor_t{}( pos.value_str.data( ),
					                        pos.value_str.size( ) );
				}

				template<typename ParseInfo>
				constexpr auto parse_value( ParseTag<JsonParseTypes::Date>,
				                            value_pos pos ) {

					using constructor_t = typename ParseInfo::constructor_t;
					return constructor_t{}( pos.value_str.data( ),
					                        pos.value_str.size( ) );
				}

				template<typename ParseInfo>
				constexpr auto parse_value( ParseTag<JsonParseTypes::Class>,
				                            value_pos pos ) {

					using element_t = nullable_type_t<typename ParseInfo::parse_to_t>;
					return from_json<element_t>( pos.value_str );
				}

				struct invalid_array {};
				template<typename ParseInfo>
				constexpr auto parse_value( ParseTag<JsonParseTypes::Array>,
				                            value_pos pos ) {
					daw::exception::precondition_check<invalid_array>(
					  pos.value_str.front( ) == '[' );
					pos.value_str.remove_prefix( );
					pos.value_str = daw::parser::trim_left( pos.value_str );
					using element_t = typename ParseInfo::json_element_t;

					auto result = typename ParseInfo::constructor_t{}( );
					auto add_value = typename ParseInfo::appender_t( result );
					while( !pos.value_str.empty( ) and pos.value_str.front( ) != ']' ) {
						auto tmp = skip_value( pos.value_str );
						auto vp = value_pos( element_t::nullable, element_t::empty_is_null,
						                     tmp.sv );
						vp.parsed_sv = tmp.parsed_sv;
						add_value( parse_value<element_t>(
						  ParseTag<element_t::expected_type>{}, vp ) );

						pos.value_str = daw::parser::trim_left( pos.value_str );
					}
					return result;
				}

				template<typename Container>
				struct basic_appender {
					daw::back_inserter_iterator<Container> appender;

					constexpr basic_appender( Container &container ) noexcept
					  : appender( container ) {}

					template<typename T>
					constexpr void operator( )( T &&value ) {
						*appender = std::forward<T>( value );
					}
				};
			} // namespace
		}   // namespace impl
	}     // namespace json
} // namespace daw
