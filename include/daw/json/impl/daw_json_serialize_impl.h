// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/daw_json_link
//
#pragma once

#include "version.h"

#include "to_daw_json_string.h"

#include <daw/daw_empty.h>
#include <daw/daw_fwd_pack_apply.h>
#include <daw/daw_string_view.h>
#include <daw/daw_undefined.h>
#include <daw/traits/daw_traits_nth_element.h>

#include <cstddef>
#include <daw/stdinc/integer_sequence.h>

namespace daw::json {
	inline namespace DAW_JSON_VER {
		namespace json_details {

			template<typename T, std::size_t Capacity>
			struct basic_array_t {
				static constexpr std::size_t capacity = Capacity;

			private:
				std::size_t position{ };
				daw::string_view array[capacity]{ };

			public:
				basic_array_t( ) = default;

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

				basic_array_t( ) = default;

				DAW_CONSTEVAL T const *data( ) const {
					return nullptr;
				}

				DAW_CONSTEVAL T *data( ) {
					return nullptr;
				}

				DAW_CONSTEVAL std::size_t size( ) const {
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
			 * @return The OutputIterator it at theposition
			 */
			template<typename... JsonMembers, typename OutputIterator,
			         json_options_t SerializationOptions, std::size_t... Is,
			         typename Tuple, typename Value>
			[[nodiscard]] DAW_ATTRIB_INLINE static constexpr serialization_policy<
			  OutputIterator, SerializationOptions>
			serialize_json_class(
			  serialization_policy<OutputIterator, SerializationOptions> it,
			  Tuple const &args, Value const &value, std::index_sequence<Is...> ) {

				it.put( '{' );
				it.add_indent( );

				constexpr auto visit_size =
				  sizeof...( JsonMembers ) +
				  ( static_cast<std::size_t>( has_dependent_member_v<JsonMembers> ) +
				    ... + 0 );
				auto visited_members = basic_array_t<daw::string_view, visit_size>{ };

				// Tag Members, if any.  Putting them ahead means we can parse this
				// faster in the future

				// Using list init to ensure serialization happens in order
				bool is_first = true;

				// gcc complains when JsonMembers is empty
				(void)visited_members;
				(void)is_first;
				{
					using Names = daw::fwd_pack<JsonMembers...>;
					daw::empty_t const expander[]{
					  ( dependent_member_to_json_str<
					      Is, daw::traits::nth_element<Is, JsonMembers...>, Names>(
					      is_first, it, args, value, visited_members ),
					    daw::empty_t{ } )...,
					  daw::empty_t{} };
					(void)expander;
				}

				// Regular Members
				{
					daw::empty_t const expander[]{
					  ( to_json_str<Is, daw::traits::nth_element<Is, JsonMembers...>>(
					      is_first, it, args, value, visited_members ),
					    daw::empty_t{ } )...,
					  daw::empty_t{} };
					(void)expander;
				}
				it.del_indent( );
				if constexpr( sizeof...( Is ) > 0 ) {
					if constexpr( it.output_trailing_comma ==
					              options::OutputTrailingComma::Yes ) {
						it.put( ',' );
					}
					it.next_member( );
				}
				it.put( '}' );
				return it;
			}

			template<typename... JsonMembers, typename OutputIterator,
			         json_options_t SerializerOptions, typename Tuple, typename Value,
			         std::size_t... Is>
			[[nodiscard]] DAW_ATTRIB_INLINE static constexpr serialization_policy<
			  OutputIterator, SerializerOptions>
			serialize_ordered_json_class(
			  serialization_policy<OutputIterator, SerializerOptions> it,
			  Tuple const &args, Value const &value, std::index_sequence<Is...> ) {

				it.put( '[' );
				it.add_indent( );
				it.next_member( );
				std::size_t array_idx = 0;
				(void)array_idx; // gcc was complaining on empty pack
				Unused( value );
				{
					daw::empty_t const expander[]{
					  ( to_json_ordered_str<Is,
					                        daw::traits::nth_element<Is, JsonMembers...>>(
					      array_idx, sizeof...( Is ), it, args ),
					    daw::empty_t{ } )...,
					  daw::empty_t{} };
					(void)expander;
				}
				it.del_indent( );
				if constexpr( sizeof...( Is ) != 0 ) {
					if constexpr( it.output_trailing_comma ==
					              options::OutputTrailingComma::Yes ) {
						it.put( ',' );
					}
					it.next_member( );
				}
				it.put( ']' );
				return it;
			}
		} // namespace json_details
	} // namespace DAW_JSON_VER
} // namespace daw::json
