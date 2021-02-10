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

#include <daw/daw_read_file.h>

#include <benchmark/benchmark.h>
#include <fstream>
#include <iostream>
#include <streambuf>
#include <string_view>

static void benchmark_nativejson( benchmark::State &state ) {
	using namespace daw::json;

	auto const mm_twitter = *daw::read_file( "../test_data/twitter.json" );
	auto const mm_citm = *daw::read_file( "../test_data/citm_catalog.json" );
	auto const mm_canada = *daw::read_file( "../test_data/canada.json" );
	auto const sv_twitter =
	  std::string_view( mm_twitter.data( ), mm_twitter.size( ) );
	auto const sv_citm = std::string_view( mm_citm.data( ), mm_citm.size( ) );
	auto const sv_canada =
	  std::string_view( mm_canada.data( ), mm_canada.size( ) );

	std::optional<daw::twitter::twitter_object_t> j1{ };
	std::optional<daw::citm::citm_object_t> j2{ };
	std::optional<daw::geojson::FeatureCollection> j3{ };
	std::cout << "non-debug run\n";
	auto const test_func = [&]( auto f1, auto f2, auto f3 ) {
		j1 = daw::json::from_json<daw::twitter::twitter_object_t,
		                          NoCommentSkippingPolicyChecked>( f1 );
		j2 = daw::json::from_json<daw::citm::citm_object_t,
		                          NoCommentSkippingPolicyChecked>( f2 );
		j3 = daw::json::from_json<daw::geojson::FeatureCollection,
		                          NoCommentSkippingPolicyChecked>( f3 );
		benchmark::DoNotOptimize( j1 );
		benchmark::DoNotOptimize( j2 );
		benchmark::DoNotOptimize( j3 );
	};

	for( auto s : state ) {
		test_func( sv_twitter, sv_citm, sv_canada );
		benchmark::DoNotOptimize( sv_twitter );
		benchmark::DoNotOptimize( sv_citm );
		benchmark::DoNotOptimize( sv_canada );
	}
	state.SetBytesProcessed( static_cast<int64_t>( state.iterations( ) ) *
	                         static_cast<int64_t>( sv_twitter.size( ) +
	                                               sv_citm.size( ) +
	                                               sv_canada.size( ) ) );
	state.SetItemsProcessed( state.iterations() * 3 );
	test_assert( j1, "Missing value" );
	test_assert( j2, "Missing value" );
	test_assert( j3, "Missing value" );
}

BENCHMARK( benchmark_nativejson );

BENCHMARK_MAIN( );