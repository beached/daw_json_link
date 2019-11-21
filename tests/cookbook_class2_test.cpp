// The MIT License (MIT)
//
// Copyright (c) 2019 Darrell Wright
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

#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <string>
#include <unordered_map>

#include <daw/daw_memory_mapped_file.h>

#include "daw/json/daw_json_link.h"

namespace daw::cookbook_class2 {
	struct MyClass1 {
		std::string member_0;
		int member_1;
		bool member_2;
	};

#if defined( __cpp_nontype_template_parameter_class )
	auto describe_json_class( MyClass1 const & ) {
		using namespace daw::json;
		return class_description_t<json_string_raw<"member0">,
		                           json_number<"member1", int>,
		                           json_bool<"member2">>{};
	}
#else
	namespace symbols_MyClass1 {
		static constexpr char const member0[] = "member0";
		static constexpr char const member1[] = "member1";
		static constexpr char const member2[] = "member2";
	} // namespace symbols_MyClass1
	auto describe_json_class( MyClass1 const & ) {
		using namespace daw::json;
		return class_description_t<json_string_raw<symbols_MyClass1::member0>,
		                           json_number<symbols_MyClass1::member1, int>,
		                           json_bool<symbols_MyClass1::member2>>{};
	}
#endif
	auto to_json_data( MyClass1 const &value ) {
		return std::forward_as_tuple( value.member_0, value.member_1,
		                              value.member_2 );
	}

	bool operator==( MyClass1 const &lhs, MyClass1 const &rhs ) {
		return to_json_data( lhs ) == to_json_data( rhs );
	}

	struct MyClass2 {
		MyClass1 a;
		unsigned b;
	};

#if defined( __cpp_nontype_template_parameter_class )
	auto describe_json_class( MyClass2 const & ) {
		using namespace daw::json;
		return class_description_t<json_class<"a", MyClass1>,
		                           json_number<"b", unsigned>>{};
	}
#else
	namespace symbols_MyClass2 {
		static constexpr char const a[] = "a";
		static constexpr char const b[] = "b";
	} // namespace symbols_MyClass2
	auto describe_json_class( MyClass2 const & ) {
		using namespace daw::json;
		return class_description_t<json_class<symbols_MyClass2::a, MyClass1>,
		                           json_number<symbols_MyClass2::b, unsigned>>{};
	}
#endif
	auto to_json_data( MyClass2 const &value ) {
		return std::forward_as_tuple( value.a, value.b );
	}

	bool operator==( MyClass2 const &lhs, MyClass2 const &rhs ) {
		return to_json_data( lhs ) == to_json_data( rhs );
	}
} // namespace daw::cookbook_class2

int main( int argc, char **argv ) {
	if( argc <= 1 ) {
		puts( "Must supply path to cookbook_class2.json file\n" );
		exit( EXIT_FAILURE );
	}
	auto data = daw::filesystem::memory_mapped_file_t<>( argv[1] );

	auto const cls = daw::json::from_json<daw::cookbook_class2::MyClass2>(
	  std::string_view( data.data( ), data.size( ) ) );

	daw::json::json_assert( cls.a.member_0 == "this is a test",
	                        "Unexpected value" );
	daw::json::json_assert( cls.a.member_1 == 314159, "Unexpected value" );
	daw::json::json_assert( cls.a.member_2 == true, "Unexpected value" );
	daw::json::json_assert( cls.b == 1234, "Unexpected value" );
	auto const str = daw::json::to_json( cls );
	puts( str.c_str( ) );

	auto const cls2 = daw::json::from_json<daw::cookbook_class2::MyClass2>(
	  std::string_view( str.data( ), str.size( ) ) );

	daw::json::json_assert( cls == cls2, "Unexpected round trip error" );
}
