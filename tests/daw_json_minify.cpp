// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/daw_json_link
//

#include "defines.h"

#include "daw/json/daw_json_link.h"

#include <daw/daw_memory_mapped_file.h>
#include <daw/daw_visit.h>

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
	std::ios::sync_with_stdio( false );
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
		if( argc > 2 ) {
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
	  std::variant<std::pair<iterator, iterator>, json_value_t>;
	using min_stack_t = std::stack<stack_value_t, std::vector<stack_value_t>>;

	auto parent_stack = min_stack_t( );

	std::function<void( daw::json::basic_json_value<Range> jv )>
	  process_pair_value = [&]( daw::json::basic_json_value<Range> jv ) {
		  switch( jv.type( ) ) {
		  case daw::json::JsonBaseParseTypes::Array: {
			  outs << '[';
			  parent_stack.push(
			    std::pair<iterator, iterator>( jv.begin( ), jv.end( ) ) );
		  } break;
		  case daw::json::JsonBaseParseTypes::Class: {
			  outs << '{';
			  if( jv.begin( ) == jv.end( ) ) {
				  outs << '}';
			  } else {
				  auto f = jv.begin( );
				  auto l = jv.end( );
				  auto p = *f;
				  if( p.name ) {
					  outs << '"' << *( p.name ) << "\":";
				  }
				  ++f;
				  parent_stack.push( std::pair<iterator, iterator>( f, l ) );
				  process_pair_value( p.value );
			  }
		  } break;
		  case daw::json::JsonBaseParseTypes::String:
			  outs << jv.get_string_view( );
			  break;
		  case daw::json::JsonBaseParseTypes::Number:
		  case daw::json::JsonBaseParseTypes::Bool:
		  case daw::json::JsonBaseParseTypes::Null:
			  outs << jv.get_string_view( );
			  break;
		  case daw::json::JsonBaseParseTypes::None:
		  default:
			  std::cerr << "Unexpected type\n";
			  std::terminate( );
		  }
	  };

	auto const process_value = [&]( json_value_t jv ) {
		if( jv.name ) {
			outs << '"' << *jv.name << "\":";
		}
		process_pair_value( jv.value );
	};

	auto const process_range = [&]( std::pair<iterator, iterator> p ) {
		if( p.first != p.second ) {
			auto jv = *p.first;
			p.first++;
			if( p.first != p.second ) {
				parent_stack.push( std::move( p ) );
			} else {
				if( jv.name ) {
					outs << '}';
				} else {
					outs << ']';
				}
			}
			outs << ',';
			process_value( std::move( jv ) );
		}
	};
	parent_stack.push( json_value_t{
	  std::nullopt, daw::json::basic_json_value<Range>(
	                  std::string_view( data.data( ), data.size( ) ) ) } );

	while( not parent_stack.empty( ) ) {
		auto v = parent_stack.top( );
		parent_stack.pop( );
		daw::visit_nt( std::move( v ), process_value, process_range );
	}
	if( not out_file ) {
		outs << '\n';
	}
} catch( daw::json::json_exception const &jex ) {
	std::cerr << "Exception thrown by parser: " << jex.reason( ) << std::endl;
	exit( 1 );
}
