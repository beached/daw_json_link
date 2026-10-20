// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/daw_json_link
//
// Arrays that were skipped before being parsed (known bounds) use the element
// count from the skip as a reserve hint.  It must only ever be a hint.

#include "defines.h"

#include <daw/json/daw_json_link.h>

#include <daw/daw_ensure.h>

#include <optional>
#include <string>
#include <string_view>
#include <vector>

struct OutOfOrder {
	std::string name;
	std::vector<int> values;
};

struct OutOfOrderStrings {
	std::string name;
	std::vector<std::string> tags;
};

struct Element {
	int id;
};

// Same shape as issue #357, a missing nullable member before arrays forces
// the arrays to be skipped and parsed with known bounds
struct System {
	std::string name;
	std::optional<std::string> version;
	std::vector<Element> b;
	std::vector<Element> br;
};

namespace daw::json {
	template<>
	struct json_data_contract<OutOfOrder> {
		static constexpr char const name[] = "name";
		static constexpr char const values[] = "values";
		using type =
		  json_member_list<json_string<name>, json_array<values, int>>;
	};

	template<>
	struct json_data_contract<OutOfOrderStrings> {
		static constexpr char const name[] = "name";
		static constexpr char const tags[] = "tags";
		using type =
		  json_member_list<json_string<name>, json_array<tags, std::string>>;
	};

	template<>
	struct json_data_contract<Element> {
		static constexpr char const id[] = "id";
		using type = json_member_list<json_number<id, int>>;
	};

	template<>
	struct json_data_contract<System> {
		static constexpr char const name[] = "name";
		static constexpr char const version[] = "version";
		static constexpr char const b[] = "b";
		static constexpr char const br[] = "br";
		using type =
		  json_member_list<json_string<name>, json_string_null<version>,
		                   json_array<b, Element>, json_array<br, Element>>;
	};
} // namespace daw::json

int main( ) {
	{
		// values comes before name, so it is skipped and then parsed with a count
		auto const v = daw::json::from_json<OutOfOrder>(
		  R"({ "values": [ 1, 2, 3, 4, 5 ], "name": "a" })" );
		daw_ensure( v.name == "a" );
		daw_ensure( ( v.values == std::vector<int>{ 1, 2, 3, 4, 5 } ) );
		// The exact count is reserved, not a speculative block
		daw_ensure( v.values.capacity( ) == v.values.size( ) );
	}
	{
		auto const v = daw::json::from_json<OutOfOrder>(
		  R"({ "values": [ ], "name": "a" })" );
		daw_ensure( v.values.empty( ) );
	}
	{
		// Commas and brackets inside strings must not be counted
		auto const v = daw::json::from_json<OutOfOrderStrings>(
		  R"({ "tags": [ "a,b", "]", "c\",d", "[x," ], "name": "a" })" );
		daw_ensure( ( v.tags == std::vector<std::string>{ "a,b", "]", "c\",d",
		                                                   "[x," } ) );
		daw_ensure( v.tags.capacity( ) == v.tags.size( ) );
	}
	{
		auto const v = daw::json::from_json<OutOfOrder>(
		  R"({ "values": [ 7 ], "name": "a" })" );
		daw_ensure( ( v.values == std::vector<int>{ 7 } ) );
	}
	{
		auto const s = daw::json::from_json<System>( R"json(
{
	"name": "foo",
	"b": [ { "id": 1 } ],
	"br": [ { "id": 2 }, { "id": 3 }, { "id": 4 } ]
})json" );
		daw_ensure( not s.version );
		daw_ensure( s.b.size( ) == 1 and s.b[0].id == 1 );
		daw_ensure( s.br.size( ) == 3 and s.br[0].id == 2 and s.br[2].id == 4 );
	}
	{
		// In order, so the size is not known.  The default reserves nothing
		auto const v = daw::json::from_json<OutOfOrder>(
		  R"({ "name": "a", "values": [ 1, 2, 3, 4, 5 ] })" );
		daw_ensure( ( v.values == std::vector<int>{ 1, 2, 3, 4, 5 } ) );
		daw_ensure( v.values.capacity( ) < 128 );
	}
	{
		using namespace daw::json::options;
		auto const v = daw::json::from_json<OutOfOrder>(
		  R"({ "name": "a", "values": [ 1, 2, 3, 4, 5 ] })",
		  parse_flags<ArrayReserve::elements_128> );
		daw_ensure( ( v.values == std::vector<int>{ 1, 2, 3, 4, 5 } ) );
		daw_ensure( v.values.capacity( ) == 128 );
	}
	{
		// A known count wins over the ArrayReserve option
		using namespace daw::json::options;
		auto const v = daw::json::from_json<OutOfOrder>(
		  R"({ "values": [ 1, 2, 3, 4, 5 ], "name": "a" })",
		  parse_flags<ArrayReserve::elements_1024> );
		daw_ensure( v.values.capacity( ) == v.values.size( ) );
	}
	{
		// Known bounds without a skip, the counter is 0 and the hint is wrong.
		// The result must still be complete
		auto const v =
		  daw::json::from_json<std::vector<int>, true>( "[ 1, 2, 3, 4, 5, 6 ]" );
		daw_ensure( ( v == std::vector<int>{ 1, 2, 3, 4, 5, 6 } ) );
	}
}
