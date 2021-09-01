// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/daw_json_link
//

#include "defines.h"

#include "daw/json/daw_json_link.h"

#include <daw/daw_benchmark.h>
#include <daw/daw_read_file.h>

#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <memory>
#include <optional>
#include <string>

namespace daw::cookbook_variant2 {
	struct MyClass {
		std::string name;
		std::optional<std::variant<std::string, int, bool>> value;
	};

	bool operator==( MyClass const &lhs, MyClass const &rhs ) {
		return std::tie( lhs.name, lhs.value ) == std::tie( rhs.name, rhs.value );
	}

	struct MyClassSwitcher {
		// Convert JSON tag member to type index
		DAW_CONSTEXPR size_t operator( )( int type ) const {
			return static_cast<size_t>( type );
		}
		// Get value for Tag from class value
		int operator( )( MyClass const &v ) const {
			return static_cast<int>( v.value->index( ) );
		}
	};
} // namespace daw::cookbook_variant2

template<>
struct daw::json::json_data_contract<daw::cookbook_variant2::MyClass> {
#ifdef __cpp_nontype_template_parameter_class
	using type = json_member_list<
	  json_string<"name">,
	  json_tagged_variant_null<
	    "value", std::optional<std::variant<std::string, int, bool>>,
	    json_number<"type", int>, daw::cookbook_variant2::MyClassSwitcher>>;
#else
	static constexpr char const type_mem[] = "type";
	static constexpr char const name[] = "name";
	static constexpr char const value[] = "value";
	using type = json_member_list<
	  json_string<name>,
	  json_tagged_variant_null<
	    value, std::optional<std::variant<std::string, int, bool>>,
	    json_number<type_mem, int>, daw::cookbook_variant2::MyClassSwitcher>>;
#endif
	static DAW_CONSTEXPR inline auto
	to_json_data( daw::cookbook_variant2::MyClass const &v ) {
		return std::forward_as_tuple( v.name, v.value );
	}
};

int main( int argc, char **argv )
#ifdef DAW_USE_EXCEPTIONS
  try
#endif
{
	if( argc <= 1 ) {
		puts( "Must supply path to optional_tagged_variant.json file\n" );
		exit( EXIT_FAILURE );
	}
	auto data = *daw::read_file( argv[1] );
	auto json_data = std::string_view( data.data( ), data.size( ) );

	std::vector<daw::cookbook_variant2::MyClass> values1 =
	  daw::json::from_json_array<daw::cookbook_variant2::MyClass>( json_data );

	std::string const json_str = daw::json::to_json_array( values1 );

	std::vector<daw::cookbook_variant2::MyClass> values2 =
	  daw::json::from_json_array<daw::cookbook_variant2::MyClass>( json_str );

	test_assert( values1 == values2, "Error in round tripping" );
}
#ifdef DAW_USE_EXCEPTIONS
catch( daw::json::json_exception const &jex ) {
	std::cerr << "Exception thrown by parser: " << jex.reason( ) << '\n';
	exit( 1 );
} catch( std::exception const &ex ) {
	std::cerr << "Unknown exception thrown during testing: " << ex.what( )
	          << '\n';
	exit( 1 );
} catch( ... ) {
	std::cerr << "Unknown exception thrown during testing\n";
	throw;
}
#endif