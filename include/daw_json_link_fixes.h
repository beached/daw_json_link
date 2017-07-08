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

#include <string>

#include <daw/daw_string_view.h>

#include "daw_json_parsers.h"
#include "daw_json_link.h"

#define link_json_quoted_integer( json_name, member_name, formats )                                                 \
	link_json_string_fn( json_name,                                                                                    \
	                     []( auto &obj, daw::string_view value ) -> void {                                             \
		                     auto number_result = daw::json::impl::parse_number( value );                              \
		                     obj.member_name =                                                                         \
		                         static_cast<std::decay_t<decltype( obj.member_name )>>( number_result.result );       \
	                     },                                                                                            \
	                     []( auto const &obj ) -> std::string {                                                        \
		                     using namespace std::string_literals;                                                     \
		                     using std::to_string;                                                                     \
		                     return "\""s + to_string( obj.member_name ) + "\""s;                                      \
	                     } );

#define link_json_quoted_real( json_name, member_name, formats )                                                 \
	link_json_string_fn( json_name,                                                                                    \
	                     []( auto &obj, daw::string_view value ) -> void {                                             \
		                     auto number_result = daw::json::impl::parse_number( value );                              \
		                     obj.member_name =                                                                         \
		                         static_cast<std::decay_t<decltype( obj.member_name )>>( number_result.result );       \
	                     },                                                                                            \
	                     []( auto const &obj ) -> std::string {                                                        \
		                     using namespace std::string_literals;                                                     \
		                     using std::to_string;                                                                     \
		                     return "\""s + to_string( obj.member_name ) + "\""s;                                      \
	                     } );


