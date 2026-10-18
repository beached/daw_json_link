// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/daw_json_link
//
// Serializes into fixed size buffers through span-like outputs: daw::span,
// which has remove_prefix, and std::span, which only has subspan. Arrays
// write their brackets and commas with put( ), so both paths of the trait's
// put( ) are used.

#include "daw/json/daw_json_link.h"

#include <daw/daw_ensure.h>
#include <daw/daw_span.h>

#include <array>
#include <cstddef>
#include <cstdio>
#include <string>
#include <string_view>
#include <vector>

#if defined( __cpp_lib_span )
#include <span>
#endif

namespace {
	static_assert( daw::json::concepts::writeable_output_details::
	                 has_remove_prefix_v<daw::span<char>> );

	std::vector<std::vector<int>> const values = { { 1, 2, 3 }, { }, { 42 } };

	template<typename Span>
	void check( ) {
		auto const expected = daw::json::to_json( values );
		auto buffer = std::array<char, 256>{ };
		auto out = Span( buffer.data( ), buffer.size( ) );
		auto const rest = daw::json::to_json( values, out );
		auto const written = buffer.size( ) - rest.size( );
		daw_ensure( std::string_view( buffer.data( ), written ) == expected );
	}
} // namespace

int main( ) {
	check<daw::span<char>>( );
#if defined( __cpp_lib_span )
	static_assert( not daw::json::concepts::writeable_output_details::
	                 has_remove_prefix_v<std::span<char>> );
	check<std::span<char>>( );
	std::puts( "span_output_test passed: daw::span, std::span" );
#else
	std::puts( "span_output_test passed: daw::span" );
#endif
}
