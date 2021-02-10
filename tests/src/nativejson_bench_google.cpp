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

static void benchmark_twitter( benchmark::State &state ) {
	using namespace daw::json;

	auto const mm_twitter = *daw::read_file( "../test_data/twitter.json" );

	for( auto s : state ) {
		auto j1 =
		  daw::json::from_json<daw::twitter::twitter_object_t,
		                       NoCommentSkippingPolicyChecked>( mm_twitter );
		benchmark::DoNotOptimize( j1 );
	}
	state.SetBytesProcessed( static_cast<int64_t>( state.iterations( ) ) *
	                         static_cast<int64_t>( mm_twitter.size( ) ) );
	state.SetItemsProcessed( state.iterations( ) );
}

BENCHMARK( benchmark_twitter );

static void benchmark_citm_catalog( benchmark::State &state ) {
	using namespace daw::json;

	auto const mm_citm = *daw::read_file( "../test_data/citm_catalog.json" );

	for( auto s : state ) {
		auto j1 = daw::json::from_json<daw::citm::citm_object_t,
		                               NoCommentSkippingPolicyChecked>( mm_citm );
		benchmark::DoNotOptimize( j1 );
	}
	state.SetBytesProcessed( static_cast<int64_t>( state.iterations( ) ) *
	                         static_cast<int64_t>( mm_citm.size( ) ) );
	state.SetItemsProcessed( state.iterations( ) );
}

BENCHMARK( benchmark_citm_catalog );

static void benchmark_canada( benchmark::State &state ) {
	using namespace daw::json;

	auto const mm_canada = *daw::read_file( "../test_data/canada.json" );

	for( auto s : state ) {
		auto j1 = daw::json::from_json<daw::geojson::FeatureCollection,
		                               NoCommentSkippingPolicyChecked>( mm_canada );
		benchmark::DoNotOptimize( j1 );
	}
	state.SetBytesProcessed( static_cast<int64_t>( state.iterations( ) ) *
	                         static_cast<int64_t>( mm_canada.size( ) ) );
	state.SetItemsProcessed( state.iterations( ) );
}

BENCHMARK( benchmark_canada );

static void benchmark_nativejson( benchmark::State &state ) {
	using namespace daw::json;

	auto const mm_twitter = *daw::read_file( "../test_data/twitter.json" );
	auto const mm_citm = *daw::read_file( "../test_data/citm_catalog.json" );
	auto const mm_canada = *daw::read_file( "../test_data/canada.json" );

	for( auto s : state ) {
		auto j1 =
		  daw::json::from_json<daw::twitter::twitter_object_t,
		                       NoCommentSkippingPolicyChecked>( mm_twitter );
		auto j2 = daw::json::from_json<daw::citm::citm_object_t,
		                               NoCommentSkippingPolicyChecked>( mm_citm );
		auto j3 = daw::json::from_json<daw::geojson::FeatureCollection,
		                               NoCommentSkippingPolicyChecked>( mm_canada );
		benchmark::DoNotOptimize( j1 );
		benchmark::DoNotOptimize( j2 );
		benchmark::DoNotOptimize( j3 );
	}
	state.SetBytesProcessed( static_cast<int64_t>( state.iterations( ) ) *
	                         static_cast<int64_t>( mm_twitter.size( ) +
	                                               mm_citm.size( ) +
	                                               mm_canada.size( ) ) );
	state.SetItemsProcessed( state.iterations( ) * 3 );
}

BENCHMARK( benchmark_nativejson );

BENCHMARK_MAIN( );
