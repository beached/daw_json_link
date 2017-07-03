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
#include <string>

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
				std::function<c_str_iterator( Derived &, c_str_iterator first, c_str_iterator last )> setter;
				std::function<std::string( Derived const & )> getter;
				bool is_optional;
			};
		} // namespace impl

		template<typename Derived>
		class daw_json_link {
			template<typename T>
			using set_function_t = std::function<void( Derived &, T value )>;

			template<typename T>
			using get_function_t = std::function<T( Derived & )>;

			template<typename T>
			using optional_t = boost::optional<T>;

			using json_integer_t = int64_t;
			using json_real_t = double;
			using json_boolean_t = bool;
			using json_string_t = std::string;

			Derived &this_as_derived( );
			Derived const &this_as_derived( ) const;

			static std::unordered_map<std::string, impl::json_link_functions_t<Derived>> &get_map( );
			static bool &ignore_missing( );

		  protected:
			static void link_json_integer( set_function_t<json_integer_t> setter,
			                               get_function_t<json_integer_t> getter );

		  public:
			static result_t<Derived> from_json_string( c_str_iterator first, c_str_iterator const last );
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
		void daw_json_link<Derived>::link_json_integer( set_function_t<json_integer_t> setter,
		                                                get_function_t<json_integer_t> getter ) {}

		template<typename Derived>
		result_t<Derived> daw_json_link<Derived>::from_json_string( c_str_iterator first, c_str_iterator const last ) {
			auto pos = daw::parser::trim_left( first, last );
			daw::exception::daw_throw_on_false( last != pos.first, "Invalid json string.  String was empty" );
			daw::exception::daw_throw_on_false( '{' != *pos.first,
			                                    "Invalid json string.  Could not find start of object" );
			std::set<std::string> found_members;

			++pos.first;
			auto const &member_map = get_map( );
			Derived result;
			pos = daw::parser::skip_ws( pos.first, last );
			while( pos.first != last ) {
				if( '}' == *pos.first ) {
					return result_t<Derived>{std::next( pos.first ), std::move( result )};
				}
				auto member_name = daw::json::parsers::parse_json_string( pos.first, last );
				auto json_link_functions_it = member_map.find( member_name.result );
				pos.first = member_name.position;
				pos = daw::parser::skip_ws( pos.first, last );
				daw::exception::daw_throw_on_false( ':' == *pos.first,
				                                    "Expected name/value separator character ':', but not found" );
				pos = daw::parser::skip_ws( ++pos.first, last );
				if( json_link_functions_it != member_map.end( ) ) {
					pos.first = json_link_functions_it->second.setter( result, pos.first, last );
				} else if( !ignore_missing( ) ) {
					using namespace std::string_literals;
					daw::exception::daw_throw( "Json string contains a member name '"s + member_name.result +
					                           "' that isn't linked"s );
				} else {
					pos.first = daw::json::parsers::skip_json_value( pos.first, last );
				}
				pos = daw::parser::skip_ws( pos.first, last );
				if( ',' == *pos.first ) {
					++pos.first;
				} else if( '}' != *pos.first ) {
					daw::exception::daw_throw( "Invalid Json object.  No ',' character separating members" );
				}
				pos = daw::parser::skip_ws( pos.first, last );
				daw::exception::daw_throw_on_true( ',' == *pos.first, "Unexpected member separator character ','" );
			}

			for( auto const &member : get_map( ) ) {
				using namespace std::string_literals;
				daw::exception::daw_throw_on_true( !member.second.is_optional &&
				                                       found_members.count( member.first ) == 0,
				                                   "Missing non-optional member"s + member.first );
			}
			return {};
		}
	} // namespace json
} // namespace daw

