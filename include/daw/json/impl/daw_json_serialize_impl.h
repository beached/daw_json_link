// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/daw_json_link
//
#pragma once

#include "daw_json_to_string.h"

#include <ciso646>

namespace daw::json::json_details {
	/***
	 * Serialize items to an output iterator as members of a class
	 * @tparam JsonMembers member items in json_class
	 * @tparam OutputIterator An Output Iterator that allows writing character
	 * data
	 * @tparam Is index_sequence index into JsonMembers
	 * @tparam Tuple tuple type holding class members
	 * @tparam Value mapped class type to serialize
	 * @param it an Output Iterator to write char data to
	 * @param args A tuple of the member values
	 * @param value class to serialize
	 * @return The OutputIterator it at the final position
	 */
	template<typename... JsonMembers, typename OutputIterator, std::size_t... Is,
	         typename Tuple, typename Value>
	[[nodiscard]] inline constexpr OutputIterator
	serialize_json_class( OutputIterator it, std::index_sequence<Is...>,
	                      Tuple const &args, Value const &value ) {

		bool is_first = true;
		*it++ = '{';

		auto visited_members =
		  daw::bounded_vector_t<daw::string_view, sizeof...( JsonMembers ) * 2U>{ };
		// Tag Members, if any.  Putting them ahead means we can parse this faster
		// in the future
		(void)( ( tags_to_json_str<Is,
		                           daw::traits::nth_element<Is, JsonMembers...>>(
		            is_first, it, args, value, visited_members ),
		          ... ),
		        0 );
		// Regular Members
		(void)( ( to_json_str<Is, daw::traits::nth_element<Is, JsonMembers...>>(
		            is_first, it, args, value, visited_members ),
		          ... ),
		        0 );

		*it++ = '}';
		return it;
	}

	template<typename... JsonMembers, typename OutputIterator, typename Tuple,
	         typename Value, std::size_t... Is>
	[[nodiscard]] inline constexpr OutputIterator
	serialize_ordered_json_class( OutputIterator it, std::index_sequence<Is...>,
	                              Tuple const &args, Value const &value ) {

		*it++ = '[';
		size_t array_idx = 0;
		Unused( value );
		(void)std::array{
		  ( to_json_ordered_str<Is, daw::traits::nth_element<Is, JsonMembers...>>(
		      array_idx, it, args ),
		    0 )... };

		*it++ = ']';
		return it;
	}
} // namespace daw::json::json_details
