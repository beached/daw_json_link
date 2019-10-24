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
	parse_value( ParseTag<JsonParseTypes::Real>,
	             IteratorRange<First, Last> &rng ) {
		using constructor_t = typename JsonMember::constructor_t;
		using element_t = typename JsonMember::parse_to_t;

		json_assert( not rng.empty( ), "Unexpected data" );

		auto result = constructor_t{}( parse_real<element_t>( rng ) );
		json_assert( rng.at_end_of_item( ),
		             "Expected whitespace or one of \",}]\" at end of number" );
		return result;
	}

	template<typename JsonMember, typename First, typename Last>
	[[nodiscard]] static constexpr auto
	parse_value( ParseTag<JsonParseTypes::Unsigned>,
	             IteratorRange<First, Last> &rng ) {
		using constructor_t = typename JsonMember::constructor_t;
		using element_t = typename JsonMember::parse_to_t;

		json_assert( not rng.empty( ), "Unexpected data" );

		if constexpr( JsonMember::range_check ) {
			auto result = constructor_t{}( daw::narrow_cast<element_t>(
			  parse_unsigned_integer<uintmax_t>( rng ) ) );
			json_assert( rng.at_end_of_item( ),
			             "Expected whitespace or one of \",}]\" at end of number" );
			return result;
		} else {
			auto result = constructor_t{}(
			  static_cast<element_t>( parse_unsigned_integer<uintmax_t>( rng ) ) );
			json_assert( rng.at_end_of_item( ),
			             "Expected whitespace or one of \",}]\" at end of number" );
			return result;
		}
	}

	template<typename JsonMember, typename First, typename Last>
	[[nodiscard]] static constexpr auto
	parse_value( ParseTag<JsonParseTypes::Signed>,
	             IteratorRange<First, Last> &rng ) {
		using constructor_t = typename JsonMember::constructor_t;
		using element_t = typename JsonMember::parse_to_t;

		json_assert( not rng.empty( ), "Unexpected data" );

		if constexpr( JsonMember::range_check ) {
			auto result = constructor_t{}(
			  daw::narrow_cast<element_t>( parse_integer<intmax_t>( rng ) ) );
			json_assert( rng.at_end_of_item( ),
			             "Expected whitespace or one of \",}]\" at end of number" );
			return result;
		} else {
			auto result = constructor_t{}( parse_integer<element_t>( rng ) );
			json_assert( rng.at_end_of_item( ),
			             "Expected whitespace or one of \",}]\" at end of number" );
			return result;
		}
	}

	template<typename JsonMember, typename First, typename Last>
	[[nodiscard]] static constexpr auto
	parse_value( ParseTag<JsonParseTypes::Bool>,
	             IteratorRange<First, Last> &rng ) {
		using constructor_t = typename JsonMember::constructor_t;

		json_assert( not rng.empty( ), "Unexpected empty range" );
		auto [v, ptr] =
		  daw::json::impl::parse_bool::bool_parser::parse( rng.first );
		rng.first = ptr;
		json_assert( rng.at_end_of_item( ),
		             "Expected whitespace or one of \",}]\" at end of number" );
		return constructor_t{}( v );
	}

	template<typename JsonMember, typename First, typename Last>
	[[nodiscard]] static constexpr auto
	parse_value( ParseTag<JsonParseTypes::String>,
	             IteratorRange<First, Last> &rng ) {

		auto str = skip_string( rng );
		json_assert( rng.at_end_of_item( ),
		             "Expected whitespace or one of \",}]\" at end of number" );
		using constructor_t = typename JsonMember::constructor_t;
		return constructor_t{}( str.begin( ), str.size( ) );
	}

	template<typename JsonMember, typename First, typename Last>
	[[nodiscard]] static constexpr auto
	parse_value( ParseTag<JsonParseTypes::Date>,
	             IteratorRange<First, Last> &rng ) {

		auto str = skip_string( rng );
		using constructor_t = typename JsonMember::constructor_t;
		return constructor_t{}( str.begin( ), str.size( ) );
	}

	template<typename JsonMember, typename First, typename Last>
	[[nodiscard]] static constexpr auto
	parse_value( ParseTag<JsonParseTypes::Custom>,
	             IteratorRange<First, Last> &rng ) {

		auto str = skip_string( rng );
		// TODO make custom require a ptr/sz pair
		using constructor_t = typename JsonMember::from_converter_t;
		return constructor_t{}( std::string_view( str.begin( ), str.size( ) ) );
	}

	template<typename JsonMember, typename First, typename Last>
	[[nodiscard]] static constexpr auto
	parse_value( ParseTag<JsonParseTypes::Class>,
	             IteratorRange<First, Last> &rng ) {

		using element_t = typename JsonMember::parse_to_t;
		return from_json<element_t>( rng );
	}

	template<typename JsonMember, typename First, typename Last>
	[[nodiscard]] static constexpr auto
	parse_value( ParseTag<JsonParseTypes::KeyValue>,
	             IteratorRange<First, Last> &rng ) {
		json_assert(
		  rng.front( '{' ),
		  "Expected keyvalue type to be of class type and beging with '{'" );

		rng.remove_prefix( );
		rng.trim_left_no_check( );

		auto array_container = typename JsonMember::constructor_t{}( );
		auto container_appender =
		  typename JsonMember::appender_t( array_container );

		using key_t = typename JsonMember::json_key_t;
		using value_t = typename JsonMember::json_element_t;
		while( not rng.in( "}" ) ) {
			auto key = parse_name( rng );
			rng.trim_left_no_check( );
			container_appender(
			  typename key_t::constructor_t{}( key.data( ), key.size( ) ),
			  parse_value<value_t, First, Last>( ParseTag<value_t::expected_type>{},
			                                     rng ) );

			rng.clean_tail( );
		}
		json_assert( rng.front( '}' ), "Expected keyvalue type to end with a '}'" );
		return array_container;
	}

	template<typename JsonMember, typename First, typename Last>
	[[nodiscard]] static constexpr auto
	parse_value( ParseTag<JsonParseTypes::Array>,
	             IteratorRange<First, Last> &rng ) {

		using element_t = typename JsonMember::json_element_t;
		json_assert( not rng.empty( ) and rng.front( '[' ),
		             "Expected array to start with a '['" );

		rng.remove_prefix( );
		rng.trim_left_no_check( );

		auto array_container = typename JsonMember::constructor_t{}( );
		auto container_appender =
		  typename JsonMember::appender_t( array_container );

		while( ( static_cast<unsigned>( rng.front( ) ) -
		         static_cast<unsigned>( ']' ) ) != 0 ) {
			auto item_loc =
			  location_info_t{element_t::name, element_t::expected_type};
			parse_location( item_loc, rng );
			container_appender( parse_value<element_t, First, Last>(
			  ParseTag<element_t::expected_type>{}, item_loc ) );
			rng.clean_tail( );
		}
		json_assert( rng.front( ']' ), "Expected array to end with a ']'" );
		return array_container;
	}

	template<typename JsonMember, typename First, typename Last>
	[[nodiscard]] static constexpr auto
	parse_value( ParseTag<JsonParseTypes::Null>,
	             IteratorRange<First, Last> &rng ) {
		using constructor_t = typename JsonMember::constructor_t;
		using element_t = typename JsonMember::sub_type;
		size_t const null_size = 4;
		if( rng.empty( ) or rng.is_null( ) ) {
			return constructor_t{}( );
		}
		if( rng.size( ) >= null_size and rng.in( 'n' ) ) {
			rng.remove_prefix( null_size );
			rng.trim_left( );
			return constructor_t{}( );
		}

		return parse_value<element_t>( ParseTag<element_t::expected_type>{}, rng );
	}
} // namespace daw::json::impl
