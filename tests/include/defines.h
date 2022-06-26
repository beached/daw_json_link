// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/daw_json_link
//

#pragma once

#include "daw/json/daw_json_exception.h"
#include "daw/json/daw_json_find_path.h"

#include <ciso646>
#include <iostream>
#include <string_view>

/***
 * If requested to disable or MSVC, there is a bug in MSVC in C++20 mode
 */
#if defined( DAW_JSON_NO_CONST_EXPR ) or                 \
  ( defined( _MSC_VER ) and not defined( __clang__ ) and \
    defined( DAW_CXX_STANDARD ) and DAW_CXX_STANDARD == 20 )
#define DAW_CONSTEXPR
#else
#define DAW_CONSTEXPR constexpr
#endif

template<typename StringView>
DAW_ATTRIB_NOINLINE void daw_ensure_error( StringView &&msg ) {
	std::cerr << msg << std::endl << std::flush;
	std::terminate( );
}

#define ensure( Bool )                                \
	if( DAW_UNLIKELY( not( Bool ) ) ) {                 \
		daw_ensure_error( "Error in assertion: " #Bool ); \
	}                                                   \
	while( false )

#define test_assert( Bool, Msg )      \
	if( DAW_UNLIKELY( not( Bool ) ) ) { \
		daw_ensure_error( Msg );          \
	}                                   \
	while( false )

DAW_ATTRIB_NOINLINE inline void
display_exception( daw::json::json_exception const &jex,
                   char const *json_data ) {
	std::cerr << "Exception thrown by parser: "
	          << to_formatted_string( jex, json_data );
	if( jex.parse_location( ) ) {
		auto path_stack = find_json_path_stack_to( jex, json_data );
		if( not path_stack.empty( ) ) {
			std::cerr << "The error is near line "
			          << ( 1 + daw::json::find_line_number_of( jex.parse_location( ),
			                                                   json_data ) )
			          << " column "
			          << ( 1 + daw::json::find_column_number_of(
			                     jex.parse_location( ), json_data ) )
			          << '\n';
			std::cerr << "JSON Path to value close to error '"
			          << to_json_path_string( path_stack ) << "'\n";
			std::cerr << std::flush;
		}
	}
}
