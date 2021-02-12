// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/daw_json_link
//
#include "defines.h"

#include "daw/json/daw_json_event_parser.h"
#include "daw/json/daw_json_link.h"

#include <daw/daw_benchmark.h>
#include <daw/daw_memory_mapped_file.h>
#include <daw/daw_parse_args.h>

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
		if( member_count_stack.empty( ) ) {
			// We are root object/array
			member_count_stack.emplace_back( p.value.is_class( ) );
		}
		auto &parent = member_count_stack.back( );
		auto const member_preamble = [&] {
			if( parent.is_first ) {
				parent.is_first = false;
			} else {
				write_chr( ',' );
			}
			if( parent.is_class and p.name ) {
				write_chr( '"' );
				write_str( *p.name );
				write_str( "\":" );
			}
		};
		switch( p.value.type( ) ) {
		case daw::json::JsonBaseParseTypes::Null:
			return true;
		case daw::json::JsonBaseParseTypes::String: {
			member_preamble( );
			auto const unescaped =
			  daw::json::from_json<std::string, ParsePolicy, true>(
			    p.value.get_string_view( ) );

			daw::json::to_json( unescaped, out_it );
			return true;
		}
		case daw::json::JsonBaseParseTypes::Class:
		case daw::json::JsonBaseParseTypes::Array:
			member_preamble( );
			return true;
		case daw::json::JsonBaseParseTypes::Number:
		case daw::json::JsonBaseParseTypes::Bool:
			member_preamble( );
			write_str( p.value.get_string_view( ) );
			return true;
		case daw::json::JsonBaseParseTypes::None:
		default: {
			auto rng = p.value.get_range( );
			auto sv = daw::string_view( rng.first, rng.size( ) ).pop_front( 10 );
			std::cerr << "Unknown JSON value type near\n" << sv << "\n\n";
			std::terminate( );
		}
		}
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
		member_count_stack.emplace_back( true );
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

template<typename Iterator>
void minify( daw::Arguments const &args, std::string_view data,
             Iterator out_it ) {

	bool const has_out_file = args.size( ) > 1 and args[1].name.empty( );
	auto handler = JSONMinifyHandler( out_it );
	if( auto pos = args.find_argument_position( "verbose" ); pos ) {
		auto const time =
		  daw::benchmark( [&] { daw::json::json_event_parser( data, handler ); } );
		if( not has_out_file ) {
			std::cout << '\n';
		}
		std::cout << "minified "
		          << daw::utility::to_bytes_per_second( data.size( ), 2 ) << " in "
		          << daw::utility::format_seconds( time, 2 ) << " at "
		          << daw::utility::to_bytes_per_second(
		               static_cast<double>( data.size( ) ) / time, 2 )
		          << "/s\n";
	} else {
		daw::json::json_event_parser( data, handler );
		if( not has_out_file ) {
			std::cout << '\n';
		}
	}
}

int main( int argc, char **argv ) {

	std::ios::sync_with_stdio( false );

	auto args = daw::Arguments( argc, argv );

	if( args.empty( ) ) {
		std::cerr << "Must supply path to json document followed optionally by the "
		             "output file\n";
		std::cerr << args.program_name( )
		          << " json_in.json [json_out.json] [--verbose]\n";
		exit( EXIT_FAILURE );
	}
	auto data = daw::filesystem::memory_mapped_file_t<>( args[0].value );

#ifdef DAW_USE_JSON_EXCEPTIONS
	try {
#endif
		if( args.size( ) > 1 and args[1].name.empty( ) ) {
			test_assert( data.size( ) > 0, "Invalid JSON document" );
			auto ofile = std::ofstream( args[1].value.to_string( ).c_str( ),
			                            std::ios::trunc | std::ios::binary );
			test_assert( ofile, "Unable to output file" );
			minify( args, data, std::ostreambuf_iterator<char>( ofile ) );
		} else {
			minify( args, data, std::ostreambuf_iterator<char>( std::cout ) );
		}
#ifdef DAW_USE_JSON_EXCEPTIONS
	} catch( daw::json::json_exception const &jex ) {
		std::cerr << "Exception thrown by parser\n"
		          << to_formatted_string( jex, data.data( ) ) << '\n';
		exit( 1 );
	}
#endif
}
