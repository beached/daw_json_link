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
#include <string_view>

auto const mm_twitter = *daw::read_file( "../test_data/twitter.json" );
auto const mm_citm = *daw::read_file( "../test_data/citm_catalog.json" );
auto const mm_canada = *daw::read_file( "../test_data/canada.json" );

//****************** Constexpr ****************************
static void bm_twitter_constexpr( benchmark::State &state ) try {
	using namespace daw::json;

	for( auto s : state ) {
		auto j1 =
		  daw::json::from_json<daw::twitter::twitter_object_t,
		                       NoCommentSkippingPolicyChecked>( mm_twitter );
		benchmark::DoNotOptimize( j1 );
	}
	state.SetBytesProcessed( static_cast<int64_t>( state.iterations( ) ) *
	                         static_cast<int64_t>( mm_twitter.size( ) ) );
	state.SetItemsProcessed( state.iterations( ) );
} catch( daw::json::json_exception const &jex ) {
	std::cerr << "Exception thrown by parser: " << jex.reason( ) << std::endl;
	exit( 1 );
}

BENCHMARK( bm_twitter_constexpr );

static void bm_citm_constexpr( benchmark::State &state ) try {
	using namespace daw::json;

	for( auto s : state ) {
		auto j1 = daw::json::from_json<daw::citm::citm_object_t,
		                               NoCommentSkippingPolicyChecked>( mm_citm );
		benchmark::DoNotOptimize( j1 );
	}
	state.SetBytesProcessed( static_cast<int64_t>( state.iterations( ) ) *
	                         static_cast<int64_t>( mm_citm.size( ) ) );
	state.SetItemsProcessed( state.iterations( ) );
} catch( daw::json::json_exception const &jex ) {
	std::cerr << "Exception thrown by parser: " << jex.reason( ) << std::endl;
	exit( 1 );
}

BENCHMARK( bm_citm_constexpr );

static void bm_canada_constexpr( benchmark::State &state ) try {
	using namespace daw::json;

	for( auto s : state ) {
		auto j1 = daw::json::from_json<daw::geojson::FeatureCollection,
		                               NoCommentSkippingPolicyChecked>( mm_canada );
		benchmark::DoNotOptimize( j1 );
	}
	state.SetBytesProcessed( static_cast<int64_t>( state.iterations( ) ) *
	                         static_cast<int64_t>( mm_canada.size( ) ) );
	state.SetItemsProcessed( state.iterations( ) );
} catch( daw::json::json_exception const &jex ) {
	std::cerr << "Exception thrown by parser: " << jex.reason( ) << std::endl;
	exit( 1 );
}

BENCHMARK( bm_canada_constexpr );

static void bm_nativejson_constexpr( benchmark::State &state ) try {
	using namespace daw::json;

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
} catch( daw::json::json_exception const &jex ) {
	std::cerr << "Exception thrown by parser: " << jex.reason( ) << std::endl;
	exit( 1 );
}

BENCHMARK( bm_nativejson_constexpr );

static void bm_twitter_constexpr_unchecked( benchmark::State &state ) try {
	using namespace daw::json;

	for( auto s : state ) {
		auto j1 =
		  daw::json::from_json<daw::twitter::twitter_object_t,
		                       NoCommentSkippingPolicyUnchecked>( mm_twitter );
		benchmark::DoNotOptimize( j1 );
	}
	state.SetBytesProcessed( static_cast<int64_t>( state.iterations( ) ) *
	                         static_cast<int64_t>( mm_twitter.size( ) ) );
	state.SetItemsProcessed( state.iterations( ) );
} catch( daw::json::json_exception const &jex ) {
	std::cerr << "Exception thrown by parser: " << jex.reason( ) << std::endl;
	exit( 1 );
}

BENCHMARK( bm_twitter_constexpr_unchecked );

static void bm_citm_constexpr_unchecked( benchmark::State &state ) try {
	using namespace daw::json;

	for( auto s : state ) {
		auto j1 = daw::json::from_json<daw::citm::citm_object_t,
		                               NoCommentSkippingPolicyUnchecked>( mm_citm );
		benchmark::DoNotOptimize( j1 );
	}
	state.SetBytesProcessed( static_cast<int64_t>( state.iterations( ) ) *
	                         static_cast<int64_t>( mm_citm.size( ) ) );
	state.SetItemsProcessed( state.iterations( ) );
} catch( daw::json::json_exception const &jex ) {
	std::cerr << "Exception thrown by parser: " << jex.reason( ) << std::endl;
	exit( 1 );
}

