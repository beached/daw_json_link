// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/daw_json_link
//

#include "daw_json_benchmark.h"

#include <daw/json/daw_json_iterator.h>
#include <daw/json/daw_json_link.h>
#include <daw/json/impl/daw_json_simd_iterator.h>

#include <daw/daw_ensure.h>

#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <iostream>
#include <string>
#include <string_view>

#if not defined( DAW_NUM_RUNS )
#if not defined( DEBUG ) or defined( NDEBUG )
static inline constexpr std::size_t DAW_NUM_RUNS = 100;
#else
static inline constexpr std::size_t DAW_NUM_RUNS = 2;
#endif
#endif
static_assert( DAW_NUM_RUNS > 0 );

#if defined( DAW_JSON_HAS_SIMD )

namespace {
	using namespace daw::json;
	using scalar_iterator =
	  json_array_iterator<double, options::CheckedParseMode::no,
	                      options::ExecModeTypes::compile_time>;
	template<std::size_t CacheBlocks>
	using simd_iterator = experimental::json_simd_number_block_iterator<
	  json_number_no_name<double>, CacheBlocks, char,
	  options::CheckedParseMode::no, options::ExecModeTypes::compile_time>;

	[[nodiscard]] std::string make_double_array( std::size_t element_count ) {
		auto result = std::string{ "[" };
		result.reserve( element_count * 14U + 1U );
		for( std::size_t n = 0; n < element_count; ++n ) {
			// Exercise signs, fractional parts, and exponents while keeping the input
			// deterministic from run to run.
			auto const whole = static_cast<long long>( n % 20'001U ) - 10'000LL;
			result += std::to_string( whole );
			result += '.';
			result += std::to_string( ( n * 97U ) % 1'000U );
			if( n % 8U == 0U ) {
				result += "e-2";
			}
			result += ',';
		}
		if( element_count == 0U ) {
			result += ']';
		} else {
			result.back( ) = ']';
		}
		return result;
	}

	[[nodiscard]] double sum_scalar( std::string_view document ) {
		auto result = 0.0;
		for( auto value : scalar_iterator( document ) ) {
			result += value;
		}
		daw::do_not_optimize( result );
		return result;
	}

	template<std::size_t CacheBlocks>
	[[nodiscard]] double sum_simd_blocks( std::string_view document ) {
		auto result = 0.0;
		for( auto const value : simd_iterator<CacheBlocks>( document ) ) {
			result += value;
		}
		daw::do_not_optimize( result );
		return result;
	}

	template<std::size_t CacheBlocks>
	void benchmark_simd_cache( std::string_view document, double expected,
	                           daw::string_view label ) {
		auto result = daw::json::benchmark::benchmark(
		  DAW_NUM_RUNS, document.size( ), label,
		  sum_simd_blocks<CacheBlocks>, document );
		daw_ensure( result.get( ) == expected );
	}
} // namespace

int main( int argc, char **argv ) {
	auto element_count = std::size_t{ 60'000UL };
	if( argc > 1 ) {
		element_count =
		  static_cast<std::size_t>( std::strtoull( argv[1], nullptr, 10 ) );
	}

	auto const json_data = make_double_array( element_count );
	auto const json_document = std::string_view( json_data );
	std::cout << "Computing expected sum: ";
	auto const expected = sum_scalar( json_document );
	std::cout << expected << "\nComputing SIMD sum: ";
	auto const simd_sum = sum_simd_blocks<2U>( json_document );
	std::cout << simd_sum << '\n' << std::flush;
	daw_ensure( simd_sum == expected );

	auto scalar_result = daw::json::benchmark::benchmark(
	  DAW_NUM_RUNS,
	  json_document.size( ),
	  "double array sum (json iterator, no SIMD)",
	  sum_scalar,
	  json_document );
	daw_ensure( scalar_result.get( ) == expected );

	benchmark_simd_cache<1U>( json_document, expected,
	                         "double array sum (SIMD, 1-block span cache)" );
	benchmark_simd_cache<2U>( json_document, expected,
	                         "double array sum (SIMD, 2-block span cache)" );
	benchmark_simd_cache<4U>( json_document, expected,
	                         "double array sum (SIMD, 4-block span cache)" );
	benchmark_simd_cache<8U>( json_document, expected,
	                         "double array sum (SIMD, 8-block span cache)" );
}

#else

int main( ) {
	std::cerr
	  << "This benchmark requires a standard library with <simd> support.\n";
	return EXIT_FAILURE;
}

#endif
