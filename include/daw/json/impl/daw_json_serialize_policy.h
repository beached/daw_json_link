// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/daw_json_link
//

#pragma once

#include "daw_json_option_bits.h"
#include "daw_json_parse_options.h"
#include "daw_json_serialize_policy_details.h"
#include "daw_json_serialize_policy_options.h"
#include "version.h"

#include <daw/daw_move.h>

#include <cstddef>
#include <iterator>

namespace daw::json {
	inline namespace DAW_JSON_VER {
		/***
		 * Create the parser options flag for BasicParsePolicy
		 * @tparam Policies Policy types that satisfy the `is_policy_flag` trait.
		 * @param policies A list of parser options to change from the defaults.
		 * @return A json_options_t that encodes the options for the parser
		 */
		template<typename... Policies>
		constexpr json_details::json_options_t
		serialize_options( Policies... policies ) {
			static_assert( ( json_details::is_option_flag<Policies> and ... ),
			               "Only registered policy types are allowed" );
			auto result = json_details::serialization::default_policy_flag;
			if constexpr( sizeof...( Policies ) > 0 ) {
				result |=
				  ( json_details::serialization::set_bits_for( policies ) | ... );
			}
			return result;
		}

		template<typename OutputIterator,
		         json_details::json_options_t PolicyFlags =
		           json_details::serialization::default_policy_flag>
		struct serialization_policy
		  : json_details::iterator_wrapper<OutputIterator> {
			using i_am_a_serialization_policy = void;

			constexpr serialization_policy( OutputIterator it )
			  : json_details::iterator_wrapper<OutputIterator>{ {DAW_MOVE( it )} } {}

			static constexpr SerializationFormat serialization_format =
			  json_details::serialization::get_bits_for<SerializationFormat>(
			    PolicyFlags );

			static constexpr IndentationType indentation_type =
			  json_details::serialization::get_bits_for<IndentationType>(
			    PolicyFlags );

			static constexpr RestrictedStringOutput restricted_string_output =
			  json_details::serialization::get_bits_for<RestrictedStringOutput>(
			    PolicyFlags );
		};

		struct use_default_serialization_policy;

		template<typename, typename = void>
		struct is_serialization_policy : std::false_type {};

		template<typename OutputIterator, json_details::json_options_t PolicyFlags>
		struct is_serialization_policy<
		  serialization_policy<OutputIterator, PolicyFlags>> : std::true_type {};
	} // namespace DAW_JSON_VER
} // namespace daw::json
