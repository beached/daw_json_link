
#include <daw/json/daw_json_link.h>

#include <cassert>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <string>

namespace daw::cookbook_class1 {
	struct MyClass1 {
		std::string member_0;
		int member_1;
		bool member_2;
	};
} // namespace daw::cookbook_class1

namespace daw::json {
	template<>
	struct json_data_contract<daw::cookbook_class1::MyClass1> {
		static constexpr char const member0[] = "member0";
		static constexpr char const member1[] = "member1";
		static constexpr char const member2[] = "member2";
		using type =
		  json_member_list<json_string<member0>, json_number<member1, int>,
		                   json_bool<member2>>;
		static inline auto
		to_json_data( daw::cookbook_class1::MyClass1 const &value ) {
			return std::forward_as_tuple( value.member_0, value.member_1,
			                              value.member_2 );
		}
	};
} // namespace daw::json

int main( ) {
	std::string_view data = R"( 
{
  "member0": "this is a test",
  "member1": 314159,
  "member2": true
}
	)";	
	auto const cls = daw::json::from_json<daw::cookbook_class1::MyClass1>(
	  std::string_view( data.data( ), data.size( ) ) );

	assert( cls.member_0 == "this is a test" );
	assert( cls.member_1 == 314159 );
	assert( cls.member_2 == true );
	std::string const str = daw::json::to_json( cls );
	puts( str.c_str( ) );

	auto const cls2 = daw::json::from_json<daw::cookbook_class1::MyClass1>(
	  std::string_view( str.data( ), str.size( ) ) );
	(void)cls2;
	puts( "All good" );
}

