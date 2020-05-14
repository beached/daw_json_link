// The MIT License (MIT)
//
// Copyright (c) 2019-2020 Darrell Wright
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
#include <type_traits>

namespace daw {
	namespace enable_if_details {
		template<class...>
		struct conjunction : std::true_type {};

		template<class B1>
		struct conjunction<B1> : B1 {};

		template<class B1, class... Bn>
		struct conjunction<B1, Bn...>
		  : std::conditional<bool( B1::value ), conjunction<Bn...>, B1>::type {};

		template<bool>
		struct enable_if {};

		template<>
		struct enable_if<true> {
			using type = std::nullptr_t;
		};
	} // namespace enable_if_details

	template<bool... B>
	using enable_when_t =
	  typename enable_if_details::enable_if<( B and ... )>::type;

	template<typename... Traits>
	using enable_when_all_t = typename enable_if_details::enable_if<
	  enable_if_details::conjunction<Traits...>::value>::type;
} // namespace daw
