// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, version 1.0. (see accompanying
// file license or copy at http://www.boost.org/license_1_0.txt)
//
// Official repository: https://github.com/beached/daw_json_link
//

#pragma once

#include "daw_from_json_fwd.h"
#include "impl/daw_json_link_impl.h"
#include "impl/daw_json_link_types_fwd.h"
#include "impl/daw_json_parse_policy.h"
#include "impl/daw_json_serialize_impl.h"
#include "impl/daw_json_traits.h"
#include "impl/daw_json_value.h"

#include <daw/daw_array.h>
#include <daw/daw_bounded_string.h>
#include <daw/daw_parser_helper_sv.h>
#include <daw/daw_traits.h>
#include <daw/daw_utility.h>
#include <daw/daw_visit.h>
#include <daw/iterator/daw_back_inserter.h>

#include <array>
#include <iterator>
#include <memory>
#include <string>
#include <string_view>
#include <tuple>

namespace daw::json {
	/**
	 * Construct the JSONMember from the JSON document argument.
	 * @tparam JsonMember any bool, arithmetic, string, string_view,
	 * daw::json::json_data_contract
	 * @param json_data JSON string data
	 * @tparam KnownBounds The bounds of the json_data are known to contain the
	 * whole value
	 * @return A reified T constructed from JSON data
	 * @throws daw::json::json_exception
	 */
	template<typename JsonMember, typename ParsePolicy, bool KnownBounds,
	         typename Result>
	[[maybe_unused, nodiscard]] constexpr Result
	from_json( std::string_view json_data ) {
		using json_member = json_details::unnamed_default_type_mapping<JsonMember>;
		daw_json_assert( not json_data.empty( ), ErrorReason::EmptyJSONDocument );
		using json_member = json_details::unnamed_default_type_mapping<JsonMember>;
		static_assert(
		  json_details::has_unnamed_default_type_mapping_v<JsonMember>,
		  "Missing specialization of daw::json::json_data_contract for class "
		  "mapping or specialization of daw::json::json_link_basic_type_map" );
		auto rng =
		  ParsePolicy( std::data( json_data ), daw::data_end( json_data ) );

		return json_details::parse_value<json_member, KnownBounds>(
		  ParseTag<json_member::expected_type>{ }, rng );
	}

	/**
	 * Construct the JSONMember from the JSON document argument.
	 * @tparam JsonMember any bool, arithmetic, string, string_view,
	 * daw::json::json_data_contract
	 * @param json_data JSON string data
	 * @tparam KnownBounds The bounds of the json_data are known to contain the
	 * whole value
	 * @return A reified T constructed from JSON data
	 * @throws daw::json::json_exception
	 */
	template<typename JsonMember, typename ParsePolicy, bool KnownBounds,
	         typename Result, typename Allocator>
	[[maybe_unused, nodiscard]] constexpr Result
	from_json_alloc( std::string_view json_data, Allocator const &alloc ) {
		using json_member = json_details::unnamed_default_type_mapping<JsonMember>;
		daw_json_assert( not json_data.empty( ), ErrorReason::EmptyJSONDocument );
		using json_member = json_details::unnamed_default_type_mapping<JsonMember>;
		static_assert(
		  json_details::has_unnamed_default_type_mapping_v<JsonMember>,
		  "Missing specialization of daw::json::json_data_contract for class "
		  "mapping or specialization of daw::json::json_link_basic_type_map" );
		using json_member = json_details::unnamed_default_type_mapping<JsonMember>;
		// static_assert( json_details::is_allocator_v<Allocator> );
		char const *f = std::data( json_data );
		char const *l = daw::data_end( json_data );
		Allocator a = alloc;
		auto rng = ParsePolicy::with_allocator( f, l, a );

		return json_details::parse_value<json_member, KnownBounds>(
		  ParseTag<json_member::expected_type>{ }, rng );
	}

