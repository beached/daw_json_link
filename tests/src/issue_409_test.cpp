// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/
//

#include <daw/json/daw_json_link.h>

#include <iostream>
#include <optional>
#include <string>

enum class TestEnum : unsigned { A, B };

constexpr std::string_view to_string( TestEnum v ) {
	switch( v ) {
	case TestEnum::A:
		return "a";
	case TestEnum::B:
		return "b";
	}
	std::terminate( );
}

constexpr TestEnum from_string( daw::tag_t<TestEnum>, std::string_view sv ) {
	if( sv == "a" ) {
		return TestEnum::A;
	}
	if( sv == "b" ) {
		return TestEnum::B;
	}
	std::terminate( );
}

struct Test {
	std::string str;
	std::optional<TestEnum> opt;
};

struct TestUserCtor : public Test {
	TestUserCtor( std::string s, std::optional<TestEnum> o )
	  : Test{ std::move( s ), std::move( o ) } {}
};

namespace daw::json {
	template<>
	struct json_data_contract<Test> {
		static constexpr char const str[] = "str";
		static constexpr char const opt[] = "opt";
		using type =
		  json_member_list<json_string<str>,
		                   json_custom_null<opt, std::optional<TestEnum>>>;

		static constexpr auto to_json_data( Test const &v ) {
			return std::forward_as_tuple( v.str, v.opt );
		}
	};

	template<>
	struct json_data_contract<TestUserCtor> {
		static constexpr char const str[] = "str";
		static constexpr char const opt[] = "opt";
		using type =
		  json_member_list<json_string<str>,
		                   json_custom_null<opt, std::optional<TestEnum>>>;

		static constexpr auto to_json_data( TestUserCtor const &v ) {
			return std::forward_as_tuple( v.str, v.opt );
		}
	};
} // namespace daw::json

int main( ) {
	static constexpr auto &json1 = R"({ "str": "wtf" })";
	static constexpr auto &json2 = R"({ "str": "" })";
	try {
		{
			auto const cls = daw::json::from_json<Test>( json1 ); // Ok
			std::cout << "Test 1: " << daw::json::to_json( cls ) << '\n';
		}
		{
			auto const cls = daw::json::from_json<TestUserCtor>( json1 ); // Ok
			std::cout << "Test 2: " << daw::json::to_json( cls ) << '\n';
		}
		{
			auto const cls = daw::json::from_json<Test>( json2 ); // Ok
			std::cout << "Test 3: " << daw::json::to_json( cls ) << '\n';
		}
		{
			auto const cls = daw::json::from_json<TestUserCtor>( json2 ); // Error
			std::cout << "Test 4: " << daw::json::to_json( cls ) << '\n';
		}
	} catch( daw::json::json_exception const &jex ) {
		std::cerr << "Exception thrown by parser: " << jex.reason( ) << '\n';
	}
}
