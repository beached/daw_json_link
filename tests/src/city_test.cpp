// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/daw_json_link
//

/// @brief This file highlights some of the ways of working with JSON array
/// documents as they can be reified into a container or iterated over.  When
/// using the iterator/range interface we can use the std algorithms to do
/// things like count/accumulate them

#include "defines.h"

#include <daw/json/daw_json_iterator.h>
#include <daw/json/daw_json_link.h>
#include <daw/json/daw_json_value_state.h>

#include <daw/daw_benchmark.h>
#include <daw/daw_read_file.h>
#include <daw/iterator/daw_back_inserter.h>

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
#ifdef DAW_JSON_CNTTP_JSON_NAME
		using type = json_member_list<
		  json_string<"country">, json_string<"name">,
		  json_number<"lat", float,
		              options::number_opt( options::LiteralAsStringOpt::Always )>,
		  json_number<"lng", float,
		              options::number_opt( options::LiteralAsStringOpt::Always )>>;
#else
		static constexpr char const country[] = "country";
		static constexpr char const name[] = "name";
		static constexpr char const lat[] = "lat";
		static constexpr char const lng[] = "lng";
		using type = json_member_list<
		  json_string_raw<country>, json_string_raw<name>,
		  json_number<lat, float,
		              options::number_opt( options::LiteralAsStringOpt::Always )>,
		  json_number<lng, float,
		              options::number_opt( options::LiteralAsStringOpt::Always )>>;
#endif

		static inline auto to_json_data( City const &c ) {
			return std::forward_as_tuple( c.country, c.name, c.lat, c.lng );
		}
	};
} // namespace daw::json

int main( int argc, char **argv )
#ifdef DAW_USE_EXCEPTIONS
  try
