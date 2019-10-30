// The MIT License( MIT )
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
#include <optional>
#include <string_view>
#include <tuple>
#include <type_traits>
#include <utility>

#include <daw/daw_algorithm.h>
#ifdef __cpp_nontype_template_parameter_class
#include <daw/daw_bounded_string.h>
#endif
#include <daw/daw_cxmath.h>
#include <daw/daw_parser_helper_sv.h>
#include <daw/daw_string_view.h>
#include <daw/iterator/daw_back_inserter.h>
#include <daw/daw_traits.h>
#include <daw/daw_utility.h>
#include <daw/iso8601/daw_date_formatting.h>
#include <daw/iso8601/daw_date_parsing.h>
#include <daw/iterator/daw_back_inserter.h>
#include <daw/iterator/daw_inserter.h>

#include "daw_iterator_range.h"
#include "daw_json_assert.h"
#include "daw_json_parse_common.h"
#include "daw_json_parse_value.h"
#include "daw_json_to_string.h"

namespace daw::json {
	struct parse_js_date {
		[[maybe_unused,
		  nodiscard]] constexpr std::chrono::time_point<std::chrono::system_clock,
		                                                std::chrono::milliseconds>
		operator( )( char const *ptr, size_t sz ) const {
			return daw::date_parsing::parse_javascript_timestamp(
			  daw::string_view( ptr, sz ) );
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
	// Paths are specified with dot separators, if the name has a dot in it,
	// it must be escaped
	// memberA.memberB.member\.C has 3 parts['memberA', 'memberB', 'member.C']
	[[nodiscard]] static constexpr daw::string_view
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
		namespace {
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
		} // namespace
	}   // namespace data_size

	[[nodiscard]] static constexpr char to_lower( char c ) noexcept {
		return static_cast<char>( static_cast<unsigned>( c ) |
		                          static_cast<unsigned>( ' ' ) );
	}

	template<typename T>
	static inline constexpr bool has_json_parser_description_v =
	  daw::is_detected_v<json_parser_description_t, T>;

	template<typename T>
	using json_parser_to_json_data_t =
	  decltype( to_json_data( std::declval<T &>( ) ) );

	template<typename T>
	static inline constexpr bool has_json_to_json_data_v =
	  daw::is_detected_v<json_parser_to_json_data_t, T>;

	template<typename string_t>
	struct kv_t {
		string_t name;
		JsonParseTypes expected_type;
		// bool nullable;
		size_t pos;

		constexpr kv_t( string_t Name, JsonParseTypes Expected, /*bool Nullable,*/
		                size_t Pos )
		  : name( daw::move( Name ) )
		  , expected_type( Expected )
		  //				  , nullable( Nullable )
		  , pos( Pos ) {}
	};

	template<typename JsonType>
	using json_parse_to = typename JsonType::parse_to_t;

	template<typename JsonType>
	static inline constexpr bool is_json_nullable_v =
	  JsonType::expected_type == JsonParseTypes::Null;

	template<typename JsonType>
	static inline constexpr bool is_json_empty_null_v = JsonType::empty_is_null;

	struct member_name_parse_error {};

	// Get the next member name
	// Assumes that the current item in stream is a double quote
	// Ensures that the stream is left at the position of the associated
	// value(e.g after the colon(:) and trimmed)
	template<typename First, typename Last>
	[[nodiscard]] static constexpr daw::string_view
	parse_name( IteratorRange<First, Last> &rng ) {
		// if( *rng.first == '"' ) {
		++rng.first;
		//}
		auto nr = daw::json::impl::name::name_parser::parse_nq( rng.first );
		auto name = daw::string_view( rng.first, nr.end_of_name );
		rng.first = nr.end_of_whitespace;

		json_assert( not name.empty( ) and not rng.empty( ),
		             "Expected a non empty name and data after name" );
		return name;
	}

	namespace {
		[[nodiscard]] static constexpr char const *str_find( char const *p,
		                                                     char c ) {
			while( *p != c ) {
				++p;
			}
			return p;
		}
	} // namespace

	template<typename Container>
	struct basic_appender {
		daw::back_inserter<Container> appender;

		constexpr basic_appender( Container &container ) noexcept
		  : appender( container ) {}

		template<typename Value>
		constexpr void operator( )( Value &&value ) {
			*appender = std::forward<Value>( value );
		}
	};

	template<typename Container>
	struct basic_kv_appender {
		daw::inserter_iterator<Container> appender;

		constexpr basic_kv_appender( Container &container ) noexcept
		  : appender( container ) {}

