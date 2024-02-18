// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/daw_json_link
//

#include <daw/json/daw_json_link.h>

#include <algorithm>
#include <cassert>
#include <string>
#include <variant>

namespace my {
	using Var2 = std::variant<int, float, std::string>;

	struct Var2Constructor {
		Var2 operator( )( ) const {
			return Var2{ };
		}

		Var2 operator( )( char const *ptr, std::size_t sz ) const {
			auto jv = daw::json::json_value( ptr, sz );
			switch( jv.type( ) ) {
			case daw::json::JsonBaseParseTypes::Number:
				if( auto sv = jv.get_string_view( );
				    std::find_if_not( sv.begin( ), sv.end( ), []( char c ) {
					    return '0' <= c and c <= '9';
				    } ) == sv.end( ) ) {
					// Only digits
					return daw::json::as<int>( jv );
				}
				return daw::json::as<float>( jv );
			case daw::json::JsonBaseParseTypes::Bool:
				return daw::json::as<bool>( jv );
			case daw::json::JsonBaseParseTypes::String:
				return daw::json::as<std::string>( jv );
			case daw::json::JsonBaseParseTypes::Array:
			case daw::json::JsonBaseParseTypes::Class:
			case daw::json::JsonBaseParseTypes::None:
			case daw::json::JsonBaseParseTypes::Null:
			default:
				std::abort( );
			}
		}
	};
} // namespace my

namespace daw::json {
	template<>
	struct json_data_contract<my::Var2> {
		using type =
		  json_type_alias<json_raw_no_name<my::Var2, my::Var2Constructor>>;
		static auto to_json_data( my::Var2 const &value ) {
			return std::visit(
			  []( auto const &alt ) {
				  return to_json( alt );
			  },
			  value );
		}
	};

} // namespace daw::json

int main( ) {
	using namespace daw::json;

	{
		auto kx = from_json<my::Var2>( R"(67)" );
		assert( std::get<int>( kx ) == 67 );
	}
	{
		auto kx = from_json<my::Var2>( R"(67.8)" );
		assert( std::get<float>( kx ) == 67.8F );
	}
	{
		auto kx = from_json<my::Var2>( R"("test")" );
		assert( std::get<std::string>( kx ) == "test" );
	}

	my::Var2 v = 1;
	auto x = to_json( v );
	assert( x == "1" );
}
