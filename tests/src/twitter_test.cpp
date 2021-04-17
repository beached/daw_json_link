// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/daw_json_link
//

#include "defines.h"

#include "twitter_test_json.h"

#include <daw/cpp_17.h>
#include <daw/daw_benchmark.h>
#include <daw/daw_read_file.h>
#include <daw/daw_traits.h>
#include <daw/json/daw_from_json.h>
#include <daw/json/daw_to_json.h>

#include <iostream>
#include <streambuf>

#if not defined( DAW_NUM_RUNS )
#if not defined( DEBUG ) or defined( NDEBUG )
static inline constexpr std::size_t DAW_NUM_RUNS = 250;
#else
static inline constexpr std::size_t DAW_NUM_RUNS = 2;
#endif
#endif
static_assert( DAW_NUM_RUNS > 0 );

namespace {
	template<typename ExecTag>
	void test( std::string_view json_data ) {
#if defined( __cpp_exceptions ) or defined( __EXCEPTIONS ) or \
  defined( _CPPUNWIND )
		try {
#endif
			auto const sz = json_data.size( );
			std::cout
			  << "Using " << ExecTag::name
			  << " exec model\n*********************************************\n";
			std::optional<daw::twitter::twitter_object_t> twitter_result;
			// ******************************
			// NoCommentSkippingPolicyChecked
			daw::bench_n_test_mbs<DAW_NUM_RUNS>(
			  "twitter bench(checked)", sz,
			  [&twitter_result]( auto f1 ) {
				  twitter_result = daw::json::from_json<
				    daw::twitter::twitter_object_t,
				    daw::json::SIMDNoCommentSkippingPolicyChecked<ExecTag>>( f1 );
				  daw::do_not_optimize( twitter_result );
			  },
			  json_data );
			daw::do_not_optimize( twitter_result );
			test_assert( twitter_result, "Missing value" );
			test_assert( twitter_result->statuses.size( ) > 0, "Expected values" );
			test_assert( twitter_result->statuses.front( ).user.id == 1186275104,
			             "Missing value" );

			// NoCommentSkippingPolicyUnchecked
			daw::bench_n_test_mbs<DAW_NUM_RUNS>(
			  "twitter bench(unchecked)", sz,
			  [&twitter_result]( auto f1 ) {
				  twitter_result = daw::json::from_json<
				    daw::twitter::twitter_object_t,
				    daw::json::SIMDNoCommentSkippingPolicyUnchecked<ExecTag>>( f1 );
				  daw::do_not_optimize( twitter_result );
			  },
			  json_data );
			daw::do_not_optimize( twitter_result );
			test_assert( twitter_result, "Missing value" );
			test_assert( twitter_result->statuses.size( ) > 0, "Expected values" );
			test_assert( twitter_result->statuses.front( ).user.id == 1186275104,
			             "Missing value" );

			// CppCommentSkippingPolicyChecked
			daw::bench_n_test_mbs<DAW_NUM_RUNS>(
			  "twitter bench(cpp comments)", sz,
			  [&twitter_result]( auto f1 ) {
				  twitter_result = daw::json::from_json<
				    daw::twitter::twitter_object_t,
				    daw::json::SIMDCppCommentSkippingPolicyChecked<ExecTag>>( f1 );
				  daw::do_not_optimize( twitter_result );
			  },
			  json_data );
			daw::do_not_optimize( twitter_result );
			test_assert( twitter_result, "Missing value" );
			test_assert( twitter_result->statuses.size( ) > 0, "Expected values" );
			test_assert( twitter_result->statuses.front( ).user.id == 1186275104,
			             "Missing value" );
#if not defined( _MSC_VER ) or defined( __clang__ )
			// CppCommentSkippingPolicyUnchecked
			daw::bench_n_test_mbs<DAW_NUM_RUNS>(
			  "twitter bench(cpp comments, unchecked)", sz,
			  [&twitter_result]( auto f1 ) {
				  twitter_result = daw::json::from_json<
				    daw::twitter::twitter_object_t,
				    daw::json::SIMDCppCommentSkippingPolicyUnchecked<ExecTag>>( f1 );
				  daw::do_not_optimize( twitter_result );
			  },
			  json_data );
			daw::do_not_optimize( twitter_result );
			test_assert( twitter_result, "Missing value" );
			test_assert( twitter_result->statuses.size( ) > 0, "Expected values" );
			test_assert( twitter_result->statuses.front( ).user.id == 1186275104,
			             "Missing value" );
#endif
			// HashCommentSkippingPolicyChecked
			daw::bench_n_test_mbs<DAW_NUM_RUNS>(
			  "twitter bench(hash comments)", sz,
			  [&twitter_result]( auto f1 ) {
				  twitter_result = daw::json::from_json<
				    daw::twitter::twitter_object_t,
				    daw::json::SIMDHashCommentSkippingPolicyChecked<ExecTag>>( f1 );
				  daw::do_not_optimize( twitter_result );
			  },
			  json_data );
			daw::do_not_optimize( twitter_result );
			test_assert( twitter_result, "Missing value" );
			test_assert( twitter_result->statuses.size( ) > 0, "Expected values" );
			test_assert( twitter_result->statuses.front( ).user.id == 1186275104,
			             "Missing value" );
#if not defined( _MSC_VER ) or defined( __clang__ )
			// HashCommentSkippingPolicyUnchecked
			daw::bench_n_test_mbs<DAW_NUM_RUNS>(
			  "twitter bench(hash comments, unchecked)", sz,
			  [&twitter_result]( auto f1 ) {
				  twitter_result = daw::json::from_json<
				    daw::twitter::twitter_object_t,
				    daw::json::SIMDHashCommentSkippingPolicyUnchecked<ExecTag>>( f1 );
				  daw::do_not_optimize( twitter_result );
			  },
			  json_data );
			daw::do_not_optimize( twitter_result );
			test_assert( twitter_result, "Missing value" );
			test_assert( twitter_result->statuses.size( ) > 0, "Expected values" );
			test_assert( twitter_result->statuses.front( ).user.id == 1186275104,
			             "Missing value" );
#endif
			// ******************************
			// NoCommentSkippingPolicyChecked Escaped Names
			daw::bench_n_test_mbs<DAW_NUM_RUNS>(
			  "twitter bench(checked, escaped names)", sz,
			  [&twitter_result]( auto f1 ) {
				  twitter_result = daw::json::from_json<
				    daw::twitter::twitter_object_t,
				    daw::json::SIMDNoCommentSkippingPolicyChecked<ExecTag>>( f1 );
				  daw::do_not_optimize( twitter_result );
			  },
			  json_data );
			daw::do_not_optimize( twitter_result );
			test_assert( twitter_result, "Missing value" );
			test_assert( twitter_result->statuses.size( ) > 0, "Expected values" );
			test_assert( twitter_result->statuses.front( ).user.id == 1186275104,
			             "Missing value" );

			// NoCommentSkippingPolicyUnchecked Escaped Names
			daw::bench_n_test_mbs<DAW_NUM_RUNS>(
			  "twitter bench(unchecked, escaped names)", sz,
			  [&twitter_result]( auto f1 ) {
				  twitter_result = daw::json::from_json<
				    daw::twitter::twitter_object_t,
				    daw::json::SIMDNoCommentSkippingPolicyUnchecked<ExecTag>>( f1 );
				  daw::do_not_optimize( twitter_result );
			  },
			  json_data );
			daw::do_not_optimize( twitter_result );
			test_assert( twitter_result, "Missing value" );
			test_assert( twitter_result->statuses.size( ) > 0, "Expected values" );
			test_assert( twitter_result->statuses.front( ).user.id == 1186275104,
			             "Missing value" );
#if defined( __cpp_exceptions ) or defined( __EXCEPTIONS ) or \
  defined( _CPPUNWIND )
		} catch( daw::json::json_exception const &jex ) {
			std::cerr << "Exception thrown by parser: "
			          << to_formatted_string( jex, nullptr ) << '\n';
			exit( 1 );
#endif
		}
	}
} // namespace

