#include <print>
#include <string>
#include <string_view>

#include <daw/json/daw_json_link.h>
#include <daw/json/daw_json_writer.h>

using namespace std::literals;

int main( ) {
	auto result = std::string{ };
	{
		auto writer =
		  daw::json::json_writer<daw::json::options::AllowEscapedNames::yes,
		                         daw::json::options::SerializationFormat::Pretty>(
		    result );
		writer.open_object( );
		writer.write_key_value( "na\"me", "A\"da" );
		writer.write_key_value( "na\"me"s, "A\"da"s );
		writer.write_key_value( "na\"me"sv, "A\"da"sv );
		writer.close_object( );
	}
	std::puts( result.c_str( ) );
}
