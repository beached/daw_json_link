// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/daw_json_link
//
//

#include <daw/json/daw_json_link.h>

#include <map>
#include <string>
#include <tuple>
#include <vector>

struct Foo {
	int a;
	std::string b;
	std::vector<std::map<std::string, double>> c;
};

namespace daw::json {
	template<>
	struct json_data_contract<Foo> {
		static constexpr char const a[] = "a";
		static constexpr char const b[] = "b";
		static constexpr char const c[] = "c";
		using type = json_member_list<
		  json_link<a, int>, json_link<b, std::string>,
		  json_link<c, std::vector<std::map<std::string, double>>>>;

		static constexpr auto to_json_data( Foo const &f ) {
			return std::forward_as_tuple( f.a, f.b, f.c );
		}
	};
} // namespace daw::json

int main( ) {
	using namespace std::literals;
	auto json_data = R"({
		"a": 5,
		"b": "hello world",
		"c": [{"a": 1.1, "b": 2.2}, {"c": 3.3, "d": 4.4}]})"s;

	auto foo_val = daw::json::from_json<Foo>( json_data );
	daw_json_ensure( foo_val.a == 5, daw::json::ErrorReason::InvalidNumber );
	auto json_data2 = daw::json::to_json( foo_val );
	auto foo_val2 = daw::json::from_json<Foo>( json_data2 );
	daw_json_ensure( foo_val.a == foo_val2.a,
	                 daw::json::ErrorReason::InvalidNumber );
	daw_json_ensure( foo_val.b == foo_val2.b,
	                 daw::json::ErrorReason::InvalidString );
	daw_json_ensure( foo_val.c == foo_val2.c, daw::json::ErrorReason::Unknown );
}
