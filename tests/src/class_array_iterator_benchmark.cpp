// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/daw_json_link
//

#include "daw_json_benchmark.h"

#include <daw/daw_random.h>
#include <daw/json/daw_json_iterator.h>
#include <daw/json/daw_json_link.h>
#include <daw/json/impl/daw_json_simd_iterator.h>

#include <daw/daw_ensure.h>

#include <algorithm>
#include <cstddef>
#include <cmath>
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

struct point_t {
	double x;
	double y;
	double z;
};

namespace daw::json {
	template<>
	struct json_data_contract<point_t> {
		static constexpr char x[] = "x";
		static constexpr char y[] = "y";
		static constexpr char z[] = "z";
		using type =
		  json_member_list<json_number<x>, json_number<y>, json_number<z>>;

		static constexpr auto to_json_data( point_t const &p ) {
			return std::forward_as_tuple( p.x, p.y, p.z );
		}
	};
} // namespace daw::json

namespace {
	using point_json = daw::json::json_class_no_name<point_t>;
	using scalar_iterator = daw::json::json_array_iterator<point_json>;
	using simd_iterator = daw::json::json_simd_block_iterator<point_json, char>;

	[[nodiscard]] std::string make_point_array( std::size_t element_count ) {
		auto v = std::vector<point_t>{ };
		v.resize( element_count );
		for( std::size_t n = 0; n < element_count; ++n ) {}
		auto rnd = daw::RandomFloat<double>{ };
		for( std::size_t n = 0; n < element_count; ++n ) {
			v[n].x = rnd( );
			v[n].y = rnd( );
			v[n].z = rnd( );
		}
		return daw::json::to_json(
		  v,
		  daw::json::options::output_flags<daw::json::options::SerializationFormat::Pretty> );
	}

	template<typename Iterator>
	[[nodiscard]] point_t average_points( std::string_view document ) {
		auto result = point_t{ };
		auto count = std::size_t{ 0 };
		for( auto const point : Iterator( document ) ) {
			result.x += point.x;
			result.y += point.y;
			result.z += point.z;
			++count;
		}
		daw_ensure( count > 0U );
		auto const divisor = static_cast<double>( count );
		result.x /= divisor;
		result.y /= divisor;
		result.z /= divisor;
		daw::do_not_optimize( result );
		return result;
	}

	[[nodiscard]] point_t average_scalar( std::string_view document ) {
		return average_points<scalar_iterator>( document );
	}

	[[nodiscard]] point_t average_simd_blocks( std::string_view document ) {
		return average_points<simd_iterator>( document );
	}

	[[nodiscard]] bool equal( point_t lhs, point_t rhs ) noexcept {
		constexpr auto abs_tolerance = 1.0e-12;
		constexpr auto rel_tolerance = 1.0e-12;
		auto nearly_equal = [&]( double l, double r ) {
			auto const difference = std::abs( l - r );
			auto const scale = std::max( { 1.0, std::abs( l ), std::abs( r ) } );
			return difference <= abs_tolerance + rel_tolerance * scale;
		};
		return nearly_equal( lhs.x, rhs.x ) and nearly_equal( lhs.y, rhs.y ) and
		       nearly_equal( lhs.z, rhs.z );
	}

	void print_point( point_t value ) {
		std::cout << "{" << value.x << ", " << value.y << ", " << value.z << "}";
	}
} // namespace

int main( int argc, char **argv ) {
	auto element_count = std::size_t{ 100'000 };
	if( argc > 1 ) {
		element_count =
		  static_cast<std::size_t>( std::strtoull( argv[1], nullptr, 10 ) );
	}
	std::cout << "testing " << element_count << " items with " << DAW_NUM_RUNS << " runs.\n";
	daw_ensure( element_count > 0U );

	auto const json_data = make_point_array( element_count );
	auto const json_document = std::string_view( json_data );
	std::cout << "Computing expected point average: ";
	auto const expected = average_scalar( json_document );
	print_point( expected );
	std::cout << "\nComputing SIMD point average: ";
	auto const simd_average = average_simd_blocks( json_document );
	print_point( simd_average );
	std::cout << "\n\n";
	daw_ensure( equal( simd_average, expected ) );

	auto scalar_result = daw::json::benchmark::benchmark(
	  DAW_NUM_RUNS,
	  json_document.size( ),
	  "point array average (json iterator, no SIMD)",
	  average_scalar,
	  json_document );
	daw_ensure( equal( scalar_result.get( ), expected ) );

	auto simd_result = daw::json::benchmark::benchmark(
	  DAW_NUM_RUNS,
	  json_document.size( ),
	  "point array average (SIMD class iterator)",
	  average_simd_blocks,
	  json_document );
	daw_ensure( equal( simd_result.get( ), expected ) );
}

#else

int main( ) {
	std::cerr
	  << "This benchmark requires a standard library with <simd> support.\n";
	return EXIT_FAILURE;
}

#endif
