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
#include <type_traits>

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

#if defined( DAW_HAS_INT128 )
template<typename Rep>
void test_wide_attosecond_duration( ) {
	if constexpr( std::is_arithmetic_v<Rep> ) {
		using wide_attoseconds = std::chrono::duration<Rep, std::atto>;
		using wide_timestamp_t =
		  std::chrono::time_point<std::chrono::system_clock, wide_attoseconds>;

		auto const parsed =
		  daw::json::from_json<daw::json::json_date_no_name<wide_timestamp_t>>(
		    R"json("1970-01-01T00:00:10.1234567890123456789Z")json" );
		auto const expected =
		  std::chrono::duration_cast<wide_attoseconds>(
		    std::chrono::seconds{ 10 } ) +
		  wide_attoseconds{ static_cast<Rep>( 123456789012345678LL ) };
		daw_ensure( parsed.time_since_epoch( ) == expected );

		auto const serialized =
		  daw::json::to_json<daw::json::json_date_no_name<wide_timestamp_t>>(
		    parsed );
		daw_ensure( serialized ==
		            R"json("1970-01-01T00:00:10.123456789012345678Z")json" );
	}
}
#endif

int main( ) {
#if defined( DAW_HAS_INT128 )
	test_wide_attosecond_duration<daw::int128_t>( );
#endif
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
		using nanosecond_timestamp_t =
		  std::chrono::time_point<std::chrono::system_clock,
		                          std::chrono::nanoseconds>;
		auto const parsed = daw::json::from_json<
		  daw::json::json_date_no_name<nanosecond_timestamp_t>>(
		  R"json("2024-12-02T05:42:04.1234567890Z")json" );
		daw_ensure(
		  parsed ==
		  daw::json::datetime::civil_to_time_point<nanosecond_timestamp_t>(
		    2024, 12, 2, 5, 42, 4, 123456789000000000ULL ) );
	}
	{
		using atto_timestamp_t =
		  std::chrono::time_point<std::chrono::system_clock,
		                          daw::json::datetime::attoseconds>;
		auto const parsed =
		  daw::json::from_json<daw::json::json_date_no_name<atto_timestamp_t>>(
		    R"json("1970-01-01T00:00:00.1234567890123456789Z")json" );
		daw_ensure( parsed.time_since_epoch( ).count( ) == 123456789012345678LL );
		auto const serialized =
		  daw::json::to_json<daw::json::json_date_no_name<atto_timestamp_t>>(
		    parsed );
		daw_ensure( serialized ==
		            R"json("1970-01-01T00:00:00.123456789012345678Z")json" );
		auto const one_millisecond =
		  atto_timestamp_t{ daw::json::datetime::attoseconds{ 1000000000000000LL } };
		auto const one_millisecond_serialized =
		  daw::json::to_json<daw::json::json_date_no_name<atto_timestamp_t>>(
		    one_millisecond );
		daw_ensure( one_millisecond_serialized ==
		            R"json("1970-01-01T00:00:00.001Z")json" );
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
