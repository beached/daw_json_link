// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/daw_json_link
//

#include "apache_builds.h"
#include "bench_result.h"
#include "citm_test.h"
#include "geojson.h"
#include "twitter_test.h"

#include <daw/daw_benchmark.h>
#include <daw/daw_memory_mapped_file.h>
#include <daw/daw_utility.h>
#include <daw/json/daw_json_link.h>

#include <cassert>
#include <chrono>
#include <cstddef>
#include <fstream>
#include <ios>
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

namespace {
#ifdef NDEBUG
	inline constexpr std::size_t NUM_RUNS = 250;
#else
	inline constexpr std::size_t NUM_RUNS = 25;
#endif
	static_assert( NUM_RUNS > 1 );

	daw::bench::bench_result
	make_bench_result( std::string const &name, std::size_t data_size,
	                   std::vector<std::chrono::nanoseconds> run_times ) {
		return { std::move( name ),
		         std::chrono::time_point_cast<std::chrono::milliseconds>(
		           std::chrono::system_clock::now( ) ),
		         data_size,
		         std::move( run_times ),
		         { },
		         { },
		         { },
		         { },
		         { },
		         SOURCE_CONTROL_REVISION,
		         PROCESSOR_DESCRIPTION,
		         OS_NAME,
		         OS_RELEASE,
		         OS_VERSION,
		         OS_PLATFORM,
		         BUILD_TYPE,
		         "daw_json_link",
		         "json_benchmark" };
	}

	std::ostream &operator<<( std::ostream &os, std::chrono::nanoseconds t ) {
		auto const ae = daw::on_scope_exit(
		  [&os, old_flags = std::ios_base::fmtflags( os.flags( ) )] {
			  os.flags( old_flags );
		  } );

		os << std::setprecision( static_cast<int>( 2 ) ) << std::fixed;
		auto val = static_cast<double>( t.count( ) );
		if( val < 1000 ) {
			os << val << "ns";
			return os;
		}
		val /= 1000.0;
		if( val < 1000 ) {
			os << val << "us";
			return os;
		}
		val /= 1000.0;
		if( val < 1000 ) {
			os << val << "ms";
			return os;
		}
		val /= 1000.0;
		os << val << "s";
		return os;
	}

	void show_result( daw::bench::bench_result const &result ) {
		std::cout << "test name:" << result.name << '\n';
		std::cout << "data size:"
		          << daw::utility::to_bytes_per_second( result.data_size, 1.0, 2 )
		          << '\n';
		std::cout << "min duration: " << result.duration_min << '\n';
		std::cout << "25th percentile duration: " << result.duration_25th_percentile
		          << '\n';
		std::cout << "50th percentile duration: " << result.duration_50th_percentile
		          << '\n';
		std::cout << "75th percentile duration: " << result.duration_75th_percentile
		          << '\n';
		std::cout << "max duration: " << result.duration_max << '\n';
		std::cout << "build type: " << result.build_type << '\n';
	}

	void process_results( daw::bench::bench_result &jr ) {

		auto runs = jr.run_times;
		std::sort( runs.begin( ), runs.end( ) );
		std::size_t const bin_25 = runs.size( ) / 4U;
		std::size_t const bin_50 = 2 * ( runs.size( ) / 4U );
		std::size_t const bin_75 = ( runs.size( ) - 1U ) - bin_50;
		jr.duration_min = runs.front( );
		jr.duration_max = runs.back( );
		jr.duration_25th_percentile = runs[bin_25];
		jr.duration_50th_percentile = runs[bin_50];
		jr.duration_75th_percentile = runs[bin_75];
	}

	daw::bench::bench_result
	do_apache_builds_from_json_test( std::string_view json_data ) {
		auto result = make_bench_result(
		  "apache builds from_json", json_data.size( ),
		  daw::bench_n_test_json<NUM_RUNS>(
		    [&]( std::string_view jd ) {
			    return daw::json::from_json<
			      apache_builds::apache_builds,
			      daw::json::NoCommentSkippingPolicyUnchecked>( jd );
		    },
		    json_data ) );

		process_results( result );
		return result;
	}

	daw::bench::bench_result
	do_twitter_from_json_test( std::string_view json_data ) {
		auto result = make_bench_result(
		  "twitter from_json", json_data.size( ),
		  daw::bench_n_test_json<NUM_RUNS>(
		    [&]( std::string_view jd ) {
			    return daw::json::from_json<
			      daw::twitter::twitter_object_t,
			      daw::json::NoCommentSkippingPolicyUnchecked>( jd );
		    },
		    json_data ) );

		process_results( result );
		return result;
	}

