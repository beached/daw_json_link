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

#include "daw/json/daw_json_link.h"

#include <daw/daw_memory_mapped_file.h>

#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <string>
#include <unordered_map>

int main( int argc, char **argv ) {
	if( argc <= 1 ) {
		puts(
		  "Must supply path to cookbook_parsing_individual_members1.json file\n" );
		exit( EXIT_FAILURE );
	}

	auto const file_data = daw::filesystem::memory_mapped_file_t<>( argv[1] );
	auto const json_data =
	  std::string_view( file_data.data( ), file_data.size( ) );

	using namespace daw::json;
	std::string_view b_value =
	  from_json<std::string_view>( json_data, "member2.b" );

	daw_json_assert( b_value == "found me", "Unexpected value" );

	std::optional<std::string> opt_value =
	  from_json<json_string_null<no_name>>( json_data, "a.b" );

	daw_json_assert( not opt_value, "Did not expect a value" );
}
