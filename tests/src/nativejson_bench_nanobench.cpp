// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/daw_json_link
//

#include "citm_test_json.h"
#include "daw_json_benchmark.h"
#include "geojson_json.h"
#include "twitter_test_json.h"

#include <daw/daw_read_file.h>
#include <daw/json/daw_json_link.h>

#include <cstdlib>
#include <iostream>
#include <string>
#include <string_view>

#if not defined( DAW_NUM_RUNS )
#if not defined( DEBUG ) or defined( NDEBUG )
static inline constexpr std::size_t DAW_NUM_RUNS = 100;
#else
static inline constexpr std::size_t DAW_NUM_RUNS = 2;
#endif
#endif
static_assert( DAW_NUM_RUNS > 0 );

using namespace daw::json::options;
using constexpr_checked_pol = parse_flags_t<ExecModeTypes::compile_time>;
using constexpr_unchecked_pol =
  parse_flags_t<ExecModeTypes::compile_time, CheckedParseMode::no>;

using PPair = std::pair<std::string_view, std::string_view>;

template<typename ParseObj, typename ParsePolicy>
struct parser_t {
	constexpr parser_t( ) = default;

	bool operator( )( std::string_view jd ) const {
		daw::do_not_optimize(
		  daw::json::from_json<ParseObj>( jd, ParsePolicy{ } ) );
		return true;
	}

	bool operator( )( PPair const &jd ) const {
		daw::do_not_optimize(
		  daw::json::from_json<ParseObj>( jd.first, jd.second, ParsePolicy{ } ) );
		return true;
	}
};

template<typename ParsePolicy, typename... ParseObjs, typename... JsonDocs>
static void do_test( JsonDocs const &...jds ) try {
	using namespace daw::json;

	(void)( parser_t<ParseObjs, ParsePolicy>{ }( jds ) and ... );

} catch( daw::json::json_exception const &jex ) {
	std::cerr << "Exception thrown by parser: " << jex.reason( ) << std::endl;
	exit( 1 );
}

template<typename ParsePolicy>
void bench( std::string_view title, std::string const &twitter_doc,
            std::string const &citm_doc, std::string const &canada_doc ) {
	using daw::json::benchmark::benchmark;

	// Constexpr Checked
	auto const twitter_title = std::string( "twitter " ) + std::string( title );
	(void)benchmark( DAW_NUM_RUNS, twitter_doc.size( ), twitter_title, [&]( ) {
		do_test<ParsePolicy, daw::twitter::twitter_object_t>( twitter_doc );
	} );

	auto const citm_title = std::string( "citm " ) + std::string( title );
	(void)benchmark( DAW_NUM_RUNS, citm_doc.size( ), citm_title, [&]( ) {
		do_test<ParsePolicy, daw::citm::citm_object_t>( citm_doc );
	} );

	auto const canada_title = std::string( "canada " ) + std::string( title );
	(void)benchmark( DAW_NUM_RUNS, canada_doc.size( ), canada_title, [&] {
		do_test<ParsePolicy, daw::geojson::Polygon>(
		  PPair{ canada_doc, "features[0].geometry" } );
	} );

	auto const nativejson_title =
	  std::string( "nativejson " ) + std::string( title );
	(void)benchmark( DAW_NUM_RUNS,
	                 twitter_doc.size( ) + citm_doc.size( ) + canada_doc.size( ),
	                 nativejson_title,
	                 [&] {
		                 do_test<ParsePolicy, daw::twitter::twitter_object_t>(
		                   twitter_doc );
		                 do_test<ParsePolicy, daw::citm::citm_object_t>( citm_doc );
		                 do_test<ParsePolicy, daw::geojson::Polygon>(
		                   PPair{ canada_doc, "features[0].geometry" } );
	                 } );
	// Constexpr Unchecked
	// Runtime Checked
	// Runtime Unchecked
	// SIMD Checked
	// SIMD Unchecked
}

int main( int argc, char **argv ) {
	if( argc < 4 ) {
		std::cerr << "Must supply a filenames to open\n";
		std::cerr << "twitter citm canada\n";
		exit( 1 );
	}

	std::string const twitter_doc = daw::read_file( argv[1] ).value( );
	std::string const citm_doc = daw::read_file( argv[2] ).value( );
	std::string const canada_doc = daw::read_file( argv[3] ).value( );

	bench<constexpr_checked_pol>(
	  "constexpr checked", twitter_doc, citm_doc, canada_doc );
	bench<constexpr_unchecked_pol>(
	  "constexpr unchecked", twitter_doc, citm_doc, canada_doc );
}