		template<typename Key, typename Value>
		constexpr void operator( )( Key &&key, Value &&value ) {
			*appender = std::make_pair( std::forward<Key>( key ),
			                            std::forward<Value>( value ) );
		}
	};

	template<size_t N, typename string_t, typename... JsonMembers>
	[[nodiscard]] static constexpr kv_t<string_t> get_item( ) noexcept {
		using type_t = traits::nth_type<N, JsonMembers...>;
		return {type_t::name, type_t::expected_type, /*type_t::nullable,*/ N};
	}

	template<typename... JsonMembers>
	[[nodiscard]] static constexpr size_t find_string_capacity( ) noexcept {
		return ( json_name_len( JsonMembers::name ) + ... );
	}

	template<typename... JsonMembers, size_t... Is>
	[[nodiscard]] static constexpr auto
	make_map( std::index_sequence<Is...> ) noexcept {
		using string_t =
		  basic_bounded_string<char, find_string_capacity<JsonMembers...>( )>;

		return daw::make_array( get_item<Is, string_t, JsonMembers...>( )... );
	}

	template<typename... JsonMembers>
	struct name_map_t {
		static inline constexpr auto name_map_data =
		  make_map<JsonMembers...>( std::index_sequence_for<JsonMembers...>{} );

		[[nodiscard]] static constexpr bool
		has_name( daw::string_view key ) noexcept {
			using std::begin;
			using std::end;
			auto result = algorithm::find_if(
			  begin( name_map_data ), end( name_map_data ),
			  [key]( auto const &kv ) { return kv.name == key; } );
			return result != std::end( name_map_data );
		}

		[[nodiscard]] static constexpr size_t
		find_name( daw::string_view key ) noexcept {
			using std::begin;
			using std::end;
			auto result = algorithm::find_if(
			  begin( name_map_data ), end( name_map_data ),
			  [key]( auto const &kv ) { return kv.name == key; } );
			if( result == std::end( name_map_data ) ) {
				std::terminate( );
			}
			return static_cast<size_t>(
			  std::distance( begin( name_map_data ), result ) );
		}
	};

	struct location_info_t {
		JSONNAMETYPE name;
		IteratorRange<char const *, char const *> location{};

		[[maybe_unused, nodiscard]] constexpr bool missing( ) const {
			return location.empty( ) or location.is_null( );
		}
	};

	template<size_t JsonMemberPosition, typename... JsonMembers, typename First,
	         typename Last>
	[[nodiscard]] static constexpr json_result_n<JsonMemberPosition,
	                                             JsonMembers...>
	parse_item( std::array<IteratorRange<First, Last>,
	                       sizeof...( JsonMembers )> const &locations ) {

		using JsonMember = traits::nth_type<JsonMemberPosition, JsonMembers...>;
		return parse_value<JsonMember>( ParseTag<JsonMember::expected_type>{},
		                                locations[JsonMemberPosition] );
	}

	template<size_t pos, typename... JsonMembers, typename First, typename Last>
	[[nodiscard]] static constexpr IteratorRange<First, Last> find_location(
	  std::array<location_info_t, sizeof...( JsonMembers )> &locations,
	  IteratorRange<First, Last> &rng ) {

		json_assert(
		  is_json_nullable_v<daw::traits::nth_element<pos, JsonMembers...>> or
		    not locations[pos].missing( ) or not rng.front( '}' ),
		  "Unexpected end of class.  Non-nullable members still not found" );

		rng.trim_left_no_check( );
		while( locations[pos].missing( ) and not rng.in( '}' ) ) {
			auto name = parse_name( rng );
			using name_map = name_map_t<JsonMembers...>;
			if( not name_map::has_name( name ) ) {
				// This is not a member we are concerned with
				(void)skip_value( rng );
				rng.clean_tail( );
				continue;
			}
			auto const name_pos = name_map::find_name( name );
			if( name_pos != pos ) {
				// We are out of order, store position for later
				// TODO:	use type knowledge to speed up skip
				// TODO:	on skipped classes see if way to store
				// 				member positions so that we don't have to
				//				reparse them after
				locations[name_pos].location = skip_value( rng );
				rng.clean_tail( );
				continue;
			}
			locations[pos].location = rng;
		}
		return locations[pos].location;
	}

