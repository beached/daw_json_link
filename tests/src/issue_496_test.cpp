// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/daw_json_link
//

#include <daw/json/daw_json_link.h>
#include <daw/json/daw_json_writer.h>

#include <iostream>
#include <string>
#include <string_view>

using namespace std::literals;

int main( ) {
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
	static constexpr daw::string_view expected_output =
	  R"json({"na\"me":"A\"da","na\"me":"A\"da","na\"me":"A\"da"})json";
	std::cout << "Expected: " << expected_output << '\n';
	std::cout << "  Result: " << result << '\n';
	daw_ensure( expected_output == result );
}
