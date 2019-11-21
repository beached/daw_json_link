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

#include <fstream>
#include <iostream>
#include <streambuf>
#include <string_view>
#include <vector>

#include <daw/daw_benchmark.h>
#include <daw/iterator/daw_back_inserter.h>

#include "daw/json/daw_json_iterator.h"
#include "daw/json/daw_json_link.h"

struct City {
	std::string_view country;
	std::string_view name;
	float lat;
	float lng;
};

namespace symbols_City {
	static constexpr char names0[] = "country";
	static constexpr char names1[] = "name";
	static constexpr char names2[] = "lat";
	static constexpr char names3[] = "lng";
} // namespace symbols_City
auto describe_json_class( City ) noexcept {
	using namespace daw::json;
#ifdef __cpp_nontype_template_parameter_class
	return class_description_t<
	  json_string_raw<"country", std::string_view>,
	  json_string_raw<"name", std::string_view>,
	  json_number<"lat", float, LiteralAsStringOpt::always>,
	  json_number<"lng", float, LiteralAsStringOpt::always>>{};
#else
	return class_description_t<
	  json_string_raw<symbols_City::names0, std::string_view>,
	  json_string_raw<symbols_City::names1, std::string_view>,
	  json_number<symbols_City::names2, float, LiteralAsStringOpt::always>,
	  json_number<symbols_City::names3, float, LiteralAsStringOpt::always>>{};
#endif
}
// Order of values must match order specified in class_description
auto to_json_data( City const &c ) {
	return std::forward_as_tuple( c.country, c.name, c.lat, c.lng );
}

int main( int argc, char **argv ) {
	using namespace daw::json;
	if( argc < 2 ) {
		std::cerr << "Must supply a filename to open\n";
		exit( 1 );
	}
	std::cout << "Opening file: '" << argv[1] << "'\n";
	auto in_file = std::ifstream( argv[1] );
	if( !in_file ) {
		std::cerr << "Could not open input file\n";
		exit( 1 );
	}
	auto json_data = std::string( std::istreambuf_iterator<char>( in_file ),
	                              std::istreambuf_iterator<char>( ) );
	in_file.close( );
	auto sz = json_data.size( );
	json_data.append( 60ULL,
	                  ' ' ); // Account for max digits in float if in bad form
	json_data += ",]\"}tfn"; // catch any thing looking for these values
	auto json_sv = std::string_view( json_data.data( ), sz );

	std::cout << "File size(B): " << json_data.size( ) << " "
	          << daw::utility::to_bytes_per_second( json_data.size( ) ) << '\n';

	auto count = *daw::bench_n_test_mbs<10>(
	  "cities parsing 1", json_sv.size( ),
	  []( auto &&sv ) {
		  auto const data =
		    daw::json::from_json_array<json_class<no_name, City>>( sv );
		  return data.size( );
	  },
	  json_sv );

	std::cout << "element count: " << count << '\n';

	using iterator_t = daw::json::json_array_iterator<json_class<no_name, City>>;

	auto data = std::vector<City>( );

	auto count2 = *daw::bench_n_test_mbs<10>(
	  "cities parsing 2", json_sv.size( ),
	  [&]( auto &&sv ) {
		  data.clear( );
		  std::copy( iterator_t( sv ), iterator_t( ), daw::back_inserter( data ) );
		  return data.size( );
	  },
	  json_sv );

	std::cout << "element count 2: " << count2 << '\n';
	auto count3 = *daw::bench_n_test_mbs<10>(
	  "cities parsing 3", json_sv.size( ),
	  []( auto &&sv ) {
		  return static_cast<size_t>(
		    std::distance( iterator_t( sv ), iterator_t( ) ) );
	  },
	  json_sv );

	std::cout << "element count 3: " << count3 << '\n';

	auto has_toronto = *daw::bench_n_test_mbs<10>(
	  "Find Toronto", json_sv.size( ),
	  []( auto &&sv ) -> std::optional<City> {
		  auto pos =
		    std::find_if( iterator_t( sv ), iterator_t( ),
		                  []( City &&city ) { return city.name == "Toronto"; } );
		  if( pos != iterator_t( ) ) {
			  return *pos;
		  }
		  return std::nullopt;
	  },
	  json_sv );
	std::cout << "Toronto was " << ( has_toronto ? "" : "not" );
	if( has_toronto ) {
		std::cout << " found at " << to_json( *has_toronto );
	}
	std::cout << '\n';
	auto has_chitungwiza = *daw::bench_n_test_mbs<10>(
	  "Find Chitungwiza(last item)", json_sv.size( ),
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
	  json_sv );

	std::cout << "Chitungwiza was " << ( has_chitungwiza ? "" : "not" )
	          << " found at " << to_json( *has_chitungwiza ) << '\n';

	std::cout << ( *iterator_t( json_sv ) ).name << '\n';

	auto const city_ary =
	  daw::json::from_json_array<json_class<no_name, City>>( json_sv );

	auto const city_ary_str = to_json_array( city_ary );
	auto sv = std::string_view( city_ary_str );
	std::cout << sv.substr( 0, 100 ) << "...	" << sv.substr( sv.size( ) - 100 )
	          << '\n';

	auto mid_lat = *daw::bench_n_test_mbs<10>(
	  "Calculate Middle Latitude", json_sv.size( ),
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
	  json_sv );

	std::cout << "mid_lat of all is: " << mid_lat << '\n';

	auto mid_lat2 = *daw::bench_n_test_mbs<10>(
	  "Calculate Middle Latitude2", json_sv.size( ),
	  []( auto const &jstr ) -> float {
		  std::vector<float> lats{};
		  daw::algorithm::transform( iterator_t( jstr ), iterator_t( ),
		                             daw::back_inserter( lats ),
		                             []( auto &&l ) { return l.lat; } );

		  auto result = daw::algorithm::accumulate( std::cbegin( lats ),
		                                            std::cend( lats ), 0.0f );
		  return result / static_cast<float>( lats.size( ) );
	  },
	  json_sv );

	std::cout << "mid_lat2 of all is: " << mid_lat2 << '\n';
}
