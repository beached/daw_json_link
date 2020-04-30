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
#include <unordered_map>

namespace daw::cookbook_class_from_array1 {
	struct Point {
		double x;
		double y;
	};

	bool operator!=( Point const &lhs, Point const &rhs ) {
		return lhs.x != rhs.x and lhs.y != rhs.y;
	}
} // namespace daw::cookbook_class_from_array1

namespace daw::json {
	template<>
	struct json_data_contract<daw::cookbook_class_from_array1::Point> {
		using type = json_ordered_member_list<double, double>;

		static inline auto
		to_json_data( daw::cookbook_class_from_array1::Point const &p ) {
			return std::forward_as_tuple( p.x, p.y );
		}
	};
} // namespace daw::json

int main( int argc, char **argv ) try {
	if( argc <= 1 ) {
		puts( "Must supply path to cookbook_class_from_array1.json file\n" );
		exit( EXIT_FAILURE );
	}
	auto data = daw::filesystem::memory_mapped_file_t<>( argv[1] );
	puts( "Original\n" );
	puts( data.data( ) );

	daw::cookbook_class_from_array1::Point const cls =
	  daw::json::from_json<daw::cookbook_class_from_array1::Point>(
	    std::string_view( data.data( ), data.size( ) ) );

	std::string const str = daw::json::to_json( cls );

	puts( "Round trip\n" );
	puts( str.c_str( ) );
	daw::cookbook_class_from_array1::Point const cls2 =
	  daw::json::from_json<daw::cookbook_class_from_array1::Point>(
	    std::string_view( str.data( ), str.size( ) ) );

	if( cls != cls2 ) {
		puts( "not exact same\n" );
	}
} catch( daw::json::json_exception const &jex ) {
	std::cerr << "Exception thrown by parser: " << jex.reason( ) << std::endl;
	exit( 1 );
}