	/***
	 * Parse a JSONMember from the json_data starting at member_path.
	 * @tparam JsonMember The type of the item being parsed
	 * @param json_data JSON string data
	 * @param member_path A dot separated path of member names, default is the
	 * root.  Array indices are specified with square brackets e.g. [5] is the 6th
	 * item
	 * @tparam KnownBounds The bounds of the json_data are known to contain the
	 * whole value
	 * @return A value reified from the JSON data member
	 * @throws daw::json::json_exception
	 */
	template<typename JsonMember, typename ParsePolicy, bool KnownBounds,
	         typename Result>
	[[maybe_unused, nodiscard]] constexpr Result
	from_json( std::string_view json_data, std::string_view member_path ) {
		using json_member = json_details::unnamed_default_type_mapping<JsonMember>;
		daw_json_assert( not json_data.empty( ), ErrorReason::EmptyJSONDocument );
		using json_member = json_details::unnamed_default_type_mapping<JsonMember>;
		static_assert(
		  json_details::has_unnamed_default_type_mapping_v<JsonMember>,
		  "Missing specialization of daw::json::json_data_contract for class "
		  "mapping or specialization of daw::json::json_link_basic_type_map" );
		auto [is_found, rng] = json_details::find_range<ParsePolicy>(
		  json_data, { std::data( member_path ), std::size( member_path ) } );
		if constexpr( json_member::expected_type == JsonParseTypes::Null ) {
			if( not is_found ) {
				return typename json_member::constructor_t{ }( );
			}
		} else {
			daw_json_assert( is_found, ErrorReason::JSONPathNotFound );
		}
		return json_details::parse_value<json_member, KnownBounds>(
		  ParseTag<json_member::expected_type>{ }, rng );
	}

	/***
	 * Parse a JSONMember from the json_data starting at member_path.
	 * @tparam JsonMember The type of the item being parsed
	 * @param json_data JSON string data
	 * @param member_path A dot separated path of member names, default is the
	 * root.  Array indices are specified with square brackets e.g. [5] is the 6th
	 * item
	 * @tparam KnownBounds The bounds of the json_data are known to contain the
	 * whole value
	 * @return A value reified from the JSON data member
	 * @throws daw::json::json_exception
	 */
	template<typename JsonMember, typename ParsePolicy, bool KnownBounds,
	         typename Result, typename Allocator>
	[[maybe_unused, nodiscard]] constexpr Result
	from_json_alloc( std::string_view json_data, std::string_view member_path,
	                 Allocator const &alloc ) {
		daw_json_assert( not json_data.empty( ), ErrorReason::EmptyJSONDocument );
		using json_member = json_details::unnamed_default_type_mapping<JsonMember>;
		static_assert(
		  json_details::has_unnamed_default_type_mapping_v<JsonMember>,
		  "Missing specialization of daw::json::json_data_contract for class "
		  "mapping or specialization of daw::json::json_link_basic_type_map" );
		Allocator a = alloc;
		auto [is_found, rng] = json_details::find_range<ParsePolicy>(
		  json_data, { std::data( member_path ), std::size( member_path ) }, a );
		if constexpr( json_member::expected_type == JsonParseTypes::Null ) {
			if( not is_found ) {
				return typename json_member::constructor_t{ }( );
			}
		} else {
			daw_json_assert( is_found, ErrorReason::JSONPathNotFound );
		}
		return json_details::parse_value<json_member, KnownBounds>(
		  ParseTag<json_member::expected_type>{ }, rng );
	}

	/***
	 * Parse a value from a json_value
	 * @tparam JsonMember The type of the item being parsed
	 * @param value JSON data, see basic_json_value
	 * @tparam KnownBounds The bounds of the json_data are known to contain the
	 * whole value
	 * @return A value reified from the JSON data member
	 * @throws daw::json::json_exception
	 */
	template<typename JsonMember, typename ParsePolicy, bool KnownBounds,
	         typename Result, typename Range>
	[[maybe_unused, nodiscard]] inline constexpr Result
	from_json( basic_json_value<Range> value ) {
		using json_member = json_details::unnamed_default_type_mapping<JsonMember>;
		using json_member = json_details::unnamed_default_type_mapping<JsonMember>;
		static_assert(
		  json_details::has_unnamed_default_type_mapping_v<JsonMember>,
		  "Missing specialization of daw::json::json_data_contract for class "
		  "mapping or specialization of daw::json::json_link_basic_type_map" );
		auto const json_data = value.get_string_view( );
		auto rng =
		  ParsePolicy( std::data( json_data ), daw::data_end( json_data ) );

		return json_details::parse_value<json_member, KnownBounds>(
		  ParseTag<json_member::expected_type>{ }, rng );
	}

