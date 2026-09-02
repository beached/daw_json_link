// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/daw_json_link
//

#include "daw_json_benchmark.h"

#include <daw/json/daw_json_link.h>

#include <daw/daw_ensure.h>

#include <algorithm>
#include <array>
#include <cstddef>
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

namespace {
	using namespace daw::json;

	using checked_string = json_string_no_name<std::string_view>;
	using assumed_valid_string = json_string_no_name<
	  std::string_view,
	  options::string_opt( options::EscapeValidUTF8::AssumeValid )>;
	using raw_string = json_string_raw_no_name<std::string_view>;

	inline constexpr std::size_t bytes_per_sample = 256U * 1024U;

	[[nodiscard]] std::string make_workload( std::size_t size,
	                                         std::string_view pattern ) {
		daw_ensure( size % pattern.size( ) == 0U );
		auto result = std::string{ };
		result.reserve( size );
		while( result.size( ) < size ) {
			result.append( pattern.data( ), pattern.size( ) );
		}
		return result;
	}

	template<typename JsonString>
	void benchmark_mapping( std::string_view mapping_name, std::string_view input,
	                        std::string const &expected ) {
		auto const iterations =
		  std::max( std::size_t{ 1 }, bytes_per_sample / expected.size( ) );
		auto const serialized_bytes = iterations * expected.size( );

		auto output = std::string{ };
		output.reserve( expected.size( ) );
		auto const serialize = [&] {
			for( std::size_t n = 0; n < iterations; ++n ) {
				output.clear( );
				(void)to_json<JsonString>( input, output );
			}
			daw::do_not_optimize( output );
			return output.size( );
		};

		daw_ensure( serialize( ) == expected.size( ) );
		daw_ensure( output == expected );

		auto const title = std::string( mapping_name.data( ), mapping_name.size( ) ) +
		                   ", " + std::to_string( input.size( ) ) + " bytes";
		(void)daw::json::benchmark::benchmark( DAW_NUM_RUNS, serialized_bytes,
		                                      title, serialize );
	}

	void benchmark_known_escaped( std::size_t string_size ) {
		auto const input_storage =
		  make_workload( string_size, "A\xC3\xA9"
		                              "B" );
		auto const input = std::string_view( input_storage );
		auto expected = std::string{ "\"" };
		expected += input;
		expected += '"';

		benchmark_mapping<checked_string>( "json_string Validate", input, expected );
		benchmark_mapping<assumed_valid_string>( "json_string AssumeValid", input,
		                                         expected );
		benchmark_mapping<raw_string>( "json_string_raw", input, expected );
	}

	template<bool UseScannedWrite>
	void benchmark_escape_copy( std::string_view implementation_name,
	                            std::string_view workload_name,
	                            std::string_view input,
	                            std::string const &expected ) {
		auto const iterations =
		  std::max( std::size_t{ 1 }, bytes_per_sample / expected.size( ) );
		auto const serialized_bytes = iterations * expected.size( );
		auto output = std::string{ };
		output.reserve( expected.size( ) );
		auto const serialize = [&] {
			for( std::size_t n = 0; n < iterations; ++n ) {
				output.clear( );
				auto it = serialization_policy<std::string>( output );
				it.put( '"' );
				it = utils::copy_to_iterator<true, options::EightBitModes::AllowFull,
				                             UseScannedWrite>( it, input );
				it.put( '"' );
			}
			daw::do_not_optimize( output );
			return output.size( );
		};

		daw_ensure( serialize( ) == expected.size( ) );
		daw_ensure( output == expected );
		auto const title =
		  std::string( implementation_name ) + ", " + std::string( workload_name ) +
		  ", " + std::to_string( input.size( ) ) + " bytes";
		(void)daw::json::benchmark::benchmark( DAW_NUM_RUNS, serialized_bytes,
		                                      title, serialize );
	}

	void benchmark_escaping( std::size_t string_size,
	                         std::string_view workload_name,
	                         std::string_view pattern ) {
		auto const input_storage = make_workload( string_size, pattern );
		auto const input = std::string_view( input_storage );
		auto const expected = to_json<checked_string>( input );
		benchmark_escape_copy<false>( "code-point write", workload_name, input,
		                              expected );
		benchmark_escape_copy<true>( "scanned-block write", workload_name, input,
		                             expected );
	}
} // namespace

int main( ) {
	for( auto const size : std::array<std::size_t, 4>{ 8U, 64U, 1024U,
	                                                  64U * 1024U } ) {
		benchmark_known_escaped( size );
	}

	for( auto const size : std::array<std::size_t, 3>{ 64U, 1024U,
	                                                  64U * 1024U } ) {
		benchmark_escaping( size, "sparse escapes",
		                    "abcdefghijklmnopqrstuvwxyz01234\n" );
		benchmark_escaping( size, "dense escapes", "\"\\\n\t" );
	}
}
