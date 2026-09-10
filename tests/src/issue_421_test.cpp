// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/daw_json_link
//

#include "defines.h"

#include <daw/json/daw_json_link.h>

#include <daw/daw_ensure.h>

#include <cassert>
#include <string>
#include <utility>
#include <variant>
#include <vector>

struct Variant {
	std::variant<int, bool, std::vector<Variant>> value;
};

template<>
struct daw::json::json_data_contract<Variant> {
	using type = json_type_alias<json_variant_no_name<
	  std::variant<int, bool, std::vector<Variant>>,
	  json_variant_type_list<
	    int, bool, json_array_no_name<json_recursive_class_no_name<Variant>>>>>;

	static auto to_json_data( Variant const &v ) {
		return v.value;
	}
};

int main( ) {
	{
		auto i = daw::json::from_json<Variant>( "5" );
		daw_ensure( i.value.index( ) == 0 );
	}
	{
		auto b0 = daw::json::from_json<Variant>( "false" );
		daw_ensure( b0.value.index( ) == 1 );
	}
	{
		auto b1 = daw::json::from_json<Variant>( "true" );
		daw_ensure( b1.value.index( ) == 1 );
	}
	{
		constexpr std::string_view json_doc = "[1,true,false,[1,false,[]]]";
		auto ary = daw::json::from_json<Variant>( json_doc );
		daw_ensure( ary.value.index( ) == 2 );

		auto d = daw::json::to_json( ary );
		daw_ensure( d == json_doc );
	}
}