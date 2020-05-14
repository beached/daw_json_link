// The MIT License (MIT)
//
// Copyright (c) 2014-2019 Darrell Wright
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

#include <iostream>
#include <string>
#include <utf8/unchecked.h>

#include <daw/cpp_17.h>
#include <daw/daw_algorithm.h>
#include <daw/daw_fnv1a_hash.h>
#include <daw/daw_string_view.h>
#include <daw/daw_traits.h>

#include "daw_utf_range.h"

namespace daw {
	struct utf_string {
		using iterator = range::utf_iterator;
		using const_iterator = range::utf_iterator const;
		using reference = range::utf_iterator::reference;
		using value_type = range::utf_iterator::value_type;
		using const_reference = value_type const &;
		using difference_type = range::utf_iterator::difference_type;

	private:
		std::string m_values;
		daw::range::utf_range m_range;

	public:
		utf_string( );
		utf_string( utf_string const &other );
		explicit utf_string( daw::string_view other );
		explicit utf_string( daw::range::utf_range other );
		utf_string( char const *other );

		template<size_t N>
		utf_string( char const ( &str )[N] )
		  : m_values( str, N - 1 )
		  , m_range( daw::range::create_char_range( m_values ) ) {}

		utf_string &operator=( utf_string const &rhs );
		utf_string &operator=( daw::string_view rhs );
		utf_string &operator=( char const *rhs );
		utf_string &operator=( std::string const &rhs );

		template<size_t N>
		utf_string &operator=( char const ( &str )[N] ) {
			m_values = str;
			m_range = daw::range::create_char_range( m_values );
			return *this;
		}

		~utf_string( ) = default;

		utf_string( utf_string && ) noexcept = default;
		utf_string &operator=( utf_string && ) noexcept = default;

		const_iterator begin( ) const noexcept;
		const_iterator cbegin( ) const noexcept;
		const_iterator end( ) const noexcept;
		const_iterator cend( ) const noexcept;
		size_t size( ) const noexcept;
		bool empty( ) const noexcept;
		range::char_iterator raw_begin( ) const noexcept;
		range::char_iterator raw_end( ) const noexcept;
		size_t raw_size( ) const noexcept;
		utf_string substr( size_t pos, size_t length ) const;

		std::string const &to_string( ) const noexcept;
		std::u32string to_u32string( ) const;
		range::utf_range const &utf_range( ) const noexcept;
		int compare( utf_string const &rhs ) const noexcept;

		void sort( );
	}; // utf_string

	bool operator==( utf_string const &lhs, utf_string const &rhs ) noexcept;
	bool operator!=( utf_string const &lhs, utf_string const &rhs ) noexcept;
	bool operator<( utf_string const &lhs, utf_string const &rhs ) noexcept;
	bool operator>( utf_string const &lhs, utf_string const &rhs ) noexcept;
	bool operator<=( utf_string const &lhs, utf_string const &rhs ) noexcept;
	bool operator>=( utf_string const &lhs, utf_string const &rhs ) noexcept;
	std::string to_string( utf_string const &str );
	daw::string_view to_string_view( utf_string const &str );

	template<typename OStream,
	         std::enable_if_t<daw::traits::is_ostream_like_v<OStream, char>,
	                          std::nullptr_t> = nullptr>
	OStream &operator<<( OStream &os, utf_string const &value ) {
		os << value.utf_range( );
		return os;
	}
} // namespace daw

std::string to_string( daw::utf_string const &str );
std::string to_string( daw::utf_string &&str );

namespace std {
	template<>
	struct hash<daw::utf_string> {
		std::hash<std::string> m_hash;
		size_t operator( )( daw::utf_string const &value ) const noexcept {
			return m_hash( value.to_string( ) );
		}
	};
} // namespace std
