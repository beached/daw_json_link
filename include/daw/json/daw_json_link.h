// The MIT License (MIT)
//
// Copyright (c) 2017-2018 Darrell Wright
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files( the "Software" ), to
// deal in the Software without restriction, including without limitation the
// rights to use, copy, modify, merge, publish, distribute, sublicense, and / or
// sell copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#pragma once

#include <boost/lexical_cast.hpp>
#include <boost/optional.hpp>
#include <cstdint>
#include <functional>
#include <iostream>
#include <iterator>
#include <mutex>
#include <sstream>
#include <string>
#include <type_traits>
#include <unordered_map>
#include <vector>

#include <daw/daw_exception.h>
#include <daw/daw_parser_addons.h>
#include <daw/daw_parser_helper.h>
#include <daw/daw_string_view.h>
#include <daw/daw_utility.h>

#include "daw_json_common.h"
#include "daw_json_parsers.h"

namespace daw {
	namespace json {
		template<typename Derived>
		class daw_json_link {
			struct json_link_functions_info {
				size_t hash;
				std::string name;
				using setter_t =
				  std::function<daw::string_view( Derived &, daw::string_view view )>;
				using getter_t = std::function<std::string( Derived const & )>;
				setter_t setter;
				getter_t getter;
				bool is_optional;

				constexpr json_link_functions_info( daw::string_view n,
				                                    setter_t setter_func,
				                                    getter_t getter_func,
				                                    bool optional = false )
				  : hash{std::hash<daw::string_view>{}( n )}
				  , name{n.to_string( )}
				  , setter{std::move( setter_func )}
				  , getter{std::move( getter_func )}
				  , is_optional{optional} {}
			}; // json_link_functions_info
			using json_link_functions_data_t = std::vector<json_link_functions_info>;

			// Do not call outside of json_link_map chain.  Use check_map
			static json_link_functions_data_t &get_link_data( ) {
				static json_link_functions_data_t result;
				return result;
			}

			template<typename LinkData>
			static constexpr auto find_link_func_member( LinkData const &link_data,
			                                             size_t const hash ) {
				auto result =
				  std::find_if( link_data.cbegin( ), link_data.cend( ),
				                [hash]( auto const &v ) { return hash == v.hash; } );
				return std::make_pair( result != link_data.cend( ), result );
			}

			static constexpr decltype( auto ) json_link_data_size( ) noexcept {
				return get_link_data( ).size( );
			}

			static void add_json_link_function(
			  daw::string_view name,
			  typename json_link_functions_info::setter_t setter,
			  typename json_link_functions_info::getter_t getter,
			  bool is_optional = false ) {
				get_link_data( ).emplace_back( name, std::move( setter ),
				                               std::move( getter ), is_optional );
			}

			static bool &ignore_missing( ) noexcept;

			static json_link_functions_data_t const &check_map( ) {
				static std::once_flag flag;
				std::call_once( flag, []( ) { Derived::json_link_map( ); } );
				return get_link_data( );
			}

			constexpr Derived const &this_as_derived( ) const noexcept {
				return *static_cast<Derived const *>( this );
			}

		protected:
			template<typename Setter, typename Getter>
			static void link_json_integer_fn( daw::string_view member_name,
			                                  Setter setter, Getter getter );

			template<typename Setter, typename Getter>
			static void link_json_integer_optional_fn( daw::string_view member_name,
			                                           Setter setter, Getter getter );

			template<typename Setter, typename Getter>
			static void link_json_real_fn( daw::string_view member_name,
			                               Setter setter, Getter getter );

			template<typename Setter, typename Getter>
			static void link_json_real_optional_fn( daw::string_view member_name,
			                                        Setter setter, Getter getter );

			template<typename Setter, typename Getter>
			static void link_json_boolean_fn( daw::string_view member_name,
			                                  Setter setter, Getter getter );

			template<typename Setter, typename Getter>
			static void link_json_boolean_optional_fn( daw::string_view member_name,
			                                           Setter setter, Getter getter );

