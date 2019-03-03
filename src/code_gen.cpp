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

#include <bitset>
#include <cstdint>
#include <iostream>
#include <string>
#include <vector>

#include <daw/daw_parser_helper_sv.h>
#include <daw/daw_read_file.h>
#include <daw/daw_string_view.h>

enum json_types : uint16_t {
	json_null,
	json_number,
	json_string,
	json_bool,
	json_class,
	json_array
};

struct json_property_t {
	size_t id;
	std::string name;
	std::bitset<6> types_seen;
	std::vector<size_t> children;
};

int main( int argc, char **argv ) {
	if( argc < 2 ) {
		std::cerr << "Must supply valid JSON file\n";
		return EXIT_FAILURE;
	}
	auto const json_str = daw::read_file( argv[1] );
	auto json_sv = daw::string_view( json_str );
	auto properties = std::vector<json_property_t>{};

	while( !json_sv.empty( ) ) {
		daw::parser::trim_left( json_sv );
		if( json_sv.empty( ) ) {
			break;
		}
		/*
		switch( json_sv.front( ) ) {
			case '[':
		}
		 */
	}
}
