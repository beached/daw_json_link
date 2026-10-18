// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/daw_json_link
//
// Serializing a class tracks the member names it has written only when a name
// could be written twice: with dependent members, such as a tagged variant's
// tag, or with two members of the same name. These check the output in both
// cases and in the untracked one.

#include "daw/json/daw_json_link.h"

#include <daw/daw_ensure.h>

#include <cstddef>
#include <cstdio>
#include <string>
#include <variant>

namespace tests {
	struct plain {
		int a;
		std::string b;
	};

	struct repeated {
		int first;
		int second;
	};

	struct tagged {
		std::string name;
		std::variant<std::string, int, bool> value;
	};

	// The tag is also a member of its own
	struct tag_member {
		int type;
		std::variant<std::string, int, bool> value;
	};

	struct tagged_switcher {
		constexpr std::size_t operator( )( int type ) const {
			return static_cast<std::size_t>( type );
		}
		int operator( )( tagged const &v ) const {
			return static_cast<int>( v.value.index( ) );
		}
		int operator( )( tag_member const &v ) const {
			return static_cast<int>( v.value.index( ) );
		}
	};

	// Named, so that C++17 can use them as template arguments
	struct names {
		static constexpr char const a[] = "a";
		static constexpr char const b[] = "b";
		static constexpr char const name[] = "name";
		static constexpr char const type[] = "type";
		static constexpr char const value[] = "value";
	};
} // namespace tests

namespace daw::json {
	template<>
	struct json_data_contract<tests::plain> {
		using type = json_member_list<json_link<tests::names::a, int>,
		                              json_link<tests::names::b, std::string>>;

		static constexpr auto to_json_data( tests::plain const &v ) {
			return std::forward_as_tuple( v.a, v.b );
		}
	};

	template<>
	struct json_data_contract<tests::repeated> {
		using type = json_member_list<json_link<tests::names::a, int>,
		                              json_link<tests::names::a, int>>;

		static constexpr auto to_json_data( tests::repeated const &v ) {
			return std::forward_as_tuple( v.first, v.second );
		}
	};

	template<>
	struct json_data_contract<tests::tagged> {
		using type = json_member_list<
		  json_string<tests::names::name>,
		  json_tagged_variant<tests::names::value,
		                      std::variant<std::string, int, bool>,
		                      json_number<tests::names::type, int>,
		                      tests::tagged_switcher>>;

		static constexpr auto to_json_data( tests::tagged const &v ) {
			return std::forward_as_tuple( v.name, v.value );
		}
	};

	template<>
	struct json_data_contract<tests::tag_member> {
		using type = json_member_list<
		  json_number<tests::names::type, int>,
		  json_tagged_variant<tests::names::value,
		                      std::variant<std::string, int, bool>,
		                      json_number<tests::names::type, int>,
		                      tests::tagged_switcher>>;

		static constexpr auto to_json_data( tests::tag_member const &v ) {
			return std::forward_as_tuple( v.type, v.value );
		}
	};
} // namespace daw::json

int main( ) {
	auto const plain = daw::json::to_json( tests::plain{ 1, "x" } );
	daw_ensure( plain == R"({"a":1,"b":"x"})" );

	// The first of the two members named "a" is the one written
	auto const repeated = daw::json::to_json( tests::repeated{ 1, 2 } );
	daw_ensure( repeated == R"({"a":1})" );

	// The tag is written ahead of the members
	auto const tagged =
	  daw::json::to_json( tests::tagged{ "n", std::variant<std::string, int, bool>{ 42 } } );
	daw_ensure( tagged == R"({"type":1,"name":"n","value":42})" );
	auto const tagged_back = daw::json::from_json<tests::tagged>( tagged );
	daw_ensure( tagged_back.name == "n" and
	            std::get<int>( tagged_back.value ) == 42 );

	// The tag is written once, as the dependent member
	auto const tag_member = daw::json::to_json(
	  tests::tag_member{ 1, std::variant<std::string, int, bool>{ 42 } } );
	daw_ensure( tag_member == R"({"type":1,"value":42})" );

	std::puts( "visited_members_test passed" );
}
