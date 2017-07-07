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

#include <array>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <chrono>
#include <date/date.h>
#include <string>

#include "daw_string_view.h"

namespace daw {
	namespace json {
		namespace impl {
			template<typename Duration>
			decltype( auto ) tp_to_string( std::chrono::time_point<std::chrono::system_clock, Duration> &tp,
			                               daw::string_view fmts ) {
				return date::format( fmt.to_string( ), tp );
			}

			template<typename Duration, typename Formats>
			std::string string_to_tp( std::string const str, Formats const &fmts ) {
				std::istringstream in;
				std::chrono::time_point<std::chrono::system_clock, Duration> tp;
				for( auto const &fmt : fmts ) {
					in.str( str );
					date::parse( in, fmt, tp );
					if( !in.fail( ) ) {
						break;
					}
					in.clear( );
					in.exceptions( std::ios::failbit );
				}
				daw::exception::daw_throw_on_true( in.fail( ), "Failed to parse timestamp string" );
				return tp;
			}
		} // namespace impl
	}     // namespace json
} // namespace daw

#define link_json_timestamp( json_name, member_name, formats )                                                         \
	link_json_string_fn( json_name,                                                                                    \
	                     []( auto &obj, daw::string_view value ) -> void {                                             \
		                     obj.member_name = daw::json::impl::string_to_tp( value.to_string( ), formats );           \
	                     },                                                                                            \
	                     []( auto const &obj ) -> std::decay_t<decltype( member_name )> const & {                      \
		                     return daw::json::impl::tp_to_string( obj.member_name, *std::begin( formats ) );          \
	                     } );

#define link_json_iso8601_timestamp( json_name, member_name )                                                          \
	link_json_string_fn( json_name,                                                                                    \
	                     []( auto &obj, daw::string_view value ) -> void {                                             \
		                     using namespace std::string_literals;                                                     \
		                     static constexpr std::array<std::string, 2> const fmts = {"%FT%TZ"s, "%FT%T%Ez"s};        \
		                     obj.member_name = daw::json::impl::string_to_tp( value.to_string( ), fmts );              \
	                     },                                                                                            \
	                     []( auto const &obj ) -> std::decay_t<decltype( member_name )> const & {                      \
		                     return daw::json::impl::tp_to_string( obj.member_name, "%FT%TZ" );                        \
	                     } );

