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
#include <fstream>
#include <iomanip>
#include <iostream>

template<typename OutputIterator>
class JSONMinifyHandler {
	struct stack_value {
		bool is_class;
		bool is_first = true;

		constexpr stack_value( bool isClass )
		  : is_class( isClass ) {}
	};
	std::vector<stack_value> member_count_stack{ };
	OutputIterator out_it;

	void write_chr( char c ) {
		*out_it++ = c;
	}

	void write_str( std::string_view s ) {
		out_it = std::copy_n( s.data( ), s.size( ), out_it );
	}

	template<std::size_t N>
	void write_str( char const ( &str )[N] ) {
		out_it = std::copy_n( str, N - 1, out_it );
	}

public:
	JSONMinifyHandler( OutputIterator it )
	  : out_it( std::move( it ) ) {}

	template<typename ParsePolicy>
	bool handle_on_value( daw::json::basic_json_pair<ParsePolicy> p ) {
		daw::json::JsonBaseParseTypes const v_type = p.value.type( );
		if( v_type == daw::json::JsonBaseParseTypes::Null ) {
			// We are minifying, do no output Null values
			return true;
		}
		if( member_count_stack.empty( ) ) {
			member_count_stack.emplace_back( p.value.is_class( ) );
		}
		if( member_count_stack.back( ).is_first ) {
			member_count_stack.back( ).is_first = false;
		} else {
			write_chr( ',' );
		}
		if( p.name and member_count_stack.back( ).is_class ) {
			write_chr( '"' );
			write_str( *p.name );
			write_str( "\":" );
		}
		if( ( v_type == daw::json::JsonBaseParseTypes::Class ) |
		    ( v_type == daw::json::JsonBaseParseTypes::Array ) ) {
			return true;
		}
		if( v_type == daw::json::JsonBaseParseTypes::String ) {
			write_chr( '"' );
			// Parsing to string will unescape it, including code points <= 0x20
			// We need to re-escape them and copy_to_iterator will do that
			using namespace daw::json;
			auto rng = p.value.get_range( );
			out_it =
			  utils::copy_to_iterator<true, daw::json::EightBitModes::AllowFull>(
			    out_it,
			    json_details::parse_string_known_stdstring<true, std::string, false>(
			      rng ) );
			write_chr( '"' );
		} else {
			write_str( p.value.get_string_view( ) );
		}
		return true;
	}

	template<typename ParsePolicy>
	bool handle_on_array_start( daw::json::basic_json_value<ParsePolicy> ) {
		member_count_stack.emplace_back( false );
		write_chr( '[' );
		return true;
	}

	bool handle_on_array_end( ) {
		member_count_stack.pop_back( );
		write_chr( ']' );
		return true;
	}

	template<typename ParsePolicy>
	bool handle_on_class_start( daw::json::basic_json_value<ParsePolicy> ) {
		member_count_stack.emplace_back( false );
		write_chr( '{' );
		return true;
	}

	bool handle_on_class_end( ) {
		member_count_stack.pop_back( );
		write_chr( '}' );
		return true;
	}

	OutputIterator get_iterator( ) {
		return out_it;
	}
};
template<typename OutputIterator>
JSONMinifyHandler( OutputIterator ) -> JSONMinifyHandler<OutputIterator>;

int main( int argc, char **argv ) try {
	if( argc < 2 ) {
		std::cerr << "Must supply path to json document followed optionally by the "
		             "output file\n";
		std::cerr << argv[0] << " json_in.json [json_out.json]\n";
		exit( EXIT_FAILURE );
	}
	std::optional<std::ofstream> out_file =
	  argc >= 3 ? std::ofstream( argv[2], std::ios::trunc | std::ios::binary )
	            : std::optional<std::ofstream>( );
	auto data = daw::filesystem::memory_mapped_file_t<>( argv[1] );

	auto handler = JSONMinifyHandler(
	  std::ostreambuf_iterator<char>( out_file ? *out_file : std::cerr ) );

	daw::json::json_event_parser( data, handler );
} catch( daw::json::json_exception const &jex ) {
	std::cerr << "Exception thrown by parser: " << jex.reason( ) << std::endl;
	exit( 1 );
}
