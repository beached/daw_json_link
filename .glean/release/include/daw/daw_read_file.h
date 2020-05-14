// The MIT License (MIT)
//
// Copyright (c) 2018-2020 Darrell Wright
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

#include <fstream>
#include <optional>
#include <string>

namespace daw {
	template<typename CharT>
	std::optional<std::basic_string<char>> read_file( CharT const *str ) {
		auto in_file = std::basic_ifstream<char>( str );
		if( not in_file ) {
			return {};
		}

		return std::basic_string<char>( std::istreambuf_iterator<char>( in_file ),
		                                {} );
	}

	template<typename CharT>
	std::optional<std::basic_string<char>>
	read_file( std::basic_string<CharT> str ) {
		return read_file( str.c_str( ) );
	}

	template<typename CharT>
	std::optional<std::basic_string<wchar_t>> read_wfile( CharT const *str ) {
		auto in_file = std::basic_ifstream<wchar_t>( str );
		if( not in_file ) {
			return {};
		}

		return std::basic_string<wchar_t>(
		  std::istreambuf_iterator<wchar_t>( in_file ), {} );
	}

	template<typename CharT>
	std::optional<std::basic_string<wchar_t>>
	read_wfile( std::basic_string<CharT> str ) {
		return read_wfile( str.c_str( ) );
	}
} // namespace daw
