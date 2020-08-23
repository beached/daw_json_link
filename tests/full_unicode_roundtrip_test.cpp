// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/daw_json_link
//

#include "defines.h"

#include "daw/json/daw_json_iterator.h"
#include "daw/json/daw_json_link.h"

#include <daw/daw_benchmark.h>
#include <daw/daw_memory_mapped_file.h>

#include <iostream>

#if not defined( DAW_NUM_RUNS )
#if not defined( DEBUG ) or defined( NDEBUG ) 
static inline constexpr std::size_t DAW_NUM_RUNS = 250;
#else
static inline constexpr std::size_t DAW_NUM_RUNS = 1;
#endif
#endif
static_assert( DAW_NUM_RUNS > 0 );

struct unicode_data {
	std::string escaped;
	std::string unicode;
};
bool operator==( unicode_data const &lhs, unicode_data const &rhs ) {
	return std::tie( lhs.escaped, lhs.unicode ) ==
	       std::tie( rhs.escaped, rhs.unicode );
}

namespace daw::json {
	template<>
	struct json_data_contract<unicode_data> {
#ifdef __cpp_nontype_template_parameter_class
		using type =
		  json_member_list<json_string<"escaped">, json_string<"unicode">>;
#else
		DAW_CONSTEXPR inline static char const escaped[] = "escaped";
		DAW_CONSTEXPR inline static char const unicode[] = "unicode";
		using type = json_member_list<json_string<escaped>, json_string<unicode>>;
#endif
		static inline auto to_json_data( unicode_data const &value ) {
			return std::forward_as_tuple( value.escaped, value.unicode );
		}
	};
} // namespace daw::json

template<typename Policy, typename MMF>
void test( MMF const &json_str, MMF const &json_str_escaped ) {
	std::vector<unicode_data> const unicode_test =
	  daw::json::from_json_array<unicode_data, std::vector<unicode_data>, Policy>(
	    std::string_view( json_str.data( ), json_str.size( ) ) );
	std::vector<unicode_data> const unicode_test_from_escaped =
	  daw::json::from_json_array<unicode_data, std::vector<unicode_data>, Policy>(
	    std::string_view( json_str_escaped.data( ), json_str_escaped.size( ) ) );

	daw_json_assert( unicode_test.size( ) == unicode_test_from_escaped.size( ),
	                 "Expected same size" );
	auto mismatch_pos = std::mismatch( unicode_test.begin( ), unicode_test.end( ),
	                                   unicode_test_from_escaped.begin( ) );
	daw_json_assert( mismatch_pos.first == unicode_test.end( ),
	                 "Should be the same after parsing" );

	std::string const json_str2 = daw::json::to_json_array( unicode_test );
	std::vector<unicode_data> unicode_test2 =
	  daw::json::from_json_array<unicode_data>(
	    std::string_view( json_str2.data( ), json_str2.size( ) ) );

	auto mismatch_pos2 = std::mismatch(
	  unicode_test.begin( ), unicode_test.end( ), unicode_test2.begin( ) );
	daw_json_assert( mismatch_pos2.first == unicode_test.end( ),
	                 "Should be the same after parsing" );
	using range_t = daw::json::json_array_range<unicode_data, Policy>;
	daw::bench_n_test_mbs<DAW_NUM_RUNS>(
	  "full unicode bench", json_str.size( ),
	  []( auto rng ) {
		  auto first = rng.begin( );
		  auto const last = rng.end( );
		  auto result = std::distance( first, last );
		  daw::do_not_optimize( result );
	  },
	  range_t( std::string_view( json_str ) ) );
	daw::bench_n_test_mbs<DAW_NUM_RUNS>(
	  "full unicode all escaped bench", json_str.size( ),
	  []( auto rng ) {
		  auto first = rng.begin( );
		  auto const last = rng.end( );
		  auto result = std::distance( first, last );
		  daw::do_not_optimize( result );
	  },
	  range_t( std::string_view( json_str_escaped ) ) );
}

int main( int argc, char **argv ) try {
	using namespace daw::json;
#if defined( NDEBUG ) and not defined( DEBUG )
	std::cout << "release run\n";
#else
	std::cout << "debug run\n";
#endif
	if( argc < 3 ) {
		std::cerr << "Must supply filenames to open\n";
		std::cerr << "full_unicode.json full_unicode_escaped.json\n";
		exit( 1 );
	}

	auto const json_str = daw::filesystem::memory_mapped_file_t<>( argv[1] );
	auto const json_str_escaped =
	  daw::filesystem::memory_mapped_file_t<>( argv[2] );

	test<
	  daw::json::SIMDCppCommentSkippingPolicyChecked<daw::json::SIMDModes::None>>(
	  json_str, json_str_escaped );
#if defined( DAW_ALLOW_SSE42 )
	std::cout << "SS3\n********************\n";
	test<
	  daw::json::SIMDCppCommentSkippingPolicyChecked<daw::json::SIMDModes::SSE42>>(
	  json_str, json_str_escaped );
#endif
} catch( daw::json::json_exception const &jex ) {
	std::cerr << "Exception thrown by parser: " << jex.reason( ) << std::endl;
	exit( 1 );
}
