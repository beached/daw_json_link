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
#include <daw/json/daw_json_simd_iterator.h>

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
	using signed_scalar_iterator = json_array_iterator<std::int64_t>;
	using signed_simd_iterator =
	  json_simd_block_iterator<json_number_no_name<std::int64_t>, char>;
	using unsigned_scalar_iterator = json_array_iterator<std::uint64_t>;
	using unsigned_simd_iterator =
	  json_simd_block_iterator<json_number_no_name<std::uint64_t>, char>;

	template<bool Signed>
	[[nodiscard]] std::string make_integer_array( std::size_t element_count ) {
		auto result = std::string{ "[" };
		result.reserve( element_count * 8U + 1U );
		for( std::size_t n = 0; n < element_count; ++n ) {
			auto const magnitude = ( n * 97U + n / 11U ) % 20'001U;
			if constexpr( Signed ) {
				auto const value = static_cast<std::int64_t>( magnitude ) - 10'000;
				result += std::to_string( value );
			} else {
				result += std::to_string( magnitude );
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

	[[nodiscard]] std::int64_t sum_signed_scalar( std::string_view document ) {
		auto result = std::int64_t{ 0 };
		for( auto const value : signed_scalar_iterator( document ) ) {
			result += value;
		}
		daw::do_not_optimize( result );
		return result;
	}

	[[nodiscard]] std::int64_t
	sum_signed_simd_blocks( std::string_view document ) {
		auto result = std::int64_t{ 0 };
		for( auto const value : signed_simd_iterator( document ) ) {
			result += value;
		}
		daw::do_not_optimize( result );
		return result;
	}

	[[nodiscard]] std::uint64_t sum_unsigned_scalar( std::string_view document ) {
		auto result = std::uint64_t{ 0 };
		for( auto const value : unsigned_scalar_iterator( document ) ) {
			result += value;
		}
		daw::do_not_optimize( result );
		return result;
	}

	[[nodiscard]] std::uint64_t
	sum_unsigned_simd_blocks( std::string_view document ) {
		auto result = std::uint64_t{ 0 };
		for( auto const value : unsigned_simd_iterator( document ) ) {
			result += value;
		}
		daw::do_not_optimize( result );
		return result;
	}
} // namespace

int main( int argc, char **argv ) {
	auto element_count = std::size_t{ 100'000U };
	if( argc > 1 ) {
		element_count =
		  static_cast<std::size_t>( std::strtoull( argv[1], nullptr, 10 ) );
	}

	auto const signed_json_data = make_integer_array<true>( element_count );
	auto const signed_document = std::string_view( signed_json_data );
	std::cout << "Computing expected signed sum: ";
	auto const expected_signed = sum_signed_scalar( signed_document );
	std::cout << expected_signed << "\nComputing SIMD signed sum: ";
	auto const simd_signed = sum_signed_simd_blocks( signed_document );
	std::cout << simd_signed << '\n' << std::flush;
	daw_ensure( simd_signed == expected_signed );

	auto signed_scalar_result = daw::json::benchmark::benchmark(
	  DAW_NUM_RUNS,
	  signed_document.size( ),
	  "signed integer array sum (json iterator, no SIMD)",
	  sum_signed_scalar,
	  signed_document );
	daw_ensure( signed_scalar_result.get( ) == expected_signed );

	auto signed_simd_result = daw::json::benchmark::benchmark(
	  DAW_NUM_RUNS,
	  signed_document.size( ),
	  "signed integer array sum (SIMD block iterator)",
	  sum_signed_simd_blocks,
	  signed_document );
	daw_ensure( signed_simd_result.get( ) == expected_signed );

	auto const unsigned_json_data = make_integer_array<false>( element_count );
	auto const unsigned_document = std::string_view( unsigned_json_data );
	std::cout << "Computing expected unsigned sum: ";
	auto const expected_unsigned = sum_unsigned_scalar( unsigned_document );
	std::cout << expected_unsigned << "\nComputing SIMD unsigned sum: ";
	auto const simd_unsigned = sum_unsigned_simd_blocks( unsigned_document );
	std::cout << simd_unsigned << '\n' << std::flush;
	daw_ensure( simd_unsigned == expected_unsigned );

	auto unsigned_scalar_result = daw::json::benchmark::benchmark(
	  DAW_NUM_RUNS,
	  unsigned_document.size( ),
	  "unsigned integer array sum (json iterator, no SIMD)",
	  sum_unsigned_scalar,
	  unsigned_document );
	daw_ensure( unsigned_scalar_result.get( ) == expected_unsigned );

	auto unsigned_simd_result = daw::json::benchmark::benchmark(
	  DAW_NUM_RUNS,
	  unsigned_document.size( ),
	  "unsigned integer array sum (SIMD block iterator)",
	  sum_unsigned_simd_blocks,
	  unsigned_document );
	daw_ensure( unsigned_simd_result.get( ) == expected_unsigned );
}

#else

int main( ) {
	std::cerr
	  << "This benchmark requires a standard library with <simd> support.\n";
	return EXIT_FAILURE;
}

#endif
