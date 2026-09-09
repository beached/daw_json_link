// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/daw_json_link
//
//  Covers options::JsonCustomTypes::String (the json_custom default) and
//  ::Literal, mirroring the examples in docs/cookbook/custom_types.md.  Only
//  ::Any had a test (custom_optional_number_test.cpp) prior to this.
//

#include <daw/json/daw_json_link.h>

#include <daw/daw_ensure.h>

#include <string>
#include <string_view>

struct Identifier {
	int value;
};

struct IdentifierFromJson {
	Identifier operator( )( std::string_view sv ) const {
		return Identifier{ std::stoi( std::string( sv ) ) };
	}
};

struct IdentifierToJson {
	std::string operator( )( Identifier const &id ) const {
		return std::to_string( id.value );
	}
};

int main( ) {
	using namespace daw::json;

	// JsonCustomTypes::String (the default): the member is a JSON string, and
	// the converter sees/produces the contents without quotes.
	{
		using identifier_json =
		  json_custom_no_name<Identifier, IdentifierFromJson, IdentifierToJson>;
		auto const id = from_json<identifier_json>( R"("42")" );
		daw_ensure( id.value == 42 );
		daw_ensure( to_json<identifier_json>( id ) == R"("42")" );
	}
	// Same, with the option spelled out explicitly.
	{
		using identifier_json = json_custom_no_name<
		  Identifier, IdentifierFromJson, IdentifierToJson,
		  options::json_custom_opt( options::JsonCustomTypes::String )>;
		auto const id = from_json<identifier_json>( R"("42")" );
		daw_ensure( id.value == 42 );
		daw_ensure( to_json<identifier_json>( id ) == R"("42")" );
	}

	// JsonCustomTypes::Literal: the member is an unquoted JSON literal
	// (number/bool/null), and the converter sees/produces it without quotes.
	{
		using identifier_literal_json =
		  json_custom_lit_no_name<Identifier, IdentifierFromJson, IdentifierToJson>;
		auto const id = from_json<identifier_literal_json>( "42" );
		daw_ensure( id.value == 42 );
		daw_ensure( to_json<identifier_literal_json>( id ) == "42" );
	}
	// Same, with the option spelled out explicitly.
	{
		using identifier_literal_json = json_custom_no_name<
		  Identifier, IdentifierFromJson, IdentifierToJson,
		  options::json_custom_opt( options::JsonCustomTypes::Literal )>;
		auto const id = from_json<identifier_literal_json>( "42" );
		daw_ensure( id.value == 42 );
		daw_ensure( to_json<identifier_literal_json>( id ) == "42" );
	}
}
