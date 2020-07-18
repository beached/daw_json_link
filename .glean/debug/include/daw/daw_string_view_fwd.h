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

#include <cstddef>
#include <cstdint>
#include <string>

namespace daw {
	inline constexpr ptrdiff_t const dynamic_string_size = -1;

	struct StringViewBoundsPointer : std::bool_constant<true> {};
	struct StringViewBoundsSize : std::bool_constant<false> {};

	template<typename T>
	using is_string_view_bounds_type =
	  std::disjunction<std::is_same<T, StringViewBoundsPointer>,
	                   std::is_same<T, StringViewBoundsSize>>;

#if defined( _MSC_VER ) and not defined( __clang__ ) 
	// MSVC has issues with the second item being a pointer
	using default_string_view_bounds_type = StringViewBoundsSize;
#else
	using default_string_view_bounds_type = StringViewBoundsPointer;
#endif

	template<typename T>
	inline constexpr bool is_string_view_bounds_type_v =
	  is_string_view_bounds_type<T>::value;

	template<typename CharT,
	         typename BoundsType = default_string_view_bounds_type,
	         ptrdiff_t Extent = dynamic_string_size>
	struct basic_string_view;

	using string_view = basic_string_view<char>;
	using wstring_view = basic_string_view<wchar_t>;
	using u16string_view = basic_string_view<char16_t>;
	using u32string_view = basic_string_view<char32_t>;
} // namespace daw
