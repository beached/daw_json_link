// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/daw_json_link
//

// Ensure that parse_options is working as expected
//

#include <daw/json/impl/daw_json_parse_options.h>

#include <cassert>

int main( ) {
	using namespace daw::json;
	{
		constexpr auto precise_opt = parse_options( IEEE754Precise::yes );
		constexpr auto precise_val = json_details::get_bits_for<IEEE754Precise>( precise_opt );
		static_assert( precise_val == IEEE754Precise::yes );
	}
}
