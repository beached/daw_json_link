// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/daw_json_link
//

#include "defines.h"

#include "daw/json/daw_json_iterator.h"
#include "daw/json/daw_json_link.h"
#include "daw_json_benchmark.h"

#include <daw/daw_read_file.h>

#include <iostream>
#include <tuple>

struct tweet {
	std::uint64_t id;
	std::string_view text;
};

namespace daw::json {
	template<>
	struct json_data_contract<tweet> {
		static constexpr char const id[] = "id";
		static constexpr char const text[] = "text";

		using type = json_member_list<json_link<id, std::uint64_t>,
		                              json_link<text, std::string_view>>;

		static constexpr auto to_json_data( tweet const &t ) {
			return std::forward_as_tuple( t.id, t.text );
		}
	};
} // namespace daw::json

#if not defined( DAW_NUM_RUNS )
#if not defined( DEBUG ) or defined( NDEBUG )
static inline constexpr std::size_t DAW_NUM_RUNS = 2500;
#else
static inline constexpr std::size_t DAW_NUM_RUNS = 2;
#endif
#endif
static_assert( DAW_NUM_RUNS > 0 );

template<daw::json::options::ExecModeTypes ExecMode>
void test( std::string_view json_sv1, std::uint64_t id ) {
	std::cout << "Using " << to_string( ExecMode )
	          << " exec model\n*********************************************\n";

	auto const sz = json_sv1.size( );
	{
		using range_t = daw::json::json_array_range<tweet, ExecMode>;
		auto result = tweet{ };
		auto res = daw::json::benchmark::benchmark(
		  DAW_NUM_RUNS, sz, "find_tweet bench(checked)",
		  [&]( auto rng ) {
			  for( tweet t : rng ) {
				  if( t.id == id ) {
					  result = t;
					  return;
				  }
			  }
			  result = tweet{ };
		  },
		  range_t( json_sv1, "statuses" ) );
		(void)res.get( );
		test_assert( result.id == id, "Invalid id found" );
	}
	{
		using range_t = daw::json::json_array_range<
		  tweet, daw::json::options::CheckedParseMode::no, ExecMode>;
		auto result = tweet{ };
		auto res = daw::json::benchmark::benchmark(
		  DAW_NUM_RUNS, sz, "find_tweet bench(unchecked)",
		  [&]( auto rng ) {
			  for( tweet t : rng ) {
				  if( t.id == id ) {
					  result = t;
					  return;
				  }
			  }
			  result = tweet{ };
		  },
		  range_t( json_sv1, "statuses" ) );
		(void)res.get( );
		test_assert( result.id == id, "Invalid id found" );
	}
	{
		auto result = tweet{ };
		using namespace daw::json;
		auto res = daw::json::benchmark::benchmark(
		  DAW_NUM_RUNS, sz, "find_tweet bench(checked, json_value)",
		  [&]( json_value jv ) {
			  for( auto jp : jv ) {
				  if( as<std::uint64_t>( jp.value["id"] ) == id ) {
					  result = as<tweet>( jp.value );
					  return;
				  }
			  }
			  result = tweet{ };
		  },
		  json_value( json_sv1 )["statuses"] );
		(void)res.get( );
		test_assert( result.id == id, "Invalid id found" );
	}
	{
		auto result = tweet{ };
		using namespace daw::json;
		auto res = daw::json::benchmark::benchmark(
		  DAW_NUM_RUNS, sz, "find_tweet bench(unchecked, json_value)",
		  [&]( auto jv ) {
			  for( auto jp : jv ) {
				  if( as<std::uint64_t>( jp.value["id"] ) == id ) {
					  result = as<tweet>( jp.value );
					  return;
				  }
			  }
			  result = tweet{ };
		  },
		  basic_json_value<parse_options( options::CheckedParseMode::no )>(
		    json_sv1 )["statuses"] );
		(void)res.get( );
		test_assert( result.id == id, "Invalid id found" );
	}
}

int main( int argc, char **argv )
#ifdef DAW_USE_EXCEPTIONS
  try
#endif
{

	using namespace daw::json;
	if( argc < 2 ) {
		std::cerr << "Must supply a filenames to open(twitter_timeline.json)\n";
		exit( 1 );
	}

	auto const json_data1 = *daw::read_file( argv[1] );
	auto const json_sv1 =
	  std::string_view( json_data1.data( ), json_data1.size( ) );
	assert( json_sv1.size( ) > 2 and "Minimum json data size is 2 '{}'" );

	constexpr std::uint64_t id = 505874901689851904ULL;
	test<options::ExecModeTypes::compile_time>( json_sv1, id );
	test<options::ExecModeTypes::runtime>( json_sv1, id );
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
