// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/daw_json_link
//

#include <daw/json/daw_json_iterator.h>
#include <daw/json/daw_json_link.h>

#include <daw/daw_ensure.h>
#include <daw/daw_string_view.h>

#include <tuple>

struct X {
	int a;
};

namespace daw::json {
	template<>
	struct json_data_contract<X> {
		static constexpr char const a[] = "a";
		using type = json_member_list<json_link<a, int>>;

		static constexpr auto to_json_data( X const &x ) {
			return std::forward_as_tuple( x.a );
		}
	};
} // namespace daw::json

int main( ) {
	static constexpr daw::string_view json_doc = R"json(
[
	{"a":1},
	{"a":2},
	{"a":3}
]
)json";

	auto it = daw::json::json_array_iterator<X>( json_doc );
	int sum = 0;
	for( X x : it ) {
		sum += x.a;
	}
	daw_ensure( sum == 6 );
}
