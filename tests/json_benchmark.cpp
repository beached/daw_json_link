// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/daw_json_link
//

#include "apache_builds.h"
#include "citm_test.h"
#include "geojson.h"
#include "twitter_test.h"

#include <daw/daw_benchmark.h>
#include <daw/daw_memory_mapped_file.h>
#include <daw/json/daw_json_link.h>

#include <cassert>
#include <chrono>
#include <cstddef>
#include <fstream>
#include <limits>
#include <string_view>

// These come from build system and must be defined
#ifndef SOURCE_CONTROL_REVISION
#error "SOURCE_CONTROL_REVSION must be defined"
#endif
#ifndef PROCESSOR_DESCRIPTION
#error "PROCESSOR_DESCRIPTION must be defined"
#endif
#ifndef OS_NAME
#error "OS_NAME must be defined"
#endif
#ifndef OS_RELEASE
#error "OS_RELEASE must be defined"
#endif
#ifndef OS_VERSION
#error "OS_VERSION must be defined"
#endif
#ifndef OS_PLATFORM
#error "OS_PLATFORM must be defined"
#endif
#ifndef BUILD_TYPE
#error "BUILD_TYPE must be defined"
#endif

#ifdef NDEBUG
inline constexpr std::size_t NUM_RUNS = 250;
#else
inline constexpr std::size_t NUM_RUNS = 25;
#endif


struct json_bench_result {
	std::string name = "ERROR";
	std::chrono::time_point<std::chrono::system_clock, std::chrono::milliseconds>
	  test_time{ };
	std::size_t data_size = 0;
	std::vector<long long> run_times{ };
	std::string avg_duration = "";
	std::string min_duration = "";
	std::string max_duration = "";
	std::string git_revision = SOURCE_CONTROL_REVISION;
	std::string processor_description = PROCESSOR_DESCRIPTION;
	std::string os_name = OS_NAME;
	std::string os_release = OS_RELEASE;
	std::string os_version = OS_VERSION;
	std::string os_platform = OS_PLATFORM;
	std::string build_type = BUILD_TYPE;
	std::string project_name = "daw_json_link";
	std::string project_subname = "json_benchmark";
};

namespace daw::json {
	template<>
	struct json_data_contract<json_bench_result> {
		static inline constexpr char const name[] = "name";
		static inline constexpr char const test_time[] = "test_time";
		static inline constexpr char const data_size[] = "data_size";
		static inline constexpr char const run_times[] = "run_times_ns";
		static inline constexpr char const avg_duration[] = "avg_duration";
		static inline constexpr char const min_duration[] = "min_duration";
		static inline constexpr char const max_duration[] = "max_duration";
		static inline constexpr char const git_revision[] = "git_revision";
		static inline constexpr char const processor_description[] =
		  "processor_description";
		static inline constexpr char const os_name[] = "os_name";
		static inline constexpr char const os_release[] = "os_release";
		static inline constexpr char const os_version[] = "os_version";
		static inline constexpr char const os_platform[] = "os_platform";
		static inline constexpr char const build_type[] = "build_type";
		static inline constexpr char const project_name[] = "project_name";
		static inline constexpr char const project_subname[] = "project_subname";
		using type =
		  json_member_list<json_string<name>, json_date<test_time>,
		                   json_number<data_size, std::size_t>,
		                   json_array<run_times, long long>,
		                   json_string<avg_duration>, json_string<min_duration>,
		                   json_string<max_duration>, json_string<git_revision>,
		                   json_string<processor_description>, json_string<os_name>,
		                   json_string<os_release>, json_string<os_version>,
		                   json_string<os_platform>, json_string<build_type>,
		                   json_string<project_name>, json_string<project_subname>>;

		[[nodiscard, maybe_unused]] static inline auto
		to_json_data( json_bench_result const &value ) {
			return std::tie(
			  value.name, value.test_time, value.data_size, value.run_times,
			  value.avg_duration, value.min_duration, value.max_duration,
			  value.git_revision, value.processor_description, value.os_name,
			  value.os_release, value.os_version, value.os_platform, value.build_type,
			  value.project_name, value.project_subname );
		}
	};
} // namespace daw::json

std::string format_ns( long long t, std::size_t prec = 0 ) {
	std::stringstream ss;
	ss << std::setprecision( static_cast<int>( prec ) ) << std::fixed;
	auto val = static_cast<double>( t );
	if( val < 1000 ) {
		ss << val << "ns";
		return ss.str( );
	}
	val /= 1000.0;
	if( val < 1000 ) {
		ss << val << "us";
		return ss.str( );
	}
	val /= 1000.0;
	if( val < 1000 ) {
		ss << val << "ms";
		return ss.str( );
	}
	val /= 1000.0;
	ss << val << "s";
	return ss.str( );
}

void process_results( json_bench_result &jr ) {
	long long sum = 0;
	long long min = std::numeric_limits<long long>::max( );
	long long max = std::numeric_limits<long long>::min( );
	for( long long d : jr.run_times ) {
		if( d < min ) {
			min = d;
		}
		if( d > max ) {
			max = d;
		}
		sum += d;
	}
	long long const avg = sum / static_cast<long long>( NUM_RUNS );
	jr.avg_duration = format_ns( avg, 2 );
	jr.min_duration = format_ns( min, 2 );
	jr.max_duration = format_ns( max, 2 );
}

