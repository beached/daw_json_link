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

namespace daw::json::impl::parse_bool {
	struct bool_parser {
		[[nodiscard]] static constexpr std::pair<bool, char const *>
		parse( char const *ptr ) {
			if( *ptr == 't' or *ptr == 'T' ) {
				return { true, ptr + 4 };
			}
			return { false, ptr + 5};
		}
	};

	static_assert( bool_parser::parse( "true" ).first == true );
	static_assert( bool_parser::parse( "True" ).first == true );
	static_assert( bool_parser::parse( "false" ).first == false );
	static_assert( bool_parser::parse( "False" ).first == false );
} // namespace daw::json::impl::unsignedint
