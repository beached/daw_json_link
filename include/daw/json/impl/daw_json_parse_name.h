// The MIT License (MIT)
//
// Copyright (c) 2019 Darrell Wright
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
#include "daw_json_parse_string_quote.h"

namespace daw::json::impl::name {
	struct name_parser_result {
		char const *end_of_name;
		char const *end_of_whitespace;
	};

	struct name_parser {
		/*
		 * end of string " -> name value separating : -> any white space
		 * the string can be escaped too
		 */
		template<typename First, typename Last, bool TrustedInput>
		static constexpr void
		trim_end_of_name( IteratorRange<First, Last, TrustedInput> &rng ) noexcept {
			while( rng.is_space( ) ) {
				rng.remove_prefix( );
			}
			json_assert_untrusted( rng.front( ) == ':', "Expected a ':'" );
			rng.remove_prefix( );
			while( rng.is_space( ) ) {
				rng.remove_prefix( );
			}
		}

		template<typename First, typename Last, bool TrustedInput>
		[[nodiscard]] static constexpr daw::string_view
		parse_nq( IteratorRange<First, Last, TrustedInput> &rng ) noexcept {
			auto ptr = rng.begin( );
			while( rng.front( ) != '"' ) {
				while( rng.front( ) != '"' and rng.front( ) != '\\' ) {
					rng.remove_prefix( );
				}
				if( rng.front( ) == '\\' ) {
					rng.remove_prefix( 2 );
				}
			}
			json_assert_untrusted( rng.front( ) == '"', "Expected a '\"'" );
			auto result =
			  daw::string_view( ptr, static_cast<size_t>( rng.begin( ) - ptr ) );
			rng.remove_prefix( );
			trim_end_of_name( rng );
			return result;
		}
	};
} // namespace daw::json::impl::name
