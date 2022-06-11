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

namespace daw::json {
	template<>
	struct json_data_contract<Thing> {
		static constexpr char const a[] = "a";
		using type = json_member_list<json_number<a, int>>;

		static constexpr auto to_json_data( Thing const &thing ) {
			return std::forward_as_tuple( thing.a );
		}
	};

	template<>
	struct json_data_contract<json_alt<Thing>> {
		static constexpr char const a[] = "a";
		using type = json_tuple_member_list<int>;

		static constexpr auto to_json_data( Thing const &thing ) {
			return std::forward_as_tuple( thing.a );
		}
	};
} // namespace daw::json

int main( ) {
	using namespace daw::json;
	std::string_view const jsonA = R"({ "a": 42 } )";
	std::string_view const jsonB = R"([ 42 ] )";
	Thing a = from_json<Thing>( jsonA );
	Thing b = from_json<json_base::json_class<json_alt<Thing>>>( jsonB );
	if( a.a != b.a ) {
		std::cerr << "Error parsing\n";
		std::terminate( );
	}
	auto const js = to_json<json_alt<Thing>>( b );
	if( js != "[42]" ) {
		std::cerr << "Round-trip failure\n";
		std::terminate( );
	}
}
