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
#include <vector>

#include <daw/daw_memory_mapped_file.h>

#include "daw/json/daw_json_link.h"

namespace daw::cookbook_array2 {
	struct MyClass4 {
		std::string a;
		unsigned b;
		float c;
		bool d;
	};

#if defined( __cpp_nontype_template_parameter_class )
	auto describe_json_class( MyClass4 const & ) {
		using namespace daw::json;
		return class_description_t<json_string<"a">, json_number<"b", unsigned>,
		                           json_number<"c", float>, json_bool<"d">>{};
	}
#else
	namespace symbols_MyClass4 {
		static constexpr char const a[] = "a";
		static constexpr char const b[] = "b";
		static constexpr char const c[] = "c";
		static constexpr char const d[] = "d";
	} // namespace symbols_MyClass4
	auto describe_json_class( MyClass4 const & ) {
		using namespace daw::json;
		return class_description_t<json_string<symbols_MyClass4::a>,
		                           json_number<symbols_MyClass4::b, unsigned>,
		                           json_number<symbols_MyClass4::c, float>,
		                           json_bool<symbols_MyClass4::d>>{};
	}
#endif
	auto to_json_data( MyClass4 const &value ) {
		return std::forward_as_tuple( value.a, value.b, value.c, value.d );
	}

	bool operator==( MyClass4 const &lhs, MyClass4 const &rhs ) {
		return to_json_data( lhs ) == to_json_data( rhs );
	}

} // namespace daw::cookbook_array2

int main( int argc, char **argv ) {
	if( argc <= 1 ) {
		puts( "Must supply path to cookbook_array2.json file\n" );
		exit( EXIT_FAILURE );
	}
	auto const data = daw::filesystem::memory_mapped_file_t<>( argv[1] );

	using namespace daw::json;

	auto const ve = from_json_array<daw::cookbook_array2::MyClass4>(
	  {data.data( ), data.size( )} );

	daw_json_assert( ve.size( ) == 2, "Expected 2 items" );
	auto const str = to_json_array( ve );
	puts( str.c_str( ) );
	auto const ve2 = from_json_array<daw::cookbook_array2::MyClass4>(
	  {str.data( ), str.size( )} );

	daw_json_assert( ve == ve2, "Expected 2 items" );
}
