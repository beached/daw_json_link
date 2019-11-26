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

#include <string>

#include <daw/daw_utility.h>

#include "daw_json_link_impl.h"

namespace daw::json {
	// Forward Declaration for some literal types used in json_arrays

	/**
	 * The member is a number
	 * @tparam Name name of json member
	 * @tparam T type of number(e.g. double, int, unsigned...) to pass to
	 * Constructor
	 * @tparam LiteralAsString Could this number be embedded in a string
	 * @tparam Constructor Callable used to construct result
	 * @tparam RangeCheck Check if the value will fit in the result
	 */
	template<JSONNAMETYPE Name, typename T = double,
	         LiteralAsStringOpt LiteralAsString = LiteralAsStringOpt::never,
	         typename Constructor = daw::construct_a_t<T>,
	         bool RangeCheck = false>
	struct json_number;

	/**
	 * The membrer is a boolean
	 * @tparam Name name of json member
	 * @tparam T result type to pass to Constructor
	 * @tparam LiteralAsString Could this number be embedded in a string
	 * @tparam Constructor Callable used to construct result
	 */
	template<JSONNAMETYPE Name, typename T = bool,
	         LiteralAsStringOpt LiteralAsString = LiteralAsStringOpt::never,
	         typename Constructor = daw::construct_a_t<T>>
	struct json_bool;

	/**
	 * Member is an escaped string and requires unescaping and escaping of string
	 * data
	 * @tparam Name of json member
	 * @tparam String result type constructed by Constructor
	 * @tparam Constructor a callable taking as arguments ( char const *, size_t )
	 * @tparam Appender Allows appending characters to the output object
	 * @tparam EmptyStringNull if string is empty, call Constructor with no
	 * arguments
	 */
	template<JSONNAMETYPE Name, typename String = std::string,
	         typename Constructor = daw::construct_a_t<String>,
	         typename Appender = impl::basic_appender<String>,
	         bool EmptyStringNull = false, bool DisallowHighEightBit = false>
	struct json_string;
} // namespace daw::json
