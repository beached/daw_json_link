// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/daw_json_link
//

#pragma once

#include "daw_from_json.h"
#include "daw_json_event_parser.h"
#include "impl/daw_json_value.h"

#include <daw/daw_move.h>

#include <optional>
#include <vector>

namespace daw::json {
	inline namespace DAW_JSON_VER {
		struct default_filter_result;
		template<typename ResultElement = default_filter_result,
		         json_options_t PolicyFlags = json_details::default_policy_flag,
		         typename Allocator = json_details::NoAllocator,
		         typename OutputIterator, typename Predicate>
		OutputIterator filter( basic_json_value<PolicyFlags, Allocator> jv,
		                       OutputIterator out_it, Predicate pred ) {

			using jp_t = basic_json_pair<PolicyFlags, Allocator>;
			using result_t =
			  std::conditional_t<std::is_same_v<default_filter_result, ResultElement>,
			                     basic_json_value<PolicyFlags, Allocator>,
			                     ResultElement>;
			static_assert( std::is_invocable_r_v<bool, Predicate, jp_t> );
			struct value_handler {
				Predicate *p;
				OutputIterator *o;
				auto handle_on_value( jp_t const &jp ) const {
					if( ( *p )( jp ) ) {
						**o = from_json<result_t>( jp.value.get_state( ) );
					}
					return true;
				}
			};

			json_event_parser( jv, value_handler{ &pred, &out_it } );
			return out_it;
		}
	} // namespace DAW_JSON_VER
} // namespace daw::json
