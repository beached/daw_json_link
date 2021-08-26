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
#include <daw/daw_parse_args.h>

#include <cstdlib>
#include <fstream>
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
			auto rng = p.value.get_raw_state( );
			auto s =
			  daw::string_view( rng.first, rng.size( ) ).pop_front( 10 ).to_string( );
			throw std::runtime_error( s );
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
void minify( std::string_view data, Iterator out_it ) {
	auto handler = JSONMinifyHandler( out_it );
	daw::json::json_event_parser<daw::json::ConformancePolicy>( data, handler );
}

extern "C" int LLVMFuzzerTestOneInput( std::uint8_t const *data,
                                       std::size_t size ) {
	if( data == nullptr or size == 0 ) {
		return 0;
	}
	auto ofile = std::ofstream( "/dev/null", std::ios::trunc | std::ios::binary );
	auto json_doc =
	  std::string_view( reinterpret_cast<char const *>( data ), size );

	try {
		auto jv =
		  daw::json::basic_json_value<daw::json::ConformancePolicy>( json_doc );
		switch( jv.type( ) ) {
		case daw::json::JsonBaseParseTypes::Number:
			ofile << daw::json::from_json<double>( jv );
			break;
		case daw::json::JsonBaseParseTypes::Bool:
			ofile << daw::json::from_json<bool>( jv );
			break;
		case daw::json::JsonBaseParseTypes::String:
			ofile << daw::json::from_json<std::string>( jv );
			break;
		case daw::json::JsonBaseParseTypes::Class:
			minify( json_doc, std::ostreambuf_iterator<char>( ofile ) );
			break;
		case daw::json::JsonBaseParseTypes::Array:
			minify( json_doc, std::ostreambuf_iterator<char>( ofile ) );
			break;
		case daw::json::JsonBaseParseTypes::Null:
			break;
		case daw::json::JsonBaseParseTypes::None:
			throw std::exception( );
		}
	} catch( ... ) {}
	return 0;
}

int main( int argc, char **argv ) {
	if( argc < 2 ) {
		std::cerr << "Must supply path to json document\n";
		std::cerr << argv[0] << " json_in.json\n";
		exit( EXIT_FAILURE );
	}
	auto data = daw::filesystem::memory_mapped_file_t<std::uint8_t>( argv[1] );
	return LLVMFuzzerTestOneInput( data.data( ), data.size( ) );
}