int main( int argc, char **argv )
#if defined( __cpp_exceptions ) or defined( __EXCEPTIONS ) or \
  defined( _CPPUNWIND )
  try
#endif
{
	using namespace daw::json;
	if( argc < 2 ) {
		std::cerr << "Must supply a filenames to open\n";
		exit( 1 );
	}

	std::string const json_data = [argv] {
		auto const mmf = *daw::read_file( argv[1] );
		test_assert( mmf.size( ) > 2, "Minimum json data size is 2 '{}'" );
		return std::string( mmf.data( ), mmf.size( ) );
	}( );

	auto const sz = json_data.size( );
	std::cout << "Processing: " << daw::utility::to_bytes_per_second( sz )
	          << '\n';
	test<constexpr_exec_tag>( json_data );
	test<runtime_exec_tag>( json_data );
	if constexpr( not std::is_same_v<runtime_exec_tag, simd_exec_tag> ) {
		test<simd_exec_tag>( json_data );
	}

	// ******************************
	// Test serialization
	std::optional<daw::twitter::twitter_object_t> twitter_result =
	  daw::json::from_json<daw::twitter::twitter_object_t>( json_data );
	std::string str{ };
	auto out_it = std::back_inserter( str );
	daw::bench_n_test_mbs<DAW_NUM_RUNS>(
	  "twitter bench(to_json_string)", sz,
	  [&]( auto const &tr ) {
		  str.clear( );
		  daw::json::to_json( *tr, out_it );
		  daw::do_not_optimize( str );
	  },
	  twitter_result );
	test_assert( not str.empty( ), "Expected a string value" );
	daw::do_not_optimize( str );
	auto const twitter_result2 =
	  daw::json::from_json<daw::twitter::twitter_object_t>( str );
	daw::do_not_optimize( twitter_result2 );
#if defined( __cpp_exceptions ) or defined( __EXCEPTIONS ) or \
  defined( _CPPUNWIND )
}
catch( daw::json::json_exception const &jex ) {
	std::cerr << "Exception thrown by parser: "
	          << to_formatted_string( jex, nullptr ) << std::endl;
	exit( 1 );
#endif
}
