// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/daw_json_link
//
// Members that appear before the member being looked for are skipped with
// skip_bracketed_item and parsed later.  Comments inside those skipped
// classes and arrays must be skipped too.  Known failures, not yet fixed:
//  * hash comments in skipped items fail in unchecked mode
//  * cpp block comments in skipped items fail; unchecked reads past the end
//    of the document
// Not part of the test suite until these are fixed.

#include "defines.h"

#include <daw/json/daw_json_link.h>

#include <daw/daw_ensure.h>

#include <string>
#include <string_view>
#include <vector>

struct inner_t {
	std::string s;
	std::vector<int> v;
};

struct doc_t {
	int first;
	inner_t inner;
	std::vector<int> arr;
};

namespace daw::json {
	template<>
	struct json_data_contract<inner_t> {
		using type = json_member_list<json_string<"s">, json_array<"v", int>>;
	};

	template<>
	struct json_data_contract<doc_t> {
		using type =
		  json_member_list<json_number<"first", int>, json_class<"inner", inner_t>,
		                   json_array<"arr", int>>;
	};
} // namespace daw::json

using namespace daw::json;

inline bool is_expected( doc_t const &d ) {
	return d.first == 7 and d.inner.s == "q" and
	       d.inner.v == std::vector<int>{ 1, 2 } and
	       d.arr == std::vector<int>{ 3, 4 };
}

template<auto... PolicyFlags>
bool parses( std::string const &json_data ) {
	constexpr auto flags = options::parse_flags<PolicyFlags...>;
#if defined( DAW_USE_EXCEPTIONS )
	try {
#endif
		// const buffer, char const * iterator
		auto const d = from_json<doc_t>( std::string_view( json_data ), flags );
		// mutable buffer, char * iterator
		auto copy = json_data;
		auto const d2 = from_json_insitu<doc_t>( copy, flags );
		return is_expected( d ) and is_expected( d2 );
#if defined( DAW_USE_EXCEPTIONS )
	} catch( json_exception const & ) { return false; }
#endif
}

// "inner" and "arr" come before "first", so both are skipped and stored
#define CPP_DOC( Comment ) \
	"{ \"inner\": { " Comment " \"s\": \"q\", \"v\": [ 1, " Comment \
	" 2 ] }, \"arr\": [ 3, " Comment " 4 ], \"first\": 7 }"

template<auto... PolicyFlags>
void test_cpp( ) {
	using options::PolicyCommentTypes;
	// Line comments in skipped items, these work in checked mode
	test_assert( ( parses<PolicyCommentTypes::cpp, PolicyFlags...>(
	               CPP_DOC( "// x\n" ) ) ),
	             "cpp: line comment in skipped item" );
	test_assert( ( parses<PolicyCommentTypes::cpp, PolicyFlags...>(
	               CPP_DOC( "// } ] \" \n" ) ) ),
	             "cpp: line comment with brackets and quote in skipped item" );
	// Block comments in skipped items
	test_assert( ( parses<PolicyCommentTypes::cpp, PolicyFlags...>(
	               CPP_DOC( "/* x */" ) ) ),
	             "cpp: block comment in skipped item" );
	test_assert( ( parses<PolicyCommentTypes::cpp, PolicyFlags...>(
	               CPP_DOC( "/**/" ) ) ),
	             "cpp: empty block comment in skipped item" );
	test_assert( ( parses<PolicyCommentTypes::cpp, PolicyFlags...>(
	               CPP_DOC( "/* } ] \" */" ) ) ),
	             "cpp: block comment with brackets and quote in skipped item" );
	test_assert( ( parses<PolicyCommentTypes::cpp, PolicyFlags...>(
	               CPP_DOC( "/* a * b / c */" ) ) ),
	             "cpp: block comment with * and / in skipped item" );
	test_assert( ( parses<PolicyCommentTypes::cpp, PolicyFlags...>(
	               CPP_DOC( "/* a\n b */" ) ) ),
	             "cpp: multi-line block comment in skipped item" );
}

template<auto... PolicyFlags>
void test_hash( ) {
	using options::PolicyCommentTypes;
	test_assert( ( parses<PolicyCommentTypes::hash, PolicyFlags...>(
	               CPP_DOC( "# x\n" ) ) ),
	             "hash: comment in skipped item" );
	test_assert( ( parses<PolicyCommentTypes::hash, PolicyFlags...>(
	               CPP_DOC( "# } ] \" \n" ) ) ),
	             "hash: comment with brackets and quote in skipped item" );
}

int main( )
#if defined( DAW_USE_EXCEPTIONS )
  try
#endif
{
	// Block comments outside of skipped items already work
	daw_ensure(
	  parses<options::PolicyCommentTypes::cpp>(
	    "{ /* x */ \"inner\": { \"s\": \"q\", \"v\": [ 1, 2 ] }, \"arr\": [ 3, 4 "
	    "], /* y */ \"first\": 7 }" ) );

	test_hash( );
	test_hash<options::CheckedParseMode::no>( );
	test_cpp( );
	test_cpp<options::CheckedParseMode::no>( );
}
#if defined( DAW_USE_EXCEPTIONS )
catch( daw::json::json_exception const &jex ) {
	std::cerr << "Exception thrown by parser: " << jex.reason( ) << '\n';
	exit( 1 );
}
#endif
