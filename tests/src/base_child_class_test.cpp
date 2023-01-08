// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/daw_json_link
//

#include <daw/json/daw_json_link.h>

#include <cassert>
#include <memory>
#include <string_view>

struct Base {
	explicit Base( ) = default;
	virtual ~Base( ) = default;
	Base( Base const & ) = delete;
	Base( Base && ) = delete;
	Base &operator=( Base const & ) = delete;
	Base &operator=( Base && ) = delete;

	[[nodiscard]] virtual int type( ) const = 0;
	[[nodiscard]] virtual int value( ) const = 0;
};

struct Child0 : Base {
	int v;

	explicit Child0( int V ) noexcept
	  : v( V ) {}

	[[nodiscard]] int type( ) const override {
		return 0;
	}

	[[nodiscard]] int value( ) const override {
		return v;
	}
};

namespace daw::json {
	template<>
	struct json_data_contract<Child0> {
		static constexpr char const v[] = "v";
		using type = json_member_list<json_number<v, int>>;

		static constexpr auto to_json_data( Child0 const &c0 ) {
			return std::forward_as_tuple( c0.v );
		}
	};
} // namespace daw::json

struct Child1 : Base {
	int d;

	explicit Child1( int D ) noexcept
	  : d( D ) {}

	[[nodiscard]] int type( ) const override {
		return 1;
	}

	[[nodiscard]] int value( ) const override {
		return d;
	}
};

namespace daw::json {
	template<>
	struct json_data_contract<Child1> {
		static constexpr char const d[] = "d";
		using type = json_member_list<json_number<d, int>>;

		static constexpr auto to_json_data( Child1 const &c1 ) {
			return std::forward_as_tuple( c1.d );
		}
	};
} // namespace daw::json

struct Switcher {
	// Convert JSON tag member to type index
	constexpr size_t operator( )( int type ) const {
		return static_cast<std::size_t>( type );
	}
	// Get value for Tag from class value
	int operator( )( Base const &b ) const {
		return static_cast<int>( b.type( ) );
	}
};

struct Foo {
	std::unique_ptr<Base> value;
};

struct FooMaker {
	std::unique_ptr<Base> operator( )( char const *str, std::size_t sz ) const {
		using namespace daw::json;
		auto jv = json_value( daw::string_view( str, sz ) );
		(void)jv;
		switch( as<std::size_t>( jv["type"] ) ) {
		case 0:
			return from_json<std::unique_ptr<Child0>>( jv );
		case 1:
			return from_json<std::unique_ptr<Child1>>( jv );
		default:
			std::terminate( );
		}
	}
};

namespace daw::json {
	template<>
	struct json_data_contract<Foo> {
		static constexpr char const value[] = "value";
		using type =
		  json_member_list<json_raw<value, std::unique_ptr<Base>, FooMaker>>;
	};
} // namespace daw::json

constexpr std::string_view json_doc = R"json(
[
  {"value":{ "type": 0, "v": 42 }},
	{"value":{ "type": 1, "d": 66 }},
	{"value":{ "type": 0, "v": 77 }}
]
)json";

int main( ) {
	auto foo = daw::json::from_json<std::vector<Foo>>( json_doc );
	assert( foo[0].value->type( ) == 0 );
	assert( foo[0].value->value( ) == 42 );
	assert( foo[1].value->type( ) == 1 );
	assert( foo[1].value->value( ) == 66 );
	assert( foo[2].value->type( ) == 0 );
	assert( foo[2].value->value( ) == 77 );
}
