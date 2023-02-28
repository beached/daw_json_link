// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/daw_json_link_describe
//

#include <daw/json/daw_json_link_describe.h>

#include <boost/describe.hpp>
#include <cassert>
#include <iostream>
#include <map>
#include <memory>
#include <optional>

struct X {
	int m1;
	int m2;
};
BOOST_DESCRIBE_STRUCT( X, ( ), ( m1, m2 ) );

struct Y {
	X m0;
	std::string m1;
};
BOOST_DESCRIBE_STRUCT( Y, ( ), ( m0, m1 ) );

struct Z {
	std::map<std::string, int> kv;
};
BOOST_DESCRIBE_STRUCT( Z, ( ), ( kv ) );

struct Foo {
	std::optional<Y> m0;
	std::vector<X> m1;
	std::shared_ptr<int> m2;
};
BOOST_DESCRIBE_STRUCT( Foo, ( ), ( m0, m1, m2 ) );

int main( ) {
	constexpr daw::string_view json_doc0 = R"json(
{
	"m1": 55,
	"m2": 123
}
)json";
	constexpr auto val0 = daw::json::from_json<X>( json_doc0 );
	assert( val0.m1 == 55 );
	assert( val0.m2 == 123 );
	std::cout << "json: " << daw::json::to_json( val0 ) << '\n';

	constexpr daw::string_view json_doc1 = R"json(
{
	"m0": { "m1": 55, "m2": 123 },
	"m1": "Hello World!"
}
)json";

	auto const val1 = daw::json::from_json<Y>( json_doc1 );
	assert( val1.m0.m1 == 55 );
	assert( val1.m0.m2 == 123 );
	assert( val1.m1 == "Hello World!" );
	std::cout << "json: " << daw::json::to_json( val1 ) << '\n';

	constexpr daw::string_view json_doc2 = R"json(
{
	"kv": { "key0": 0, "key1": 1, "key2": 2 },
}
)json";
	auto val2 = daw::json::from_json<Z>( json_doc2 );
	auto p = val2.kv.find( "key0" );
	assert( p != val2.kv.end( ) );
	assert( p->second == 0 );

	p = val2.kv.find( "key1" );
	assert( p != val2.kv.end( ) );
	assert( p->second == 1 );

	p = val2.kv.find( "key2" );
	assert( p != val2.kv.end( ) );
	assert( p->second == 2 );
	std::cout << "json: " << daw::json::to_json( val2 ) << '\n';

	constexpr daw::string_view json_doc3 = R"json(
{
	"m1": [ { "m1": 0, "m2": 1 }, { "m1": 2, "m2": 3 } ]
}
)json";
	auto val3 = daw::json::from_json<Foo>( json_doc3 );
	assert( not val3.m0 );
	assert( val3.m1.size( ) == 2 );
	assert( val3.m1[0].m1 == 0 );
	assert( val3.m1[0].m2 == 1 );
	assert( val3.m1[1].m1 == 2 );
	assert( val3.m1[1].m2 == 3 );
	std::cout << "json: " << daw::json::to_json( val3 ) << '\n';
	using namespace daw::json::options;
	auto json_doc3b = daw::json::to_json( val3, output_flags<SerializationFormat::Pretty> );
	std::cout << "pretty json: " << json_doc3b << '\n';
	auto val3b = daw::json::from_json<Foo>( json_doc3b );
	assert( not val3b.m0 );
	assert( val3b.m1.size( ) == 2 );
	assert( val3b.m1[0].m1 == 0 );
	assert( val3b.m1[0].m2 == 1 );
	assert( val3b.m1[1].m1 == 2 );
	assert( val3b.m1[1].m2 == 3 );
}
