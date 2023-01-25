// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/daw_json_link
//

#include "defines.h"

#include <daw/json/daw_json_link.h>

#include <iostream>
#include <optional>
#include <tuple>

struct Foo {
	std::optional<int> bar;
};

struct Zaz {
	int z;
};

struct Foo2 {
	std::optional<int> bar;
	Zaz z;
};

namespace daw::json {
	template<>
	struct json_data_contract<Foo> {
		static constexpr char const bar[] = "bar";
		using type = json_member_list<json_number_null<bar, std::optional<int>>>;

		static constexpr auto to_json_data( Foo const &foo ) {
			return std::forward_as_tuple( foo.bar );
		}
	};

	template<>
	struct json_data_contract<Zaz> {
		static constexpr char const z[] = "z";
		using type = json_member_list<json_number<z, int>>;

		static constexpr auto to_json_data( Zaz const &foo ) {
			return std::forward_as_tuple( foo.z );
		}
	};

	template<>
	struct json_data_contract<Foo2> {
		using exact_class_mapping = void;

		static constexpr char const bar[] = "bar";
		static constexpr char const z[] = "z";
		using type = json_member_list<json_number_null<bar, std::optional<int>>,
		                              json_class<z, Zaz>>;

		static constexpr auto to_json_data( Foo2 const &foo ) {
			return std::forward_as_tuple( foo.bar, foo.z );
		}
	};
} // namespace daw::json

inline constexpr std::string_view json_doc1 = R"json(
{
	"barr": 5;
}
)json";

inline constexpr std::string_view json_doc2 = R"json(
{
	"bar": 5,
	"z": {
		"z": 42,
		"Hello": 4
	}
}
)json";

inline constexpr std::string_view json_doc3 = R"json(
{
	"Hello": 4,
	"bar": 5,
	"z": {
		"z": 42
	}
}
)json";

int main( ) {
	bool error1_found = false;
	namespace opt = daw::json::options;
	try {
		// Any extra member is an error globally
		auto f = daw::json::from_json<Foo>(
		  json_doc1, opt::parse_flags<opt::UseExactMappingsByDefault::yes> );
		(void)f;
	} catch( daw::json::json_exception const & ) { error1_found = true; }
	ensure( error1_found );

	bool error2_found = false;
	try {
		// Per class error, only exact for Foo2, not child objects are errors
		auto f = daw::json::from_json<Foo2>( json_doc2 );
		(void)f;
	} catch( daw::json::json_exception const & ) {
		error2_found = true;
	}
	ensure( not error2_found );

	try {
		// Per class error, only exact for Foo2, not child objects are errors
		auto f = daw::json::from_json<Foo2>( json_doc3 );
		(void)f;
	} catch( daw::json::json_exception const & ) {
		error2_found = true;
	}
	ensure( error2_found );
}
