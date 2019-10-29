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
#include <daw/daw_traits.h>
#include <daw/daw_utility.h>
#include <daw/daw_visit.h>
#include <daw/iso8601/daw_date_formatting.h>
#include <daw/iso8601/daw_date_parsing.h>
#include <daw/iterator/daw_back_inserter.h>
#include <daw/iterator/daw_inserter.h>

#include "daw_bool.h"
#include "daw_iterator_range.h"
#include "daw_json_assert.h"
#include "daw_json_link_base.h"
#include "daw_literal_end_parse.h"
#include "daw_name_parse.h"
#include "daw_parse_numbers.h"
#include "daw_parse_value_fwd.h"
#include "daw_signed_int.h"
#include "daw_string_quote_parse.h"
#include "daw_unsigned_int.h"

namespace daw::json::impl {
	template<typename JsonMember, typename First, typename Last>
	[[nodiscard]] static constexpr auto
	parse_value( ParseTag<JsonParseTypes::Real>, location_info_t &loc ) {

		using constructor_t = typename JsonMember::constructor_t;
		using element_t = typename JsonMember::parse_to_t;

		json_assert( loc.contains<JsonParseTypes::Real>( ), "Unexpected data" );
		return constructor_t{}(
		  static_cast<element_t>( loc.as<JsonParseTypes::Real>( ) ) );
	}

	template<typename JsonMember, typename First, typename Last>
	[[nodiscard]] static constexpr auto
	parse_value( ParseTag<JsonParseTypes::Unsigned>, location_info_t &loc ) {
		using constructor_t = typename JsonMember::constructor_t;
		using element_t = typename JsonMember::parse_to_t;

		json_assert( loc.contains<JsonParseTypes::Unsigned>( ), "Unexpected data" );
		if constexpr( JsonMember::range_check ) {
			return constructor_t{}(
			  daw::narrow_cast<element_t>( loc.as<JsonParseTypes::Unsigned>( ) ) );
		} else {
			return constructor_t{}(
			  static_cast<element_t>( loc.as<JsonParseTypes::Unsigned>( ) ) );
		}
	}

	template<typename JsonMember, typename First, typename Last>
	[[nodiscard]] static constexpr auto
	parse_value( ParseTag<JsonParseTypes::Signed>, location_info_t const &loc ) {
		using constructor_t = typename JsonMember::constructor_t;
		using element_t = typename JsonMember::parse_to_t;

		json_assert( loc.contains<JsonParseTypes::Signed>( ), "Unexpected data" );
		if constexpr( JsonMember::range_check ) {
			return constructor_t{}(
			  daw::narrow_cast<element_t>( loc.as<JsonParseTypes::Signed>( ) ) );
		} else {
			return constructor_t{}(
			  static_cast<element_t>( loc.as<JsonParseTypes::Signed>( ) ) );
		}
	}

	template<typename JsonMember, typename First, typename Last>
	[[nodiscard]] static constexpr auto
	parse_value( ParseTag<JsonParseTypes::Bool>, location_info_t const &loc ) {
		using constructor_t = typename JsonMember::constructor_t;

		json_assert( loc.contains<JsonParseTypes::Bool>( ), "Unexpected data" );

		return constructor_t{}( loc.as<JsonParseTypes::Bool>( ) );
	}

	template<typename JsonMember, typename First, typename Last>
	[[nodiscard]] static constexpr auto
	parse_value( ParseTag<JsonParseTypes::String>, location_info_t const &loc ) {

		// TODO ensure end of rng is valid
		auto const &rng = loc.rng( );
		using constructor_t = typename JsonMember::constructor_t;
		return constructor_t{}( rng.begin( ), rng.size( ) );
	}

	template<typename JsonMember, typename First, typename Last>
	[[nodiscard]] static constexpr auto
	parse_value( ParseTag<JsonParseTypes::Date>, location_info_t const &loc ) {

		auto const &rng = loc.rng( );
		using constructor_t = typename JsonMember::constructor_t;
		return constructor_t{}( rng.begin( ), rng.size( ) );
	}

