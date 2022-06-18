// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/daw_json_link
//

#include "defines.h"

#include "daw/json/daw_json_link.h"
#include "twitter_test_alloc_json.h"

#include <daw/cpp_17.h>
#include <daw/daw_benchmark.h>
#include <daw/daw_read_file.h>
#include <daw/daw_traits.h>

#include <fstream>
#include <iostream>
#include <streambuf>

#if not defined( DAW_NUM_RUNS )
#if not defined( DEBUG ) or defined( NDEBUG )
static inline constexpr std::size_t DAW_NUM_RUNS = 250;
#else
static inline constexpr std::size_t DAW_NUM_RUNS = 3;
#endif
#endif
static_assert( DAW_NUM_RUNS > 0 );

template<typename T>
using is_to_json_data_able = decltype( to_json_data( std::declval<T>( ) ) );

template<typename T>
inline bool DAW_CONSTEXPR is_to_json_data_able_v =
  daw::is_detected_v<is_to_json_data_able, T>;

template<typename T,
         std::enable_if_t<is_to_json_data_able_v<T>, std::nullptr_t> = nullptr>
DAW_CONSTEXPR bool operator==( T const &lhs, T const &rhs ) {
	test_assert( to_json_data( lhs ) == to_json_data( rhs ),
	             "Expected that values would be equal" );
	return true;
}

using AllocType = daw::fixed_allocator<daw::twitter::twitter_object_t>;
using namespace daw::json::options;

template<ExecModeTypes ExecMode>
void test( std::string_view json_data, AllocType &alloc )
#ifdef DAW_USE_EXCEPTIONS
  try
