// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/daw_json_link
//
// The members that others depend on, the tag of a tagged variant and the size
// of a sized array, are found from the class being parsed whatever the order of
// the members

#include "defines.h"

#include "daw/json/daw_json_link.h"

#include <optional>
#include <string>
#include <string_view>
#include <variant>
#include <vector>

#if defined( DAW_USE_EXCEPTIONS )
namespace tests {
	struct Point {
		int x;
		int y;
	};

	struct Tagged {
		std::string name;
		std::variant<std::string, int, bool, Point> value;
	};

	// The tag is also a mapped member
	struct TagMember {
		int type;
		std::variant<std::string, int, bool, Point> value;
	};

	// Two variants with one tag
	struct SharedTag {
		std::variant<std::string, int, bool, Point> first;
		std::variant<std::string, int, bool, Point> second;
	};

	struct NullableTagged {
		std::optional<std::variant<std::string, int, bool, Point>> value;
	};

	// An in-situ member is decoded in place before the variant is parsed
	struct InsituTagged {
		std::string_view name;
		std::variant<std::string, int, bool, Point> value;
	};

	struct Switcher {
		constexpr std::size_t operator( )( int type ) const {
			return static_cast<std::size_t>( type );
		}
	};

	// Builds the vector of a sized array, checking the size it was given
	struct SizedVector {
		std::vector<int> operator( )( ) const {
			return { };
		}

		template<typename Iterator>
		std::vector<int> operator( )( Iterator first, Iterator last,
		                              std::size_t size ) const {
			auto result = std::vector<int>( );
			result.reserve( size );
			result.assign( first, last );
			test_assert( result.size( ) == size, "size does not match" );
			return result;
		}
	};

	struct Sized {
		std::string name;
		std::vector<int> values;
	};

	struct NullableSized {
		std::optional<std::vector<int>> values;
	};

	struct InsituSized {
		std::string_view name;
		std::vector<int> values;
	};
} // namespace tests

namespace daw::json {
	template<>
	struct json_data_contract<tests::Point> {
		static constexpr char const x[] = "x";
		static constexpr char const y[] = "y";
		using type = json_member_list<json_number<x, int>, json_number<y, int>>;
	};

	template<>
	struct json_data_contract<tests::Tagged> {
		static constexpr char const name[] = "name";
		static constexpr char const type_mem[] = "type";
		static constexpr char const value[] = "value";
		using type = json_member_list<
		  json_string<name>,
		  json_tagged_variant<value, std::variant<std::string, int, bool, tests::Point>,
		                      json_number<type_mem, int>, tests::Switcher>>;
	};

	template<>
	struct json_data_contract<tests::TagMember> {
		static constexpr char const type_mem[] = "type";
		static constexpr char const value[] = "value";
		using type = json_member_list<
		  json_number<type_mem, int>,
		  json_tagged_variant<value, std::variant<std::string, int, bool, tests::Point>,
		                      json_number<type_mem, int>, tests::Switcher>>;
	};

	template<>
	struct json_data_contract<tests::SharedTag> {
		static constexpr char const type_mem[] = "type";
		static constexpr char const first[] = "first";
		static constexpr char const second[] = "second";
		using type = json_member_list<
		  json_tagged_variant<first, std::variant<std::string, int, bool, tests::Point>,
		                      json_number<type_mem, int>, tests::Switcher>,
		  json_tagged_variant<second,
		                      std::variant<std::string, int, bool, tests::Point>,
		                      json_number<type_mem, int>, tests::Switcher>>;
	};

	template<>
	struct json_data_contract<tests::NullableTagged> {
		static constexpr char const type_mem[] = "type";
		static constexpr char const value[] = "value";
		using type = json_member_list<json_tagged_variant_null<
		  value,
		  std::optional<std::variant<std::string, int, bool, tests::Point>>,
		  json_number<type_mem, int>, tests::Switcher>>;
	};

