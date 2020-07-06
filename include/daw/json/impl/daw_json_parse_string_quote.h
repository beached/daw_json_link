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
				while( *first != '"' and *first != '\\' ) {
					++first;
				}
				if( *first == '\\' ) {
					need_slow_path = true;
					++first;
					++first;
				}
			}
			daw_json_assert_weak( *first == '"', "Expected a '\"'" );
			rng.first = first;
			return need_slow_path;
		}

		template<typename Range>
		[[nodiscard]] static constexpr auto parse_nq( Range &rng )
		  -> std::enable_if_t<not Range::is_unchecked_input, bool> {
			bool need_slow_path = false;
			char const *first = rng.first;
			char const *const last = rng.last;
			while( first != rng.last and *first != '"' ) {
				while( first != rng.last and *first != '"' and *first != '\\' ) {
					++first;
				}
				if( first != last and *first == '\\' ) {
					need_slow_path = true;
					++first;
					daw_json_assert_weak( first != last, "Unexpected end of string" );
					++first;
				}
			}
			daw_json_assert_weak( first != rng.class_last and *first == '"',
			                      "Expected a '\"' at end of string" );
			rng.first = first;
			return need_slow_path;
		}
	};
} // namespace daw::json::json_details::string_quote
