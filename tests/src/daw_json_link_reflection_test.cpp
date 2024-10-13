// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/daw_json_link
//

#include <daw/daw_ensure.h>
#include <daw/daw_print.h>
#include <daw/json/daw_json_link.h>
#include <daw/json/daw_json_reflection.h>

#include <map>
#include <memory>
#include <optional>

using daw::json::reflect;

struct[[= reflect]] X {
	[[= reflect.map_as<daw::json::json_number<"member1", int>>]]
	int m1;

	[[= reflect.rename( "member2" )]]
	int m2;
};

struct[[= reflect]] Y {
	X m0;
	std::string m1;
};

struct[[= reflect]] Z {
	std::map<std::string, int> kv;
};

struct[[= reflect]] Foo {
	std::optional<Y> m0;
	std::vector<X> m1;
	std::shared_ptr<int> m2;
};

class[[= reflect]] A {
	mutable int counter = 0;

public:
	std::string value = "";

	explicit A( ) = default;

	explicit A( std::string v )
	  : value( std::move( v ) ) {}

	int foo( ) const {
		return ++counter;
	}
};

struct NoRefl {
	int x = 55;
};

struct[[= reflect]] NumberHalf {
	constexpr auto operator( )( auto y ) const {
		return y / 2;
	}
};

template<>
struct daw::json::json_data_contract<NoRefl> {
	using type =
	  json_member_list<json_number<"x", int, number_opts_def, NumberHalf>>;

	static constexpr auto to_json_data( NoRefl const &nr ) {
		return std::tuple{ nr.x * 2 };
	}
};

enum class EFoo {
	AChoo,
	BlessYou
};

struct [[=reflect]] EnumMember {
	EFoo foo;
};

struct [[=reflect]] EnumMemberString {
	[[=reflect.enum_string]]
	EFoo foo;
};

int main( ) {
	constexpr daw::string_view json_doc0 = R"json(
{
	"member1": 55,
	"member2": 123
}
)json";
	constexpr auto val0 = daw::json::from_json<X>( json_doc0 );
	daw_ensure( val0.m1 == 55 );
	daw_ensure( val0.m2 == 123 );
	auto const val0_json = daw::json::to_json( val0 );
	daw::println( "json: {}", val0_json );

	constexpr daw::string_view json_doc1 = R"json(
	{
	  "m0": { "member1": 55, "member2": 123 },
	  "m1": "Hello World!"
	}
	)json";
	auto const val1 = daw::json::from_json<Y>( json_doc1 );
	daw_ensure( val1.m0.m1 == 55 );
	daw_ensure( val1.m0.m2 == 123 );
	daw_ensure( val1.m1 == "Hello World!" );
	daw::println( "json: {}", daw::json::to_json( val1 ) );

	constexpr daw::string_view json_doc2 = R"json(
	{
	  "kv": { "key0": 0, "key1": 1, "key2": 2 },
	}
	)json";
	auto val2 = daw::json::from_json<Z>( json_doc2 );
	auto p = val2.kv.find( "key0" );
	daw_ensure( p != val2.kv.end( ) );
	daw_ensure( p->second == 0 );

	p = val2.kv.find( "key1" );
	daw_ensure( p != val2.kv.end( ) );
	daw_ensure( p->second == 1 );

	p = val2.kv.find( "key2" );
	daw_ensure( p != val2.kv.end( ) );
	daw_ensure( p->second == 2 );
	daw::println( "json: {}", daw::json::to_json( val2 ) );

	constexpr daw::string_view json_doc3 = R"json(
	{
	  "m1": [ { "member1": 0, "member2": 1 }, { "member1": 2, "member2": 3 } ]
	}
	)json";
	auto val3 = daw::json::from_json<Foo>( json_doc3 );
	daw_ensure( not val3.m0 );
	daw_ensure( val3.m1.size( ) == 2 );
	daw_ensure( val3.m1[0].m1 == 0 );
	daw_ensure( val3.m1[0].m2 == 1 );
	daw_ensure( val3.m1[1].m1 == 2 );
	daw_ensure( val3.m1[1].m2 == 3 );
	daw::println( "json: {}", daw::json::to_json( val3 ) );

	using namespace daw::json::options;
	auto json_doc3b =
	  daw::json::to_json( val3, output_flags<SerializationFormat::Pretty> );
	daw::println( "pretty json: {}", json_doc3b );
	auto val3b = daw::json::from_json<Foo>( json_doc3b );
	daw_ensure( not val3b.m0 );
	daw_ensure( val3b.m1.size( ) == 2 );
	daw_ensure( val3b.m1[0].m1 == 0 );
	daw_ensure( val3b.m1[0].m2 == 1 );
	daw_ensure( val3b.m1[1].m1 == 2 );
	daw_ensure( val3b.m1[1].m2 == 3 );

	static constexpr daw::string_view json_doc4 = R"json({"value": "42"})json";
	daw::println( "json_doc4: {}", json_doc4 );
	auto val4 = daw::json::from_json<A>( json_doc4 );
	daw_ensure( val4.value == "42" );
	auto const val4_json = daw::json::to_json( val4 );
	daw::println( "json: {}", val4_json );

	static constexpr daw::string_view json_doc5 = R"json({"x": 42})json";
	daw::println( "json_doc5: {}", json_doc5 );
	auto val5 = daw::json::from_json<NoRefl>( json_doc5 );
	// If refletion is used, x will be 42
	daw_ensure( val5.x == 21 );
	auto const val5_json = daw::json::to_json( val4 );
	daw::println( "json: {}", val5_json );

	auto afoo0 = EnumMember{ EFoo::AChoo };
	auto afoo1 = EnumMember{ EFoo::BlessYou };
	auto const val6_json = daw::json::to_json( afoo0 );
	daw::println( "EnumMember{{ EFoo::AChoo }}; as json: {}", val6_json );
	auto const val7_json = daw::json::to_json( afoo1 );
	daw::println( "EnumMember{{ EFoo::BlessYou }}; as json: {}", val7_json );

	static constexpr auto achoo = daw::json::refl_details::enum_to_string( EFoo::AChoo );
	static_assert( achoo == "AChoo" );
	
	auto bfoo0 = EnumMemberString{ EFoo::AChoo };
	auto bfoo1 = EnumMemberString{ EFoo::BlessYou };
	auto const val8_json = daw::json::to_json( bfoo0 );
	daw::println( "EnumMemberString{{ EFoo::AChoo }}; as json: {}", val8_json );
	auto const val9_json = daw::json::to_json( bfoo1 );
	daw::println( "EnumMemberString{{ EFoo::BlessYou }}; as json: {}", val9_json );
	
}
