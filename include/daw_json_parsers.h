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

#include <boost/utility/string_view.hpp>
#include <string>

#include <daw/daw_exception.h>
#include <daw/daw_parser_addons.h>
#include <daw/daw_parser_helper.h>

#include "daw_json_common.h"

namespace daw {
	namespace json {
		namespace parsers {
			result_t<bool> is_null( c_str_iterator first, c_str_iterator const last );
			result_t<std::string> parse_json_string( c_str_iterator first, c_str_iterator const last );
			result_t<int64_t> parse_json_integer( c_str_iterator first, c_str_iterator const last );
			result_t<double> parse_json_real( c_str_iterator first, c_str_iterator const last );
			result_t<bool> parse_json_bool( c_str_iterator first, c_str_iterator const last );
			result_t<bool> skip_json_value( c_str_iterator first, c_str_iterator const last );
		} // namespace parsers
	}     // namespace json
} // namespace daw

