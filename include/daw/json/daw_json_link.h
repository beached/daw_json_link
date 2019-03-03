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

#include <array>
#include <chrono>
#include <cstdint>
#include <cstdlib>
#include <iterator>
#include <limits>
#include <string>

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

#include "daw_json_link_impl.h"

namespace daw {
	namespace json {
		template<size_t N, typename string_t, typename... JsonMembers>
		static constexpr impl::kv_t<string_t> get_item( ) noexcept {
			using type_t = traits::nth_type<N, JsonMembers...>;
			return {type_t::name, type_t::expected_type, type_t::nullable, N};
		}

		template<typename... JsonMembers>
		class class_description_t {
			static constexpr size_t find_string_capacity( ) noexcept {
				return ( json_name_len( JsonMembers::name ) + ... );
			}
			using string_t = basic_bounded_string<char, find_string_capacity( )>;

			template<size_t... Is>
			static constexpr auto make_map( std::index_sequence<Is...> ) noexcept {
				return daw::make_array( get_item<Is, string_t, JsonMembers...>( )... );
			}

			static constexpr auto name_map =
			  make_map( std::index_sequence_for<JsonMembers...>{} );

			static constexpr bool has_name( daw::string_view key ) noexcept {
				using std::begin;
				using std::end;
				auto result = algorithm::find_if(
				  begin( name_map ), end( name_map ),
				  [key]( auto const &kv ) { return kv.name == key; } );
				return result != std::end( name_map );
			}

			static constexpr size_t find_name( daw::string_view key ) noexcept {
				using std::begin;
				using std::end;
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
			static constexpr decltype( auto )
			parse_item( std::array<impl::value_pos, sizeof...( JsonMembers )> const
			              &locations ) {

				using type_t = traits::nth_type<N, JsonMembers...>;

				return impl::parse_value<type_t>(
				  impl::ParseTag<type_t::expected_type>{}, locations[N] );
			}

			static constexpr auto get_locations( daw::string_view &sv ) {
				auto result = daw::make_array(
				  impl::value_pos( impl::is_json_nullable_v<JsonMembers>,
				                   impl::is_json_empty_null_v<JsonMembers> )... );

				while( !sv.empty( ) and sv.front( ) != '}' ) {
					auto name = impl::parse_name( sv );
					if( !has_name( name ) ) {
						impl::skip_value( sv );
						continue;
					}
					size_t const pos = find_name( name );
					auto v = impl::skip_value( sv );
					if( v.is_null or ( v.sv.empty( ) and !result[pos].empty_is_null ) ) {
						throw impl::missing_nonnullable_value_expection<
						  impl::JsonParseTypes::Null>{};
					}

					result[pos].value_str = v.sv;
					result[pos].parsed_sv = std::move( v.parsed_sv );
					sv = parser::trim_left( sv );
				}

				daw::exception::precondition_check(
				  ::daw::algorithm::all_of( begin( result ), end( result ),
				                            []( auto const &loc ) -> bool {
					                            return static_cast<bool>( loc );
				                            } ),
				  "Missing mandatory field" );
				return result;
			}

			template<typename Result, size_t... Is>
			static constexpr Result parse_json_class( daw::string_view sv,
			                                          std::index_sequence<Is...> ) {
				static_assert(
				  can_construct_a_v<Result, typename JsonMembers::parse_to_t...>,
				  "Supplied types cannot be used for construction of this type" );

				sv = parser::trim_left( sv );
				exception::precondition_check( !sv.empty( ) and sv.front( ) == '{' );
				sv.remove_prefix( );
				sv = parser::trim_left( sv );
				auto const locations = get_locations( sv );

				// Ensure locations of all parts there

				return construct_a<Result>{}( parse_item<Is>( locations )... );
			}

			template<typename OutputIterator, size_t... Is, typename... Args>
			static constexpr OutputIterator
			serialize_json_class( OutputIterator it, std::index_sequence<Is...>,
			                      std::tuple<Args...> &&args ) {

				*it++ = '{';
				(void)( ( impl::make_json_string<
				            daw::traits::nth_element<Is, JsonMembers...>, Is>( it,
				                                                               args ),
				          0 ) +
				        ... );
				*it++ = '}';
				return it;
			}

		public:
			template<typename OutputIterator, typename... Args>
			static constexpr OutputIterator serialize( OutputIterator it,
			                                           std::tuple<Args...> &&args ) {
				static_assert( sizeof...( Args ) == sizeof...( JsonMembers ),
				               "Argument count is incorrect" );
				return serialize_json_class( it, std::index_sequence_for<Args...>{},
				                             std::move( args ) );
			}

			template<typename Result>
			static constexpr decltype( auto ) parse( daw::string_view sv ) {
				return parse_json_class<Result>(
				  sv, std::index_sequence_for<JsonMembers...>{} );
			}
		}; // namespace json