			template<typename Setter, typename Getter>
			static void link_json_string_fn( daw::string_view member_name,
			                                 Setter setter, Getter getter );

			template<typename Setter, typename Getter>
			static void link_json_string_optional_fn( daw::string_view member_name,
			                                          Setter setter, Getter getter );

			template<typename Setter, typename Getter>
			static void link_json_object_fn( daw::string_view member_name,
			                                 Setter setter, Getter getter );

			template<typename Setter, typename Getter>
			static void link_json_object_optional_fn( daw::string_view member_name,
			                                          Setter setter, Getter getter );

			template<typename Setter, typename Getter>
			static void link_json_integer_array_fn( daw::string_view member_name,
			                                        Setter item_setter,
			                                        Getter getter );

			template<typename Setter, typename Getter>
			static void link_json_real_array_fn( daw::string_view member_name,
			                                     Setter item_setter, Getter getter );

			template<typename Setter, typename Getter>
			static void link_json_boolean_array_fn( daw::string_view member_name,
			                                        Setter item_setter,
			                                        Getter getter );

			template<typename Setter, typename Getter>
			static void link_json_string_array_fn( daw::string_view member_name,
			                                       Setter item_setter,
			                                       Getter getter );

			template<typename Setter, typename Getter>
			static void link_json_object_array_fn( daw::string_view member_name,
			                                       Setter item_setter,
			                                       Getter getter );

		public:
			static result_t<Derived> from_json_string( daw::string_view view );
			static std::vector<Derived>
			from_json_array_string( daw::string_view view );

			static std::string to_json_string( Derived const &obj );
			std::string to_json_string( ) const;
		}; // daw_json_link

		// Implementations
		//
		//
		template<typename Derived>
		bool &daw_json_link<Derived>::ignore_missing( ) noexcept {
			static bool result = false;
			return result;
		}

		template<typename Derived>
		result_t<Derived>
		daw_json_link<Derived>::from_json_string( daw::string_view view ) {
			view = parser::impl::skip_ws( view );
			daw::exception::daw_throw_on_true(
			  view.empty( ), "Invalid json string.  String was empty" );
			daw::exception::daw_throw_on_false(
			  '{' == view.front( ),
			  "Invalid json string.  Could not find start of object" );
			auto const &member_map = check_map( );

			std::vector<size_t> found_members;
			found_members.reserve( member_map.size( ) );

			view.remove_prefix( );
			view = parser::impl::skip_ws( view );
			Derived result;

			while( !view.empty( ) ) {
				if( '}' == view.front( ) ) {
					break;
				}
				auto const member_name = daw::json::parser::parse_json_string( view );
				auto const member_name_hash =
				  std::hash<daw::string_view>{}( member_name.result );

				view = member_name.view;
				view = parser::impl::skip_ws( view );

				daw::exception::daw_throw_on_false(
				  ':' == view.front( ),
				  "Expected name/value separator character ':', but not found" );

				view.remove_prefix( );
				view = parser::impl::skip_ws( view );

				auto const &json_link_function =
				  find_link_func_member( member_map, member_name_hash );
				if( json_link_function.first ) {
					found_members.emplace_back( member_name_hash );
					view = json_link_function.second->setter( result, view );
				} else if( !ignore_missing( ) ) {
					using namespace std::string_literals;
					daw::exception::daw_throw( "Json string contains a member name '"s +
					                           member_name.result +
					                           "' that isn't linked"s );
				} else {
					view = daw::json::parser::skip_json_value( view ).view;
				}

				view = parser::impl::skip_ws( view );
				if( ',' == view.front( ) ) {
					view.remove_prefix( );
				} else if( '}' != view.front( ) ) {
					daw::exception::daw_throw(
					  "Invalid Json object.  No ',' character separating members" );
				}
				view = parser::impl::skip_ws( view );
			}

			auto const member_found = [&found_members]( size_t hash ) -> bool {
				return std::find( found_members.cbegin( ), found_members.cend( ),
				                  hash ) != found_members.cend( );
			};

			for( auto const &member : member_map ) {
				using namespace std::string_literals;
				daw::exception::daw_throw_on_true(
				  !member.is_optional && !member_found( member.hash ),
				  "Missing non-optional member '"s + member.name + "'"s );
			}
			view = parser::impl::skip_ws( view );
			view.remove_prefix( );
			return result_t<Derived>{view, std::move( result )};
		}

