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
#include <vector>

#include <daw/daw_exception.h>
#include <daw/daw_parser_addons.h>
#include <daw/daw_parser_helper.h>
#include <daw/daw_utility.h>

#include "daw_json_common.h"
#include "daw_json_parsers.h"

namespace daw {
	namespace json {
		namespace impl {
			template<typename Derived>
			struct json_link_functions_t {
				using setter_t =
				    std::function<c_str_iterator( Derived &, c_str_iterator first, c_str_iterator const last )>;
				using getter_t = std::function<std::string( Derived const & )>;

				setter_t setter;
				getter_t getter;
				bool is_optional;
			};

			template<typename Derived>
			constexpr auto make_json_link_functions( typename json_link_functions_t<Derived>::setter_t setter, typename json_link_functions_t<Derived>::getter_t getter,
			                          bool is_optional = false ) {
				return json_link_functions_t<Derived>{std::move( setter ), std::move( getter ), is_optional};
			}

			template<typename Iterator>
			std::string to_string( daw::json::result_t<std::pair<Iterator, Iterator>> result ) {
				return std::string{ result.result.first, result.result.second };
			}
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

			struct json_link_functions_info {
				std::string name;
				size_t hash;
				impl::json_link_functions_t<Derived> link_functions;

				json_link_functions_info( boost::string_view n, impl::json_link_functions_t<Derived> funcs ):
					name{ n.to_string( ) },
					hash{ std::hash<std::string>{ }( name ) },
					link_functions{ std::move( funcs ) } { }
			};
			using json_link_functions_data_t = std::vector<json_link_functions_info>;

			static json_link_functions_data_t & get_link_data( ) {
				static json_link_functions_data_t result;
				return result;
			}

			template<typename LinkData>
			static auto find_link_func_member( LinkData const & link_data, size_t hash ) {
				auto result = std::find_if( link_data.begin( ), link_data.end( ), [hash]( auto const & v ) { return hash == v.hash; } );
				return std::make_pair( result != link_data.end( ), result );
			}

			constexpr static decltype( auto ) json_link_data_size( ) noexcept {
				return get_link_data( ).size( );
			}

			static void add_json_link_function( boost::string_view name, impl::json_link_functions_t<Derived> link_functions ) {
				get_link_data( ).emplace_back( name, std::move( link_functions ) );
			}

			static bool &ignore_missing( );
			static std::mutex &get_mutex( ) {
				static std::mutex s_mutex;
				return s_mutex;
			}

			static json_link_functions_data_t &check_map( ) {
				std::lock_guard<std::mutex> guard( get_mutex( ) );
				if( get_link_data( ).empty( ) ) {
					Derived::json_link_map( );
				}
				return get_link_data( );
			}

			Derived const &this_as_derived( ) const {
				return *static_cast<Derived const *>( this );
			}

		  protected:
			template<typename Setter, typename Getter>
			static void link_json_integer_fn( boost::string_view member_name, Setter setter, Getter getter );

			template<typename Setter, typename Getter>
			static void link_json_real_fn( boost::string_view member_name, Setter setter, Getter getter );

			template<typename Setter, typename Getter>
			static void link_json_boolean_fn( boost::string_view member_name, Setter setter, Getter getter );

			template<typename Setter, typename Getter>
			static void link_json_string_fn( boost::string_view member_name, Setter setter, Getter getter );

			template<typename Setter, typename Getter>
			static void link_json_object_fn( boost::string_view member_name, Setter setter, Getter getter );

		  public:
			static result_t<Derived> from_json_string( c_str_iterator first, c_str_iterator const last );
			static std::vector<Derived> from_json_array_string( c_str_iterator first, c_str_iterator const last );
			static std::string to_json_string( Derived const &obj );
			std::string to_json_string( ) const;
		}; // daw_json_link

		// Implementations
		//
		//
		template<typename Derived>
		bool &daw_json_link<Derived>::ignore_missing( ) {
			static bool result = false;
			return result;
		}

