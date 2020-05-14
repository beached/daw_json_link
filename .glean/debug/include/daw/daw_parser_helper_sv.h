// The MIT License (MIT)
//
// Copyright (c) 2014-2020 Darrell Wright
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

#include "daw_parser_addons.h"
#include "daw_parser_helper.h"
#include "daw_string_view.h"
#include "daw_traits.h"

namespace daw {
	namespace parser {
		template<typename CharT>
		constexpr auto find_first_of( daw::basic_string_view<CharT> const str,
		                              CharT const value ) noexcept {
			auto it = str.cbegin( );
			while( it != str.cend( ) and *it != value ) {
				++it;
			}
			return it;
		}

		template<typename CharT, typename Predicate>
		constexpr auto find_first_of_when(
		  daw::basic_string_view<CharT> str,
		  Predicate pred ) noexcept( noexcept( pred( CharT{} ) ) ) {
			auto it = str.cbegin( );
			while( it != str.cend( ) and !pred( *it ) ) {
				++it;
			}
			return it;
		}

		template<typename CharT>
		constexpr daw::basic_string_view<CharT>
		trim_left( daw::basic_string_view<CharT> str ) noexcept {
			while( !str.empty( ) and is_unicode_whitespace( str.front( ) ) ) {
				str.remove_prefix( );
			}
			return str;
		}

		template<typename CharT>
		constexpr daw::basic_string_view<CharT>
		trim_right( daw::basic_string_view<CharT> str ) noexcept {
			while( !str.empty( ) and is_unicode_whitespace( str.back( ) ) ) {
				str.remove_suffix( );
			}
			return str;
		}

		template<typename CharT>
		constexpr daw::basic_string_view<CharT>
		trim( daw::basic_string_view<CharT> str ) noexcept {
			return trim_right( trim_left( str ) );
		}

		template<typename Int, typename CharT>
		constexpr Int parse_unsigned_int( daw::basic_string_view<CharT> str ) {
			Int i = 0;
			daw::parser::parse_unsigned_int( str.cbegin( ), str.cend( ), i );
			return i;
		}

		template<typename Int, typename CharT>
		constexpr Int parse_int( daw::basic_string_view<CharT> str ) {
			Int i = 0;
			daw::parser::parse_int( str.cbegin( ), str.cend( ), i );
			return i;
		}

		template<char... vals>
		class char_in_t {
			template<char check_val, char... check_vals>
			constexpr bool check( char const c ) const noexcept {
				return ( c == check_val ) or ( ( c == check_vals ) or ... );
			}

		public:
			constexpr bool operator( )( char const c ) const noexcept {
				return check<vals...>( c );
			}
		};
	} // namespace parser
} // namespace daw
