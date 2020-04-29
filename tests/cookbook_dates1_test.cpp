// The MIT License (MIT)
//
// Copyright (c) 2020 Darrell Wright
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

#include <chrono>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <string>

namespace daw::cookbook_dates1 {
	struct MyClass1 {
		std::string name;
		std::chrono::time_point<std::chrono::system_clock,
		                        std::chrono::milliseconds>
		  timestamp;
	};

	bool operator==( MyClass1 const &lhs, MyClass1 const &rhs ) {
		return std::tie( lhs.name, lhs.timestamp ) ==
		       std::tie( rhs.name, rhs.timestamp );
	}
} // namespace daw::cookbook_dates1

namespace daw::json {
	template<>
	struct json_data_contract<daw::cookbook_dates1::MyClass1> {
#if defined( __cpp_nontype_template_parameter_class )
		using type = json_member_list<json_string<"name">, json_date<"timestamp">>;
#else
		static inline constexpr char const name[] = "name";
		static inline constexpr char const timestamp[] = "timestamp";
		using type = json_member_list<json_string<name>, json_date<timestamp>>;
#endif
		static inline auto
		to_json_data( daw::cookbook_dates1::MyClass1 const &value ) {
			return std::forward_as_tuple( value.name, value.timestamp );
		}
	};
} // namespace daw::json

int main( int argc, char **argv ) try {
	if( argc <= 1 ) {
		puts( "Must supply path to cookbook_dates1.json file\n" );
		exit( EXIT_FAILURE );
	}
	auto data = daw::filesystem::memory_mapped_file_t<>( argv[1] );

	daw::cookbook_dates1::MyClass1 const cls =
	  daw::json::from_json<daw::cookbook_dates1::MyClass1>(
	    std::string_view( data.data( ), data.size( ) ) );

	daw_json_assert( cls.name == "Toronto", "Unexpected value" );

	std::string const str = daw::json::to_json( cls );
	puts( str.c_str( ) );

	daw::cookbook_dates1::MyClass1 const cls2 =
	  daw::json::from_json<daw::cookbook_dates1::MyClass1>( str );

	daw_json_assert( cls == cls2, "Unexpected round trip error" );
} catch( daw::json::json_exception const &jex ) {
	std::cerr << "Exception thrown by parser: " << jex.reason( ) << std::endl;
	exit( 1 );
}
