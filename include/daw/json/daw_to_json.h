// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, version 1.0. (see accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/daw_json_link
//

#pragma once

#include "daw_to_json_fwd.h"
#include "impl/daw_json_container_appender.h"
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

		template<typename Value, typename JsonClass, typename OutputIterator,
		         auto... PolicyFlags>
		[[maybe_unused]] constexpr OutputIterator
		to_json( Value const &value, OutputIterator it,
		         options::output_flags_t<PolicyFlags...> ) {
			if constexpr( std::is_pointer_v<OutputIterator> ) {
				daw_json_assert( it != nullptr, ErrorReason::NullOutputIterator );
			}
			auto out_it = [&] {
				if constexpr( is_serialization_policy_v<OutputIterator> ) {
					if constexpr( sizeof...( PolicyFlags ) == 0 ) {
						return it;
					} else {
						return serialization_policy<typename OutputIterator::iterator_type,
						                            json_details::serialization::set_bits(
						                              OutputIterator::policy_flags( ),
						                              PolicyFlags... )>( it.get( ) );
					}
				} else {
					return serialization_policy<
					  OutputIterator, options::output_flags_t<PolicyFlags...>::value>(
					  it );
				}
			}( );
			it =
			  json_details::member_to_string( template_arg<JsonClass>, out_it, value )
			    .get( );

			return it;
		}

		template<typename Result, typename Value, typename JsonClass,
		         auto... PolicyFlags>
		[[maybe_unused, nodiscard]] constexpr Result
		to_json( Value const &value,
		         options::output_flags_t<PolicyFlags...> flags ) {
			Result result{ };
			if constexpr( std::is_same_v<Result, std::string> ) {
				result.reserve( 4096 );
			}

			using iter_t = std::back_insert_iterator<Result>;
			(void)to_json<Value, JsonClass>( value, iter_t{ result }, flags );
			if constexpr( std::is_same_v<Result, std::string> ) {
				result.shrink_to_fit( );
			}
			return result;
		}

		template<typename JsonElement, typename Container, typename OutputIterator,
		         auto... PolicyFlags>
		[[maybe_unused]] constexpr OutputIterator
		to_json_array( Container const &c, OutputIterator it,
		               options::output_flags_t<PolicyFlags...> ) {
			static_assert(
			  traits::is_container_like_v<daw::remove_cvref_t<Container>>,
			  "Supplied container must support begin( )/end( )" );
			if constexpr( std::is_pointer_v<OutputIterator> ) {
				daw_json_assert( it != nullptr, ErrorReason::NullOutputIterator );
			}
			auto out_it = [&] {
				if constexpr( is_serialization_policy_v<OutputIterator> ) {
					if constexpr( sizeof...( PolicyFlags ) == 0 ) {
						return it;
					} else {
						return serialization_policy<typename OutputIterator::iterator_type,
						                            json_details::serialization::set_bits(
						                              OutputIterator::policy_flags( ),
						                              PolicyFlags... )>( it.get( ) );
					}
				} else {
					return serialization_policy<
					  OutputIterator, options::output_flags_t<PolicyFlags...>::value>(
					  it );
				}
			}( );
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
				  std::is_same_v<JsonElement, json_details::auto_detect_array_element>;
				using JsonMember =
				  std::conditional_t<is_auto_detect_v,
				                     json_details::json_deduced_type<v_type>,
				                     JsonElement>;

				static_assert(
				  not std::is_same_v<JsonMember,
				    missing_json_data_contract_for_or_unknown_type<JsonElement>>,
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

		template<typename Result, typename JsonElement, typename Container,
		         auto... PolicyFlags>
		[[maybe_unused, nodiscard]] constexpr Result
		to_json_array( Container const &c,
		               options::output_flags_t<PolicyFlags...> flags ) {
			static_assert( traits::is_container_like_v<Container>,
			               "Supplied container must support begin( )/end( )" );
			using iter_t = basic_appender<Result>;
			Result result{ };

			(void)to_json_array<JsonElement>( c, iter_t{ result }, flags );
			return result;
		}
	} // namespace DAW_JSON_VER
} // namespace daw::json
