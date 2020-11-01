// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/daw_json_link
//
// This file highlights some of the ways of working with JSON array documents as
// they can be reified into a container or iterated over.  When using the
// iterator/range interface we can use the std algorithms to do things like
// count/accumulate them

#include "defines.h"

#include "daw/json/daw_json_iterator.h"
#include "daw/json/daw_json_link.h"

#include <daw/daw_benchmark.h>
#include <daw/daw_read_file.h>
#include <daw/iterator/daw_back_inserter.h>

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

struct City {
	std::string country;
	std::string name;
	float lat;
	float lng;
};

namespace daw::json {
	template<>
	struct json_data_contract<City> {
#ifdef __cpp_nontype_template_parameter_class
		using type =
		  json_member_list<json_string<"country">, json_string<"name">,
		                   json_number<"lat", float, LiteralAsStringOpt::Always>,
		                   json_number<"lng", float, LiteralAsStringOpt::Always>>;
#else
		static constexpr char const country[] = "country";
		static constexpr char const name[] = "name";
		static constexpr char const lat[] = "lat";
		static constexpr char const lng[] = "lng";
		using type =
		  json_member_list<json_string<country>, json_string<name>,
		                   json_number<lat, float, LiteralAsStringOpt::Always>,
		                   json_number<lng, float, LiteralAsStringOpt::Always>>;
#endif

		static inline auto to_json_data( City const &c ) {
			return std::forward_as_tuple( c.country, c.name, c.lat, c.lng );
		}
	};
} // namespace daw::json

int main( int argc, char **argv )
#ifdef DAW_USE_JSON_EXCEPTIONS
  try
#endif
{
	if( argc <= 1 ) {
		puts( "Must supply path to cities.json file\n" );
		exit( EXIT_FAILURE );
	}
	auto file_data = *daw::read_file( argv[1] );
	auto const json_data =
	  std::string_view( file_data.data( ), file_data.size( ) );

	std::cout << "File size(B): " << json_data.size( ) << " "
	          << daw::utility::to_bytes_per_second( json_data.size( ) ) << '\n';

	auto count = *daw::bench_n_test_mbs<DAW_NUM_RUNS>(
	  "cities parsing 1", json_data.size( ),
	  []( auto sv ) {
		  std::vector<City> data = daw::json::from_json_array<City>( sv );
		  daw::do_not_optimize( data );
		  return data.size( );
	  },
	  json_data );

	std::cout << "element count: " << count << '\n';

	using iterator_t = daw::json::json_array_iterator<City>;

	auto data = std::vector<City>( );

	auto count2 = *daw::bench_n_test_mbs<DAW_NUM_RUNS>(
	  "cities parsing 2", json_data.size( ),
	  [&]( auto const &sv ) {
		  data.clear( );
		  std::copy( iterator_t( sv ), iterator_t( ), daw::back_inserter( data ) );
		  daw::do_not_optimize( data );
		  return data.size( );
	  },
	  json_data );

	std::cout << "element count 2: " << count2 << '\n';
	auto count3 = *daw::bench_n_test_mbs<DAW_NUM_RUNS>(
	  "cities parsing 3", json_data.size( ),
	  []( auto const &sv ) {
		  return static_cast<size_t>(
		    std::distance( iterator_t( sv ), iterator_t( ) ) );
	  },
	  json_data );

	std::cout << "element count 3: " << count3 << '\n';

	auto has_toronto = *daw::bench_n_test_mbs<DAW_NUM_RUNS>(
	  "Find Toronto", json_data.size( ),
	  []( auto &&sv ) -> std::optional<City> {
		  auto pos =
		    std::find_if( iterator_t( sv ), iterator_t( ),
		                  []( City &&city ) { return city.name == "Toronto"; } );
		  if( pos != iterator_t( ) ) {
			  return *pos;
		  }
		  return std::nullopt;
	  },
	  json_data );
	std::cout << "Toronto was " << ( has_toronto ? "" : "not" );
	if( has_toronto ) {
		std::cout << " found at " << daw::json::to_json( *has_toronto );
	}
	std::cout << '\n';
	auto has_chitungwiza = *daw::bench_n_test_mbs<DAW_NUM_RUNS>(
	  "Find Chitungwiza(last item)", json_data.size( ),
	  []( auto &&sv ) -> std::optional<City> {
		  auto pos =
		    std::find_if( iterator_t( sv ), iterator_t( ), []( City &&city ) {
			    return city.name == "Chitungwiza";
		    } );
		  if( pos != iterator_t( ) ) {
			  return *pos;
		  }
		  return std::nullopt;
	  },
	  json_data );

	std::cout << "Chitungwiza was " << ( has_chitungwiza ? "" : "not" )
	          << " found at " << daw::json::to_json( *has_chitungwiza ) << '\n';

	std::cout << ( *iterator_t( json_data ) ).name << '\n';

	auto const city_ary = daw::json::from_json_array<City>( json_data );

	auto const city_ary_str = daw::json::to_json_array( city_ary );
	auto sv = std::string_view( city_ary_str );
	std::cout << sv.substr( 0, 100 ) << "...	" << sv.substr( sv.size( ) - 100 )
	          << '\n';

	auto mid_lat = *daw::bench_n_test_mbs<DAW_NUM_RUNS>(
	  "Calculate Middle Latitude", json_data.size( ),
	  []( auto const &jstr ) -> float {
		  uint32_t tot = 0;
		  auto result =
		    daw::algorithm::accumulate( iterator_t( jstr ), iterator_t( ), 0.0f,
		                                [&tot]( float cur, City &&city ) {
			                                ++tot;
			                                return cur + city.lat;
		                                } );
		  return result / static_cast<float>( tot );
	  },
	  json_data );

	std::cout << "mid_lat of all is: " << mid_lat << '\n';

	auto mid_lat2 = *daw::bench_n_test_mbs<DAW_NUM_RUNS>(
	  "Calculate Middle Latitude2", json_data.size( ),
	  []( auto const &jstr ) -> float {
		  std::vector<float> lats{ };
		  daw::algorithm::transform( iterator_t( jstr ), iterator_t( ),
		                             daw::back_inserter( lats ),
		                             []( auto &&l ) { return l.lat; } );

		  auto result = daw::algorithm::accumulate( std::cbegin( lats ),
		                                            std::cend( lats ), 0.0f );
		  return result / static_cast<float>( lats.size( ) );
	  },
	  json_data );

	std::cout << "mid_lat2 of all is: " << mid_lat2 << '\n';
}
#ifdef DAW_USE_JSON_EXCEPTIONS
catch( daw::json::json_exception const &jex ) {
	std::cerr << "Exception thrown by parser: " << jex.reason( ) << std::endl;
	exit( 1 );
}
#endif
