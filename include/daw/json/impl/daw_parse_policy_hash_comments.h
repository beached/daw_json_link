// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/daw_json_link
//

#pragma once

#include "daw_json_assert.h"
#include "daw_json_parse_common.h"
#include "daw_mem_functions.h"
#include "daw_parse_policy_policy_details.h"

#include <daw/daw_hide.h>

namespace daw::json {
	struct HashCommentSkippingPolicy {
		static constexpr bool has_alternate_parse_tokens = true;

		template<typename ExecTag>
		DAW_ATTRIBUTE_FLATTEN static constexpr char const *
		skip_alternate_token_checked( ExecTag const &tag, char const *first,
		                              char const *last ) {
			if( first < last and *first == '#' ) {
				++first;
				daw_json_assert( first < last, "Unexpected end of stream" );
				first =
				  json_details::mem_move_to_next_of<false, '\n'>( tag, first, last );
				++first;
			}
			return first;
		}

		template<typename ExecTag>
		DAW_ATTRIBUTE_FLATTEN static constexpr char const *
		skip_alternate_token_unchecked( ExecTag const &tag, char const *first,
		                                char const *last ) {
			if( *first == '#' ) {
				++first;
				first =
				  json_details::mem_move_to_next_of<true, '\n'>( tag, first, last );
				++first;
			}
			return first;
		}

		template<bool is_unchecked_input, typename ExecTag>
		DAW_ATTRIBUTE_FLATTEN static constexpr char const *
		skip_alternate_token( ExecTag const &tag, char const *first,
		                      char const *last ) {
			if constexpr( is_unchecked_input ) {
				return skip_alternate_token_unchecked( tag, first, last );
			} else {
				return skip_alternate_token_checked( tag, first, last );
			}
		}

		DAW_ATTRIBUTE_FLATTEN static constexpr bool is_alternate_token( char c ) {
			return c == '#';
		}
	};
} // namespace daw::json
