// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/daw_json_link
//

#include "defines.h"

#include <daw/daw_memory_mapped_file.h>

#include "daw/json/daw_json_link.h"

#include <chrono>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <string>

namespace daw::cookbook_dates4 {
	struct MyClass4 {
		std::string name;
		std::chrono::time_point<std::chrono::system_clock,
		                        std::chrono::milliseconds>
		  timestamp;
	};

	bool operator==( MyClass4 const &lhs, MyClass4 const &rhs ) {
		return std::tie( lhs.name, lhs.timestamp ) ==
		       std::tie( rhs.name, rhs.timestamp );
	}

	struct TimestampConverter {
		DAW_CONSTEXPR std::chrono::time_point<std::chrono::system_clock,
		                                      std::chrono::milliseconds>
		operator( )( std::string_view sv ) const {
			auto sv2 = daw::string_view( sv.data( ), sv.size( ) );
			DAW_CONSTEXPR daw::string_view prefix = "/DATE(";
			DAW_CONSTEXPR daw::string_view suffix = ")/";
			daw_json_assert( sv2.starts_with( prefix ), "Unexpected date format" );
			daw_json_assert( sv2.ends_with( suffix ), "Unexpected date format" );
			sv2.remove_prefix( prefix.size( ) );
			sv2.remove_suffix( suffix.size( ) );

			int64_t val =
			  daw::json::from_json<int64_t, daw::json::NoCommentSkippingPolicyChecked,
			                       true>( sv2 );
			DAW_CONSTEXPR const auto epoch =
			  daw::json::datetime::civil_to_time_point( 1970, 1, 1, 0, 0, 0, 0 );

			return epoch + std::chrono::seconds( val );
		}

		template<typename OutputIterator>
		DAW_CONSTEXPR OutputIterator operator( )(
		  OutputIterator it, std::chrono::time_point<std::chrono::system_clock,
		                                             std::chrono::milliseconds>
		                       tp ) const {

			auto const &[yr, mo, dy, hr, mn, se, ms] =
			  daw::json::datetime::time_point_to_civil( tp );
			// Day of Week
			it = daw::json::utils::copy_to_iterator(
			  it, daw::json::datetime::short_day_of_week( tp ) );
			*it++ = ' ';
			// Month
			it = daw::json::utils::copy_to_iterator(
			  it, daw::json::datetime::month_short_name( mo ) );
			*it++ = ' ';
			it = daw::json::utils::integer_to_string( it, dy );
			*it++ = ' ';
			if( hr < 10 ) {
				*it++ = '0';
			}
			it = daw::json::utils::integer_to_string( it, hr );
			*it++ = ':';
			if( mn < 10 ) {
				*it++ = '0';
			}
			it = daw::json::utils::integer_to_string( it, mn );
			*it++ = ':';
			if( se < 10 ) {
				*it++ = '0';
			}
			it = daw::json::utils::integer_to_string( it, se );
			it = daw::json::utils::copy_to_iterator( it, " +0000 " );
			it = daw::json::utils::integer_to_string( it, yr );
			return it;
		}
	};

	template<JSONNAMETYPE name>
	using json_timestamp =
	  daw::json::json_custom<name,
	                         std::chrono::time_point<std::chrono::system_clock,
	                                                 std::chrono::milliseconds>,
	                         TimestampConverter, TimestampConverter>;
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

int main( int argc, char **argv ) try {
	if( argc <= 1 ) {
		puts( "Must supply path to cookbook_dates4.json file\n" );
		exit( EXIT_FAILURE );
	}
	auto data = daw::filesystem::memory_mapped_file_t<>( argv[1] );
	puts( data.data( ) );

	daw::cookbook_dates4::MyClass4 const cls =
	  daw::json::from_json<daw::cookbook_dates4::MyClass4>(
	    std::string_view( data.data( ), data.size( ) ) );

	daw_json_assert( cls.name == "Toronto", "Unexpected value" );

	std::string const str = daw::json::to_json( cls );
	puts( str.c_str( ) );

	daw::cookbook_dates4::MyClass4 const cls4 =
	  daw::json::from_json<daw::cookbook_dates4::MyClass4>( str );

	daw_json_assert( cls == cls4, "Unexpected round trip error" );
} catch( daw::json::json_exception const &jex ) {
	std::cerr << "Exception thrown by parser: " << jex.reason( ) << std::endl;
	exit( 1 );
}