	template<>
	struct json_data_contract<tests::InsituTagged> {
		static constexpr char const name[] = "name";
		static constexpr char const type_mem[] = "type";
		static constexpr char const value[] = "value";
		using type = json_member_list<
		  json_string_insitu<name>,
		  json_tagged_variant<value, std::variant<std::string, int, bool, tests::Point>,
		                      json_number<type_mem, int>, tests::Switcher>>;
	};

	template<>
	struct json_data_contract<tests::Sized> {
		static constexpr char const name[] = "name";
		static constexpr char const size[] = "size";
		static constexpr char const values[] = "values";
		using type = json_member_list<
		  json_string<name>,
		  json_sized_array<values, int, json_number<size, std::size_t>,
		                   std::vector<int>, tests::SizedVector>>;
	};

	template<>
	struct json_data_contract<tests::NullableSized> {
		static constexpr char const size[] = "size";
		static constexpr char const values[] = "values";
		using type = json_member_list<json_nullable<
		  values, std::optional<std::vector<int>>,
		  json_sized_array_no_name<int, json_number<size, std::size_t>,
		                           std::vector<int>, tests::SizedVector>>>;
	};

	template<>
	struct json_data_contract<tests::InsituSized> {
		static constexpr char const name[] = "name";
		static constexpr char const size[] = "size";
		static constexpr char const values[] = "values";
		using type = json_member_list<
		  json_string_insitu<name>,
		  json_sized_array<values, int, json_number<size, std::size_t>,
		                   std::vector<int>, tests::SizedVector>>;
	};
} // namespace daw::json

namespace {
	using value_t = std::variant<std::string, int, bool, tests::Point>;

	bool same( value_t const &lhs, value_t const &rhs ) {
		if( lhs.index( ) != rhs.index( ) ) {
			return false;
		}
		if( auto const *p = std::get_if<tests::Point>( &lhs ) ) {
			auto const &q = std::get<tests::Point>( rhs );
			return p->x == q.x and p->y == q.y;
		}
		if( auto const *s = std::get_if<std::string>( &lhs ) ) {
			return *s == std::get<std::string>( rhs );
		}
		if( auto const *i = std::get_if<int>( &lhs ) ) {
			return *i == std::get<int>( rhs );
		}
		return std::get<bool>( lhs ) == std::get<bool>( rhs );
	}

	template<typename F>
	bool throws( F &&f ) {
		try {
			(void)f( );
		} catch( daw::json::json_exception const & ) { return true; }
		return false;
	}
} // namespace