BENCHMARK( bm_citm_constexpr_unchecked );

static void bm_canada_constexpr_unchecked( benchmark::State &state ) try {
	using namespace daw::json;

	for( auto s : state ) {
		auto j1 =
		  daw::json::from_json<daw::geojson::FeatureCollection,
		                       NoCommentSkippingPolicyUnchecked>( mm_canada );
		benchmark::DoNotOptimize( j1 );
	}
	state.SetBytesProcessed( static_cast<int64_t>( state.iterations( ) ) *
	                         static_cast<int64_t>( mm_canada.size( ) ) );
	state.SetItemsProcessed( state.iterations( ) );
} catch( daw::json::json_exception const &jex ) {
	std::cerr << "Exception thrown by parser: " << jex.reason( ) << std::endl;
	exit( 1 );
}

BENCHMARK( bm_canada_constexpr_unchecked );

static void bm_nativejson_constexpr_unchecked( benchmark::State &state ) try {
	using namespace daw::json;

	for( auto s : state ) {
		auto j1 =
		  daw::json::from_json<daw::twitter::twitter_object_t,
		                       NoCommentSkippingPolicyUnchecked>( mm_twitter );
		auto j2 = daw::json::from_json<daw::citm::citm_object_t,
		                               NoCommentSkippingPolicyUnchecked>( mm_citm );
		auto j3 =
		  daw::json::from_json<daw::geojson::FeatureCollection,
		                       NoCommentSkippingPolicyUnchecked>( mm_canada );
		benchmark::DoNotOptimize( j1 );
		benchmark::DoNotOptimize( j2 );
		benchmark::DoNotOptimize( j3 );
	}
	state.SetBytesProcessed( static_cast<int64_t>( state.iterations( ) ) *
	                         static_cast<int64_t>( mm_twitter.size( ) +
	                                               mm_citm.size( ) +
	                                               mm_canada.size( ) ) );
	state.SetItemsProcessed( state.iterations( ) * 3 );
} catch( daw::json::json_exception const &jex ) {
	std::cerr << "Exception thrown by parser: " << jex.reason( ) << std::endl;
	exit( 1 );
}

BENCHMARK( bm_nativejson_constexpr_unchecked );
//****************** Runtime ****************************
using RuntimePolicyChecked =
  daw::json::SIMDNoCommentSkippingPolicyChecked<daw::json::runtime_exec_tag>;
using RuntimePolicyUnchecked =
  daw::json::SIMDNoCommentSkippingPolicyUnchecked<daw::json::runtime_exec_tag>;

static void bm_twitter_runtime( benchmark::State &state ) try {
	using namespace daw::json;

	for( auto s : state ) {
		auto j1 = daw::json::from_json<daw::twitter::twitter_object_t,
		                               RuntimePolicyChecked>( mm_twitter );
		benchmark::DoNotOptimize( j1 );
	}
	state.SetBytesProcessed( static_cast<int64_t>( state.iterations( ) ) *
	                         static_cast<int64_t>( mm_twitter.size( ) ) );
	state.SetItemsProcessed( state.iterations( ) );
} catch( daw::json::json_exception const &jex ) {
	std::cerr << "Exception thrown by parser: " << jex.reason( ) << std::endl;
	exit( 1 );
}

BENCHMARK( bm_twitter_runtime );

static void bm_citm_runtime( benchmark::State &state ) try {
	using namespace daw::json;

	for( auto s : state ) {
		auto j1 =
		  daw::json::from_json<daw::citm::citm_object_t, RuntimePolicyChecked>(
		    mm_citm );
		benchmark::DoNotOptimize( j1 );
	}
	state.SetBytesProcessed( static_cast<int64_t>( state.iterations( ) ) *
	                         static_cast<int64_t>( mm_citm.size( ) ) );
	state.SetItemsProcessed( state.iterations( ) );
} catch( daw::json::json_exception const &jex ) {
	std::cerr << "Exception thrown by parser: " << jex.reason( ) << std::endl;
	exit( 1 );
}

BENCHMARK( bm_citm_runtime );

