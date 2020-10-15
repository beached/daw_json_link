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
#include <string>
#include <vector>

namespace daw::json {

	inline std::string find_json_path_to( json_exception const &jex,
	                                      char const *doc_start ) {
		if( not jex.parse_location( ) or not doc_start ) {
			return { };
		}

		struct stack_value {
			std::string name;
			bool is_class;
		};

		struct handler_t {
			char const *first;
			char const *last;
			std::vector<stack_value> parse_stack{ };
			std::string member_name{ };
			long long index = -1;

			bool handle_on_value( json_pair jp ) {
				if( last >= jp.value.get_string_view( ).data( ) ) {
					return false;
				}
				if( parse_stack.empty( ) ) {
					return true;
				}
				using namespace std::string_literals;
				if( parse_stack.back( ).is_class ) {
					member_name = "."s + ( jp.name ? static_cast<std::string>( *jp.name )
					                               : std::string{ } );
					return true;
				}
				++index;
				member_name = "["s + std::to_string( index ) + "]"s;
				return true;
			}

			bool handle_on_array_start( json_value jv ) {
				parse_stack.push_back( { member_name, false } );
				return true;
			}

			bool handle_on_array_end( json_value jv ) {
				parse_stack.pop_back( );
				return true;
			}

			bool handle_on_class_start( json_value jv ) {
				parse_stack.push_back( { member_name, false } );
				return true;
			}

			bool handle_on_class_end( json_value jv ) {
				parse_stack.pop_back( );
				return true;
			}
		} handler{ doc_start, jex.parse_location( ) + 1 };
		json_event_parser( doc_start, handler );

		return std::accumulate(
		  handler.parse_stack.begin( ), handler.parse_stack.end( ), std::string{ },
		  []( std::string state, stack_value const &sv ) { return sv.name; } );
	}
} // namespace daw::json