int main( ) {
	using daw::json::from_json;
	using tests::Point;

	// Every order of the tag, the variant and another member, with each kind of
	// alternative
	struct order_case {
		std::string_view json;
		value_t expected;
	};
	for( auto const &c : {
	       order_case{ R"({"type":0,"name":"n","value":"s"})", value_t( "s" ) },
	       order_case{ R"({"type":1,"value":42,"name":"n"})", value_t( 42 ) },
	       order_case{ R"({"name":"n","type":2,"value":true})", value_t( true ) },
	       order_case{ R"({"name":"n","value":"s","type":0})", value_t( "s" ) },
	       order_case{ R"({"value":42,"type":1,"name":"n"})", value_t( 42 ) },
	       order_case{ R"({"value":{"x":1,"y":2},"name":"n","type":3})",
	                   value_t( Point{ 1, 2 } ) },
	       order_case{ R"({"other":[1,{"a":2}],"value":"s","extra":null,"type":0,"name":"n"})",
	                   value_t( "s" ) } } ) {
		auto const t = from_json<tests::Tagged>( c.json );
		test_assert( t.name == "n" and same( t.value, c.expected ),
		             "wrong result for " + std::string( c.json ) );
	}

	// The first member of a name is the tag
	{
		auto const t =
		  from_json<tests::Tagged>( R"({"type":1,"name":"n","value":4,"type":0})" );
		test_assert( same( t.value, value_t( 4 ) ), "first tag used" );
	}

	// The tag is also a mapped member
	for( std::string_view json :
	     { R"({"type":1,"value":42})", R"({"value":42,"type":1})" } ) {
		auto const t = from_json<tests::TagMember>( json );
		test_assert( t.type == 1 and same( t.value, value_t( 42 ) ),
		             "tag as member: " + std::string( json ) );
	}

	// Two variants with one tag
	for( std::string_view json : { R"({"type":1,"first":1,"second":2})",
	                               R"({"first":1,"second":2,"type":1})",
	                               R"({"second":2,"type":1,"first":1})" } ) {
		auto const t = from_json<tests::SharedTag>( json );
		test_assert( same( t.first, value_t( 1 ) ) and
		               same( t.second, value_t( 2 ) ),
		             "shared tag: " + std::string( json ) );
	}

	// A nullable variant only needs its tag when it has a value
	test_assert( not from_json<tests::NullableTagged>( R"({})" ).value,
	             "missing nullable variant" );
	test_assert( not from_json<tests::NullableTagged>( R"({"value":null})" ).value,
	             "null variant without tag" );
	test_assert(
	  not from_json<tests::NullableTagged>( R"({"value":null,"type":1})" ).value,
	  "null variant with tag" );
	{
		auto const t =
		  from_json<tests::NullableTagged>( R"({"value":42,"type":1})" );
		test_assert( t.value and same( *t.value, value_t( 42 ) ),
		             "nullable variant before tag" );
	}

	// A variant with a value needs its tag
	test_assert( throws( [] {
		             return from_json<tests::Tagged>( R"({"name":"n","value":4})" );
	             } ),
	             "missing tag" );
	test_assert( throws( [] {
		             return from_json<tests::NullableTagged>( R"({"value":4})" );
	             } ),
	             "missing tag for nullable variant with a value" );

	// In-situ members are decoded in place, the tag is not found by reading
	// them again
	for( std::string json : { std::string( R"({"name":"a\"b","value":"x","type":0})" ),
	                          std::string( R"({"type":0,"value":"x","name":"a\"b"})" ),
	                          std::string( R"({"value":"x","name":"a\"b","type":0})" ) } ) {
		auto const t = daw::json::from_json_insitu<tests::InsituTagged>( json );
		test_assert( t.name == R"(a"b)" and same( t.value, value_t( "x" ) ),
		             "in-situ member with tagged variant" );
	}

	// Sized arrays, in every order of the size, the array and another member
	auto const one_two_three = std::vector<int>{ 1, 2, 3 };
	auto const one_two = std::vector<int>{ 1, 2 };
	auto const four_five = std::vector<int>{ 4, 5 };
	for( std::string_view json : { R"({"size":3,"name":"n","values":[1,2,3]})",
	                               R"({"name":"n","size":3,"values":[1,2,3]})",
	                               R"({"name":"n","values":[1,2,3],"size":3})",
	                               R"({"values":[1,2,3],"size":3,"name":"n"})",
	                               R"({"values":[1,2,3],"name":"n","size":3})" } ) {
		auto const t = from_json<tests::Sized>( json );
		test_assert( t.name == "n" and t.values == one_two_three,
		             "sized array: " + std::string( json ) );
	}

	// A nullable sized array only needs its size when it has a value
	test_assert( not from_json<tests::NullableSized>( R"({})" ).values,
	             "missing nullable sized array" );
	test_assert( not from_json<tests::NullableSized>( R"({"values":null})" ).values,
	             "null sized array without size" );
	{
		auto const t =
		  from_json<tests::NullableSized>( R"({"values":[4,5],"size":2})" );
		test_assert( t.values and *t.values == four_five,
		             "nullable sized array before size" );
	}
	test_assert( throws( [] {
		             return from_json<tests::Sized>( R"({"name":"n","values":[1]})" );
	             } ),
	             "missing size" );

	// In-situ members are decoded in place, the size is not found by reading
	// them again
	for( std::string json :
	     { std::string( R"({"name":"a\"b","size":2,"values":[1,2]})" ),
	       std::string( R"({"values":[1,2],"name":"a\"b","size":2})" ) } ) {
		auto const t = daw::json::from_json_insitu<tests::InsituSized>( json );
		test_assert( t.name == R"(a"b)" and t.values == one_two,
		             "in-situ member with sized array" );
	}
}
#else
int main( ) {}
#endif
