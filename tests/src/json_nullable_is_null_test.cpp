// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/daw_json_link
//

#include "defines.h"

#include "daw/json/daw_json_link.h"

#include <daw/daw_benchmark.h>
#include <daw/daw_read_file.h>

#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <memory>
#include <optional>
#include <string>

struct Foo {
	std::variant<std::monostate, int, std::string> member;
};

struct CheckVariantNull {
	template<typename... Ts>
	constexpr bool
	operator( )( std::variant<std::monostate, Ts...> const &v ) const {
		return v.index( ) == 0;
	}
};

namespace daw::json {
	template<>
	struct json_data_contract<Foo> {
		static constexpr char member[] = "member";
		using type = json_member_list<json_nullable<
		  member, std::variant<std::monostate, int, std::string>,
		  json_variant_no_name<std::variant<std::monostate, int, std::string>>,
		  JsonNullable::NullVisible, daw::use_default, CheckVariantNull>>;

		static auto to_json_data( Foo const &f ) {
			return std::forward_as_tuple( f.member );
		}
	};
} // namespace daw::json

int main( )
#if defined( DAW_USE_EXCEPTIONS )
  try
#endif
{
	auto const foo0 = daw::json::from_json<Foo>( R"json({"member":null})json" );
	daw_ensure( foo0.member.index( ) == 0 );
	auto const s0 = daw::json::to_json( foo0 );
	daw_ensure( s0 == R"json({"member":null})json" );

	auto const foo1 =
	  daw::json::from_json<Foo>( R"json({"member":"Hello"})json" );
	daw_ensure( foo1.member.index( ) == 2 );
	auto const s1 = daw::json::to_json( foo1 );
	daw_ensure( s1 == R"json({"member":"Hello"})json" );
}
#if defined( DAW_USE_EXCEPTIONS )
catch( daw::json::json_exception const &jex ) {
	std::cerr << "Exception thrown by parser: " << jex.reason( ) << '\n';
	exit( 1 );
} catch( std::exception const &ex ) {
	std::cerr << "Unknown exception thrown during testing: " << ex.what( )
	          << '\n';
	exit( 1 );
} catch( ... ) {
	std::cerr << "Unknown exception thrown during testing\n";
	throw;
}
#endif