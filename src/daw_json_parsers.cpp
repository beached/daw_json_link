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

#include <boost/utility/string_view.hpp>
#include <iterator>
#include <string>

#include <daw/daw_exception.h>
#include <daw/daw_parser_addons.h>
#include <daw/daw_parser_helper.h>

#include "daw_json_parsers.h"

namespace daw {
	namespace json {
		namespace parsers {
			result_t<bool> is_null( c_str_iterator first, c_str_iterator const last ) {
				boost::string_view const null_str = "null";
				auto const trimmed = daw::parser::trim_left( first, last );
				auto pos = std::search( trimmed.begin( ), trimmed.end( ), null_str.begin( ), null_str.end( ) );
				if( pos != last ) {
					pos = std::next(
					    pos, static_cast<std::iterator_traits<decltype( pos )>::difference_type>( null_str.size( ) ) );
					return { std::move( pos ), true };
				}
				return { std::move( pos ), false };
			}

			result_t<std::string> parse_json_string( c_str_iterator first, c_str_iterator const last ) {
				auto const parse_result = daw::parser::parse_string_literal( first, last );
				daw::exception::daw_throw_on_false( parse_result.found, "Expected string, couldn't find one" );
				return result_t<std::string>{std::next(parse_result.last), std::string{parse_result.first, parse_result.last}};
			}

			namespace {
				namespace impl {
					auto to_char_const_ptr( char const * ptr ) {
						return ptr;
					}
				}	// namespace impl
			}	// namespace anonymous

			result_t<int64_t> parse_json_integer( c_str_iterator first, c_str_iterator const last ) {
				auto const parse_result = daw::parser::skip_ws( first, last );
				daw::exception::daw_throw_on_true( parse_result.first == parse_result.last, "Unexpected empty string" );
				char * end_char;
				int64_t result = static_cast<int64_t>( strtod( parse_result.begin( ), &end_char ) );
				return result_t<int64_t>{ parse_result.begin( ) + std::distance( parse_result.begin( ), impl::to_char_const_ptr( end_char ) ), std::move( result ) };
			}

			result_t<double> parse_json_real( c_str_iterator first, c_str_iterator const last ) {
				auto const parse_result = daw::parser::skip_ws( first, last );
				daw::exception::daw_throw_on_true( parse_result.first == parse_result.last, "Unexpected empty string" );
				char * end_char;
				double result = strtod( parse_result.begin( ), &end_char );
				return result_t<double>{ parse_result.begin( ) + std::distance( parse_result.begin( ), impl::to_char_const_ptr( end_char ) ), std::move( result ) };
			}

			namespace {
				namespace impl {
					result_t<bool> parse_true( c_str_iterator first, c_str_iterator const last ) {
						boost::string_view const true_str = "true";
						auto pos = std::search( first, last, true_str.begin( ), true_str.end( ) );
						daw::exception::daw_throw_on_false( pos != last, "Expected string 'true', did not find it" );
						return result_t<bool>{
						    std::next( pos, static_cast<std::iterator_traits<decltype( first )>::difference_type>(
						                        true_str.size( ) ) ),
						    true};
					}

					result_t<bool> parse_false( c_str_iterator first, c_str_iterator const last ) {
						boost::string_view const false_str = "false";
						auto pos = std::search( first, last, false_str.begin( ), false_str.end( ) );
						daw::exception::daw_throw_on_false( pos != last, "Expected string 'false', did not find it" );
						return result_t<bool>{
						    std::next( pos, static_cast<std::iterator_traits<decltype( first )>::difference_type>(
						                        false_str.size( ) ) ),
						    false};
					}
				} // namespace impl
			}	// namespace

			result_t<bool> parse_json_boolean( c_str_iterator first, c_str_iterator const last ) {
				auto trimmed = daw::parser::trim_left( first, last );
				if( *trimmed.begin( ) == 't' ) {
					return impl::parse_true( trimmed.first, last );
				} else if( *trimmed.begin( ) == 'f' ) {
					return impl::parse_false( trimmed.first, last );
				}
				daw::exception::daw_throw( "Expected boolean, couldn't find one" );
			}

			result_t<bool> skip_json_value( c_str_iterator first, c_str_iterator const last ) {
				daw::exception::daw_throw( "skip_json_value isn't implemented yet" );
			}
		} // namespace parsers
	}     // namespace json
} // namespace daw

