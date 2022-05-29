// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/daw_json_link
//

#include <daw/json/daw_json_lines_iterator.h>
#include <daw/json/daw_json_link.h>

#include <iostream>
#include <string>
#include <tuple>
#include <vector>

struct Element {
	int a;
	bool b;
};

namespace daw::json {
	template<>
	struct json_data_contract<Element> {
		static constexpr char const a[] = "a";
		static constexpr char const b[] = "b";
		using type = json_member_list<json_link<a, int>, json_link<b, bool>>;

		static constexpr auto to_json_data( Element const &e ) {
			return std::forward_as_tuple( e.a, e.b );
		}
	};
} // namespace daw::json

int main( ) {
	constexpr daw::string_view json_lines = R"json(
{"a":1,"b":false}
{"a":2,"b":true}
)json";

	auto rng = daw::json::json_lines_range<Element>( json_lines );
	auto elements = std::vector( rng.begin( ), rng.end( ) );
	assert( elements.size( ) == 2 );
	assert( elements[0].a == 1 );
	assert( not elements[0].b );
	assert( elements[1].a == 2 );
	assert( elements[1].b );

	auto result = std::string( );
	auto it = std::back_inserter( result );
	for( auto const &e : elements ) {
		(void)daw::json::to_json( e, it );
		*it = '\n';
	}
	std::cout << result;
}
