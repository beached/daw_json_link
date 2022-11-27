// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/daw_json_link
//

#include <daw/daw_tuple_forward.h>
#include <daw/json/daw_json_link.h>

#include <array>
#include <iostream>

struct Foo {
	int data[5];
};

struct FooConstructor {
	constexpr Foo operator( )( std::array<int, 5> const &ary ) const {
		return Foo{ { ary[0], ary[1], ary[2], ary[3], ary[4] } };
	}
};

namespace daw::json {
	template<>
	struct json_data_contract<Foo> {
		using constructor_t = FooConstructor;
		static constexpr char const data[] = "data";
		using type = json_member_list<json_array<data, int, std::array<int, 5>>>;

		static constexpr auto to_json_data( Foo const &f ) {
			return daw::forward_nonrvalue_as_tuple(
			  std::array{ f.data[0], f.data[1], f.data[2], f.data[3], f.data[4] } );
		}
	};
} // namespace daw::json

template<typename Container>
void display_all( Container const &values ) {
	for( auto const &v : values ) {
		std::cout << v << ' ';
	}
	std::cout << '\n';
}

int main( ) {
	std::string_view json_doc = R"json(
{
    "data": [1,2,3,4,5]
}
)json";
	auto f = daw::json::from_json<Foo>( json_doc );
	display_all( f.data );
	daw::json::to_json( f, std::cout );
}
