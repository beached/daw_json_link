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

#include <boost/optional.hpp>
#include <cstdint>
#include <functional>
#include <mutex>
#include <string>
#include <unordered_map>

#include <daw/daw_exception.h>
#include <daw/daw_parser_addons.h>
#include <daw/daw_parser_helper.h>

#include "daw_json_common.h"
#include "daw_json_parsers.h"

namespace daw {
	namespace json {
		namespace impl {
			template<typename Derived>
			struct json_link_functions_t {
				std::function<c_str_iterator( Derived &, c_str_iterator first, c_str_iterator const last )> setter;
				std::function<std::string( Derived const & )> getter;
				bool is_optional;
			};
		} // namespace impl

		template<typename Derived>
		class daw_json_link {
			using set_function_t =
			    std::function<c_str_iterator( Derived &, c_str_iterator first, c_str_iterator const last )>;
			using get_function_t = std::function<std::string( Derived & )>;

			using json_integer_t = int64_t;
			using json_real_t = double;
			using json_boolean_t = bool;
			using json_string_t = std::string;

			static std::unordered_map<std::string, impl::json_link_functions_t<Derived>> &get_map( );
			static bool &ignore_missing( );
			static std::mutex &get_mutex( ) {
				static std::mutex s_mutex;
				return s_mutex;
			}

			static auto &check_map( ) {
				std::lock_guard<std::mutex> guard( get_mutex( ) );
				if( get_map( ).empty( ) ) {
					Derived::json_link_map( );
				}
				return get_map( );
			}

			Derived const &this_as_derived( ) const {
				return *static_cast<Derived const *>( this );
			}

		  protected:
			template<typename Setter, typename Getter>
			static void link_json_integer( std::string member_name, Setter setter, Getter getter );

			template<typename Setter, typename Getter>
			static void link_json_real( std::string member_name, Setter setter, Getter getter );

		  public:
			static result_t<Derived> from_json_string( c_str_iterator first, c_str_iterator const last );
			static std::string to_json_string( Derived const &obj );
			std::string to_json_string( );
		}; // daw_json_link

		// Implementations
		//
		//
		template<typename Derived>
		std::unordered_map<std::string, impl::json_link_functions_t<Derived>> &daw_json_link<Derived>::get_map( ) {
			static std::unordered_map<std::string, impl::json_link_functions_t<Derived>> result;
			return result;
		}

		template<typename Derived>
		bool &daw_json_link<Derived>::ignore_missing( ) {
			static bool result = false;
			return result;
		}

		template<typename Derived>
		template<typename Setter, typename Getter>
		void daw_json_link<Derived>::link_json_integer( std::string member_name, Setter setter, Getter getter ) {

			auto funcs = impl::json_link_functions_t<Derived>{
			    [setter]( Derived &obj, c_str_iterator first, c_str_iterator const last ) mutable -> c_str_iterator {
				    auto result = daw::json::parsers::parse_json_integer( first, last );
				    setter( obj, std::move( result.result ) );
				    return result.position;
			    },
			    [getter, member_name]( Derived const &obj ) -> std::string {
				    using namespace std::string_literals;
				    return "\""s + member_name + "\":"s + getter( obj );
			    }};

			get_map( )[member_name] = std::move( funcs );
		}

		template<typename Derived>
		template<typename Setter, typename Getter>
		void daw_json_link<Derived>::link_json_real( std::string member_name, Setter setter, Getter getter ) {
			auto funcs = impl::json_link_functions_t<Derived>{
			    [setter]( Derived &obj, c_str_iterator first, c_str_iterator const last ) mutable -> c_str_iterator {
				    auto result = daw::json::parsers::parse_json_real( first, last );
				    setter( obj, std::move( result.result ) );
				    return result.position;
			    },
			    [getter, member_name]( Derived const &obj ) -> std::string {
				    using namespace std::string_literals;
				    return "\""s + member_name + "\":"s + getter( obj );
			    }};
			get_map( )[member_name] = std::move( funcs );
		}

		template<typename Derived>
		result_t<Derived> daw_json_link<Derived>::from_json_string( c_str_iterator first, c_str_iterator const last ) {
			auto pos = daw::parser::trim_left( first, last );
			daw::exception::daw_throw_on_false( last != pos.first, "Invalid json string.  String was empty" );
			daw::exception::daw_throw_on_false( '{' == *pos.first,
			                                    "Invalid json string.  Could not find start of object" );
			std::set<std::string> found_members;

			++pos.first;
			auto const &member_map = check_map( );
			Derived result;
			pos = daw::parser::skip_ws( pos.first, last );
			while( pos.first != last ) {
				if( '}' == *pos.first ) {
					break;
				}
				auto member_name = daw::json::parsers::parse_json_string( pos.first, last );
				auto json_link_functions_it = member_map.find( member_name.result );
				pos.first = member_name.position;
				pos = daw::parser::skip_ws( pos.first, last );
				daw::exception::daw_throw_on_false( ':' == *pos.first,
				                                    "Expected name/value separator character ':', but not found" );
				pos = daw::parser::skip_ws( ++pos.first, last );
				if( json_link_functions_it != member_map.end( ) ) {
					found_members.insert( member_name.result );
					pos.first = json_link_functions_it->second.setter( result, pos.first, last );
				} else if( !ignore_missing( ) ) {
					using namespace std::string_literals;
					daw::exception::daw_throw( "Json string contains a member name '"s + member_name.result +
					                           "' that isn't linked"s );
				} else {
					pos.first = daw::json::parsers::skip_json_value( pos.first, last ).position;
				}
				pos = daw::parser::skip_ws( pos.first, last );
				if( ',' == *pos.first ) {
					++pos.first;
				} else if( '}' != *pos.first ) {
					daw::exception::daw_throw( "Invalid Json object.  No ',' character separating members" );
				}
				pos = daw::parser::skip_ws( pos.first, last );
			}

			for( auto const &member : member_map ) {
				using namespace std::string_literals;
				daw::exception::daw_throw_on_true( !member.second.is_optional &&
				                                       found_members.count( member.first ) == 0,
				                                   "Missing non-optional member '"s + member.first + "'"s );
			}
			return result_t<Derived>{std::next( pos.first ), std::move( result )};
		}

		template<typename Derived>
		std::string daw_json_link<Derived>::to_json_string( Derived const &obj ) {
			auto const &member_map = check_map( );
			std::string result = "{";
			bool is_first = true;
			for( auto const &member_func : member_map ) {
				if( !is_first ) {
					result.push_back( ',' );
				} else {
					is_first = false;
				}
				result += member_func.second.getter( obj );
			}
			result.push_back( '}' );
			return result;
		}

		template<typename Derived>
		std::string daw_json_link<Derived>::to_json_string( ) {
			return to_json_string( this_as_derived( ) );
		}
	} // namespace json
} // namespace daw
