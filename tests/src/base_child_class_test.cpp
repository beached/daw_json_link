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
	[[nodiscard]] virtual std::string to_json( ) const = 0;
};

struct Child0 : Base {
	int t;
	int v;

	explicit Child0( int Type, int V ) noexcept
	  : t( Type )
	  , v( V ) {}

	[[nodiscard]] int type( ) const override {
		return t;
	}

	[[nodiscard]] int value( ) const override {
		return v;
	}

	[[nodiscard]] std::string to_json( ) const override;
};

namespace daw::json {
	template<>
	struct json_data_contract<Child0> {
		static constexpr char const type_mem[] = "type";
		static constexpr char const v[] = "v";
		using type =
		  json_member_list<json_number<type_mem, int>, json_number<v, int>>;

		static constexpr auto to_json_data( Child0 const &c0 ) {
			return std::forward_as_tuple( c0.t, c0.v );
		}
	};
} // namespace daw::json

[[nodiscard]] std::string Child0::to_json( ) const {
	return daw::json::to_json( *this );
}

struct Child1 : Base {
	int t;
	int d;

	explicit Child1( int Type, int D ) noexcept
	  : t( Type )
	  , d( D ) {}

	[[nodiscard]] int type( ) const override {
		return t;
	}

	[[nodiscard]] int value( ) const override {
		return d;
	}

	[[nodiscard]] std::string to_json( ) const override;
};

namespace daw::json {
	template<>
	struct json_data_contract<Child1> {
		static constexpr char const d[] = "d";
		static constexpr char const type_mem[] = "type";
		using type =
		  json_member_list<json_number<type_mem, int>, json_number<d, int>>;

		static constexpr auto to_json_data( Child1 const &c1 ) {
			return std::forward_as_tuple( c1.t, c1.d );
		}
	};
} // namespace daw::json

[[nodiscard]] std::string Child1::to_json( ) const {
	return daw::json::to_json( *this );
}

struct Foo {
	std::unique_ptr<Base> value;

	[[nodiscard]] bool operator==( Foo const &rhs ) const noexcept {
		return value->type( ) == rhs.value->type( ) and
		       value->value( ) == rhs.value->value( );
	}
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

		[[nodiscard]] static auto to_json_data( Foo const &foo ) {
			return std::tuple{ foo.value->to_json( ) };
		}
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
	ensure( foo[0].value->type( ) == 0 );
	ensure( foo[0].value->value( ) == 42 );
	ensure( foo[1].value->type( ) == 1 );
	ensure( foo[1].value->value( ) == 66 );
	ensure( foo[2].value->type( ) == 0 );
	ensure( foo[2].value->value( ) == 77 );

	auto str = daw::json::to_json( foo );
	std::cout << str << '\n';
	auto foo2 = daw::json::from_json<std::vector<Foo>>( str );
	ensure( foo == foo2 );
}
