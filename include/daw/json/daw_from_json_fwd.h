// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, version 1.0. (see accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/daw_json_link
//

#pragma once

#include "impl/version.h"

#include "impl/daw_json_link_types_fwd.h"
#include "impl/daw_json_parse_policy.h"
#include "impl/daw_json_traits.h"

#include <ciso646>
#include <string_view>

namespace daw::json {
	inline namespace DAW_JSON_VER {

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
		template<typename JsonMember, bool KnownBounds = false,
		         /* typename Result =
		            json_details::from_json_result_t<JsonMember>,*/
		         typename String, auto... PolicyFlags>
		[[maybe_unused, nodiscard]] constexpr auto
		from_json( String &&json_data, options::parse_flags_t<PolicyFlags...> ) /*
		  -> std::enable_if_t<json_details::is_string_view_like_v<String>, Result>*/
		  ;

		/// @brief Construct the JSONMember from the JSON document argument.
		/// @tparam JsonMember any bool, arithmetic, string, string_view,
		/// daw::json::json_data_contract
		/// @param json_data JSON string data
		/// @tparam KnownBounds The bounds of the json_data are known to contain the
		/// whole value
		/// @return A reified JSONMember constructed from JSON data
		/// @throws daw::json::json_exception
		template<typename JsonMember, bool KnownBounds = false,
		         /*typename Result = json_details::from_json_result_t<JsonMember>,*/
		         typename String>
		[[maybe_unused, nodiscard]] constexpr auto
		from_json( String &&json_data ) /*
-> std::enable_if_t<json_details::is_string_view_like_v<String>, Result> */
		  ;

		/// @brief Construct the JSONMember from the JSON document argument.
		/// @tparam JsonMember any bool, arithmetic, string, string_view,
		/// daw::json::json_data_contract
		/// @param json_data JSON string data
		/// @tparam KnownBounds The bounds of the json_data are known to contain the
		/// whole value
		/// @return A reified T constructed from JSON data
		/// @throws daw::json::json_exception
		template<typename JsonMember, bool KnownBounds = false,
		         /*typename Result = json_details::from_json_result_t<JsonMember>,*/
		         typename String, typename Allocator, auto... PolicyFlags>
		[[maybe_unused, nodiscard]] constexpr auto
		from_json_alloc( String &&json_data, Allocator const &alloc,
		                 options::parse_flags_t<PolicyFlags...> ) /*
		  -> std::enable_if_t<json_details::is_string_view_like_v<String>, Result>*/
		  ;

		/// @brief Construct the JSONMember from the JSON document argument.
		/// @tparam JsonMember any bool, arithmetic, string, string_view,
		/// daw::json::json_data_contract
		/// @param json_data JSON string data
		/// @tparam KnownBounds The bounds of the json_data are known to contain the
		/// whole value
		/// @return A reified T constructed from JSON data
		/// @throws daw::json::json_exception
		template<typename JsonMember, bool KnownBounds = false,
		         /* typename Result =
		            json_details::from_json_result_t<JsonMember>,*/
		         typename String, typename Allocator>
		[[maybe_unused, nodiscard]] constexpr auto
		from_json_alloc( String &&json_data, Allocator const &alloc ) /*
		  -> std::enable_if_t<json_details::is_string_view_like_v<String>, Result>*/
		  ;

		/// @brief Parse a JSONMember from the json_data starting at member_path.
		/// @tparam JsonMember The type of the item being parsed
		/// @param json_data JSON string data
		/// @param member_path A dot separated path of member names, default is the
		/// root.  Array indices are specified with square brackets e.g. [5] is the
		/// 6th item
		/// @tparam KnownBounds The bounds of the json_data are known to contain the
		/// whole value
		/// @return A value reified from the JSON data member
		/// @throws daw::json::json_exception
		template<typename JsonMember, bool KnownBounds = false,
		         /*    typename Result =
		            json_details::from_json_result_t<JsonMember>,*/
		         typename String, auto... PolicyFlags>
		[[maybe_unused, nodiscard]] constexpr auto from_json(
		  String &&json_data, std::string_view member_path,
		  options::parse_flags_t<PolicyFlags...> ) /*
-> std::enable_if_t<json_details::is_string_view_like_v<String>, Result>*/
		  ;

		/// @brief Parse a JSONMember from the json_data starting at member_path.
		/// @tparam JsonMember The type of the item being parsed
		/// @param json_data JSON string data
		/// @param member_path A dot separated path of member names, default is the
		/// root.  Array indices are specified with square brackets e.g. [5] is the
		/// 6th item
		/// @tparam KnownBounds The bounds of the json_data are known to contain the
		/// whole value
		/// @return A value reified from the JSON data member
		/// @throws daw::json::json_exception
		template<typename JsonMember, bool KnownBounds = false,
		         /*   typename Result =
		            json_details::from_json_result_t<JsonMember>,*/
		         typename String>
		[[maybe_unused, nodiscard]] constexpr auto
		from_json( String &&json_data, std::string_view member_path ) /*
		  -> std::enable_if_t<json_details::is_string_view_like_v<String>, Result>*/
		  ;

		/// @brief Parse a JSONMember from the json_data starting at member_path.
		/// @tparam JsonMember The type of the item being parsed
		/// @param json_data JSON string data
		/// @param member_path A dot separated path of member names, default is the
		/// root.  Array indices are specified with square brackets e.g. [5] is the
		/// 6th element/member
		/// @tparam KnownBounds The bounds of the json_data are known to contain the
		/// whole value
		/// @return A value reified from the JSON data member
		/// @throws daw::json::json_exception
		template<typename JsonMember, bool KnownBounds = false,
		         /*  typename Result =
		            json_details::from_json_result_t<JsonMember>,*/
		         typename String, typename Allocator, auto... PolicyFlags>
		[[maybe_unused, nodiscard]] constexpr auto from_json_alloc(
		  String &&json_data, std::string_view member_path, Allocator const &alloc,
		  options::parse_flags_t<PolicyFlags...> ) /*
-> std::enable_if_t<json_details::is_string_view_like_v<String>, Result>*/
		  ;

		/// @brief Parse a JSONMember from the json_data starting at member_path.
		/// @tparam JsonMember The type of the item being parsed
		/// @param json_data JSON string data
		/// @param member_path A dot separated path of member names, default is the
		/// root.  Array indices are specified with square brackets e.g. [5] is the
		/// 6th item
		/// @tparam KnownBounds The bounds of the json_data are known to contain the
		/// whole value
		/// @return A value reified from the JSON data member
		/// @throws daw::json::json_exception
		template<typename JsonMember, bool KnownBounds = false,
		         /*typename Result = json_details::from_json_result_t<JsonMember>,*/
		         typename String, typename Allocator>
		[[maybe_unused, nodiscard]] constexpr auto
		from_json_alloc( String &&json_data, std::string_view member_path,
		                 Allocator const &alloc ) /*
		  -> std::enable_if_t<json_details::is_string_view_like_v<String>, Result>*/
		  ;

		/// @brief Parse a value from a json_value
		/// @tparam JsonMember The type of the item being parsed
		/// @param value JSON data, see basic_json_value
		/// @tparam KnownBounds The bounds of the json_data are known to contain the
		/// whole value
		/// @return A value reified from the JSON data member
		/// @throws daw::json::json_exception
		template<typename JsonMember, bool KnownBounds = false,
		         /* typename Result =
		            json_details::from_json_result_t<JsonMember>,*/
		         json_options_t P, typename A, auto... PolicyFlags>
		[[maybe_unused, nodiscard]] inline constexpr auto
		from_json( basic_json_value<P, A> value,
		           options::parse_flags_t<PolicyFlags...> );

		/// @brief Parse a value from a json_value
		/// @tparam JsonMember The type of the item being parsed
		/// @param value JSON data, see basic_json_value
		/// @tparam KnownBounds The bounds of the json_data are known to contain the
		/// whole value
		/// @return A value reified from the JSON data member
		/// @throws daw::json::json_exception
		template<typename JsonMember, bool KnownBounds = false,
		         /*  typename Result =
		            json_details::from_json_result_t<JsonMember>,*/
		         json_options_t P, typename A>
		[[maybe_unused, nodiscard]] inline constexpr auto
		from_json( basic_json_value<P, A> value );

		/// @brief Parse a JSONMember from the json_data starting at member_path.
		/// @param value JSON data, see basic_json_value
		/// @param member_path A dot separated path of member names, default is the
		/// root.  Array indices are specified with square brackets e.g. [5] is the
		/// 6th item
		/// @tparam JsonMember The type of the item being parsed
		/// @tparam KnownBounds The bounds of the json_data are known to contain the
		/// whole value
		/// @return A value reified from the JSON data member
		/// @throws daw::json::json_exception
		template<typename JsonMember, bool KnownBounds = false,
		         /*  typename Result =
		            json_details::from_json_result_t<JsonMember>,*/
		         json_options_t P, typename A, auto... PolicyFlags>
		[[maybe_unused, nodiscard]] constexpr auto
		from_json( basic_json_value<P, A> value, std::string_view member_path,
		           options::parse_flags_t<PolicyFlags...> );

		/// @brief Parse a JSONMember from the json_data starting at member_path.
		/// @param value JSON data, see basic_json_value
		/// @param member_path A dot separated path of member names, default is the
		/// root.  Array indices are specified with square brackets e.g. [5] is the
		/// 6th item
		/// @tparam JsonMember The type of the item being parsed
		/// @tparam KnownBounds The bounds of the json_data are known to contain the
		/// whole value
		/// @return A value reified from the JSON data member
		/// @throws daw::json::json_exception
		template<typename JsonMember, bool KnownBounds, /* typename Result,*/
		         json_options_t P, typename A>
		[[maybe_unused, nodiscard]] constexpr auto
		from_json( basic_json_value<P, A> value, std::string_view member_path );

		/// @brief Parse JSON data where the root item is an array
		/// @tparam JsonElement The type of each element in array.  Must be one of
		/// the above json_XXX classes.  This version is checked
		/// @tparam Container Container to store values in
		/// @tparam Constructor Callable to construct Container with no arguments
		/// @param json_data JSON string data containing array
		/// @param member_path A dot separated path of member names to start parsing
		/// from. Array indices are specified with square brackets e.g. [5] is the
		/// 6th item
		/// @tparam KnownBounds The bounds of the json_data are known to contain the
		/// whole value
		/// @return A Container containing parsed data from JSON string
		/// @throws daw::json::json_exception
		template<typename JsonElement,
		         typename Container =
		           std::vector<json_details::from_json_result_t<JsonElement>>,
		         typename Constructor = use_default, bool KnownBounds = false,
		         typename String, auto... PolicyFlags>
		[[maybe_unused, nodiscard]] constexpr auto
		from_json_array( String &&json_data, std::string_view member_path,
		                 options::parse_flags_t<PolicyFlags...> )
		  -> std::enable_if_t<json_details::is_string_view_like_v<String>,
		                      Container>;

		/// @brief Parse JSON data where the root item is an array
		/// @tparam JsonElement The type of each element in array.  Must be one of
		/// the above json_XXX classes.  This version is checked
		/// @tparam Container Container to store values in
		/// @tparam Constructor Callable to construct Container with no arguments
		/// @param json_data JSON string data containing array
		/// @param member_path A dot separated path of member names to start parsing
		/// from. Array indices are specified with square brackets e.g. [5] is the
		/// 6th item
		/// @tparam KnownBounds The bounds of the json_data are known to contain the
		/// whole value
		/// @return A Container containing parsed data from JSON string
		/// @throws daw::json::json_exception
		template<typename JsonElement,
		         typename Container =
		           std::vector<json_details::from_json_result_t<JsonElement>>,
		         typename Constructor = use_default, bool KnownBounds = false,
		         typename String>
		[[maybe_unused, nodiscard]] constexpr auto
		from_json_array( String &&json_data, std::string_view member_path )
		  -> std::enable_if_t<json_details::is_string_view_like_v<String>,
		                      Container>;

		/// @brief Parse JSON data where the root item is an array
		/// @tparam JsonElement The type of each element in array.  Must be one of
		/// the above json_XXX classes.  This version is checked
		/// @tparam Container Container to store values in
		/// @tparam Constructor Callable to construct Container with no arguments
		/// @param json_data JSON string data containing array
		/// @tparam KnownBounds The bounds of the json_data are known to contain the
		/// whole value
		/// @return A Container containing parsed data from JSON string
		/// @throws daw::json::json_exception
		template<typename JsonElement,
		         typename Container =
		           std::vector<json_details::from_json_result_t<JsonElement>>,
		         typename Constructor = use_default, bool KnownBounds = false,
		         typename String, auto... PolicyFlags>
		[[maybe_unused, nodiscard]] constexpr auto
		from_json_array( String &&json_data,
		                 options::parse_flags_t<PolicyFlags...> )
		  -> std::enable_if_t<json_details::is_string_view_like_v<String>,
		                      Container>;

		/// @brief Parse JSON data where the root item is an array
		/// @tparam JsonElement The type of each element in array.  Must be one of
		/// the above json_XXX classes.  This version is checked
		/// @tparam Container Container to store values in
		/// @tparam Constructor Callable to construct Container with no arguments
		/// @param json_data JSON string data containing array
		/// @tparam KnownBounds The bounds of the json_data are known to contain the
		/// whole value
		/// @return A Container containing parsed data from JSON string
		/// @throws daw::json::json_exception
		template<typename JsonElement,
		         typename Container =
		           std::vector<json_details::from_json_result_t<JsonElement>>,
		         typename Constructor = use_default, bool KnownBounds = false,
		         typename String>
		[[maybe_unused, nodiscard]] constexpr auto
		from_json_array( String &&json_data )
		  -> std::enable_if_t<json_details::is_string_view_like_v<String>,
		                      Container>;
	} // namespace DAW_JSON_VER
} // namespace daw::json