		template<typename Derived>
		template<typename Setter, typename Getter>
		void daw_json_link<Derived>::link_json_integer_fn( boost::string_view member_name, Setter setter, Getter getter ) {
			using member_t = std::decay_t<decltype( getter( std::declval<Derived>( ) ) )>;
			auto funcs = impl::make_json_link_functions<Derived>(
			    [setter]( Derived &obj, c_str_iterator first, c_str_iterator const last ) mutable -> c_str_iterator {
				    auto result = daw::json::parsers::parse_json_integer( first, last );
					daw::exception::dbg_throw_on_false( daw::can_fit<member_t>( result.result ), "Invalid json string.  String was empty" );
				    setter( obj, static_cast<member_t>( result.result ) );
				    return result.position;
			    },
			    [getter]( Derived const &obj ) -> std::string {
				    using std::to_string;
				    return to_string( getter( obj ) );
			    } );

			add_json_link_function( member_name, std::move( funcs ) );
		}

		template<typename Derived>
		template<typename Setter, typename Getter>
		void daw_json_link<Derived>::link_json_real_fn( boost::string_view member_name, Setter setter, Getter getter ) {
			auto funcs = impl::make_json_link_functions<Derived>(
			    [setter]( Derived &obj, c_str_iterator first, c_str_iterator const last ) mutable -> c_str_iterator {
				    auto result = daw::json::parsers::parse_json_real( first, last );
				    setter( obj, std::move( result.result ) );
				    return result.position;
			    },
			    [getter]( Derived const &obj ) -> std::string {
					using std::to_string;
					return to_string( getter( obj ) );
			    });
			add_json_link_function( member_name, std::move( funcs ) );
		}

		template<typename Derived>
		template<typename Setter, typename Getter>
		void daw_json_link<Derived>::link_json_boolean_fn( boost::string_view member_name, Setter setter, Getter getter ) {
			auto funcs = impl::make_json_link_functions<Derived>(
			    [setter]( Derived &obj, c_str_iterator first, c_str_iterator const last ) mutable -> c_str_iterator {
				    auto result = daw::json::parsers::parse_json_boolean( first, last );
				    setter( obj, std::move( result.result ) );
				    return result.position;
			    },
			    [getter]( Derived const &obj ) -> std::string {
					using std::to_string;
				    return getter( obj ) ? "true" : "false";
			    });
			add_json_link_function( member_name, std::move( funcs ) );
		}

		template<typename Derived>
		template<typename Setter, typename Getter>
		void daw_json_link<Derived>::link_json_string_fn( boost::string_view member_name, Setter setter, Getter getter ) {
			auto funcs = impl::make_json_link_functions<Derived>(
			    [setter]( Derived &obj, c_str_iterator first, c_str_iterator const last ) mutable -> c_str_iterator {
				    auto result = daw::json::parsers::parse_json_string( first, last );
				    setter( obj, std::string{ result.result.first, result.result.second } );
				    return result.position;
			    },
			    [getter]( Derived const &obj ) -> std::string {
					using namespace std::string_literals;
				    return "\""s + getter( obj ) + "\""s;
			    });
			add_json_link_function( member_name, std::move( funcs ) );
		}

		template<typename Derived>
		template<typename Setter, typename Getter>
		void daw_json_link<Derived>::link_json_object_fn( boost::string_view member_name, Setter setter, Getter getter ) {
			using member_t = std::decay_t<decltype( getter( std::declval<Derived>( ) ) )>;
			auto funcs = impl::make_json_link_functions<Derived>(
			    [setter]( Derived &obj, c_str_iterator first, c_str_iterator const last ) mutable -> c_str_iterator {
					auto result = member_t::from_json_string( first, last );
				    setter( obj, std::move( result.result ) );
				    return result.position;
			    },
			    [getter]( Derived const &obj ) -> std::string {
				    return getter( obj ).to_json_string( );
			    });
			add_json_link_function( member_name, std::move( funcs ) );
		}

