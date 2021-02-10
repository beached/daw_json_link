// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/daw_json_link
//

#include "citm_test_json.h"
#include "geojson.h"
#include "twitter_test_json.h"

#include "daw/json/daw_json_link.h"

#include <daw/daw_read_file.h>

#include <benchmark/benchmark.h>
#include <cstdint>
#include <string_view>

std::string const twitter_doc =
  daw::read_file( "../test_data/twitter.json" ).value( );
std::string const citm_doc =
  daw::read_file( "../test_data/citm_catalog.json" ).value( );
std::string const canada_doc =
  daw::read_file( "../test_data/canada.json" ).value( );

using cx_checked_pol =
  daw::json::SIMDNoCommentSkippingPolicyChecked<daw::json::constexpr_exec_tag>;
using cx_unchecked_pol = daw::json::SIMDNoCommentSkippingPolicyUnchecked<
  daw::json::constexpr_exec_tag>;
using runtime_checked_pol =
  daw::json::SIMDNoCommentSkippingPolicyChecked<daw::json::runtime_exec_tag>;
using runtime_unchecked_pol =
  daw::json::SIMDNoCommentSkippingPolicyUnchecked<daw::json::runtime_exec_tag>;
using simd_checked_pol =
  daw::json::SIMDNoCommentSkippingPolicyChecked<daw::json::simd_exec_tag>;
using simd_unchecked_pol =
  daw::json::SIMDNoCommentSkippingPolicyUnchecked<daw::json::simd_exec_tag>;

template<typename ParsePolicy, typename... ParseObjs, typename... JsonDocs>
static void do_test( benchmark::State &state, JsonDocs const &...jds ) try {
	using namespace daw::json;

	for( auto s : state ) {
		(void)( ( benchmark::DoNotOptimize(
		            daw::json::from_json<ParseObjs, ParsePolicy>( jds ) ),
		          true ) and
		        ... );
	}
	state.SetBytesProcessed( static_cast<std::int64_t>( state.iterations( ) ) *
	                         ( static_cast<std::int64_t>( jds.size( ) ) + ... ) );
	state.SetItemsProcessed( static_cast<std::int64_t>( state.iterations( ) ) );
} catch( daw::json::json_exception const &jex ) {
	std::cerr << "Exception thrown by parser: " << jex.reason( ) << std::endl;
	exit( 1 );
}

//****************** Constexpr Checked ****************************
static void bm_twitter_constexpr( benchmark::State &state ) {
	do_test<cx_checked_pol, daw::twitter::twitter_object_t>( state, twitter_doc );
}
BENCHMARK( bm_twitter_constexpr );

static void bm_citm_constexpr( benchmark::State &state ) {
	do_test<cx_checked_pol, daw::citm::citm_object_t>( state, citm_doc );
}
BENCHMARK( bm_citm_constexpr );

static void bm_canada_constexpr( benchmark::State &state ) {
	do_test<cx_checked_pol, daw::geojson::FeatureCollection>( state, canada_doc );
}
BENCHMARK( bm_canada_constexpr );

static void bm_nativejson_constexpr( benchmark::State &state ) {
	do_test<cx_checked_pol, daw::twitter::twitter_object_t,
	        daw::citm::citm_object_t, daw::geojson::FeatureCollection>(
	  state, twitter_doc, citm_doc, canada_doc );
}
BENCHMARK( bm_nativejson_constexpr );

//****************** Constexpr Unchecked ****************************
static void bm_twitter_constexpr_unchecked( benchmark::State &state ) {
	do_test<cx_unchecked_pol, daw::twitter::twitter_object_t>( state,
	                                                           twitter_doc );
}
BENCHMARK( bm_twitter_constexpr_unchecked );

static void bm_citm_constexpr_unchecked( benchmark::State &state ) {
	do_test<cx_unchecked_pol, daw::citm::citm_object_t>( state, citm_doc );
}
BENCHMARK( bm_citm_constexpr_unchecked );

static void bm_canada_constexpr_unchecked( benchmark::State &state ) {
	do_test<cx_unchecked_pol, daw::geojson::FeatureCollection>( state,
	                                                            canada_doc );
}
BENCHMARK( bm_canada_constexpr_unchecked );

static void bm_nativejson_constexpr_unchecked( benchmark::State &state ) {
	do_test<cx_unchecked_pol, daw::twitter::twitter_object_t,
	        daw::citm::citm_object_t, daw::geojson::FeatureCollection>(
	  state, twitter_doc, citm_doc, canada_doc );
}
BENCHMARK( bm_nativejson_constexpr_unchecked );

