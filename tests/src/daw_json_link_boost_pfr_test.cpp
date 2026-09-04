// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/daw_json_link
//

#include <daw/json/daw_json_boost_pfr_mapping.h>

#include <cassert>
#include <string>
#include <vector>

struct Person {
	int id;
	std::string name;
	std::vector<int> scores;

	bool operator==( Person const & ) const = default;
};

template<>
inline constexpr bool daw::json::use_boost_pfr<Person> = true;

int main( ) {
	auto const person = daw::json::from_json<Person>(
	  R"json({"id":42,"name":"Jane Smith","scores":[10,20,30]})json" );
	assert( person.id == 42 );
	assert( person.name == "Jane Smith" );
	assert( person.scores == std::vector<int>( { 10, 20, 30 } ) );

	auto const json = daw::json::to_json( person );
	assert(
	  json == R"json({"id":42,"name":"Jane Smith","scores":[10,20,30]})json" );
	assert( daw::json::from_json<Person>( json ) == person );
}
