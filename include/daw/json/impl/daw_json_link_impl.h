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

#include "daw_iso8601_utils.h"
#include "daw_iterator_range.h"
#include "daw_json_assert.h"
#include "daw_json_parse_common.h"
#include "daw_json_parse_value.h"
#include "daw_json_to_string.h"
#ifndef _MSC_VER
#include "daw_murmur3.h"
#endif

#include <daw/daw_algorithm.h>
#include <daw/daw_cxmath.h>
#include <daw/daw_parser_helper_sv.h>
#include <daw/daw_scope_guard.h>
#include <daw/daw_sort_n.h>
#include <daw/daw_string_view.h>
#include <daw/daw_traits.h>
#include <daw/daw_utility.h>
#include <daw/iterator/daw_back_inserter.h>
#include <daw/iterator/daw_inserter.h>

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

namespace daw::json {
	template<JsonNullable>
	struct construct_from_iso8601_timestamp;

	template<>
	struct construct_from_iso8601_timestamp<JsonNullable::Never> {
		using result_type = std::chrono::time_point<std::chrono::system_clock,
		                                            std::chrono::milliseconds>;

		[[maybe_unused, nodiscard]] constexpr result_type
		operator( )( char const *ptr, std::size_t sz ) const {
			return datetime::parse_iso8601_timestamp( daw::string_view( ptr, sz ) );
		}
	};

	template<>
	struct construct_from_iso8601_timestamp<JsonNullable::Nullable> {
		using result_type =
		  std::optional<std::chrono::time_point<std::chrono::system_clock,
		                                        std::chrono::milliseconds>>;

		[[maybe_unused, nodiscard]] constexpr result_type operator( )( ) const {
			return {};
		}

