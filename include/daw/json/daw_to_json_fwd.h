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
#include "impl/daw_json_serialize_policy.h"

namespace daw::json {
	inline namespace DAW_JSON_VER {
		namespace options {
			/// @brief Specify output policy flags in to_json calls.  See cookbook
			/// item output_options.md
			template<auto... PolicyFlags>
			struct output_flags_t {
				static_assert(
				  ( json_details::is_output_option_v<decltype( PolicyFlags )> and ... ),
				  "Only valid output flags can be used.  See cookbook "
				  "output_options.md" );
				static constexpr json_options_t value =
				  json_details::serialization::set_bits(
				    json_details::serialization::default_policy_flag, PolicyFlags... );
			};
			/// @brief Specify output policy flags in to_json calls.  See cookbook
			/// item output_options.md
			template<>
			struct output_flags_t<> {
				static constexpr json_options_t value =
				  json_details::serialization::default_policy_flag;
			};

			/// @brief Specify output policy flags in to_json calls.  See cookbook
			/// item output_options.md
			template<auto... PolicyFlags>
			inline constexpr auto output_flags =
			  options::output_flags_t<PolicyFlags...>{ };
		} // namespace options

		/// @brief Serialize a value to JSON.  Some types(std::string, string_view,
		/// integer's and floating point numbers do not need a mapping setup).  For
		/// user classes, a json_data_contract specialization is needed.
		/// @tparam JsonClass Type that has json_parser_description and to_json_data
		/// function overloads.V  Defaults to deducing based on Value
		/// @param value  value to serialize
		/// @param it The value to return after JSON document has been written to it
		/// via the writeable_output_trait
		/// @param out_it result to serialize to
		/// @return it as is with ref qual or as a value if rvalue ref
		template<typename JsonClass = use_default, typename Value,
		         typename WritableType = std::string, auto... PolicyFlags>
		requires( concepts::is_writable_output_type_v<
		          daw::remove_cvref_t<WritableType>> ) //
		  constexpr daw::rvalue_to_value_t<WritableType> to_json(
		    Value const &value, WritableType &&it = std::string{ },
		    options::output_flags_t<PolicyFlags...> = options::output_flags<> );

		/// @brief Serialize a value to JSON.  Some types(std::string, string_view,
		/// integer's and floating point numbers do not need a mapping setup).  For
		/// user classes, a json_data_contract specialization is needed.
		/// @tparam JsonClass Type that has json_parser_description and to_json_data
		/// function overloads
		/// @param value  value to serialize
		/// @param out_it result to serialize to
		/// @return std::string with JSON representation of value
		template<typename JsonClass = use_default, typename Value,
		         auto... PolicyFlags>
		inline std::string to_json( Value const &value,
		                            options::output_flags_t<PolicyFlags...> );

		namespace json_details {
			/// @brief Tag type to indicate that the element of a Container is not
			/// being specified.  This is the default.
			struct auto_detect_array_element {};
		} // namespace json_details

		/**
		 * Serialize a container to JSON.  This convenience method allows for easier
		 * serialization of containers when the root of the document is an array
		 * Serialize Container
		 * @tparam Container Type of Container to serialize the elements of
		 * @tparam WritableType Iterator to write data to
		 * @param c Container containing data to serialize.
		 * @return WritableType with final state of iterator
		 */
		template<typename JsonElement = use_default, typename Container,
		         typename WritableType, auto... PolicyFlags>
		requires( concepts::is_writable_output_type_v<
		          daw::remove_cvref_t<WritableType>> ) //
		  constexpr daw::rvalue_to_value_t<WritableType> to_json_array(
		    Container const &c, WritableType &&it,
		    options::output_flags_t<PolicyFlags...> = options::output_flags<> );
		/**
		 * Serialize a container to JSON.  This convenience method allows for
		 * easier serialization of containers when the root of the document is an
		 * array Serialize Container
		 * @tparam Container Type of Container to serialize the elements of
		 * @param c Container containing data to serialize.
		 * @return A std::string containing the serialized elements of c
		 */
		template<typename JsonElement = use_default, typename Container,
		         auto... PolicyFlags>
		inline std::string to_json_array(
		  Container const &c,
		  options::output_flags_t<PolicyFlags...> = options::output_flags<> );
	} // namespace DAW_JSON_VER
} // namespace daw::json
