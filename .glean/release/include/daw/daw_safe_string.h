// The MIT License (MIT)
//
// Copyright (c) 2016-2020 Darrell Wright
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

#include <string>

#include "daw_move.h"

namespace daw {
	template<typename SanitizeFunction, typename charT,
	         typename traits = std::char_traits<charT>,
	         typename Alloc = std::allocator<charT>>
	struct basic_safe_string {
		using string_type = std::basic_string<charT, traits, Alloc>;

	private:
		string_type m_unsafe_string;
		SanitizeFunction m_sanitize_function;

	public:
		basic_safe_string( ) = default;
		~basic_safe_string( ) = default;
		basic_safe_string( basic_safe_string const & ) = default;
		basic_safe_string( basic_safe_string && ) = default;
		basic_safe_string &operator=( basic_safe_string const & ) = default;
		basic_safe_string &operator=( basic_safe_string && ) = default;

		basic_safe_string( string_type unsafe_string,
		                   SanitizeFunction sanitize_function = SanitizeFunction{} )
		  : m_unsafe_string{daw::move( unsafe_string )}
		  , m_sanitize_function{daw::move( sanitize_function )} {}

		basic_safe_string( charT const *const unsafe_cstring,
		                   SanitizeFunction sanitize_function = SanitizeFunction{} )
		  : basic_safe_string{string_type{unsafe_cstring},
		                      daw::move( sanitize_function )} {}

		basic_safe_string &operator=( string_type unsafe_string ) {
			m_unsafe_string = daw::move( unsafe_string );
			return *this;
		}

		string_type get( ) const {
			return m_sanitize_function( m_unsafe_string );
		}

		string_type const &unsafe_get( ) {
			return m_unsafe_string;
		}

		explicit operator string_type( ) const {
			return get( );
		}
	}; // basic_safe_string

	template<typename SanitizeFunction>
	using safe_string = basic_safe_string<SanitizeFunction, char>;

	template<typename SanitizeFunction>
	using safe_wstring = basic_safe_string<SanitizeFunction, wchar_t>;

	template<typename SanitizeFunction>
	using safe_u16string = basic_safe_string<SanitizeFunction, char16_t>;

	template<typename SanitizeFunction>
	using safe_u32string = basic_safe_string<SanitizeFunction, char32_t>;

	template<typename SanitizeFunction, typename charT, typename traits,
	         typename Alloc>
	auto make_safe_string( std::basic_string<charT, traits, Alloc> unsafe_string,
	                       SanitizeFunction &&sanitize_function ) {
		return basic_safe_string<SanitizeFunction, charT, traits, Alloc>{
		  daw::move( unsafe_string ),
		  std::forward<SanitizeFunction>( sanitize_function )};
	}

	template<typename SanitizeFunction, typename charT,
	         typename traits = std::char_traits<charT>,
	         typename Alloc = std::allocator<charT>>
	auto make_safe_string( charT const *const unsafe_cstring,
	                       SanitizeFunction &&sanitize_function ) {
		return basic_safe_string<SanitizeFunction, charT, traits, Alloc>{
		  unsafe_cstring, std::forward<SanitizeFunction>( sanitize_function )};
	}
} // namespace daw
