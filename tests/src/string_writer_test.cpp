// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/daw_json_link
//
// Checks string_writer, the output to_json uses when it returns a
// std::string, against serializing into a std::string directly. Growth is
// forced by starting small and by writing past the initial 4096 bytes.

#include "daw/json/daw_json_link.h"

#include <daw/daw_ensure.h>

#include <cstdint>
#include <cstdio>
#include <string>
#include <utility>
#include <vector>

namespace tests {
	struct post {
		std::int64_t id;
		std::string body;
		std::vector<std::int64_t> likes;
	};
} // namespace tests

namespace daw::json {
	template<>
	struct json_data_contract<tests::post> {
		// Named, so that C++17 can use them as template arguments
		static constexpr char const id[] = "id";
		static constexpr char const body[] = "body";
		static constexpr char const likes[] = "likes";
		using type = json_member_list<json_link<id, std::int64_t>,
		                              json_link<body, std::string>,
		                              json_link<likes, std::vector<std::int64_t>>>;

		static constexpr auto to_json_data( tests::post const &p ) {
			return std::forward_as_tuple( p.id, p.body, p.likes );
		}
	};
} // namespace daw::json

namespace {
	std::vector<tests::post> make_posts( std::size_t count ) {
		auto result = std::vector<tests::post>{ };
		for( std::size_t n = 0; n < count; ++n ) {
			auto body = std::string( n % 300, 'a' );
			body += "\"quoted\"\n\ttabbed \xC3\xA9";
			result.push_back( { static_cast<std::int64_t>( n ), std::move( body ),
			                    std::vector<std::int64_t>( n % 7, 42 ) } );
		}
		return result;
	}

	std::string via_std_string( std::vector<tests::post> const &posts ) {
		auto out = std::string( );
		(void)daw::json::to_json_array( posts, out );
		return out;
	}

#if defined( __cpp_lib_constexpr_string ) and \
  __cpp_lib_constexpr_string >= 201907L and \
  defined( __cpp_constexpr_dynamic_alloc )
	constexpr bool constexpr_writes( ) {
		auto w = daw::json::string_writer( 2 );
		w.write( "hello" );
		w.put( ',' );
		w.write( " world" );
		auto const s = std::move( w ).finish( );
		return s == "hello, world" and s.size( ) == 12;
	}
	static_assert( constexpr_writes( ) );
#endif
} // namespace

int main( ) {
#if defined( DAW_JSON_STRING_WRITER_SKIPS_ZERO_FILL )
	std::puts( "string_writer: resize_and_overwrite, no zero fill" );
#else
	std::puts( "string_writer: resize( ), zero fills" );
#endif
	// Small, and well past the 4096 bytes to_json starts with
	for( auto count : { std::size_t{ 0 }, std::size_t{ 1 }, std::size_t{ 500 } } ) {
		auto const posts = make_posts( count );
		auto const expected = via_std_string( posts );
		daw_ensure( daw::json::to_json_array( posts ) == expected );
		if( count > 0 ) {
			daw_ensure( daw::json::to_json( posts.back( ) ) ==
			            daw::json::to_json( posts.back( ), std::string( ) ) );
		}

		// Growth from one byte, many times over
		auto w = daw::json::string_writer( 1 );
		w.write( "[" );
		(void)daw::json::to_json_array( posts, w );
		w.put( ']' );
		daw_ensure( w.size( ) == expected.size( ) + 2 );
		daw_ensure( std::move( w ).finish( ) == "[" + expected + "]" );
	}

	// Default constructed starts with no room
	auto empty = daw::json::string_writer( );
	daw_ensure( empty.size( ) == 0 );
	daw_ensure( std::move( empty ).finish( ).empty( ) );
	std::puts( "string_writer_test passed" );
}
