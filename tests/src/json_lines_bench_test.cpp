// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/daw_json_link
//

/// @brief Counts the size of a body member in a JSONL file where each line is a
/// json object that has a body method.  Used
/// https://files.pushshift.io/reddit/comments/ for testing

#include "daw_json_benchmark.h"
#include "defines.h"

#include <daw/daw_algorithm.h>
#include <daw/daw_memory_mapped_file.h>
#include <daw/json/daw_json_lines_iterator.h>
#include <daw/json/daw_json_link.h>

#include <cstdlib>
#include <future>
#include <iostream>
#include <numeric>
#include <thread>

#if not defined( DAW_NUM_RUNS )
#if not defined( DEBUG ) or defined( NDEBUG )
static inline constexpr std::size_t DAW_NUM_RUNS = 4;
#else
static inline constexpr std::size_t DAW_NUM_RUNS = 2;
#endif
#endif
static_assert( DAW_NUM_RUNS > 0 );

struct jsonl_entry {
	std::string_view body;
};

namespace daw::json {
	template<>
	struct json_data_contract<jsonl_entry> {
		static constexpr char const body[] = "body";
		using type = json_member_list<json_link<body, std::string_view>>;
	};
} // namespace daw::json

int main( int argc, char **argv ) {
	if( argc < 2 ) {
		std::cerr << "Must supply a jsonl filename to open\n";
		exit( EXIT_FAILURE );
	}
	auto const jsonl_doc = daw::filesystem::memory_mapped_file_t<char>( argv[1] );
	for( std::size_t i = 0; i < jsonl_doc.size( ); ++i ) {
		daw::do_not_optimize( jsonl_doc[i] );
	}

	auto real_count = daw::json::benchmark::benchmark(
	  DAW_NUM_RUNS, jsonl_doc.size( ), "json_lines untyped checked",
	  []( daw::string_view jd ) {
		  auto tp_range =
		    daw::json::json_lines_range<daw::json::json_raw_no_name<>>( jd );
		  auto count = daw::algorithm::accumulate(
		    tp_range.begin( ), tp_range.end( ), std::size_t{ 0 },
		    []( std::size_t c, auto jv ) {
			    return c += jv["body"].get_string_view( ).size( );
		    } );
		  return count;
	  },
	  jsonl_doc );
	ensure( real_count.has_value( ) );

	auto untyped_uncheck_count = daw::json::benchmark::benchmark(
	  DAW_NUM_RUNS, jsonl_doc.size( ), "json_lines untyped unchecked",
	  []( daw::string_view jd ) constexpr {
		  auto tp_range =
		    daw::json::json_lines_range<daw::json::json_raw_no_name<>,
		                                daw::json::options::CheckedParseMode::no>(
		      jd );
		  auto count = daw::algorithm::accumulate(
		    tp_range.begin( ), tp_range.end( ), std::size_t{ 0 },
		    []( std::size_t c, auto jv ) {
			    return c += jv["body"].get_string_view( ).size( );
		    } );
		  return count;
	  },
	  jsonl_doc );
	ensure( untyped_uncheck_count.has_value( ) );
	ensure( untyped_uncheck_count.get( ) == real_count.get( ) );

	auto typed_check_count = daw::json::benchmark::benchmark(
	  DAW_NUM_RUNS, jsonl_doc.size( ), "json_lines typed checked",
	  []( daw::string_view jd ) constexpr {
		  auto tp_range = daw::json::json_lines_range<jsonl_entry>( jd );
		  auto count = daw::algorithm::accumulate(
		    tp_range.begin( ), tp_range.end( ), std::size_t{ 0 },
		    []( std::size_t c, jsonl_entry entry ) {
			    return c += entry.body.size( );
		    } );
		  return count;
	  },
	  jsonl_doc );
	ensure( typed_check_count.has_value( ) );
	ensure( typed_check_count.get( ) == real_count.get( ) );

	auto typed_uncheck_count = daw::json::benchmark::benchmark(
	  DAW_NUM_RUNS, jsonl_doc.size( ), "json_lines typed unchecked",
	  []( daw::string_view jd ) constexpr {
		  auto tp_range = daw::json::json_lines_range<
		    jsonl_entry, daw::json::options::CheckedParseMode::no>( jd );
		  auto count = daw::algorithm::accumulate(
		    tp_range.begin( ), tp_range.end( ), std::size_t{ 0 },
		    []( std::size_t c, jsonl_entry entry ) {
			    return c += entry.body.size( );
		    } );
		  return count;
	  },
	  jsonl_doc );
	ensure( typed_uncheck_count.has_value( ) );
	ensure( typed_uncheck_count.get( ) == real_count.get( ) );

	auto const chkpartitions = daw::json::partition_jsonl_document<jsonl_entry>(
	  std::thread::hardware_concurrency( ), jsonl_doc );

	auto typed_checked_threaded_count = daw::json::benchmark::benchmark(
	  DAW_NUM_RUNS * 10, jsonl_doc.size( ), "json_lines typed threaded checked",
	  []( auto const &parts ) {
		  auto results = std::vector<std::future<std::size_t>>( );
		  for( auto const &part : parts ) {
			  results.push_back( std::async( std::launch::async, [&] {
				  std::size_t count = daw::algorithm::accumulate(
				    part.begin( ), part.end( ), std::size_t{ 0 },
				    []( std::size_t c, jsonl_entry entry ) {
					    return c += entry.body.size( );
				    } );
				  return count;
			  } ) );
		  }
		  std::size_t count = daw::algorithm::accumulate(
		    std::begin( results ), std::end( results ), std::size_t{ 0 },
		    []( auto lhs, auto &rhs ) {
			    return lhs + rhs.get( );
		    } );
		  return count;
	  },
	  chkpartitions );
	ensure( typed_checked_threaded_count.has_value( ) );
	ensure( typed_checked_threaded_count.get( ) == real_count.get( ) );

	auto const unchkpartitions = daw::json::partition_jsonl_document<
	  jsonl_entry, daw::json::options::CheckedParseMode::no>(
	  std::thread::hardware_concurrency( ), jsonl_doc );

	auto typed_unchecked_threaded_count = daw::json::benchmark::benchmark(
	  DAW_NUM_RUNS * 10, jsonl_doc.size( ), "json_lines typed threaded unchecked",
	  []( auto const &parts ) {
		  auto results = std::vector<std::future<std::size_t>>( );
		  for( auto const &part : parts ) {
			  results.push_back( std::async( std::launch::async, [&] {
				  std::size_t count = daw::algorithm::accumulate(
				    part.begin( ), part.end( ), std::size_t{ 0 },
				    []( std::size_t c, jsonl_entry entry ) {
					    return c += entry.body.size( );
				    } );
				  return count;
			  } ) );
		  }
		  std::size_t count = daw::algorithm::accumulate(
		    std::begin( results ), std::end( results ), std::size_t{ 0 },
		    []( auto lhs, auto &rhs ) {
			    return lhs + rhs.get( );
		    } );
		  return count;
	  },
	  unchkpartitions );
	ensure( typed_unchecked_threaded_count.has_value( ) );
	ensure( typed_unchecked_threaded_count.get( ) == real_count.get( ) );
}