json_bench_result
do_apache_builds_from_json_test( std::string_view json_data ) {
	auto result = json_bench_result{
	  "apache builds from_json",
	  std::chrono::time_point_cast<std::chrono::milliseconds>(
	    std::chrono::system_clock::now( ) ),
	  json_data.size( ),
	  daw::bench_n_test_silent<NUM_RUNS>(
	    []( auto const & ) { return true; },
	    [&]( std::string_view jd ) {
		    return daw::json::from_json<apache_builds::apache_builds>( jd );
	    },
	    json_data ) };

	process_results( result );
	return result;
}

json_bench_result do_twitter_from_json_test( std::string_view json_data ) {
	auto result = json_bench_result{
	  "twitter from_json",
	  std::chrono::time_point_cast<std::chrono::milliseconds>(
	    std::chrono::system_clock::now( ) ),
	  json_data.size( ),
	  daw::bench_n_test_silent<NUM_RUNS>(
	    []( auto const & ) { return true; },
	    [&]( std::string_view jd ) {
		    return daw::json::from_json<daw::twitter::twitter_object_t>( jd );
	    },
	    json_data ) };

	process_results( result );
	return result;
}

json_bench_result do_citm_from_json_test( std::string_view json_data ) {
	auto result = json_bench_result{
	  "citm_catalog from_json",
	  std::chrono::time_point_cast<std::chrono::milliseconds>(
	    std::chrono::system_clock::now( ) ),
	  json_data.size( ),
	  daw::bench_n_test_silent<NUM_RUNS>(
	    []( auto const & ) { return true; },
	    [&]( std::string_view jd ) {
		    return daw::json::from_json<daw::citm::citm_object_t>( jd );
	    },
	    json_data ) };

	process_results( result );
	return result;
}

json_bench_result do_canada_from_json_test( std::string_view json_data ) {
	auto result = json_bench_result{
	  "canada from_json",
	  std::chrono::time_point_cast<std::chrono::milliseconds>(
	    std::chrono::system_clock::now( ) ),
	  json_data.size( ),
	  daw::bench_n_test_silent<NUM_RUNS>(
	    []( auto const & ) { return true; },
	    [&]( std::string_view jd ) {
		    return daw::json::from_json<daw::geojson::FeatureCollection>( jd );
	    },
	    json_data ) };

	process_results( result );
	return result;
}

json_bench_result
do_nativejson_from_json_test( std::string_view json_data_twitter,
                              std::string_view json_data_citm,
                              std::string_view json_data_canada ) {
	auto result = json_bench_result{
	  "nativejson benchmark from_json",
	  std::chrono::time_point_cast<std::chrono::milliseconds>(
	    std::chrono::system_clock::now( ) ),
	  json_data_twitter.size( ) + json_data_citm.size( ) +
	    json_data_canada.size( ),
	  daw::bench_n_test_silent<NUM_RUNS>(
	    []( auto const & ) { return true; },
	    [&]( std::string_view tw, std::string_view ci, std::string_view ca ) {
		    return std::tuple{
		      daw::json::from_json<daw::twitter::twitter_object_t>( tw ),
		      daw::json::from_json<daw::citm::citm_object_t>( ci ),
		      daw::json::from_json<daw::geojson::FeatureCollection>( ca ) };
	    },
	    json_data_twitter, json_data_citm, json_data_canada ) };

	process_results( result );
	return result;
}

int main( int argc, char **argv ) {
	if( argc < 5 ) {
		std::cerr << "Must supply a path to apache_builds.json, twitter.json, "
		             "citm_catalog.json, and canada.json\n";
		exit( 1 );
	}
	auto const json_data_apache =
	  daw::filesystem::memory_mapped_file_t<>( argv[1] );
	assert( json_data_apache.size( ) > 2 and "Minimum json data size is 2 '{}'" );
	auto const json_data_twitter =
	  daw::filesystem::memory_mapped_file_t<>( argv[2] );
	assert( json_data_twitter.size( ) > 2 and
	        "Minimum json data size is 2 '{}'" );
	auto const json_data_citm =
	  daw::filesystem::memory_mapped_file_t<>( argv[3] );
	assert( json_data_citm.size( ) > 2 and "Minimum json data size is 2 '{}'" );
	auto const json_data_canada =
	  daw::filesystem::memory_mapped_file_t<>( argv[4] );
	assert( json_data_canada.size( ) > 2 and "Minimum json data size is 2 '{}'" );

	auto const results = std::vector<json_bench_result>{
	  do_apache_builds_from_json_test( json_data_apache ),
	  do_twitter_from_json_test( json_data_twitter ),
	  do_citm_from_json_test( json_data_citm ),
	  do_canada_from_json_test( json_data_canada ),
	  do_nativejson_from_json_test( json_data_twitter, json_data_citm,
	                                json_data_canada ) };

	if( argc < 6 ) {
		std::cout << daw::json::to_json_array( results ) << '\n';
		return EXIT_SUCCESS;
	}
	std::string out_data{ };
	{
		auto const json_data_results_file =
		  daw::filesystem::memory_mapped_file_t<>( argv[5] );
		auto old_results = [&]( ) -> std::vector<json_bench_result> {
			if( json_data_results_file.size( ) < 2U ) {
				return { };
			}
			return daw::json::from_json_array<json_bench_result>(
			  json_data_results_file );
		}( );
		old_results.insert( old_results.end( ), results.begin( ), results.end( ) );
		out_data = daw::json::to_json_array( old_results );
	}

	std::ofstream out_file( argv[5], std::ios::out | std::ios::trunc );
	assert( out_file );
	out_file.write( out_data.data( ),
	                static_cast<std::streamsize>( out_data.size( ) ) );
}
