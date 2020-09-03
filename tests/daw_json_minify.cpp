// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/daw_json_link
//

#include "daw/json/daw_json_link.h"

#include <daw/daw_memory_mapped_file.h>

#include <cstdlib>
#include <fstream>
#include <iostream>
#include <stack>

template<bool VerifyValues, typename ParsePolicy>
constexpr void minify( std::string_view json_data, std::ostream &outs ) {
	using iterator = daw::json::basic_json_value_iterator<ParsePolicy>;
	using json_value_t = daw::json::basic_json_pair<ParsePolicy>;
	enum class RangeType { Class, Array };
	struct stack_value_t {
		RangeType type;
		std::pair<iterator, iterator> value;
		bool is_first = true;
	};
	using min_stack_t = std::stack<stack_value_t>;

	auto parent_stack = min_stack_t( );

	auto const process_value = [&]( json_value_t p ) {
		auto &jv = p.value;
		if( p.name and not p.name->empty( ) ) {
			outs << '"' << *p.name << "\":";
		}
		switch( jv.type( ) ) {
		case daw::json::JsonBaseParseTypes::Array:
			outs << '[';
			parent_stack.push( { RangeType::Array, std::pair<iterator, iterator>(
			                                         jv.begin( ), jv.end( ) ) } );
			break;
		case daw::json::JsonBaseParseTypes::Class: {
			outs << '{';
			parent_stack.push( { RangeType::Class, std::pair<iterator, iterator>(
			                                         jv.begin( ), jv.end( ) ) } );
			break;
		case daw::json::JsonBaseParseTypes::Number:
			if constexpr( VerifyValues ) {
				auto v = jv;
				(void)daw::json::from_json<daw::json::json_number<daw::json::no_name>,
				                           ParsePolicy>( v );
			}
			outs << jv.get_string_view( );
			break;
		case daw::json::JsonBaseParseTypes::Bool:
			if constexpr( VerifyValues ) {
				auto v = jv;
				(void)daw::json::from_json<daw::json::json_bool<daw::json::no_name>,
				                           ParsePolicy>( v );
			}
			outs << jv.get_string_view( );
			break;
		case daw::json::JsonBaseParseTypes::String:
			outs << jv.get_string_view( );
			break;
		case daw::json::JsonBaseParseTypes::Null:
			if constexpr( VerifyValues ) {
				auto v = jv;
				(void)daw::json::from_json<
				  daw::json::json_number_null<daw::json::no_name>, ParsePolicy>( v );
			}
			break;
		case daw::json::JsonBaseParseTypes::None:
		default:
			std::cerr << "Unexpected type\n";
			std::terminate( );
		}
		}
	};

	auto const process_range = [&]( stack_value_t v ) {
		if( v.value.first != v.value.second ) {
			auto jv = *v.value.first;
			if( not jv.value.is_null( ) ) {
				if( v.is_first ) {
					v.is_first = false;
				} else {
					outs << ',';
				}
			}
			v.value.first++;
			parent_stack.push( std::move( v ) );
			if( not jv.value.is_null( ) ) {
				process_value( std::move( jv ) );
			}
		} else {
			switch( v.type ) {
			case RangeType::Class:
				outs << '}';
				break;
			case RangeType::Array:
				outs << ']';
				break;
			}
		}
	};

	process_value(
	  { std::nullopt, daw::json::basic_json_value<ParsePolicy>( json_data ) } );

	while( not parent_stack.empty( ) ) {
		auto v = std::move( parent_stack.top( ) );
		parent_stack.pop( );
		process_range( v );
	}
}

int main( int argc, char **argv ) try {
	std::ios::sync_with_stdio( false );
	if( argc <= 1 ) {
		std::cerr << "Must supply path to json document followed optionally by the "
		             "output file\n";
		std::cerr << argv[0] << " json_in.json [json_out.json]\n";
		exit( EXIT_FAILURE );
	}
	auto data = daw::filesystem::memory_mapped_file_t<>( argv[1] );

	auto out_file = std::ofstream( );
	if( argc > 2 ) {
		out_file.open( argv[2], std::ios::binary | std::ios::trunc );
		if( not out_file ) {
			std::cerr << "Could not open " << argv[2] << " for writing\n";
			exit( 1 );
		}
	}

	std::ostream &outs = [&out_file]( ) -> std::ostream & {
		if( out_file ) {
			return out_file;
		}
		return std::cout;
	}( );

	using ParsePolicy =
	  daw::json::SIMDNoCommentSkippingPolicyChecked<daw::json::simd_exec_tag>;

	if( argc > 3 and ( std::string_view( argv[3] ) == "fast" ) ) {
		minify<false, ParsePolicy>( std::string_view( data.data( ), data.size( ) ),
		                            outs );
	} else {
		minify<true, ParsePolicy>( std::string_view( data.data( ), data.size( ) ),
		                           outs );
	}
	if( not out_file ) {
		outs << '\n';
	}
} catch( daw::json::json_exception const &jex ) {
	std::cerr << "Exception thrown by parser: " << jex.reason( ) << std::endl;
	exit( 1 );
}
