// The MIT License( MIT )
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

#include "daw_iterator_range.h"
#include "daw_json_assert.h"
#include "daw_json_parse_common.h"
#include "daw_json_parse_value.h"
#include "daw_json_to_string.h"

#include <daw/daw_algorithm.h>
#include <daw/daw_cxmath.h>
#include <daw/daw_parser_helper_sv.h>
#include <daw/daw_string_view.h>
#include <daw/daw_traits.h>
#include <daw/daw_utility.h>
#include <daw/iterator/daw_back_inserter.h>
#include <daw/iterator/daw_inserter.h>

#include <array>
#include <chrono>
#include <cstdint>
#include <cstdlib>
#include <date/date.h>
#include <iterator>
#include <optional>
#include <string_view>
#include <tuple>
#include <type_traits>
#include <utility>

namespace daw::json::impl {
	namespace {
		[[nodiscard]] constexpr char to_lower( char c ) noexcept {
			return static_cast<char>( static_cast<unsigned>( c ) |
			                          static_cast<unsigned>( ' ' ) );
		}

		[[nodiscard]] constexpr wchar_t to_lower( wchar_t const c ) noexcept {
			return static_cast<wchar_t>( static_cast<unsigned>( c ) |
			                             static_cast<unsigned>( L' ' ) );
		}

		template<typename Result>
		constexpr Result to_integer( char const c ) noexcept {
			return static_cast<Result>( c - '0' );
		}

		template<typename Result, size_t count, typename CharT>
		constexpr Result parse_unsigned( const CharT *digit_str ) noexcept {
			Result result = 0;
			for( size_t n = 0; n < count; ++n ) {
				result = static_cast<Result>( ( result << 1 ) + ( result << 3 ) ) +
				         to_integer<Result>( digit_str[n] );
			}
			return result;
		}

		template<typename CharT, typename Traits>
		constexpr std::chrono::time_point<std::chrono::system_clock,
		                                  std::chrono::milliseconds>
		parse_javascript_timestamp(
		  daw::basic_string_view<CharT, Traits> timestamp_str ) {
			daw_json_assert(
			  ( timestamp_str.size( ) == 24 ) and
			    ( to_lower( timestamp_str[23] ) == static_cast<CharT>( 'z' ) ),
			  "Invalid ISO8601 Timestamp" );
			auto const yr = parse_unsigned<uint16_t, 4>( timestamp_str.data( ) );
			auto const mo = parse_unsigned<uint8_t, 2>( timestamp_str.data( ) + 5 );
			auto const dy = parse_unsigned<uint8_t, 2>( timestamp_str.data( ) + 8 );
			auto const hr = parse_unsigned<uint8_t, 2>( timestamp_str.data( ) + 11 );
			auto const mi = parse_unsigned<uint8_t, 2>( timestamp_str.data( ) + 14 );
			auto const sc = parse_unsigned<uint8_t, 2>( timestamp_str.data( ) + 17 );
			auto const ms = parse_unsigned<uint16_t, 3>( timestamp_str.data( ) + 20 );

			daw_json_assert( 0 <= yr and yr <= 9999, "Invalid year" );
			daw_json_assert( 1 <= mo and mo <= 12, "Invalid month" );
			daw_json_assert( 1 <= dy and dy <= 31, "Invalid month" );
			daw_json_assert( 0 <= hr and hr <= 24, "Invalid hour" );
			daw_json_assert( 0 <= mi and mi <= 59, "Invalid minute" );
			daw_json_assert( 0 <= sc and sc <= 60, "Invalid second" );
			daw_json_assert( 0 <= ms and ms <= 999, "Invalid millisecond" );

			return std::chrono::time_point<std::chrono::system_clock,
			                               std::chrono::milliseconds>(
			  date::sys_days( date::year( yr ) / date::month( mo ) /
			                  date::day( dy ) ) +
			  std::chrono::hours( hr ) + std::chrono::minutes( mi ) +
			  std::chrono::seconds( sc ) + std::chrono::milliseconds( ms ) );
		}
	} // namespace
} // namespace daw::json::impl

namespace daw::json {
	template<JsonNullable>
	struct parse_js_date;

	template<>
	struct parse_js_date<JsonNullable::Never> {
		using result_type = std::chrono::time_point<std::chrono::system_clock,
		                                            std::chrono::milliseconds>;

		[[maybe_unused, nodiscard]] constexpr result_type
		operator( )( char const *ptr, size_t sz ) const {
			return impl::parse_javascript_timestamp( daw::string_view( ptr, sz ) );
		}
	};

