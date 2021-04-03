// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/daw_json_link
//

#include <daw/json/daw_json_link.h>

#include <cstddef>
#include <iostream>
#include <string_view>

struct Thing {
	int a;
};

template<typename C, std::size_t Idx = 0>
struct json_alt {
	using type = C;
	static constexpr std::size_t index = Idx;
};

namespace daw::json {
	template<typename T, std::size_t I>
	struct default_constructor<json_alt<T, I>> : default_constructor<T> {};

	template<>
	struct json_data_contract<Thing> {
		static constexpr char const a[] = "a";
		using type = json_member_list<json_number<a, int>>;
	};

	template<>
	struct json_data_contract<json_alt<Thing>> {
		static constexpr char const a[] = "a";
		using type =
		  json_member_list<json_number<a, int, LiteralAsStringOpt::Always>>;
	};
} // namespace daw::json

int main( ) {
	std::string_view const jsonA = R"({ "a": 42 } )";
	std::string_view const jsonB = R"({ "a": "42" } )";
	Thing a = daw::json::from_json<Thing>( jsonA );
	Thing b = daw::json::from_json<json_alt<Thing>>( jsonB );
	if( a.a != b.a ) {
		std::cerr << "Error parsing\n";
		std::terminate( );
	}
}
