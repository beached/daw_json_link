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
#include <iostream>

#if not defined( DAW_NUM_RUNS )
#if not defined( DEBUG ) or defined( NDEBUG )
static inline constexpr std::size_t DAW_NUM_RUNS = 5;
#else
static inline constexpr std::size_t DAW_NUM_RUNS = 1;
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
	auto json_doc = daw::filesystem::memory_mapped_file_t<char>( argv[1] );
	daw::bench_n_test_mbs<DAW_NUM_RUNS>(
	  "json_lines typed checked", json_doc.size( ),
	  []( daw::string_view jd ) {
		  std::size_t count = 0;
		  auto tp_range =
		    daw::json::json_lines_range<daw::json::json_raw_no_name<>>( jd );
		  for( auto jv : tp_range ) {
			  count += jv["body"].get_string_view( ).size( );
		  }
		  daw::do_not_optimize( count );
	  },
	  json_doc );

	daw::bench_n_test_mbs<DAW_NUM_RUNS>(
	  "json_lines typed unchecked", json_doc.size( ),
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
	  },
	  json_doc );

	daw::bench_n_test_mbs<DAW_NUM_RUNS>(
	  "json_lines checked", json_doc.size( ),
	  []( daw::string_view jd ) {
		  std::size_t count = 0;
		  auto tp_range = daw::json::json_lines_range<jsonl_entry>( jd );
		  for( jsonl_entry entry : tp_range ) {
			  count += entry.body.size( );
		  }
		  daw::do_not_optimize( count );
	  },
	  json_doc );

	daw::bench_n_test_mbs<DAW_NUM_RUNS>(
	  "json_lines unchecked", json_doc.size( ),
	  []( daw::string_view jd ) {
		  std::size_t count = 0;
		  auto tp_range = daw::json::json_lines_range<
		    jsonl_entry, daw::json::options::CheckedParseMode::no>( jd );
		  for( jsonl_entry entry : tp_range ) {
			  count += entry.body.size( );
		  }
		  daw::do_not_optimize( count );
	  },
	  json_doc );
}
