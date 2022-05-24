// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/daw_json_link
//

#include "defines.h"

#include "daw/json/daw_json_iterator.h"
#include "daw/json/daw_json_link.h"

#include <daw/daw_benchmark.h>
#include <daw/daw_read_file.h>

#include <fstream>
#include <iostream>
#include <streambuf>
#include <string_view>
#include <vector>

#if not defined( DAW_NUM_RUNS )
#if not defined( DEBUG ) or defined( NDEBUG )
static inline constexpr std::size_t DAW_NUM_RUNS = 250;
#else
static inline constexpr std::size_t DAW_NUM_RUNS = 2;
#endif
#endif
static_assert( DAW_NUM_RUNS > 0 );

struct coordinate_t {
	double x;
	double y;
	double z;
};

struct coordinates_t {
	std::vector<coordinate_t> coordinates;
	std::string info;
};

namespace daw::json {
	template<>
	struct json_data_contract<coordinate_t> {
#ifdef DAW_JSON_CNTTP_JSON_NAME
		using type =
		  json_member_list<json_number<"x">, json_number<"y">, json_number<"z">>;
#else
		static constexpr char const x[] = "x";
		static constexpr char const y[] = "y";
		static constexpr char const z[] = "z";
		using type =
		  json_member_list<json_number<x>, json_number<y>, json_number<z>>;
#endif
	};

	template<>
	struct json_data_contract<coordinates_t> {
#ifdef DAW_JSON_CNTTP_JSON_NAME
		using type = json_member_list<json_array<"coordinates", coordinate_t>>;
#else
		static constexpr char const coordinates[] = "coordinates";
		using type = json_member_list<json_array<coordinates, coordinate_t>>;
#endif
	};
} // namespace daw::json

int main( int argc, char **argv )
#ifdef DAW_USE_EXCEPTIONS
  try
#endif
{
	using namespace daw::json;
	if( argc < 2 ) {
		std::cerr << "Must supply a filename to open\n";
		exit( 1 );
	}
	auto const json_data = *daw::read_file( argv[1] );
	auto json_sv = std::string_view( json_data.data( ), json_data.size( ) );

	using iterator_t = daw::json::json_array_iterator<coordinate_t>;

	auto first = iterator_t( json_sv, "coordinates" );
	auto last = iterator_t( );

	auto const [x, y, z, sz] = *daw::bench_n_test_mbs<DAW_NUM_RUNS>(
	  "coords bench", json_sv.size( ),
	  [&]( iterator_t f, iterator_t l ) noexcept {
		  double x1 = 0.0;
		  double y1 = 0.0;
		  double z1 = 0.0;
		  size_t sz1 = 0U;
		  while( f != l ) {
			  auto c = *f;
			  ++sz1;
			  x1 += c.x;
			  y1 += c.y;
			  z1 += c.z;
			  ++f;
		  }
		  return std::make_tuple( x1, y1, z1, sz1 );
	  },
	  first, last );

	//	auto const sz = cls.coordinates.size( );
	auto const dsz = static_cast<double>( sz );
	std::cout << x / dsz << '\n';
	std::cout << y / dsz << '\n';
	std::cout << z / dsz << '\n';
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
