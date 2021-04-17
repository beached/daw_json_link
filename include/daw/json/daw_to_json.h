// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, version 1.0. (see accompanying
// file license or copy at http://www.boost.org/license_1_0.txt)
//
// Official repository: https://github.com/beached/daw_json_link
//

#pragma once

#include "daw_to_json_fwd.h"
#include "impl/daw_json_link_types_fwd.h"
#include "impl/daw_json_to_string.h"

#include <iterator>
#include <string>
#include <string_view>
#include <type_traits>

namespace daw::json {
	template<typename Value, typename JsonClass, typename OutputIterator>
	[[maybe_unused]] constexpr OutputIterator to_json( Value const &value,
	                                                   OutputIterator out_it ) {
		if constexpr( std::is_pointer_v<OutputIterator> ) {
			daw_json_assert( out_it, ErrorReason::NullOutputIterator );
		}
		out_it = json_details::member_to_string<JsonClass>( out_it, value );
		return out_it;
	}

	template<typename Result, typename Value, typename JsonClass>
	[[maybe_unused, nodiscard]] constexpr Result to_json( Value const &value ) {
		Result result{ };
		(void)json_details::member_to_string<JsonClass>(
		  std::back_inserter( result ), value );
		return result;
	}

	template<typename JsonElement, typename Container, typename OutputIterator>
	[[maybe_unused]] constexpr OutputIterator
	to_json_array( Container const &c, OutputIterator out_it ) {
		static_assert(
		  daw::traits::is_container_like_v<daw::remove_cvref_t<Container>>,
		  "Supplied container must support begin( )/end( )" );

		if constexpr( std::is_pointer_v<OutputIterator> ) {
			daw_json_assert( out_it, ErrorReason::NullOutputIterator );
		}
		*out_it++ = '[';
		bool is_first = true;
		for( auto const &v : c ) {
			using v_type = daw::remove_cvref_t<decltype( v )>;
			constexpr bool is_auto_detect_v =
			  std::is_same_v<JsonElement, json_details::auto_detect_array_element>;
			using JsonMember =
			  std::conditional_t<is_auto_detect_v,
			                     json_details::unnamed_default_type_mapping<v_type>,
			                     JsonElement>;

			static_assert(
			  not std::is_same_v<
			    JsonMember, daw::json::missing_json_data_contract_for<JsonElement>>,
			  "Unable to detect unnamed mapping" );
			static_assert( not std::is_same_v<JsonElement, JsonMember> );
			if( is_first ) {
				is_first = false;
			} else {
				*out_it++ = ',';
			}
			out_it = json_details::member_to_string<JsonMember>( out_it, v );
		}
		// The last character will be a ',' prior to this
		*out_it++ = ']';
		return out_it;
	}

	template<typename Result, typename JsonElement, typename Container>
	[[maybe_unused, nodiscard]] constexpr Result to_json_array( Container &&c ) {
		static_assert(
		  daw::traits::is_container_like_v<daw::remove_cvref_t<Container>>,
		  "Supplied container must support begin( )/end( )" );

		Result result{ };
		auto out_it = json_details::basic_appender<Result>( result );
		to_json_array<JsonElement>( c, out_it );
		return result;
	}
} // namespace daw::json