static void bm_canada_runtime( benchmark::State &state ) try {
	using namespace daw::json;

	for( auto s : state ) {
		auto j1 = daw::json::from_json<daw::geojson::FeatureCollection,
		                               RuntimePolicyChecked>( mm_canada );
		benchmark::DoNotOptimize( j1 );
	}
	state.SetBytesProcessed( static_cast<int64_t>( state.iterations( ) ) *
	                         static_cast<int64_t>( mm_canada.size( ) ) );
	state.SetItemsProcessed( state.iterations( ) );
} catch( daw::json::json_exception const &jex ) {
	std::cerr << "Exception thrown by parser: " << jex.reason( ) << std::endl;
	exit( 1 );
}

BENCHMARK( bm_canada_runtime );

static void bm_nativejson_runtime( benchmark::State &state ) try {
	using namespace daw::json;

	for( auto s : state ) {
		auto j1 = daw::json::from_json<daw::twitter::twitter_object_t,
		                               RuntimePolicyChecked>( mm_twitter );
		auto j2 =
		  daw::json::from_json<daw::citm::citm_object_t, RuntimePolicyChecked>(
		    mm_citm );
		auto j3 = daw::json::from_json<daw::geojson::FeatureCollection,
		                               RuntimePolicyChecked>( mm_canada );
		benchmark::DoNotOptimize( j1 );
		benchmark::DoNotOptimize( j2 );
		benchmark::DoNotOptimize( j3 );
	}
	state.SetBytesProcessed( static_cast<int64_t>( state.iterations( ) ) *
	                         static_cast<int64_t>( mm_twitter.size( ) +
	                                               mm_citm.size( ) +
	                                               mm_canada.size( ) ) );
	state.SetItemsProcessed( state.iterations( ) * 3 );
} catch( daw::json::json_exception const &jex ) {
	std::cerr << "Exception thrown by parser: " << jex.reason( ) << std::endl;
	exit( 1 );
}

BENCHMARK( bm_nativejson_runtime );

static void bm_twitter_runtime_unchecked( benchmark::State &state ) try {
	using namespace daw::json;

	for( auto s : state ) {
		auto j1 = daw::json::from_json<daw::twitter::twitter_object_t,
		                               RuntimePolicyUnchecked>( mm_twitter );
		benchmark::DoNotOptimize( j1 );
	}
	state.SetBytesProcessed( static_cast<int64_t>( state.iterations( ) ) *
	                         static_cast<int64_t>( mm_twitter.size( ) ) );
	state.SetItemsProcessed( state.iterations( ) );
} catch( daw::json::json_exception const &jex ) {
	std::cerr << "Exception thrown by parser: " << jex.reason( ) << std::endl;
	exit( 1 );
}

BENCHMARK( bm_twitter_runtime_unchecked );

static void bm_citm_runtime_unchecked( benchmark::State &state ) try {
	using namespace daw::json;

	for( auto s : state ) {
		auto j1 =
		  daw::json::from_json<daw::citm::citm_object_t, RuntimePolicyUnchecked>(
		    mm_citm );
		benchmark::DoNotOptimize( j1 );
	}
	state.SetBytesProcessed( static_cast<int64_t>( state.iterations( ) ) *
	                         static_cast<int64_t>( mm_citm.size( ) ) );
	state.SetItemsProcessed( state.iterations( ) );
} catch( daw::json::json_exception const &jex ) {
	std::cerr << "Exception thrown by parser: " << jex.reason( ) << std::endl;
	exit( 1 );
}

BENCHMARK( bm_citm_runtime_unchecked );

static void bm_canada_runtime_unchecked( benchmark::State &state ) try {
	using namespace daw::json;

	for( auto s : state ) {
		auto j1 = daw::json::from_json<daw::geojson::FeatureCollection,
		                               RuntimePolicyUnchecked>( mm_canada );
		benchmark::DoNotOptimize( j1 );
	}
	state.SetBytesProcessed( static_cast<int64_t>( state.iterations( ) ) *
	                         static_cast<int64_t>( mm_canada.size( ) ) );
	state.SetItemsProcessed( state.iterations( ) );
} catch( daw::json::json_exception const &jex ) {
	std::cerr << "Exception thrown by parser: " << jex.reason( ) << std::endl;
	exit( 1 );
}

BENCHMARK( bm_canada_runtime_unchecked );

