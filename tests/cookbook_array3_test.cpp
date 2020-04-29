// The MIT License (MIT)
//
// Copyright (c) 2019-2020 Darrell Wright
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files( the "Software" ), to
// deal in the Software without restriction, including without limitation the
// rights to use, copy, modify, merge, publish, distribute, sublicense, and / or
// sell copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#include "daw/json/daw_json_link.h"

#include <daw/daw_memory_mapped_file.h>

#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <string>
#include <vector>

namespace daw::cookbook_array3 {
	struct MyArrayClass1 {
		int member0;
		std::vector<int> member1;
		std::vector<std::string> member2;
	};

	bool operator==( MyArrayClass1 const &lhs, MyArrayClass1 const &rhs ) {
		return std::tie( lhs.member0, lhs.member1, lhs.member2 ) ==
		       std::tie( rhs.member0, rhs.member1, rhs.member2 );
	}
} // namespace daw::cookbook_array3

namespace daw::json {
	template<>
	struct json_data_contract<daw::cookbook_array3::MyArrayClass1> {
#ifdef __cpp_nontype_template_parameter_class
		using type =
		  json_member_list<json_number<"member0", int>, json_array<"member1", int>,
		                   json_array<"member2", std::string>>;
#else
		static constexpr char const member0[] = "member0";
		static constexpr char const member1[] = "member1";
		static constexpr char const member2[] = "member2";
		using type =
		  json_member_list<json_number<member0, int>, json_array<member1, int>,
		                   json_array<member2, std::string>>;
#endif
		static inline auto
		to_json_data( daw::cookbook_array3::MyArrayClass1 const &value ) {
			return std::forward_as_tuple( value.member0, value.member1,
			                              value.member2 );
		}
	};
} // namespace daw::json

int main( int argc, char **argv ) try {
	if( argc <= 1 ) {
		puts( "Must supply path to cookbook_array3.json file\n" );
		exit( EXIT_FAILURE );
	}
	auto const data = daw::filesystem::memory_mapped_file_t<>( argv[1] );

	using namespace daw::json;

	auto const my_array_class = from_json<daw::cookbook_array3::MyArrayClass1>(
	  {data.data( ), data.size( )} );

	daw_json_assert( my_array_class.member1.size( ) == 5, "Expected 5 items" );
	daw_json_assert( my_array_class.member2.size( ) == 2, "Expected 2 items" );
	auto const str = to_json( my_array_class );
	puts( str.c_str( ) );
	auto const my_array_class2 = from_json<daw::cookbook_array3::MyArrayClass1>(
	  {str.data( ), str.size( )} );

	daw_json_assert( my_array_class == my_array_class2, "Round trip failed" );
} catch( daw::json::json_exception const &jex ) {
	std::cerr << "Exception thrown by parser: " << jex.reason( ) << std::endl;
	exit( 1 );
}
