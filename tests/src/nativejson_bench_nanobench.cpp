// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/daw_json_link
//

#include "citm_test_json.h"
#include "geojson_json.h"
#include "twitter_test_json.h"

#include <daw/daw_read_file.h>
#include <daw/json/daw_from_json.h>

#include <cstdlib>
#include <fmt/format.h>
#include <fstream>
#include <iostream>
#include <nanobench.h>
#include <string>
#include <string_view>

using constexpr_checked_pol =
  daw::json::SIMDNoCommentSkippingPolicyChecked<daw::json::constexpr_exec_tag>;
using constexpr_unchecked_pol = daw::json::SIMDNoCommentSkippingPolicyUnchecked<
  daw::json::constexpr_exec_tag>;
using runtime_checked_pol =
  daw::json::SIMDNoCommentSkippingPolicyChecked<daw::json::runtime_exec_tag>;
using runtime_unchecked_pol =
  daw::json::SIMDNoCommentSkippingPolicyUnchecked<daw::json::runtime_exec_tag>;
using simd_checked_pol =
  daw::json::SIMDNoCommentSkippingPolicyChecked<daw::json::simd_exec_tag>;
using simd_unchecked_pol =
  daw::json::SIMDNoCommentSkippingPolicyUnchecked<daw::json::simd_exec_tag>;

using PPair = std::pair<std::string_view, std::string_view>;

template<typename ParseObj, typename ParsePolicy>
struct parser_t {
	constexpr parser_t( ) = default;

	bool operator( )( std::string_view jd ) const {
		ankerl::nanobench::doNotOptimizeAway(
		  daw::json::from_json<ParseObj, ParsePolicy>( jd ) );
		return true;
	}

	bool operator( )( PPair const &jd ) const {
		ankerl::nanobench::doNotOptimizeAway(
		  daw::json::from_json<ParseObj, ParsePolicy>( jd.first, jd.second ) );
		return true;
	}
};

template<typename ParsePolicy, typename... ParseObjs, typename... JsonDocs>
static inline void do_test( JsonDocs const &...jds ) try {
	using namespace daw::json;

	(void)( parser_t<ParseObjs, ParsePolicy>{ }( jds ) and ... );

} catch( daw::json::json_exception const &jex ) {
	std::cerr << "Exception thrown by parser: " << jex.reason( ) << std::endl;
	exit( 1 );
}

template<typename ParsePolicy>
void bench( ankerl::nanobench::Bench &b, std::string_view title,
            std::string const &twitter_doc, std::string const &citm_doc,
            std::string const &canada_doc ) {

	// Constexpr Checked
	b.batch( twitter_doc.size( ) );
	b.run( fmt::format( "twitter {}", title ).c_str( ), [&]( ) {
		do_test<ParsePolicy, daw::twitter::twitter_object_t>( twitter_doc );
	} );

	b.batch( citm_doc.size( ) );
	b.run( fmt::format( "citm {}", title ).c_str( ), [&]( ) {
		do_test<ParsePolicy, daw::citm::citm_object_t>( citm_doc );
	} );

	b.batch( canada_doc.size( ) );
	b.run( fmt::format( "canada {}", title ).c_str( ), [&]( ) {
		do_test<ParsePolicy, daw::geojson::Polygon>(
		  PPair{ canada_doc, "features[0].geometry" } );
	} );

	b.batch( twitter_doc.size( ) + citm_doc.size( ) + canada_doc.size( ) );
	b.run( fmt::format( "nativejson {0}", title ).c_str( ), [&]( ) {
		do_test<ParsePolicy, daw::twitter::twitter_object_t>( twitter_doc );
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

	auto b1 = ankerl::nanobench::Bench( )
	            .title( "nativejson parts" )
	            .unit( "byte" )
	            .warmup( 100 )
	            .minEpochIterations( 100 );

	std::string const twitter_doc = *daw::read_file( argv[1] );
	std::string const citm_doc = *daw::read_file( argv[2] );
	std::string const canada_doc = *daw::read_file( argv[3] );

	bench<constexpr_checked_pol>( b1, "constexpr checked", twitter_doc, citm_doc,
	                              canada_doc );
	bench<constexpr_unchecked_pol>( b1, "constexpr unchecked", twitter_doc,
	                                citm_doc, canada_doc );
	bench<runtime_checked_pol>( b1, "runtime checked", twitter_doc, citm_doc,
	                            canada_doc );
	bench<runtime_unchecked_pol>( b1, "runtime unchecked", twitter_doc, citm_doc,
	                              canada_doc );

	bench<simd_checked_pol>( b1, "simd checked", twitter_doc, citm_doc,
	                         canada_doc );
	bench<simd_unchecked_pol>( b1, "simd unchecked", twitter_doc, citm_doc,
	                           canada_doc );

	if( argc > 4 ) {
		std::string_view const fname = argv[4];
		std::ofstream ofile = std::ofstream( argv[4] );
		if( not ofile ) {
			std::cerr << "Could not open output file '" << fname << "'\n";
			exit( 1 );
		}
		if( fname.size( ) > 5 ) {
			auto const ext = fname.substr( fname.size( ) - 4 );
			if( ext == "json" ) {
				b1.render( ankerl::nanobench::templates::json( ), ofile );
				return 0;
			} else if( ext == "html" ) {
				b1.render( ankerl::nanobench::templates::htmlBoxplot( ), ofile );
				return 0;
			}
		}
		if( fname.size( ) > 4 and fname.substr( fname.size( ) - 3 ) == "csv" ) {
			static constexpr char const csvout_template[] =
			  R"DELIM("title", "name", "unit", "batch", "minimum time", "median time", "iterations", "error %", "instructions", "branches", "branch misses", "total"
{{#result}}"{{title}}", "{{name}}", "{{unit}}", {{batch}}, {{minimum(elapsed)}}, {{median(elapsed)}}, {{median(iterations)}}, {{medianAbsolutePercentError(elapsed)}}, {{median(instructions)}}, {{median(branchinstructions)}}, {{median(branchmisses)}}, {{sumProduct(iterations, elapsed)}}
{{/result}})DELIM";
			b1.render( csvout_template, ofile );
			return 0;
		}
	}
}