	template<size_t JsonMemberPosition, typename... JsonMembers, typename First,
	         typename Last>
	[[nodiscard]] static constexpr json_result_n<JsonMemberPosition,
	                                             JsonMembers...>
	parse_item( std::array<location_info_t, sizeof...( JsonMembers )> &locations,
	            IteratorRange<First, Last> &rng ) {

		using JsonMember = traits::nth_type<JsonMemberPosition, JsonMembers...>;

		// If we are an array element
		if constexpr( json_name_eq( JsonMember::name, no_name ) ) {
			auto result =
			  parse_value<JsonMember>( ParseTag<JsonMember::expected_type>{}, rng );
			rng.clean_tail( );
			return result;
		} else {
			auto loc =
			  find_location<JsonMemberPosition, JsonMembers...>( locations, rng );

			constexpr auto const name = JsonMember::name;
			::Unused( name );
			// Only allow missing members for Null-able type
			json_assert( JsonMember::expected_type == JsonParseTypes::Null or
			               not loc.empty( ),
			             "Could not find required class member" );

			if( loc.is_null( ) or
			    ( not rng.is_null( ) and rng.begin( ) != loc.begin( ) ) ) {

				return parse_value<JsonMember>( ParseTag<JsonMember::expected_type>{},
				                                loc );
			} else {
				auto result =
				  parse_value<JsonMember>( ParseTag<JsonMember::expected_type>{}, rng );
				rng.clean_tail( );
				return result;
			}
		}
	}

	template<size_t N, typename... JsonMembers>
	using nth = daw::traits::nth_element<N, JsonMembers...>;

	template<typename... JsonMembers, typename OutputIterator, size_t... Is,
	         typename... Args>
	[[nodiscard]] static constexpr OutputIterator
	serialize_json_class( OutputIterator it, std::index_sequence<Is...>,
	                      std::tuple<Args...> &&args ) {

		*it++ = '{';

		(void)( ( to_json_str<nth<Is, JsonMembers...>, Is>( it, daw::move( args ) ),
		          ... ),
		        0 );

		*it++ = '}';
		return it;
	}

	template<typename Result, typename... JsonMembers, size_t... Is,
	         typename First, typename Last>
	[[nodiscard]] static constexpr Result
	parse_json_class( IteratorRange<First, Last> &rng,
	                  std::index_sequence<Is...> ) {
		static_assert(
		  can_construct_a_v<Result, typename JsonMembers::parse_to_t...>,
		  "Supplied types cannot be used for construction of this type" );

		rng.trim_left_no_check( );
		json_assert( rng.front( '{' ), "Expected class to begin with '{'" );
		rng.remove_prefix( );
		rng.trim_left_no_check( );
		if constexpr( sizeof...( JsonMembers ) == 0 ) {
			return construct_a<Result>( );
			json_assert( rng.front( '}' ), "Expected class to end with '}'" );
			rng.remove_prefix( );
			rng.trim_left( );
		} else {
			constexpr auto cknown_locations =
			  daw::make_array( location_info_t{JsonMembers::name}... );
			auto known_locations = cknown_locations;

			auto result = daw::construct_a<Result>(
			  parse_item<Is, JsonMembers...>( known_locations, rng )... );
			rng.trim_left_no_check( );
			// If we fullfill the contract before all values are found
			while( not rng.in( '}' ) ) {
				(void)parse_name( rng );
				(void)skip_value( rng );
				rng.clean_tail( );
			}

			json_assert( rng.front( '}' ), "Expected class to end with '}'" );
			rng.remove_prefix( );
			rng.trim_left( );
			return result;
		}
	}

	template<typename Result, typename... JsonMembers, size_t... Is>
	[[nodiscard]] static constexpr Result
	parse_json_class( std::string_view sv, std::index_sequence<Is...> is ) {

		auto rng = IteratorRange( sv.data( ), sv.data( ) + sv.size( ) );
		return parse_json_class<Result, JsonMembers...>( rng, is );
	}

	[[nodiscard]] static constexpr bool
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

	template<typename First, typename Last>
	static constexpr void find_range2( IteratorRange<First, Last> &rng,
	                                   daw::string_view path ) {
		auto current = impl::pop_json_path( path );
		while( not current.empty( ) ) {
			json_assert( rng.front( '{' ), "Invalid Path Entry" );
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

	template<typename String>
	[[nodiscard]] static constexpr auto
	find_range( String &&str, daw::string_view start_path ) {

		auto rng =
		  IteratorRange( std::data( str ), std::data( str ) + std::size( str ) );
		if( not start_path.empty( ) ) {
			find_range2( rng, start_path );
		}
		return rng;
	}
} // namespace daw::json::impl
