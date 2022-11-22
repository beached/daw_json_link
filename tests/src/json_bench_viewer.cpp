// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/daw_json_link
//

#include "bench_result.h"

#include <daw/daw_benchmark.h>
#include <daw/daw_memory_mapped_file.h>
#include <daw/json/daw_json_link.h>

#include <cassert>
#include <cstdlib>
#include <iostream>
#include <map>
#include <string>
#include <string_view>
#include <vector>

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

constexpr double to_fract( std::chrono::nanoseconds ns ) {
	using dest_t = std::chrono::duration<double>;
	return std::chrono::duration_cast<dest_t>( ns ).count( );
}

static void show_result( daw::bench::bench_result const &result ) {
	using timestamp_t = std::chrono::time_point<std::chrono::system_clock,
	                                            std::chrono::milliseconds>;
	auto const min_ts =
	  static_cast<double>(
	    std::min_element( result.run_times.begin( ), result.run_times.end( ) )
	      ->count( ) ) /
	  1'000'000'000.0;
	std::cout << "project:                  " << result.project_name << ' '
	          << result.project_subname << '\n';
	std::cout << "test name:                " << result.name << '\n';
	auto const date_str =
	  daw::json::to_json<daw::json::json_date_no_name<timestamp_t>>(
	    result.test_time );
	auto date_sv = daw::string_view( date_str );
	// trim quotes
	date_sv.remove_prefix( );
	date_sv.remove_suffix( );
	std::cout << "test date:                " << date_sv << '\n';
	std::cout << "os info:                  " << result.os_name << ' '
	          << result.os_platform << " version: " << result.os_version
	          << " release: " << result.os_release << '\n';
	std::cout << "git tag:                  " << result.git_revision << '\n';
	std::cout << "build type:               " << result.build_type << '\n';
	std::cout << "processor:                " << result.processor_description
	          << '\n';
	std::cout << "data size:                "
	          << daw::utility::to_bytes_per_second( result.data_size, 1.0, 2 )
	          << '\n';
	std::cout << "speed:                    "
	          << daw::utility::to_bytes_per_second(
	               static_cast<double>( result.data_size ) / min_ts, 1.0, 2 )
	          << "/s\n";
	std::cout << "runs/second:              " << std::fixed
	          << ( 1.0 / to_fract( result.duration_min ) ) << '\n';
	std::cout << "min duration:             " << result.duration_min << '\n';
	std::cout << "25th percentile duration: " << result.duration_25th_percentile
	          << '\n';
	std::cout << "50th percentile duration: " << result.duration_50th_percentile
	          << '\n';
	std::cout << "75th percentile duration: " << result.duration_75th_percentile
	          << '\n';
	std::cout << "max duration:             " << result.duration_max << '\n';
	std::cout << "build type:               " << result.build_type << '\n';
}

int main( int argc, char **argv ) {
	if( argc < 2 ) {
		std::cerr << "Must supply benchmark result file\n";
		std::exit( EXIT_FAILURE );
	}
	auto in_file = daw::filesystem::memory_mapped_file_t<char>( argv[1] );
	assert( in_file.size( ) > 2 );
	using namespace daw::json;
	auto results = from_json<std::vector<daw::bench::bench_result>>( in_file );

	std::map<std::string_view, daw::bench::bench_result> min_results{ };
	for( auto const &result : results ) {
		auto pos = min_results.find( result.name );
		if( pos == min_results.end( ) ) {
			min_results.insert( { result.name, result } );
			continue;
		}
		if( result.duration_min <= pos->second.duration_min ) {
			pos->second = result;
		}
	}
	std::cout << "Top results\n-----------------------------\n";
	for( auto const &result : min_results ) {
		show_result( result.second );
		std::cout << "\n\n";
	}
}