#endif
{
	auto const sz = json_data.size( );
	std::cout << "Using " << to_string( ExecMode )
	          << " exec model\n*********************************************\n";
	std::optional<daw::twitter::twitter_object_t> twitter_result;
	// ******************************
	(void)daw::bench_n_test_mbs<DAW_NUM_RUNS>(
	  "twitter bench(checked)", sz,
	  [&]( auto f1 ) {
		  twitter_result.reset( );
		  alloc.release( );
		  twitter_result =
		    daw::json::from_json_alloc<daw::twitter::twitter_object_t>(
		      f1, alloc, parse_flags<ExecMode> );
		  daw::do_not_optimize( twitter_result );
	  },
	  json_data );
	std::cout << "Total Allocations: " << alloc.used( ) << " bytes\n";
	daw::do_not_optimize( twitter_result );
	test_assert( twitter_result, "Missing value" );
	test_assert( not twitter_result->statuses.empty( ), "Expected values" );
	test_assert( twitter_result->statuses.front( ).user.id == 1186275104,
	             "Missing value" );

	// options::CheckedParseMode::no
	(void)daw::bench_n_test_mbs<DAW_NUM_RUNS>(
	  "twitter bench(unchecked)", sz,
	  [&]( auto f1 ) {
		  twitter_result.reset( );
		  alloc.release( );
		  twitter_result =
		    daw::json::from_json_alloc<daw::twitter::twitter_object_t>(
		      f1, alloc, parse_flags<ExecMode, CheckedParseMode::no> );
		  daw::do_not_optimize( twitter_result );
	  },
	  json_data );
	std::cout << "Total Allocations: " << alloc.used( ) << " bytes\n";
	daw::do_not_optimize( twitter_result );
	test_assert( twitter_result, "Missing value" );
	test_assert( not twitter_result->statuses.empty( ), "Expected values" );
	test_assert( twitter_result->statuses.front( ).user.id == 1186275104,
	             "Missing value" );

	// CppCommentSkippingPolicyChecked
	(void)daw::bench_n_test_mbs<DAW_NUM_RUNS>(
	  "twitter bench(cpp comments)", sz,
	  [&]( auto f1 ) {
		  twitter_result.reset( );
		  alloc.release( );
		  twitter_result =
		    daw::json::from_json_alloc<daw::twitter::twitter_object_t>(
		      f1, alloc, parse_flags<ExecMode, PolicyCommentTypes::cpp> );
		  daw::do_not_optimize( twitter_result );
	  },
	  json_data );
	std::cout << "Total Allocations: " << alloc.used( ) << " bytes\n";
	daw::do_not_optimize( twitter_result );
	test_assert( twitter_result, "Missing value" );
	test_assert( not twitter_result->statuses.empty( ), "Expected values" );
	test_assert( twitter_result->statuses.front( ).user.id == 1186275104,
	             "Missing value" );
#if not defined( _MSC_VER ) or defined( __clang__ )
	// CppCommentSkippingPolicyUnchecked
	(void)daw::bench_n_test_mbs<DAW_NUM_RUNS>(
	  "twitter bench(cpp comments, unchecked)", sz,
	  [&]( auto f1 ) {
		  twitter_result.reset( );
		  alloc.release( );
		  twitter_result =
		    daw::json::from_json_alloc<daw::twitter::twitter_object_t>(
		      f1, alloc,
		      parse_flags<ExecMode, PolicyCommentTypes::cpp,
		                  CheckedParseMode::no> );
		  daw::do_not_optimize( twitter_result );
	  },
	  json_data );
	std::cout << "Total Allocations: " << alloc.used( ) << " bytes\n";
	daw::do_not_optimize( twitter_result );
	test_assert( twitter_result, "Missing value" );
	test_assert( not twitter_result->statuses.empty( ), "Expected values" );
	test_assert( twitter_result->statuses.front( ).user.id == 1186275104,
	             "Missing value" );
#endif
	// HashCommentSkippingPolicyChecked
	(void)daw::bench_n_test_mbs<DAW_NUM_RUNS>(
	  "twitter bench(hash comments)", sz,
	  [&]( auto f1 ) {
		  twitter_result.reset( );
		  alloc.release( );
		  twitter_result =
		    daw::json::from_json_alloc<daw::twitter::twitter_object_t>(
		      f1, alloc, parse_flags<ExecMode, PolicyCommentTypes::hash> );
		  daw::do_not_optimize( twitter_result );
	  },
	  json_data );
	std::cout << "Total Allocations: " << alloc.used( ) << " bytes\n";
	daw::do_not_optimize( twitter_result );
	test_assert( twitter_result, "Missing value" );
	test_assert( not twitter_result->statuses.empty( ), "Expected values" );
	test_assert( twitter_result->statuses.front( ).user.id == 1186275104,
	             "Missing value" );
#if not defined( _MSC_VER ) or defined( __clang__ )
	// HashCommentSkippingPolicyUnchecked
	(void)daw::bench_n_test_mbs<DAW_NUM_RUNS>(
	  "twitter bench(hash comments, unchecked)", sz,
	  [&]( auto f1 ) {
		  twitter_result.reset( );
		  alloc.release( );
		  twitter_result =
		    daw::json::from_json_alloc<daw::twitter::twitter_object_t>(
		      f1, alloc,
		      parse_flags<ExecMode, PolicyCommentTypes::hash,
		                  CheckedParseMode::no> );
		  daw::do_not_optimize( twitter_result );
	  },
	  json_data );
	std::cout << "Total Allocations: " << alloc.used( ) << " bytes\n";
	daw::do_not_optimize( twitter_result );
	test_assert( twitter_result, "Missing value" );
	test_assert( not twitter_result->statuses.empty( ), "Expected values" );
	test_assert( twitter_result->statuses.front( ).user.id == 1186275104,
	             "Missing value" );
#endif
	// ******************************
	// Escaped Names
	(void)daw::bench_n_test_mbs<DAW_NUM_RUNS>(
	  "twitter bench(checked, escaped names)", sz,
	  [&]( auto f1 ) {
		  twitter_result.reset( );
		  alloc.release( );
		  twitter_result =
		    daw::json::from_json_alloc<daw::twitter::twitter_object_t>(
		      f1, alloc, parse_flags<ExecMode, AllowEscapedNames::yes> );
		  daw::do_not_optimize( twitter_result );
	  },
	  json_data );
	std::cout << "Total Allocations: " << alloc.used( ) << " bytes\n";
	daw::do_not_optimize( twitter_result );
	test_assert( twitter_result, "Missing value" );
	test_assert( not twitter_result->statuses.empty( ), "Expected values" );
	test_assert( twitter_result->statuses.front( ).user.id == 1186275104,
	             "Missing value" );

	// options::CheckedParseMode::no Escaped Names
	(void)daw::bench_n_test_mbs<DAW_NUM_RUNS>(
	  "twitter bench(unchecked, escaped names)", sz,
	  [&]( auto f1 ) {
		  twitter_result.reset( );
		  alloc.release( );
		  twitter_result =
		    daw::json::from_json_alloc<daw::twitter::twitter_object_t>(
		      f1, alloc,
		      parse_flags<ExecMode, AllowEscapedNames::yes, CheckedParseMode::no> );
		  daw::do_not_optimize( twitter_result );
	  },
	  json_data );
	std::cout << "Total Allocations: " << alloc.used( ) << " bytes\n";
	daw::do_not_optimize( twitter_result );
	test_assert( twitter_result, "Missing value" );
	test_assert( not twitter_result->statuses.empty( ), "Expected values" );
	test_assert( twitter_result->statuses.front( ).user.id == 1186275104,
	             "Missing value" );
}
#ifdef DAW_USE_EXCEPTIONS
catch( daw::json::json_exception const &jex ) {
	std::cerr << "Exception thrown by parser: " << jex.reason( ) << '\n';
	exit( 1 );
} catch( std::exception const &ex ) {
	std::cerr << "Unknown exception thrown during testing: " << ex.what( )
	          << '\n';
	exit( 1 );
} catch( ... ) {
	std::cerr << "Unknown exception thrown during testing\n";
	throw;
}
#endif