static void bm_nativejson_runtime_unchecked( benchmark::State &state ) try {
	using namespace daw::json;

	for( auto s : state ) {
		auto j1 = daw::json::from_json<daw::twitter::twitter_object_t,
		                               RuntimePolicyUnchecked>( mm_twitter );
		auto j2 =
		  daw::json::from_json<daw::citm::citm_object_t, RuntimePolicyUnchecked>(
		    mm_citm );
		auto j3 = daw::json::from_json<daw::geojson::FeatureCollection,
		                               RuntimePolicyUnchecked>( mm_canada );
		benchmark::DoNotOptimize( j1 );
		benchmark::DoNotOptimize( j2 );
		benchmark::DoNotOptimize( j3 );
	}
	state.SetBytesProcessed( static_cast<int64_t>( state.iterations( ) ) *
	                         static_cast<int64_t>( mm_twitter.size( ) +
	                                               mm_citm.size( ) +
	                                               mm_canada.size( ) ) );
	state.SetItemsProcessed( state.iterations( ) * 3 );
} catch( daw::json::json_exception const &jex ) {
	std::cerr << "Exception thrown by parser: " << jex.reason( ) << std::endl;
	exit( 1 );
}

BENCHMARK( bm_nativejson_runtime_unchecked );

//****************** SIMD ****************************
using SIMDPolicyChecked =
  daw::json::SIMDNoCommentSkippingPolicyChecked<daw::json::simd_exec_tag>;
using SIMDPolicyUnchecked =
  daw::json::SIMDNoCommentSkippingPolicyUnchecked<daw::json::simd_exec_tag>;

static void bm_twitter_simd( benchmark::State &state ) try {
	using namespace daw::json;

	for( auto s : state ) {
		auto j1 =
		  daw::json::from_json<daw::twitter::twitter_object_t, SIMDPolicyChecked>(
		    mm_twitter );
		benchmark::DoNotOptimize( j1 );
	}
	state.SetBytesProcessed( static_cast<int64_t>( state.iterations( ) ) *
	                         static_cast<int64_t>( mm_twitter.size( ) ) );
	state.SetItemsProcessed( state.iterations( ) );
} catch( daw::json::json_exception const &jex ) {
	std::cerr << "Exception thrown by parser: " << jex.reason( ) << std::endl;
	exit( 1 );
}

BENCHMARK( bm_twitter_simd );

static void bm_citm_simd( benchmark::State &state ) try {
	using namespace daw::json;

	for( auto s : state ) {
		auto j1 = daw::json::from_json<daw::citm::citm_object_t, SIMDPolicyChecked>(
		  mm_citm );
		benchmark::DoNotOptimize( j1 );
	}
	state.SetBytesProcessed( static_cast<int64_t>( state.iterations( ) ) *
	                         static_cast<int64_t>( mm_citm.size( ) ) );
	state.SetItemsProcessed( state.iterations( ) );
} catch( daw::json::json_exception const &jex ) {
	std::cerr << "Exception thrown by parser: " << jex.reason( ) << std::endl;
	exit( 1 );
}

BENCHMARK( bm_citm_simd );

static void bm_canada_simd( benchmark::State &state ) try {
	using namespace daw::json;

	for( auto s : state ) {
		auto j1 =
		  daw::json::from_json<daw::geojson::FeatureCollection, SIMDPolicyChecked>(
		    mm_canada );
		benchmark::DoNotOptimize( j1 );
	}
	state.SetBytesProcessed( static_cast<int64_t>( state.iterations( ) ) *
	                         static_cast<int64_t>( mm_canada.size( ) ) );
	state.SetItemsProcessed( state.iterations( ) );
} catch( daw::json::json_exception const &jex ) {
	std::cerr << "Exception thrown by parser: " << jex.reason( ) << std::endl;
	exit( 1 );
}

BENCHMARK( bm_canada_simd );

