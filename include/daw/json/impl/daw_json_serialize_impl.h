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

			template<typename T, std::size_t Capacity>
			struct basic_array_t {
				static constexpr std::size_t capacity = Capacity;

			private:
				std::size_t position{ };
				std::string_view array[capacity]{ };

			public:
				constexpr basic_array_t( ) = default;

				constexpr T const *data( ) const {
					return array;
				}

				constexpr T *data( ) {
					return array;
				}

				constexpr std::size_t size( ) const {
					return position;
				}

				constexpr void push_back( T const &v ) {
					assert( position < capacity );
					array[position] = v;
					++position;
				}
			};
			template<typename T>
			struct basic_array_t<T, 0> {
				static constexpr std::size_t capacity = 0;

				constexpr basic_array_t( ) = default;

				constexpr T const *data( ) const {
					return nullptr;
				}

				constexpr T *data( ) {
					return nullptr;
				}

				constexpr std::size_t size( ) const {
					return 0;
				}
			};

			/***
			 * Serialize items to an output iterator as members of a class
			 * @tparam JsonMembers member items in json_class
			 * @tparam OutputIterator An Output Iterator that allows writing
			 * character data
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
			[[nodiscard]] inline constexpr OutputIterator
			serialize_json_class( OutputIterator it, Tuple const &args,
			                      Value const &value, std::index_sequence<Is...> ) {

				*it++ = '{';

				using visit_size = std::integral_constant<
				  std::size_t,
				  ( sizeof...( JsonMembers ) +
				    ( static_cast<std::size_t>( has_dependent_member_v<JsonMembers> ) +
				      ... + 0 ) )>;
				auto visited_members =
				  basic_array_t<std::string_view, visit_size::value>{ };

				// Tag Members, if any.  Putting them ahead means we can parse this
				// faster in the future

				// Using list init to ensure serialization happens in order
				bool is_first = true;
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
			serialize_ordered_json_class( OutputIterator it, Tuple const &args,
			                              Value const &value,
			                              std::index_sequence<Is...> ) {

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
