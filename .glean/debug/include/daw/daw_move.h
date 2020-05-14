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

#include <type_traits>

namespace daw {
	/// Convert a value to an rvalue.
	/// \param  value  A thing of arbitrary type.
	/// \return The parameter cast to an rvalue-reference to allow moving it.
	template<typename T>
	[[nodiscard]] constexpr typename std::remove_reference<T>::type &&
	move( T &&value ) noexcept {
		static_assert(
		  not std::is_const<typename std::remove_reference<T>::type>::value,
		  "Attempt to move const value" );
		static_assert( not std::is_rvalue_reference<decltype( value )>::value,
		               "Value is already an rvalue" );
		return static_cast<typename std::remove_reference<T>::type &&>( value );
	}
} // namespace daw
