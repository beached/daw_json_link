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

#include "daw/json/daw_json_iterator.h"
#include "daw/json/daw_json_link.h"

#include <daw/daw_benchmark.h>
#include <daw/daw_memory_mapped_file.h>

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

namespace symbols_coordinate_t {
	constexpr static char const x[] = "x";
	constexpr static char const y[] = "y";
	constexpr static char const z[] = "z";
} // namespace symbols_coordinate_t
auto describe_json_class( coordinate_t ) noexcept {
	using namespace daw::json;
#ifdef __cpp_nontype_template_parameter_class
	return class_description_t<json_number<"x">, json_number<"y">,
	                           json_number<"z">>{};
#else
	return class_description_t<json_number<symbols_coordinate_t::x>,
	                           json_number<symbols_coordinate_t::y>,
	                           json_number<symbols_coordinate_t::z>>{};
#endif
}

namespace symbols_coordinates_t {
	constexpr static char const coordinates[] = "coordinates";
}
struct coordinates_t {
	std::vector<coordinate_t> coordinates;
	std::string info;
};
auto describe_json_class( coordinates_t ) noexcept {
	using namespace daw::json;
#ifdef __cpp_nontype_template_parameter_class
	return class_description_t<json_array<"coordinates", coordinate_t>>{};
#else
	return class_description_t<
	  json_array<symbols_coordinates_t::coordinates, coordinate_t>>{};
#endif
}

int main( int argc, char **argv ) {
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

	auto const [x, y, z, sz] = *daw::bench_n_test_mbs<10>(
	  "coords bench", json_sv.size( ),
	  [&]( iterator_t f, iterator_t l ) noexcept {
		  double x1 = 0.0;
		  double y1 = 0.0;
		  double z1 = 0.0;
		  size_t sz1 = 0U;
		  while( f != l ) {
			  auto c = *f;
			  ++sz1;
			  x1 += c.x;
			  y1 += c.y;
			  z1 += c.z;
			  ++f;
		  }
		  return std::make_tuple( x1, y1, z1, sz1 );
	  },
	  first, last );

	//	auto const sz = cls.coordinates.size( );
	auto const dsz = static_cast<double>( sz );
	std::cout << x / dsz << '\n';
	std::cout << y / dsz << '\n';
	std::cout << z / dsz << '\n';
}
