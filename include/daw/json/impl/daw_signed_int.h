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
#include <cstdio>
#include <cstdlib>
#include <tuple>
#include <type_traits>
#include <utility>
#include <variant>

#include <daw/daw_array.h>

namespace daw::json::impl::signedint {
	class signed_parser {
		using pf_t = std::add_pointer_t<std::pair<intmax_t, char const *>(
		  char const *, bool )>;

		static constexpr std::pair<intmax_t, char const *> ret( char const *,
		                                                        bool ) {
			json_assert( false, "Invalid number data" );
			return {0, nullptr};
		}

		static constexpr std::pair<intmax_t, char const *> dig( char const *c,
		                                                        bool sign ) {
			auto dig = static_cast<uint_least32_t>( *c - '0' );
			intmax_t n = 0;
			while( dig < 10 ) {
				n = n * 10 + dig;
				++c;
				dig = static_cast<uint_least32_t>( *c - '0' );
			}
			return {sign ? n : -n, c};
		}

		static constexpr std::pair<intmax_t, char const *> neg( char const *c,
		                                                        bool ) {
			++c;
			return dig( c, false );
		}

		static constexpr std::pair<intmax_t, char const *> pos( char const *c,
		                                                        bool ) {
			return dig( c, true );
		}

		static constexpr auto ftable =
		  daw::make_array<pf_t>( pos, ret, neg, ret, ret, dig, dig, dig, dig, dig,
		                         dig, dig, dig, dig, dig, ret );

		static_assert( ftable.size( ) == 16 );

	public:
		static constexpr auto parse( size_t index, char const *ptr ) {
			return ftable[( static_cast<unsigned>( index ) -
			                static_cast<unsigned>( '+' ) ) &
			              0b1111U]( ptr, true );
		}
	};

	static_assert( signed_parser::parse( '-', "-12345" ).first == -12345 );
} // namespace daw::json::impl::signedint
