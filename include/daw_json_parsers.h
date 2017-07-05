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

#include <daw/daw_exception.h>
#include <daw/daw_parser_addons.h>
#include <daw/daw_parser_helper.h>

#include "daw_json_common.h"

namespace daw {
	namespace json {
		namespace parsers {
			namespace {
				namespace impl {
					template<typename Iterator>
					constexpr result_t<double> parse_number( Iterator first, Iterator const last ) {
						double r = 0.0;
						bool neg = false;
						if( *first == '-' ) {
							neg = true;
							++first;
						}
						while( *first >= '0' && *first <= '9' ) {
							r = ( r * 10.0 ) + ( *first - '0' );
							++first;
						}
						if( *first == '.' ) {
							double f = 0.0;
							int n = 0;
							++first;
							while( *first >= '0' && *first <= '9' ) {
								f = ( f * 10.0 ) + ( *first - '0' );
								++first;
								++n;
							}
							r += f / std::pow( 10.0, n );
						}
						if( neg ) {
							r = -r;
						}
						return {first, r};
					}

					template<typename Iterator>
					constexpr result_t<bool> parse_false( Iterator const first, Iterator const last ) {
						daw::exception::daw_throw_on_false( ( last - first ) >= 5,
						                                    "Expected boolean false, something else found" );
						auto const a = 'a' == *( first + 1 );
						auto const l = 'l' == *( first + 2 );
						auto const s = 's' == *( first + 3 );
						auto const e = 'e' == *( first + 4 );
						auto const value = a * l * s * e;
						daw::exception::dbg_throw_on_false( value != 0,
						                                    "Expected boolean false, something else found" );
						return {( first + 5 ), false};
					}

					template<typename Iterator>
					constexpr result_t<bool> parse_true( Iterator const first, Iterator const last ) {
						daw::exception::daw_throw_on_false( ( last - first ) >= 4,
						                                    "Expected boolean true, something else found" );
						auto const t = 't' == *( first + 0 );
						auto const r = 'r' == *( first + 1 );
						auto const u = 'u' == *( first + 2 );
						auto const e = 'e' == *( first + 3 );
						auto const value = t * r * u * e;
						daw::exception::dbg_throw_on_false( value != 0, "Expected boolean true, something else found" );
						return {( first + 4 ), true};
					}
				} // namespace impl
			}     // namespace

			template<typename Iterator>
			constexpr result_t<int64_t> parse_json_integer( Iterator const first, Iterator const last ) {
				auto result = impl::parse_number( first, last );
				return {result.position, static_cast<int64_t>( result.result )};
			}

			template<typename Iterator>
			constexpr result_t<double> parse_json_real( Iterator const first, Iterator const last ) {
				return impl::parse_number( first, last );
			}

			template<typename Iterator>
			result_t<bool> skip_json_value( Iterator const first, Iterator const last ) {
				daw::exception::daw_throw( "skip_json_value isn't implemented yet" );
			}

			template<typename Iterator>
			constexpr result_t<bool> is_null( Iterator const first, Iterator const last ) noexcept {
				auto const trimmed = daw::parser::trim_left( first, last );
				if( ( last - trimmed.first ) < 4 ) {
					return {trimmed.first, false};
				}
				auto const n = 'n' == *( trimmed.first + 1 );
				auto const u = 'u' == *( trimmed.first + 2 );
				auto const l = 'l' == *( trimmed.first + 3 );
				auto const l2 = 'l' == *( trimmed.first + 4 );
				auto const value = n * u * l * l2;
				return {trimmed.first, value != 0};
			}

			template<typename Iterator>
			constexpr result_t<bool> parse_json_boolean( Iterator const first, Iterator const last ) {
				auto trimmed = daw::parser::trim_left( first, last );
				if( *trimmed.begin( ) == 'f' ) {
					return impl::parse_false( trimmed.first, last );
				}
				return impl::parse_true( trimmed.first, last );
			}

			template<typename Iterator>
			constexpr result_t<std::pair<Iterator, Iterator>> parse_json_string( Iterator const first, Iterator const last ) {
				auto const parse_result = daw::parser::parse_string_literal( first, last );
				daw::exception::daw_throw_on_false( parse_result.found, "Expected string, couldn't find one" );
				return {std::next( parse_result.last ), std::make_pair( parse_result.first, parse_result.last )};
			}
		} // namespace parsers
	}     // namespace json
} // namespace daw
