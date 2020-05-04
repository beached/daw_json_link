// The MIT License (MIT)
//
// Copyright (c) Darrell Wright
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files( the "Software" ), to
// deal in the Software without restriction, including without limitation the
// rights to use, copy, modify, merge, publish, distribute, sublicense, and / or
// sell copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#pragma once

#include "daw_json_assert.h"

namespace daw::json::json_details::string_quote {

	struct string_quote_parser {
		template<typename Range, std::enable_if_t<Range::is_unchecked_input,
		                                          std::nullptr_t> = nullptr>
		static constexpr void parse_nq( Range &rng ) {
			while( *rng.first != '"' ) {
				while( *rng.first != '"' and *rng.first != '\\' ) {
					++rng.first;
				}
				if( *rng.first == '\\' ) {
					++rng.first;
					++rng.first;
				}
			}
			daw_json_assert_weak( *rng.first == '"', "Expected a '\"'" );
		}

		template<typename Range, std::enable_if_t<not Range::is_unchecked_input,
		                                          std::nullptr_t> = nullptr>
		static constexpr void parse_nq( Range &rng ) {
			while( rng.first != rng.last and *rng.first != '"' ) {
				while( rng.first != rng.last and *rng.first != '"' and
				       *rng.first != '\\' ) {
					++rng.first;
				}
				if( rng.first != rng.last and *rng.first == '\\' ) {
					++rng.first;
					daw_json_assert_weak( rng.first != rng.last,
					                      "Unexpected end of string" );
					++rng.first;
				}
			}
			daw_json_assert_weak( rng.first != rng.class_last and *rng.first == '"',
			                      "Expected a '\"' at end of string" );
		}
	};
} // namespace daw::json::json_details::string_quote
