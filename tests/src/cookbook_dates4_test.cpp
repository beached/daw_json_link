// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/daw_json_link
//
// See cookbook/dates.md for the 4th example
//

#include "defines.h"

#include <daw/daw_read_file.h>

#include "daw/json/daw_json_link.h"

#include <chrono>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <string>

namespace daw::cookbook_dates4 {
	using timepoint_t = std::chrono::time_point<std::chrono::system_clock,
	                                            std::chrono::milliseconds>;
	struct MyClass4 {
		std::string name;
		timepoint_t timestamp;
	};

	bool operator==( MyClass4 const &lhs, MyClass4 const &rhs ) {
		return std::tie( lhs.name, lhs.timestamp ) ==
		       std::tie( rhs.name, rhs.timestamp );
	}
	static DAW_CONSTEXPR std::string_view const prefix = "/Date(";
	static DAW_CONSTEXPR std::string_view const suffix = ")/";

	struct TimestampConverter {
		DAW_CONSTEXPR timepoint_t operator( )( std::string_view sv ) const {
			test_assert( sv.size( ) > ( prefix.size( ) + suffix.size( ) ),
			             "Unexpected date size" );
			auto const sv_prefix = sv.substr( 0, prefix.size( ) );
			test_assert( sv_prefix == prefix, "Unexpected date format" );
			sv.remove_prefix( prefix.size( ) );
			auto const sv_suffix =
			  sv.substr( sv.size( ) - suffix.size( ), suffix.size( ) );
			test_assert( sv_suffix == suffix, "Unexpected date format" );
			sv.remove_suffix( suffix.size( ) );

			auto const val = daw::json::from_json<
			  std::int64_t, daw::json::NoCommentSkippingPolicyChecked, true>( sv );
			DAW_CONSTEXPR const auto epoch =
			  daw::json::datetime::civil_to_time_point( 1970, 1, 1, 0, 0, 0, 0 );

			return epoch + std::chrono::seconds( val );
		}

		template<typename OutputIterator>
		DAW_CONSTEXPR OutputIterator operator( )( OutputIterator it,
		                                          timepoint_t tp ) const {

			DAW_CONSTEXPR const auto epoch =
			  daw::json::datetime::civil_to_time_point( 1970, 1, 1, 0, 0, 0, 0 );

			// We divide by 1000 because the storage is in milliseconds
			std::int64_t const seconds_since_epoch = ( tp - epoch ).count( ) / 1000;
			it = daw::json::utils::copy_to_iterator( it, "/Date(" );
			it = daw::json::utils::integer_to_string( it, seconds_since_epoch );
			it = daw::json::utils::copy_to_iterator( it, ")/" );
			return it;
		}
	};

	template<JSONNAMETYPE name>
	using json_timestamp =
	  daw::json::json_custom<name, timepoint_t, TimestampConverter,
	                         TimestampConverter>;
} // namespace daw::cookbook_dates4

namespace daw::json {
	template<>
	struct json_data_contract<daw::cookbook_dates4::MyClass4> {
#if defined( __cpp_nontype_template_parameter_class )
		using type =
		  json_member_list<json_string<"name">,
		                   daw::cookbook_dates4::json_timestamp<"timestamp">>;
#else
		static constexpr char const name[] = "name";
		static constexpr char const timestamp[] = "timestamp";
		using type =
		  json_member_list<json_string<name>,
		                   daw::cookbook_dates4::json_timestamp<timestamp>>;
#endif
		static inline auto
		to_json_data( daw::cookbook_dates4::MyClass4 const &value ) {
			return std::forward_as_tuple( value.name, value.timestamp );
		}
	};
} // namespace daw::json

int main( int argc, char **argv )
#ifdef DAW_USE_JSON_EXCEPTIONS
  try
#endif
{
	if( argc <= 1 ) {
		puts( "Must supply path to cookbook_dates4.json file\n" );
		exit( EXIT_FAILURE );
	}
	auto data = *daw::read_file( argv[1] );
	puts( data.data( ) );

	auto const cls = daw::json::from_json<daw::cookbook_dates4::MyClass4>(
	  std::string_view( data.data( ), data.size( ) ) );

	test_assert( cls.name == "Toronto", "Unexpected value" );

	std::string const str = daw::json::to_json( cls );
	puts( str.c_str( ) );

	auto const cls4 = daw::json::from_json<daw::cookbook_dates4::MyClass4>( str );

	test_assert( cls == cls4, "Unexpected round trip error" );
}
#ifdef DAW_USE_JSON_EXCEPTIONS
catch( daw::json::json_exception const &jex ) {
	std::cerr << "Exception thrown by parser: " << jex.reason( ) << std::endl;
	exit( 1 );
}
#endif
