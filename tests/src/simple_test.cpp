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

#include <fstream>
#include <iostream>
#include <string>

struct City {
	std::string_view country;
	std::string_view name;
	float lat;
	float lng;
};

namespace daw::json {
	template<>
	struct json_data_contract<City> {
#ifdef __cpp_nontype_template_parameter_class
		using type =
		  json_member_list<json_string_raw<"country", std::string_view>,
		                   json_string_raw<"name", std::string_view>,
		                   json_number<"lat", float, LiteralAsStringOpt::Always>,
		                   json_number<"lng", float, LiteralAsStringOpt::Always>>;
#else
		static constexpr char const country[] = "country";
		static constexpr char const name[] = "name";
		static constexpr char const lat[] = "lat";
		static constexpr char const lng[] = "lng";
		using type =
		  json_member_list<json_string_raw<country, std::string_view>,
		                   json_string_raw<name, std::string_view>,
		                   json_number<lat, float, LiteralAsStringOpt::Always>,
		                   json_number<lng, float, LiteralAsStringOpt::Always>>;
#endif
		static DAW_CONSTEXPR auto to_json_data( City const &c ) {
			return std::forward_as_tuple( c.country, c.name, c.lat, c.lng );
		}
	};
} // namespace daw::json

std::string get_json_data( std::string file_name ) {
	auto in_file = std::ifstream( std::data( file_name ) );
	if( !in_file ) {
		std::cerr << "Could not open input file\n";
		exit( 1 );
	}
	return std::string( std::istreambuf_iterator<char>( in_file ),
	                    std::istreambuf_iterator<char>( ) );
}

int main( int argc, char **argv )
#ifdef DAW_USE_JSON_EXCEPTIONS
  try
#endif
{
	using namespace daw::json;
	// uses cities.json
	if( argc < 2 ) {
		std::cerr << "Must supply a filename to open\n";
		exit( 1 );
	}
	auto json_data = get_json_data( argv[1] );
	auto sz = json_data.size( );
	json_data.append( 60ULL,
	                  ' ' ); // Account for max digits in float if in bad form
	json_data += ",]\"}tfn"; // catch any thing looking for these values
	auto json_sv = std::string_view( json_data.data( ), sz );

	using iterator_t = daw::json::json_array_iterator<City>;

	auto const pos =
	  std::find_if( iterator_t( json_sv ), iterator_t( ),
	                []( City &&city ) { return city.name == "Chitungwiza"; } );
	if( pos == iterator_t( ) ) {
		std::cout << "Not found\n";
		return 0;
	}

	std::cout << "Chitungwiza was found.\n" << to_json( *pos ) << '\n';
}
#ifdef DAW_USE_JSON_EXCEPTIONS
catch( daw::json::json_exception const &jex ) {
	std::cerr << "Exception thrown by parser: " << jex.reason( ) << std::endl;
	exit( 1 );
}
#endif
