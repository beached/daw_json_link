// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/daw_json_link
//

#pragma once

#include "daw_json_assert.h"

namespace daw::json::json_details::string_quote {

	struct string_quote_parser {
		template<typename Range>
		[[nodiscard]] static constexpr auto parse_nq( Range &rng )
		  -> std::enable_if_t<Range::is_unchecked_input, bool> {
			bool need_slow_path = false;
			char const *first = rng.first;
			while( *first != '"' ) {
				while( *first != '"' ) {
					++first;
				}
				if( DAW_JSON_UNLIKELY( *std::prev( first ) == '\\' ) ) {
					need_slow_path = true;
					++first;
				} else {
					break;
				}
			}
			rng.first = first;
			return need_slow_path;
		}

		template<typename Range>
		[[nodiscard]] static constexpr auto parse_nq( Range &rng )
		  -> std::enable_if_t<not Range::is_unchecked_input, bool> {
			bool need_slow_path = false;
			char const *first = rng.first;
			char const *const last = rng.class_last;
			while( first < last and *first != '"' ) {
				while( first < last and *first != '"' ) {
					++first;
				}
				if( DAW_JSON_UNLIKELY( first < last and
				                       *std::prev( first ) == '\\' ) ) {
					need_slow_path = true;
					++first;
				} else {
					break;
				}
			}
			daw_json_assert_weak( first < last and *first == '"',
			                      "Expected a '\"' at end of string" );
			rng.first = first;
			return need_slow_path;
		}
	};
} // namespace daw::json::json_details::string_quote