int main( int argc, char **argv )
#ifdef DAW_USE_EXCEPTIONS
  try
#endif
{
	auto alloc = AllocType( 10'000'000ULL );
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
	test<ExecModeTypes::compile_time>( json_data, alloc );
	test<ExecModeTypes::runtime>( json_data, alloc );
	if constexpr( not std::is_same_v<runtime_exec_tag, simd_exec_tag> ) {
		test<ExecModeTypes::simd>( json_data, alloc );
	}

	// ******************************
	// Test serialization
	alloc.release( );
	std::string str{ };
	{
		auto twitter_result =
		  daw::json::from_json_alloc<daw::twitter::twitter_object_t>( json_data,
		                                                              alloc );
		std::cout << "Total Allocations: " << alloc.used( ) << " bytes\n";
		(void)daw::bench_n_test_mbs<DAW_NUM_RUNS>(
		  "twitter bench(to_json_string)", sz,
		  [&]( auto const &tr ) {
			  str.clear( );
			  daw::json::to_json( tr, str );
			  daw::do_not_optimize( str );
		  },
		  twitter_result );
		test_assert( not str.empty( ), "Expected a string value" );
		daw::do_not_optimize( str );
	}
	alloc.release( );
	auto const twitter_result2 =
	  daw::json::from_json_alloc<daw::twitter::twitter_object_t>( str, alloc );
	daw::do_not_optimize( twitter_result2 );
}
#ifdef DAW_USE_EXCEPTIONS
catch( daw::json::json_exception const &jex ) {
	std::cerr << "Exception thrown by parser: " << jex.reason( ) << '\n';
	exit( 1 );
} catch( std::exception const &ex ) {
	std::cerr << "Unknown exception thrown during testing: " << ex.what( )
	          << '\n';
	exit( 1 );
} catch( ... ) {
	std::cerr << "Unknown exception thrown during testing\n";
	throw;
}
#endif