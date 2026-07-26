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
#include "daw/json/daw_json_writer.h"

#include <daw/daw_benchmark.h>
#include <daw/daw_parse_args.h>
#include <daw/daw_read_file.h>

#include <cstdlib>
#include <fstream>
#include <iomanip>
#include <iostream>

template<typename Writable>
class JSONMinifyHandler {
	using writer_t =
	  decltype( daw::json::json_writer( std::declval<Writable &>( ) ) );
	writer_t m_out;

public:
	explicit constexpr JSONMinifyHandler( Writable &w )
	  : m_out( daw::json::json_writer( w ) ) {}

	constexpr bool handle_on_number( double d ) {
		m_out.write_number( d );
		return true;
	}

	template<daw::json::json_options_t PolicyFlags, typename Allocator>
	bool
	handle_on_string( daw::json::basic_json_value<PolicyFlags, Allocator> jv ) {
		m_out.write_string( daw::json::from_json<daw::string_view>( jv ) );
		return true;
	}

	bool handle_on_null( ) {
		m_out.write_null( );
		return true;
	}

	bool handle_on_bool( bool b ) {
		m_out.write_boolean( b );
		return true;
	}

	template<daw::json::json_options_t PolicyFlags, typename Allocator>
	bool handle_on_value( daw::json::basic_json_pair<PolicyFlags, Allocator> p ) {
		if( p.name ) {
			m_out.add_key( *p.name );
		}
		return true;
	}

	template<daw::json::json_options_t PolicyFlags, typename Allocator>
	bool
	handle_on_array_start( daw::json::basic_json_value<PolicyFlags, Allocator> ) {
		m_out.open_array( );
		return true;
	}

	bool handle_on_array_end( ) {
		m_out.close_array( );
		return true;
	}

	template<daw::json::json_options_t PolicyFlags, typename Allocator>
	bool
	handle_on_class_start( daw::json::basic_json_value<PolicyFlags, Allocator> ) {
		m_out.open_object( );
		return true;
	}

	bool handle_on_class_end( ) {
		m_out.close_object( );
		return true;
	}
};
template<typename Writable>
JSONMinifyHandler( Writable ) -> JSONMinifyHandler<Writable>;

template<typename Writable>
void roundtrip( daw::Arguments const &args, std::string_view data,
                Writable &out ) {

	bool const has_out_file = args.size( ) > 1 and args[1].name.empty( );
	auto handler = JSONMinifyHandler( out );

	if( auto pos = args.find_argument_position( "verbose" ); pos ) {
		auto const time = daw::benchmark( [&] {
			daw::json::json_event_parser(
			  data, handler, daw::json::ConformancePolicy );
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

int main( int argc, char **argv )
#if defined( DAW_USE_EXCEPTIONS )
  try
#endif
{
	std::ios::sync_with_stdio( false );

	auto args = daw::Arguments( argc, argv );

	if( args.empty( ) ) {
		std::cerr << "Must supply path to json document followed optionally by the "
		             "output file\n";
		std::cerr << args.program_name( )
		          << " json_in.json [json_out.json] [--verbose]\n";
		exit( EXIT_FAILURE );
	}
	auto const data = daw::read_file( std::string( args[0].value ) ).value( );

#if defined( DAW_USE_EXCEPTIONS )
	try {
#endif
		if( args.size( ) > 1 and args[1].name.empty( ) ) {
			test_assert( data.size( ) > 0, "Could not open JSON document" );
			auto ofile = std::ofstream( std::string( args[1].value ),
			                            std::ios::trunc | std::ios::binary );
			if( not ofile ) {
				std::cerr << "Failed to open outputfile '" << args[1].value << "'\n";
				exit( 1 );
			}
			roundtrip( args, data, ofile );
		} else {
			auto result = std::string{ };
			roundtrip( args, data, result );
			std::cout << result << '\n';
		}
#if defined( DAW_USE_EXCEPTIONS )
	} catch( daw::json::json_exception const &jex ) {
		std::cerr << "Exception thrown by parser\n"
		          << to_formatted_string( jex, data.data( ) ) << '\n';
		exit( 1 );
	}
#endif
}
#if defined( DAW_USE_EXCEPTIONS )
catch( std::exception const &ex ) {
	std::cerr << "Unknown exception thrown during testing: " << ex.what( )
	          << '\n';
	exit( 1 );
} catch( ... ) {
	std::cerr << "Unknown exception thrown during testing\n";
	throw;
}
#endif