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
		template<typename Range, std::enable_if_t<Range::is_unchecked_input,
		                                          std::nullptr_t> = nullptr>
		[[nodiscard]] static constexpr bool parse_nq( Range &rng ) {
			bool need_slow_path = false;
			while( *rng.first != '"' ) {
				while( *rng.first != '"' and *rng.first != '\\' ) {
					++rng.first;
				}
				if( *rng.first == '\\' ) {
					need_slow_path = true;
					++rng.first;
					++rng.first;
				}
			}
			daw_json_assert_weak( *rng.first == '"', "Expected a '\"'" );
			return need_slow_path;
		}

		template<typename Range, std::enable_if_t<not Range::is_unchecked_input,
		                                          std::nullptr_t> = nullptr>
		[[nodiscard]] static constexpr bool parse_nq( Range &rng ) {
			bool need_slow_path = false;
			while( rng.first != rng.last and *rng.first != '"' ) {
				while( rng.first != rng.last and *rng.first != '"' and
				       *rng.first != '\\' ) {
					++rng.first;
				}
				if( rng.first != rng.last and *rng.first == '\\' ) {
					need_slow_path = true;
					++rng.first;
					daw_json_assert_weak( rng.first != rng.last,
					                      "Unexpected end of string" );
					++rng.first;
				}
			}
			daw_json_assert_weak( rng.first != rng.class_last and *rng.first == '"',
			                      "Expected a '\"' at end of string" );
			return need_slow_path;
		}
	};
} // namespace daw::json::json_details::string_quote