		template<typename Derived>
		std::vector<Derived>
		daw_json_link<Derived>::from_json_array_string( daw::string_view view ) {

			std::vector<Derived> result;
			view = parser::impl::skip_ws( view );
			daw::exception::daw_throw_on_false(
			  '[' == view.front( ), "Expected json array but none found" );
			view.remove_prefix( );
			view = parser::impl::skip_ws( view );

			while( ']' != view.front( ) ) {
				auto item = from_json_string( view );
				result.emplace_back( std::move( item.result ) );
				view = item.view;
				view = parser::impl::skip_ws( view );

				if( ',' == view.front( ) ) {
					view.remove_prefix( );
					view = parser::impl::skip_ws( view );
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
				result += "\""s + member_func.name + "\":"s + member_func.getter( obj );
			}
			result.push_back( '}' );
			return result;
		}

		template<typename Derived>
		std::string daw_json_link<Derived>::to_json_string( ) const {
			return to_json_string( this_as_derived( ) );
		}

		template<typename Derived>
		std::string to_json_string( daw_json_link<Derived> const &obj ) {
			return obj.to_json_string( );
		}

		template<typename Container>
		std::string to_json_string( Container const &container ) {
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

		template<typename Derived>
		template<typename Setter, typename Getter>
		void daw_json_link<Derived>::link_json_integer_fn(
		  daw::string_view member_name, Setter setter, Getter getter ) {
			using member_t =
			  std::decay_t<decltype( getter( std::declval<Derived>( ) ) )>;
			add_json_link_function(
			  member_name,
			  [setter]( Derived &obj, daw::string_view view ) -> daw::string_view {
				  auto result = daw::json::parser::parse_json_integer( view );
				  daw::exception::dbg_throw_on_false(
				    daw::can_fit<member_t>( result.result ),
				    "Invalid json string.  String was empty" );
				  setter( obj, static_cast<member_t>( result.result ) );
				  view = result.view;
				  return view;
			  },
			  [getter]( Derived const &obj ) -> std::string {
				  using std::to_string;
				  return to_string( getter( obj ) );
			  } );
		}

		template<typename Derived>
		template<typename Setter, typename Getter>
		void daw_json_link<Derived>::link_json_integer_optional_fn(
		  daw::string_view member_name, Setter setter, Getter getter ) {
			add_json_link_function(
			  member_name,
			  [setter]( Derived &obj, daw::string_view view ) -> daw::string_view {
				  auto result = daw::json::parser::parse_json_integer_optional( view );
				  setter( obj, result.result );
				  return result.view;
			  },
			  [getter]( Derived const &obj ) -> std::string {
				  using namespace std::string_literals;
				  auto value = getter( obj );
				  if( value ) {
					  using std::to_string;
					  return to_string( *value );
				  }
				  return "null";
			  },
			  true );
		}

		template<typename Derived>
		template<typename Setter, typename Getter>
		void
		daw_json_link<Derived>::link_json_real_fn( daw::string_view member_name,
		                                           Setter setter, Getter getter ) {
			add_json_link_function(
			  member_name,
			  [setter]( Derived &obj, daw::string_view view ) -> daw::string_view {
				  auto result = daw::json::parser::parse_json_real( view );
				  setter( obj, result.result );
				  view = result.view;
				  return view;
			  },
			  [getter]( Derived const &obj ) -> std::string {
				  using std::to_string;
				  return to_string( getter( obj ) );
			  } );
		}

		template<typename Derived>
		template<typename Setter, typename Getter>
		void daw_json_link<Derived>::link_json_real_optional_fn(
		  daw::string_view member_name, Setter setter, Getter getter ) {
			add_json_link_function(
			  member_name,
			  [setter]( Derived &obj, daw::string_view view ) -> daw::string_view {
				  auto result = daw::json::parser::parse_json_real_optional( view );
				  setter( obj, result.result );
				  return result.view;
			  },
			  [getter]( Derived const &obj ) -> std::string {
				  using namespace std::string_literals;
				  auto value = getter( obj );
				  if( value ) {
					  using std::to_string;
					  return to_string( *value );
				  }
				  return "null";
			  },
			  true );
		}

		template<typename Derived>
		template<typename Setter, typename Getter>
		void daw_json_link<Derived>::link_json_boolean_fn(
		  daw::string_view member_name, Setter setter, Getter getter ) {
			add_json_link_function(
			  member_name,
			  [setter]( Derived &obj, daw::string_view view ) -> daw::string_view {
				  auto result = daw::json::parser::parse_json_boolean( view );
				  setter( obj, result.result );
				  view = result.view;
				  return view;
			  },
			  [getter]( Derived const &obj ) -> std::string {
				  using std::to_string;
				  return getter( obj ) ? "true" : "false";
			  } );
		}

		template<typename Derived>
		template<typename Setter, typename Getter>
		void daw_json_link<Derived>::link_json_boolean_optional_fn(
		  daw::string_view member_name, Setter setter, Getter getter ) {
			add_json_link_function(
			  member_name,
			  [setter]( Derived &obj, daw::string_view view ) -> daw::string_view {
				  auto result = daw::json::parser::parse_json_boolean_optional( view );
				  setter( obj, result.result );
				  return result.view;
			  },
			  [getter]( Derived const &obj ) -> std::string {
				  using namespace std::string_literals;
				  auto value = getter( obj );
				  if( value ) {
					  using std::to_string;
					  return to_string( *value );
				  }
				  return "null";
			  },
			  true );
		}

		template<typename CharT, typename TraitT>
		auto unescape_string( daw::basic_string_view<CharT, TraitT> src ) {
			using result_t = std::basic_string<CharT, TraitT>;
			if( src.empty( ) ) {
				return result_t{};
			}
			result_t result;
			result.reserve( src.size( ) );
			for( size_t n = 0; n < src.size( ); ++n ) {
				if( src[n] == '\\' ) {
					++n;
					switch( src[n] ) {
					case 'b':
						result += '\b';
						break;
					case 'f':
						result += '\f';
						break;
					case 'n':
						result += '\n';
						break;
					case 'r':
						result += '\r';
						break;
					case 't':
						result += '\t';
						break;
					case '\'':
						result += '\'';
						break;
					case '\\':
						result += '\\';
						break;
					case '/':
						result += '/';
						break;
					// TODO /u for unicode escaping
					default:
						result += src[n];
					}
				} else {
					result += src[n];
				}
			}
			result.shrink_to_fit( );
			return result;
		}

		template<typename Derived>
		template<typename Setter, typename Getter>
		void daw_json_link<Derived>::link_json_string_fn(
		  daw::string_view member_name, Setter setter, Getter getter ) {
			add_json_link_function(
			  member_name,
			  [setter]( Derived &obj, daw::string_view view ) -> daw::string_view {
				  auto result = daw::json::parser::parse_json_string( view );
				  setter( obj, result.result );
				  view = result.view;
				  return view;
			  },
			  [getter]( Derived const &obj ) -> std::string {
				  using namespace std::string_literals;
				  return "\""s + getter( obj ) + "\""s;
			  } );
		}

		template<typename Derived>
		template<typename Setter, typename Getter>
		void daw_json_link<Derived>::link_json_string_optional_fn(
		  daw::string_view member_name, Setter setter, Getter getter ) {
			add_json_link_function(
			  member_name,
			  [setter]( Derived &obj, daw::string_view view ) -> daw::string_view {
				  auto result = daw::json::parser::parse_json_string_optional( view );
				  setter( obj, result.result );
				  return result.view;
			  },
			  [getter]( Derived const &obj ) -> std::string {
				  using namespace std::string_literals;
				  auto value = getter( obj );
				  if( value ) {
					  return "\""s + *value + "\""s;
				  }
				  return "null";
			  },
			  true );
		}

		template<typename Derived>
		template<typename Setter, typename Getter>
		void daw_json_link<Derived>::link_json_object_fn(
		  daw::string_view member_name, Setter setter, Getter getter ) {
			using member_t =
			  std::decay_t<decltype( getter( std::declval<Derived>( ) ) )>;
			add_json_link_function(
			  member_name,
			  [setter]( Derived &obj, daw::string_view view ) -> daw::string_view {
				  auto result = member_t::from_json_string( view );
				  setter( obj, std::move( result.result ) );
				  view = result.view;
				  return view;
			  },
			  [getter]( Derived const &obj ) -> std::string {
				  return getter( obj ).to_json_string( );
			  } );
		}

		template<typename Derived>
		template<typename Setter, typename Getter>
		void daw_json_link<Derived>::link_json_object_optional_fn(
		  daw::string_view member_name, Setter setter, Getter getter ) {
			using member_t =
			  std::decay_t<decltype( *getter( std::declval<Derived>( ) ) )>;
			add_json_link_function(
			  member_name,
			  [setter]( Derived &obj, daw::string_view view ) -> daw::string_view {
				  auto result =
				    daw::json::parser::parse_json_object_optional<member_t>( view );
				  setter( obj, std::move( result.result ) );
				  return result.view;
			  },
			  [getter]( Derived const &obj ) -> std::string {
				  using namespace std::string_literals;
				  auto const &value = getter( obj );
				  if( value ) {
					  return value->to_json_string( );
				  }
				  return "null";
			  },
			  true );
		}

		namespace impl {
			template<typename CharT, typename Traits>
			auto to_string( std::basic_string<CharT, Traits> s ) {
				return "\"" + s + "\"";
			}

			template<typename Boolean,
			         typename = std::enable_if_t<std::is_same<bool, Boolean>::value>>
			std::string to_string( Boolean b ) noexcept {
				using namespace std::string_literals;
				return b ? "true"s : "false"s;
			}

			template<typename Container>
			std::string container_to_string( Container const &c ) {
				using daw::json::impl::to_string;
				using std::begin;
				using std::end;
				using std::to_string;
				std::string result = "[";
				auto it = begin( c );
				if( it != end( c ) ) {
					result += to_string( *it );
					std::advance( it, 1 );
					for( ; it != end( c ); ++it ) {
						result += "," + to_string( *it );
					}
				}
				result += "]";
				return result;
			}

			template<typename Container>
			std::string object_container_to_string( Container const &c ) {
				using std::begin;
				using std::end;
				std::string result = "[";
				auto it = begin( c );
				if( it != end( c ) ) {
					result += it->to_json_string( );
					std::advance( it, 1 );
					for( ; it != end( c ); ++it ) {
						result += "," + it->to_json_string( );
					}
				}
				result += "]";
				return result;
			}

		} // namespace impl
		template<typename Derived>
		template<typename Setter, typename Getter>
		void daw_json_link<Derived>::link_json_integer_array_fn(
		  daw::string_view member_name, Setter item_setter, Getter getter ) {
			add_json_link_function(
			  member_name,
			  [item_setter]( Derived &obj,
			                 daw::string_view view ) -> daw::string_view {
				  auto const setter = [&obj, &item_setter]( int64_t value ) {
					  item_setter( obj, value );
				  };
				  auto result =
				    daw::json::parser::parse_json_integer_array( view, setter );
				  return result;
			  },
			  [getter]( Derived const &obj ) -> std::string {
				  auto const &container = getter( obj );
				  return impl::container_to_string( container );
			  } );
		}

		template<typename Derived>
		template<typename Setter, typename Getter>
		void daw_json_link<Derived>::link_json_real_array_fn(
		  daw::string_view member_name, Setter item_setter, Getter getter ) {
			add_json_link_function(
			  member_name,
			  [item_setter]( Derived &obj,
			                 daw::string_view view ) -> daw::string_view {
				  auto const setter = [&obj, &item_setter]( double value ) {
					  item_setter( obj, value );
				  };
				  auto result =
				    daw::json::parser::parse_json_real_array( view, setter );
				  return result;
			  },
			  [getter]( Derived const &obj ) -> std::string {
				  auto const &container = getter( obj );
				  return impl::container_to_string( container );
			  } );
		}

		template<typename Derived>
		template<typename Setter, typename Getter>
		void daw_json_link<Derived>::link_json_boolean_array_fn(
		  daw::string_view member_name, Setter item_setter, Getter getter ) {
			add_json_link_function(
			  member_name,
			  [item_setter]( Derived &obj,
			                 daw::string_view view ) -> daw::string_view {
				  auto const setter = [&obj, &item_setter]( bool value ) {
					  item_setter( obj, value );
				  };
				  auto result =
				    daw::json::parser::parse_json_boolean_array( view, setter );
				  return result;
			  },
			  [getter]( Derived const &obj ) -> std::string {
				  auto const &container = getter( obj );
				  return impl::container_to_string( container );
			  } );
		}

		template<typename Derived>
		template<typename Setter, typename Getter>
		void daw_json_link<Derived>::link_json_string_array_fn(
		  daw::string_view member_name, Setter item_setter, Getter getter ) {
			add_json_link_function(
			  member_name,
			  [item_setter]( Derived &obj,
			                 daw::string_view view ) -> daw::string_view {
				  auto const setter = [&obj, &item_setter]( daw::string_view value ) {
					  item_setter( obj, value );
				  };
				  auto result =
				    daw::json::parser::parse_json_string_array( view, setter );
				  return result;
			  },
			  [getter]( Derived const &obj ) -> std::string {
				  auto const &container = getter( obj );
				  return impl::container_to_string( container );
			  } );
		}

		template<typename Derived>
		template<typename Setter, typename Getter>
		void daw_json_link<Derived>::link_json_object_array_fn(
		  daw::string_view member_name, Setter item_setter, Getter getter ) {
			using member_t = std::decay_t<decltype(
			  *std::begin( getter( std::declval<Derived>( ) ) ) )>;
			add_json_link_function(
			  member_name,
			  [item_setter]( Derived &obj,
			                 daw::string_view view ) -> daw::string_view {
				  auto const setter = [&obj, &item_setter]( auto value ) {
					  item_setter( obj, std::move( value ) );
				  };
				  auto result = daw::json::parser::parse_json_object_array<member_t>(
				    view, setter );
				  return result;
			  },
			  [getter]( Derived const &obj ) -> std::string {
				  auto const &container = getter( obj );
				  return impl::object_container_to_string( container );
			  } );
		}

#define link_json_integer( json_name, member_name )                            \
	link_json_integer_fn(                                                        \
	  json_name,                                                                 \
	  []( auto &obj, int64_t value ) -> void {                                   \
		  obj.member_name =                                                        \
		    static_cast<std::decay_t<decltype( obj.member_name )>>( value );       \
	  },                                                                         \
	  []( auto const &obj ) -> std::decay_t<decltype( member_name )> const & {   \
		  return obj.member_name;                                                  \
	  } );

#define link_json_integer_optional( json_name, member_name, default_value )    \
	link_json_integer_optional_fn(                                               \
	  json_name,                                                                 \
	  []( auto &obj, boost::optional<int64_t> value ) -> void {                  \
		  if( value ) {                                                            \
			  obj.member_name = *value;                                              \
		  } else {                                                                 \
			  obj.member_name = default_value;                                       \
		  }                                                                        \
	  },                                                                         \
	  []( auto const &obj ) -> std::decay_t<decltype( member_name )> const & {   \
		  return obj.member_name;                                                  \
	  } );

#define link_json_real( json_name, member_name )                               \
	link_json_real_fn(                                                           \
	  json_name,                                                                 \
	  []( auto &obj, double value ) -> void {                                    \
		  obj.member_name =                                                        \
		    static_cast<std::decay_t<decltype( obj.member_name )>>( value );       \
	  },                                                                         \
	  []( auto const &obj ) -> std::decay_t<decltype( member_name )> const & {   \
		  return obj.member_name;                                                  \
	  } );

#define link_json_real_optional( json_name, member_name, default_value )       \
	link_json_real_optional_fn(                                                  \
	  json_name,                                                                 \
	  []( auto &obj, boost::optional<double> value ) -> void {                   \
		  if( value ) {                                                            \
			  obj.member_name = *value;                                              \
		  } else {                                                                 \
			  obj.member_name = default_value;                                       \
		  }                                                                        \
	  },                                                                         \
	  []( auto const &obj ) -> std::decay_t<decltype( member_name )> const & {   \
		  return obj.member_name;                                                  \
	  } );

#define link_json_boolean( json_name, member_name )                            \
	link_json_boolean_fn(                                                        \
	  json_name,                                                                 \
	  []( auto &obj, bool value ) -> void { obj.member_name = value; },          \
	  []( auto const &obj ) -> std::decay_t<decltype( member_name )> const & {   \
		  return obj.member_name;                                                  \
	  } );

#define link_json_boolean_optional( json_name, member_name, default_value )    \
	link_json_boolean_optional_fn(                                               \
	  json_name,                                                                 \
	  []( auto &obj, boost::optional<bool> value ) -> void {                     \
		  if( value ) {                                                            \
			  obj.member_name = *value;                                              \
		  } else {                                                                 \
			  obj.member_name = default_value;                                       \
		  }                                                                        \
	  },                                                                         \
	  []( auto const &obj ) -> std::decay_t<decltype( member_name )> const & {   \
		  return obj.member_name;                                                  \
	  } );

#define link_json_string( json_name, member_name )                             \
	link_json_string_fn(                                                         \
	  json_name,                                                                 \
	  []( auto &obj, daw::string_view value ) -> void {                          \
		  obj.member_name = daw::json::unescape_string( value );                   \
	  },                                                                         \
	  []( auto const &obj ) -> std::decay_t<decltype( member_name )> const & {   \
		  return obj.member_name;                                                  \
	  } );

#define link_json_streamable( json_name, member_name )                         \
	link_json_string_fn(                                                         \
	  json_name,                                                                 \
	  []( auto &obj, daw::string_view value ) -> void {                          \
		  std::stringstream ss{daw::json::unescape_string( value )};               \
		  ss >> obj.member_name;                                                   \
	  },                                                                         \
	  []( auto const &obj ) -> std::string {                                     \
		  return boost::lexical_cast<std::string>( obj.member_name );              \
	  } );

#define link_json_streamable_optional( json_name, member_name, default_value ) \
	link_json_string_optional_fn(                                                \
	  json_name,                                                                 \
	  []( auto &obj, boost::optional<daw::string_view> value ) -> void {         \
		  if( value ) {                                                            \
			  std::stringstream ss{daw::json::unescape_string( *value )};            \
			  ss >> *obj.member_name;                                                \
		  } else {                                                                 \
			  obj.member_name = default_value;                                       \
		  }                                                                        \
	  },                                                                         \
	  []( auto const &obj ) -> boost::optional<std::string> {                    \
		  if( obj.member_name ) {                                                  \
			  return boost::lexical_cast<std::string>( *obj.member_name );           \
		  } else {                                                                 \
			  return boost::optional<std::string>{};                                 \
		  }                                                                        \
	  } );

#define link_json_string_optional( json_name, member_name, default_value )     \
	link_json_string_optional_fn(                                                \
	  json_name,                                                                 \
	  []( auto &obj, boost::optional<daw::string_view> value ) -> void {         \
		  if( value ) {                                                            \
			  obj.member_name = daw::json::unescape_string( *value );                \
		  } else {                                                                 \
			  obj.member_name = default_value;                                       \
		  }                                                                        \
	  },                                                                         \
	  []( auto const &obj ) -> std::decay_t<decltype( member_name )> const & {   \
		  return obj.member_name;                                                  \
	  } );

#define link_json_object( json_name, member_name )                             \
	link_json_object_fn(                                                         \
	  json_name,                                                                 \
	  []( auto &obj, std::decay_t<decltype( member_name )> value ) -> void {     \
		  obj.member_name = std::move( value );                                    \
	  },                                                                         \
	  []( auto const &obj ) -> std::decay_t<decltype( member_name )> const & {   \
		  return obj.member_name;                                                  \
	  } );

#define link_json_object_optional( json_name, member_name, default_value )     \
	link_json_object_optional_fn(                                                \
	  json_name,                                                                 \
	  []( auto &obj,                                                             \
	      boost::optional<std::decay_t<decltype( *obj.member_name )>> value )    \
	    -> void {                                                                \
		  if( value ) {                                                            \
			  obj.member_name = std::move( *value );                                 \
		  } else {                                                                 \
			  obj.member_name = default_value;                                       \
		  }                                                                        \
	  },                                                                         \
	  []( auto const &obj ) -> std::decay_t<decltype( member_name )> const & {   \
		  return obj.member_name;                                                  \
	  } );

#define link_json_integer_array( json_name, member_name )                      \
	link_json_integer_array_fn(                                                  \
	  json_name,                                                                 \
	  []( auto &obj, int64_t value ) -> void {                                   \
		  obj.member_name.insert( std::end( obj.member_name ),                     \
		                          std::move( value ) );                            \
	  },                                                                         \
	  []( auto const &obj ) -> std::decay_t<decltype( member_name )> const & {   \
		  return obj.member_name;                                                  \
	  } );

#define link_json_real_array( json_name, member_name )                         \
	link_json_real_array_fn(                                                     \
	  json_name,                                                                 \
	  []( auto &obj, double value ) -> void {                                    \
		  obj.member_name.insert( std::end( obj.member_name ),                     \
		                          std::move( value ) );                            \
	  },                                                                         \
	  []( auto const &obj ) -> std::decay_t<decltype( member_name )> const & {   \
		  return obj.member_name;                                                  \
	  } );

#define link_json_boolean_array( json_name, member_name )                      \
	link_json_boolean_array_fn(                                                  \
	  json_name,                                                                 \
	  []( auto &obj, bool value ) -> void {                                      \
		  obj.member_name.insert( std::end( obj.member_name ),                     \
		                          std::move( value ) );                            \
	  },                                                                         \
	  []( auto const &obj ) -> std::decay_t<decltype( member_name )> const & {   \
		  return obj.member_name;                                                  \
	  } );

#define link_json_string_array( json_name, member_name )                       \
	link_json_string_array_fn(                                                   \
	  json_name,                                                                 \
	  []( auto &obj, daw::string_view value ) -> void {                          \
		  obj.member_name.insert( std::end( obj.member_name ),                     \
		                          daw::json::unescape_string( value ) );           \
	  },                                                                         \
	  []( auto const &obj ) -> std::decay_t<decltype( member_name )> const & {   \
		  return obj.member_name;                                                  \
	  } );

#define link_json_object_array( json_name, member_name )                       \
	link_json_object_array_fn(                                                   \
	  json_name,                                                                 \
	  []( auto &obj, auto value ) -> void {                                      \
		  obj.member_name.insert( std::end( obj.member_name ),                     \
		                          std::move( value ) );                            \
	  },                                                                         \
	  []( auto const &obj ) -> std::decay_t<decltype( member_name )> const & {   \
		  return obj.member_name;                                                  \
	  } );

	} // namespace json
} // namespace daw
