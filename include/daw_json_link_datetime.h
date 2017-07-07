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

#include <daw/daw_string_view.h>

#include "daw_json_link.h"

namespace daw {
	namespace json {
		namespace impl {
			template<typename Duration>
			decltype( auto ) tp_to_string( std::chrono::time_point<std::chrono::system_clock, Duration> &tp,
			                               daw::string_view fmt ) {
				return date::format( fmt.to_string( ), tp );
			}

			template<typename Duration, typename Formats>
			auto string_to_tp( std::string const str, Formats const &fmts ) {
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

			template<typename Duration, typename Formats>
			void string_to_tp( std::string &&str,
			                   std::chrono::time_point<std::chrono::system_clock, Duration> &out_value,
			                   Formats const &fmts ) {
				out_value = string_to_tp<Duration>( std::forward<std::string>( str ), fmts );
			}

			auto const &get_iso8601_formats( ) noexcept {
				using namespace std::string_literals;
				static std::array<std::string, 2> const fmts = {{"%FT%TZ"s, "%FT%T%Ez"s}};
				return fmts;
			}

			template<typename Integer>
			constexpr auto timestamp_from_epoch( Integer i ) noexcept {
				using namespace date;
				using namespace std::chrono;
				std::chrono::system_clock::time_point const epoch{};
				auto result = epoch + milliseconds{i};
				return result;
			}

			template<typename Duration>
			constexpr int64_t
			epoch_from_timestamp( std::chrono::time_point<std::chrono::system_clock, Duration> const &t ) noexcept {
				using namespace date;
				using namespace std::chrono;
				std::chrono::system_clock::time_point const epoch{};
				return static_cast<int64_t>(
				    std::chrono::duration_cast<std::chrono::milliseconds>( t - epoch ).count( ) );
			}
		} // namespace impl
	}     // namespace json
} // namespace daw

#define link_json_timestamp( json_name, member_name, formats )                                                         \
	link_json_string_fn( json_name,                                                                                    \
	                     []( auto &obj, daw::string_view value ) -> void {                                             \
		                     daw::json::impl::string_to_tp( obj.member_name, value.to_string( ), formats );            \
	                     },                                                                                            \
	                     []( auto const &obj ) -> std::string {                                                        \
		                     return daw::json::impl::tp_to_string( obj.member_name, *std::begin( formats ) );          \
	                     } );

#define link_json_iso8601_timestamp( json_name, member_name )                                                          \
	link_json_string_fn( json_name,                                                                                    \
	                     []( auto &obj, daw::string_view value ) -> void {                                             \
		                     daw::json::impl::string_to_tp( value.to_string( ), obj.member_name,                       \
		                                                    daw::json::impl::get_iso8601_formats( ) );                 \
	                     },                                                                                            \
	                     []( auto const &obj ) -> std::string {                                                        \
		                     return daw::json::impl::tp_to_string( obj.member_name,                                    \
		                                                           daw::json::impl::get_iso8601_formats( ).front( ) ); \
	                     } );

#define link_json_timestamp_optional( json_name, member_name, formats, default_value )                                 \
	link_json_string_optional_fn(                                                                                      \
	    json_name,                                                                                                     \
	    []( auto &obj, boost::optional<daw::string_view> value ) -> void {                                             \
		    if( value ) {                                                                                              \
			    daw::json::impl::string_to_tp( value.to_string( ), *obj.member_name, formats );                        \
		    } else {                                                                                                   \
			    obj.member_name = default_value;                                                                       \
		    }                                                                                                          \
	    },                                                                                                             \
	    []( auto const &obj ) -> boost::optional<std::string> {                                                        \
		    if( obj.member_name ) {                                                                                    \
			    return daw::json::impl::tp_to_string( obj.member_name, *std::begin( formats ) );                       \
		    } else {                                                                                                   \
			    return boost::optional<std::string>{};                                                                 \
		    }                                                                                                          \
	    } );

#define link_json_iso8601_timestamp_optional( json_name, member_name, default_value )                                  \
	link_json_string_optional_fn( json_name,                                                                           \
	                              []( auto &obj, boost::optional<daw::string_view> value ) -> void {                   \
		                              if( value ) {                                                                    \
			                              daw::json::impl::string_to_tp( value.to_string( ), *obj.member_name,         \
			                                                             daw::json::impl::get_iso8601_formats( ) );    \
		                              } else {                                                                         \
			                              obj.member_name = default_value;                                             \
		                              }                                                                                \
	                              },                                                                                   \
	                              []( auto const &obj ) -> std::string {                                               \
		                              if( obj.member_name ) {                                                          \
			                              return daw::json::impl::tp_to_string(                                        \
			                                  obj.member_name, daw::json::impl::get_iso8601_formats( ).front( ) );     \
		                              } else {                                                                         \
			                              return boost::optional<std::string>{};                                       \
		                              }                                                                                \
	                              } );

#define link_json_epoch_milliseconds_timestamp( json_name, member_name )                                               \
	link_json_integer_fn(                                                                                              \
	    json_name,                                                                                                     \
	    []( auto &obj, int64_t value ) -> void { obj.member_name = daw::json::impl::timestamp_from_epoch( value ); },  \
	    []( auto const &obj ) -> int64_t { return daw::json::impl::epoch_from_timestamp( obj.member_name ); } );

