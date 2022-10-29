// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/daw_json_link
//

#include <daw/json/daw_json_link.h>

#include <cassert>
#include <iostream>
#include <optional>
#include <string_view>
#include <tuple>

struct Foo {
	double a;
	double b;
	bool c;
	bool d;
	int e;
	int f;
	unsigned g;
	unsigned h;
};

namespace daw::json {
	template<>
	struct json_data_contract<Foo> {
		static constexpr char const a[] = "a";
		static constexpr char const b[] = "b";
		static constexpr char const c[] = "c";
		static constexpr char const d[] = "d";
		static constexpr char const e[] = "e";
		static constexpr char const f[] = "f";
		static constexpr char const g[] = "g";
		static constexpr char const h[] = "h";
		using type = json_member_list<
		  json_number<a, double,
		              options::number_opt( options::LiteralAsStringOpt::Always )>,
		  json_number<b, double,
		              options::number_opt( options::LiteralAsStringOpt::Always )>,
		  json_bool<c, bool,
		            options::bool_opt( options::LiteralAsStringOpt::Always )>,
		  json_bool<d, bool,
		            options::bool_opt( options::LiteralAsStringOpt::Always )>,
		  json_number<e, int,
		              options::number_opt( options::LiteralAsStringOpt::Always )>,
		  json_number<f, int,
		              options::number_opt( options::LiteralAsStringOpt::Always )>,
		  json_number<g, unsigned,
		              options::number_opt( options::LiteralAsStringOpt::Always )>,
		  json_number<h, unsigned,
		              options::number_opt( options::LiteralAsStringOpt::Always )>>;

		static constexpr auto to_json_data( Foo const &foo ) {
			return std::forward_as_tuple( foo.a, foo.b, foo.c, foo.d, foo.e, foo.f,
			                              foo.g, foo.h );
		}
	};
} // namespace daw::json

int main( ) {
	{
		std::string_view json_doc =
		  R"json({"b":"1.23456","a":"6.54321","d":"false","c":"true","f":"-123","e":"-321","h":"123","g":"321"})json";
		auto const f = daw::json::from_json<Foo>( json_doc );
		assert( f.a >= 6.54320 and f.a <= 6.54322 );
		assert( f.b >= 1.23455 and f.b <= 1.23457 );
		assert( f.c );
		assert( not f.d );
		assert( f.e == -321 );
		assert( f.f == -123 );
		assert( f.g == 321 );
		assert( f.h == 123 );
		std::cout << daw::json::to_json( f ) << '\n';
	}
}
