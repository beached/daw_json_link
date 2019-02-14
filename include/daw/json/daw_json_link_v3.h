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
#include <iostream>
#include <limits>
#include <string>

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
		constexpr char to_lower( char c ) noexcept {
			return c | ' ';
		}

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
			daw::string_view value_str{};
			bool is_nullable;

			constexpr value_pos( bool Nullable ) noexcept
			  : is_nullable( Nullable ) {}

			constexpr explicit operator bool( ) const noexcept {
				return is_nullable or !value_str.empty( );
			}
		};

		template<typename string_t>
		kv_t( string_t )->kv_t<string_t>;

		template<typename JsonType>
		using json_parse_to = typename JsonType::parse_to_t;

		template<typename JsonType>
		inline constexpr bool is_json_nullable_v = JsonType::nullable;

		daw::string_view parse_name( daw::string_view &sv ) {
			sv = daw::parser::trim_left( sv );
			std::clog << "parse_name1#" << sv << "#\n";
			std::clog << "front #" << sv.front( ) << "#\n";
			daw::exception::precondition_check( sv.front( ) == '"' );
			sv.remove_prefix( );
			auto name = sv.pop_front( "\"" );
			std::clog << "name #" << name << "#\n";
			sv = daw::parser::trim_left( sv );
			std::clog << "find :#" << sv.pop_front( ":" ) << "#\n";
			sv = daw::parser::trim_left( sv );
			std::clog << "parse_name2#" << sv << "#\n";
			return name;
		}

		daw::string_view skip_string( daw::string_view &sv ) noexcept {
			// TODO: handle escaped quotes
			// Assumes front()  == '"'
			std::clog << "skip_string 1#" << sv << "#\n";
			auto tmp = sv.substr( 1 );
			auto result = tmp.pop_front( "\"" );
			sv = tmp;
			auto pos = sv.find_first_of( ",}]" );
			daw::exception::precondition_check( pos != sv.npos, "Invalid class" );
			sv.remove_prefix( pos + 1 );
			sv = daw::parser::trim_left( sv );
			std::clog << "skip_string 2#" << sv << "#\n";
			return result;
		}

		daw::string_view skip_other( daw::string_view &sv ) noexcept {
			std::clog << "skip_other 1#" << sv << "#\n";
			auto pos = sv.find_first_of( ",}]" );
			daw::exception::precondition_check( pos != sv.npos, "Invalid class" );
			auto result = sv.pop_front( pos );
			sv.remove_prefix( );
			sv = daw::parser::trim_left( sv );
			std::clog << "skip_other 2#" << sv << "#\n";
			return result;
		}

		daw::string_view skip_class( daw::string_view &sv ) noexcept {
			std::clog << "skip_class 1#" << sv << "#\n";
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
						in_quotes != in_quotes;
						sv.remove_prefix( );
						continue;
					}
					break;
				case '{':
					if( !in_quotes and !is_escaped ) {
						++bracket_count;
					}
					break;
				case '}':
					if( !in_quotes and !is_escaped ) {
						--bracket_count;
					}
				}
				is_escaped = false;
				sv.remove_prefix( );
			}
			tmp_sv = tmp_sv.pop_front( tmp_sv.size( ) - sv.size( ) );
			std::clog << "class #" << tmp_sv << "#\n";
			auto pos = sv.find_first_of( ",}]" );
			daw::exception::precondition_check( pos != sv.npos, "Invalid class" );
			auto result = sv.pop_front( pos );
			sv.remove_prefix( );
			sv = daw::parser::trim_left( sv );
			std::clog << "skip_class 2#" << sv << "#\n";
			return tmp_sv;
		}

		daw::string_view skip_array( daw::string_view &sv ) noexcept {
			std::clog << "skip_array 1#" << sv << "#\n";
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
						in_quotes != in_quotes;
						sv.remove_prefix( );
						continue;
					}
					break;
				case '[':
					if( !in_quotes and !is_escaped ) {
						++bracket_count;
					}
					break;
				case ']':
					if( !in_quotes and !is_escaped ) {
						--bracket_count;
					}
				}
				is_escaped = false;
				sv.remove_prefix( );
			}
			tmp_sv = tmp_sv.pop_front( tmp_sv.size( ) - sv.size( ) );
			std::clog << "array #" << tmp_sv << "#\n";
			auto pos = sv.find_first_of( ",}]" );
			daw::exception::precondition_check( pos != sv.npos, "Invalid array" );
			auto result = sv.pop_front( pos );
			sv.remove_prefix( );
			sv = daw::parser::trim_left( sv );
			std::clog << "skip_array 2#" << sv << "#\n";
			return tmp_sv;
		}

		constexpr daw::string_view skip_value( daw::string_view &sv ) {
			sv = daw::parser::trim_left( sv );
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
		// Keep a stack of the class/array positions and
		// skip individual values of other types until
		// stack is empty

		template<daw::basic_bounded_string Name, typename T = double,
		         bool Nullable = false>
		struct json_number {
			static_assert( std::is_arithmetic_v<T>,
			               "Number must be an arithmentic type" );
			static constexpr auto const name = Name;
			static constexpr JsonParseTypes expected_type = JsonParseTypes::Number;
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

		template<daw::basic_bounded_string Name, typename T = void, // std::string,
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
		auto parse_value( ParseTag<JsonParseTypes::Number>, value_pos pos ) {
			using result_t = typename ParseInfo::parse_to_t;
			std::clog << "parsing number: #" << pos.value_str << "#\n";
			if constexpr( is_floating_point_v<result_t> ) {
				// TODO
				return static_cast<result_t>( 0.12345 );
			} else {
				return daw::parser::parse_int<result_t>( pos.value_str );
			}
		}

		template<typename ParseInfo>
		constexpr auto parse_value( ParseTag<JsonParseTypes::Bool>,
		                            value_pos pos ) {
			using result_t = typename ParseInfo::parse_to_t;
			std::clog << "parsing bool: #" << pos.value_str << "#\n";

			return daw::construct_a<result_t>{}( to_lower( pos.value_str.front( ) ) ==
			                                     't' );
		}

		template<typename ParseInfo>
		constexpr auto parse_value( ParseTag<JsonParseTypes::String>,
		                            value_pos pos ) {
			std::clog << "parsing string: #" << pos.value_str << "#\n";

			using result_t = typename ParseInfo::parse_to_t;
			return result_t{};
		}

		template<typename ParseInfo>
		constexpr auto parse_value( ParseTag<JsonParseTypes::Date>,
		                            value_pos pos ) {
			std::clog << "parsing date: #" << pos.value_str << "#\n";
			using result_t = typename ParseInfo::parse_to_t;
			return result_t{};
		}

		template<typename ParseInfo>
		constexpr auto parse_value( ParseTag<JsonParseTypes::Class>,
		                            value_pos pos ) {

			std::clog << "parsing class: #" << pos.value_str << "#\n";
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
				                            locations[N] );
			}

		private:
			template<typename Result, size_t... Is>
			static Result parse_json_class( daw::string_view sv,
			                                std::index_sequence<Is...> ) {
				static_assert(
				  daw::can_construct_a_v<Result, typename JsonMembers::parse_to_t...>,
				  "Supplied types cannot be used for construction of this type" );

				auto sv_orig = sv;
				auto const first_pos = sv.begin( );

				sv = daw::parser::trim_left( sv );
				daw::exception::precondition_check( sv.front( ) == '{' );
				sv.remove_prefix( );
				sv = daw::parser::trim_left( sv );

				auto const locations = [&]( ) {
					std::array result = {value_pos( is_json_nullable_v<JsonMembers> )...};

					while( !sv.empty( ) and sv.front( ) != '}' ) {
						std::clog << "parse_json_class: " << std::endl;
						auto name = parse_name( sv );
						if( !has_name( name ) ) {
							std::clog << "skipping: " << name << '\n';
							skip_value( sv );
							continue;
						}
						size_t const pos = find_name( name );
						result[pos].value_str = skip_value( sv );
						sv = daw::parser::trim_left( sv );
					}
					return result;
				}( );
				// Ensure locations of all parts there
				std::clog << "locations\n";
				for( auto const &loc : locations ) {
					std::clog << "{'" << loc.value_str << "', " << loc.is_nullable
					          << "}\n";
				}
				daw::exception::precondition_check( daw::algorithm::all_of(
				  begin( locations ), end( locations ), []( auto const &loc ) -> bool {
					  return static_cast<bool>( loc );
				  } ) );

				return daw::construct_a<Result>{}(
				  parse_item<Is>( locations, sv_orig )... );
			}

		public:
			template<typename Result>
			static constexpr decltype( auto ) parse( daw::string_view sv ) {
				return parse_json_class<Result>(
				  sv, std::index_sequence_for<JsonMembers...>{} );
			}
		};

		template<typename T>
		T from_json_t( daw::string_view sv ) {
			std::clog << "from_json_t" << std::endl;
			using parse_info_t = decltype( get_json_link( std::declval<T>( ) ) );
			return parse_info_t::template parse<T>( sv );
		}

	} // namespace json
} // namespace daw
