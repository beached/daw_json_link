// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/daw_json_link
//

#include "defines.h"

#include <daw/json/daw_json_query.h>

#include <iterator>
#include <string_view>
#include <vector>

int main( ) {
	constexpr std::string_view json = R"json(
{
	"a": 5,
	"b": [1,2,3,4,5],
	"c": "Hello World",
	"d": "Goodbye"
	"e": { "a": "Hey Hey" }
}
)json";

	{
		/// Extract all array elements that are numbers
		auto results = std::vector<double>{ };
		(void)daw::json::filter<double>(
		  daw::json::json_value( json ), std::back_inserter( results ),
		  []( auto const &jp ) {
			  return not jp.name and jp.value.is_number( );
		  } );
		auto const expected_vec = std::vector{ 1.0, 2.0, 3.0, 4.0, 5.0 };
		ensure( results == expected_vec );
	}

	{
		auto results = std::vector<std::string>{ };
		(void)daw::json::filter<std::string>( daw::json::json_value( json ),
		                                      std::back_inserter( results ),
		                                      []( auto const &jp ) {
			                                      return jp.value.is_string( );
		                                      } );
		auto const expected_vec =
		  std::vector<std::string>{ "Hello World", "Goodbye", "Hey Hey" };
		ensure( results == expected_vec );
	}
}
