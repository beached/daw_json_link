// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/daw_json_link
//

#include "defines.h"

#include <daw/daw_memory_mapped_file.h>

#include "daw/json/daw_json_link.h"

#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <stack>
#include <string>

template<typename ExecTag, typename OutIt>
constexpr void minify( std::string_view json_data, OutIt out_it ) {}

int main( int argc, char **argv ) try {
	if( argc <= 1 ) {
		std::cerr << "Must supply path to json document followed optionally by the "
		             "output file\n";
		std::cerr << argv[0] << " json_in.json [json_out.json]\n";
		exit( EXIT_FAILURE );
	}
	auto data = daw::filesystem::memory_mapped_file_t<>( argv[1] );

	using Range =
	  daw::json::SIMDNoCommentSkippingPolicyChecked<daw::json::simd_exec_tag>;

	std::optional<std::ofstream> out_file =
	  [&]( ) -> std::optional<std::ofstream> {
		if( argc >= 2 ) {
			auto result =
			  std::ofstream( argv[2], std::ios::binary | std::ios::trunc );
			if( not result ) {
				std::cerr << "Could not open " << argv[2] << " for writing\n";
				exit( 1 );
			}
		}
		return { };
	}( );

	std::ostream &outs = [&]( ) -> std::ostream & {
		if( out_file ) {
			return *out_file;
		}
		return std::cout;
	}( );

	using iterator = daw::json::basic_json_value_iterator<Range>;
	using json_value_t = daw::json::basic_json_pair<Range>;
	using stack_value_t =
	  std::variant<std::pair<iterator, iterator>, json_value_t> > ;
	using stack_t = std::stack<stack_value_t, std::vector<stack_value_t>>;

	auto parent_stack = stack_t( );

	auto const process_value = [&]( json_value_t const &jv ) {
		if( jv.name ) {
			outs << '"' << *jv.name << ": ";
		}
		switch( jv.value.type( ) ) {
		case daw::json::JsonBaseParseTypes::Array: {
			outs << '[';
		} break;
		case daw::json::JsonBaseParseTypes::Class: {
			auto p =
			  std::pair<iterator, iterator>( jv.value.begin( ), jv.value.end( ) );
			auto v = *p.first;
			++p.first;
			parent_stack.push( std::move( p ) );
			parent_stack.push( )
		} break;
		case daw::json::JsonBaseParseTypes::String:
			outs << '"' << jv.get_string_view( ) << '"';
		default:
			outs << jv.get_string_view( );
		}
	};
	parent_stack.push( json_value_t{
	  std::nullopt, daw::json::basic_json_value<Range>(
	                  std::string_view( data.data( ), data.size( ) ) ) } );

	while( not parent_stack.empty( ) ) {}
} catch( daw::json::json_exception const &jex ) {
	std::cerr << "Exception thrown by parser: " << jex.reason( ) << std::endl;
	exit( 1 );
}
