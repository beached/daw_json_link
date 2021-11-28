// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/daw_json_link
//

#include <daw/daw_do_not_optimize.h>
#include <daw/daw_memory_mapped_file.h>
#include <daw/json/daw_json_event_parser.h>
#include <daw/json/daw_json_link.h>

#include <cstdlib>
#include <filesystem>
#include <iostream>
#include <sstream>
#include <string>

using namespace std::string_literals;

enum class expected_result : char { accept = 'y', reject = 'n', either = 'i' };

std::string to_string( expected_result r ) {
	switch( r ) {
	case expected_result::accept:
		return R"("accept")";
	case expected_result::reject:
		return R"("reject")";
	case expected_result::either:
		return R"("either")";
	}
	DAW_UNREACHABLE( );
}

constexpr std::pair<std::string_view, bool> check_expected( expected_result expected,
                                                            expected_result actual ) {
	if( expected == actual or expected == expected_result::either ) {
		return { R"("not_error")", true };
	}
	return { R"("error")", false };
}

class JSONValidateHandler {
	struct stack_value {
		bool is_class;
		bool is_first = true;

		constexpr stack_value( bool isClass )
		  : is_class( isClass ) {}
	};
	std::vector<stack_value> member_count_stack{ };

public:
	JSONValidateHandler( ) = default;

	template<typename ParsePolicy>
	bool handle_on_value( daw::json::basic_json_pair<ParsePolicy> p ) {
		if( member_count_stack.empty( ) ) {
			// We are root object/array
			member_count_stack.emplace_back( p.value.is_class( ) );
		}
		auto const type = p.value.type( );
		switch( type ) {
		case daw::json::JsonBaseParseTypes::String: {
			auto r = daw::json::from_json<std::string>( p.value );
			(void)r;
		}
			return true;
		case daw::json::JsonBaseParseTypes::Null: {
			auto r = daw::json::from_json<std::optional<int>>( p.value );
			(void)r;
		}
			return true;
		case daw::json::JsonBaseParseTypes::Number: {
			auto r = daw::json::from_json<double>( p.value );
			(void)r;
		}
			return true;
		case daw::json::JsonBaseParseTypes::Bool: {
			auto r = daw::json::from_json<bool>( p.value );
			(void)r;
		}
			return true;
		case daw::json::JsonBaseParseTypes::Class:
		case daw::json::JsonBaseParseTypes::Array:
			return true;
		case daw::json::JsonBaseParseTypes::None:
			daw::json::daw_json_error( daw::json::ErrorReason::UnexpectedEndOfData );
		default: {
			auto rng = p.value.get_raw_state( );
			throw daw::json::json_exception( daw::json::ErrorReason::Unknown, rng.first );
		}
		}
	}

	template<typename ParsePolicy>
	bool handle_on_array_start( daw::json::basic_json_value<ParsePolicy> ) {
		member_count_stack.emplace_back( false );
		return true;
	}

	bool handle_on_array_end( ) {
		member_count_stack.pop_back( );
		return true;
	}

	template<typename ParsePolicy>
	bool handle_on_class_start( daw::json::basic_json_value<ParsePolicy> ) {
		member_count_stack.emplace_back( true );
		return true;
	}

	bool handle_on_class_end( ) {
		member_count_stack.pop_back( );
		return true;
	}
};