		template<typename Derived>
		result_t<Derived> daw_json_link<Derived>::from_json_string( c_str_iterator first, c_str_iterator const last ) {
			auto pos = daw::parser::trim_left( first, last );
			daw::exception::daw_throw_on_false( last != pos.first, "Invalid json string.  String was empty" );
			daw::exception::daw_throw_on_false( '{' == *pos.first,
			                                    "Invalid json string.  Could not find start of object" );
			auto const &member_map = check_map( );

			std::vector<size_t> found_members;
			found_members.reserve( member_map.size( ) );

			++pos.first;
			Derived result;
			pos = daw::parser::skip_ws( pos.first, last );
			while( pos.first != last ) {
				if( '}' == *pos.first ) {
					break;
				}
				auto const member_name = daw::json::parsers::parse_json_string( pos.first, last );
				auto const member_name_str = impl::to_string( member_name );
				auto const member_name_hash = std::hash<std::string>{}( member_name_str );


				pos.first = member_name.position;
				pos = daw::parser::skip_ws( pos.first, last );
				daw::exception::daw_throw_on_false( ':' == *pos.first,
				                                    "Expected name/value separator character ':', but not found" );

				pos = daw::parser::skip_ws( ++pos.first, last );

				auto const json_link_function = find_link_func_member( member_map, member_name_hash );
				if( json_link_function.first ) {
					found_members.push_back( member_name_hash );
					pos.first = json_link_function.second->link_functions.setter( result, pos.first, last );
				} else if( !ignore_missing( ) ) {
					using namespace std::string_literals;
					daw::exception::daw_throw( "Json string contains a member name '"s + member_name_str +
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

			auto const member_found = [&found_members]( size_t hash ) -> bool {
				return std::find( found_members.cbegin( ), found_members.cend( ), hash ) != found_members.cend( );
			};

			for( auto const &member : member_map ) {
				using namespace std::string_literals;
				daw::exception::daw_throw_on_true( !member.link_functions.is_optional && !member_found( member.hash ),
				                                   "Missing non-optional member '"s + member.name + "'"s );
			}
			return result_t<Derived>{std::next( pos.first ), std::move( result )};
		}

		template<typename Derived>
		std::vector<Derived> daw_json_link<Derived>::from_json_array_string( c_str_iterator first, c_str_iterator const last ) {
			std::vector<Derived> result;
			auto pos = daw::parser::skip_ws( first, last );
			daw::exception::daw_throw_on_false( '[' == *pos.first, "Expected json array but none found" );
			pos = daw::parser::skip_ws( ++pos.first, last );
			while( ']' != *pos.first ) {
				daw::exception::daw_throw_on_false( '{' == *pos.first, "Expected start of json object" );
				auto item = from_json_string( pos.first, last );
				result.push_back( std::move( item.result ) );
				pos = daw::parser::skip_ws( item.position, last );
				if( ',' == *pos.first ) {
					pos = daw::parser::skip_ws( ++pos.first, last );
				}
			}
			return result;
		}

		template<typename Derived>
		std::string daw_json_link<Derived>::to_json_string( Derived const &obj ) {
			auto const &member_map = check_map( );
			std::string result = "{";
			using namespace std::string_literals;
			bool is_first = true;
			for( auto const &member_func : member_map ) {
				if( !is_first ) {
					result.push_back( ',' );
				} else {
					is_first = false;
				}
				result += "\""s + member_func.name + "\":"s + member_func.link_functions.getter( obj );
			}
			result.push_back( '}' );
			return result;
		}

		template<typename Derived>
		std::string daw_json_link<Derived>::to_json_string( ) const {
			return to_json_string( this_as_derived( ) );
		}

		template<typename Derived>
		std::string to_json_string( daw_json_link<Derived> const & obj ) {
			return obj.to_json_string( );
		}

		template<typename Container>
		std::string to_json_string( Container const & container ) {
			std::string result = "[";
			using std::begin;
			using std::end;
			auto it = begin( container );
			if( it != end( container ) ) {
				result += it->to_json_string( );
				for( ; it != end( container ); ++it ) {
					result += ',' + it->to_json_string( );
				}
			}
			result += ']';
			return result;
		}
	} // namespace json
} // namespace daw
