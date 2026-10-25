// Copyright (c) Darrell Wright
// Distributed under the Boost Software License, Version 1.0.

// Compare the existing parser and explicit SIMD parser using the shared
// harness.
// #include "apache_builds_json.h"
// #include "citm_test_json.h"
// #include "geojson_json.h"
// #include "gsoc.h"
#include "daw_json_benchmark.h"
#include "twitter_test_json.h"

#include "daw/json/daw_from_json_simd.h"
#include "daw/json/daw_json_link.h"

#include <daw/daw_read_file.h>

#include <cstdlib>
#include <iostream>
#include <string>

template<class T>
void bench( char const *name, char const *path, int batches, int runs ) {
	auto data = daw::read_file( path ).value( );
	auto scalar = [&] {
		auto v = daw::json::from_json<T>( data );
		daw::do_not_optimize( v );
	};
	auto scan = [&] {
		auto v = daw::json::from_json_simd<T>( data );
		daw::do_not_optimize( v );
	};
	auto a = daw::json::from_json<T>( data );
	if( daw::json::to_json( a ) !=
	    daw::json::to_json( daw::json::from_json_simd<T>( data ) ) )
		std::abort( );
	for( int n = 0; n < batches; ++n ) {
		std::cout << name << " batch " << n + 1 << '/' << batches << '\n';
		auto measure = [&]( char const *parser, auto const &parse ) {
			auto title = std::string( name ) + " " + parser + " (checked)";
			auto result = daw::json::benchmark::benchmark(
			  static_cast<std::size_t>( runs ), data.size( ), title, parse );
			if( not result.has_value( ) )
				std::abort( );
		};
		for( int j = 0; j < 2; ++j ) {
			switch( ( n + j ) % 2 ) {
			case 0:
				measure( "from_json", scalar );
				break;
			case 1:
				measure( "from_json_simd", scan );
				break;
			}
		}
	}
}

int main( int argc, char **argv ) {
	if( argc < 6 ) {
		std::cerr << "Usage: " << argv[0]
		          << " twitter.json citm_catalog.json gsoc-2018.json canada.json "
		             "apache_builds.json [batches=9] [runs=20]\n";
		return 1;
	}
	int batches = argc > 6 ? std::atoi( argv[6] ) : 9;
	int runs = argc > 7 ? std::atoi( argv[7] ) : 20;
	if( batches < 3 || runs < 1 )
		return 1;
	std::cout << __VERSION__ << " backend=";
#if defined( DAW_JSON_HAS_AVX512_SIMD )
	std::cout << "AVX-512 shim";
#elif defined( DAW_JSON_HAS_AVX2_SIMD )
	std::cout << "AVX2 shim";
#elif defined( DAW_JSON_HAS_NEON_SIMD )
	std::cout << "NEON";
#elif defined( DAW_JSON_HAS_SSE42_SIMD )
	std::cout << "SSE4.2 shim";
#else
	std::cout << "scalar";
#endif
	std::cout << " batches=" << batches << " requested runs=" << runs << '\n';
	bench<daw::twitter::twitter_object_t>( "twitter", argv[1], batches, runs );
	/*
	bench<daw::citm::citm_object_t>( "citm", argv[2], batches, runs );
	bench<daw::gsoc::gsoc_object_t>( "gsoc", argv[3], batches, runs );
	bench<daw::geojson::FeatureCollection>( "canada", argv[4], batches, runs );
	bench<apache_builds::apache_builds>(
	  "apache_builds", argv[5], batches, runs );
	  */
}
