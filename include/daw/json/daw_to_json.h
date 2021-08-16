// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, version 1.0. (see accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/daw_json_link
//

#pragma once

#include "daw_to_json_fwd.h"
#include "impl/daw_json_link_types_fwd.h"
#include "impl/to_daw_json_string.h"
#include "impl/version.h"

#include <daw/daw_traits.h>

#include <iterator>
#include <string>
#include <string_view>
#include <type_traits>

namespace daw::json {
	inline namespace DAW_JSON_VER {
		template<typename Value, typename JsonClass, typename OutputIterator>
		[[maybe_unused]] constexpr OutputIterator to_json( Value const &value,
		                                                   OutputIterator out_it ) {
			if constexpr( std::is_pointer<OutputIterator>::value ) {
				daw_json_assert( out_it, ErrorReason::NullOutputIterator );
			}
			if constexpr( is_serialization_policy<OutputIterator>::value ) {
				auto state = json_details::member_to_string( template_arg<JsonClass>,
				                                             out_it, value );
			} else {
				out_it = json_details::member_to_string(
				           template_arg<JsonClass>,
				           serialization_policy<OutputIterator>( out_it ), value )
				           .get( );
			}
			return out_it;
		}

		template<typename Result, typename Value, typename JsonClass,
		         typename SerializationPolicy>
		[[maybe_unused, nodiscard]] constexpr Result to_json( Value const &value ) {
			Result result{ };
			if constexpr( std::is_same_v<Result, std::string> ) {
				result.reserve( 4096 );
			}

			using iter_t = std::back_insert_iterator<Result>;
			using policy = std::conditional_t<
			  std::is_same_v<SerializationPolicy, use_default_serialization_policy>,
			  serialization_policy<iter_t>, SerializationPolicy>;

			(void)json_details::member_to_string( template_arg<JsonClass>,
			                                      policy( iter_t( result ) ), value );
			if constexpr( std::is_same_v<Result, std::string> ) {
				result.shrink_to_fit( );
			}
			return result;
		}

		template<typename JsonElement, typename Container, typename OutputIterator>
		[[maybe_unused]] constexpr OutputIterator
		to_json_array( Container const &c, OutputIterator it ) {
			static_assert(
			  traits::is_container_like_v<daw::remove_cvref_t<Container>>,
			  "Supplied container must support begin( )/end( )" );
			using iter_t =
			  std::conditional_t<is_serialization_policy<OutputIterator>::value,
			                     OutputIterator,
			                     serialization_policy<OutputIterator>>;

			auto out_it = iter_t( it );
			if constexpr( std::is_pointer<OutputIterator>::value ) {
				daw_json_assert( out_it, ErrorReason::NullOutputIterator );
			}
			*out_it++ = '[';
			out_it.add_indent( );
			// Not const & as some types(vector<bool>::const_reference are not ref
			// types
			auto first = std::begin( c );
			auto last = std::end( c );
			bool const has_elements = first != last;
			while( first != last ) {
				auto const &v = *first;
				using v_type = DAW_TYPEOF( v );
				constexpr bool is_auto_detect_v =
				  std::is_same<JsonElement,
				               json_details::auto_detect_array_element>::value;
				using JsonMember =
				  std::conditional_t<is_auto_detect_v,
				                     json_details::json_deduced_type<v_type>,
				                     JsonElement>;

				static_assert(
				  not std::is_same_v<JsonMember,
				                     missing_json_data_contract_for<JsonElement>>,
				  "Unable to detect unnamed mapping" );
				// static_assert( not std::is_same_v<JsonElement, JsonMember> );
				out_it.next_member( );

				out_it =
				  json_details::member_to_string( template_arg<JsonMember>, out_it, v );
				++first;
				if( first != last ) {
					*out_it++ = ',';
				}
			}
			// The last character will be a ',' prior to this
			out_it.del_indent( );
			if( has_elements ) {
				out_it.output_newline( );
			}
			*out_it++ = ']';
			return out_it.get( );
		}

		template<typename Result, typename JsonElement,
		         typename SerializationPolicy, typename Container>
		[[maybe_unused, nodiscard]] constexpr Result
		to_json_array( Container &&c ) {
			static_assert(
			  traits::is_container_like_v<daw::remove_cvref_t<Container>>,
			  "Supplied container must support begin( )/end( )" );
			using iter_t = json_details::basic_appender<Result>;
			using policy = std::conditional_t<
			  std::is_same_v<SerializationPolicy, use_default_serialization_policy>,
			  serialization_policy<iter_t>, SerializationPolicy>;
			Result result{ };
			auto out_it = policy( iter_t( result ) );

			to_json_array<JsonElement>( c, out_it );
			return result;
		}
	} // namespace DAW_JSON_VER
} // namespace daw::json
