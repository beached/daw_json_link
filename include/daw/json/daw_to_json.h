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
			out_it = json_details::member_to_string( template_arg<JsonClass>, out_it,
			                                         value );
			return out_it;
		}

		template<typename Result, typename Value, typename JsonClass>
		[[maybe_unused, nodiscard]] constexpr Result to_json( Value const &value ) {
			Result result{ };
			if constexpr( std::is_same_v<Result, std::string> ) {
				result.reserve( 4096 );
			}
			(void)json_details::member_to_string(
			  template_arg<JsonClass>, std::back_inserter( result ), value );
			if constexpr( std::is_same_v<Result, std::string> ) {
				result.shrink_to_fit( );
			}
			return result;
		}

		template<typename JsonElement, typename Container, typename OutputIterator>
		[[maybe_unused]] constexpr OutputIterator
		to_json_array( Container const &c, OutputIterator out_it ) {
			static_assert(
			  traits::is_container_like_v<daw::remove_cvref_t<Container>>,
			  "Supplied container must support begin( )/end( )" );

			if constexpr( std::is_pointer<OutputIterator>::value ) {
				daw_json_assert( out_it, ErrorReason::NullOutputIterator );
			}
			*out_it++ = '[';
			bool is_first = true;
			// Not const & as some types(vector<bool>::const_reference are not ref
			// types
			for( auto &&v : c ) {

				using v_type = daw::remove_cvref_t<decltype( v )>;
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
				if( is_first ) {
					is_first = false;
				} else {
					*out_it++ = ',';
				}
				out_it =
				  json_details::member_to_string( template_arg<JsonMember>, out_it, v );
			}
			// The last character will be a ',' prior to this
			*out_it++ = ']';
			return out_it;
		}

		template<typename Result, typename JsonElement, typename Container>
		[[maybe_unused, nodiscard]] constexpr Result
		to_json_array( Container &&c ) {
			static_assert(
			  traits::is_container_like_v<daw::remove_cvref_t<Container>>,
			  "Supplied container must support begin( )/end( )" );

			Result result{ };
			auto out_it = json_details::basic_appender<Result>( result );
			to_json_array<JsonElement>( c, out_it );
			return result;
		}
	} // namespace DAW_JSON_VER
} // namespace daw::json
