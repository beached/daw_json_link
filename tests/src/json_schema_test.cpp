// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/daw_json_link
//

#include <daw/json/daw_json_link.h>
#include <daw/json/to_json_schema.h>

#include <cstdio>
#include <string>
#include <tuple>

struct Foo {
	int a;
	double b;
	std::string c;
	std::vector<int> d;
};

namespace daw::json {
	template<>
	struct json_data_contract<Foo> {
		static constexpr char const a[] = "a";
		static constexpr char const b[] = "b";
		static constexpr char const c[] = "c";
		static constexpr char const d[] = "d";
		using type = json_member_list<json_link<a, int>, json_link<b, double>,
		                              json_link<c, std::string>,
		                              json_link<d, std::vector<int>>>;

		static inline auto to_json_data( Foo const &v ) {
			return std::forward_as_tuple( v.a, v.b, v.c, v.d );
		}
	};
} // namespace daw::json

int main( ) {
	std::string result = daw::json::to_json_schema<Foo>( "", "Foo" );
	puts( result.c_str( ) );

	puts( "----\n" );

	puts( daw::json::to_json( Foo{ } ).c_str( ) );
}