std::pair<expected_result, std::string> test_pass_fail( std::string_view json_data ) {
	if( json_data.data( ) == nullptr ) {
		return { expected_result::reject, "empty file"s };
	}
	auto ps = daw::json::ConformancePolicy( std::data( json_data ), daw::data_end( json_data ) );
	ps.trim_left( );
	if( not ps.has_more( ) ) {
		return { expected_result::reject, "empty file"s };
	}
	try {
		auto jv = daw::json::basic_json_value<daw::json::ConformancePolicy>( ps );

		switch( jv.type( ) ) {
		case daw::json::JsonBaseParseTypes::String: {
			auto r = daw::json::from_json<std::string>( jv );
			(void)r;
		} break;
		case daw::json::JsonBaseParseTypes::Null: {
			auto r = daw::json::from_json<std::optional<int>>( jv );
			(void)r;
		} break;
		case daw::json::JsonBaseParseTypes::Number: {
			auto r = daw::json::from_json<double>( jv );
			(void)r;
		} break;
		case daw::json::JsonBaseParseTypes::Bool: {
			auto r = daw::json::from_json<bool>( jv );
			(void)r;
		} break;
		case daw::json::JsonBaseParseTypes::Class:
		case daw::json::JsonBaseParseTypes::Array:
			daw::json::json_event_parser<daw::json::ConformancePolicy>( jv, JSONValidateHandler{ } );
			break;
		case daw::json::JsonBaseParseTypes::None:
			return { expected_result::reject, "empty file"s };
		default: {
			auto rng = jv.get_raw_state( );
			throw daw::json::json_exception( daw::json::ErrorReason::Unknown, rng.first );
		}
		}
		ps = jv.get_raw_state( );
		(void)daw::json::json_details::skip_value( ps );
		if( not ps.is_null( ) ) {
			ps.trim_left( );
			if( ps.has_more( ) ) {
				return { expected_result::reject, "unexpected data after value"s };
			}
		}
		return { expected_result::accept, ""s };
	} catch( daw::json::json_exception const &jex ) {
		return { expected_result::reject,
		         static_cast<std::string>( reason_message( jex.reason_type( ) ) ) };
	}
}

int main( int argc, char **argv ) {
	if( argc < 1 ) {
		std::cerr << "Must supply folder with JSONTestSuite files as argument\n";
		exit( EXIT_FAILURE );
	}
	std::filesystem::path p = std::filesystem::path( argv[1] );
	if( not is_directory( p ) ) {
		std::cerr << "Must supply folder with JSONTestSuite files as argument\n";
		exit( EXIT_FAILURE );
	}
	std::cout << "\"filename\",\"status\",\"is error\",\"error message\"\n";
	std::string error_lines{ };

	// These files are expected to fail.  These are things like trailing comma
	// extensions
	std::string_view allow_list[] = { "n_object_trailing_comma.json",
	                                  "n_array_number_and_comma.json",
	                                  "n_array_extra_comma.json" };
	for( auto const &entry : std::filesystem::recursive_directory_iterator( p ) ) {
		if( not entry.is_regular_file( ) ) {
			continue;
		}
		auto const fname = entry.path( ).filename( ).string( );
		/*
		if( fname != "n_number_-2..json" ) {
		  continue;
		}
		*/
		if( fname.size( ) <= 3 or fname[1] != '_' or
		    not( fname[0] == 'i' or fname[0] == 'y' or fname[0] == 'n' ) ) {
			continue;
		}
		auto const json_data = [&] {
			if constexpr( std::is_same_v<std::filesystem::path::value_type, char> ) {
				return daw::filesystem::memory_mapped_file_t<char>( entry.path( ).string( ) );
			} else {
				return daw::filesystem::memory_mapped_file_t<char>( entry.path( ).wstring( ) );
			}
		}( );
		bool const is_on_allow_list =
		  std::find( std::data( allow_list ),
		             daw::data_end( allow_list ),
		             std::string_view( fname ) ) != daw::data_end( allow_list );

		auto [result, message] = test_pass_fail( json_data );
		auto [check_msg, check_stat] =
		  check_expected( static_cast<expected_result>( fname[0] ), result );
		std::stringstream ss;
		ss << check_msg << ',' << std::quoted( fname ) << ',' << to_string( result ) << ','
		   << std::quoted( message );
		if( is_on_allow_list ) {
			ss << ", \"allowed\"\n";
		} else {
			ss << '\n';
		}
		if( check_stat or is_on_allow_list ) {
			std::cout << ss.str( );
		} else {
			error_lines.append( ss.str( ) );
		}
	}
	std::cout << error_lines;
	std::cout << error_lines;
	if( not error_lines.empty( ) ) {
		return 1;
	}
}