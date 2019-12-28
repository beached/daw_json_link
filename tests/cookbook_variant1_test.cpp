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

#include <daw/daw_benchmark.h>
#include <daw/daw_memory_mapped_file.h>

#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <memory>
#include <optional>
#include <string>

namespace daw::cookbook_variant1 {
	struct MyVariantStuff1 {
		std::variant<int, std::string> member0;
		std::variant<std::string, bool> member1;
	};

#if defined( __cpp_nontype_template_parameter_class )
	auto json_data_contract_for( MyVariantStuff1 const & ) {
		using namespace daw::json;
		return json_data_contract<
		  json_variant<"member0", std::variant<int, std::string>,
		               json_variant_type_list<json_number<no_name, int>,
		                                      json_string<no_name>>>,
		  json_variant<
		    "member1", std::variant<std::string, bool>,
		    json_variant_type_list<json_string<no_name>, json_bool<no_name>>>>{};
	}
#else
	namespace symbols_MyVariantStuff1 {
		static constexpr char const member0[] = "member0";
		static constexpr char const member1[] = "member1";
	} // namespace symbols_MyVariantStuff1

	auto json_data_contract_for( MyVariantStuff1 const & ) {
		using namespace daw::json;
		return json_data_contract<
		  json_variant<symbols_MyVariantStuff1::member0,
		               std::variant<int, std::string>,
		               json_variant_type_list<json_number<no_name, int>,
		                                      json_string<no_name>>>,
		  json_variant<
		    symbols_MyVariantStuff1::member1, std::variant<std::string, bool>,
		    json_variant_type_list<json_string<no_name>, json_bool<no_name>>>>{};
	}
#endif
	auto to_json_data( MyVariantStuff1 const &value ) {
		return std::forward_as_tuple( value.member0, value.member1 );
	}

	bool operator==( MyVariantStuff1 const &lhs, MyVariantStuff1 const &rhs ) {
		return lhs.member0 == rhs.member0 and lhs.member1 == rhs.member1;
	}
} // namespace daw::cookbook_variant1

int main( int argc, char **argv ) {
	if( argc <= 1 ) {
		puts( "Must supply path to cookbook_variant1.json file\n" );
		exit( EXIT_FAILURE );
	}
	auto data = daw::filesystem::memory_mapped_file_t<>( argv[1] );
	puts( "Original" );
	puts( std::string( data.data( ), data.size( ) ).c_str( ) );
	auto stuff =
	  daw::json::from_json_array<daw::cookbook_variant1::MyVariantStuff1>( data );
	daw_json_assert( stuff.size( ) == 2, "Unexpected size" );
	daw_json_assert( stuff.front( ).member0.index( ) == 0, "Unexpected value" );
	daw_json_assert( std::get<0>( stuff.front( ).member0 ) == 5,
	                 "Unexpected value" );
	daw_json_assert( stuff.front( ).member1.index( ) == 0, "Unexpected value" );
	daw_json_assert( std::get<0>( stuff.front( ).member1 ) == "hello",
	                 "Unexpected value" );

	auto const str = daw::json::to_json_array( stuff );
	puts( "After" );
	puts( str.c_str( ) );
	auto stuff2 =
	  daw::json::from_json_array<daw::cookbook_variant1::MyVariantStuff1>( str );

	daw_json_assert( stuff == stuff2, "Unexpected round trip error" );
	return 0;
}
