// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, version 1.0. (see accompanying
// file license or copy at http://www.boost.org/license_1_0.txt)
//
// Official repository: https://github.com/beached/daw_json_link
//

#pragma once

#include "impl/daw_json_link_types_fwd.h"
#include "impl/daw_json_parse_policy.h"
#include "impl/daw_json_traits.h"

#include <string_view>

namespace daw::json {
	namespace json_details {

	}
	/**
	 * Construct the JSONMember from the JSON document argument.
	 * @tparam JsonMember any bool, arithmetic, string, string_view,
	 * daw::json::json_data_contract
	 * @param json_data JSON string data
	 * @tparam KnownBounds The bounds of the json_data are known to contain the
	 * whole value
	 * @tparam ParsePolicy Parse policy used while parsing.  Default is with
	 * checking and no comments
	 * @return A reified JSONMember constructed from JSON data
	 * @throws daw::json::json_exception
	 */
	template<typename JsonMember,
	         typename ParsePolicy = NoCommentSkippingPolicyChecked,
	         bool KnownBounds = false,
	         typename Result = json_details::from_json_result_t<JsonMember>>
	[[maybe_unused, nodiscard]] constexpr Result
	from_json( std::string_view json_data );

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
	template<typename JsonMember,
	         typename ParsePolicy = NoCommentSkippingPolicyChecked,
	         bool KnownBounds = false,
	         typename Result = json_details::from_json_result_t<JsonMember>,
	         typename Allocator>
	[[maybe_unused, nodiscard]] constexpr Result
	from_json_alloc( std::string_view json_data, Allocator const &alloc );

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
	template<typename JsonMember,
	         typename ParsePolicy = NoCommentSkippingPolicyChecked,
	         bool KnownBounds = false,
	         typename Result = json_details::from_json_result_t<JsonMember>>
	[[maybe_unused, nodiscard]] constexpr Result
	from_json( std::string_view json_data, std::string_view member_path );

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
	template<typename JsonMember,
	         typename ParsePolicy = NoCommentSkippingPolicyChecked,
	         bool KnownBounds = false,
	         typename Result = json_details::from_json_result_t<JsonMember>,
	         typename Allocator>
	[[maybe_unused, nodiscard]] constexpr Result
	from_json_alloc( std::string_view json_data, std::string_view member_path,
	                 Allocator const &alloc );

	/***
	 * Parse a value from a json_value
	 * @tparam JsonMember The type of the item being parsed
	 * @param value JSON data, see basic_json_value
	 * @tparam KnownBounds The bounds of the json_data are known to contain the
	 * whole value
	 * @return A value reified from the JSON data member
	 * @throws daw::json::json_exception
	 */
	template<typename JsonMember,
	         typename ParsePolicy = NoCommentSkippingPolicyChecked,
	         bool KnownBounds = false,
	         typename Result = json_details::from_json_result_t<JsonMember>,
	         typename Range>
	[[maybe_unused, nodiscard]] inline constexpr Result
	from_json( basic_json_value<Range> value );

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
	template<typename JsonMember,
	         typename ParsePolicy = NoCommentSkippingPolicyChecked,
	         bool KnownBounds = false,
	         typename Result = json_details::from_json_result_t<JsonMember>,
	         typename Range>
	[[maybe_unused, nodiscard]] constexpr Result
	from_json( basic_json_value<Range> value, std::string_view member_path );

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
	template<typename JsonElement,
	         typename Container =
	           std::vector<typename json_details::unnamed_default_type_mapping<
	             JsonElement>::parse_to_t>,
	         typename ParsePolicy = NoCommentSkippingPolicyChecked,
	         typename Constructor = daw::construct_a_t<Container>,
	         bool KnownBounds = false>
	[[maybe_unused, nodiscard]] constexpr Container
	from_json_array( std::string_view json_data,
	                 std::string_view member_path = "" );
} // namespace daw::json
