// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/daw_json_link
//
#pragma once

#include "to_daw_json_string.h"
#include "version.h"

#include <array>
#include <ciso646>
#include <cstddef>
#include <utility>

namespace daw::json {
	inline namespace DAW_JSON_VER {
		namespace json_details {
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
			template<typename... JsonMembers, typename OutputIterator,
			         std::size_t... Is, typename Tuple, typename Value>
			[[nodiscard]] DAW_ATTRIB_INLINE inline constexpr OutputIterator
			serialize_json_class( OutputIterator it, std::index_sequence<Is...>,
			                      Tuple const &args, Value const &value ) {

				bool is_first = true;
				*it++ = '{';

				auto visited_members =
				  daw::bounded_vector_t<daw::string_view,
				                        sizeof...( JsonMembers ) * 2U>{ };
				// Tag Members, if any.  Putting them ahead means we can parse this
				// faster in the future

				// Using list init to ensure serialization happens in order
				{
					using Names = fwd_pack<JsonMembers...>;
					daw::Empty const expander[]{
					  ( dependent_member_to_json_str<
					      Is, traits::nth_element<Is, JsonMembers...>, Names>(
					      is_first, it, args, value, visited_members ),
					    daw::Empty{ } )...,
					  daw::Empty{} };
					(void)expander;
				}

				// Regular Members
				{
					daw::Empty const expander[]{
					  ( to_json_str<Is, traits::nth_element<Is, JsonMembers...>>(
					      is_first, it, args, value, visited_members ),
					    daw::Empty{ } )...,
					  daw::Empty{} };
					(void)expander;
				}
				*it++ = '}';
				return it;
			}

			template<typename... JsonMembers, typename OutputIterator, typename Tuple,
			         typename Value, std::size_t... Is>
			[[nodiscard]] inline constexpr OutputIterator
			serialize_ordered_json_class( OutputIterator it,
			                              std::index_sequence<Is...>,
			                              Tuple const &args, Value const &value ) {

				*it++ = '[';
				size_t array_idx = 0;
				Unused( value );
				{
					daw::Empty const expander[]{
					  ( to_json_ordered_str<Is, traits::nth_element<Is, JsonMembers...>>(
					      array_idx, it, args ),
					    daw::Empty{ } )...,
					  daw::Empty{} };
					(void)expander;
				}
				*it++ = ']';
				return it;
			}
		} // namespace json_details
	}   // namespace DAW_JSON_VER
} // namespace daw::json
