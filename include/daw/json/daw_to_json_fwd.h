// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, version 1.0. (see accompanying
// file license or copy at http://www.boost.org/license_1_0.txt)
//
// Official repository: https://github.com/beached/daw_json_link
//

#pragma once

#include "impl/daw_json_link_types_fwd.h"
#include "impl/version.h"

namespace daw::json {
	inline namespace DAW_JSON_VER {
		/**
		 *
		 * @tparam OutputIterator Iterator to character data to
		 * @tparam JsonClass Type that has json_parser_description and to_json_data
		 * function overloads
		 * @param value  value to serialize
		 * @param out_it result to serialize to
		 */
		template<
		  typename Value,
		  typename JsonClass = typename json_details::json_deduced_type<Value>,
		  typename OutputIterator>
		[[maybe_unused]] constexpr OutputIterator to_json( Value const &value,
		                                                   OutputIterator out_it );

		/**
		 * Serialize a value to JSON.  Some types(std::string, string_view.
		 * integer's and floating point numbers do not need a mapping setup).  For
		 * user classes, a json_data_contract specialization is needed.
		 * @tparam Result std::string like type to put result into
		 * @tparam JsonClass Type that has json_parser_description and to_json_data
		 * function overloads
		 * @param value  value to serialize
		 * @return  JSON string data
		 */
		template<
		  typename Result = std::string, typename Value,
		  typename JsonClass = typename json_details::json_deduced_type<Value>>
		[[maybe_unused, nodiscard]] constexpr Result to_json( Value const &value );

		namespace json_details {
			/***
			 * Tag type to indicate that the element of a Container is not being
			 * specified.  This is the default.
			 */
			struct auto_detect_array_element {};
		} // namespace json_details

		/**
		 * Serialize a container to JSON.  This convenience method allows for easier
		 * serialization of containers when the root of the document is an array
		 * Serialize Container
		 * @tparam Container Type of Container to serialize the elements of
		 * @tparam OutputIterator Iterator to write data to
		 * @param c Container containing data to serialize.
		 * @return OutputIterator with final state of iterator
		 */
		template<typename JsonElement = json_details::auto_detect_array_element,
		         typename Container, typename OutputIterator>
		[[maybe_unused]] constexpr OutputIterator
		to_json_array( Container const &c, OutputIterator out_it );

		/**
		 * Serialize a container to JSON.  This convenience method allows for easier
		 * serialization of containers when the root of the document is an array
		 * Serialize Container
		 * @tparam Container Type of Container to serialize the elements of
		 * @param c Container containing data to serialize.
		 * @return A std::string containing the serialized elements of c
		 */
		template<typename Result = std::string,
		         typename JsonElement = json_details::auto_detect_array_element,
		         typename Container>
		[[maybe_unused, nodiscard]] constexpr Result to_json_array( Container &&c );
	} // namespace DAW_JSON_VER
} // namespace daw::json