static void bm_nativejson_simd( benchmark::State &state ) try {
	using namespace daw::json;

	for( auto s : state ) {
		auto j1 =
		  daw::json::from_json<daw::twitter::twitter_object_t, SIMDPolicyChecked>(
		    mm_twitter );
		auto j2 = daw::json::from_json<daw::citm::citm_object_t, SIMDPolicyChecked>(
		  mm_citm );
		auto j3 =
		  daw::json::from_json<daw::geojson::FeatureCollection, SIMDPolicyChecked>(
		    mm_canada );
		benchmark::DoNotOptimize( j1 );
		benchmark::DoNotOptimize( j2 );
		benchmark::DoNotOptimize( j3 );
	}
	state.SetBytesProcessed( static_cast<int64_t>( state.iterations( ) ) *
	                         static_cast<int64_t>( mm_twitter.size( ) +
	                                               mm_citm.size( ) +
	                                               mm_canada.size( ) ) );
	state.SetItemsProcessed( state.iterations( ) * 3 );
} catch( daw::json::json_exception const &jex ) {
	std::cerr << "Exception thrown by parser: " << jex.reason( ) << std::endl;
	exit( 1 );
}

BENCHMARK( bm_nativejson_simd );

static void bm_twitter_simd_unchecked( benchmark::State &state ) try {
	using namespace daw::json;

	for( auto s : state ) {
		auto j1 =
		  daw::json::from_json<daw::twitter::twitter_object_t, SIMDPolicyUnchecked>(
		    mm_twitter );
		benchmark::DoNotOptimize( j1 );
	}
	state.SetBytesProcessed( static_cast<int64_t>( state.iterations( ) ) *
	                         static_cast<int64_t>( mm_twitter.size( ) ) );
	state.SetItemsProcessed( state.iterations( ) );
} catch( daw::json::json_exception const &jex ) {
	std::cerr << "Exception thrown by parser: " << jex.reason( ) << std::endl;
	exit( 1 );
}

BENCHMARK( bm_twitter_simd_unchecked );

static void bm_citm_simd_unchecked( benchmark::State &state ) try {
	using namespace daw::json;

	for( auto s : state ) {
		auto j1 =
		  daw::json::from_json<daw::citm::citm_object_t, SIMDPolicyUnchecked>(
		    mm_citm );
		benchmark::DoNotOptimize( j1 );
	}
	state.SetBytesProcessed( static_cast<int64_t>( state.iterations( ) ) *
	                         static_cast<int64_t>( mm_citm.size( ) ) );
	state.SetItemsProcessed( state.iterations( ) );
} catch( daw::json::json_exception const &jex ) {
	std::cerr << "Exception thrown by parser: " << jex.reason( ) << std::endl;
	exit( 1 );
}

BENCHMARK( bm_citm_simd_unchecked );

static void bm_canada_simd_unchecked( benchmark::State &state ) try {
	using namespace daw::json;

	for( auto s : state ) {
		auto j1 = daw::json::from_json<daw::geojson::FeatureCollection,
		                               SIMDPolicyUnchecked>( mm_canada );
		benchmark::DoNotOptimize( j1 );
	}
	state.SetBytesProcessed( static_cast<int64_t>( state.iterations( ) ) *
	                         static_cast<int64_t>( mm_canada.size( ) ) );
	state.SetItemsProcessed( state.iterations( ) );
} catch( daw::json::json_exception const &jex ) {
	std::cerr << "Exception thrown by parser: " << jex.reason( ) << std::endl;
	exit( 1 );
}

BENCHMARK( bm_canada_simd_unchecked );

static void bm_nativejson_simd_unchecked( benchmark::State &state ) try {
	using namespace daw::json;

	for( auto s : state ) {
		auto j1 =
		  daw::json::from_json<daw::twitter::twitter_object_t, SIMDPolicyUnchecked>(
		    mm_twitter );
		auto j2 =
		  daw::json::from_json<daw::citm::citm_object_t, SIMDPolicyUnchecked>(
		    mm_citm );
		auto j3 = daw::json::from_json<daw::geojson::FeatureCollection,
		                               SIMDPolicyUnchecked>( mm_canada );
		benchmark::DoNotOptimize( j1 );
		benchmark::DoNotOptimize( j2 );
		benchmark::DoNotOptimize( j3 );
	}
	state.SetBytesProcessed( static_cast<int64_t>( state.iterations( ) ) *
	                         static_cast<int64_t>( mm_twitter.size( ) +
	                                               mm_citm.size( ) +
	                                               mm_canada.size( ) ) );
	state.SetItemsProcessed( state.iterations( ) * 3 );
} catch( daw::json::json_exception const &jex ) {
	std::cerr << "Exception thrown by parser: " << jex.reason( ) << std::endl;
	exit( 1 );
}

BENCHMARK( bm_nativejson_simd_unchecked );

BENCHMARK_MAIN( );
