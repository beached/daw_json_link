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
#include <cstdint>
#include <utility>

namespace daw::json::impl::signedint {
	struct signed_parser {
		static constexpr auto minus =
		  static_cast<unsigned>( '-' ) - static_cast<unsigned>( '0' );

		[[nodiscard]] static constexpr std::pair<intmax_t, char const *>
		parse( size_t index, char const *ptr ) {
			bool sign = true;

			auto dig = static_cast<unsigned>( *ptr ) - static_cast<unsigned>( '0' );
			if( dig >= 10 ) {
				if( dig == minus ) {
					sign = false;
				}
				++ptr;
				dig = static_cast<unsigned>( *ptr ) - static_cast<unsigned>( '0' );
			}
			intmax_t n = 0;
			while( dig < 10 ) {
				n = n * 10 + dig;
				++ptr;
				dig = static_cast<unsigned>( *ptr ) - static_cast<unsigned>( '0' );
			}
			return {sign ? n : -n, ptr};
		}
	};

	static_assert( signed_parser::parse( '-', "-12345" ).first == -12345 );
} // namespace daw::json::impl::signedint
