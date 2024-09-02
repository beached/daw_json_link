// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/daw_json_link
//

#include <daw/json/daw_json_link.h>

#include <daw/daw_ensure.h>

#include <chrono>
#include <tuple>

using timestamp_t =
  std::chrono::time_point<std::chrono::system_clock, std::chrono::milliseconds>;

struct Date {
	timestamp_t timestamp;
};

namespace daw::json {
	template<>
	struct json_data_contract<Date> {
		static constexpr char const timestamp[] = "timestamp";
		using type = json_member_list<json_date<timestamp, timestamp_t>>;

		static constexpr auto to_json_data( Date const &d ) {
			return std::forward_as_tuple( d.timestamp );
		}
	};
} // namespace daw::json
static_assert( daw::json::from_json<Date>(
                 R"json({"timestamp":"2024-09-02T01:14:54Z"})json" )
                 .timestamp ==
               daw::json::datetime::civil_to_time_point( 2024, 9, 2, 1, 14, 54,
                                                         0 ) );

struct NullableDate {
	std::optional<timestamp_t> timestamp;
};

namespace daw::json {
	template<>
	struct json_data_contract<NullableDate> {
		static constexpr char const timestamp[] = "timestamp";
		using type =
		  json_member_list<json_date_null<timestamp, std::optional<timestamp_t>>>;

		static constexpr auto to_json_data( NullableDate const &d ) {
			return std::forward_as_tuple( d.timestamp );
		}
	};
} // namespace daw::json
static_assert(
  not daw::json::from_json<NullableDate>( R"json({})json" ).timestamp );

static_assert( daw::json::from_json<NullableDate>(
                 R"json({"timestamp":"2024-09-02T01:14:54Z"})json" )
                 .timestamp ==
               daw::json::datetime::civil_to_time_point( 2024, 9, 2, 1, 14, 54,
                                                         0 ) );

int main( ) {
#if defined( DAW_USE_EXCEPTIONS )
	{
		bool success = false;
		try {
			// Invalid month
			(void)daw::json::from_json<Date>(
			  R"json({"timestamp":"2024-15-02T01:14:54Z"})json" );
		} catch( std::exception const & ) { success = true; }
		daw_ensure( success );
	}
	{
		bool success = false;
		try {
			// Invalid month
			(void)daw::json::from_json<Date>(
			  R"json({"timestamp":"2024--12-02T01:14:54Z"})json" );
		} catch( std::exception const & ) { success = true; }
		daw_ensure( success );
	}
	{
		bool success = false;
		try {
			// Invalid day
			(void)daw::json::from_json<Date>(
			  R"json({"timestamp":"2024-12-42T01:14:54Z"})json" );
			success = false;
		} catch( std::exception const & ) { success = true; }
		daw_ensure( success );
	}
	{
		bool success = false;
		try {
			// Invalid day
			(void)daw::json::from_json<Date>(
			  R"json({"timestamp":"2024-12--12T01:14:54Z"})json" );
			success = false;
		} catch( std::exception const & ) { success = true; }
		daw_ensure( success );
	}
	{
		bool success = false;
		try {
			// Invalid hour
			(void)daw::json::from_json<Date>(
			  R"json({"timestamp":"2024-12-02T55:14:54Z"})json" );
			success = false;
		} catch( std::exception const & ) { success = true; }
		daw_ensure( success );
	}
	{
		bool success = false;
		try {
			// Invalid hour
			(void)daw::json::from_json<Date>(
			  R"json({"timestamp":"2024-12-02Tb5:14:54Z"})json" );
			success = false;
		} catch( std::exception const & ) { success = true; }
		daw_ensure( success );
	}
	{
		bool success = false;
		try {
			// Invalid minute
			(void)daw::json::from_json<Date>(
			  R"json({"timestamp":"2024-12-02T12:60:54Z"})json" );
			success = false;
		} catch( std::exception const & ) { success = true; }
		daw_ensure( success );
	}
	{
		bool success = false;
		try {
			// Invalid minute
			(void)daw::json::from_json<Date>(
			  R"json({"timestamp":"2024-12-02T05:-42:54Z"})json" );
			success = false;
		} catch( std::exception const & ) { success = true; }
		daw_ensure( success );
	}
	{
		bool success = false;
		try {
			// Invalid second
			(void)daw::json::from_json<Date>(
			  R"json({"timestamp":"2024-12-02T12:59:61Z"})json" );
			success = false;
		} catch( std::exception const & ) { success = true; }
		daw_ensure( success );
	}
	{
		bool success = false;
		try {
			// Invalid second
			(void)daw::json::from_json<Date>(
			  R"json({"timestamp":"2024-12-02T05:42:b4Z"})json" );
			success = false;
		} catch( std::exception const & ) { success = true; }
		daw_ensure( success );
	}
#endif
}
