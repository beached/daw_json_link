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

#include <cstddef>
#include <cstdint>

#include "daw_iterator_range.h"
#include "daw_json_assert.h"
#include "daw_json_link_impl.h"
#include "daw_json_parse_array_iterator.h"
#include "daw_json_parse_common.h"
#include "daw_json_parse_name.h"
#include "daw_json_parse_real.h"
#include "daw_json_parse_signed_int.h"
#include "daw_json_parse_string_quote.h"
#include "daw_json_parse_unsigned_int.h"
#include "daw_json_parse_value_fwd.h"

namespace daw::json::impl {
	template<typename JsonMember, typename First, typename Last,
	         bool TrustedInput>
	constexpr void skip_quote_when_literal_as_string(
	  IteratorRange<First, Last, TrustedInput> &rng ) {
		if constexpr( JsonMember::literal_as_string == LiteralAsStringOpt::never ) {
			return;
		} else if constexpr( JsonMember::literal_as_string ==
		                     LiteralAsStringOpt::always ) {
			rng.remove_prefix( );
			return;
		} else {
			if( rng.front( ) == '"' ) {
				rng.remove_prefix( );
			}
			return;
		}
	}

	template<typename JsonMember, typename First, typename Last,
	         bool TrustedInput>
	[[nodiscard]] static constexpr json_result<JsonMember>
	parse_value( ParseTag<JsonParseTypes::Real>,
	             IteratorRange<First, Last, TrustedInput> &rng ) {
		using constructor_t = typename JsonMember::constructor_t;
		using element_t = typename JsonMember::parse_to_t;

		skip_quote_when_literal_as_string<JsonMember>( rng );
		if constexpr( not TrustedInput ) {
			json_assert( rng.is_real_number_part( ),
			             "Expected number to start with on of \"0123456789eE+-\"" );
		}

		auto result = constructor_t{}( parse_real<element_t>( rng ) );
		skip_quote_when_literal_as_string<JsonMember>( rng );
		if constexpr( not TrustedInput ) {
			json_assert( rng.at_end_of_item( ),
			             "Expected whitespace or one of \",}]\" at end of number" );
		}
		return result;
	}

	template<typename JsonMember, typename First, typename Last,
	         bool TrustedInput>
	[[nodiscard]] static constexpr json_result<JsonMember>
	parse_value( ParseTag<JsonParseTypes::Signed>,
	             IteratorRange<First, Last, TrustedInput> &rng ) {
		using constructor_t = typename JsonMember::constructor_t;
		using element_t = typename JsonMember::parse_to_t;

		skip_quote_when_literal_as_string<JsonMember>( rng );
		if constexpr( not TrustedInput ) {
			json_assert( rng.is_real_number_part( ),
			             "Expected number to start with on of \"0123456789eE+-\"" );
		}

		auto result = constructor_t{}(
		  parse_integer<element_t, JsonMember::range_check>( rng ) );
		skip_quote_when_literal_as_string<JsonMember>( rng );
		if constexpr( not TrustedInput ) {
			json_assert( rng.at_end_of_item( ),
			             "Expected whitespace or one of \",}]\" at end of number" );
		}
		return result;
	}

	template<typename JsonMember, typename First, typename Last,
	         bool TrustedInput>
	[[nodiscard]] static constexpr json_result<JsonMember>
	parse_value( ParseTag<JsonParseTypes::Unsigned>,
	             IteratorRange<First, Last, TrustedInput> &rng ) {
		using constructor_t = typename JsonMember::constructor_t;
		using element_t = typename JsonMember::parse_to_t;

		skip_quote_when_literal_as_string<JsonMember>( rng );
		if constexpr( not TrustedInput ) {
			json_assert( rng.is_real_number_part( ),
			             "Expected number to start with on of \"0123456789eE+-\"" );
		}
		auto result = constructor_t{}(
		  parse_unsigned_integer<element_t, JsonMember::range_check>( rng ) );
		skip_quote_when_literal_as_string<JsonMember>( rng );
		if constexpr( not TrustedInput ) {
			json_assert( rng.at_end_of_item( ),
			             "Expected whitespace or one of \",}]\" at end of number" );
		}
		return result;
	}

