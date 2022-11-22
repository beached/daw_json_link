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

using namespace daw::json::options;

inline namespace {
	template<ExecModeTypes ExecMode>
	void test( std::string_view json_data, bool do_asserts )
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
		  [&twitter_result]( auto f1 ) {
			  twitter_result = daw::json::from_json<daw::twitter::twitter_object_t>(
			    f1, parse_flags<ExecMode> );
			  daw::do_not_optimize( twitter_result );
		  },
		  json_data );
		daw::do_not_optimize( twitter_result );
		if( do_asserts ) {
			test_assert( twitter_result, "Missing value" );
			test_assert( not twitter_result->statuses.empty( ), "Expected values" );
			test_assert( twitter_result->statuses.front( ).user.id == 1186275104,
			             "Missing value" );
		}

		// options::CheckedParseMode::no
		(void)daw::bench_n_test_mbs<DAW_NUM_RUNS>(
		  "twitter bench(unchecked)", sz,
		  [&twitter_result]( auto f1 ) {
			  twitter_result = daw::json::from_json<daw::twitter::twitter_object_t>(
			    f1, parse_flags<CheckedParseMode::no, ExecMode> );
			  daw::do_not_optimize( twitter_result );
		  },
		  json_data );
		daw::do_not_optimize( twitter_result );
		if( do_asserts ) {
			test_assert( twitter_result, "Missing value" );
			test_assert( not twitter_result->statuses.empty( ), "Expected values" );
			test_assert( twitter_result->statuses.front( ).user.id == 1186275104,
			             "Missing value" );
		}
		// CppCommentSkippingPolicyChecked
		(void)daw::bench_n_test_mbs<DAW_NUM_RUNS>(
		  "twitter bench(cpp comments)", sz,
		  [&twitter_result]( auto f1 ) {
			  twitter_result = daw::json::from_json<daw::twitter::twitter_object_t>(
			    f1, parse_flags<ExecMode, PolicyCommentTypes::cpp> );
			  daw::do_not_optimize( twitter_result );
		  },
		  json_data );
		daw::do_not_optimize( twitter_result );
		if( do_asserts ) {
			test_assert( twitter_result, "Missing value" );
			test_assert( not twitter_result->statuses.empty( ), "Expected values" );
			test_assert( twitter_result->statuses.front( ).user.id == 1186275104,
			             "Missing value" );
		}
#if not defined( _MSC_VER ) or defined( __clang__ )
		// CppCommentSkippingPolicyUnchecked
		(void)daw::bench_n_test_mbs<DAW_NUM_RUNS>(
		  "twitter bench(cpp comments, unchecked)", sz,
		  [&twitter_result]( auto f1 ) {
			  twitter_result = daw::json::from_json<daw::twitter::twitter_object_t>(
			    f1, parse_flags<ExecMode, CheckedParseMode::no,
			                    PolicyCommentTypes::cpp> );
			  daw::do_not_optimize( twitter_result );
		  },
		  json_data );
		daw::do_not_optimize( twitter_result );
		if( do_asserts ) {
			test_assert( twitter_result, "Missing value" );
			test_assert( not twitter_result->statuses.empty( ), "Expected values" );
			test_assert( twitter_result->statuses.front( ).user.id == 1186275104,
			             "Missing value" );
		}
#endif
		// HashCommentSkippingPolicyChecked
		(void)daw::bench_n_test_mbs<DAW_NUM_RUNS>(
		  "twitter bench(hash comments)", sz,
		  [&twitter_result]( auto f1 ) {
			  twitter_result =
			    daw::json::from_json<daw::twitter::twitter_object_t>( f1 );
			  daw::do_not_optimize( twitter_result,
			                        parse_flags<ExecMode, PolicyCommentTypes::hash> );
		  },
		  json_data );
		daw::do_not_optimize( twitter_result );
		if( do_asserts ) {
			test_assert( twitter_result, "Missing value" );
			test_assert( not twitter_result->statuses.empty( ), "Expected values" );
			test_assert( twitter_result->statuses.front( ).user.id == 1186275104,
			             "Missing value" );
		}
