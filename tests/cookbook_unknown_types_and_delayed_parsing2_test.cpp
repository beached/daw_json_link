// The MIT License (MIT)
//
// Copyright (c) Darrell Wright
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

#include <daw/daw_memory_mapped_file.h>

#include "daw/json/daw_json_link.h"

#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <string>

struct MyClass2 {
	int member0;
	daw::json::json_value member_later;
	std::string member1;
};

namespace daw::json {
	template<>
	struct json_data_contract<MyClass2> {
#if defined( __cpp_nontype_template_parameter_class )
		using type =
		  json_member_list<json_number<"member0", int>,
		                   json_delayed<"member_later">, json_string<"member1">>;
#else
		static constexpr char const member0[] = "member0";
		static constexpr char const member_later[] = "member_later";
		static constexpr char const member1[] = "member1";
		using type =
		  json_member_list<json_number<member0, int>, json_delayed<member_later>,
		                   json_string<member1>>;
#endif
		static inline auto to_json_data( MyClass2 const &value ) {
			return std::forward_as_tuple( value.member0, value.member_later,
			                              value.member1 );
		}
	};
} // namespace daw::json

// This isn't necessary to parse the values as but allows for direcly
// constructing the object
struct MyDelayedClass {
	int a;
	bool b;
};

namespace daw::json {
	template<>
	struct json_data_contract<MyDelayedClass> {
#if defined( __cpp_nontype_template_parameter_class )
		using type = json_member_list<json_number<"a", int>, json_bool<"b">>;
#else
		static constexpr char const a[] = "a";
		static constexpr char const b[] = "b";
		using type = json_member_list<json_number<a, int>, json_bool<b>>;
#endif

		static inline auto to_json_data( MyDelayedClass const &value ) {
			return std::forward_as_tuple( value.a, value.b );
		}
	};
} // namespace daw::json

int main( int argc, char **argv ) try {
	if( argc <= 1 ) {
		puts(
		  "Must supply path to cookbook_unknown_types_and_delayed_parsing2.json "
		  "file\n" );
		exit( EXIT_FAILURE );
	}
	auto data = daw::filesystem::memory_mapped_file_t<>( argv[1] );

	MyClass2 val = daw::json::from_json<MyClass2>(
	  std::string_view( data.data( ), data.size( ) ) );

	MyDelayedClass delayed_val =
	  daw::json::from_json<MyDelayedClass>( val.member_later );

	daw_json_assert( delayed_val.a == 1, "Unexpected value" );
} catch( daw::json::json_exception const &jex ) {
	std::cerr << "Exception thrown by parser: " << jex.reason( ) << std::endl;
	exit( 1 );
}