		// Member types
		template<typename JsonMember>
		struct json_nullable {
			using i_am_a_json_type = typename JsonMember::i_am_a_json_type;
			static constexpr auto const name = JsonMember::name;
			static constexpr impl::JsonParseTypes expected_type =
			  JsonMember::expected_type;
			static constexpr bool nullable = true;
			using parse_to_t = typename JsonMember::parse_to_t;
			using constructor_t = typename JsonMember::constructor_t;
			static constexpr bool empty_is_null = JsonMember::empty_is_null;

			template<typename OutputIterator, typename Optional,
			         daw::enable_if_t<nullable, impl::is_valid_optional_v<Optional>> =
			           nullptr>
			static constexpr OutputIterator to_string( OutputIterator it,
			                                           Optional const &value ) {
				if( !value ) {
					it = impl::copy_to_iterator( daw::string_view( "null" ), it );
					return it;
				}
				return JsonMember::to_string( it, *value );
			}
		};

		template<JSONNAMETYPE Name, typename T = double,
		         typename Constructor = daw::construct_a<T>>
		struct json_number {
			// TODO maybe convertable or constructible is better
			static_assert( std::is_invocable_v<Constructor, T>,
			               "Constructor must be callable with T" );

			using i_am_a_json_type = void;
			static constexpr auto const name = Name;
			static constexpr impl::JsonParseTypes expected_type =
			  impl::JsonParseTypes::Number;
			static constexpr bool nullable = false;
			// Sometimes numbers are wrapped in strings
			using parse_to_t = T;
			using constructor_t = Constructor;
			static constexpr bool empty_is_null = true;

			template<typename OutputIterator>
			static constexpr OutputIterator to_string( OutputIterator it,
			                                           parse_to_t const &value ) {
				using ::daw::json::to_strings::to_string;
				using std::to_string;
				return impl::copy_to_iterator( to_string( value ), it );
			}
		};

		template<JSONNAMETYPE Name, typename T = bool,
		         typename Constructor = daw::construct_a<T>>
		struct json_bool {
			static_assert( std::is_invocable_v<Constructor, T>,
			               "Constructor must be callable with T" );

			static_assert( std::is_convertible_v<bool, T>,
			               "Supplied result type must be convertable from bool" );
			using i_am_a_json_type = void;
			static constexpr auto const name = Name;
			static constexpr impl::JsonParseTypes expected_type =
			  impl::JsonParseTypes::Bool;
			static constexpr bool nullable = false;
			using parse_to_t = T;
			using constructor_t = Constructor;
			static constexpr bool empty_is_null = true;

			template<typename OutputIterator>
			static constexpr OutputIterator to_string( OutputIterator it,
			                                           parse_to_t const &value ) {
				if( value ) {
					return impl::copy_to_iterator( daw::string_view( "true" ), it );
				}
				return impl::copy_to_iterator( daw::string_view( "false" ), it );
			}
		};

		template<JSONNAMETYPE Name, typename T = std::string,
		         typename Constructor = daw::construct_a<T>>
		struct json_string {
			static_assert(
			  std::is_invocable_v<Constructor, char const *, size_t>,
			  "Constructor must be callable with a char const * and a size_t" );

			using i_am_a_json_type = void;
			static constexpr auto const name = Name;
			static constexpr impl::JsonParseTypes expected_type =
			  impl::JsonParseTypes::String;
			static constexpr bool nullable = false;
			using parse_to_t = T;
			using constructor_t = Constructor;
			static constexpr bool empty_is_null = false;

