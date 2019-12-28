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

#include "daw/json/daw_json_link.h"

#include <daw/daw_memory_mapped_file.h>

namespace daw {
	struct Data {
		int a;
	};

#if defined( __cpp_nontype_template_parameter_class )
	auto json_data_contract_for( Data const & ) {
		using namespace daw::json;
		return json_data_contract<json_number<"a", int>>{};
	}
#else
	namespace symbols_Data {
		static constexpr char const a[] = "a";
	} // namespace symbols_Data

	auto json_data_contract_for( Data const & ) {
		using namespace daw::json;
		return json_data_contract<json_number<symbols_Data::a, int>>{};
	}
#endif
	auto to_json_data( Data const &value ) {
		return std::forward_as_tuple( value.a );
	}

	bool operator==( Data const &lhs, Data const &rhs ) {
		return to_json_data( lhs ) == to_json_data( rhs );
	}
} // namespace daw

int main( int argc, char **argv ) {
	if( argc <= 1 ) {
		puts( "Must supply path to small_test.json file\n" );
		exit( EXIT_FAILURE );
	}
	auto data = daw::filesystem::memory_mapped_file_t<>( argv[1] );

	auto const cls = daw::json::from_json<daw::Data>(
	  std::string_view( data.data( ), data.size( ) ) );

	daw_json_assert( cls.a == 12345, "Unexpected value" );
}