	template<typename JsonMember, typename First, typename Last,
	         bool TrustedInput>
	[[nodiscard]] static constexpr json_result<JsonMember>
	parse_value( ParseTag<JsonParseTypes::Null>,
	             IteratorRange<First, Last, TrustedInput> &rng ) {

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

	template<typename JsonMember, typename First, typename Last,
	         bool TrustedInput>
	[[nodiscard]] static constexpr json_result<JsonMember>
	parse_value( ParseTag<JsonParseTypes::Bool>,
	             IteratorRange<First, Last, TrustedInput> &rng ) {
		if constexpr( not TrustedInput ) {
			json_assert( not rng.empty( ) and rng.size( ) >= 4,
			             "Range to small to be a bool" );
		}

		using constructor_t = typename JsonMember::constructor_t;

		skip_quote_when_literal_as_string<JsonMember>( rng );
		bool result = false;
		if( rng.in( 't' ) and rng.size( ) > 4 ) {
			rng.remove_prefix( 4 );
			result = true;
		} else {
			rng.remove_prefix( 5 );
		}
		skip_quote_when_literal_as_string<JsonMember>( rng );
		return constructor_t{}( result );
	}

	template<typename JsonMember, typename First, typename Last,
	         bool TrustedInput>
	[[nodiscard]] static constexpr json_result<JsonMember>
	parse_value( ParseTag<JsonParseTypes::String>,
	             IteratorRange<First, Last, TrustedInput> &rng ) {

		auto str = skip_string( rng );
		using constructor_t = typename JsonMember::constructor_t;
		return constructor_t{}( str.begin( ), str.size( ) );
	}

	template<typename JsonMember, typename First, typename Last,
	         bool TrustedInput>
	[[nodiscard]] static constexpr json_result<JsonMember>
	parse_value( ParseTag<JsonParseTypes::Date>,
	             IteratorRange<First, Last, TrustedInput> &rng ) {

		auto str = skip_string( rng );
		using constructor_t = typename JsonMember::constructor_t;
		return constructor_t{}( str.begin( ), str.size( ) );
	}

	template<typename JsonMember, typename First, typename Last,
	         bool TrustedInput>
	[[nodiscard]] static constexpr json_result<JsonMember>
	parse_value( ParseTag<JsonParseTypes::Custom>,
	             IteratorRange<First, Last, TrustedInput> &rng ) {

		if constexpr( not TrustedInput ) {
			json_assert( rng.front( '"' ),
			             "Custom types requite a string at the beginning" );
		}
		rng.remove_prefix( );
		auto str = skip_string_nq( rng );
		// TODO make custom require a ptr/sz pair
		using constructor_t = typename JsonMember::from_converter_t;
		return constructor_t{}( std::string_view( str.begin( ), str.size( ) ) );
	}

	template<typename JsonMember, typename First, typename Last,
	         bool TrustedInput>
	[[nodiscard]] static constexpr json_result<JsonMember>
	parse_value( ParseTag<JsonParseTypes::Class>,
	             IteratorRange<First, Last, TrustedInput> &rng ) {

		using element_t = typename JsonMember::parse_to_t;
		return from_json<element_t>( rng );
	}

	template<typename JsonMember, typename First, typename Last,
	         bool TrustedInput>
	[[nodiscard]] static constexpr json_result<JsonMember>
	parse_value( ParseTag<JsonParseTypes::KeyValue>,
	             IteratorRange<First, Last, TrustedInput> &rng ) {

		if constexpr( not TrustedInput ) {
			json_assert(
			  rng.front( '{' ),
			  "Expected keyvalue type to be of class type and beging with '{'" );
		}

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
			  parse_value<value_t>( ParseTag<value_t::expected_type>{}, rng ) );

			rng.clean_tail( );
		}
		if constexpr( not TrustedInput ) {
			json_assert( rng.front( '}' ),
			             "Expected keyvalue type to end with a '}'" );
		}
		rng.remove_prefix( );
		rng.trim_left( );
		return array_container;
	}

	template<typename JsonMember, typename First, typename Last,
	         bool TrustedInput>
	[[nodiscard]] static constexpr json_result<JsonMember>
	parse_value( ParseTag<JsonParseTypes::Array>,
	             IteratorRange<First, Last, TrustedInput> &rng ) {

		using element_t = typename JsonMember::json_element_t;
		if constexpr( not TrustedInput ) {
			json_assert( rng.front( '[' ), "Expected array to start with a '['" );
		}

		rng.remove_prefix( );
		rng.trim_left_no_check( );
		using container_t = typename JsonMember::parse_to_t;
		if constexpr( std::is_same_v<container_t, basic_appender<container_t>> and
		              std::is_same_v<typename JsonMember::constructor_t,
		                             daw::construct_a_t<container_t>> and
		              is_range_constructable_v<container_t> ) {
			// We are using the default constructor and appender.  This should allow
			// for
			using iterator_t =
			  json_parse_value_array_iterator<JsonMember, First, Last, TrustedInput>;
			return container_t( iterator_t( rng ), iterator_t( ) );
		} else {
			auto array_container = typename JsonMember::constructor_t{}( );
			auto container_appender =
			  typename JsonMember::appender_t( array_container );

			while( rng.front( ) != ']' ) {
				container_appender(
				  parse_value<element_t>( ParseTag<element_t::expected_type>{}, rng ) );
				rng.clean_tail( );
				if constexpr( not TrustedInput ) {
					json_assert( rng.has_more( ), "Unexpected end of data" );
				}
			}
			rng.remove_prefix( );
			rng.trim_left( );
			return array_container;
		}
	}
} // namespace daw::json::impl