#endif
{
	if( argc <= 1 ) {
		puts( "Must supply path to cities.json file\n" );
		exit( EXIT_FAILURE );
	}
	auto const file_data = *daw::read_file( argv[1] );
	std::string_view const json_data = file_data;

	std::cout << "File size(B): " << json_data.size( ) << " "
	          << daw::utility::to_bytes_per_second( json_data.size( ) ) << '\n';

	std::cout << "-------------------------------------------------\n";
	auto count = *daw::bench_n_test_mbs<DAW_NUM_RUNS>(
	  "cities parsing 1", json_data.size( ),
	  []( auto sv ) {
		  std::vector<City> data = daw::json::from_json_array<City>( sv );
		  daw::do_not_optimize( data );
		  return data.size( );
	  },
	  json_data );

	std::cout << "element count: " << count << '\n';

	using iterator_t =
	  daw::json::json_array_iterator<City,
	                                 daw::json::options::CheckedParseMode::no>;

	auto data = std::vector<City>( );

	std::cout << "-------------------------------------------------\n";
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
	std::cout << "-------------------------------------------------\n";
	auto count3 = *daw::bench_n_test_mbs<DAW_NUM_RUNS>(
	  "cities parsing 3", json_data.size( ),
	  []( auto const &sv ) {
		  return static_cast<size_t>(
		    std::distance( iterator_t( sv ), iterator_t( ) ) );
	  },
	  json_data );

	std::cout << "element count 3: " << count3 << '\n';

	std::cout << "-------------------------------------------------\n";
	auto has_toronto = *daw::bench_n_test_mbs<DAW_NUM_RUNS>(
	  "Find Toronto", json_data.size( ),
	  []( auto &&sv ) -> std::optional<City> {
		  auto pos =
		    std::find_if( iterator_t( sv ), iterator_t( ), []( City const &city ) {
			    return city.name == "Toronto";
		    } );
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
	std::cout << "-------------------------------------------------\n";
	auto has_chitungwiza = *daw::bench_n_test_mbs<DAW_NUM_RUNS>(
	  "Find Chitungwiza(last item)", json_data.size( ),
	  []( auto &&sv ) -> std::optional<City> {
		  auto pos =
		    std::find_if( iterator_t( sv ), iterator_t( ), []( City const &city ) {
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

	std::cout << "-------------------------------------------------\n";
	auto has_chitungwiza2 = *daw::bench_n_test_mbs<DAW_NUM_RUNS>(
	  "Find Chitungwiza2(last item)", json_data.size( ),
	  []( auto &&sv ) -> std::optional<City> {
		  for( auto jp : daw::json::json_value( sv ) ) {
			  auto name = jp.value["name"].get_string_view( );
			  if( name == "Chitungwiza" ) {
				  return daw::json::from_json<City>( jp.value );
			  }
		  }
		  return std::nullopt;
	  },
	  json_data );

	std::cout << "Chitungwiza was " << ( has_chitungwiza2 ? "" : "not" )
	          << " found at " << daw::json::to_json( *has_chitungwiza2 ) << '\n';

	std::cout << ( *iterator_t( json_data ) ).name << '\n';
	std::cout << "-------------------------------------------------\n";

	auto const city_ary = daw::json::from_json_array<City>( json_data );

	auto const city_ary_str = daw::json::to_json_array( city_ary );
	auto sv_city_ary = std::string_view( city_ary_str );
	std::cout << sv_city_ary.substr( 0, 100 ) << "...	"
	          << sv_city_ary.substr( sv_city_ary.size( ) - 100 ) << '\n';
	std::cout << "-------------------------------------------------\n";
	auto has_chitungwiza3 = *daw::bench_n_test_mbs<DAW_NUM_RUNS>(
	  "Find Chitungwiza3(last item)", json_data.size( ),
	  []( auto &&sv ) -> std::optional<City> {
		  for( auto jp : daw::json::json_value( sv ) ) {
			  if( jp.value[1].get_string_view( ) == "Chitungwiza" ) {
				  return daw::json::from_json<City>( jp.value );
			  }
		  }
		  return std::nullopt;
	  },
	  json_data );

	std::cout << "Chitungwiza was " << ( has_chitungwiza3 ? "" : "not" )
	          << " found at " << daw::json::to_json( *has_chitungwiza3 ) << '\n';
	std::cout << "-------------------------------------------------\n";
	std::cout << "Second element name\n";
	std::cout
	  << daw::json::json_value( json_data )["[1].name"].as<std::string_view>( )
	  << '\n';
	std::cout << "Third element name\n";
	std::cout
	  << daw::json::json_value( json_data )[2]["name"].as<std::string_view>( )
	  << '\n';
	std::cout << "-------------------------------------------------\n";
	auto mid_lat = *daw::bench_n_test_mbs<DAW_NUM_RUNS>(
	  "Calculate Middle Latitude", json_data.size( ),
	  []( auto const &jstr ) -> float {
		  uint32_t tot = 0;
		  auto result =
		    daw::algorithm::accumulate( iterator_t( jstr ), iterator_t( ), 0.0f,
		                                [&tot]( float cur, City const &city ) {
			                                ++tot;
			                                return cur + city.lat;
		                                } );
		  return result / static_cast<float>( tot );
	  },
	  json_data );

	std::cout << "mid_lat of all is: " << mid_lat << '\n';

	std::cout << "-------------------------------------------------\n";
	auto mid_lat2 = *daw::bench_n_test_mbs<DAW_NUM_RUNS>(
	  "Calculate Middle Latitude2", json_data.size( ),
	  []( auto const &jstr ) -> float {
		  std::vector<float> lats{ };
		  daw::algorithm::transform( iterator_t( jstr ), iterator_t( ),
		                             daw::back_inserter( lats ), []( auto &&l ) {
			                             return l.lat;
		                             } );

		  auto result = daw::algorithm::accumulate( std::cbegin( lats ),
		                                            std::cend( lats ), 0.0f );
		  return result / static_cast<float>( lats.size( ) );
	  },
	  json_data );
	std::cout << "mid_lat2 of all is: " << mid_lat2 << '\n';
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