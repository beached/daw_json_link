// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/daw_json_link
//

#include "daw/json/daw_json_event_parser.h"

#include <daw/daw_memory_mapped_file.h>

#include <cstdlib>
#include <fstream>
#include <iostream>
#include <stack>

struct StringLenHandler {
	std::size_t name_count = 0;
	std::size_t name_total = 0;
	std::size_t string_count = 0;
	std::size_t string_total = 0;

	constexpr StringLenHandler( ) = default;

	template<typename ParsePolicy>
	bool handle_on_next_value( daw::json::basic_json_pair<ParsePolicy> p ) {
		if( p.name ) {
			++name_count;
			name_total += p.name->size( );
		}
		return true;
	}

	template<typename Range>
	bool handle_on_string( daw::json::basic_json_value<Range> jv ) {
		if( jv.is_string( ) ) {
			++string_count;
			string_total += jv.get_string_view( ).size( );
		}
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

	using ParsePolicy =
	  daw::json::SIMDNoCommentSkippingPolicyChecked<daw::json::simd_exec_tag>;

	auto handler = StringLenHandler( );
	daw::json::json_event_parser( data, handler );
	std::cout << "total member names: " << handler.name_count << '\n';
	std::cout << "total string members: " << handler.string_count << '\n';
	if( handler.name_count != 0 ) {
		std::cout << "average name length:"
		          << ( handler.name_total / handler.name_count ) << '\n';
	}
	if( handler.string_count != 0 ) {
		std::cout << "average string length:"
		          << ( handler.string_total / handler.string_count ) << '\n';
	}
	if( handler.string_count + handler.name_count != 0 ) {
		if( handler.string_count != 0 ) {
			std::cout << "average of both:"
			          << ( ( handler.string_total + handler.name_total ) /
			               ( handler.string_count + handler.name_count ) )
			          << '\n';
		}
	}
} catch( daw::json::json_exception const &jex ) {
	std::cerr << "Exception thrown by parser: " << jex.reason( ) << std::endl;
	exit( 1 );
}
