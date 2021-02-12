// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/daw_json_link
//
// See cookbook/dates.md for the 2nd example
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

namespace daw::cookbook_dates2 {
	struct MyClass2 {
		std::string name;
		std::chrono::time_point<std::chrono::system_clock,
		                        std::chrono::milliseconds>
		  timestamp;
	};

	bool operator==( MyClass2 const &lhs, MyClass2 const &rhs ) {
		return std::tie( lhs.name, lhs.timestamp ) ==
		       std::tie( rhs.name, rhs.timestamp );
	}

	struct TimestampConverter {
		DAW_CONSTEXPR std::chrono::time_point<std::chrono::system_clock,
		                                      std::chrono::milliseconds>
		operator( )( std::string_view sv ) const {
			test_assert( sv.size( ) >= 26,
			             "Date format is always 26 characters long" );
			// Skip Day of Week
			sv.remove_prefix( 4 );
			auto const mo = daw::json::datetime::parse_short_month( sv );
			sv.remove_prefix( 4 );
			auto const dy =
			  daw::json::parse_utils::parse_unsigned<uint_least32_t, 2>( sv.data( ) );
			sv.remove_prefix( 3 );
			auto const hr =
			  daw::json::parse_utils::parse_unsigned<uint_least32_t, 2>( sv.data( ) );
			sv.remove_prefix( 3 );
			auto const mn =
			  daw::json::parse_utils::parse_unsigned<uint_least32_t, 2>( sv.data( ) );
			sv.remove_prefix( 3 );
			auto const se =
			  daw::json::parse_utils::parse_unsigned<uint_least32_t, 2>( sv.data( ) );
			sv.remove_prefix( 3 );
			auto const sign = sv.front( ) == '+' ? 1 : -1;
			sv.remove_prefix( 1 );
			auto const off_hr =
			  daw::json::parse_utils::parse_unsigned<int_least32_t, 2>( sv.data( ) ) *
			  sign;
			sv.remove_prefix( 2 );
			auto const off_mn =
			  daw::json::parse_utils::parse_unsigned<int_least32_t, 2>( sv.data( ) ) *
			  sign;
			sv.remove_prefix( 3 );
			int const yr_sign = [&] {
				if( sv.front( ) == '-' ) {
					sv.remove_prefix( 1 );
					return -1;
				} else if( sv.front( ) == '+' ) {
					sv.remove_prefix( 1 );
				}
				return 1;
			}( );
			auto const yr =
			  yr_sign *
			  daw::json::parse_utils::parse_unsigned2<int_least32_t>( sv.data( ) );

			return daw::json::datetime::civil_to_time_point( yr, mo, dy, hr, mn, se,
			                                                 0 ) +
			       std::chrono::hours( off_hr ) + std::chrono::minutes( off_mn );
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
} // namespace daw::cookbook_dates2

namespace daw::json {
	template<>
	struct json_data_contract<daw::cookbook_dates2::MyClass2> {
#if defined( __cpp_nontype_template_parameter_class )
		using type =
		  json_member_list<json_string<"name">,
		                   daw::cookbook_dates2::json_timestamp<"timestamp">>;
#else
		static constexpr char const name[] = "name";
		static constexpr char const timestamp[] = "timestamp";
		using type =
		  json_member_list<json_string<name>,
		                   daw::cookbook_dates2::json_timestamp<timestamp>>;
#endif
		static inline auto
		to_json_data( daw::cookbook_dates2::MyClass2 const &value ) {
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
		puts( "Must supply path to cookbook_dates2.json file\n" );
		exit( EXIT_FAILURE );
	}
	auto data = *daw::read_file( argv[1] );
	puts( data.data( ) );

	auto const cls = daw::json::from_json<daw::cookbook_dates2::MyClass2>(
	  std::string_view( data.data( ), data.size( ) ) );

	test_assert( cls.name == "Toronto", "Unexpected value" );

	std::string const str = daw::json::to_json( cls );
	puts( str.c_str( ) );

	auto const cls2 = daw::json::from_json<daw::cookbook_dates2::MyClass2>( str );

	test_assert( cls == cls2, "Unexpected round trip error" );
}
#ifdef DAW_USE_JSON_EXCEPTIONS
catch( daw::json::json_exception const &jex ) {
	std::cerr << "Exception thrown by parser: " << jex.reason( ) << std::endl;
	exit( 1 );
}
#endif
