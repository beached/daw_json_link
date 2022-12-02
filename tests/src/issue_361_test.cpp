// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/daw_json_link
//

#include "defines.h"

#include "daw/json/daw_json_link.h"

#include <daw/daw_read_file.h>

#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <string>
#include <unordered_map>

namespace daw::cookbook_kv1b {
	struct MyKeyValue1 {
		std::unordered_map<int, double> kv;
	};

	bool operator==( MyKeyValue1 const &lhs, MyKeyValue1 const &rhs ) {
		return lhs.kv == rhs.kv;
	}
} // namespace daw::cookbook_kv1b

namespace daw::json {
	template<>
	struct json_data_contract<daw::cookbook_kv1b::MyKeyValue1> {
		static constexpr char const kv[] = "kv";
		using type = json_member_list<json_key_value<
		  kv, std::unordered_map<int, double>>>; /*, double,
		   json_number_no_name<int, options::number_opt(
		                              options::LiteralAsStringOpt::Always
		   )>>>;*/

		static inline auto
		to_json_data( daw::cookbook_kv1b::MyKeyValue1 const &value ) {
			return std::forward_as_tuple( value.kv );
		}
	};
} // namespace daw::json

int main( int argc, char **argv )
#ifdef DAW_USE_EXCEPTIONS
  try
#endif
{
	if( argc <= 1 ) {
		puts( "Must supply path to cookbook_kv1b.json file\n" );
		exit( EXIT_FAILURE );
	}
	auto json_doc = *daw::read_file( argv[1] );

	auto kv = daw::json::from_json<daw::cookbook_kv1b::MyKeyValue1>( json_doc );

	test_assert( kv.kv.size( ) == 2, "Expected data to have 2 items" );
	test_assert( kv.kv[123] == 1234, "Unexpected value" );
	test_assert( kv.kv[545] == 314159, "Unexpected value" );
	auto const str = daw::json::to_json( kv );
	puts( str.c_str( ) );
	auto const kv2 = daw::json::from_json<daw::cookbook_kv1b::MyKeyValue1>( str );

	test_assert( kv == kv2, "Unexpected round trip error" );
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
