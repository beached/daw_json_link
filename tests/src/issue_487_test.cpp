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

template<typename Writer, typename T>
void test_write( Writer &writer, T const &value ) {
	writer.write_value( value );
}

int main( ) {
	{
		auto res = std::string{ };
		{
			auto writer = daw::json::json_writer( res );
			char const *test = "test";
			test_write( writer, test );
		}
		daw_ensure( res == R"json("test")json" );
	}
	{
		auto res = std::string{ };
		{
			auto writer = daw::json::json_writer( res );
			char const *test = "test";
			writer.write_value( test );
		}
		daw_ensure( res == R"json("test")json" );
	}
	{
		auto res = std::string{ };
		{
			auto writer = daw::json::json_writer( res );
			writer.write_value( "test" );
		}
		daw_ensure( res == R"json("test")json" );
	}
}
