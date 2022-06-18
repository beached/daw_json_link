// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/daw_json_link
//

#pragma once

#include <daw/daw_do_not_optimize.h>
#include <daw/daw_expected.h>
#include <daw/daw_string_view.h>

#include <chrono>
#include <iomanip>
#include <iostream>

namespace daw::json::benchmark {
	template<typename Rep, typename Period>
	DAW_ATTRIB_NOINLINE [[nodiscard]] std::string
	ns_to_string( std::chrono::duration<Rep, Period> time,
	              std::size_t prec = 0 ) {
		std::stringstream ss;
		ss << std::setprecision( static_cast<int>( prec ) ) << std::fixed;
		using dsec_t = std::chrono::duration<double, std::ratio<1>>;
		auto val = std::chrono::duration_cast<dsec_t>( time ).count( ) *
		           1'000'000'000'000'000.0;
		if( val < 1000.0 ) {
			ss << val << "fs";
			return ss.str( );
		}
		val /= 1000.0;
		if( val < 1000 ) {
			ss << val << "ps";
			return ss.str( );
		}
		val /= 1000.0;
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

	template<typename Bytes>
	DAW_ATTRIB_NOINLINE std::string to_min_SI_unit_full( Bytes bytes,
	                                                     std::size_t prec = 1 ) {
		std::stringstream ss;
		ss << std::setprecision( static_cast<int>( prec ) ) << std::fixed;
		auto val = static_cast<double>( bytes );
		if( val < 1000.0 ) {
			ss << ( static_cast<double>( val * 100.0 ) / 100 );
			return ss.str( );
		}
		val /= 1000.0;
		if( val < 1000.0 ) {
			ss << ( static_cast<double>( val * 100.0 ) / 100 ) << "kilo";
			return ss.str( );
		}
		val /= 1000.0;
		if( val < 1000.0 ) {
			ss << ( static_cast<double>( val * 100.0 ) / 100 ) << "mega";
			return ss.str( );
		}
		val /= 1000.0;
		if( val < 1000.0 ) {
			ss << ( static_cast<double>( val * 100.0 ) / 100 ) << "giga";
			return ss.str( );
		}
		val /= 1000.0;
		if( val < 1000.0 ) {
			ss << ( static_cast<double>( val * 100.0 ) / 100 ) << "tera";
			return ss.str( );
		}
		val /= 1000.0;
		ss << ( static_cast<double>( val * 100.0 ) / 100 ) << "peta";
		return ss.str( );
	}

	template<typename Bytes>
	DAW_ATTRIB_NOINLINE std::string to_min_SI_unit( Bytes bytes,
	                                                std::size_t prec = 1 ) {
		std::stringstream ss;
		ss << std::setprecision( static_cast<int>( prec ) ) << std::fixed;
		auto val = static_cast<double>( bytes );
		if( val < 1000.0 ) {
			ss << ( static_cast<double>( val * 100.0 ) / 100 );
			return ss.str( );
		}
		val /= 1000.0;
		if( val < 1000.0 ) {
			ss << ( static_cast<double>( val * 100.0 ) / 100 ) << 'K';
			return ss.str( );
		}
		val /= 1000.0;
		if( val < 1000.0 ) {
			ss << ( static_cast<double>( val * 100.0 ) / 100 ) << 'M';
			return ss.str( );
		}
		val /= 1000.0;
		if( val < 1000.0 ) {
			ss << ( static_cast<double>( val * 100.0 ) / 100 ) << 'G';
			return ss.str( );
		}
		val /= 1000.0;
		if( val < 1000.0 ) {
			ss << ( static_cast<double>( val * 100.0 ) / 100 ) << 'T';
			return ss.str( );
		}
		val /= 1000.0;
		ss << ( static_cast<double>( val * 100.0 ) / 100 ) << 'P';
		return ss.str( );
	}

	template<typename Func, typename... Args>
	DAW_ATTRIB_NOINLINE daw::expected_t<std::invoke_result_t<Func, Args...>>
	benchmark( std::size_t min_num_runs, std::size_t data_size,
	           daw::string_view title, Func &&func, Args const &...args ) {
		if( min_num_runs % 2 == 1 ) {
			++min_num_runs;
		}
		if( min_num_runs < 2 ) {
			min_num_runs = 2;
		}

		using ns_duration_t = std::chrono::nanoseconds;
		using namespace std::chrono_literals;
		using func_result_t = std::invoke_result_t<Func, Args...>;
		auto result = daw::expected_t<func_result_t>( );

		ns_duration_t min_duration = 4'611'686'018'427'387'904ns;
		ns_duration_t max_duration = 0ns;
		auto const base_start = std::chrono::steady_clock::now( );
		for( std::size_t n = 0; n < min_num_runs; ++n ) {
			auto const run_start = std::chrono::steady_clock::now( );
			daw::do_not_optimize( args... );
#if defined( DAW_USE_EXCEPTIONS )
			try {
#endif
				(void)func( args... );
#if defined( DAW_USE_EXCEPTIONS )
			} catch( ... ) {
				std::cerr << "Error during benchmark run: " << title << '\n';
				result.set_exception( );
				return result;
			}
#endif
			auto const run_finish = std::chrono::steady_clock::now( );
			auto const run_duration = run_finish - run_start;
			if( run_duration < min_duration ) {
				min_duration = run_duration;
			}
			if( run_duration > max_duration ) {
				max_duration = run_duration;
			}
		}
		auto const base_finish = std::chrono::steady_clock::now( );

		daw::do_not_optimize( min_duration );
		min_duration = 4'611'686'018'427'387'904ns;
		daw::do_not_optimize( max_duration );
		max_duration = 0ns;
		//*******************************
		auto const full_start = std::chrono::steady_clock::now( );
		for( std::size_t n = 0; n < min_num_runs * 2; ++n ) {
			auto const run_start = std::chrono::steady_clock::now( );
			daw::do_not_optimize( func );
			daw::do_not_optimize( args... );
#if defined( DAW_USE_EXCEPTIONS )
			try {
#endif
				(void)func( args... );
#if defined( DAW_USE_EXCEPTIONS )
			} catch( ... ) {}
#endif
			auto const run_finish = std::chrono::steady_clock::now( );
			auto const run_duration = run_finish - run_start;
			if( run_duration < min_duration ) {
				min_duration = run_duration;
			}
			if( run_duration > max_duration ) {
				max_duration = run_duration;
			}
		}
		auto const full_finish = std::chrono::steady_clock::now( );

		auto const base_duration = base_finish - base_start;
		auto const run_duration = ( full_finish - full_start ) - base_duration;
		auto const avg_duration = run_duration / min_num_runs;
		using dsec_t = std::chrono::duration<double, std::ratio<1>>;
		auto const max_per_second = static_cast<std::uint64_t>(
		  static_cast<double>( min_num_runs ) /
		  std::chrono::duration_cast<dsec_t>( min_duration ).count( ) );
		auto const max_throughput =
		  static_cast<double>( data_size ) /
		  std::chrono::duration_cast<dsec_t>( min_duration ).count( );
		auto const min_throughput =
		  static_cast<double>( data_size ) /
		  std::chrono::duration_cast<dsec_t>( max_duration ).count( );
		auto const min_per_second = static_cast<std::uint64_t>(
		  static_cast<double>( min_num_runs ) /
		  std::chrono::duration_cast<dsec_t>( max_duration ).count( ) );
		auto const avg_throughput =
		  static_cast<double>( data_size ) /
		  std::chrono::duration_cast<dsec_t>( avg_duration ).count( );
		auto const avg_per_second = static_cast<std::uint64_t>(
		  static_cast<double>( min_num_runs ) /
		  std::chrono::duration_cast<dsec_t>( avg_duration ).count( ) );

		std::cout << "Benchmark Run: " << title << '\n';
		std::cout << "min time: " << ns_to_string( min_duration, 2 )
		          << "\tthroughput: " << to_min_SI_unit( max_throughput, 2 )
		          << "B/s\titems/s: " << to_min_SI_unit_full( max_per_second, 2 )
		          << " items/s\n";
		std::cout << "avg time: " << ns_to_string( avg_duration, 2 )
		          << "\tthroughput: " << to_min_SI_unit( avg_throughput, 2 )
		          << "B/s\titems/s: " << to_min_SI_unit_full( avg_per_second, 2 )
		          << " items/s\n";
		std::cout << "max time: " << ns_to_string( max_duration, 2 )
		          << "\tthroughput: " << to_min_SI_unit( min_throughput, 2 )
		          << "B/s\titems/s: " << to_min_SI_unit_full( min_per_second )
		          << " items/s\n";
		std::cout << "total time: " << ns_to_string( run_duration, 2 )
		          << "\tdata size: " << to_min_SI_unit( data_size )
		          << "B\tnumber of runs: " << min_num_runs << "\n\n";

#if defined( DAW_USE_EXCEPTIONS )
		try {
#endif
			if constexpr( std::is_same_v<func_result_t, void> ) {
				func( args... );
				result.set_value( );
			} else {
				result.set_value( func( args... ) );
			}
#if defined( DAW_USE_EXCEPTIONS )
		} catch( ... ) {
			std::cerr << "Error during benchmark run: " << title << '\n';
			result.set_exception( std::current_exception( ) );
			return result;
		}
#endif
		return result;
	}

} // namespace daw::json::benchmark
