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
	using pf_t = std::add_pointer_t<std::pair<intmax_t, char const *>(
	  intmax_t, char const * )>;

	using ftable_t = daw::function_table_t<
	  std::pair<intmax_t, char const *>, pf_t, pf_t, pf_t, pf_t, pf_t, pf_t, pf_t,
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
	  pf_t, pf_t, pf_t, pf_t, pf_t, pf_t, pf_t, pf_t, pf_t>;

	extern ftable_t ftable;

	constexpr std::pair<intmax_t, char const *> ret( intmax_t n, char const *c ) {
		return {n, c};
	}

	constexpr std::pair<intmax_t, char const *> dig( intmax_t n, char const *c ) {
		n = n * 10 + static_cast<intmax_t>( *c );
		++c;
		return ftable( static_cast<size_t>( *c ), n, c );
	}

	constexpr std::pair<intmax_t, char const *> neg( intmax_t n, char const *c ) {
		++c;
		// TODO: Fix this risk
		n = -static_cast<intmax_t>( *c );
		++c;
		return ftable( static_cast<size_t>( *c ), n, c );
	}

	constexpr std::pair<intmax_t, char const *> pos( intmax_t n, char const *c ) {
		return ftable( static_cast<size_t>( *c ), n, c );
	}

	ftable_t ftable = decltype( ftable ){
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

} // namespace daw::json::impl::signedint
