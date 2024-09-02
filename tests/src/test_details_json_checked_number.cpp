// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/daw_json_link
//

#include <daw/json/daw_json_link.h>

#include <daw/daw_ensure.h>

#include <cstddef>
#include <exception>
#include <tuple>

struct Byte {
	std::byte x;
};

namespace daw::json {
	template<>
	struct json_data_contract<Byte> {
		static constexpr char const x[] = "x";
		using type = json_member_list<json_checked_number<x, std::byte>>;

		static constexpr auto to_json_data( Byte const &v ) {
			return std::forward_as_tuple( v.x );
		}
	};
} // namespace daw::json

static_assert( daw::json::from_json<Byte>( R"json({"x":42})json" ).x ==
               std::byte{ 42 } );

int main( ) {
	bool success = false;
	try {
		(void)daw::json::from_json<Byte>( R"json({"x":1024})json" );
	} catch( std::exception const &ex ) { success = true; }
	daw_ensure( success );
}
