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

#include <daw/daw_function_table.h>

namespace daw::json::impl::signedint {
	class signed_parser_t {
		using pf_t = std::add_pointer_t<std::pair<intmax_t, char const *>(
		  intmax_t, char const *, bool )>;

		/*
		using ftable_t = daw::function_table_t<
		  std::pair<intmax_t, char const *>, pf_t, pf_t, pf_t, pf_t, pf_t, pf_t,
		  pf_t, pf_t, pf_t, pf_t, pf_t, pf_t, pf_t, pf_t, pf_t, pf_t, pf_t, pf_t,
		  pf_t, pf_t, pf_t, pf_t, pf_t, pf_t, pf_t, pf_t, pf_t, pf_t, pf_t, pf_t,
		  pf_t, pf_t, pf_t, pf_t, pf_t, pf_t, pf_t, pf_t, pf_t, pf_t, pf_t, pf_t,
		  pf_t, pf_t, pf_t, pf_t, pf_t, pf_t, pf_t, pf_t, pf_t, pf_t, pf_t, pf_t,
		  pf_t, pf_t, pf_t, pf_t, pf_t, pf_t, pf_t, pf_t, pf_t, pf_t, pf_t, pf_t,
		  pf_t, pf_t, pf_t, pf_t, pf_t, pf_t, pf_t, pf_t, pf_t, pf_t, pf_t, pf_t,
		  pf_t, pf_t, pf_t, pf_t, pf_t, pf_t, pf_t, pf_t, pf_t, pf_t, pf_t, pf_t,
		  pf_t, pf_t, pf_t, pf_t, pf_t, pf_t, pf_t, pf_t, pf_t, pf_t, pf_t, pf_t,
		  pf_t, pf_t, pf_t, pf_t, pf_t, pf_t, pf_t, pf_t, pf_t, pf_t, pf_t, pf_t,
		  pf_t, pf_t, pf_t, pf_t, pf_t, pf_t, pf_t, pf_t, pf_t, pf_t, pf_t, pf_t,
		  pf_t, pf_t, pf_t, pf_t, pf_t, pf_t, pf_t, pf_t, pf_t, pf_t, pf_t, pf_t,
		  pf_t, pf_t, pf_t, pf_t, pf_t, pf_t, pf_t, pf_t, pf_t, pf_t, pf_t, pf_t,
		  pf_t, pf_t, pf_t, pf_t, pf_t, pf_t, pf_t, pf_t, pf_t, pf_t, pf_t, pf_t,
		  pf_t, pf_t, pf_t, pf_t, pf_t, pf_t, pf_t, pf_t, pf_t, pf_t, pf_t, pf_t,
		  pf_t, pf_t, pf_t, pf_t, pf_t, pf_t, pf_t, pf_t, pf_t, pf_t, pf_t, pf_t,
		  pf_t, pf_t, pf_t, pf_t, pf_t, pf_t, pf_t, pf_t, pf_t, pf_t, pf_t, pf_t,
		  pf_t, pf_t, pf_t, pf_t, pf_t, pf_t, pf_t, pf_t, pf_t, pf_t, pf_t, pf_t,
		  pf_t, pf_t, pf_t, pf_t, pf_t, pf_t, pf_t, pf_t, pf_t, pf_t, pf_t, pf_t,
		  pf_t, pf_t, pf_t, pf_t, pf_t, pf_t, pf_t, pf_t, pf_t, pf_t, pf_t, pf_t,
		  pf_t, pf_t, pf_t, pf_t, pf_t, pf_t, pf_t, pf_t, pf_t, pf_t, pf_t, pf_t,
		  pf_t, pf_t, pf_t, pf_t, pf_t, pf_t, pf_t, pf_t, pf_t, pf_t>;
*/
		static constexpr std::pair<intmax_t, char const *>
		ret( intmax_t, char const *, bool ) {
			json_assert( false, "Invalid number data" );
			return {0, nullptr};
		}

		static constexpr std::pair<intmax_t, char const *>
		dig( intmax_t n, char const *c, bool sign ) {
			auto dig = static_cast<uint_least32_t>( *c - '0' );
			while( dig < 10 ) {
				n = n * 10 + dig;
				++c;
				dig = static_cast<uint_least32_t>( *c - '0' );
			}
			return {sign ? n : -n, c};
		}

		static constexpr std::pair<intmax_t, char const *>
		neg( intmax_t, char const *c, bool ) {
			++c;
			return dig( 0ULL, c, false );
		}

		static constexpr std::pair<intmax_t, char const *>
		pos( intmax_t, char const *c, bool ) {
			return dig( 0ULL, c, true );
		}

		static constexpr std::array<pf_t, 256> ftable{
		  ret, ret, ret, ret, ret, ret, ret, ret, ret, ret, ret, ret, ret, ret, ret,
		  ret, ret, ret, ret, ret, ret, ret, ret, ret, ret, ret, ret, ret, ret, ret,
		  ret, ret, ret, ret, ret, ret, ret, ret, ret, ret, ret, ret, ret, pos, ret,
		  neg, ret, ret, dig, dig, dig, dig, dig, dig, dig, dig, dig, dig, ret, ret,
		  ret, ret, ret, ret, ret, ret, ret, ret, ret, ret, ret, ret, ret, ret, ret,
		  ret, ret, ret, ret, ret, ret, ret, ret, ret, ret, ret, ret, ret, ret, ret,
		  ret, ret, ret, ret, ret, ret, ret, ret, ret, ret, ret, ret, ret, ret, ret,
		  ret, ret, ret, ret, ret, ret, ret, ret, ret, ret, ret, ret, ret, ret, ret,
		  ret, ret, ret, ret, ret, ret, ret, ret, ret, ret, ret, ret, ret, ret, ret,
		  ret, ret, ret, ret, ret, ret, ret, ret, ret, ret, ret, ret, ret, ret, ret,
		  ret, ret, ret, ret, ret, ret, ret, ret, ret, ret, ret, ret, ret, ret, ret,
		  ret, ret, ret, ret, ret, ret, ret, ret, ret, ret, ret, ret, ret, ret, ret,
		  ret, ret, ret, ret, ret, ret, ret, ret, ret, ret, ret, ret, ret, ret, ret,
		  ret, ret, ret, ret, ret, ret, ret, ret, ret, ret, ret, ret, ret, ret, ret,
		  ret, ret, ret, ret, ret, ret, ret, ret, ret, ret, ret, ret, ret, ret, ret,
		  ret, ret, ret, ret, ret, ret, ret, ret, ret, ret, ret, ret, ret, ret, ret,
		  ret, ret, ret, ret, ret, ret, ret, ret, ret, ret, ret, ret, ret, ret, ret,
		  ret};

		static_assert( ftable.size( ) == 256 );
//		static_assert( ftable_t::using_array_v );

	public:
		constexpr signed_parser_t( ) = default;

		constexpr auto operator( )( size_t index, intmax_t val,
		                            char const *ptr ) const {
			return ftable[index]( val, ptr, true );
		}
	};

	inline constexpr auto signed_parser = signed_parser_t( );

	static_assert( signed_parser( '-', 0LL, "-12345" ).first == -12345 );
} // namespace daw::json::impl::signedint
