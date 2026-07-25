// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/daw_json_link
//

#include "daw/json/daw_json_switches.h"

#include <daw/json/daw_json_writer.h>

#include <iostream>
#include <string>

struct Foo {
	int a = 0;
	std::string b = "";
	std::vector<int> c = { };
};

namespace daw::json {
	template<>
	struct json_data_contract<Foo> {
		static constexpr char const a[] = "a";
		static constexpr char const b[] = "b";
		static constexpr char const c[] = "c";
		using type =
		  json_member_list<json_number<a, int>, json_string<b>, json_array<c, int>>;

		static DAW_JSON_CX_STRVEC auto to_json_data( Foo const &f ) {
			return std::forward_as_tuple( f.a, f.b, f.c );
		}
	};
} // namespace daw::json

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
			auto w = daw::json::json_writer( out );
			w.open_object( );
			w.close_object( );
		}
		daw_ensure( out == R"json({})json" );
	}
	{
		auto out = std::string{ };
		{
			auto w =
			  daw::json::json_writer<daw::json::options::SerializationFormat::Pretty>(
			    out );
			w.open_object( );
			w.close_object( );
		}
		daw_ensure( out == R"json({})json" );
	}
	{
		auto out = std::string{ };
		{
			auto w =
			  daw::json::json_writer<daw::json::options::SerializationFormat::Pretty>(
			    out );
			w.open_array( );
			w.close_array( );
		}
		daw_ensure( out == R"json([])json" );
	}
	{
		auto out = std::string{ };
		{
			auto w = daw::json::json_writer( out );
			w.open_array( );
			w.close_array( );
		}
		daw_ensure( out == R"json([])json" );
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
			w.add_key( "c" );
			w.add_key( "d" );
		}
		daw_ensure( out == R"json({
  "a": 42,
  "b": [
    1,
    2,
    3
  ],
  "c": null,
  "d": null
})json" );
	}
	{
		auto out = std::string{ };
		{
			auto w = daw::json::json_writer( out );
			w.write_value( 42 );
		}
		daw_ensure( out == "42" );
	}
	{
		auto out = std::string{ };
		{
			auto w = daw::json::json_writer( out );
			w.open_object( );
			w.add_key( "a" );
			w.open_array( );
			for( int n = 1; n <= 3; ++n ) {
				w.write_value( n * 2 );
			}
			w.write_values( { 1, 2, 3 } );
			w.close_array( );
			w.close_object( );
		}
		daw_ensure( out == R"json({"a":[2,4,6,1,2,3]})json" );
	}
	{
		auto out = std::string{ };
		{
			auto w = daw::json::json_writer( out );
			w.open_object( );
			w.add_key( "a" );
			w.open_array( );
			w.write_values( 1, 2, "3", 4 );
			w.write_value( "5" );
			w.write_values( { 6, 7 } );
			w.close_array( );
			w.close_object( );
		}
		daw_ensure( out == R"json({"a":[1,2,"3",4,"5",6,7]})json" );
	}
	{
		auto out = std::string{ };
		{
			auto w = daw::json::json_writer( out );
			w.write_value( Foo{ } );
		}
		daw_ensure( out == R"json({"a":0,"b":"","c":[]})json" );
	}
	{
		auto out = std::string{ };
		{
			auto w = daw::json::json_writer( out );
			w.open_object( );
			w.write_key_value( "a", Foo{ 42, "Hello", { 1, 2, 3 } } );
		}
		daw_ensure( out == R"json({"a":{"a":42,"b":"Hello","c":[1,2,3]}})json" );
	}
}
