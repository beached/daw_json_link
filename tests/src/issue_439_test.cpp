// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/daw_json_link/
//

#include <daw/json/daw_json_link.h>

#include <daw/daw_ensure.h>

#include <string>
#include <vector>

struct TestClass {
	int i;
	double d;
	bool b;
	std::string s{ };
	std::vector<int> y{ };

	bool operator==( TestClass const &rhs ) const {
		return std::tie( i, d, b, s, y ) ==
		       std::tie( rhs.i, rhs.d, rhs.b, rhs.s, rhs.y );
	}
};

namespace daw::json {
	template<>
	struct json_data_contract<TestClass> {
		static constexpr char const mem_i[] = "i";
		static constexpr char const mem_d[] = "d";
		static constexpr char const mem_b[] = "b";
		static constexpr char const mem_s[] = "s";
		static constexpr char const mem_y[] = "y";
		using type = json_member_list<json_number<mem_i, int>, json_number<mem_d>,
		                              json_bool<mem_b>, json_string<mem_s>,
		                              json_array<mem_y, int>>;

		static inline auto to_json_data( TestClass const &value ) {
			return std::forward_as_tuple( value.i, value.d, value.b, value.s,
			                              value.y );
		}
	};
} // namespace daw::json

int main( ) {
	static constexpr std::string_view json_doc1 = R"([
	{
		"i":5,
		"d":0.002,
		"b":false,
		"s":"hello world",
		"y":[1,2,3,4] 
    },
	{
		"i":4,
		"d":1.000005,
		"b":true,
		"s":"goodbye world",
		"y":[4,3,1,4] 
    }])";

	static constexpr std::string_view json_doc2 = R"json(
	{
		"i":4,
		"d":1.000005,
		"b":true,
		"s":"goodbye world",
		"y":[4,3,1,4]
	}
 )json";

	auto const flttest0 = daw::json::to_json( 1.000005f );
	daw_ensure( flttest0 == "1.000005" );

	auto const flttest1 = daw::json::to_json( 1.05f );
	daw_ensure( flttest1 == "1.05" );

	auto const flttest2 = daw::json::to_json( 1.55f );
	daw_ensure( flttest2 == "1.55" );

	auto const flttest3 = daw::json::to_json( 21.05f );
	daw_ensure( flttest3 == "21.05" );
	auto obj = daw::json::from_json<TestClass>( json_doc2 );
	daw_ensure( obj.d == 1.000005 );
	auto obj_json = daw::json::to_json( obj );
	daw_ensure( obj_json[0] == '{' );

	auto arry_of_test_class = daw::json::from_json_array<TestClass>( json_doc1 );

	daw_ensure( arry_of_test_class[1].d == 1.000005 );
	auto json_array = daw::json::to_json( arry_of_test_class );

	auto arry_of_test_class2 =
	  daw::json::from_json_array<TestClass>( json_array );

	daw_ensure( arry_of_test_class2[1].d == 1.000005 );

	daw_ensure( arry_of_test_class == arry_of_test_class2 );
}