	daw::bench::bench_result
	do_citm_from_json_test( std::string_view json_data ) {
		auto result = make_bench_result(
		  "citm_catalog from_json", json_data.size( ),
		  daw::bench_n_test_json<NUM_RUNS>(
		    [&]( std::string_view jd ) {
			    return daw::json::from_json<
			      daw::citm::citm_object_t,
			      daw::json::NoCommentSkippingPolicyUnchecked>( jd );
		    },
		    json_data ) );

		process_results( result );
		return result;
	}

	daw::bench::bench_result
	do_canada_from_json_test( std::string_view json_data ) {
		auto result = make_bench_result(
		  "canada from_json", json_data.size( ),
		  daw::bench_n_test_json<NUM_RUNS>(
		    [&]( std::string_view jd ) {
			    return daw::json::from_json<
			      daw::geojson::FeatureCollection,
			      daw::json::NoCommentSkippingPolicyUnchecked>( jd );
		    },
		    json_data ) );

		process_results( result );
		return result;
	}

	daw::bench::bench_result
	do_nativejson_from_json_test( std::string_view json_data_twitter,
	                              std::string_view json_data_citm,
	                              std::string_view json_data_canada ) {
		auto result = make_bench_result(
		  "nativejson benchmark from_json",
		  json_data_twitter.size( ) + json_data_citm.size( ) +
		    json_data_canada.size( ),
		  daw::bench_n_test_json<NUM_RUNS>(
		    [&]( std::string_view tw, std::string_view ci, std::string_view ca ) {
			    auto const j1 =
			      daw::json::from_json<daw::twitter::twitter_object_t>( tw );
			    auto const j2 = daw::json::from_json<daw::citm::citm_object_t>( ci );
			    auto const j3 =
			      daw::json::from_json<daw::geojson::FeatureCollection>( ca );
			    daw::do_not_optimize( j1 );
			    daw::do_not_optimize( j2 );
			    daw::do_not_optimize( j3 );
		    },
		    json_data_twitter, json_data_citm, json_data_canada ) );

		process_results( result );
		return result;
	}

	daw::bench::bench_result
	do_nativejson_from_json_test_unchecked( std::string_view json_data_twitter,
	                                        std::string_view json_data_citm,
	                                        std::string_view json_data_canada ) {
		auto result = make_bench_result(
		  "nativejson unchecked benchmark from_json",
		  json_data_twitter.size( ) + json_data_citm.size( ) +
		    json_data_canada.size( ),
		  daw::bench_n_test_json<NUM_RUNS>(
		    [&]( std::string_view tw, std::string_view ci, std::string_view ca ) {
			    auto const j1 =
			      daw::json::from_json<daw::twitter::twitter_object_t,
			                           daw::json::NoCommentSkippingPolicyUnchecked>(
			        tw );
			    auto const j2 =
			      daw::json::from_json<daw::citm::citm_object_t,
			                           daw::json::NoCommentSkippingPolicyUnchecked>(
			        ci );
			    auto const j3 =
			      daw::json::from_json<daw::geojson::FeatureCollection,
			                           daw::json::NoCommentSkippingPolicyUnchecked>(
			        ca );
			    daw::do_not_optimize( j1 );
			    daw::do_not_optimize( j2 );
			    daw::do_not_optimize( j3 );
		    },
		    json_data_twitter, json_data_citm, json_data_canada ) );

		process_results( result );
		return result;
	}
} // namespace

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

	auto const results = std::vector<daw::bench::bench_result>{
	  do_apache_builds_from_json_test( json_data_apache ),
	  do_twitter_from_json_test( json_data_twitter ),
	  do_citm_from_json_test( json_data_citm ),
	  do_canada_from_json_test( json_data_canada ),
	  do_nativejson_from_json_test_unchecked( json_data_twitter, json_data_citm,
	                                          json_data_canada ),
	  do_nativejson_from_json_test( json_data_twitter, json_data_citm,
	                                json_data_canada ) };

	if( argc < 6 ) {
		for( auto const &r : results ) {
			show_result( r );
			std::cout << '\n';
			std::cout << '\n';
		}
		// std::cout << daw::json::to_json_array( results ) << '\n';
		return EXIT_SUCCESS;
	}
	std::string out_data{ };
	{
		auto const json_data_results_file =
		  daw::filesystem::memory_mapped_file_t<>( argv[5] );
		auto old_results = [&]( ) -> std::vector<daw::bench::bench_result> {
			if( json_data_results_file.size( ) < 2U ) {
				return { };
			}
			return daw::json::from_json_array<daw::bench::bench_result>(
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