	template<>
	struct parse_js_date<JsonNullable::Nullable> {
		using result_type =
		  std::optional<std::chrono::time_point<std::chrono::system_clock,
		                                        std::chrono::milliseconds>>;

		[[maybe_unused, nodiscard]] constexpr result_type operator( )( ) const {
			return {};
		}

		[[maybe_unused, nodiscard]] constexpr result_type
		operator( )( char const *ptr, size_t sz ) const {
			return impl::parse_javascript_timestamp( daw::string_view( ptr, sz ) );
		}
	};

	template<typename T>
	struct custom_from_converter_t {
		[[nodiscard]] constexpr decltype( auto )
		operator( )( std::string_view sv ) {
			return from_string( daw::tag<T>, sv );
		}
	};
} // namespace daw::json

namespace daw::json::impl {
	namespace {
		// Paths are specified with dot separators, if the name has a dot in it,
		// it must be escaped
		// memberA.memberB.member\.C has 3 parts['memberA', 'memberB', 'member.C']
		[[nodiscard]] constexpr daw::string_view
		pop_json_path( daw::string_view &path ) {
			return path.pop_front( [in_escape = false]( char c ) mutable {
				if( in_escape ) {
					in_escape = false;
					return false;
				}
				if( c == '\\' ) {
					in_escape = true;
					return false;
				}
				return ( c == '.' );
			} );
		}

		namespace data_size {
			[[nodiscard]] constexpr char const *data( char const *ptr ) noexcept {
				return ptr;
			}

			[[nodiscard]] constexpr size_t size( char const *ptr ) noexcept {
				return std::string_view( ptr ).size( );
			}

			using std::data;
			template<typename T>
			using data_detect = decltype( data( std::declval<T &>( ) ) );

			using std::size;
			template<typename T>
			using size_detect = decltype( size( std::declval<T &>( ) ) );

			template<typename T>
			inline constexpr bool has_data_size_v = daw::is_detected_v<data_detect, T>
			  and daw::is_detected_v<size_detect, T>;
		} // namespace data_size

		template<typename string_t>
		struct kv_t {
			string_t name;
			JsonParseTypes expected_type;
			size_t pos;

			constexpr kv_t( string_t Name, JsonParseTypes Expected, size_t Pos )
			  : name( daw::move( Name ) )
			  , expected_type( Expected )
			  , pos( Pos ) {}
		};

		template<typename JsonType>
		using json_parse_to = typename JsonType::parse_to_t;

		struct member_name_parse_error {};

		// Get the next member name
		// Assumes that the current item in stream is a double quote
		// Ensures that the stream is left at the position of the associated
		// value(e.g after the colon(:) and trimmed)
		template<typename First, typename Last, bool IsTrustedInput>
		[[nodiscard]] constexpr daw::string_view
		parse_name( IteratorRange<First, Last, IsTrustedInput> &rng ) {
			daw_json_assert_untrusted( rng.front( '"' ),
			                           "Expected name to start with a quote" );
			rng.remove_prefix( );
			return daw::json::impl::name::name_parser::parse_nq( rng );
		}

		[[nodiscard]] constexpr char const *str_find( char const *p, char c ) {
			while( *p != c ) {
				++p;
			}
			return p;
		}

		template<typename Container>
		struct basic_kv_appender {
			Container *m_container;

			constexpr basic_kv_appender( Container &container ) noexcept
			  : m_container( &container ) {}

			template<typename Key, typename Value>
			constexpr void operator( )( Key &&key, Value &&value ) {
				m_container->insert( std::make_pair( std::forward<Key>( key ),
				                                     std::forward<Value>( value ) ) );
			}
		};

		template<size_t N, typename string_t, typename... JsonMembers>
		[[nodiscard]] constexpr kv_t<string_t> get_item( ) noexcept {
			using type_t = traits::nth_type<N, JsonMembers...>;
			return kv_t<string_t>( type_t::name, type_t::expected_type, N );
		}

		template<typename First, typename Last, bool IsTrustedInput>
		struct location_info_t {
			daw::string_view name;
			IteratorRange<First, Last, IsTrustedInput> location{};

			explicit constexpr location_info_t( daw::string_view Name ) noexcept
			  : name( Name ) {}

			[[maybe_unused, nodiscard]] constexpr bool missing( ) const {
				return location.is_null( );
			}
		};

		template<size_t N, typename First, typename Last, bool IsTrustedInput>
		struct locations_info_t {
			using value_type = location_info_t<First, Last, IsTrustedInput>;
			std::array<value_type, N> names;

			constexpr decltype( auto ) begin( ) const {
				return names.begin( );
			}

