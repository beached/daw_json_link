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

#include <cstddef>
#include <utility>

#include "daw_json_assert.h"

namespace daw::json::impl::parse_bool {
	namespace impl {
		template<size_t N>
		constexpr bool cxstrcmp( char const *ptr, char const ( &str )[N] ) {
			bool result = true;
			for( size_t n = 0; n < ( N - 1 ); ++n ) {
				result &= ( ptr[n] - str[n] ) == 0;
			}
			return result;
		}
	} // namespace impl
	struct bool_parser {
		[[nodiscard]] static constexpr std::pair<bool, char const *>
		parse( char const *ptr ) {
			json_assert( impl::cxstrcmp( ptr, "true" ) or
			               impl::cxstrcmp( ptr, "false" ),
			             "Expected true or false" );

			if( *ptr == 'f' ) {
				return {false, ptr + 5};
			}
			return {true, ptr + 4};
		}
	};

	static_assert( bool_parser::parse( "true" ).first == true );
	static_assert( bool_parser::parse( "false" ).first == false );
} // namespace daw::json::impl::parse_bool