			template<typename OutputIterator>
			static constexpr OutputIterator to_string( OutputIterator it,
			                                           parse_to_t const &value ) {
				*it++ = '"';
				it = impl::copy_to_iterator( value, it );
				*it++ = '"';
				return it;
			}
		};

		struct parse_js_date {
			constexpr std::chrono::time_point<std::chrono::system_clock,
			                                  std::chrono::milliseconds>
			operator( )( char const *ptr, size_t sz ) const {
				return daw::date_parsing::parse_javascript_timestamp(
				  daw::string_view( ptr, sz ) );
			}
		};

		template<JSONNAMETYPE Name,
		         typename T = std::chrono::time_point<std::chrono::system_clock,
		                                              std::chrono::milliseconds>,
		         typename Constructor = parse_js_date>
		struct json_date {
			static_assert(
			  std::is_invocable_v<Constructor, char const *, size_t>,
			  "Constructor must be callable with a char const * and a size_t" );

			using i_am_a_json_type = void;
			static constexpr auto const name = Name;
			static constexpr impl::JsonParseTypes expected_type =
			  impl::JsonParseTypes::Date;
			static constexpr bool nullable = false;
			using parse_to_t = T;
			using constructor_t = Constructor;
			static constexpr bool empty_is_null = true;

			template<typename OutputIterator>
			static constexpr OutputIterator to_string( OutputIterator it,
			                                           parse_to_t const &value ) {
				using ::daw::json::is_null;
				if( is_null( value ) ) {
					it = impl::copy_to_iterator( daw::string_view( "null" ), it );
				} else {
					*it++ = '"';
					using namespace ::daw::date_formatting::formats;
					it =
					  impl::copy_to_iterator( ::daw::date_formatting::fmt_string(
					                            "{0}T{1}:{2}:{3}Z", value, YearMonthDay{},
					                            Hour{}, Minute{}, Second{} ),
					                          it );
					*it++ = '"';
				}
				return it;
			}
		};

		template<JSONNAMETYPE Name, typename T,
		         typename Constructor = daw::construct_a<T>>
		struct json_class {
			using i_am_a_json_type = void;
			static constexpr auto const name = Name;
			static constexpr impl::JsonParseTypes expected_type =
			  impl::JsonParseTypes::Class;
			static constexpr bool nullable = false;
			using parse_to_t = T;
			using constructor_t = Constructor;
			static constexpr bool empty_is_null = true;

			template<typename OutputIterator>
			static constexpr OutputIterator to_string( OutputIterator it,
			                                           parse_to_t const &value ) {

				return impl::json_parser_description_t<parse_to_t>::template serialize(
				  it, to_json_data( value ) );
			}
		};

		template<typename T>
		struct custom_to_converter_t {
			constexpr decltype( auto ) operator( )( T &&value ) const {
				using std::to_string;
				return to_string( std::move( value ) );
			}
			constexpr decltype( auto ) operator( )( T const &value ) const {
				using std::to_string;
				return to_string( value );
			}
		};

		template<typename T>
		struct custom_from_converter_t {
			constexpr decltype( auto ) operator( )( daw::string_view sv ) {
				return from_string( daw::tag<T>, sv );
			}
		};

		template<JSONNAMETYPE Name, typename T,
		         typename FromConverter = custom_from_converter_t<T>,
		         typename ToConverter = custom_to_converter_t<T>>
		struct json_custom {
			using i_am_a_json_type = void;
			static constexpr auto const name = Name;
			static constexpr impl::JsonParseTypes expected_type =
			  impl::JsonParseTypes::Custom;
			static constexpr bool nullable = false;
			// Sometimes numbers are wrapped in strings
			using parse_to_t = T;
			using to_converter_t = ToConverter;
			using from_converter_t = FromConverter;
			static constexpr bool empty_is_null = false;

			template<typename OutputIterator>
			static constexpr OutputIterator to_string( OutputIterator it,
			                                           parse_to_t const &value ) {
				using ::daw::json::to_strings::to_string;
				return impl::copy_to_iterator( to_converter_t{}( value ), it );
			}
		};

