// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/
//

#include "defines.h"

#include <daw/json/daw_json_find_path.h>

#include <iostream>

namespace {
	template<typename Exception>
	void display_exception_impl( Exception const &jex, char const *json_data ) {
		std::cerr << "Exception thrown by parser: " << daw::json::to_formatted_string( jex, json_data );
		if( jex.parse_location( ) ) {
			auto path_stack = find_json_path_stack_to( jex, json_data );
			if( not path_stack.empty( ) ) {
				std::cerr << "The error is near line "
				          << ( 1 + daw::json::find_line_number_of( jex.parse_location( ), json_data ) )
				          << " column "
				          << ( 1 + daw::json::find_column_number_of( jex.parse_location( ), json_data ) )
				          << '\n';
				std::cerr << "JSON Path to value close to error '" << to_json_path_string( path_stack )
				          << "'\n";
			}
		}
	}
} // namespace

#if defined( DAW_JSON_INHERIT_STDEXCEPTION )
void display_exception( daw::json::basic_json_exception<true> const &jex, char const *json_data ) {
	display_exception_impl( jex, json_data );
}
#else
void display_exception( daw::json::basic_json_exception<false> const &jex, char const *json_data ) {
	display_exception_impl( jex, json_data );
}
#endif