	/***
	 * Parse a JSONMember from the json_data starting at member_path.
	 * @param value JSON data, see basic_json_value
	 * @param member_path A dot separated path of member names, default is the
	 * root.  Array indices are specified with square brackets e.g. [5] is the 6th
	 * item
	 * @tparam JsonMember The type of the item being parsed
	 * @tparam KnownBounds The bounds of the json_data are known to contain the
	 * whole value
	 * @return A value reified from the JSON data member
	 * @throws daw::json::json_exception
	 */
	template<typename JsonMember, typename ParsePolicy, bool KnownBounds,
	         typename Result, typename Range>
	[[maybe_unused, nodiscard]] constexpr Result
	from_json( basic_json_value<Range> value, std::string_view member_path ) {
		using json_member = json_details::unnamed_default_type_mapping<JsonMember>;
		using json_member = json_details::unnamed_default_type_mapping<JsonMember>;
		static_assert(
		  json_details::has_unnamed_default_type_mapping_v<JsonMember>,
		  "Missing specialization of daw::json::json_data_contract for class "
		  "mapping or specialization of daw::json::json_link_basic_type_map" );
		using json_member = json_details::unnamed_default_type_mapping<JsonMember>;
		auto const json_data = value.get_string_view( );
		auto [is_found, rng] = json_details::find_range<ParsePolicy>(
		  json_data, { std::data( member_path ), std::size( member_path ) } );
		if constexpr( json_member::expected_type == JsonParseTypes::Null ) {
			if( not is_found ) {
				return typename json_member::constructor_t{ }( );
			}
		} else {
			daw_json_assert( is_found, ErrorReason::JSONPathNotFound );
		}
		return json_details::parse_value<json_member, KnownBounds>(
		  ParseTag<json_member::expected_type>{ }, rng );
	}

	/**
	 * Parse JSON data where the root item is an array
	 * @tparam JsonElement The type of each element in array.  Must be one of
	 * the above json_XXX classes.  This version is checked
	 * @tparam Container Container to store values in
	 * @tparam Constructor Callable to construct Container with no arguments
	 * @param json_data JSON string data containing array
	 * @param member_path A dot separated path of member names to start parsing
	 * from. Array indices are specified with square brackets e.g. [5] is the 6th
	 * item
	 * @tparam KnownBounds The bounds of the json_data are known to contain the
	 * whole value
	 * @return A Container containing parsed data from JSON string
	 * @throws daw::json::json_exception
	 */
	template<typename JsonElement, typename Container, typename ParsePolicy,
	         typename Constructor, bool KnownBounds>
	[[maybe_unused, nodiscard]] constexpr Container
	from_json_array( std::string_view json_data, std::string_view member_path ) {
		daw_json_assert( not json_data.empty( ), ErrorReason::EmptyJSONDocument );
		daw_json_assert( std::data( member_path ) != nullptr,
		                 ErrorReason::EmptyJSONPath );
		static_assert(
		  json_details::has_unnamed_default_type_mapping_v<JsonElement>,
		  "Missing specialization of daw::json::json_data_contract for class "
		  "mapping or specialization of daw::json::json_link_basic_type_map" );
		using element_type =
		  json_details::unnamed_default_type_mapping<JsonElement>;
		static_assert( not std::is_same_v<element_type, void>,
		               "Unknown JsonElement type." );

		using parser_t = json_array<no_name, JsonElement, Container, Constructor>;

		auto [is_found, rng] = json_details::find_range<ParsePolicy>(
		  json_data, { std::data( member_path ), std::size( member_path ) } );

		if constexpr( parser_t::expected_type == JsonParseTypes::Null ) {
			if( not is_found ) {
				return typename parser_t::constructor_t{ }( );
			}
		} else {
			daw_json_assert( is_found, ErrorReason::JSONPathNotFound );
		}
		rng.trim_left_unchecked( );
#if defined( _MSC_VER ) and not defined( __clang__ )
		// Work around MSVC ICE
		daw_json_assert( rng.is_opening_bracket_checked( ),
		                 ErrorReason::InvalidArrayStart, rng );
#else
		using Range = daw::remove_cvref_t<decltype( rng )>;
		daw_json_assert_weak( rng.is_opening_bracket_checked( ),
		                      ErrorReason::InvalidArrayStart, rng );
#endif

		return json_details::parse_value<parser_t, KnownBounds>(
		  ParseTag<JsonParseTypes::Array>{ }, rng );
	}
} // namespace daw::json
