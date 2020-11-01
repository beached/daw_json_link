// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/daw_json_link
//

#pragma once

#include "daw_json_event_parser.h"
#include "impl/daw_json_assert.h"

#include <algorithm>
#include <iostream>
#include <string>
#include <vector>

namespace daw::json {

	inline std::string find_json_path_to( char const *parse_location,
	                                      char const *doc_start ) {
		if( parse_location == nullptr or doc_start == nullptr ) {
			return { };
		}
		if( std::less<>{ }( parse_location, doc_start ) ) {
			return { };
		}

		struct stack_value {
			JsonBaseParseTypes type;
			std::string_view name;
			long long index;
		};

		struct handler_t {
			char const *first;
			char const *last;
			std::vector<stack_value> parse_stack{ };
			std::string_view member_name{ };
			std::optional<stack_value> last_popped{ };
			long long index = -1;
			JsonBaseParseTypes type = JsonBaseParseTypes::None;

			JsonBaseParseTypes child_of( ) {
				if( parse_stack.empty( ) ) {
					return JsonBaseParseTypes::None;
				}
				return parse_stack.back( ).type;
			}

			bool handle_on_value( json_pair jp ) {
				if( auto const range = jp.value.get_range( );
				    range.empty( ) or last <= range.data( ) ) {
					return false;
				}
				switch( child_of( ) ) {
				case JsonBaseParseTypes::Class:
					member_name = *jp.name;
					index = -1;
					break;
				case JsonBaseParseTypes::Array:
					member_name = { };
					++index;
					break;
				case JsonBaseParseTypes::None:
				case JsonBaseParseTypes::Bool:
				case JsonBaseParseTypes::Number:
				case JsonBaseParseTypes::Null:
				case JsonBaseParseTypes::String:
				default:
					member_name = { };
					index = -1;
					break;
				}
				type = jp.value.type( );
				last_popped = std::nullopt;
				return true;
			}

			bool handle_on_array_start( json_value const & ) {
				parse_stack.push_back( stack_value{ type, member_name, index } );
				member_name = { };
				index = -1;
				type = JsonBaseParseTypes::None;
				return true;
			}

			bool handle_on_array_end( ) {
				if( not parse_stack.empty( ) ) {
					last_popped = parse_stack.back( );
					member_name = last_popped->name;
					index = last_popped->index;
					type = last_popped->type;
					parse_stack.pop_back( );
				}
				return true;
			}

			bool handle_on_class_start( json_value const & ) {
				parse_stack.push_back( stack_value{ type, member_name, index } );
				member_name = { };
				index = -1;
				type = JsonBaseParseTypes::None;
				return true;
			}

			bool handle_on_class_end( ) {
				if( not parse_stack.empty( ) ) {
					last_popped = parse_stack.back( );
					member_name = last_popped->name;
					index = last_popped->index;
					type = last_popped->type;
					parse_stack.pop_back( );
				}
				return true;
			}

			bool handle_on_number( json_value jv ) {
				auto sv = std::string_view( );
				try {
					sv = jv.get_string_view( );
				} catch( json_exception const & ) {
					parse_stack.push_back( stack_value{ type, member_name, index } );
					return false;
				}
				if( sv.data( ) <= last and last <= ( sv.data( ) + sv.size( ) ) ) {
					parse_stack.push_back( stack_value{ type, member_name, index } );
					return false;
				}
				return true;
			}

			bool handle_on_bool( json_value jv ) {
				auto sv = std::string_view( );
				try {
					sv = jv.get_string_view( );
				} catch( json_exception const & ) {
					parse_stack.push_back( stack_value{ type, member_name, index } );
					return false;
				}
				if( sv.data( ) <= last and last <= ( sv.data( ) + sv.size( ) ) ) {
					parse_stack.push_back( stack_value{ type, member_name, index } );
					return false;
				}
				return true;
			}

			bool handle_on_string( json_value jv ) {
				auto sv = std::string_view( );
				try {
					sv = jv.get_string_view( );
				} catch( json_exception const & ) {
					parse_stack.push_back( stack_value{ type, member_name, index } );
					return false;
				}
				if( sv.data( ) <= last and last <= ( sv.data( ) + sv.size( ) ) ) {
					parse_stack.push_back( stack_value{ type, member_name, index } );
					return false;
				}
				return true;
			}

			bool handle_on_null( json_value jv ) {
				auto sv = std::string_view( );
				try {
					sv = jv.get_string_view( );
				} catch( json_exception const & ) {
					parse_stack.push_back( stack_value{ type, member_name, index } );
					return false;
				}
				if( sv.data( ) <= last and last <= ( sv.data( ) + sv.size( ) ) ) {
					parse_stack.push_back( stack_value{ type, member_name, index } );
					return false;
				}
				return true;
			}
		} handler{ doc_start, parse_location + 1 };

		try {
			json_event_parser( doc_start, handler );
		} catch( json_exception const & ) {
			// Ignoring because we are only looking for the stack leading up to this
			// and it may have come from an error
		}
		if( handler.last_popped ) {
			handler.parse_stack.push_back( *handler.last_popped );
		}
		return std::accumulate(
		  handler.parse_stack.begin( ), handler.parse_stack.end( ), std::string{ },
		  []( auto &&state, stack_value const &sv ) mutable {
			  if( sv.index >= 0 ) {
				  state += "[" + std::to_string( sv.index ) + "]";
			  } else if( not sv.name.empty( ) ) {
				  state += "." + static_cast<std::string>( sv.name );
			  }
			  return std::forward<decltype( state )>( state );
		  } );
	}

	inline std::string find_json_path_to( json_exception const &jex,
	                                      char const *doc_start ) {
		return find_json_path_to( jex.parse_location( ), doc_start );
	}
} // namespace daw::json
