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

#include "daw/json/daw_json_link.h"

struct City {
	daw::string_view country;
	daw::string_view name;
	float lat;
	float lng;
};

auto describe_json_class( City ) noexcept {
	using namespace daw::json;
#ifdef USECPP20
	return class_description_t<json_string<"country", daw::string_view>,
	                           json_string<"name", daw::string_view>,
	                           json_number<"lat", float>,
	                           json_number<"lng", float>>{};
#else
	static constexpr char const names0[] = "country";
	static constexpr char const names1[] = "name";
	static constexpr char const names2[] = "lat";
	static constexpr char const names3[] = "lng";
	return class_description_t<json_string<names0, daw::string_view>,
	                           json_string<names1, daw::string_view>,
	                           json_number<names2, float>,
	                           json_number<names3, float>>{};
#endif
}

// Order of values must match order specified in class_description
constexpr auto to_json_data( City const &c ) {
	return std::forward_as_tuple( c.country, c.name, c.lat, c.lng );
}

std::string get_json_data( daw::string_view file_name ) {
	auto in_file = std::ifstream( file_name.data( ) );
	if( !in_file ) {
		std::cerr << "Could not open input file\n";
		exit( 1 );
	}
	return std::string( std::istreambuf_iterator<char>( in_file ),
	                    std::istreambuf_iterator<char>( ) );
}

int main( int argc, char **argv ) {
	using namespace daw::json;
	if( argc < 2 ) {
		std::cerr << "Must supply a filename to open\n";
		exit( 1 );
	}
	auto const json_data = get_json_data( argv[1] );
	using iterator_t = daw::json::json_array_iterator<json_class<no_name, City>>;

	auto const pos =
	  std::find_if( iterator_t( json_data ), iterator_t( ),
	                []( City &&city ) { return city.name == "Chitungwiza"; } );
	if( pos == iterator_t( ) ) {
		std::cout << "Not found\n";
		return 0;
	}

	std::cout << "Chitungwiza was found.\n" << to_json( *pos ) << '\n';
}
