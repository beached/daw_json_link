// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/daw_json_link
//

#include "defines.h"

/**************
 * OLD BENCH
 */

#include "daw/json/daw_json_iterator.h"
#include "daw/json/daw_json_link.h"

#include <daw/daw_read_file.h>

#include <cstdint>
#include <daw/daw_benchmark.h>
#include <fstream>
#include <iostream>
#include <streambuf>
#include <string_view>
#include <vector>

struct coordinate_t {
	double x;
	double y;
	double z;
	// ignore string name
	// ignore object opts
};

struct coordinates_t {
	std::vector<coordinate_t> coordinates;
};

namespace daw::json {
	template<>
	struct json_data_contract<coordinate_t> {
#ifdef __cpp_nontype_template_parameter_class
		using type =
		  json_member_list<json_number<"x">, json_number<"y">, json_number<"z">>;
#else
		constexpr inline static char const x[] = "x";
		constexpr inline static char const y[] = "y";
		constexpr inline static char const z[] = "z";
		using type =
		  json_member_list<json_number<x>, json_number<y>, json_number<z>>;
#endif
	};
} // namespace daw::json

int main( int, char ** )
#ifdef DAW_USE_JSON_EXCEPTIONS
  try
#endif
{
	using namespace daw::json;
	std::ios_base::sync_with_stdio( false );
	/*
	  if( argc < 2 ) {
	    std::cerr << "Must supply a filename to open\n";
	    exit( 1 );
	  }
	*/
	auto const json_data = *daw::read_file( "/tmp/1.json" );
	auto const json_sv = std::string_view( json_data.data( ), json_data.size( ) );

	using range_t =
	  daw::json::json_array_range<coordinate_t, NoCommentSkippingPolicyUnchecked>;

	double x = 0.0;
	double y = 0.0;
	double z = 0.0;
	uint_fast32_t sz = 0U;

	// first will be json_array_iterator to the array coordinates in root object
	for( auto c : range_t( json_sv, "coordinates" ) ) {
		++sz;
		x += c.x;
		y += c.y;
		z += c.z;
	}

	auto const dsz = static_cast<double>( sz );
	std::cout << x / dsz << '\n';
	std::cout << y / dsz << '\n';
	std::cout << z / dsz << '\n';
}
catch( daw::json::json_exception const &jex ) {
	std::cerr << "Exception thrown by parser: " << jex.reason( ) << std::endl;
	exit( 1 );
}
