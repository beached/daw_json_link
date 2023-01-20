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
#include <tuple>
#include <type_traits>
#include <vector>

namespace daw::cookbook_optional_values1 {
	/**
	 * This is used for nullables who's member is a unique_ptr.
	 * @tparam T Type of value stored in unique_ptr
	 */
	struct MyOptionalStuff1 {
		std::optional<int> member0{ };
		std::string member1{ };
		std::unique_ptr<bool> member2{ };

		bool operator==( MyOptionalStuff1 const &rhs ) const {
			bool result = member0 == rhs.member0;
			result = result and ( member1 == rhs.member1 );
			result =
			  result and ( member2 and rhs.member2 and ( *member2 == *rhs.member2 ) );
			result = result or ( not rhs.member2 );
			return result;
		}

		bool operator!=( MyOptionalStuff1 const &rhs ) const {
			return not( *this == rhs );
		}
	};
} // namespace daw::cookbook_optional_values1

namespace daw::json {
	template<>
	struct json_data_contract<daw::cookbook_optional_values1::MyOptionalStuff1> {
		static constexpr char const member0[] = "member0";
		static constexpr char const member1[] = "member1";
		static constexpr char const member2[] = "member2";
		using type = json_member_list<
		  json_number_null<member0, std::optional<int>>, json_string<member1>,
		  json_bool_null<member2, std::unique_ptr<bool>,
		                 options::bool_opt( options::LiteralAsStringOpt::Never )>>;

		static inline auto to_json_data(
		  daw::cookbook_optional_values1::MyOptionalStuff1 const &value ) {
			return std::forward_as_tuple( value.member0, value.member1,
			                              value.member2 );
		}
	};
} // namespace daw::json

int main( int argc, char **argv )
#ifdef DAW_USE_EXCEPTIONS
  try
#endif
{
	if( argc <= 1 ) {
		puts( "Must supply path to cookbook_optional_values1.json file\n" );
		exit( EXIT_FAILURE );
	}
	auto data = *daw::read_file( argv[1] );
	puts( "Original" );
	puts( data.c_str( ) );

	auto stuff = daw::json::from_json_array<
	  daw::cookbook_optional_values1::MyOptionalStuff1>( data );

	test_assert( stuff.size( ) == 2, "Unexpected size" );
	test_assert( not stuff.front( ).member2, "Unexpected value" );
	test_assert( not stuff.back( ).member0, "Unexpected value" );

	std::string const str = daw::json::to_json_array( stuff );

	puts( "After" );
	puts( str.c_str( ) );

	std::vector<daw::cookbook_optional_values1::MyOptionalStuff1> stuff2 =
	  daw::json::from_json_array<
	    daw::cookbook_optional_values1::MyOptionalStuff1>( str );

	test_assert( stuff == stuff2, "Unexpected round trip error" );
	return 0;
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