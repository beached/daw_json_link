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

#include "daw_json_parsers.h"

namespace daw {
	namespace json {
		namespace parsers {
			bool is_null( c_str_iterator first, c_str_iterator const last ) {
				boost::string_view const null_str = "null";
				auto const trimmed = daw::parser::trim_left( first, last );
				auto pos = std::search( trimmed.begin( ), trimmed.end( ), null_str.begin( ), null_str.end( ) );
				if( pos != last ) {
					pos = std::next( pos, null_str.size( ) );
					return daw::parser::make_find_result( std::move( pos ), std::move( last ), true );
				}
				return daw::parser::make_find_result( std::move( first ), std::move( last ), false );
			}

			result_t<std::string> parse_json_string( c_str_iterator first, c_str_iterator const last ) {
				auto const parse_result = daw::parser::parse_string_literal( first, last );
				daw::exception::daw_throw_on_false( parse_result.found, "Expected string, couldn't find one" );
				return result_t<std::string>{std::next(parse_result.last), std::string{parse_result.first, parse_result.last}};
			}

			result_t<int64_t> parse_json_integer( c_str_iterator first, c_str_iterator const last ) {
				auto const parse_result = daw::parser::find_number( first, last );
				daw::exception::daw_throw_on_false( parse_result.result, "Expected integer, couldn't find one" );
				int64_t destination;
				daw::parser::parse_int( parse_result.first, parse_result.last, destination );
				return result_t<int64_t>{parse_result.last, std::move( destination )};
			}

			result_t<double> parse_json_real( c_str_iterator first, c_str_iterator const last ) {
				auto const parse_result = daw::parser::find_number( first, last );
				daw::exception::daw_throw_on_false( parse_result.result, "Expected integer, couldn't find one" );
				daw::parser::parse_int( parse_result.first, parse_result.last, destination );
				return result_t<double>{parse_result.last, std::strod( parse_result.first )};
			}

			namespace impl {
				result_t<bool> parse_true( c_str_iterator first, c_str_iterator const last ) {
					boost::string_view const true_str = "true";
					auto const found = std::search( first, last, true_str.begin( ), true_str.end( ) ) != last;
					daw::exception::daw_throw_on_false( found, "Expected string 'true', did not find it" );
					return result_t<bool>{std::next( first, true_str.size( ) ), true};
				}

				result_t<bool> parse_false( c_str_iterator first, c_str_iterator const last ) {
					boost::string_view const false_str = "false";
					auto const found = std::search( first, last, false_str.begin( ), false_str.end( ) ) != last;
					daw::exception::daw_throw_on_false( found, "Expected string 'false', did not find it" );
					return result_t<bool>{std::next( first, false_str.size( ) ), false};
				}
			} // namespace impl

			result_t<bool> parse_json_bool( c_str_iterator first, c_str_iterator const last ) {
				auto trimmed = daw::parser::trim_left( first, last );
				if( *trimmed == 't' ) {
					return parse_true( trimmed, last );
				} else if( *trimmed == 'f' ) {
					return parse_false( trimmed, last );
				}
				daw::exception::daw_throw( "Expected boolean, couldn't find one" );
			}

			result_t<bool> skip_json_value( c_str_iterator first, c_str_iterator const last ) {
				daw::exception::daw_throw( "skip_json_value isn't implemented yet" );
			}
		} // namespace parsers
	}     // namespace json
} // namespace daw

