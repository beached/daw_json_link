// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/daw_json_link
//

#pragma once

#include "daw/json/daw_json_exception.h"

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

template<typename Bool>
constexpr void test_assert( Bool &&b, std::string_view msg ) {
	if( not b ) {
		std::cerr << msg << '\n';
		exit( 1 );
	}
}

void display_exception( daw::json::json_exception const &jex,
                        char const *json_data );
