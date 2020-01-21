// The MIT License (MIT)
//
// Copyright (c) 2020 Darrell Wright
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

#pragma once

#include <daw/daw_string_view.h>

#include <chrono>
#include <cstdint>

namespace daw::json::json_details {
	template<typename Result, typename CharT>
	constexpr Result to_integer( CharT const c ) {
		return static_cast<Result>( c - static_cast<CharT>( '0' ) );
	}

	template<typename Result, size_t count, typename CharT>
	constexpr Result parse_unsigned( const CharT *digit_str ) {
		Result result = 0;
		for( size_t n = 0; n < count; ++n ) {
			result = static_cast<Result>( ( result << 1 ) + ( result << 3 ) ) +
			         to_integer<Result>( digit_str[n] );
		}
		return result;
	}

	template<typename CharT>
	constexpr bool is_number( CharT c ) {
		unsigned const dig = static_cast<unsigned>( c - static_cast<CharT>( '0' ) );
		return dig < 10U;
	}

	template<typename Result, typename CharT, typename Traits>
	constexpr Result parse_number( daw::basic_string_view<CharT, Traits> sv ) {
		static_assert( std::numeric_limits<Result>::digits10 >= 4 );
		if( sv.empty( ) ) {
			daw_json_error( "Invalid number" );
		}
		Result result = 0;
		Result sign = 1;
		if( sv.front( ) == '-' ) {
			if constexpr( std::is_signed_v<Result> ) {
				sign = -1;
			}
			sv.remove_prefix( );
		} else if( sv.front( ) == static_cast<CharT>( '+' ) ) {
			sv.remove_prefix( );
		}
		while( not sv.empty( ) ) {
			unsigned const dig =
			  static_cast<unsigned>( sv.pop_front( ) - static_cast<CharT>( '0' ) );
			daw_json_assert( dig < 10U, "Invalid digit" );
			result *= 10;
			result += static_cast<Result>( dig );
		}
		return result * sign;
	}

	// See:
	// https://stackoverflow.com/questions/16773285/how-to-convert-stdchronotime-point-to-stdtm-without-using-time-t
	template<typename Clock = std::chrono::system_clock,
	         typename Duration = std::chrono::milliseconds>
	constexpr std::chrono::time_point<Clock, Duration>
	civil_to_time_point( int_least32_t yr, uint_least32_t mo, uint_least32_t dy,
	                     uint_least32_t hr, uint_least32_t mn, uint_least32_t se,
	                     uint_least32_t ms ) {
		constexpr auto calc =
		  []( int_least32_t y, uint_least32_t m, uint_least32_t d, uint_least32_t h,
		      uint_least32_t min, uint_least32_t s, uint_least32_t mil ) {
			  y -= static_cast<int_least32_t>( m ) <= 2;
			  int_least32_t const era = ( y >= 0 ? y : y - 399 ) / 400;
			  uint_least32_t const yoe = static_cast<uint_least32_t>(
			    static_cast<int_least32_t>( y ) - era * 400 ); // [0, 399]
			  uint_least32_t const doy = static_cast<uint_least32_t>(
			    ( 153 * ( static_cast<int_least32_t>( m ) +
			              ( static_cast<int_least32_t>( m ) > 2 ? -3 : 9 ) ) +
			      2 ) /
			      5 +
			    static_cast<int_least32_t>( d ) - 1 ); // [0, 365]
			  uint_least32_t const doe =
			    yoe * 365 + yoe / 4 - yoe / 100 + doy; // [0, 146096]
			  int_least32_t const days_since_epoch =
			    era * 146097 + static_cast<int_least32_t>( doe ) - 719468;

			  using Days = std::chrono::duration<int_least32_t, std::ratio<86400>>;
			  return std::chrono::time_point<std::chrono::system_clock,
			                                 std::chrono::milliseconds>{} +
			         ( Days( days_since_epoch ) + std::chrono::hours( h ) +
			           std::chrono::minutes( min ) +
			           std::chrono::seconds( static_cast<uint_least32_t>( s ) ) +
			           std::chrono::milliseconds( mil ) );
		  };
		// Not all clocks have the same epoch.  This should account for the offset
		// and adjust the time_point so that the days prior are in relation to unix
		// epoch.  If system_clock is used, as is the default for the return value,
		// it will be zero and should be removed by the compiler
		constexpr auto system_epoch = std::chrono::time_point<Clock, Duration>{};
		constexpr auto unix_epoch = calc( 1970, 1, 1, 0, 0, 0, 0 );
		constexpr auto offset = unix_epoch - system_epoch;
		return calc( yr, mo, dy, hr, mn, se, ms ) + offset;
	}

