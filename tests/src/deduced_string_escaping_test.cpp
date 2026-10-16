// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/daw_json_link
//
// When the JSON type of a string-like value is deduced rather than declared
// in a json_data_contract, the value has not come from a parse and must be
// escaped on output.

#include "defines.h"

#include <daw/json/daw_json_link.h>
#include <daw/json/daw_json_writer.h>

#include <map>
#include <optional>
#include <string>
#include <string_view>
#include <vector>

using namespace std::literals;

namespace {
	void check( std::string_view test_name, std::string_view expected,
	            std::string_view result ) {
		if( expected != result ) {
			daw_ensure_error( true,
			                  std::string( test_name ) +
			                    "\n  Expected: " + std::string( expected ) +
			                    "\n    Result: " + std::string( result ) );
		}
	}

	void to_json_top_level( ) {
		check( "to_json string literal",
		       R"("Hello\"")",
		       daw::json::to_json( "Hello\"" ) );

		char const *cptr = "Hello\"";
		check( "to_json char const *", R"("Hello\"")", daw::json::to_json( cptr ) );

		char const *null_cptr = nullptr;
		check(
		  "to_json null char const *", "null", daw::json::to_json( null_cptr ) );

		char buff[] = "Hello\"";
		char *mptr = buff;
		check( "to_json char *", R"("Hello\"")", daw::json::to_json( mptr ) );
		check( "to_json char[N]", R"("Hello\"")", daw::json::to_json( buff ) );

		check(
		  "to_json std::string", R"("Hello\"")", daw::json::to_json( "Hello\""s ) );
		check( "to_json std::string_view",
		       R"("Hello\"")",
		       daw::json::to_json( "Hello\""sv ) );
		check( "to_json daw::string_view",
		       R"("Hello\"")",
		       daw::json::to_json( daw::string_view( "Hello\"" ) ) );

		check( "to_json control characters",
		       R"("a\nb\tc\\d")",
		       daw::json::to_json( "a\nb\tc\\d"sv ) );
	}

	void to_json_nested( ) {
		check( "to_json vector<string_view>",
		       R"(["A\"da","B\\b"])",
		       daw::json::to_json(
		         std::vector<std::string_view>{ "A\"da"sv, "B\\b"sv } ) );

		check(
		  "to_json vector<char const *>",
		  R"(["A\"da",null])",
		  daw::json::to_json( std::vector<char const *>{ "A\"da", nullptr } ) );

		check( "to_json optional<string_view>",
		       R"("A\"da")",
		       daw::json::to_json( std::optional<std::string_view>( "A\"da" ) ) );

		check( "to_json map<string, string_view>",
		       R"({"na\"me":"A\"da"})",
		       daw::json::to_json( std::map<std::string, std::string_view>{
		         { "na\"me", "A\"da" } } ) );
	}

	void json_writer_values( ) {
		auto result = std::string{ };
		{
			auto writer =
			  daw::json::json_writer<daw::json::options::AllowEscapedNames::yes>(
			    result );
			writer.open_array( );
			writer.write_value( "A\"da" );
			char const *cptr = "A\"da";
			writer.write_value( cptr );
			char const *null_cptr = nullptr;
			writer.write_value( null_cptr );
			writer.write_value( "A\"da"s );
			writer.write_value( "A\"da"sv );
			writer.write_value( daw::string_view( "A\"da" ) );
			writer.write_value( std::vector<std::string_view>{ "A\"da"sv } );
			writer.close_array( );
		}
		check( "json_writer write_value",
		       R"(["A\"da","A\"da",null,"A\"da","A\"da","A\"da",["A\"da"]])",
		       result );
	}

	void json_writer_key_values( ) {
		auto result = std::string{ };
		{
			auto writer =
			  daw::json::json_writer<daw::json::options::AllowEscapedNames::yes>(
			    result );
			writer.open_object( );
			writer.write_key_value( "na\"me", "A\"da" );
			writer.write_key_value( "na\"me"s, "A\"da"s );
			writer.write_key_value( "na\"me"sv, "A\"da"sv );
			writer.close_object( );
		}
		check( "json_writer write_key_value",
		       R"({"na\"me":"A\"da","na\"me":"A\"da","na\"me":"A\"da"})",
		       result );
	}
} // namespace

// Inside a contract a string_view member is a raw string: the value is
// expected to already be JSON escaped, as it would be after parsing.
struct RawStringMember {
	std::string_view value;
};

namespace daw::json {
	template<>
	struct json_data_contract<RawStringMember> {
		static constexpr char const value[] = "value";
		using type = json_member_list<json_link<value, std::string_view>>;

		static constexpr auto to_json_data( RawStringMember const &v ) {
			return std::forward_as_tuple( v.value );
		}
	};
} // namespace daw::json

namespace {
	void contract_string_view_is_raw( ) {
		constexpr std::string_view json_doc = R"({"value":"A\"da"})";
		auto const parsed = daw::json::from_json<RawStringMember>( json_doc );
		check( "contract string_view parse is raw", R"(A\"da)", parsed.value );
		check( "contract string_view round trip",
		       json_doc,
		       daw::json::to_json( parsed ) );
	}
} // namespace

int main( ) {
	to_json_top_level( );
	to_json_nested( );
	json_writer_values( );
	json_writer_key_values( );
	contract_string_view_is_raw( );
}
