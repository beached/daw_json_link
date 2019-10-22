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

namespace daw::json::impl::quote {
	class quote_parser_t {
		using pf_t =
		  std::add_pointer_t<char const *( char const *, quote_parser_t const & )>;

		using ftable_t = daw::function_table_t<
		  char const *, pf_t, pf_t, pf_t, pf_t, pf_t, pf_t, pf_t, pf_t, pf_t, pf_t,
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
		  pf_t, pf_t, pf_t, pf_t, pf_t, pf_t>;

		static constexpr char const *ret( char const *c, quote_parser_t const & ) {
			return c;
		}

		static constexpr char const *escape( char const *c,
		                                     quote_parser_t const &p ) {
			++c;
			++c; // TODO: potentially unsafe
			return p.ftable( static_cast<size_t>( *c ), c, p );
		}

		static constexpr char const *chr( char const *c, quote_parser_t const &p ) {
			++c;
			return p.ftable( static_cast<size_t>( *c ), c, p );
		}

		ftable_t ftable{
		  ret, ret, ret, ret, ret, ret, ret, ret, ret,    ret, ret, ret, ret, ret,
		  ret, ret, ret, ret, ret, ret, ret, ret, ret,    ret, ret, ret, ret, ret,
		  ret, ret, ret, ret, chr, chr, ret, chr, chr,    chr, chr, chr, chr, chr,
		  chr, chr, chr, chr, chr, chr, chr, chr, chr,    chr, chr, chr, chr, chr,
		  chr, chr, chr, chr, chr, chr, chr, chr, chr,    chr, chr, chr, chr, chr,
		  chr, chr, chr, chr, chr, chr, chr, chr, chr,    chr, chr, chr, chr, chr,
		  chr, chr, chr, chr, chr, chr, chr, chr, escape, chr, chr, chr, chr, chr,
		  chr, chr, chr, chr, chr, chr, chr, chr, chr,    chr, chr, chr, chr, chr,
		  chr, chr, chr, chr, chr, chr, chr, chr, chr,    chr, chr, chr, chr, chr,
		  chr, chr, chr, chr, chr, chr, chr, chr, chr,    chr, chr, chr, chr, chr,
		  chr, chr, chr, chr, chr, chr, chr, chr, chr,    chr, chr, chr, chr, chr,
		  chr, chr, chr, chr, chr, chr, chr, chr, chr,    chr, chr, chr, chr, chr,
		  chr, chr, chr, chr, chr, chr, chr, chr, chr,    chr, chr, chr, chr, chr,
		  chr, chr, chr, chr, chr, chr, chr, chr, chr,    chr, chr, chr, chr, chr,
		  chr, chr, chr, chr, chr, chr, chr, chr, chr,    chr, chr, chr, chr, chr,
		  chr, chr, chr, chr, chr, chr, chr, chr, chr,    chr, chr, chr, chr, chr,
		  chr, chr, chr, chr, chr, chr, chr, chr, chr,    chr, chr, chr, chr, chr,
		  chr, chr, chr, chr, chr, chr, chr, chr, chr,    chr, chr, chr, chr, chr,
		  chr, chr, chr, chr};

		static_assert( ftable_t::size( ) == 256 );
		static_assert( ftable_t::using_array_v );

	public:
		constexpr quote_parser_t( ) = default;

		constexpr char const *operator( )( char const *ptr ) const {
			return ftable( static_cast<size_t>( *ptr ), ptr, *this );
		}
	};

	inline constexpr auto quote_parser = quote_parser_t( );
} // namespace daw::json::impl::quote
