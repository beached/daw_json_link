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

#include <cstdint>
#include <limits>
#include <string>
//#include <iostream>

#include <daw/daw_algorithm.h>
#include <daw/daw_bounded_string.h>
#include <daw/daw_exception.h>
#include <daw/daw_parser_helper_sv.h>
#include <daw/daw_sort_n.h>
#include <daw/daw_string_view.h>
#include <daw/daw_traits.h>
#include <daw/daw_utility.h>

namespace daw {
	namespace json {
		enum class JsonParseTypes : uint_fast8_t {
			Number,
			Bool,
			String,
			Date,
			Class
		};

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
			size_t first = std::numeric_limits<size_t>::max( );
			size_t last = std::numeric_limits<size_t>::max( );
			bool is_nullable;

			constexpr value_pos( bool Nullable ) noexcept
			  : is_nullable( Nullable ) {}
		};

		template<typename string_t>
		kv_t( string_t )->kv_t<string_t>;

		template<typename JsonType>
		using json_parse_to = typename JsonType::parse_to_t;

		template<typename JsonType>
		inline constexpr bool is_json_nullable_v = JsonType::nullable;

		daw::string_view parse_name( daw::string_view &sv ) {
			sv = daw::parser::trim_left( sv );
//			std::cout << sv.front( ) << std::endl;
			daw::exception::precondition_check( sv.front( ) == '"' );
			sv.remove_prefix( );
			auto result = sv.pop_front( '"' );
			sv = daw::parser::trim_left( sv );
			sv.remove_prefix( ':' );
			sv = daw::parser::trim_left( sv );
			return result;
		}

		constexpr daw::string_view skip_string( daw::string_view sv ) noexcept {
			// Assumes front()  == '"'
			sv.remove_prefix( );
			sv.pop_front( '"' );
			if( sv.front( ) == ',' ) {
				sv.remove_prefix( );
			}
			return sv;
		}

		constexpr daw::string_view skip_other( daw::string_view sv ) noexcept {
			sv.pop_front( ' ' );
			if( sv.front( ) == ',' ) {
				sv.remove_prefix( );
			}
			return sv;
		}

		constexpr daw::string_view skip_class( daw::string_view sv ) noexcept {
			// TODO
			return sv;
		}

		constexpr daw::string_view skip_array( daw::string_view sv ) noexcept {
			// TODO
			return sv;
		}

		constexpr void skip_value( daw::string_view &sv ) {
			sv = daw::parser::trim_left( sv );
			switch( sv.front( ) ) {
			case '"':
				sv = skip_string( sv );
				break;
			case '[':
				sv = skip_array( sv );
				break;
			case '{':
				sv = skip_class( sv );
			default:
				sv = skip_other( sv );
				break;
			}
		}
		// Keep a stack of the class/array positions and
		// skip individual values of other types until
		// stack is empty

		template<daw::basic_bounded_string Name, typename T = double,
		         bool Nullable = false>
		struct json_number {
			static_assert( std::is_arithmetic_v<T>,
			               "Number must be an arithmentic type" );
			static constexpr auto const name = Name;
			static constexpr JsonParseTypes expected_type =
			  JsonParseTypes::Number;
			static constexpr bool nullable = Nullable;
			using parse_to_t = T;
		};

		template<daw::basic_bounded_string Name, typename T = bool,
		         bool Nullable = false>
		struct json_bool {
			static_assert( std::is_convertible_v<bool, T>,
			               "Supplied result type must be convertable from bool" );
			static constexpr auto const name = Name;
			static constexpr JsonParseTypes expected_type = JsonParseTypes::Bool;
			static constexpr bool nullable = Nullable;
			using parse_to_t = T;
		};

		template<daw::basic_bounded_string Name, typename T = void, //std::string,
		         bool Nullable = false>
		struct json_string {
			static constexpr auto const name = Name;
			static constexpr JsonParseTypes expected_type = JsonParseTypes::String;
			static constexpr bool nullable = Nullable;
			using parse_to_t = T;
		};

		template<daw::basic_bounded_string Name,
		         typename T = std::chrono::system_clock::time_point,
		         bool Nullable = false>
		struct json_date {
			static constexpr auto const name = Name;
			static constexpr JsonParseTypes expected_type = JsonParseTypes::Date;
			static constexpr bool nullable = Nullable;
			using parse_to_t = T;
		};

		template<typename ParseInfo>
		constexpr auto parse_value( ParseTag<JsonParseTypes::Number>, value_pos pos,
		                            daw::string_view sv ) {
			using result_t = typename ParseInfo::parse_to_t;
			if constexpr( is_floating_point_v<result_t> ) {
				// TODO
				return static_cast<result_t>( 0.12345 );
			} else {
				return daw::parser::parse_int<result_t>(
				  sv.substr( pos.first, pos.last - pos.first ) );
			}
		}