	struct date_parts {
		int_least32_t year;
		uint_least32_t month;
		uint_least32_t day;
	};

	template<typename CharT, typename Traits>
	constexpr date_parts
	parse_iso_8601_date( daw::basic_string_view<CharT, Traits> timestamp_str ) {
		auto result = date_parts{0, 0, 0};
		result.day =
		  parse_unsigned<uint_least32_t, 2>( timestamp_str.pop_back( 2U ).data( ) );
		if( not is_number( timestamp_str.back( ) ) ) {
			timestamp_str.remove_suffix( );
		}
		result.month =
		  parse_unsigned<uint_least32_t, 2>( timestamp_str.pop_back( 2U ).data( ) );
		if( not is_number( timestamp_str.back( ) ) ) {
			timestamp_str.remove_suffix( );
		}
		result.year = parse_number<int_least32_t>( timestamp_str );
		return result;
	}

	struct time_parts {
		uint_least32_t hour;
		uint_least32_t minute;
		uint_least32_t second;
		uint_least32_t millisecond;
	};

	template<typename CharT, typename Traits>
	constexpr time_parts
	parse_iso_8601_time( daw::basic_string_view<CharT, Traits> timestamp_str ) {
		auto result = time_parts{0, 0, 0, 0};
		result.hour =
		  parse_unsigned<uint_least32_t, 2>( timestamp_str.pop_front( 2 ).data( ) );
		if( not is_number( timestamp_str.front( ) ) ) {
			timestamp_str.remove_prefix( );
		}
		result.minute =
		  parse_unsigned<uint_least32_t, 2>( timestamp_str.pop_front( 2 ).data( ) );
		if( timestamp_str.empty( ) ) {
			return result;
		}
		if( not is_number( timestamp_str.front( ) ) ) {
			timestamp_str.remove_prefix( );
		}
		result.second =
		  parse_unsigned<uint_least32_t, 2>( timestamp_str.pop_front( 2 ).data( ) );
		if( timestamp_str.empty( ) ) {
			return result;
		}
		if( not is_number( timestamp_str.front( ) ) ) {
			timestamp_str.remove_prefix( );
		}
		result.millisecond =
		  parse_number<uint_least32_t>( timestamp_str.pop_front( 3 ) );
		return result;
	}

