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
	  json_array_iterator<bool, options::CheckedParseMode::yes,
	                      options::ExecModeTypes::compile_time>;
	using simd_iterator =
	  json_simd_block_iterator<json_bool_no_name<bool>, char,
	                           options::CheckedParseMode::yes,
	                           options::ExecModeTypes::compile_time>;

	[[nodiscard]] std::string make_bool_array( std::size_t element_count ) {
		auto result = std::string{ "[" };
		result.reserve( element_count * 6U + 1U );
		for( std::size_t n = 0; n < element_count; ++n ) {
			// Use a deterministic, non-alternating pattern so both literals occur at
			// different alignments within SIMD blocks.
			result += ( ( n * 17U + n / 7U ) % 5U < 2U ) ? "true," : "false,";
		}
		if( element_count == 0U ) {
			result += ']';
		} else {
			result.back( ) = ']';
		}
		return result;
	}

	[[nodiscard]] std::size_t count_scalar( std::string_view document ) {
		auto result = std::size_t{ 0 };
		for( auto value : scalar_iterator( document ) ) {
			result += static_cast<std::size_t>( value );
		}
		daw::do_not_optimize( result );
		return result;
	}

	[[nodiscard]] std::size_t count_simd_blocks( std::string_view document ) {
		auto result = std::size_t{ 0 };
		for( auto value : simd_iterator( document ) ) {
			result += static_cast<std::size_t>( value );
		}
		daw::do_not_optimize( result );
		return result;
	}
} // namespace

int main( int argc, char **argv ) {
	auto element_count = std::size_t{ 100'000 };
	if( argc > 1 ) {
		element_count =
		  static_cast<std::size_t>( std::strtoull( argv[1], nullptr, 10 ) );
	}

	auto const json_data = make_bool_array( element_count );
	auto const json_document = std::string_view( json_data );
	std::cout << "Computing expected true count: ";
	auto const expected = count_scalar( json_document );
	std::cout << expected << "\nComputing SIMD true count: ";
	auto const simd_count = count_simd_blocks( json_document );
	std::cout << simd_count << '\n' << std::flush;
	daw_ensure( simd_count == expected );

	auto scalar_result = daw::json::benchmark::benchmark(
	  DAW_NUM_RUNS,
	  json_document.size( ),
	  "boolean array count (json iterator, no SIMD)",
	  count_scalar,
	  json_document );
	daw_ensure( scalar_result.get( ) == expected );

	auto simd_result = daw::json::benchmark::benchmark(
	  DAW_NUM_RUNS,
	  json_document.size( ),
	  "boolean array count (SIMD block iterator)",
	  count_simd_blocks,
	  json_document );
	daw_ensure( simd_result.get( ) == expected );
}

#else

int main( ) {
	std::cerr
	  << "This benchmark requires a standard library with <simd> support.\n";
	return EXIT_FAILURE;
}

#endif