		template<typename ParseInfo>
		constexpr auto parse_value( ParseTag<JsonParseTypes::Bool>, value_pos pos,
		                            daw::string_view sv ) {
			using result_t = typename ParseInfo::parse_to_t;
			return result_t{};
		}

		template<typename ParseInfo>
		constexpr auto parse_value( ParseTag<JsonParseTypes::String>, value_pos pos,
		                            daw::string_view sv ) {

			using result_t = typename ParseInfo::parse_to_t;
			return result_t{};
		}

		template<typename ParseInfo>
		constexpr auto parse_value( ParseTag<JsonParseTypes::Date>, value_pos pos,
		                            daw::string_view sv ) {
			using result_t = typename ParseInfo::parse_to_t;
			return result_t{};
		}

		template<typename ParseInfo>
		constexpr auto parse_value( ParseTag<JsonParseTypes::Class>, value_pos pos,
		                            daw::string_view sv ) {
			using result_t = typename ParseInfo::parse_to_t;
			return result_t{};
		}

		template<typename... JsonMembers>
		struct json_link {
			template<size_t... Is>
			static constexpr size_t find_string_capacity( ) noexcept {
				return ( JsonMembers::name.extent + ... );
			}
			using string_t = daw::basic_bounded_string<char, find_string_capacity( )>;

			template<size_t N>
			static constexpr kv_t<string_t> get_item( ) noexcept {
				using type_t = daw::traits::nth_type<N, JsonMembers...>;
				return {type_t::name, type_t::expected_type, type_t::nullable, N};
			}

			template<size_t... Is>
			static constexpr auto make_map( std::index_sequence<Is...> ) noexcept {

				return std::array{get_item<Is>( )...};
			}

			static constexpr auto name_map =
			  make_map( std::index_sequence_for<JsonMembers...>{} );

			static constexpr bool has_name( daw::string_view key ) noexcept {
				auto result = daw::algorithm::find_if(
				  begin( name_map ), end( name_map ),
				  [key]( auto const &kv ) { return kv.name == key; } );
				return result != std::end( name_map );
			}

			static constexpr size_t find_name( daw::string_view key ) noexcept {
				auto result = daw::algorithm::find_if(
				  begin( name_map ), end( name_map ),
				  [key]( auto const &kv ) { return kv.name == key; } );
				if( result == std::end( name_map ) ) {
					std::terminate( );
				}
				return std::distance( begin( name_map ), result );
			}

			template<size_t N>
			static constexpr decltype( auto ) parse_item(
			  std::array<value_pos, sizeof...( JsonMembers )> const &locations,
			  daw::string_view sv ) {
				using type_t = daw::traits::nth_type<N, JsonMembers...>;
				return parse_value<type_t>( ParseTag<type_t::expected_type>{},
				                            locations[N], sv );
			}

		private:
			template<typename Result, size_t... Is>
			static Result parse_json_class( daw::string_view sv,
			                               std::index_sequence<Is...> ) {
				static_assert(
				  daw::can_construct_a_v<Result, typename JsonMembers::parse_to_t...>,
				  "Supplied types cannot be used for construction of this type" );

				std::array locations = {
				  value_pos( is_json_nullable_v<JsonMembers> )...};
				auto sv_orig = sv;
				auto const first_pos = sv.begin( );
				sv = daw::parser::trim_left( sv );
				daw::exception::precondition_check( sv.front( ) == '{' );
				sv.remove_prefix( );
				sv = daw::parser::trim_left( sv );
				while( !sv.empty( ) and sv.front( ) != '}' ) {
//					std::cout << "parse_json_class: " << sv << std::endl;
					auto name = parse_name( sv );
					if( !has_name( name ) ) {
						skip_value( sv );
						continue;
					}
					size_t const pos = find_name( name );
					locations[pos].first = std::distance( first_pos, sv.begin( ) );
					skip_value( sv );
					locations[pos].last = std::distance( first_pos, sv.begin( ) );
					sv = daw::parser::trim_left( sv );
				}
				// Ensure locations of all parts there
				daw::exception::precondition_check( daw::algorithm::all_of(
				  begin( locations ), end( locations ), []( auto const &loc ) {
					  return loc.is_nullable or
					         ( loc.first != std::numeric_limits<size_t>::max( ) and
					           loc.last != std::numeric_limits<size_t>::max( ) );
				  } ) );

				return daw::construct_a<Result>{}(
				  parse_item<Is>( locations, sv_orig )... );
			}

		public:
			template<typename Result>
			static constexpr decltype( auto ) parse( daw::string_view sv ) {
				return parse_json_class<Result>( sv, std::index_sequence_for<JsonMembers...>{} );
			}
		};

		template<typename T>
		T from_json_t( daw::string_view sv ) {
			using parse_info_t = decltype( get_json_link( std::declval<T>( ) ) );
			return parse_info_t::template parse<T>( sv );
		}

	} // namespace json
} // namespace daw
