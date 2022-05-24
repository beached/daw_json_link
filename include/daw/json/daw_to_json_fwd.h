// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, version 1.0. (see accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/daw_json_link
//

#pragma once

#include "impl/daw_json_link_types_fwd.h"
#include "impl/daw_json_serialize_policy.h"
#include "impl/version.h"

namespace daw::json {
	inline namespace DAW_JSON_VER {
		namespace options {
			/***
			 * @brief Specify output policy flags in to_json calls.  See cookbook item
			 * output_options.md
			 */
			template<auto... PolicyFlags>
			struct output_flags_t {
				static_assert(
				  ( json_details::is_output_option_v<decltype( PolicyFlags )> and ... ),
				  "Only valid output flags can be used.  See cookbook "
				  "output_options.md" );
				static constexpr json_details::json_options_t value =
				  json_details::serialization::set_bits(
				    json_details::serialization::default_policy_flag, PolicyFlags... );
			};
			/***
			 * @brief Specify output policy flags in to_json calls.  See cookbook item
			 * output_options.md
			 */
			template<>
			struct output_flags_t<> {
				static constexpr json_details::json_options_t value =
				  json_details::serialization::default_policy_flag;
			};

			/***
			 * @brief Specify output policy flags in to_json calls.  See cookbook item
			 * output_options.md
			 */
			template<auto... PolicyFlags>
			inline constexpr auto output_flags =
			  options::output_flags_t<PolicyFlags...>{ };
		} // namespace options

		/**
		 *
		 * @tparam OutputIterator Iterator to character data to
		 * @tparam JsonClass Type that has json_parser_description and to_json_data
		 * function overloads
		 * @param value  value to serialize
		 * @param out_it result to serialize to
		 */
		template<typename JsonClass = use_default, typename Value,
		         typename OutputIterator, auto... PolicyFlags>
		[[maybe_unused]] constexpr OutputIterator to_json(
		  Value const &value, OutputIterator out_it,
		  options::output_flags_t<PolicyFlags...> flags = options::output_flags<> );

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
		template<typename Result = std::string, typename JsonClass = use_default,
		         typename Value, auto... PolicyFlags>
		[[maybe_unused, nodiscard]] constexpr Result to_json(
		  Value const &value,
		  options::output_flags_t<PolicyFlags...> flags = options::output_flags<> );

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
		template<typename JsonElement = use_default, typename Container,
		         typename OutputIterator, auto... PolicyFlags>
		[[maybe_unused]] constexpr OutputIterator to_json_array(
		  Container const &c, OutputIterator out_it,
		  options::output_flags_t<PolicyFlags...> flags = options::output_flags<> );

		/**
		 * Serialize a container to JSON.  This convenience method allows for easier
		 * serialization of containers when the root of the document is an array
		 * Serialize Container
		 * @tparam Container Type of Container to serialize the elements of
		 * @param c Container containing data to serialize.
		 * @return A std::string containing the serialized elements of c
		 */
		template<typename Result = std::string, typename JsonElement = use_default,
		         typename Container, auto... PolicyFlags>
		[[maybe_unused, nodiscard]] constexpr Result to_json_array(
		  Container const &c,
		  options::output_flags_t<PolicyFlags...> flags = options::output_flags<> );
	} // namespace DAW_JSON_VER
} // namespace daw::json