	template<typename JsonMember, typename First, typename Last>
	[[nodiscard]] static constexpr auto
	parse_value( ParseTag<JsonParseTypes::Custom>, location_info_t const &loc ) {

		auto const &rng = loc.rng( );
		// TODO make custom require a ptr/sz pair
		using constructor_t = typename JsonMember::from_converter_t;
		return constructor_t{}( std::string_view( rng.begin( ), rng.size( ) ) );
	}

	template<typename JsonMember, typename First, typename Last>
	[[nodiscard]] static constexpr auto
	parse_value( ParseTag<JsonParseTypes::Class>, location_info_t loc ) {

		using element_t = typename JsonMember::parse_to_t;

		auto &rng = loc.rng( );
		auto result =
		  json_parser_description_t<element_t>::template parse<element_t>( rng );
		rng.trim_left( );
		return result;
	}

	template<typename JsonMember, typename First, typename Last>
	[[nodiscard]] static constexpr auto
	parse_value( ParseTag<JsonParseTypes::KeyValue>, location_info_t loc ) {
		json_assert(
		  loc.rng( ).front( '{' ),
		  "Expected keyvalue type to be of class type and beging with '{'" );

		loc.rng( ).remove_prefix( );
		loc.rng( ).trim_left_no_check( );

		auto array_container = typename JsonMember::constructor_t{}( );
		auto container_appender =
		  typename JsonMember::appender_t( array_container );

		using key_t = typename JsonMember::json_key_t;
		using value_t = typename JsonMember::json_element_t;
		while( not loc.rng( ).in( "}" ) ) {
			auto key = parse_name( loc.rng( ) );
			loc.rng( ).trim_left_no_check( );
			container_appender(
			  typename key_t::constructor_t{}( key.data( ), key.size( ) ),
			  parse_value<value_t, First, Last>( ParseTag<value_t::expected_type>{},
			                                     loc ) );

			loc.rng( ).clean_tail( );
		}
		json_assert( loc.rng( ).front( '}' ),
		             "Expected keyvalue type to end with a '}'" );
		return array_container;
	}

	template<typename JsonMember, typename First, typename Last>
	[[nodiscard]] static constexpr auto
	parse_value( ParseTag<JsonParseTypes::Array>, location_info_t loc ) {

		using element_t = typename JsonMember::json_element_t;
		json_assert( not loc.empty( ) and loc.contains<JsonParseTypes::Array>( ),
		             "Expected type" );
		auto rng = loc.rng( );
		json_assert( not rng.empty( ) and rng.front( '[' ),
		             "Expected array to start with a '['" );

		rng.remove_prefix( );
		rng.trim_left_no_check( );

		auto array_container = typename JsonMember::constructor_t{}( );
		auto container_appender =
		  typename JsonMember::appender_t( array_container );

		while( rng.front( ) != ']' ) {
			json_assert( not rng.empty( ), "Unexpected end of range" );
			auto item_loc =
			  location_info_t{element_t::name, element_t::expected_type};
			parse_location( item_loc, rng );
			container_appender( parse_value<element_t, First, Last>(
			  ParseTag<element_t::expected_type>{}, item_loc ) );
			rng.clean_tail( );
		}
		json_assert( rng.front( ']' ), "Expected array to end with a ']'" );
		rng.remove_prefix( );
		return array_container;
	}

	template<typename JsonMember, typename First, typename Last>
	[[nodiscard]] static constexpr auto
	parse_value( ParseTag<JsonParseTypes::Null>, location_info_t loc ) {
		using constructor_t = typename JsonMember::constructor_t;
		using element_t = typename JsonMember::sub_type;

		if( loc.empty( ) or loc.rng( ).is_null( ) ) {
			return constructor_t{}( );
		}
		json_assert( loc.contains<JsonParseTypes::Null>( ),
		             "Expected string data to parse" );

		loc.expected_type = element_t::expected_type;
		parse_location<First, Last>( loc );
		return parse_value<element_t, First, Last>(
		  ParseTag<element_t::expected_type>{}, loc );
	}

} // namespace daw::json::impl
