// The MIT License (MIT)
//
// Copyright (c) 2018-2019 Darrell Wright
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

#include <chrono>
#include <cstdint>
#include <cstdlib>
#include <iterator>
#include <limits>
#include <string>

#include <daw/daw_algorithm.h>
#include <daw/daw_bounded_string.h>
#include <daw/daw_exception.h>
#include <daw/daw_parser_helper_sv.h>
#include <daw/daw_string_view.h>
#include <daw/daw_traits.h>
#include <daw/daw_utility.h>
#include <daw/iterator/daw_back_inserter.h>

namespace daw {
	namespace json {
		template<typename T>
		constexpr T from_json( daw::string_view sv );

		namespace impl {
			namespace {
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

				constexpr char to_lower( char c ) noexcept {
					return c | ' ';
				}

				enum class JsonParseTypes : uint_fast8_t {
					Number,
					Bool,
					String,
					Date,
					Class,
					Array
				};

				template<typename T>
				using json_parser_description_t =
				  decltype( describe_json_class( std::declval<T &>( ) ) );

				template<typename T>
				inline constexpr bool has_json_parser_description_v =
				  is_detected_v<json_parser_description_t, T>;

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
					string_view value_str{};

					explicit constexpr value_pos( bool Nullable ) noexcept
					  : is_nullable( Nullable ) {}

					constexpr value_pos( bool Nullable, daw::string_view sv ) noexcept
					  : is_nullable( Nullable )
					  , value_str( sv ) {}

					explicit constexpr operator bool( ) const noexcept {
						return is_nullable or !value_str.empty( );
					}
				};

				template<typename string_t>
				kv_t( string_t )->kv_t<string_t>;

				template<typename JsonType>
				using json_parse_to = typename JsonType::parse_to_t;

