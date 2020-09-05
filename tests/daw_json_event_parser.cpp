// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/daw_json_link
//

#include "daw/json/daw_json_event_parser.h"
#include "daw/json/daw_json_link.h"

#include <daw/daw_memory_mapped_file.h>

#include <cstdlib>
#include <iostream>

struct StringLenHandler {
	std::vector<std::size_t> member_count = { 0 };

	StringLenHandler( ) = default;

	template<typename ParsePolicy>
	daw::json::json_parse_handler_result
	handle_on_next_value( daw::json::basic_json_pair<ParsePolicy> p ) {
		daw::json::JsonBaseParseTypes const v_type = p.value.type( );
		if( v_type == daw::json::JsonBaseParseTypes::Null ) {
			return daw::json::json_parse_handler_result::Continue;
		}
		if( member_count.back( )++ > 0 ) {
			std::cout << ',';
		}
		if( p.name ) {
			std::cout << '"' << *p.name << "\":";
		}
		if( ( v_type == daw::json::JsonBaseParseTypes::Class ) |
		    ( v_type == daw::json::JsonBaseParseTypes::Array ) ) {
			return daw::json::json_parse_handler_result::Continue;
		}
		std::cout << p.value.get_string_view( );
		return daw::json::json_parse_handler_result::Continue;
	}

	template<typename ParsePolicy>
	bool handle_on_array_start( daw::json::basic_json_value<ParsePolicy> ) {
		member_count.emplace_back( 0 );
		std::cout << '[';
		return true;
	}

	bool handle_on_array_end( ) {
		member_count.pop_back( );
		std::cout << ']';
		return true;
	}

	template<typename ParsePolicy>
	bool handle_on_class_start( daw::json::basic_json_value<ParsePolicy> ) {
		member_count.emplace_back( 0 );
		std::cout << '{';
		return true;
	}

	bool handle_on_class_end( ) {
		member_count.pop_back( );
		std::cout << '}';
		return true;
	}
};

int main( int argc, char **argv ) try {
	if( argc <= 1 ) {
		std::cerr << "Must supply path to json document followed optionally by the "
		             "output file\n";
		std::cerr << argv[0] << " json_in.json [json_out.json]\n";
		exit( EXIT_FAILURE );
	}
	auto data = daw::filesystem::memory_mapped_file_t<>( argv[1] );

	auto handler = StringLenHandler( );
	daw::json::json_event_parser( data, handler );
} catch( daw::json::json_exception const &jex ) {
	std::cerr << "Exception thrown by parser: " << jex.reason( ) << std::endl;
	exit( 1 );
}
