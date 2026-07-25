// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/daw_json_link
//

#include <daw/json/daw_json_writer.h>

#include <iostream>
#include <string>

int main( ) {
	{
		auto out = std::string{ };
		{
			auto w = daw::json::json_writer( out );
			w.open_object( );
			w.write_key_value( "a", 42 );
			w.write_key_value( "b", { 1, 2, 3 } );
			w.write_key_value( "c", nullptr );
			daw_ensure( out == R"json({"a":42,"b":[1,2,3],"c":null)json" );
		}
		daw_ensure( out == R"json({"a":42,"b":[1,2,3],"c":null})json" );
	}
	{
		auto out = std::string{ };
		{
			auto w =
				daw::json::json_writer<daw::json::options::SerializationFormat::Pretty>(
					out );
			w.open_object( );
			w.write_key_value( "a", 42 );
			w.write_key_value( "b", { 1, 2, 3 } );
			w.write_key_value( "c", "Hello" );
		}
		daw_ensure( out == R"json({
  "a": 42,
  "b": [
    1,
    2,
    3
  ],
  "c": "Hello"
})json" );
	}
	{
		auto out = std::string{ };
		{
			auto w =
			  daw::json::json_writer<daw::json::options::SerializationFormat::Pretty>(
			    out );
			w.open_object( );
			w.write_key_value( "a", 42 );
			w.add_key( "b" );
			w.open_array( );
			w.write_value( 1 );
			w.write_value( 2 );
			w.write_value( 3 );
			w.close_array( );
			w.write_key_value( "c", "Hello" );
		}
		daw_ensure( out == R"json({
  "a": 42,
  "b": [
    1,
    2,
    3
  ],
  "c": "Hello"
})json" );
	}
}
