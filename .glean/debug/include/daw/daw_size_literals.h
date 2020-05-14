// The MIT License (MIT)
//
// Copyright (c) 2017-2020 Darrell Wright
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

namespace daw {
	namespace size_literals {
		constexpr size_t operator"" _KB( unsigned long long val ) noexcept {
			return val * 1024ull;
		}

		constexpr size_t operator"" _MB( unsigned long long val ) noexcept {
			return val * 1024ull * 1024ull;
		}

		constexpr size_t operator"" _GB( unsigned long long val ) noexcept {
			return val * 1024ull * 1024ull * 1024ull;
		}

		constexpr size_t operator"" _TB( unsigned long long val ) noexcept {
			return val * 1024ull * 1024ull * 1024ull * 1024ull;
		}

		constexpr size_t operator"" _PB( unsigned long long val ) noexcept {
			return val * 1024ull * 1024ull * 1024ull * 1024ull * 1024ull;
		}
	} // namespace size_literals
} // namespace daw