	template<typename CharT, typename Traits>
	constexpr std::chrono::time_point<std::chrono::system_clock,
	                                  std::chrono::milliseconds>
	parse_iso8601_timestamp( daw::basic_string_view<CharT, Traits> ts ) {
		constexpr CharT t_str[2] = {static_cast<CharT>( 'T' ), 0};
		auto const date_str = ts.pop_front( t_str );
		if( ts.empty( ) ) {
			daw_json_error( "Invalid timestamp, missing T separator" );
		}
		date_parts const ymd = parse_iso_8601_date( date_str );
		auto time_str = ts.pop_front( []( CharT c ) {
			return not( is_number( c ) or c == static_cast<CharT>( ':' ) or
			            c == static_cast<CharT>( '.' ) );
		} );
		// TODO: verify or parse timezone
		time_parts hms = parse_iso_8601_time( time_str );
		if( not( ts.empty( ) or ts.front( ) == static_cast<CharT>( 'Z' ) ) ) {
			daw_json_assert( ts.size( ) == 5 or ts.size( ) == 6,
			                 "Invalid timezone offset" );
			// The format will be (+|-)hh[:]mm
			bool const sign = ts.front( ) == static_cast<CharT>( '+' );
			ts.remove_prefix( );
			auto hr_offset = parse_unsigned<uint_least32_t, 2>( ts.data( ) );
			if( ts.front( ) == static_cast<CharT>( ':' ) ) {
				ts.remove_prefix( );
			}
			auto mn_offset = parse_unsigned<uint_least32_t, 2>( ts.data( ) );
			// Want to subtract offset from current time, we are converting to UTC
			if( sign ) {
				// Positive offset
				hms.hour -= hr_offset;
				hms.minute -= mn_offset;
			} else {
				// Negative offset
				hms.hour += hr_offset;
				hms.minute += mn_offset;
			}
		}
		return civil_to_time_point( ymd.year, ymd.month, ymd.day, hms.hour,
		                            hms.minute, hms.second, hms.millisecond );
	}
	struct ymdhms {
		int_least32_t year;
		uint_least32_t month;
		uint_least32_t day;
		uint_least32_t hour;
		uint_least32_t minute;
		uint_least32_t second;
		uint_least32_t millisecond;
	};

	template<typename Clock, typename Duration>
	constexpr ymdhms time_point_to_civil(
	  std::chrono::time_point<Clock, Duration> const &tp ) noexcept {
		auto dur_from_epoch = tp.time_since_epoch( );
		using Days = std::chrono::duration<int_least32_t, std::ratio<86400>>;
		auto const days_since_epoch =
		  std::chrono::duration_cast<Days>( dur_from_epoch );
		int_least32_t z = days_since_epoch.count( );
		z += 719468;
		int_least32_t const era = ( z >= 0 ? z : z - 146096 ) / 146097;
		uint_least32_t const doe =
		  static_cast<uint_least32_t>( z - era * 146097 ); // [0, 146096]
		uint_least32_t const yoe =
		  ( doe - doe / 1460 + doe / 36524 - doe / 146096 ) / 365; // [0, 399]
		int_least32_t const y = static_cast<int_least32_t>( yoe ) + era * 400;
		uint_least32_t const doy =
		  doe - ( 365 * yoe + yoe / 4 - yoe / 100 );             // [0, 365]
		uint_least32_t const mp = ( 5 * doy + 2 ) / 153;         // [0, 11]
		uint_least32_t const d = doy - ( 153 * mp + 2 ) / 5 + 1; // [1, 31]
		uint_least32_t const m = static_cast<uint_least32_t>(
		  static_cast<int_least32_t>( mp ) +
		  ( static_cast<int_least32_t>( mp ) < 10 ? 3 : -9 ) ); // [1, 12]

		dur_from_epoch -= days_since_epoch;
		auto const hrs =
		  std::chrono::duration_cast<std::chrono::hours>( dur_from_epoch );
		dur_from_epoch -= hrs;
		auto const min =
		  std::chrono::duration_cast<std::chrono::minutes>( dur_from_epoch );
		dur_from_epoch -= min;
		auto const sec =
		  std::chrono::duration_cast<std::chrono::seconds>( dur_from_epoch );
		dur_from_epoch -= sec;
		auto const ms =
		  std::chrono::duration_cast<std::chrono::milliseconds>( dur_from_epoch );
		return ymdhms{y + ( m <= 2 ),
		              m,
		              d,
		              static_cast<uint_least32_t>( hrs.count( ) ),
		              static_cast<uint_least32_t>( min.count( ) ),
		              static_cast<uint_least32_t>( sec.count( ) ),
		              static_cast<uint_least32_t>( ms.count( ) )};
	}
} // namespace daw::json::json_details
