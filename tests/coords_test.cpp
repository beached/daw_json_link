// The MIT License (MIT)
//
// Copyright (c) 2018-2019 Darrell Wright
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

#include <fstream>
#include <iostream>
#include <string_view>
#include <vector>

#include <daw/daw_benchmark.h>
#include <daw/daw_memory_mapped_file.h>

#include "daw/json/daw_json_link.h"

struct coordinate_t {
	double x;
	double y;
	double z;
	std::string name;
};

auto describe_json_class( coordinate_t ) noexcept {
	using namespace daw::json;
#if __cplusplus > 201703L or ( defined( __GNUC__ ) and __GNUC__ >= 9 )
	return class_description_t<json_number<"x">, json_number<"y">,
	                           json_number<"z">, json_string<"name">>{};
#else
	constexpr static char const x[] = "x";
	constexpr static char const y[] = "y";
	constexpr static char const z[] = "z";
	constexpr static char const name[] = "name";
	return class_description_t<json_number<x>, json_number<y>, json_number<z>,
	                           json_string<name>>{};
#endif
}

struct coordinates_t {
	std::vector<coordinate_t> coordinates;
	std::string info;
};
auto describe_json_class( coordinates_t ) noexcept {
	using namespace daw::json;
#if __cplusplus > 201703L or ( defined( __GNUC__ ) and __GNUC__ >= 9 )
	return class_description_t<
	  json_array<"coordinates", std::vector<coordinate_t>,
	             json_class<no_name, coordinate_t>>,
	  json_string<"info">>{};
#else
	constexpr static char const coordinates[] = "coordinates";
	constexpr static char const info[] = "info";
	return class_description_t<json_array<coordinates, std::vector<coordinate_t>,
	                                      json_class<no_name, coordinate_t>>,
	                           json_string<info>>{};
#endif
}

int main( int argc, char **argv ) {
	using namespace daw::json;
	if( argc < 2 ) {
		std::cerr << "Must supply a filename to open\n";
		exit( 1 );
	}
	auto json_data = daw::filesystem::memory_mapped_file_t<char>( argv[1] );
	auto json_sv = std::string_view( json_data.data( ), json_data.size( ) );
	std::cout << "File size(B): " << json_data.size( ) << " "
	          << daw::utility::to_bytes_per_second( json_data.size( ) ) << '\n';

	auto [x, y, z, sz] = *daw::bench_n_test_mbs<100>(
	  "finding sum of x,y, and z's", json_sv.size( ),
	  [&]( auto &&sv ) {
		  auto cls = daw::json::from_json<coordinates_t>( sv );
		  auto result =
		    std::accumulate( cls.coordinates.cbegin( ), cls.coordinates.cend( ),
		                     std::make_tuple( 0.0, 0.0, 0.0, 0ULL ),
		                     []( auto &&r, coordinate_t const &c ) {
			                     std::get<0>( r ) += c.x;
			                     std::get<1>( r ) += c.y;
			                     std::get<2>( r ) += c.z;
			                     return r;
		                     } );
		  std::get<3>( result ) = cls.coordinates.size( );
		  return result;
	  },
	  json_sv );

	std::cout << x / sz << '\n';
	std::cout << y / sz << '\n';
	std::cout << z / sz << '\n';
}