		[[maybe_unused, nodiscard]] constexpr result_type
		operator( )( char const *ptr, std::size_t sz ) const {
			return datetime::parse_iso8601_timestamp( daw::string_view( ptr, sz ) );
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

namespace daw::json::json_details {
	[[nodiscard]] constexpr char const *str_find( char const *p, char c ) {
		while( *p != c ) {
			++p;
		}
		return p;
	}

	template<typename Container>
	struct basic_kv_appender {
		Container *m_container;

		constexpr basic_kv_appender( Container &container )
		  : m_container( &container ) {}

		template<typename Key, typename Value>
		constexpr void operator( )( Key &&key, Value &&value ) {
			m_container->insert( std::make_pair( std::forward<Key>( key ),
			                                     std::forward<Value>( value ) ) );
		}
	};

	template<typename string_t>
	struct kv_t {
		string_t name;
		std::size_t pos;
		JsonParseTypes expected_type;

		constexpr kv_t( string_t Name, JsonParseTypes Expected, std::size_t Pos )
		  : name( daw::move( Name ) )
		  , pos( Pos )
		  , expected_type( Expected ) {}
	};

	template<std::size_t N, typename string_t, typename... JsonMembers>
	[[nodiscard]] constexpr kv_t<string_t> get_item( ) {
		using type_t = traits::nth_type<N, JsonMembers...>;
		return kv_t<string_t>( type_t::name, type_t::expected_type, N );
	}

	template<typename First, typename Last, bool IsUnCheckedInput>
	struct location_info_t {
		daw::string_view name;
#ifndef _MSC_VER
		std::uint32_t hash_value;
#endif
		IteratorRange<First, Last, IsUnCheckedInput> location{};

#ifndef _MSC_VER
		explicit constexpr location_info_t( daw::string_view Name ) noexcept
		  : name( Name )
		  , hash_value( daw::murmur3_32( Name ) ) {}
#else
		explicit constexpr location_info_t( daw::string_view Name ) noexcept
		  : name( Name ) {}
#endif

		[[maybe_unused, nodiscard]] constexpr bool missing( ) const {
			return location.is_null( );
		}
	};

	template<std::size_t N, typename First, typename Last, bool IsUnCheckedInput>
	struct locations_info_t {
		using value_type = location_info_t<First, Last, IsUnCheckedInput>;
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

		constexpr location_info_t<First, Last, IsUnCheckedInput> const &
		operator[]( std::size_t idx ) const {
			return names[idx];
		}

		constexpr location_info_t<First, Last, IsUnCheckedInput> &
		operator[]( std::size_t idx ) {
			return names[idx];
		}

		static constexpr std::size_t size( ) noexcept {
			return N;
		}

		[[nodiscard]] constexpr std::size_t
		find_name( daw::string_view key ) const {
#ifdef _MSC_VER
			// Bug in MSVC is making the constexpr string creation not work
			return algorithm::find_index_of_if(
			  begin( ), end( ),
			  [key]( auto const &loc ) { return loc.name == key; } );
#else
			return algorithm::find_index_of_if(
			  begin( ), end( ),
			  [&, hash = daw::murmur3_32( key )]( auto const &loc ) {
				  return loc.hash_value == hash and loc.name == key;
			  } );
#endif
		}
	}; // namespace daw::json::json_details

	template<typename JsonMember, std::size_t N, typename First, typename Last,
	         bool IsUnCheckedInput>
	[[nodiscard]] constexpr IteratorRange<First, Last, IsUnCheckedInput>
	find_class_member(
	  std::size_t pos,
	  locations_info_t<N, First, Last, IsUnCheckedInput> &locations,
	  IteratorRange<First, Last, IsUnCheckedInput> &rng ) {

		daw_json_assert_weak(
		  ( is_json_nullable_v<JsonMember> or not locations[pos].missing( ) or
		    not rng.front( '}' ) ),
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
				// Using locations to switch on BaseType is slower too
				locations[name_pos].location = skip_value( rng );
				rng.clean_tail( );
				continue;
			}
			locations[pos].location = rng;
		}
		return locations[pos].location;
	}

	template<typename JsonMember, typename First, typename Last,
	         bool IsUnCheckedInput>
	[[nodiscard]] constexpr auto parse_ordered_class_member(
	  std::size_t &member_position,
	  IteratorRange<First, Last, IsUnCheckedInput> &rng ) {

		if constexpr( is_an_ordered_member_v<JsonMember> ) {
			daw_json_assert( member_position <= JsonMember::member_index,
			                 "Order of ordered members must be ascending" );
			rng.clean_tail( );
			for( ; ( member_position < JsonMember::member_index ) and
			       ( rng.front( ) != ']' );
			     ++member_position ) {
				(void)skip_value( rng );
				rng.clean_tail( );
			}
		}
		using json_member_type = ordered_member_subtype_t<JsonMember>;

		rng.clean_tail( );
		++member_position;
		if( rng.front( ) == ']' ) {
			auto rng2 = IteratorRange<First, Last, IsUnCheckedInput>{};
			return parse_value<json_member_type>(
			  ParseTag<json_member_type::expected_type>{}, rng2 );
		}
		return parse_value<json_member_type>(
		  ParseTag<json_member_type::expected_type>{}, rng );
	}

	template<typename JsonMember, std::size_t N, typename First, typename Last,
	         bool IsUnCheckedInput>
	[[nodiscard]] constexpr json_result<JsonMember> parse_class_member(
	  std::size_t member_position,
	  locations_info_t<N, First, Last, IsUnCheckedInput> &locations,
	  IteratorRange<First, Last, IsUnCheckedInput> &rng ) {

		rng.clean_tail( );
		if constexpr( is_no_name<JsonMember::name> ) {
			// If we are an array element
			return parse_value<JsonMember>( ParseTag<JsonMember::expected_type>{},
			                                rng );
		} else {
			daw_json_assert_weak( rng.front( "\"}" ),
			                      "Expected end of class or start of member" );
			auto loc =
			  find_class_member<JsonMember>( member_position, locations, rng );

			daw_json_assert_weak(
			  ( is_json_nullable_v<JsonMember> or not loc.is_null( ) ),
			  missing_member( JsonMember::name ) );

			if( loc.is_null( ) or
			    ( not rng.is_null( ) and rng.begin( ) != loc.begin( ) ) ) {

				return parse_value<JsonMember>( ParseTag<JsonMember::expected_type>{},
				                                loc );
			}
			return parse_value<JsonMember>( ParseTag<JsonMember::expected_type>{},
			                                rng );
		}
	}

	template<std::size_t N, typename... JsonMembers>
	using nth = daw::traits::nth_element<N, JsonMembers...>;

	template<typename... JsonMembers, typename OutputIterator, std::size_t... Is,
	         typename Tuple, typename Value>
	[[nodiscard]] constexpr OutputIterator
	serialize_json_class( OutputIterator it, std::index_sequence<Is...>,
	                      Tuple const &args, Value const &value ) {

		bool is_first = true;
		*it++ = '{';

		daw::bounded_vector_t<daw::string_view, sizeof...( JsonMembers ) * 2U>
		  visited_members{};
		// Tag Members, if any
		(void)( ( tags_to_json_str<Is, nth<Is, JsonMembers...>>(
		            is_first, it, value, visited_members ),
		          ... ),
		        0 );
		// Regular Members
		(void)( ( to_json_str<Is, nth<Is, JsonMembers...>>(
		            is_first, it, args, value, visited_members ),
		          ... ),
		        0 );

		*it++ = '}';
		return it;
	}

	template<typename... JsonMembers, typename OutputIterator, typename Tuple,
	         typename Value, std::size_t... Is>
	[[nodiscard]] constexpr OutputIterator
	serialize_ordered_json_class( OutputIterator it, std::index_sequence<Is...>,
	                              Tuple const &args, Value const &value ) {

		*it++ = '[';
		size_t array_idx = 0;
		Unused( value );
		(void)std::array{
		  ( to_json_ordered_str<Is, nth<Is, JsonMembers...>>( array_idx, it, args ),
		    0 )...};

		*it++ = ']';
		return it;
	}

	/***
	 * This is here to force constant evaluation prior to C++20.
	 */
	template<typename First, typename Last, bool IsUnCheckedInput,
	         typename... JsonMembers>
	static inline constexpr auto known_locations_v =
	  locations_info_t<sizeof...( JsonMembers ), First, Last, IsUnCheckedInput>{
	    location_info_t<First, Last, IsUnCheckedInput>( JsonMembers::name )...};

	template<typename JsonClass, typename... JsonMembers, std::size_t... Is,
	         typename First, typename Last, bool IsUnCheckedInput>
	[[nodiscard]] constexpr JsonClass
	parse_json_class( IteratorRange<First, Last, IsUnCheckedInput> &rng,
	                  std::index_sequence<Is...> ) {
		static_assert( has_json_data_contract_trait_v<JsonClass>,
		               "Unexpected type" );
		static_assert(
		  can_construct_a_v<JsonClass, typename JsonMembers::parse_to_t...>,
		  "Supplied types cannot be used for	construction of this type" );

		rng.move_to_next_of( '{' );
		rng.class_first = rng.first;
		rng.remove_prefix( );
		rng.move_to_next_of( "\"}" );

		auto const clean_up_fn = [&] {
			rng.clean_tail( );
			// If we fullfill the contract before all values are parses
			while( rng.front( ) != '}' ) {
				(void)parse_name( rng );
				(void)skip_value( rng );
				rng.clean_tail( );
			}

			daw_json_assert_weak( rng.front( ) == '}',
			                      "Expected class to end with '}'" );
			rng.remove_prefix( );
			rng.trim_left( );
		};
		if constexpr( sizeof...( JsonMembers ) == 0 ) {
			clean_up_fn( );
			return daw::construct_a<JsonClass>( );
		} else {
			auto known_locations =
			  known_locations_v<First, Last, IsUnCheckedInput, JsonMembers...>;

			using tp_t = std::tuple<decltype(
			  parse_class_member<traits::nth_type<Is, JsonMembers...>>(
			    Is, known_locations, rng ) )...>;

#if defined( __cpp_constexpr_dynamic_alloc ) or                                \
  defined( DAW_JSON_NO_CONST_EXPR )
			// This relies on non-trivial dtor's being allowed.  So C++20 constexpr or
			// not in a constant expression.  It does allow for construction of
			// classes without move/copy special members

			// Do this before we exit but after return
			auto const oe = daw::on_exit_success( std::move( clean_up_fn ) );
			/*
			 * Rather than call directly use apply/tuple to evaluate left->right
			 */
			return std::apply(
			  daw::construct_a<JsonClass>,
			  tp_t{parse_class_member<traits::nth_type<Is, JsonMembers...>>(
			    Is, known_locations, rng )...} );
#else
			auto result = std::apply(
			  daw::construct_a<JsonClass>,
			  tp_t{parse_class_member<traits::nth_type<Is, JsonMembers...>>(
			    Is, known_locations, rng )...} );
			clean_up_fn( );
			return result;
#endif
		}
	}

	template<typename JsonClass, typename... JsonMembers, typename First,
	         typename Last, bool IsUnCheckedInput>
	[[nodiscard]] constexpr JsonClass parse_ordered_json_class(
	  IteratorRange<First, Last, IsUnCheckedInput> &rng ) {
		static_assert( has_json_data_contract_trait_v<JsonClass>,
		               "Unexpected type" );
		static_assert(
		  can_construct_a_v<JsonClass, typename JsonMembers::parse_to_t...>,
		  "Supplied types cannot be used for	construction of this type" );

		rng.move_to_next_of( '[' );
		rng.remove_prefix( );
		rng.trim_left( );
		/*
		 * Rather than call directly use apply/tuple to evaluate left->right
		 */
		size_t current_idx = 0;
		JsonClass result =
		  std::apply( daw::construct_a<JsonClass>,
		              std::tuple{parse_ordered_class_member<JsonMembers>(
		                current_idx, rng )...} );

		// TODO investigate using on_successful_exit to do this
		rng.clean_tail( );
		if constexpr( true ) { // ignore_trailing_elements_v<JsonClass> ) {
			while( rng.front( ) != ']' ) {
				(void)skip_value( rng );
				rng.clean_tail( );
			}
		} else {
			daw_json_assert_weak( rng.front( ) == ']', "Must specify all members" );
		}
		rng.remove_prefix( );
		rng.trim_left( );
		return result;
	}
} // namespace daw::json::json_details