				template<typename JsonType>
				inline constexpr bool is_json_nullable_v = JsonType::nullable;

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
					size_t pos = 0;
					bool found = false;
					auto result = daw::string_view{};
					while( pos != daw::string_view::npos and !found ) {
						++pos;
						pos = sv.find_first_of( "\"", pos );
						if( pos != daw::string_view::npos and sv[pos - 1] != '\\' ) {
							result = sv.pop_front( pos );
							found = true;
							continue;
						}
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
					exception::precondition_check( pos != sv.npos, "Invalid class" );
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

				constexpr daw::string_view skip_value( daw::string_view &sv ) {
					sv = parser::trim_left( sv );
					daw::exception::precondition_check( !sv.empty( ) );
					switch( sv.front( ) ) {
					case '"':
						return skip_string( sv );
					case '[':
						return skip_array( sv );
					case '{':
						return skip_class( sv );
					default:
						return skip_other( sv );
					}
				}

				struct missing_nonnullable_value_expection {};

				template<typename ParseInfo>
				constexpr auto parse_value( ParseTag<JsonParseTypes::Number>,
				                            value_pos pos ) {
					using constructor_t = typename ParseInfo::constructor_t;
					using element_t = nullable_type_t<typename ParseInfo::parse_to_t>;

					if( pos.value_str.empty( ) or
					    to_lower( pos.value_str.front( ) ) == 'n' ) {
						daw::exception::precondition_check<
						  missing_nonnullable_value_expection>( ParseInfo::nullable );
						return constructor_t{}( );
					}
					if constexpr( is_floating_point_v<element_t> ) {
						return constructor_t{}( static_cast<element_t>(
						  std::strtod( pos.value_str.data( ), nullptr ) ) );
					} else {
						return constructor_t{}( static_cast<element_t>(
						  parser::parse_int<int64_t>( pos.value_str ) ) );
					}
				}

				template<typename ParseInfo>
				constexpr auto parse_value( ParseTag<JsonParseTypes::Bool>,
				                            value_pos pos ) {
					using constructor_t = typename ParseInfo::constructor_t;
					if( pos.value_str.empty( ) or
					    to_lower( pos.value_str.front( ) ) == 'n' ) {
						daw::exception::precondition_check<
						  missing_nonnullable_value_expection>( ParseInfo::nullable );
						return constructor_t{}( );
					}
					return constructor_t{}( to_lower( pos.value_str.front( ) ) == 't' );
				}

				template<typename ParseInfo>
				constexpr auto parse_value( ParseTag<JsonParseTypes::String>,
				                            value_pos pos ) {

					using constructor_t = typename ParseInfo::constructor_t;
					if( pos.value_str.empty( ) or
					    to_lower( pos.value_str.front( ) ) == 'n' ) {
						daw::exception::precondition_check<
						  missing_nonnullable_value_expection>( ParseInfo::nullable );
						return constructor_t{}( );
					}
					return constructor_t{}( pos.value_str.data( ),
					                        pos.value_str.size( ) );
				}

				template<typename ParseInfo>
				constexpr auto parse_value( ParseTag<JsonParseTypes::Date>,
				                            value_pos pos ) {

					using constructor_t = typename ParseInfo::constructor_t;
					if( pos.value_str.empty( ) or
					    to_lower( pos.value_str.front( ) ) == 'n' ) {
						daw::exception::precondition_check<
						  missing_nonnullable_value_expection>( ParseInfo::nullable );
						return constructor_t{}( );
					}
					return constructor_t{}( pos.value_str.data( ),
					                        pos.value_str.size( ) );
				}

				template<typename ParseInfo>
				constexpr auto parse_value( ParseTag<JsonParseTypes::Class>,
				                            value_pos pos ) {

					using element_t = nullable_type_t<typename ParseInfo::parse_to_t>;
					using constructor_t = typename ParseInfo::constructor_t;
					if( pos.value_str.empty( ) or
					    to_lower( pos.value_str.front( ) ) == 'n' ) {
						daw::exception::precondition_check<
						  missing_nonnullable_value_expection>( ParseInfo::nullable );
						return constructor_t{}( );
					}
					return from_json<element_t>( pos.value_str );
				}

				struct invalid_array {};
				template<typename ParseInfo>
				constexpr auto parse_value( ParseTag<JsonParseTypes::Array>,
				                            value_pos pos ) {
					using constructor_t = typename ParseInfo::constructor_t;
					if( pos.value_str.empty( ) or
					    to_lower( pos.value_str.front( ) ) == 'n' ) {
						daw::exception::precondition_check<
						  missing_nonnullable_value_expection>( ParseInfo::nullable );
						return constructor_t{}( );
					}
					daw::exception::precondition_check<invalid_array>(
					  pos.value_str.front( ) == '[' );
					pos.value_str.remove_prefix( );
					pos.value_str = daw::parser::trim_left( pos.value_str );
					using element_t = typename ParseInfo::json_element_t;

					auto result = typename ParseInfo::constructor_t{}( );
					auto add_value = typename ParseInfo::appender_t( result );
					while( !pos.value_str.empty( ) and pos.value_str.front( ) != ']' ) {
						auto val_str = skip_value( pos.value_str );
						add_value( parse_value<element_t>(
						  ParseTag<element_t::expected_type>{},
						  value_pos( element_t::nullable, val_str ) ) );

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

		template<typename... JsonMembers>
		class json_parser_t {
			static constexpr size_t find_string_capacity( ) noexcept {
				return ( JsonMembers::name.extent + ... );
			}
			using string_t = basic_bounded_string<char, find_string_capacity( )>;

			template<size_t N>
			static constexpr impl::kv_t<string_t> get_item( ) noexcept {
				using type_t = traits::nth_type<N, JsonMembers...>;
				return {type_t::name, type_t::expected_type, type_t::nullable, N};
			}

			template<size_t... Is>
			static constexpr auto make_map( std::index_sequence<Is...> ) noexcept {

				return std::array{get_item<Is>( )...};
			}

			static constexpr auto name_map =
			  make_map( std::index_sequence_for<JsonMembers...>{} );

			static constexpr bool has_name( daw::string_view key ) noexcept {
				auto result = algorithm::find_if(
				  begin( name_map ), end( name_map ),
				  [key]( auto const &kv ) { return kv.name == key; } );
				return result != std::end( name_map );
			}

			static constexpr size_t find_name( daw::string_view key ) noexcept {
				auto result = algorithm::find_if(
				  begin( name_map ), end( name_map ),
				  [key]( auto const &kv ) { return kv.name == key; } );
				if( result == std::end( name_map ) ) {
					std::terminate( );
				}
				return static_cast<size_t>(
				  std::distance( begin( name_map ), result ) );
			}

			template<size_t N>
			static constexpr decltype( auto ) parse_item(
			  std::array<impl::value_pos, sizeof...( JsonMembers )> const &locations,
			  daw::string_view sv ) {
				using type_t = traits::nth_type<N, JsonMembers...>;
				return impl::parse_value<type_t>(
				  impl::ParseTag<type_t::expected_type>{}, locations[N] );
			}

			static constexpr auto get_locations( daw::string_view &sv ) {
				std::array result = {
				  impl::value_pos( impl::is_json_nullable_v<JsonMembers> )...};

				while( !sv.empty( ) and sv.front( ) != '}' ) {
					auto name = impl::parse_name( sv );
					if( !has_name( name ) ) {
						impl::skip_value( sv );
						continue;
					}
					size_t const pos = find_name( name );
					result[pos].value_str = impl::skip_value( sv );
					sv = parser::trim_left( sv );
				}
				return result;
			}

			template<typename Result, size_t... Is>
			static constexpr Result parse_json_class( daw::string_view sv,
			                                          std::index_sequence<Is...> ) {
				static_assert(
				  can_construct_a_v<Result, typename JsonMembers::parse_to_t...>,
				  "Supplied types cannot be used for construction of this type" );

				auto sv_orig = sv;

				sv = parser::trim_left( sv );
				exception::precondition_check( !sv.empty( ) and sv.front( ) == '{' );
				sv.remove_prefix( );
				sv = parser::trim_left( sv );

				auto const locations = get_locations( sv );

				// Ensure locations of all parts there
				exception::precondition_check( algorithm::all_of(
				  begin( locations ), end( locations ), []( auto const &loc ) -> bool {
					  return static_cast<bool>( loc );
				  } ) );

				return construct_a<Result>{}( parse_item<Is>( locations, sv_orig )... );
			}

		public:
			template<typename Result>
			static constexpr decltype( auto ) parse( daw::string_view sv ) {
				return parse_json_class<Result>(
				  sv, std::index_sequence_for<JsonMembers...>{} );
			}
		};

		template<basic_bounded_string Name, typename T = double,
		         bool Nullable = false, typename Constructor = daw::construct_a<T>>
		struct json_number {
			static_assert( std::is_arithmetic_v<T> or Nullable,
			               "Number must be an arithmentic type" );
			static constexpr auto const name = Name;
			static constexpr impl::JsonParseTypes expected_type =
			  impl::JsonParseTypes::Number;
			static constexpr bool nullable = Nullable;
			using parse_to_t = T;
			using constructor_t = Constructor;
		};

		template<basic_bounded_string Name, typename T = bool,
		         bool Nullable = false, typename Constructor = daw::construct_a<T>>
		struct json_bool {
			static_assert( std::is_convertible_v<bool, T>,
			               "Supplied result type must be convertable from bool" );
			static constexpr auto const name = Name;
			static constexpr impl::JsonParseTypes expected_type =
			  impl::JsonParseTypes::Bool;
			static constexpr bool nullable = Nullable;
			using parse_to_t = T;
			using constructor_t = Constructor;
		};

		template<basic_bounded_string Name, typename T = std::string,
		         bool Nullable = false, typename Constructor = daw::construct_a<T>>
		struct json_string {
			static constexpr auto const name = Name;
			static constexpr impl::JsonParseTypes expected_type =
			  impl::JsonParseTypes::String;
			static constexpr bool nullable = Nullable;
			using parse_to_t = T;
			using constructor_t = Constructor;
		};

		template<basic_bounded_string Name,
		         typename T = std::chrono::system_clock::time_point,
		         bool Nullable = false, typename Constructor = daw::construct_a<T>>
		struct json_date {
			static constexpr auto const name = Name;
			static constexpr impl::JsonParseTypes expected_type =
			  impl::JsonParseTypes::Date;
			static constexpr bool nullable = Nullable;
			using parse_to_t = T;
			using constructor_t = Constructor;
		};

		template<basic_bounded_string Name, typename T, bool Nullable = false,
		         typename Constructor = daw::construct_a<T>>
		struct json_class {
			static constexpr auto const name = Name;
			static constexpr impl::JsonParseTypes expected_type =
			  impl::JsonParseTypes::Class;
			static constexpr bool nullable = Nullable;
			using parse_to_t = T;
			using constructor_t = Constructor;
		};

		template<basic_bounded_string Name, typename Container,
		         typename JsonElement, bool Nullable = false,
		         typename Constructor = daw::construct_a<Container>,
		         typename Appender = impl::basic_appender<Container>>
		struct json_array {
			static constexpr auto const name = Name;
			static constexpr impl::JsonParseTypes expected_type =
			  impl::JsonParseTypes::Array;
			static constexpr bool nullable = Nullable;
			using parse_to_t = Container;
			using constructor_t = Constructor;
			using appender_t = Appender;
			using json_element_t = JsonElement;
		};

		template<typename T>
		constexpr T from_json( daw::string_view json_data ) {
			static_assert(
			  impl::has_json_parser_description_v<T>,
			  "A function call describe_json_class must exist for type." );

			return impl::json_parser_description_t<T>::template parse<T>( json_data );
		}

		template<typename JsonElement>
		class json_array_iterator {
			daw::string_view m_state{};
			daw::string_view m_cur_value{};

		public:
			using value_type = typename JsonElement::parse_to_t;
			using reference = value_type;
			using pointer = value_type;
			using difference_type = ptrdiff_t;
			// Can do forward iteration and be stored
			using iterator_category = std::input_iterator_tag;

			constexpr json_array_iterator( ) noexcept = default;

			constexpr json_array_iterator( daw::string_view json_data )
			  : m_state( json_data ) {
				daw::exception::precondition_check<impl::invalid_array>(
				  m_state.front( ) == '[' );

				m_state.remove_prefix( );
				m_state = daw::parser::trim_left( m_state );

				if( m_state.empty( ) ) {
					return;
				}
				m_cur_value = impl::skip_value( m_state );
				m_state = daw::parser::trim_left( m_state );
			}

			constexpr value_type operator*( ) const {
				daw::exception::precondition_check<impl::invalid_array>(
				  !m_cur_value.empty( ) );

				return impl::parse_value<JsonElement>(
				  impl::ParseTag<JsonElement::expected_type>{},
				  impl::value_pos( false, m_cur_value ) );
			}

			constexpr json_array_iterator &operator++( ) {
				if( m_state.empty( ) or m_state.front( ) == ']' ) {
					m_cur_value = daw::string_view{};
					return *this;
				}
				m_cur_value = impl::skip_value( m_state );
				m_state = daw::parser::trim_left( m_state );
				return *this;
			}

			constexpr json_array_iterator &operator++( int ) {
				auto tmp = *this;
				++( *this );
				return tmp;
			}

			explicit constexpr operator bool( ) const noexcept {
				return !m_cur_value.empty( );
			}

			constexpr bool operator==( json_array_iterator const &rhs ) const
			  noexcept {
				return ( m_cur_value.empty( ) and !rhs ) or
				       ( m_state == rhs.m_state and m_cur_value == rhs.m_cur_value );
			}

			constexpr bool operator!=( json_array_iterator const &rhs ) const
			  noexcept {
				return !( *this == rhs );
			}
		};

		template<typename JsonElement,
		         typename Container = std::vector<typename JsonElement::parse_to_t>,
		         typename Constructor = daw::construct_a<Container>,
		         typename Appender = impl::basic_appender<Container>>
		constexpr auto from_json_array( daw::string_view json_data ) {
			using parser_t =
			  json_array<"", Container, JsonElement, false, Constructor, Appender>;

			return impl::parse_value<parser_t>(
			  impl::ParseTag<impl::JsonParseTypes::Array>{},
			  impl::value_pos( false, json_data ) );
		}

	} // namespace json
} // namespace daw
