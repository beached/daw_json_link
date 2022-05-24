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

		explicit constexpr stack_value( bool isClass )
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
	explicit JSONMinifyHandler( OutputIterator it )
	  : out_it( std::move( it ) ) {}

	template<typename ParsePolicy>
	bool handle_on_value( daw::json::basic_json_pair<ParsePolicy> p ) {
		daw::json::JsonBaseParseTypes const v_type = p.value.type( );
		if( member_count_stack.empty( ) ) {
			member_count_stack.emplace_back( p.value.is_class( ) );
		}
		auto &parent = member_count_stack.back( );
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
		if( ( v_type == daw::json::JsonBaseParseTypes::Class ) |
		    ( v_type == daw::json::JsonBaseParseTypes::Array ) ) {
			return true;
		}
		write_str( p.value.get_string_view( ) );
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
		auto const time = daw::benchmark( [&] {
			daw::json::json_event_parser( data, handler,
			                              daw::json::ConformancePolicy );
		} );
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
		daw::json::json_event_parser( data, handler, daw::json::ConformancePolicy );
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

#ifdef DAW_USE_EXCEPTIONS
	try
#endif
	{
		if( args.size( ) > 1 and args[1].name.empty( ) ) {
			test_assert( data.size( ) > 0, "Could not open JSON document" );
			auto ofile =
			  std::ofstream( static_cast<std::string>( args[1].value ).c_str( ),
			                 std::ios::trunc | std::ios::binary );
			if( not ofile ) {
				std::cerr << "Failed to open outputfile '" << args[1].value << "'\n";
				exit( 1 );
			}
			minify( args, data, std::ostreambuf_iterator<char>( ofile ) );
		} else {
			minify( args, data, std::ostreambuf_iterator<char>( std::cout ) );
		}
	}
#ifdef DAW_USE_EXCEPTIONS
	catch( daw::json::json_exception const &jex ) {
		std::cerr << "Exception thrown by parser: " << jex.reason( ) << '\n';
		exit( 1 );
	} catch( std::exception const &ex ) {
		std::cerr << "Unknown exception thrown during testing: " << ex.what( )
		          << '\n';
		exit( 1 );
	} catch( ... ) {
		std::cerr << "Unknown exception thrown during testing\n";
		throw;
	}
#endif
}