//****************** Runtime Checked ****************************
static void bm_twitter_runtime( benchmark::State &state ) {
	do_test<runtime_checked_pol, daw::twitter::twitter_object_t>( state,
	                                                              twitter_doc );
}
BENCHMARK( bm_twitter_runtime );

static void bm_citm_runtime( benchmark::State &state ) {
	do_test<runtime_checked_pol, daw::citm::citm_object_t>( state, citm_doc );
}
BENCHMARK( bm_citm_runtime );

static void bm_canada_runtime( benchmark::State &state ) {
	do_test<runtime_checked_pol, daw::geojson::FeatureCollection>( state,
	                                                               canada_doc );
}
BENCHMARK( bm_canada_runtime );

static void bm_nativejson_runtime( benchmark::State &state ) {
	do_test<runtime_checked_pol, daw::twitter::twitter_object_t,
	        daw::citm::citm_object_t, daw::geojson::FeatureCollection>(
	  state, twitter_doc, citm_doc, canada_doc );
}
BENCHMARK( bm_nativejson_runtime );

//****************** Runtime Unchecked ****************************
static void bm_twitter_runtime_unchecked( benchmark::State &state ) {
	do_test<runtime_unchecked_pol, daw::twitter::twitter_object_t>( state,
	                                                                twitter_doc );
}
BENCHMARK( bm_twitter_runtime_unchecked );

static void bm_citm_runtime_unchecked( benchmark::State &state ) {
	do_test<runtime_unchecked_pol, daw::citm::citm_object_t>( state, citm_doc );
}
BENCHMARK( bm_citm_runtime_unchecked );

static void bm_canada_runtime_unchecked( benchmark::State &state ) {
	do_test<runtime_unchecked_pol, daw::geojson::FeatureCollection>( state,
	                                                                 canada_doc );
}
BENCHMARK( bm_canada_runtime_unchecked );

static void bm_nativejson_runtime_unchecked( benchmark::State &state ) {
	do_test<runtime_unchecked_pol, daw::twitter::twitter_object_t,
	        daw::citm::citm_object_t, daw::geojson::FeatureCollection>(
	  state, twitter_doc, citm_doc, canada_doc );
}
BENCHMARK( bm_nativejson_runtime_unchecked );

//****************** SIMD Checked ****************************
static void bm_twitter_simd( benchmark::State &state ) {
	do_test<simd_checked_pol, daw::twitter::twitter_object_t>( state,
	                                                           twitter_doc );
}
BENCHMARK( bm_twitter_simd );

static void bm_citm_simd( benchmark::State &state ) {
	do_test<simd_checked_pol, daw::citm::citm_object_t>( state, citm_doc );
}
BENCHMARK( bm_citm_simd );

static void bm_canada_simd( benchmark::State &state ) {
	do_test<simd_checked_pol, daw::geojson::FeatureCollection>( state,
	                                                            canada_doc );
}
BENCHMARK( bm_canada_simd );

static void bm_nativejson_simd( benchmark::State &state ) {
	do_test<simd_checked_pol, daw::twitter::twitter_object_t,
	        daw::citm::citm_object_t, daw::geojson::FeatureCollection>(
	  state, twitter_doc, citm_doc, canada_doc );
}
BENCHMARK( bm_nativejson_simd );

//****************** SIMD Unchecked ****************************
static void bm_twitter_simd_unchecked( benchmark::State &state ) {
	do_test<simd_unchecked_pol, daw::twitter::twitter_object_t>( state,
	                                                             twitter_doc );
}
BENCHMARK( bm_twitter_simd_unchecked );

static void bm_citm_simd_unchecked( benchmark::State &state ) {
	do_test<simd_unchecked_pol, daw::citm::citm_object_t>( state, citm_doc );
}
BENCHMARK( bm_citm_simd_unchecked );

static void bm_canada_simd_unchecked( benchmark::State &state ) {
	do_test<simd_unchecked_pol, daw::geojson::FeatureCollection>( state,
	                                                              canada_doc );
}
BENCHMARK( bm_canada_simd_unchecked );

static void bm_nativejson_simd_unchecked( benchmark::State &state ) {
	do_test<simd_unchecked_pol, daw::twitter::twitter_object_t,
	        daw::citm::citm_object_t, daw::geojson::FeatureCollection>(
	  state, twitter_doc, citm_doc, canada_doc );
}
BENCHMARK( bm_nativejson_simd_unchecked );

BENCHMARK_MAIN( );
