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

#include "defines.h"

#include <daw/daw_benchmark.h>
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
static inline constexpr std::size_t DAW_NUM_RUNS = 5;
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
	auto jsonl_doc = daw::filesystem::memory_mapped_file_t<char>( argv[1] );

	std::size_t real_count = *daw::bench_n_test_mbs<DAW_NUM_RUNS>(
	  "json_lines untyped checked", jsonl_doc.size( ),
	  []( daw::string_view jd ) {
		  std::size_t count = 0;
		  auto tp_range =
		    daw::json::json_lines_range<daw::json::json_raw_no_name<>>( jd );
		  for( auto jv : tp_range ) {
			  count += jv["body"].get_string_view( ).size( );
		  }
		  daw::do_not_optimize( count );
		  return count;
	  },
	  jsonl_doc );

	auto untyped_uncheck_count = *daw::bench_n_test_mbs<DAW_NUM_RUNS>(
	  "json_lines untyped unchecked", jsonl_doc.size( ),
	  []( daw::string_view jd ) {
		  std::size_t count = 0;
		  auto tp_range =
		    daw::json::json_lines_range<daw::json::json_raw_no_name<>,
		                                daw::json::options::CheckedParseMode::no>(
		      jd );
		  for( auto jv : tp_range ) {
			  count += jv["body"].get_string_view( ).size( );
		  }
		  daw::do_not_optimize( count );
		  return count;
	  },
	  jsonl_doc );
	ensure( untyped_uncheck_count == real_count );

	auto typed_check_count = *daw::bench_n_test_mbs<DAW_NUM_RUNS>(
	  "json_lines typed checked", jsonl_doc.size( ),
	  []( daw::string_view jd ) {
		  std::size_t count = 0;
		  auto tp_range = daw::json::json_lines_range<jsonl_entry>( jd );
		  for( jsonl_entry entry : tp_range ) {
			  count += entry.body.size( );
		  }
		  daw::do_not_optimize( count );
		  return count;
	  },
	  jsonl_doc );
	ensure( typed_check_count == real_count );

	auto typed_uncheck_count = *daw::bench_n_test_mbs<DAW_NUM_RUNS>(
	  "json_lines unchecked", jsonl_doc.size( ),
	  []( daw::string_view jd ) {
		  std::size_t count = 0;
		  auto tp_range = daw::json::json_lines_range<
		    jsonl_entry, daw::json::options::CheckedParseMode::no>( jd );
		  for( jsonl_entry entry : tp_range ) {
			  count += entry.body.size( );
		  }
		  daw::do_not_optimize( count );
		  return count;
	  },
	  jsonl_doc );
	ensure( typed_uncheck_count == real_count );

	auto const chkpartitions = daw::json::partition_jsonl_document<jsonl_entry>(
	  std::thread::hardware_concurrency( ), jsonl_doc );

	auto typed_checked_threaded_count = *daw::bench_n_test_mbs<DAW_NUM_RUNS*10>(
	  "json_lines typed threaded checked", jsonl_doc.size( ),
	  []( auto const &parts ) {
		  auto results = std::vector<std::future<std::size_t>>( );
		  for( auto const &part : parts ) {
			  results.push_back( std::async( std::launch::async, [&] {
				  std::size_t count = 0;
				  auto tp_range = part;
				  for( jsonl_entry entry : tp_range ) {
					  count += entry.body.size( );
				  }
				  return count;
			  } ) );
		  }
		  std::size_t count = std::accumulate(
		    std::begin( results ), std::end( results ), std::size_t{ 0 },
		    []( auto lhs, auto &rhs ) { return lhs + rhs.get( ); } );
		  daw::do_not_optimize( count );
		  return count;
	  },
	  chkpartitions );
	ensure( typed_checked_threaded_count == real_count );

	auto const unchkpartitions = daw::json::partition_jsonl_document<
	  jsonl_entry, daw::json::options::CheckedParseMode::no>(
	  std::thread::hardware_concurrency( ), jsonl_doc );

	auto typed_unchecked_threaded_count = *daw::bench_n_test_mbs<DAW_NUM_RUNS*10>(
	  "json_lines typed threaded unchecked", jsonl_doc.size( ),
	  []( auto const &parts ) {
		  auto results = std::vector<std::future<std::size_t>>( );
		  for( auto const &part : parts ) {
			  results.push_back( std::async( std::launch::async, [&] {
				  std::size_t count = 0;
				  auto tp_range = part;
				  for( jsonl_entry entry : tp_range ) {
					  count += entry.body.size( );
				  }
				  return count;
			  } ) );
		  }
		  std::size_t count = std::accumulate(
		    std::begin( results ), std::end( results ), std::size_t{ 0 },
		    []( auto lhs, auto &rhs ) { return lhs + rhs.get( ); } );
		  daw::do_not_optimize( count );
		  return count;
	  },
	  unchkpartitions );
	ensure( typed_unchecked_threaded_count == real_count );
}
