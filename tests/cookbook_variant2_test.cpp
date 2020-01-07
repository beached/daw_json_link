// The MIT License (MIT)
//
// Copyright (c) 2019-2020 Darrell Wright
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

namespace daw::cookbook_variant2 {
	struct MyClass {
		std::string name;
		std::variant<std::string, int, bool> value;
	};

	bool operator==( MyClass const &lhs, MyClass const &rhs ) {
		return std::tie( lhs.name, lhs.value ) == std::tie( rhs.name, rhs.value );
	}

	struct MyClassSwitcher {
		// Convert JSON tag member to type index
		constexpr size_t operator( )( int type ) const {
			return static_cast<size_t>( type );
		}
		// Get value for Tag from class value
		int operator( )( MyClass const &v ) const {
			return static_cast<int>( v.value.index( ) );
		}
	};
} // namespace daw::cookbook_variant2

template<>
struct daw::json::json_data_contract<daw::cookbook_variant2::MyClass> {
#ifdef __cpp_nontype_template_parameter_class
	using type = json_member_list<
	  json_string<"name">,
	  json_tagged_variant<"value", std::variant<std::string, int, bool>,
	                      json_number<"type", int>,
	                      daw::cookbook_variant2::MyClassSwitcher,
	                      json_tagged_variant_type_list<std::string, int, bool>>>;
#else
	static inline constexpr char const type_mem[] = "type";
	static inline constexpr char const name[] = "name";
	static inline constexpr char const value[] = "value";
	using type = json_member_list<
	  json_string<name>,
	  json_tagged_variant<value, std::variant<std::string, int, bool>,
	                      json_number<type_mem, int>,
	                      daw::cookbook_variant2::MyClassSwitcher,
	                      json_tagged_variant_type_list<std::string, int, bool>>>;
#endif
	static constexpr inline auto
	to_json_data( daw::cookbook_variant2::MyClass const &v ) {
		return std::forward_as_tuple( v.name, v.value );
	}
};

int main( int argc, char **argv ) {
	if( argc <= 1 ) {
		puts( "Must supply path to cookbook_variant2.json file\n" );
		exit( EXIT_FAILURE );
	}
	auto data = daw::filesystem::memory_mapped_file_t<>( argv[1] );
	auto json_data = std::string_view( data.data( ), data.size( ) );

	std::vector<daw::cookbook_variant2::MyClass> values1 =
	  daw::json::from_json_array<daw::cookbook_variant2::MyClass>( json_data );

	std::string const json_str = daw::json::to_json_array( values1 );

	std::vector<daw::cookbook_variant2::MyClass> values2 =
	  daw::json::from_json_array<daw::cookbook_variant2::MyClass>( json_str );

	daw_json_assert( values1 == values2, "Error in round tripping" );
}
