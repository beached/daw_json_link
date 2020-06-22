// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/daw_json_link
//

#include "twitter_test.h"
#include "daw/json/daw_json_link.h"

#include <daw/cpp_17.h>
#include <daw/daw_benchmark.h>
#include <daw/daw_memory_mapped_file.h>
#include <daw/daw_traits.h>

#include <fstream>
#include <iostream>
#include <streambuf>

template<typename T>
using is_to_json_data_able = decltype( to_json_data( std::declval<T>( ) ) );

template<typename T>
inline bool constexpr is_to_json_data_able_v =
  daw::is_detected_v<is_to_json_data_able, T>;

template<typename T,
         std::enable_if_t<is_to_json_data_able_v<T>, std::nullptr_t> = nullptr>
constexpr bool operator==( T const &lhs, T const &rhs ) {
	if( to_json_data( lhs ) == to_json_data( rhs ) ) {
		return true;
	}
	daw_json_error( "Expected that values would be equal" );
}

int main( int argc, char **argv ) try {
	static constexpr std::size_t NUM_RUNS = 250;
	using namespace daw::json;
	if( argc < 2 ) {
		std::cerr << "Must supply a filenames to open\n";
		exit( 1 );
	}

  std::string const json_data = [argv] {
    auto const mmf = daw::filesystem::memory_mapped_file_t<>( argv[1] );
    daw_json_assert( mmf.size( ) > 2, "Minimum json data size is 2 '{}'" );
	  return std::string( mmf.data( ), mmf.size( ) );
	}( );

	auto const sz = json_data.size( );
	std::cout << "Processing: " << daw::utility::to_bytes_per_second( sz )
	          << '\n';

	std::optional<daw::twitter::twitter_object_t> twitter_result;

	// ******************************
	// NoCommentSkippingPolicyChecked
	daw::bench_n_test_mbs<NUM_RUNS>(
	  "twitter bench(checked)", sz,
	  [&twitter_result]( auto f1 ) {
		  twitter_result =
		    daw::json::from_json<daw::twitter::twitter_object_t,
		                         daw::json::NoCommentSkippingPolicyChecked>( f1 );
		  daw::do_not_optimize( twitter_result );
	  },
	  json_data );
	daw::do_not_optimize( twitter_result );
	daw_json_assert( twitter_result, "Missing value" );
	daw_json_assert( twitter_result->statuses.size( ) > 0, "Expected values" );
	daw_json_assert( twitter_result->statuses.front( ).user.id == 1186275104,
	                 "Missing value" );

	// NoCommentSkippingPolicyUnchecked
	daw::bench_n_test_mbs<NUM_RUNS>(
	  "twitter bench(unchecked)", sz,
	  [&twitter_result]( auto f1 ) {
		  twitter_result =
		    daw::json::from_json<daw::twitter::twitter_object_t,
		                         daw::json::NoCommentSkippingPolicyUnchecked>( f1 );
		  daw::do_not_optimize( twitter_result );
	  },
	  json_data );
	daw::do_not_optimize( twitter_result );
	daw_json_assert( twitter_result, "Missing value" );
	daw_json_assert( twitter_result->statuses.size( ) > 0, "Expected values" );
	daw_json_assert( twitter_result->statuses.front( ).user.id == 1186275104,
	                 "Missing value" );

	// CppCommentSkippingPolicyChecked
	daw::bench_n_test_mbs<NUM_RUNS>(
	  "twitter bench(cpp comments)", sz,
	  [&twitter_result]( auto f1 ) {
		  twitter_result =
		    daw::json::from_json<daw::twitter::twitter_object_t,
		                         daw::json::CppCommentSkippingPolicyChecked>( f1 );
		  daw::do_not_optimize( twitter_result );
	  },
	  json_data );
	daw::do_not_optimize( twitter_result );
	daw_json_assert( twitter_result, "Missing value" );
	daw_json_assert( twitter_result->statuses.size( ) > 0, "Expected values" );
	daw_json_assert( twitter_result->statuses.front( ).user.id == 1186275104,
	                 "Missing value" );

	// CppCommentSkippingPolicyUnchecked
	daw::bench_n_test_mbs<NUM_RUNS>(
	  "twitter bench(cpp comments, unchecked)", sz,
	  [&twitter_result]( auto f1 ) {
		  twitter_result =
		    daw::json::from_json<daw::twitter::twitter_object_t,
		                         daw::json::CppCommentSkippingPolicyUnchecked>(
		      f1 );
		  daw::do_not_optimize( twitter_result );
	  },
	  json_data );
	daw::do_not_optimize( twitter_result );
	daw_json_assert( twitter_result, "Missing value" );
	daw_json_assert( twitter_result->statuses.size( ) > 0, "Expected values" );
	daw_json_assert( twitter_result->statuses.front( ).user.id == 1186275104,
	                 "Missing value" );

	// HashCommentSkippingPolicyChecked
	daw::bench_n_test_mbs<NUM_RUNS>(
	  "twitter bench(hash comments)", sz,
	  [&twitter_result]( auto f1 ) {
		  twitter_result =
		    daw::json::from_json<daw::twitter::twitter_object_t,
		                         daw::json::HashCommentSkippingPolicyChecked>( f1 );
		  daw::do_not_optimize( twitter_result );
	  },
	  json_data );
	daw::do_not_optimize( twitter_result );
	daw_json_assert( twitter_result, "Missing value" );
	daw_json_assert( twitter_result->statuses.size( ) > 0, "Expected values" );
	daw_json_assert( twitter_result->statuses.front( ).user.id == 1186275104,
	                 "Missing value" );

	// HashCommentSkippingPolicyUnchecked
	daw::bench_n_test_mbs<NUM_RUNS>(
	  "twitter bench(hash comments, unchecked)", sz,
	  [&twitter_result]( auto f1 ) {
		  twitter_result =
		    daw::json::from_json<daw::twitter::twitter_object_t,
		                         daw::json::HashCommentSkippingPolicyUnchecked>(
		      f1 );
		  daw::do_not_optimize( twitter_result );
	  },
	  json_data );
	daw::do_not_optimize( twitter_result );
	daw_json_assert( twitter_result, "Missing value" );
	daw_json_assert( twitter_result->statuses.size( ) > 0, "Expected values" );
	daw_json_assert( twitter_result->statuses.front( ).user.id == 1186275104,
	                 "Missing value" );

#ifdef DAW_ALLOW_SSE3
	// ******************************
	// SIMDNoCommentSkippingPolicyChecked
	daw::bench_n_test_mbs<NUM_RUNS>(
	  "twitter bench(checked, SSE3)", sz,
	  [&twitter_result]( auto f1 ) {
		  twitter_result = daw::json::from_json<
		    daw::twitter::twitter_object_t,
		    daw::json::SIMDNoCommentSkippingPolicyChecked<SIMDModes::SSE3>>( f1 );
		  daw::do_not_optimize( twitter_result );
	  },
	  json_data );
	daw::do_not_optimize( twitter_result );
	daw_json_assert( twitter_result, "Missing value" );
	daw_json_assert( twitter_result->statuses.size( ) > 0, "Expected values" );
	daw_json_assert( twitter_result->statuses.front( ).user.id == 1186275104,
	                 "Missing value" );

	// SIMDNoCommentSkippingPolicyUnchecked
	daw::bench_n_test_mbs<NUM_RUNS>(
	  "twitter bench(unchecked, SSE3)", sz,
	  [&twitter_result]( auto f1 ) {
		  twitter_result = daw::json::from_json<
		    daw::twitter::twitter_object_t,
		    daw::json::SIMDNoCommentSkippingPolicyUnchecked<SIMDModes::SSE3>>( f1 );
		  daw::do_not_optimize( twitter_result );
	  },
	  json_data );
	daw::do_not_optimize( twitter_result );
	daw_json_assert( twitter_result, "Missing value" );
	daw_json_assert( twitter_result->statuses.size( ) > 0, "Expected values" );
	daw_json_assert( twitter_result->statuses.front( ).user.id == 1186275104,
	                 "Missing value" );

	// SIMDCppCommentSkippingPolicyChecked
	daw::bench_n_test_mbs<NUM_RUNS>(
	  "twitter bench(cpp comments SSE3)", sz,
	  [&twitter_result]( auto f1 ) {
		  twitter_result = daw::json::from_json<
		    daw::twitter::twitter_object_t,
		    daw::json::SIMDCppCommentSkippingPolicyChecked<SIMDModes::SSE3>>( f1 );
		  daw::do_not_optimize( twitter_result );
	  },
	  json_data );
	daw::do_not_optimize( twitter_result );
	daw_json_assert( twitter_result, "Missing value" );
	daw_json_assert( twitter_result->statuses.size( ) > 0, "Expected values" );
	daw_json_assert( twitter_result->statuses.front( ).user.id == 1186275104,
	                 "Missing value" );

	// SIMDCppCommentSkippingPolicyUnchecked
	daw::bench_n_test_mbs<NUM_RUNS>(
	  "twitter bench(cpp comments, unchecked, SSE3)", sz,
	  [&twitter_result]( auto f1 ) {
		  twitter_result = daw::json::from_json<
		    daw::twitter::twitter_object_t,
		    daw::json::SIMDCppCommentSkippingPolicyUnchecked<SIMDModes::SSE3>>(
		    f1 );
		  daw::do_not_optimize( twitter_result );
	  },
	  json_data );
	daw::do_not_optimize( twitter_result );
	daw_json_assert( twitter_result, "Missing value" );
	daw_json_assert( twitter_result->statuses.size( ) > 0, "Expected values" );
	daw_json_assert( twitter_result->statuses.front( ).user.id == 1186275104,
	                 "Missing value" );

	// SIMDHashCommentSkippingPolicyChecked
	daw::bench_n_test_mbs<NUM_RUNS>(
	  "twitter bench(hash comments, SSE3)", sz,
	  [&twitter_result]( auto f1 ) {
		  twitter_result = daw::json::from_json<
		    daw::twitter::twitter_object_t,
		    daw::json::SIMDHashCommentSkippingPolicyChecked<SIMDModes::SSE3>>( f1 );
		  daw::do_not_optimize( twitter_result );
	  },
	  json_data );
	daw::do_not_optimize( twitter_result );
	daw_json_assert( twitter_result, "Missing value" );
	daw_json_assert( twitter_result->statuses.size( ) > 0, "Expected values" );
	daw_json_assert( twitter_result->statuses.front( ).user.id == 1186275104,
	                 "Missing value" );

	// SIMDHashCommentSkippingPolicyUnchecked
	daw::bench_n_test_mbs<NUM_RUNS>(
	  "twitter bench(hash comments, unchecked, SSE3)", sz,
	  [&twitter_result]( auto f1 ) {
		  twitter_result = daw::json::from_json<
		    daw::twitter::twitter_object_t,
		    daw::json::SIMDHashCommentSkippingPolicyUnchecked<SIMDModes::SSE3>>(
		    f1 );
		  daw::do_not_optimize( twitter_result );
	  },
	  json_data );
	daw::do_not_optimize( twitter_result );
	daw_json_assert( twitter_result, "Missing value" );
	daw_json_assert( twitter_result->statuses.size( ) > 0, "Expected values" );
	daw_json_assert( twitter_result->statuses.front( ).user.id == 1186275104,
	                 "Missing value" );
	// ******************************
#endif

	// ******************************
	// NoCommentSkippingPolicyChecked Escaped Names
	daw::bench_n_test_mbs<NUM_RUNS>(
	  "twitter bench(checked, escaped names)", sz,
	  [&twitter_result]( auto f1 ) {
		  twitter_result = daw::json::from_json<
		    daw::twitter::twitter_object_t,
		    daw::json::BasicNoCommentSkippingPolicy<false, SIMDModes::None, true>>(
		    f1 );
		  daw::do_not_optimize( twitter_result );
	  },
	  json_data );
	daw::do_not_optimize( twitter_result );
	daw_json_assert( twitter_result, "Missing value" );
	daw_json_assert( twitter_result->statuses.size( ) > 0, "Expected values" );
	daw_json_assert( twitter_result->statuses.front( ).user.id == 1186275104,
	                 "Missing value" );

	// NoCommentSkippingPolicyUnchecked Escaped Names
	daw::bench_n_test_mbs<NUM_RUNS>(
	  "twitter bench(unchecked, escaped names)", sz,
	  [&twitter_result]( auto f1 ) {
		  twitter_result = daw::json::from_json<
		    daw::twitter::twitter_object_t,
		    daw::json::BasicNoCommentSkippingPolicy<false, SIMDModes::None, true>>(
		    f1 );
		  daw::do_not_optimize( twitter_result );
	  },
	  json_data );
	daw::do_not_optimize( twitter_result );
	daw_json_assert( twitter_result, "Missing value" );
	daw_json_assert( twitter_result->statuses.size( ) > 0, "Expected values" );
	daw_json_assert( twitter_result->statuses.front( ).user.id == 1186275104,
	                 "Missing value" );

	// ******************************
	std::string str{ };
	auto out_it = std::back_inserter( str );
	daw::bench_n_test_mbs<NUM_RUNS>(
	  "twitter bench(to_json_string)", sz,
	  [&]( auto const &tr ) {
		  str.clear( );
		  daw::json::to_json( *tr, out_it );
		  daw::do_not_optimize( str );
	  },
	  twitter_result );
	daw_json_assert( not str.empty( ), "Expected a string value" );
	daw::do_not_optimize( str );
	auto const twitter_result2 =
	  daw::json::from_json<daw::twitter::twitter_object_t>( str );
	daw::do_not_optimize( twitter_result2 );
} catch( daw::json::json_exception const &jex ) {
	std::cerr << "Exception thrown by parser: " << jex.reason( ) << std::endl;
	exit( 1 );
}