			constexpr decltype( auto ) begin( ) {
				return names.begin( );
			}

			constexpr decltype( auto ) end( ) const {
				return names.end( );
			}

			constexpr decltype( auto ) end( ) {
				return names.end( );
			}

			constexpr location_info_t<First, Last, IsTrustedInput> const &
			operator[]( size_t idx ) const {
				return names[idx];
			}

			constexpr location_info_t<First, Last, IsTrustedInput> &
			operator[]( size_t idx ) {
				return names[idx];
			}

			static constexpr size_t size( ) noexcept {
				return N;
			}

			[[nodiscard]] constexpr size_t find_name( daw::string_view key ) const
			  noexcept {

				auto result =
				  algorithm::find_if( begin( ), end( ), [key]( auto const &loc ) {
					  return loc.name == key;
				  } );

				return static_cast<size_t>( std::distance( begin( ), result ) );
			}
		};

		template<typename JsonMember, size_t N, typename First, typename Last,
		         bool IsTrustedInput>
		[[nodiscard]] constexpr IteratorRange<First, Last, IsTrustedInput>
		find_class_member(
		  size_t pos, locations_info_t<N, First, Last, IsTrustedInput> &locations,
		  IteratorRange<First, Last, IsTrustedInput> &rng ) {

			daw_json_assert_untrusted(
			  is_json_nullable_v<JsonMember> or not locations[pos].missing( ) or
			    not rng.front( '}' ),
			  "Unexpected end of class.  Non-nullable members still not found" );

			rng.trim_left_no_check( );
			while( locations[pos].missing( ) and rng.front( ) != '}' ) {
				auto const name = parse_name( rng );
				auto const name_pos = locations.find_name( name );
				if( name_pos >= locations.size( ) ) {
					// This is not a member we are concerned with
					(void)skip_value( rng );
					rng.clean_tail( );
					continue;
				}
				if( name_pos != pos ) {
					// We are out of order, store position for later
					// TODO:	use type knowledge to speed up skip
					// TODO:	on skipped classes see if way to store
					// 				member positions so that we don't have to
					//				reparse them after
					// RESULT: storing preparsed is slower, don't try 3 times
					// it also limits the type of things we can parse potentially
					locations[name_pos].location = skip_value( rng );
					rng.clean_tail( );
					continue;
				}
				locations[pos].location = rng;
			}
			return locations[pos].location;
		}

		template<typename JsonMember, size_t N, typename First, typename Last,
		         bool IsTrustedInput>
		[[nodiscard]] constexpr json_result<JsonMember> parse_class_member(
		  size_t member_position,
		  locations_info_t<N, First, Last, IsTrustedInput> &locations,
		  IteratorRange<First, Last, IsTrustedInput> &rng ) {

			rng.clean_tail( );
			if constexpr( is_no_name<JsonMember::name> ) {
				// If we are an array element
				return parse_value<JsonMember>( ParseTag<JsonMember::expected_type>{},
				                                rng );
			} else {
				daw_json_assert_untrusted( rng.front( "\"}" ),
				                           "Expected end of class or start of member" );
				auto loc =
				  find_class_member<JsonMember>( member_position, locations, rng );

				daw_json_assert_untrusted( is_json_nullable_v<JsonMember> or
				                             not loc.is_null( ),
				                           "Could not find required class member" );
				if( loc.is_null( ) or
				    ( not rng.is_null( ) and rng.begin( ) != loc.begin( ) ) ) {

					return parse_value<JsonMember>( ParseTag<JsonMember::expected_type>{},
					                                loc );
				}
				return parse_value<JsonMember>( ParseTag<JsonMember::expected_type>{},
				                                rng );
			}
		}

		template<size_t N, typename... JsonMembers>
		using nth = daw::traits::nth_element<N, JsonMembers...>;

		template<typename... JsonMembers, typename OutputIterator, size_t... Is,
		         typename Tuple>
		[[nodiscard]] constexpr OutputIterator
		serialize_json_class( OutputIterator it, std::index_sequence<Is...>,
		                      Tuple const &args ) {

			bool is_first = true;
			*it++ = '{';

			(void)( ( to_json_str<Is, nth<Is, JsonMembers...>>( is_first, it, args ),
			          ... ),
			        0 );

			*it++ = '}';
			return it;
		}

		template<typename First, typename Last, bool IsTrustedInput,
		         typename... JsonMembers>
		static inline constexpr auto known_locations_v =
		  locations_info_t<sizeof...( JsonMembers ), First, Last, IsTrustedInput>{
		    location_info_t<First, Last, IsTrustedInput>( JsonMembers::name )...};

