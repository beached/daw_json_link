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

#include "daw/json/daw_json_iterator.h"
#include "daw/json/daw_json_link.h"

#include <daw/daw_memory_mapped_file.h>

#include <daw/daw_benchmark.h>
#include <fstream>
#include <iostream>
#include <streambuf>
#include <string_view>
#include <vector>

struct coordinate_t {
	double x;
	double y;
	double z;
	std::string name;
};

struct coordinates_t {
	std::vector<coordinate_t> coordinates;
	std::string info;
};

namespace daw::json {
	template<>
	struct json_data_contract<coordinate_t> {
#ifdef __cpp_nontype_template_parameter_class
		using type =
		  json_member_list<json_number<"x">, json_number<"y">, json_number<"z">>;
#else
		static inline constexpr char const x[] = "x";
		static inline constexpr char const y[] = "y";
		static inline constexpr char const z[] = "z";
		using type =
		  json_member_list<json_number<x>, json_number<y>, json_number<z>>;
#endif
	};

	template<>
	struct json_data_contract<coordinates_t> {
#ifdef __cpp_nontype_template_parameter_class
		using type = json_member_list<json_array<"coordinates", coordinate_t>>;
#else
		static inline constexpr char const coordinates[] = "coordinates";
		using type = json_member_list<json_array<coordinates, coordinate_t>>;
#endif
	};
} // namespace daw::json

int main( int argc, char **argv ) try {
	using namespace daw::json;

	if( argc < 2 ) {
		std::cerr << "Must supply a filename to open\n";
		exit( 1 );
	}

	auto const json_data = daw::filesystem::memory_mapped_file_t<>( argv[1] );
	auto json_sv = std::string_view( json_data.data( ), json_data.size( ) );

	using iterator_t = daw::json::json_array_iterator<coordinate_t>;

	auto first = iterator_t( json_sv, "coordinates" );
	auto last = iterator_t( );

	double x = 0.0;
	double y = 0.0;
	double z = 0.0;
	size_t sz = 0U;
	while( first != last ) {
		auto c = *first;
		++sz;
		x += c.x;
		y += c.y;
		z += c.z;
		++first;
	}

	//	auto const sz = cls.coordinates.size( );
	auto const dsz = static_cast<double>( sz );
	std::cout << x / dsz << '\n';
	std::cout << y / dsz << '\n';
	std::cout << z / dsz << '\n';
} catch( daw::json::json_exception const &jex ) {
	std::cerr << "Exception thrown by parser: " << jex.reason( ) << std::endl;
	exit( 1 );
}
