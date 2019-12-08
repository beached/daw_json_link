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

#include <fstream>
#include <iostream>
#include <string>

#include "daw/json/daw_json_iterator.h"
#include "daw/json/daw_json_link.h"

struct City {
	std::string_view country;
	std::string_view name;
	float lat;
	float lng;
};

namespace symbols_City {
	static constexpr char const names0[] = "country";
	static constexpr char const names1[] = "name";
	static constexpr char const names2[] = "lat";
	static constexpr char const names3[] = "lng";
} // namespace symbols_City
auto describe_json_class( City ) noexcept {
	using namespace daw::json;
#ifdef __cpp_nontype_template_parameter_class
	return class_description_t<
	  json_string_raw<"country", std::string_view>,
	  json_string_raw<"name", std::string_view>,
	  json_number<"lat", float, LiteralAsStringOpt::Always>,
	  json_number<"lng", float, LiteralAsStringOpt::Always>>{};
#else
	return class_description_t<
	  json_string_raw<symbols_City::names0, std::string_view>,
	  json_string_raw<symbols_City::names1, std::string_view>,
	  json_number<symbols_City::names2, float, LiteralAsStringOpt::Always>,
	  json_number<symbols_City::names3, float, LiteralAsStringOpt::Always>>{};
#endif
}

// Order of values must match order specified in class_description
constexpr auto to_json_data( City const &c ) {
	return std::forward_as_tuple( c.country, c.name, c.lat, c.lng );
}

std::string get_json_data( std::string file_name ) {
	auto in_file = std::ifstream( std::data( file_name ) );
	if( !in_file ) {
		std::cerr << "Could not open input file\n";
		exit( 1 );
	}
	return std::string( std::istreambuf_iterator<char>( in_file ),
	                    std::istreambuf_iterator<char>( ) );
}

int main( int argc, char **argv ) {
	using namespace daw::json;
	// uses cities.json
	if( argc < 2 ) {
		std::cerr << "Must supply a filename to open\n";
		exit( 1 );
	}
	auto json_data = get_json_data( argv[1] );
	auto sz = json_data.size( );
	json_data.append( 60ULL,
	                  ' ' ); // Account for max digits in float if in bad form
	json_data += ",]\"}tfn"; // catch any thing looking for these values
	auto json_sv = std::string_view( json_data.data( ), sz );

	using iterator_t = daw::json::json_array_iterator<City>;

	auto const pos =
	  std::find_if( iterator_t( json_sv ), iterator_t( ),
	                []( City &&city ) { return city.name == "Chitungwiza"; } );
	if( pos == iterator_t( ) ) {
		std::cout << "Not found\n";
		return 0;
	}

	std::cout << "Chitungwiza was found.\n" << to_json( *pos ) << '\n';
}
