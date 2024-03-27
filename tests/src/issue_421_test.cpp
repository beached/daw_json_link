// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/daw_json_link
//

#include "defines.h"

#include <daw/json/daw_json_link.h>

#include <cassert>
#include <string>
#include <utility>
#include <variant>
#include <vector>

struct Variant {
	std::variant<int, bool, std::vector<Variant>> value;
};

struct VariantCtor {
	Variant operator( )( char const *ptr, std::size_t sz ) const {
		auto value = daw::json::json_value( std::string_view( ptr, sz ) );
		return operator( )( value );
	}

	Variant operator( )( daw::json::json_value value ) const {
		using namespace daw::json;
		switch( value.type( ) ) {
		case JsonBaseParseTypes::Number:
			return Variant{ from_json<int>( value ) };
		case JsonBaseParseTypes::Bool:
			return Variant{ from_json<bool>( value ) };
		case JsonBaseParseTypes::Array: {
			auto res = std::vector<Variant>( );
			for( auto jp : value ) {
				res.push_back( operator( )( jp.value ) );
			}
			return Variant{ std::move( res ) };
		}
		default:
			std::abort( );
		}
	}
};

namespace daw::json {
	template<>
	struct json_data_contract<Variant> {
		using type = json_type_alias<json_raw_no_name<
		  std::variant<int, bool, std::vector<Variant>>, VariantCtor>>;

		static auto to_json_data( const Variant &value ) {
			return value.value;
		}
	};
} // namespace daw::json

int main( ) {
	{
		auto i = daw::json::from_json<Variant>( "5" );
		ensure( i.value.index( ) == 0 );
	}
	{
		auto b0 = daw::json::from_json<Variant>( "false" );
		ensure( b0.value.index( ) == 1 );
	}
	{
		auto b1 = daw::json::from_json<Variant>( "true" );
		ensure( b1.value.index( ) == 1 );
	}
	{
		constexpr std::string_view json_doc = "[1, true, false, [1, false, []]]";
		auto ary = daw::json::from_json<Variant>( json_doc );
		ensure( ary.value.index( ) == 2 );
	}
}