		template<JSONNAMETYPE Name, typename Container, typename JsonElement,
		         typename Constructor = daw::construct_a<Container>,
		         typename Appender = impl::basic_appender<Container>>
		struct json_array {
			using i_am_a_json_type = void;
			static constexpr auto const name = Name;
			static constexpr impl::JsonParseTypes expected_type =
			  impl::JsonParseTypes::Array;
			static constexpr bool nullable = false;
			using parse_to_t = Container;
			using constructor_t = Constructor;
			using appender_t = Appender;
			using json_element_t = JsonElement;
			static constexpr bool empty_is_null = true;

			template<typename OutputIterator>
			static constexpr OutputIterator to_string( OutputIterator it,
			                                           parse_to_t const &container ) {
				*it++ = '[';
				if( !std::empty( container ) ) {
					auto count = std::size( container ) - 1;
					for( auto const &v : container ) {
						it = JsonElement::to_string( it, v );
						if( count-- > 0 ) {
							*it++ = ',';
						}
					}
				}
				*it++ = ']';
				return it;
			}
		};

		template<typename T>
		constexpr T from_json( daw::string_view json_data ) {
			static_assert(
			  impl::has_json_parser_description_v<T>,
			  "A function call describe_json_class must exist for type." );

			return impl::json_parser_description_t<T>::template parse<T>( json_data );
		}

		template<typename Result = std::string, typename T>
		constexpr Result to_json( T &&value ) {
			static_assert(
			  impl::has_json_parser_description_v<T>,
			  "A function called describe_json_class must exist for type." );
			static_assert( impl::has_json_to_json_data_v<T>,
			               "A function called to_json_data must exist for type." );

			Result result{};
			impl::json_parser_description_t<T>::template serialize(
			  daw::back_inserter( result ),
			  to_json_data( std::forward<T>( value ) ) );
			return result;
		}

		/// allow iteration over an array of json
		template<typename JsonElement>
		class json_array_iterator {
			daw::string_view m_state{};
			impl::skip_value_result_t m_cur_value{};

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

			constexpr value_type operator*( ) const noexcept {
				daw::exception::precondition_check<impl::invalid_array>(
				  !m_cur_value.sv.empty( ) );

				auto vp = impl::value_pos(
				  false, impl::is_json_empty_null_v<JsonElement>, m_cur_value.sv );
				vp.parsed_sv = m_cur_value.parsed_sv;
				return impl::parse_value<JsonElement>(
				  impl::ParseTag<JsonElement::expected_type>{}, vp );
			}

			constexpr json_array_iterator &operator++( ) {
				if( m_state.empty( ) or m_state.front( ) == ']' ) {
					m_cur_value = daw::string_view{};
					return *this;
				}
				m_cur_value = impl::skip_value( m_state ).sv;
				m_state = daw::parser::trim_left( m_state );
				return *this;
			}

			constexpr json_array_iterator operator++( int ) {
				auto tmp = *this;
				operator++( );
				return tmp;
			}

			explicit constexpr operator bool( ) const noexcept {
				return !m_cur_value.sv.empty( );
			}

			constexpr bool operator==( json_array_iterator const &rhs ) const
			  noexcept {
				return ( m_cur_value.sv.empty( ) and !rhs ) or
				       ( m_state == rhs.m_state and
				         m_cur_value.sv == rhs.m_cur_value.sv );
			}

			constexpr bool operator!=( json_array_iterator const &rhs ) const
			  noexcept {
				return !( *this == rhs );
			}
		};

		static inline constexpr char const no_name[] = "";
		template<typename JsonElement,
		         typename Container = std::vector<typename JsonElement::parse_to_t>,
		         typename Constructor = daw::construct_a<Container>,
		         typename Appender = impl::basic_appender<Container>>
		constexpr auto from_json_array( daw::string_view json_data ) {

			using parser_t =
			  json_array<no_name, Container, JsonElement, Constructor, Appender>;
			return impl::parse_value<parser_t>(
			  impl::ParseTag<impl::JsonParseTypes::Array>{},
			  impl::value_pos( false, false, json_data ) );
		}

		template<typename Result = std::string, typename Container>
		constexpr Result to_json_array( Container &&c ) {
			Result result = "[";
			for( auto const &v : c ) {
				result += to_json( v );
				result += ',';
			}
			result.back( ) = ']';
			return result;
		}
	} // namespace json
} // namespace daw
