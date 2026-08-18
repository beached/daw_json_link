// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/daw_json_link
//

#include "defines.h"

#include <daw/json/impl/daw_json_simd_iterator.h>

#include <daw/daw_benchmark.h>
#include <daw/daw_read_file.h>

#include <cstdint>
#include <cstdlib>
#include <iostream>
#include <string_view>

#if defined( __cpp_lib_simd ) or defined( __glibcxx_simd )

namespace {
	using iterator = daw::json::experimental::json_simd_block_iterator<>;

	[[nodiscard]] std::uint64_t classify( std::string_view document ) {
		auto checksum = std::uint64_t{ 0 };
		for( auto const current : iterator( document ) ) {
			checksum += current.structural_start.to_ullong( );
			checksum += current.number_start.to_ullong( );
			checksum += current.boolean_start.to_ullong( );
			checksum += current.string_start.to_ullong( );
		}
		return checksum;
	}
} // namespace

int main( int argc, char **argv ) {
	auto const filename = argc > 1 ? argv[1] : "/tmp/1.json";
	auto const json_data = daw::read_file( filename ).value( );
	auto const document = std::string_view( json_data );

	auto const checksum = *daw::bench_n_test_mbs<10>(
	  "json_simd_block_iterator", document.size( ), classify, document );
	daw::do_not_optimize( checksum );
	std::cout << "checksum: " << checksum << '\n';
}

#else

int main( ) {
	std::cerr << "json_simd_block_iterator requires an implementation of <simd>\n";
	return EXIT_FAILURE;
}

#endif
