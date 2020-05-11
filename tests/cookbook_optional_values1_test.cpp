// The MIT License (MIT)
//
// Copyright (c) Darrell Wright
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
#include <iostream>
#include <memory>
#include <optional>
#include <string>

namespace daw::cookbook_optional_values1 {
	struct MyOptionalStuff1 {
		std::optional<int> member0{};
		std::string member1{};
		std::unique_ptr<bool> member2{};
	};

	bool operator==( MyOptionalStuff1 const &lhs, MyOptionalStuff1 const &rhs ) {
		bool result = lhs.member0 == rhs.member0;
		result = result and ( lhs.member1 == rhs.member1 );
		result = result and ( lhs.member2 and rhs.member2 and
		                      ( *lhs.member2 == *rhs.member2 ) );
		result = result or ( not rhs.member2 );
		return result;
	}
} // namespace daw::cookbook_optional_values1

namespace daw::json {
	template<>
	struct json_data_contract<daw::cookbook_optional_values1::MyOptionalStuff1> {
#if defined( __cpp_nontype_template_parameter_class )
		using type = json_member_list<
		  json_number_null<"member0", std::optional<int>>, json_string<"member1">,
		  json_bool_null<"member2", std::unique_ptr<bool>,
		                 LiteralAsStringOpt::Never, UniquePtrConstructor<bool>>>;
#else
		static inline constexpr char const member0[] = "member0";
		static inline constexpr char const member1[] = "member1";
		static inline constexpr char const member2[] = "member2";
		using type = json_member_list<
		  json_number_null<member0, std::optional<int>>, json_string<member1>,
		  json_bool_null<member2, std::unique_ptr<bool>, LiteralAsStringOpt::Never,
		                 UniquePtrConstructor<bool>>>;
#endif
		static inline auto to_json_data(
		  daw::cookbook_optional_values1::MyOptionalStuff1 const &value ) {
			return std::forward_as_tuple( value.member0, value.member1,
			                              value.member2 );
		}
	};
} // namespace daw::json

int main( int argc, char **argv ) try {
	if( argc <= 1 ) {
		puts( "Must supply path to cookbook_optional_values1.json file\n" );
		exit( EXIT_FAILURE );
	}
	auto data = daw::filesystem::memory_mapped_file_t<>( argv[1] );
	puts( "Original" );
	puts( std::string( data.data( ), data.size( ) ).c_str( ) );

	std::vector<daw::cookbook_optional_values1::MyOptionalStuff1> stuff =
	  daw::json::from_json_array<
	    daw::cookbook_optional_values1::MyOptionalStuff1>( data );

	daw_json_assert( stuff.size( ) == 2, "Unexpected size" );
	daw_json_assert( not stuff.front( ).member2, "Unexpected value" );
	daw_json_assert( not stuff.back( ).member0, "Unexpected value" );

	std::string const str = daw::json::to_json_array( stuff );

	puts( "After" );
	puts( str.c_str( ) );

	std::vector<daw::cookbook_optional_values1::MyOptionalStuff1> stuff2 =
	  daw::json::from_json_array<
	    daw::cookbook_optional_values1::MyOptionalStuff1>( str );

	daw_json_assert( stuff == stuff2, "Unexpected round trip error" );
	return 0;
} catch( daw::json::json_exception const &jex ) {
	std::cerr << "Exception thrown by parser: " << jex.reason( ) << std::endl;
	exit( 1 );
}
