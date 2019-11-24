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
#include <daw/daw_cxmath.h>
#include <daw/daw_parser_helper_sv.h>
#include <daw/daw_string_view.h>
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

	template<typename JsonType>
	static inline constexpr bool is_json_nullable_v =
	  JsonType::expected_type == JsonParseTypes::Null;

	struct member_name_parse_error {};

	// Get the next member name
	// Assumes that the current item in stream is a double quote
	// Ensures that the stream is left at the position of the associated
	// value(e.g after the colon(:) and trimmed)
	template<typename First, typename Last, bool TrustedInput>
	[[nodiscard]] static constexpr daw::string_view
	parse_name( IteratorRange<First, Last, TrustedInput> &rng ) {
		daw_json_assert_untrusted( rng.front( '"' ),
		                           "Expected name to start with a quote" );
		rng.remove_prefix( );
		return daw::json::impl::name::name_parser::parse_nq( rng );
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
	[[nodiscard]] static constexpr kv_t<string_t> get_item( ) noexcept {
		using type_t = traits::nth_type<N, JsonMembers...>;
		return kv_t<string_t>( type_t::name, type_t::expected_type, N );
	}

	template<typename... JsonMembers, size_t... Is>
	[[nodiscard]] static constexpr auto
	make_map( std::index_sequence<Is...> ) noexcept {
		using string_t = daw::string_view;

		return daw::make_array( get_item<Is, string_t, JsonMembers...>( )... );
	}

	template<typename... JsonMembers>
	struct name_map_t {
		static constexpr auto name_map_data =
		  make_map<JsonMembers...>( std::index_sequence_for<JsonMembers...>{} );

		[[nodiscard]] static constexpr size_t size( ) noexcept {
			return sizeof...( JsonMembers );
		}

		[[nodiscard]] static constexpr size_t
		find_name( daw::string_view key ) noexcept {
			using std::begin;
			using std::end;
			auto result = algorithm::find_if(
			  begin( name_map_data ), end( name_map_data ),
			  [key]( auto const &kv ) { return kv.name == key; } );

			return static_cast<size_t>(
			  std::distance( begin( name_map_data ), result ) );
		}
	};

	template<typename First, typename Last, bool TrustedInput>
	struct location_info_t {
		daw::string_view name;
		IteratorRange<First, Last, TrustedInput> location{};

		explicit constexpr location_info_t( daw::string_view Name ) noexcept
		  : name( Name ) {}

		[[maybe_unused, nodiscard]] constexpr bool missing( ) const {
			return location.is_null( );
		}
	};

	template<size_t pos, typename... JsonMembers, typename First, typename Last,
	         bool TrustedInput>
	[[nodiscard]] static constexpr IteratorRange<First, Last, TrustedInput>
	find_class_member( std::array<location_info_t<First, Last, TrustedInput>,
	                              sizeof...( JsonMembers )> &locations,
	                   IteratorRange<First, Last, TrustedInput> &rng ) {

		daw_json_assert_untrusted(
		  is_json_nullable_v<daw::traits::nth_element<pos, JsonMembers...>> or
		    not locations[pos].missing( ) or not rng.front( '}' ),
		  "Unexpected end of class.  Non-nullable members still not found" );

		rng.trim_left_no_check( );
		while( locations[pos].missing( ) and rng.front( ) != '}' ) {
			using name_map = name_map_t<JsonMembers...>;
			auto const name = parse_name( rng );
			auto const name_pos = name_map::find_name( name );
			if( name_pos >= name_map::size( ) ) {
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

	template<size_t JsonMemberPosition, typename... JsonMembers, typename First,
	         typename Last, bool TrustedInput>
	[[nodiscard]] static constexpr json_result_n<JsonMemberPosition,
	                                             JsonMembers...>
	parse_class_member( std::array<location_info_t<First, Last, TrustedInput>,
	                               sizeof...( JsonMembers )> &locations,
	                    IteratorRange<First, Last, TrustedInput> &rng ) {

		using JsonMember = traits::nth_type<JsonMemberPosition, JsonMembers...>;

		rng.clean_tail( );
		if constexpr( is_no_name<JsonMember::name> ) {
			// If we are an array element
			return parse_value<JsonMember>( ParseTag<JsonMember::expected_type>{},
			                                rng );
		} else {
			daw_json_assert_untrusted( rng.front( "\"}" ),
			                           "Expected end of class or start of member" );
			auto loc =
			  find_class_member<JsonMemberPosition, JsonMembers...>( locations, rng );

			daw_json_assert_untrusted(
			  JsonMember::expected_type == JsonParseTypes::Null or not loc.is_null( ),
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
	         typename... Args>
	[[nodiscard]] static constexpr OutputIterator
	serialize_json_class( OutputIterator it, std::index_sequence<Is...>,
	                      std::tuple<Args...> const &args ) {

		*it++ = '{';

		(void)( ( to_json_str<nth<Is, JsonMembers...>, Is>( it, args ), ... ), 0 );

		*it++ = '}';
		return it;
	}
	template<typename First, typename Last, bool TrustedInput,
	         typename... JsonMembers>
	inline constexpr auto known_locations_v = daw::make_array(
	  location_info_t<First, Last, TrustedInput>( JsonMembers::name )... );

	template<typename Result, typename... JsonMembers, size_t... Is,
	         typename First, typename Last, bool TrustedInput>
	[[nodiscard]] static constexpr Result
	parse_json_class( IteratorRange<First, Last, TrustedInput> &rng,
	                  std::index_sequence<Is...> ) {
		static_assert(
		  can_construct_a_v<Result, typename JsonMembers::parse_to_t...>,
		  "Supplied types cannot be used for construction of this type" );

		rng.move_to_next_of( '{' );
		rng.remove_prefix( );
		rng.move_to_next_of( "\"}" );
		if constexpr( sizeof...( JsonMembers ) == 0 ) {
			// We are an empty class
			daw_json_assert_untrusted( rng.front( '}' ),
			                           "Expected class to end with '}'" );
			rng.remove_prefix( );
			rng.trim_left( );
			return construct_a<Result>( );
		} else {
			auto known_locations =
			  known_locations_v<First, Last, TrustedInput, JsonMembers...>;

			Result result = daw::construct_a<Result>(
			  parse_class_member<Is, JsonMembers...>( known_locations, rng )... );
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

	template<typename First, typename Last, bool TrustedInput>
	static constexpr void
	find_range2( IteratorRange<First, Last, TrustedInput> &rng,
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

	template<bool TrustedInput, typename String>
	[[nodiscard]] static constexpr auto
	find_range( String &&str, daw::string_view start_path ) {

		auto rng = IteratorRange<char const *, char const *, TrustedInput>(
		  std::data( str ), std::data( str ) + std::size( str ) );
		if( not start_path.empty( ) ) {
			find_range2( rng, start_path );
		}
		daw_json_assert( rng.front( ) == '[', "Expected start of json array" );
		return rng;
	}

	template<typename T, bool TrustedInput>
	[[maybe_unused, nodiscard]] constexpr T
	from_json_impl( std::string_view json_data ) {
		static_assert( impl::has_json_parser_description_v<T>,
		               "A function call describe_json_class must exist for type." );
		daw_json_assert_untrusted( not json_data.empty( ),
		                           "Attempt to parse empty string" );
		using desc_t = impl::json_parser_description_t<T>;

		return desc_t::template parse<T, TrustedInput>( json_data );
	}

} // namespace daw::json::impl
