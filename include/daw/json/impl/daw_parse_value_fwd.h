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

#include "daw_iterator_range.h"
#include "daw_json_link_base.h"

namespace daw::json::impl {
	template<typename First, typename Last>
	static constexpr void parse_location( location_info_t &location,
	                                      IteratorRange<First, Last> &rng );

	template<typename First, typename Last>
	static constexpr void parse_location( location_info_t &location );

	template<typename JsonMember, typename First, typename Last>
	[[nodiscard]] static constexpr auto
	parse_value( ParseTag<JsonParseTypes::Real>, location_info_t &loc );

	template<typename JsonMember, typename First, typename Last>
	[[nodiscard]] static constexpr auto
	parse_value( ParseTag<JsonParseTypes::Real>,
	             IteratorRange<First, Last> &rng );

	template<typename JsonMember, typename First, typename Last>
	[[nodiscard]] static constexpr auto
	parse_value( ParseTag<JsonParseTypes::Unsigned>, location_info_t &loc );

	template<typename JsonMember, typename First, typename Last>
	[[nodiscard]] static constexpr auto
	parse_value( ParseTag<JsonParseTypes::Unsigned>,
	             IteratorRange<First, Last> &rng );

	template<typename JsonMember, typename First, typename Last>
	[[nodiscard]] static constexpr auto
	parse_value( ParseTag<JsonParseTypes::Signed>, location_info_t const &loc );

	template<typename JsonMember, typename First, typename Last>
	[[nodiscard]] static constexpr auto
	parse_value( ParseTag<JsonParseTypes::Signed>,
	             IteratorRange<First, Last> &rng );

	template<typename JsonMember, typename First, typename Last>
	[[nodiscard]] static constexpr auto
	parse_value( ParseTag<JsonParseTypes::Bool>, location_info_t const &loc );

	template<typename JsonMember, typename First, typename Last>
	[[nodiscard]] static constexpr auto
	parse_value( ParseTag<JsonParseTypes::Bool>,
	             IteratorRange<First, Last> &rng );

	template<typename JsonMember, typename First, typename Last>
	[[nodiscard]] static constexpr auto
	parse_value( ParseTag<JsonParseTypes::String>, location_info_t const &loc );

	template<typename JsonMember, typename First, typename Last>
	[[nodiscard]] static constexpr auto
	parse_value( ParseTag<JsonParseTypes::String>,
	             IteratorRange<First, Last> &rng );

	template<typename JsonMember, typename First, typename Last>
	[[nodiscard]] static constexpr auto
	parse_value( ParseTag<JsonParseTypes::Date>, location_info_t const &loc );

	template<typename JsonMember, typename First, typename Last>
	[[nodiscard]] static constexpr auto
	parse_value( ParseTag<JsonParseTypes::Date>,
	             IteratorRange<First, Last> &rng );

	template<typename JsonMember, typename First, typename Last>
	[[nodiscard]] static constexpr auto
	parse_value( ParseTag<JsonParseTypes::Custom>, location_info_t const &loc );

	template<typename JsonMember, typename First, typename Last>
	[[nodiscard]] static constexpr auto
	parse_value( ParseTag<JsonParseTypes::Custom>,
	             IteratorRange<First, Last> &rng );

	template<typename JsonMember, typename First, typename Last>
	[[nodiscard]] static constexpr auto
	parse_value( ParseTag<JsonParseTypes::Class>, location_info_t loc );

	template<typename JsonMember, typename First, typename Last>
	[[nodiscard]] static constexpr auto
	parse_value( ParseTag<JsonParseTypes::Class>,
	             IteratorRange<First, Last> &rng );

	template<typename JsonMember, typename First, typename Last>
	[[nodiscard]] static constexpr auto
	parse_value( ParseTag<JsonParseTypes::KeyValue>, location_info_t loc );

	template<typename JsonMember, typename First, typename Last>
	[[nodiscard]] static constexpr auto
	parse_value( ParseTag<JsonParseTypes::KeyValue>,
	             IteratorRange<First, Last> &rng );

	template<typename JsonMember, typename First, typename Last>
	[[nodiscard]] static constexpr auto
	parse_value( ParseTag<JsonParseTypes::Array>, location_info_t loc );

	template<typename JsonMember, typename First, typename Last>
	[[nodiscard]] static constexpr auto
	parse_value( ParseTag<JsonParseTypes::Array>,
	             IteratorRange<First, Last> &rng );

	template<typename JsonMember, typename First, typename Last>
	[[nodiscard]] static constexpr auto
	parse_value( ParseTag<JsonParseTypes::Null>, location_info_t loc );

	template<typename JsonMember, typename First, typename Last>
	[[nodiscard]] static constexpr auto
	parse_value( ParseTag<JsonParseTypes::Null>,
	             IteratorRange<First, Last> &rng );

	template<typename Container>
	struct basic_appender {
		daw::back_inserter_iterator<Container> appender;

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

	template<typename First, typename Last>
	[[nodiscard]] static constexpr daw::string_view
	parse_name( IteratorRange<First, Last> &rng );
} // namespace daw::json::impl
