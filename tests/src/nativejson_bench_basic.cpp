// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/daw_json_link
//

#include "defines.h"

#include "citm_test_json.h"
#include "geojson.h"
#include "twitter_test_json.h"

#include "daw/json/daw_json_link.h"

#include <daw/daw_benchmark.h>
#include <daw/daw_read_file.h>

#include <fstream>
#include <iostream>
#include <streambuf>
#include <string_view>

#if not defined( DAW_NUM_RUNS )
#if not defined( DEBUG ) or defined( NDEBUG )
static inline constexpr std::size_t DAW_NUM_RUNS = 250;
#else
static inline constexpr std::size_t DAW_NUM_RUNS = 1;
#endif
#endif
static_assert( DAW_NUM_RUNS > 0 );

int main( int argc, char **argv ) try {
	using namespace daw::json;
	if( argc < 4 ) {
		std::cerr << "Must supply a filenames to open\n";
		std::cerr << "twitter citm canada\n";
		exit( 1 );
	}

	auto const mm_twitter = *daw::read_file( argv[1] );
	auto const mm_citm = *daw::read_file( argv[2] );
	auto const mm_canada = *daw::read_file( argv[3] );
	auto const sv_twitter =
	  std::string_view( mm_twitter.data( ), mm_twitter.size( ) );
	auto const sv_citm = std::string_view( mm_citm.data( ), mm_citm.size( ) );
	auto const sv_canada =
	  std::string_view( mm_canada.data( ), mm_canada.size( ) );

	std::optional<daw::twitter::twitter_object_t> j1{ };
	std::optional<daw::citm::citm_object_t> j2{ };
	std::optional<daw::geojson::FeatureCollection> j3{ };
#ifdef NDEBUG
	std::cout << "non-debug run\n";
	auto const sz = sv_twitter.size( ) + sv_citm.size( ) + sv_canada.size( );
	daw::bench_n_test_mbs<DAW_NUM_RUNS>(
	  "nativejson bench", sz,
	  [&]( auto f1, auto f2, auto f3 ) {
		  j1 = daw::json::from_json<daw::twitter::twitter_object_t,
		                            NoCommentSkippingPolicyChecked>( f1 );
		  j2 = daw::json::from_json<daw::citm::citm_object_t,
		                            NoCommentSkippingPolicyChecked>( f2 );
		  j3 = daw::json::from_json<daw::geojson::FeatureCollection,
		                            NoCommentSkippingPolicyChecked>( f3 );
		  daw::do_not_optimize( sv_twitter );
		  daw::do_not_optimize( sv_citm );
		  daw::do_not_optimize( sv_canada );
		  daw::do_not_optimize( j1 );
		  daw::do_not_optimize( j2 );
		  daw::do_not_optimize( j3 );
	  },
	  sv_twitter, sv_citm, sv_canada );
#else
	for( size_t n = 0; n < 25; ++n ) {
		j1 = daw::json::from_json<daw::twitter::twitter_object_t,
		                          NoCommentSkippingPolicyChecked>( sv_twitter );
		j2 = daw::json::from_json<daw::citm::citm_object_t,
		                          NoCommentSkippingPolicyChecked>( sv_citm );
		j3 = daw::json::from_json<daw::geojson::FeatureCollection,
		                          NoCommentSkippingPolicyChecked>( sv_canada );
		daw::do_not_optimize( sv_twitter );
		daw::do_not_optimize( sv_citm );
		daw::do_not_optimize( sv_canada );
		daw::do_not_optimize( j1 );
		daw::do_not_optimize( j2 );
		daw::do_not_optimize( j3 );
	}
#endif
	if( not j1 ) {
		daw_json_error( "Missing value" );
	}
	if( not j2 ) {
		daw_json_error( "Missing value" );
	}
	if( not j3 ) {
		daw_json_error( "Missing value" );
	}
} catch( daw::json::json_exception const &jex ) {
	std::cerr << "Exception thrown by parser: " << jex.reason( ) << std::endl;
	exit( 1 );
}
