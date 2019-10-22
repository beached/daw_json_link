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

namespace daw::json::impl::unsignedint {
	using parser_func_t = std::add_pointer_t<std::pair<uintmax_t, char const *>(
	  uintmax_t, char const * )>;

	constexpr std::pair<uintmax_t, char const *> ret( uintmax_t n,
	                                                  char const *c ) {
		return {n, c};
	}

	inline constexpr daw::function_table_t<
	  std::pair<uintmax_t, char const *>, parser_func_t, parser_func_t,
	  parser_func_t, parser_func_t, parser_func_t, parser_func_t, parser_func_t,
	  parser_func_t, parser_func_t, parser_func_t, parser_func_t, parser_func_t,
	  parser_func_t, parser_func_t, parser_func_t, parser_func_t, parser_func_t,
	  parser_func_t, parser_func_t, parser_func_t, parser_func_t, parser_func_t,
	  parser_func_t, parser_func_t, parser_func_t, parser_func_t, parser_func_t,
	  parser_func_t, parser_func_t, parser_func_t, parser_func_t, parser_func_t,
	  parser_func_t, parser_func_t, parser_func_t, parser_func_t, parser_func_t,
	  parser_func_t, parser_func_t, parser_func_t, parser_func_t, parser_func_t,
	  parser_func_t, parser_func_t, parser_func_t, parser_func_t, parser_func_t,
	  parser_func_t, parser_func_t, parser_func_t, parser_func_t, parser_func_t,
	  parser_func_t, parser_func_t, parser_func_t, parser_func_t, parser_func_t,
	  parser_func_t, parser_func_t, parser_func_t, parser_func_t, parser_func_t,
	  parser_func_t, parser_func_t, parser_func_t, parser_func_t, parser_func_t,
	  parser_func_t, parser_func_t, parser_func_t, parser_func_t, parser_func_t,
	  parser_func_t, parser_func_t, parser_func_t, parser_func_t, parser_func_t,
	  parser_func_t, parser_func_t, parser_func_t, parser_func_t, parser_func_t,
	  parser_func_t, parser_func_t, parser_func_t, parser_func_t, parser_func_t,
	  parser_func_t, parser_func_t, parser_func_t, parser_func_t, parser_func_t,
	  parser_func_t, parser_func_t, parser_func_t, parser_func_t, parser_func_t,
	  parser_func_t, parser_func_t, parser_func_t, parser_func_t, parser_func_t,
	  parser_func_t, parser_func_t, parser_func_t, parser_func_t, parser_func_t,
	  parser_func_t, parser_func_t, parser_func_t, parser_func_t, parser_func_t,
	  parser_func_t, parser_func_t, parser_func_t, parser_func_t, parser_func_t,
	  parser_func_t, parser_func_t, parser_func_t, parser_func_t, parser_func_t,
	  parser_func_t, parser_func_t, parser_func_t, parser_func_t, parser_func_t,
	  parser_func_t, parser_func_t, parser_func_t, parser_func_t, parser_func_t,
	  parser_func_t, parser_func_t, parser_func_t, parser_func_t, parser_func_t,
	  parser_func_t, parser_func_t, parser_func_t, parser_func_t, parser_func_t,
	  parser_func_t, parser_func_t, parser_func_t, parser_func_t, parser_func_t,
	  parser_func_t, parser_func_t, parser_func_t, parser_func_t, parser_func_t,
	  parser_func_t, parser_func_t, parser_func_t, parser_func_t, parser_func_t,
	  parser_func_t, parser_func_t, parser_func_t, parser_func_t, parser_func_t,
	  parser_func_t, parser_func_t, parser_func_t, parser_func_t, parser_func_t,
	  parser_func_t, parser_func_t, parser_func_t, parser_func_t, parser_func_t,
	  parser_func_t, parser_func_t, parser_func_t, parser_func_t, parser_func_t,
	  parser_func_t, parser_func_t, parser_func_t, parser_func_t, parser_func_t,
	  parser_func_t, parser_func_t, parser_func_t, parser_func_t, parser_func_t,
	  parser_func_t, parser_func_t, parser_func_t, parser_func_t, parser_func_t,
	  parser_func_t, parser_func_t, parser_func_t, parser_func_t, parser_func_t,
	  parser_func_t, parser_func_t, parser_func_t, parser_func_t, parser_func_t,
	  parser_func_t, parser_func_t, parser_func_t, parser_func_t, parser_func_t,
	  parser_func_t, parser_func_t, parser_func_t, parser_func_t, parser_func_t,
	  parser_func_t, parser_func_t, parser_func_t, parser_func_t, parser_func_t,
	  parser_func_t, parser_func_t, parser_func_t, parser_func_t, parser_func_t,
	  parser_func_t, parser_func_t, parser_func_t, parser_func_t, parser_func_t,
	  parser_func_t, parser_func_t, parser_func_t, parser_func_t, parser_func_t,
	  parser_func_t, parser_func_t, parser_func_t, parser_func_t, parser_func_t,
	  parser_func_t, parser_func_t, parser_func_t, parser_func_t, parser_func_t,
	  parser_func_t, parser_func_t, parser_func_t, parser_func_t, parser_func_t,
	  parser_func_t, parser_func_t, parser_func_t, parser_func_t, parser_func_t,
	  parser_func_t, parser_func_t, parser_func_t>
	  ftable{
	    ret,
	    ret,
	    ret,
	    ret,
	    ret,
	    ret,
	    ret,
	    ret,
	    ret,
	    ret,
	    ret,
	    ret,
	    ret,
	    ret,
	    ret,
	    ret,
	    ret,
	    ret,
	    ret,
	    ret,
	    ret,
	    ret,
	    ret,
	    ret,
	    ret,
	    ret,
	    ret,
	    ret,
	    ret,
	    ret,
	    ret,
	    ret,
	    ret,
	    ret,
	    ret,
	    ret,
	    ret,
	    ret,
	    ret,
	    ret,
	    ret,
	    ret,
	    ret,
	    ret,
	    ret,
	    ret,
	    ret,
	    ret,
	    []( uintmax_t n, char const *c ) {
		    n = n * 10U + 0;
		    ++c;
		    return ftable( *c, n, c );
	    },
	    []( uintmax_t n, char const *c ) {
		    n = n * 10U + 1;
		    ++c;
		    return ftable( *c, n, c );
	    },
	    []( uintmax_t n, char const *c ) {
		    n = n * 10U + 2;
		    ++c;
		    return ftable( *c, n, c );
	    },
	    []( uintmax_t n, char const *c ) {
		    n = n * 10U + 3;
		    ++c;
		    return ftable( *c, n, c );
	    },
	    []( uintmax_t n, char const *c ) {
		    n = n * 10U + 4;
		    ++c;
		    return ftable( *c, n, c );
	    },
	    []( uintmax_t n, char const *c ) {
		    n = n * 10U + 5;
		    ++c;
		    return ftable( *c, n, c );
	    },
	    []( uintmax_t n, char const *c ) {
		    n = n * 10U + 6;
		    ++c;
		    return ftable( *c, n, c );
	    },
	    []( uintmax_t n, char const *c ) {
		    n = n * 10U + 7;
		    ++c;
		    return ftable( *c, n, c );
	    },
	    []( uintmax_t n, char const *c ) {
		    n = n * 10U + 8;
		    ++c;
		    return ftable( *c, n, c );
	    },
	    []( uintmax_t n, char const *c ) {
		    n = n * 10U + 9;
		    ++c;
		    return ftable( *c, n, c );
	    },
	    ret,
	    ret,
	    ret,
	    ret,
	    ret,
	    ret,
	    ret,
	    ret,
	    ret,
	    ret,
	    ret,
	    ret,
	    ret,
	    ret,
	    ret,
	    ret,
	    ret,
	    ret,
	    ret,
	    ret,
	    ret,
	    ret,
	    ret,
	    ret,
	    ret,
	    ret,
	    ret,
	    ret,
	    ret,
	    ret,
	    ret,
	    ret,
	    ret,
	    ret,
	    ret,
	    ret,
	    ret,
	    ret,
	    ret,
	    ret,
	    ret,
	    ret,
	    ret,
	    ret,
	    ret,
	    ret,
	    ret,
	    ret,
	    ret,
	    ret,
	    ret,
	    ret,
	    ret,
	    ret,
	    ret,
	    ret,
	    ret,
	    ret,
	    ret,
	    ret,
	    ret,
	    ret,
	    ret,
	    ret,
	    ret,
	    ret,
	    ret,
	    ret,
	    ret,
	    ret,
	    ret,
	    ret,
	    ret,
	    ret,
	    ret,
	    ret,
	    ret,
	    ret,
	    ret,
	    ret,
	    ret,
	    ret,
	    ret,
	    ret,
	    ret,
	    ret,
	    ret,
	    ret,
	    ret,
	    ret,
	    ret,
	    ret,
	    ret,
	    ret,
	    ret,
	    ret,
	    ret,
	    ret,
	    ret,
	    ret,
	    ret,
	    ret,
	    ret,
	    ret,
	    ret,
	    ret,
	    ret,
	    ret,
	    ret,
	    ret,
	    ret,
	    ret,
	    ret,
	    ret,
	    ret,
	    ret,
	    ret,
	    ret,
	    ret,
	    ret,
	    ret,
	    ret,
	    ret,
	    ret,
	    ret,
	    ret,
	    ret,
	    ret,
	    ret,
	    ret,
	    ret,
	    ret,
	    ret,
	    ret,
	    ret,
	    ret,
	    ret,
	    ret,
	    ret,
	    ret,
	    ret,
	    ret,
	    ret,
	    ret,
	    ret,
	    ret,
	    ret,
	    ret,
	    ret,
	    ret,
	    ret,
	    ret,
	    ret,
	    ret,
	    ret,
	    ret,
	    ret,
	    ret,
	    ret,
	    ret,
	    ret,
	    ret,
	    ret,
	    ret,
	    ret,
	    ret,
	    ret,
	    ret,
	    ret,
	    ret,
	    ret,
	    ret,
	    ret,
	    ret,
	    ret,
	    ret,
	    ret,
	    ret,
	    ret,
	    ret,
	    ret,
	    ret,
	    ret,
	    ret,
	    ret,
	    ret,
	    ret,
	    ret,
	    ret,
	    ret,
	    ret,
	    ret,
	    ret,
	    ret,
	    ret,
	    ret,
	    ret
	  };
} // namespace daw::json::impl::unsignedint
