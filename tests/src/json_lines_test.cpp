// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/daw_json_link
//

#include <daw/json/daw_json_lines_iterator.h>
#include <daw/json/daw_json_link.h>

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

	template<>
	struct json_data_contract<json_alt<Element>> {
		using type = json_ordered_member_list<int, bool>;

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

	auto lines_range = daw::json::json_lines_range<Element>( json_lines );
	auto elements =
	  std::vector<Element>( lines_range.begin( ), lines_range.end( ) );
	assert( elements.size( ) == 2 );
	assert( elements[0].a == 1 );
	assert( not elements[0].b );
	assert( elements[1].a == 2 );
	assert( elements[1].b );

	auto json_lines_result = std::string( );
	auto it = std::back_inserter( json_lines_result );
	for( auto const &e : elements ) {
		(void)daw::json::to_json( e, it );
		*it = '\n';
	}

	auto lines_range2 = daw::json::json_lines_range<Element>( json_lines_result );
	auto elements2 =
	  std::vector<Element>( lines_range2.begin( ), lines_range2.end( ) );
	assert( elements2.size( ) == 2 );
	assert( elements2[0].a == 1 );
	assert( not elements2[0].b );
	assert( elements2[1].a == 2 );
	assert( elements2[1].b );

	// Use a tuple to parse
	constexpr daw::string_view json_lines2 = R"json(
[1,false]
[2,true]
)json";

	using tp_iter_t = daw::json::json_lines_iterator<std::tuple<int, bool>>;

	auto first = tp_iter_t( json_lines2 );
	auto last = tp_iter_t( );
	assert( first != last );
	auto [a0, b0] = *first;
	assert( a0 == 1 );
	assert( not b0 );
	++first;
	assert( first != last );
	auto [a1, b1] = *first;
	assert( a1 == 2 );
	assert( b1 );
	++first;
	assert( first == last );
	first = tp_iter_t( json_lines2 );
	auto elements3 = std::vector<std::tuple<int, bool>>( first, last );
	assert( elements3.size( ) == 2 );
	assert( std::get<0>( elements3[0] ) == 1 );
	assert( not std::get<1>( elements3[0] ) );
	assert( std::get<0>( elements3[1] ) == 2 );
	assert( std::get<1>( elements3[1] ) );

	auto tp_range =
	  daw::json::json_lines_range<daw::json::json_alt<Element>>( json_lines2 );
	auto elements4 = std::vector<Element>( tp_range.begin( ), tp_range.end( ) );
	assert( elements4.size( ) == 2 );
	assert( elements4[0].a == 1 );
	assert( not elements4[0].b );
	assert( elements4[1].a == 2 );
	assert( elements4[1].b );
}
