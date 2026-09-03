// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/daw_json_link
//

#include <daw/json/daw_json_link.h>

#include <cassert>
#include <cstdio>
#include <optional>
#include <string_view>

struct Foo {
	std::optional<int> number;
};

struct FooFromJson {
	std::optional<int> operator( )( std::string_view doc ) const {
		if( doc.empty( ) ) {
			return std::nullopt;
		}
		auto jv = daw::json::json_value( doc );
		if( jv.is_string( ) ) {
			auto sv = jv.template as<std::string_view>( );
			if( sv.empty( ) ) {
				return std::nullopt;
			}
			return daw::json::from_json<int>( sv );
		}
		assert( jv.is_number( ) );
		return daw::json::from_json<int>( jv );
	}
};

struct FooToJson {
	std::string operator( )( std::optional<int> value ) const {
		if( not value.has_value( ) ) {
			return "null";
		}
		return daw::json::to_json( *value );
	}
};

namespace daw::json {
	template<>
	struct json_data_contract<Foo> {
		static constexpr char number[] = "number";
		using type = json_member_list<
		  json_custom<number, std::optional<int>, FooFromJson, FooToJson,
		              daw::json::options::json_custom_opt(
		                daw::json::options::JsonCustomTypes::Any )>>;

		static auto to_json_data( Foo const &v ) {
			return std::forward_as_tuple( v.number );
		}
	};
} // namespace daw::json

int main( ) {
	std::string_view doc0 = R"json({"number":""})json";
	auto f0 = daw::json::from_json<Foo>( doc0 );
	assert( not f0.number.has_value( ) );
	auto fd0 = daw::json::to_json( f0 );
	std::puts( fd0.c_str( ) );

	std::string_view doc1 = R"json({"number":42})json";
	auto f1 = daw::json::from_json<Foo>( doc1 );
	assert( f1.number.has_value( ) );
	auto fd1 = daw::json::to_json( f1 );
	std::puts( fd1.c_str( ) );

	std::string_view doc2 = R"json({"number":"42"})json";
	auto f2 = daw::json::from_json<Foo>( doc2 );
	assert( f2.number.has_value( ) );
	auto fd2 = daw::json::to_json( f2 );
	std::puts( fd2.c_str( ) );

	std::string_view doc3 = R"json({"number":null})json";
	auto f3 = daw::json::from_json<Foo>( doc3 );
	assert( not f3.number.has_value( ) );
	auto fd3 = daw::json::to_json( f3 );
	std::puts( fd3.c_str( ) );
}
