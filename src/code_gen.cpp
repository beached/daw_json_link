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
#include <daw/json/impl/daw_json_link_impl.h>

namespace daw {
	namespace json {
		enum class json_types : uint16_t {
			json_null,
			json_number,
			json_string,
			json_bool,
			json_class,
			json_array
		};

		struct json_property_t {
			std::bitset<6> types_seen;

			constexpr json_property_t( json_types t_seen )
			  : types_seen( static_cast<uint16_t>( t_seen ) << 1U ) {}

			void add_type( json_types t_seen ) noexcept {
				types_seen[t_seen] = true;
			}
		};

		template<typename First, typename Last>
		constexpr json_types get_type( impl::IteratorRange<First, Last> &rng ) {
			rng.trim_left( );
			switch( rng.front( ) ) {
			case '"':
				rng.remove_prefix( );
				return json_types::json_string;
			case '{':
				rng.remove_prefix( );
				return json_types::json_class;
			case '[':
				rng.remove_prefix( );
				return json_types::json_array;
			case 't':
			case 'f':
				rng.remove_prefix( );
				return json_types::json_bool;
			case 'n':
				rng.remove_prefix( );
				return json_types::json_null;
			default:
				rng.remove_prefix( );
				return json_types::json_number;
			}
		}

		struct json_class_t {
			size_t parent;
			std::map<std::string, json_property_t>;
		};
	} // namespace json
} // namespace daw

int main( int argc, char **argv ) {
	if( argc < 2 ) {
		std::cerr << "Must supply valid JSON file\n";
		return EXIT_FAILURE;
	}
	auto const json_str = daw::read_file( argv[1] );
	auto json_sv = daw::string_view( json_str );
	auto properties = std::vector<daw::json::json_property_t>{};

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
