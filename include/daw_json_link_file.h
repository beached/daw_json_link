// The MIT License (MIT)
//
// Copyright (c) 2017 Darrell Wright
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files( the "Software" ), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and / or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#pragma once

#include <boost/filesystem.hpp>
#include <vector>

#include <daw/daw_exception.h>
#include <daw/daw_memory_mapped_file.h>
#include <daw/daw_string_view.h>

#include "daw_json_link.h"

namespace daw {
	namespace json {
		template<typename Derived>
		auto from_file( daw::string_view path_str ) {
			using namespace std::string_literals;
			static auto const make_path_str = []( daw::string_view s ) {
#ifdef WIN32
				std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
				return converter.from_bytes( s.to_string( ) );
#else
				return s.to_string( );
#endif
			};
			auto const localized_path = make_path_str( path_str );
			daw::exception::daw_throw_on_false( boost::filesystem::exists( localized_path ), path_str + " does not exist" );
			daw::filesystem::memory_mapped_file_t<char> json_file{localized_path };
			daw::exception::daw_throw_on_false( json_file, "Failed to open test file '"s + localized_path + "'"s );

			return Derived::from_json_string( daw::string_view{json_file.data( ), json_file.size( )} ).result;
		}

		template<typename Derived>
		std::vector<Derived> array_from_file( daw::string_view path_str ) {
			using namespace std::string_literals;
			static auto const make_path_str = []( daw::string_view s ) {
#ifdef WIN32
				std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
				return converter.from_bytes( s.to_string( ) );
#else
				return s.to_string( );
#endif
			};
			auto const localized_path = make_path_str( path_str );
			daw::exception::daw_throw_on_false( boost::filesystem::exists( localized_path ), path_str + " does not exist" );
			daw::filesystem::memory_mapped_file_t<char> json_file{localized_path };
			daw::exception::daw_throw_on_false( json_file, "Failed to open test file '"s + localized_path + "'"s );

			return Derived::from_json_array_string( daw::string_view{json_file.data( ), json_file.size( )} );
		}
	} // namespace json
}    // namespace daw


