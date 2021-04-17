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

#include <cassert>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <map>
#include <memory>
#include <string>

namespace daw::cookbook_variant4 {
	namespace v1 {
		struct Config {
			int version;
			std::string name;
			int value;
			std::string next_question;
		};
	} // namespace v1

	namespace v2 {
		struct Config {
			int version;
			std::map<std::string, int> config_options;
			int option2;
		};
	} // namespace v2
	using configs_t = std::variant<daw::cookbook_variant4::v1::Config,
	                               daw::cookbook_variant4::v2::Config>;

	struct Switcher {
		// Convert JSON tag member to type index
		DAW_CONSTEXPR size_t operator( )( int type ) const {
			assert( ( 1 <= type and type <= 2 ) );
			return static_cast<std::size_t>( type - 1 );
		}

		// Get value for Tag from class value
		DAW_CONSTEXPR std::size_t operator( )( configs_t const &v ) const {
			return static_cast<std::size_t>( v.index( ) );
		}
	};
} // namespace daw::cookbook_variant4

namespace daw::json {
	template<>
	struct json_data_contract<daw::cookbook_variant4::v1::Config> {
		static constexpr char const mem_version[] = "version";
		static constexpr char const mem_value[] = "value";
		static constexpr char const mem_name[] = "name";
		static constexpr char const mem_next_question[] = "next question";
		using type =
		  json_member_list<json_number<mem_version, int>, json_string<mem_name>,
		                   json_number<mem_value, int>,
		                   json_string<mem_next_question>>;

		static constexpr auto
		to_json_data( daw::cookbook_variant4::v1::Config const &v ) {
			return std::forward_as_tuple( v.version, v.name, v.value,
			                              v.next_question );
		}
	};

	template<>
	struct json_data_contract<daw::cookbook_variant4::v2::Config> {
		static constexpr char const mem_version[] = "version";
		static constexpr char const mem_config_options[] = "config_options";
		static constexpr char const mem_value[] = "value";
		static constexpr char const mem_name[] = "name";
		static constexpr char const mem_option2[] = "option2";
		using type = json_member_list<
		  json_number<mem_version, int>,
		  json_key_value_array<mem_config_options, std::map<std::string, int>,
		                       json_number<mem_value, int>, json_string<mem_name>>,
		  json_number<mem_option2, int>>;

		static constexpr auto
		to_json_data( daw::cookbook_variant4::v2::Config const &v ) {
			return std::forward_as_tuple( v.version, v.config_options, v.option2 );
		}
	};

	template<>
	struct json_data_contract<daw::cookbook_variant4::configs_t> {
		static constexpr char const mem_version[] = "version";
		using type = json_submember_tagged_variant<
		  json_number<mem_version, int>, daw::cookbook_variant4::Switcher,
		  daw::cookbook_variant4::v1::Config, daw::cookbook_variant4::v2::Config>;
	};
} // namespace daw::json

int main( int argc, char **argv )
#ifdef DAW_USE_JSON_EXCEPTIONS
  try
#endif
{
	if( argc <= 1 ) {
		puts( "Must supply path to cookbook_variant4.json file\n" );
		exit( EXIT_FAILURE );
	}
	auto data = *daw::read_file( argv[1] );
	auto json_data = std::string_view( data.data( ), data.size( ) );

	auto configs =
	  daw::json::from_json_array<daw::cookbook_variant4::configs_t>( json_data );

	std::string const json_str = daw::json::to_json_array( configs );
	std::cout << json_str << '\n';
}
catch( daw::json::json_exception const &jex ) {
	std::cerr << "Exception thrown by parser: " << jex.reason( ) << std::endl;
	exit( 1 );
}