		template<typename JsonClass, typename... JsonMembers, size_t... Is,
		         typename First, typename Last, bool IsTrustedInput>
		[[nodiscard]] constexpr JsonClass
		parse_json_class( IteratorRange<First, Last, IsTrustedInput> &rng,
		                  std::index_sequence<Is...> ) {
			static_assert( has_json_data_contract_trait_v<JsonClass>,
			               "Unexpected type" );
			static_assert(
			  can_construct_a_v<
			    JsonClass,
			    decltype( std::declval<typename JsonMembers::parse_to_t>( ) )...>,
			  "Supplied types cannot be used for	construction of this type" );

			rng.move_to_next_of( '{' );
			rng.remove_prefix( );
			rng.move_to_next_of( "\"}" );
			if constexpr( sizeof...( JsonMembers ) == 0 ) {
				// We are an empty class
				daw_json_assert_untrusted( rng.front( '}' ),
				                           "Expected class to end with '}'" );
				rng.remove_prefix( );
				rng.trim_left( );
				return construct_a<JsonClass>( );
			} else {
				auto known_locations =
				  known_locations_v<First, Last, IsTrustedInput, JsonMembers...>;

				using tp_t = std::tuple<decltype(
				  parse_class_member<traits::nth_type<Is, JsonMembers...>>(
				    Is, known_locations, rng ) )...>;
				/*
				 * Rather than call directly use apply/tuple to evaluate left->right
				 */
				JsonClass result = std::apply(
				  daw::construct_a<JsonClass>,
				  tp_t{parse_class_member<traits::nth_type<Is, JsonMembers...>>(
				    Is, known_locations, rng )...} );

				rng.clean_tail( );
				// If we fullfill the contract before all values are parses
				while( rng.front( ) != '}' ) {
					(void)parse_name( rng );
					(void)skip_value( rng );
					rng.clean_tail( );
				}

				daw_json_assert_untrusted( rng.front( ) == '}',
				                           "Expected class to end with '}'" );
				rng.remove_prefix( );
				rng.trim_left( );
				return result;
			}
		}

		[[nodiscard]] constexpr bool
		json_path_compare( daw::string_view json_path_item,
		                   daw::string_view member_name ) noexcept {
			if( json_path_item.front( ) == '\\' ) {
				json_path_item.remove_prefix( );
			}
			while( not json_path_item.empty( ) and not member_name.empty( ) ) {
				if( json_path_item.front( ) != member_name.front( ) ) {
					return false;
				}
				json_path_item.remove_prefix( );
				if( not json_path_item.empty( ) and json_path_item.front( ) == '\\' ) {
					json_path_item.remove_prefix( );
				}
				member_name.remove_prefix( );
			}
			return json_path_item.size( ) == member_name.size( );
		}

		template<typename First, typename Last, bool IsTrustedInput>
		constexpr void find_range2( IteratorRange<First, Last, IsTrustedInput> &rng,
		                            daw::string_view path ) {
			auto current = impl::pop_json_path( path );
			while( not current.empty( ) ) {
				daw_json_assert_untrusted( rng.front( '{' ), "Invalid Path Entry" );
				rng.remove_prefix( );
				rng.trim_left_no_check( );
				auto name = parse_name( rng );
				while( not json_path_compare( current, name ) ) {
					(void)skip_value( rng );
					rng.clean_tail( );
					name = parse_name( rng );
				}
				current = impl::pop_json_path( path );
			}
		}

		template<bool IsTrustedInput, typename String>
		[[nodiscard]] constexpr auto find_range( String &&str,
		                                         daw::string_view start_path ) {

			auto rng = IteratorRange<char const *, char const *, IsTrustedInput>(
			  std::data( str ), std::data( str ) + std::size( str ) );
			rng.trim_left( );
			if( rng.has_more( ) and not start_path.empty( ) ) {
				find_range2( rng, start_path );
			}
			daw_json_assert( rng.front( ) == '[', "Expected start of json array" );
			return rng;
		}

		template<typename JsonClass, bool IsTrustedInput>
		[[maybe_unused, nodiscard]] constexpr JsonClass
		from_json_impl( std::string_view json_data ) {
			static_assert( impl::has_json_data_contract_trait_v<JsonClass>,
			               "Expected a typed that has been mapped via specialization "
			               "of daw::json::json_data_contract" );

			daw_json_assert_untrusted( not json_data.empty( ),
			                           "Attempt to parse empty string" );

			return impl::json_data_contract_trait_t<JsonClass>::template parse<
			  JsonClass, IsTrustedInput>( json_data );
		}
	} // namespace
} // namespace daw::json::impl
