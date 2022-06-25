// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/daw_json_link
//

#include "defines.h"

#include "daw_json_benchmark.h"

#include <daw/json/daw_json_iterator.h>
#include <daw/json/daw_json_lines_iterator.h>
#include <daw/json/daw_json_link.h>

#include <daw/daw_read_file.h>

#include <cstdint>
#include <iostream>
#include <map>
#include <string>
#include <tuple>

struct brand {
	double cumulative_rating;
	std::uint64_t reviews_count;
	constexpr bool operator==( const brand &other ) const {
		return cumulative_rating == other.cumulative_rating and
		       reviews_count == other.reviews_count;
	}
	constexpr bool operator!=( const brand &other ) const {
		return not( *this == other );
	}
};

#if not defined( DAW_NUM_RUNS )
#if not defined( DEBUG ) or defined( NDEBUG )
static inline constexpr std::size_t DAW_NUM_RUNS = 2500;
#else
static inline constexpr std::size_t DAW_NUM_RUNS = 2;
#endif
#endif
static_assert( DAW_NUM_RUNS > 0 );

template<daw::json::options::ExecModeTypes ExecMode>
void test( daw::string_view json ) {
	std::cout << "Using " << to_string( ExecMode )
	          << " exec model\n*********************************************\n";

	using namespace daw::json;

	using line_item_t = json_tuple_no_name<
	  std::tuple<std::string_view, double, std::uint64_t>,
	  json_tuple_member_list<json_tuple_member<1, std::string_view>,
	                         json_tuple_member<5, double>,
	                         json_tuple_member<7, std::uint64_t>>>;

	auto it = json_lines_iterator( json );
	++it;
	auto json_sv =
	  daw::string_view( it.get_raw_json_document( ) ).trim_prefix_copy( );

	daw_json_ensure( json_sv.starts_with( '[' ),
	                 ErrorReason::ExpectedArrayOrClassStart );
	auto const sz = json_sv.size( );

	{
		auto result = std::map<std::string, brand, std::less<>>( );
		auto json_lines_doc = json_lines_range<line_item_t>( json_sv );
		auto res = daw::json::benchmark::benchmark(
		  DAW_NUM_RUNS, sz, "amazon cellphone(checked)",
		  [&result]( auto jl ) {
			  for( auto [key, rating, reviews] : jl ) {
				  auto pos = result.find( key );
				  if( pos == result.end( ) ) {
					  result.emplace(
					    key, brand{ rating * static_cast<double>( reviews ), reviews } );
				  } else {
					  pos->second.cumulative_rating +=
					    rating * static_cast<double>( reviews );
					  pos->second.reviews_count += reviews;
				  }
			  }
		  },
		  json_lines_doc );
		(void)res.get( );
		test_assert( not result.empty( ),
		             "Exception while parsing: res.get_exception_message()" );
	}
	{
		auto result = std::map<std::string, brand, std::less<>>( );
		auto json_lines_doc =
		  json_lines_range<line_item_t, options::CheckedParseMode::no>( json_sv );
		auto res = daw::json::benchmark::benchmark(
		  DAW_NUM_RUNS, sz, "amazon cellphone(unchecked)",
		  [&]( auto jl ) {
			  for( auto [key, rating, reviews] : jl ) {
				  auto pos = result.find( key );
				  if( pos == result.end( ) ) {
					  result.emplace(
					    key, brand{ rating * static_cast<double>( reviews ), reviews } );
				  } else {
					  pos->second.cumulative_rating +=
					    rating * static_cast<double>( reviews );
					  pos->second.reviews_count += reviews;
				  }
			  }
		  },
		  json_lines_doc );
		(void)res.get( );
		test_assert( not result.empty( ),
		             "Exception while parsing: res.get_exception_message()" );
	}
}

int main( int argc, char **argv ) {
	auto json_doc = std::optional<std::string>{ };
#ifdef DAW_USE_EXCEPTIONS
	try
#endif
	{
		using namespace daw::json;
		if( argc < 2 ) {
			std::cerr << "Must supply a filenames to open(twitter_timeline.json)\n";
			exit( 1 );
		}

		json_doc = daw::read_file( argv[1] );
		daw::string_view const json_sv = json_doc.value( );
		assert( json_sv.size( ) > 2 and "Minimum json data size is 2 '{}'" );

		test<options::ExecModeTypes::compile_time>( json_sv );
		test<options::ExecModeTypes::runtime>( json_sv );
	}
#ifdef DAW_USE_EXCEPTIONS
	catch( daw::json::json_exception const &jex ) {
		std::cerr << "Exception thrown by parser: " << jex.reason( ) << '\n';
		std::cerr << to_formatted_string( jex, json_doc->data( ) ) << '\n';
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
}