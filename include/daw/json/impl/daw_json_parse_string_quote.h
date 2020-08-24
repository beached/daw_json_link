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

#include <daw/daw_uint_buffer.h>

namespace daw::json::json_details::string_quote {
	struct string_quote_parser {
		template<typename Range>
		[[nodiscard]] static constexpr std::size_t parse_nq( Range &rng ) {
			std::ptrdiff_t need_slow_path = -1;
			char const *const first = rng.first;
			while( rng.has_more( ) ) {
				json_details::mem_skip_string( Range::exec_tag, rng );

				if constexpr( Range::is_unchecked_input ) {
					if( DAW_JSON_LIKELY( *rng.first == '"' ) ) {
						break;
					}
				} else {
					if( DAW_JSON_LIKELY( rng.has_more( ) and *rng.first == '"' ) ) {
						break;
					}
				}
				// We are at a backslash
				if( need_slow_path < 0 ) {
					need_slow_path = rng.first - first;
				}
				rng.first += 2;
			}
			daw_json_assert_weak( rng.has_more( ) and *rng.first == '"',
			                      "Expected a '\"' at end of string" );
			return static_cast<std::size_t>( need_slow_path );
		}
	};
} // namespace daw::json::json_details::string_quote