#if not defined( _MSC_VER ) or defined( __clang__ )
		// HashCommentSkippingPolicyUnchecked
		(void)daw::bench_n_test_mbs<DAW_NUM_RUNS>(
		  "twitter bench(hash comments, unchecked)", sz,
		  [&twitter_result]( auto f1 ) {
			  twitter_result = daw::json::from_json<daw::twitter::twitter_object_t>(
			    f1, parse_flags<ExecMode, CheckedParseMode::no,
			                    PolicyCommentTypes::hash> );
			  daw::do_not_optimize( twitter_result );
		  },
		  json_data );
		daw::do_not_optimize( twitter_result );
		if( do_asserts ) {
			test_assert( twitter_result, "Missing value" );
			test_assert( not twitter_result->statuses.empty( ), "Expected values" );
			test_assert( twitter_result->statuses.front( ).user.id == 1186275104,
			             "Missing value" );
		}
#endif
		// ******************************
		// Escaped Names
		(void)daw::bench_n_test_mbs<DAW_NUM_RUNS>(
		  "twitter bench(checked, escaped names)", sz,
		  [&twitter_result]( auto f1 ) {
			  twitter_result = daw::json::from_json<daw::twitter::twitter_object_t>(
			    f1, parse_flags<ExecMode, AllowEscapedNames::yes> );
			  daw::do_not_optimize( twitter_result );
		  },
		  json_data );
		daw::do_not_optimize( twitter_result );
		if( do_asserts ) {
			test_assert( twitter_result, "Missing value" );
			test_assert( not twitter_result->statuses.empty( ), "Expected values" );
			test_assert( twitter_result->statuses.front( ).user.id == 1186275104,
			             "Missing value" );
		}

		// options::CheckedParseMode::no Escaped Names
		(void)daw::bench_n_test_mbs<DAW_NUM_RUNS>(
		  "twitter bench(unchecked, escaped names)", sz,
		  [&twitter_result]( auto f1 ) {
			  twitter_result = daw::json::from_json<daw::twitter::twitter_object_t>(
			    f1, parse_flags<ExecMode, CheckedParseMode::no,
			                    PolicyCommentTypes::cpp, AllowEscapedNames::yes> );
			  daw::do_not_optimize( twitter_result );
		  },
		  json_data );
		daw::do_not_optimize( twitter_result );
		if( do_asserts ) {
			test_assert( twitter_result, "Missing value" );
			test_assert( not twitter_result->statuses.empty( ), "Expected values" );
			test_assert( twitter_result->statuses.front( ).user.id == 1186275104,
			             "Missing value" );
		}
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
} // namespace

int main( int argc, char **argv )
#ifdef DAW_USE_EXCEPTIONS
  try
#endif
{
	using namespace daw::json;
	if( argc < 2 ) {
		std::cerr << "Must supply a filenames to open\n";
		exit( 1 );
	}
	bool const do_asserts = [&] {
		if( argc > 2 ) {
			std::string_view arg2 = argv[2];
			return arg2 != "noassert";
		}
		return true;
	}( );
	std::string const json_data = [argv] {
		auto const mmf = *daw::read_file( argv[1] );
		test_assert( mmf.size( ) > 2, "Minimum json data size is 2 '{}'" );
		return std::string( mmf.data( ), mmf.size( ) );
	}( );

	auto const sz = json_data.size( );
	std::cout << "Processing: " << daw::utility::to_bytes_per_second( sz )
	          << '\n';
	test<ExecModeTypes::compile_time>( json_data, do_asserts );
	test<ExecModeTypes::runtime>( json_data, do_asserts );
	if constexpr( not std::is_same_v<runtime_exec_tag, simd_exec_tag> ) {
		test<ExecModeTypes::simd>( json_data, do_asserts );
	}

	// ******************************
	// Test serialization
	std::optional<daw::twitter::twitter_object_t> twitter_result =
	  daw::json::from_json<daw::twitter::twitter_object_t>( json_data );
	std::string str{ };
	(void)daw::bench_n_test_mbs<DAW_NUM_RUNS>(
	  "twitter bench(to_json_string)", sz,
	  [&]( auto const &tr ) {
		  str.clear( );
		  daw::json::to_json( *tr, str );
		  daw::do_not_optimize( str );
	  },
	  twitter_result );
	if( do_asserts ) {
		test_assert( not str.empty( ), "Expected a string value" );
	}
	daw::do_not_optimize( str );
	auto const twitter_result2 =
	  daw::json::from_json<daw::twitter::twitter_object_t>( str );
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