// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/daw_json_link
//
// Members that appear before the member being looked for are skipped with
// skip_bracketed_item and parsed later.  Comments inside those skipped
// classes and arrays must be skipped too, as must comments that follow
// whitespace when unchecked.

#include "defines.h"

#include <daw/json/daw_json_link.h>

#include <daw/daw_ensure.h>

#include <cmath>
#include <optional>
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
	// Line comments in skipped items
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
	// A '*' right before the closing "*/"
	test_assert( ( parses<PolicyCommentTypes::cpp, PolicyFlags...>(
	               CPP_DOC( "/* a **/" ) ) ),
	             "cpp: block comment ending in **/ in skipped item" );
	test_assert( ( parses<PolicyCommentTypes::cpp, PolicyFlags...>(
	               CPP_DOC( "/***/" ) ) ),
	             "cpp: block comment /***/ in skipped item" );
	test_assert( ( parses<PolicyCommentTypes::cpp, PolicyFlags...>(
	               "{ /* a **/ \"inner\": { \"s\": \"q\", \"v\": [ 1, /***/ 2 ] }, "
	               "\"arr\": [ 3, 4 ], \"first\": 7 }" ) ),
	             "cpp: block comment ending in **/" );
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

#if defined( DAW_USE_EXCEPTIONS )
// Parse from a buffer without a trailing zero so that reads past the end are
// caught by sanitizers
template<typename T, auto... PolicyFlags>
bool parses_unterminated( std::string_view json_data, T const &expected ) {
	auto const buffer = std::vector<char>( json_data.begin( ), json_data.end( ) );
	try {
		return from_json<T>( std::string_view( buffer.data( ), buffer.size( ) ),
		                     options::parse_flags<PolicyFlags...> ) == expected;
	} catch( json_exception const & ) { return false; }
}

template<typename T, auto... PolicyFlags>
bool rejects_unterminated( std::string_view json_data ) {
	auto const buffer = std::vector<char>( json_data.begin( ), json_data.end( ) );
	try {
		(void)from_json<T>( std::string_view( buffer.data( ), buffer.size( ) ),
		                    options::parse_flags<PolicyFlags...> );
	} catch( json_exception const & ) { return true; }
	return false;
}

void test_end_of_data( ) {
	using options::PolicyCommentTypes;
	using ints = std::vector<int>;
	// A comment on the last line, without a newline
	test_assert( ( parses_unterminated<ints, PolicyCommentTypes::hash>(
	               "[1]#x", ints{ 1 } ) ),
	             "hash: comment at end of data" );
	test_assert( ( parses_unterminated<ints, PolicyCommentTypes::hash>(
	               "[1]#", ints{ 1 } ) ),
	             "hash: empty comment at end of data" );
	test_assert( ( parses_unterminated<ints, PolicyCommentTypes::cpp>(
	               "[1]//x", ints{ 1 } ) ),
	             "cpp: line comment at end of data" );
	test_assert( ( parses_unterminated<ints, PolicyCommentTypes::cpp>(
	               "[1]/*x*/", ints{ 1 } ) ),
	             "cpp: block comment at end of data" );
	test_assert( ( parses_unterminated<ints, PolicyCommentTypes::cpp>(
	               "[1,/***/2]", ints{ 1, 2 } ) ),
	             "cpp: block comment /***/ without a trailing zero" );
	// Truncated documents
	test_assert( ( rejects_unterminated<ints, PolicyCommentTypes::hash>(
	               "[1,#x" ) ),
	             "hash: truncated after comment" );
	test_assert( ( rejects_unterminated<ints, PolicyCommentTypes::cpp>(
	               "[1,/" ) ),
	             "cpp: truncated after '/'" );
	test_assert( ( rejects_unterminated<ints, PolicyCommentTypes::cpp>(
	               "[1,/*x" ) ),
	             "cpp: truncated inside block comment" );
	test_assert( ( rejects_unterminated<ints, PolicyCommentTypes::cpp>(
	               "[1,/x2]" ) ),
	             "cpp: '/' that does not start a comment" );
}

// A comment directly after a null literal
template<auto... PolicyFlags>
void test_null_literal( ) {
	using options::PolicyCommentTypes;
	using opt_ints = std::vector<std::optional<int>>;
	auto const expected = opt_ints{ std::nullopt, 1 };
	test_assert( ( parses_unterminated<opt_ints, PolicyCommentTypes::cpp,
	                                   PolicyFlags...>( "[null/*x*/,1]",
	                                                    expected ) ),
	             "cpp: block comment after null" );
	test_assert( ( parses_unterminated<opt_ints, PolicyCommentTypes::cpp,
	                                   PolicyFlags...>( "[null//x\n,1]",
	                                                    expected ) ),
	             "cpp: line comment after null" );
	test_assert( ( parses_unterminated<opt_ints, PolicyCommentTypes::hash,
	                                   PolicyFlags...>( "[null#x\n,1]",
	                                                    expected ) ),
	             "hash: comment after null" );
}

// A comment directly after a number
template<auto... PolicyFlags>
void test_number( ) {
	using options::PolicyCommentTypes;
	using reals = std::vector<double>;
	using uints = std::vector<unsigned>;
	test_assert( ( parses_unterminated<reals, PolicyCommentTypes::cpp,
	                                   PolicyFlags...>( "[1.5/*x*/,2]",
	                                                    reals{ 1.5, 2.0 } ) ),
	             "cpp: block comment after real" );
	test_assert( ( parses_unterminated<reals, PolicyCommentTypes::cpp,
	                                   PolicyFlags...>( "[1,2.5//x\n]",
	                                                    reals{ 1.0, 2.5 } ) ),
	             "cpp: line comment after last real" );
	test_assert( ( parses_unterminated<reals, PolicyCommentTypes::hash,
	                                   PolicyFlags...>( "[1.5#x\n,2]",
	                                                    reals{ 1.5, 2.0 } ) ),
	             "hash: comment after real" );
	test_assert( ( parses_unterminated<uints, PolicyCommentTypes::cpp,
	                                   PolicyFlags...>( "[1/*x*/,2]",
	                                                    uints{ 1, 2 } ) ),
	             "cpp: block comment after unsigned" );
	test_assert( ( parses_unterminated<uints, PolicyCommentTypes::hash,
	                                   PolicyFlags...>( "[1#x\n,2]",
	                                                    uints{ 1, 2 } ) ),
	             "hash: comment after unsigned" );
}

// A comment directly after a quoted NaN or Infinity
void test_nan_inf( ) {
	using options::PolicyCommentTypes;
	using nan_inf_t = json_number_no_name<
	  double, options::number_opt( options::LiteralAsStringOpt::Maybe,
	                               options::JsonNumberErrors::AllowNanInf )>;
	auto const parse = []<auto... PolicyFlags>( std::string_view json_data ) {
		auto const buffer =
		  std::vector<char>( json_data.begin( ), json_data.end( ) );
		return from_json_array<nan_inf_t>(
		  std::string_view( buffer.data( ), buffer.size( ) ),
		  options::parse_flags<PolicyFlags...> );
	};
	auto const nan = parse.template operator( )<PolicyCommentTypes::cpp>(
	  R"(["NaN"/*x*/,1])" );
	test_assert( nan.size( ) == 2 and std::isnan( nan[0] ) and nan[1] == 1.0,
	             "cpp: block comment after \"NaN\"" );
	auto const inf = parse.template operator( )<PolicyCommentTypes::hash>(
	  "[\"-Infinity\"#x\n,1]" );
	test_assert( inf.size( ) == 2 and std::isinf( inf[0] ) and inf[0] < 0.0 and
	               inf[1] == 1.0,
	             "hash: comment after \"-Infinity\"" );
}
#endif

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
#if defined( DAW_USE_EXCEPTIONS )
	test_end_of_data( );
	test_null_literal( );
	test_null_literal<options::CheckedParseMode::no>( );
	test_number( );
	test_number<options::CheckedParseMode::no>( );
	test_nan_inf( );
	test_assert( ( rejects_unterminated<std::vector<double>,
	                                    options::PolicyCommentTypes::cpp>(
	               "[1.5x,2]" ) ),
	             "cpp: invalid characters after real" );
	test_assert( ( rejects_unterminated<std::vector<std::optional<int>>,
	                                    options::PolicyCommentTypes::cpp>(
	               "[nullx,1]" ) ),
	             "cpp: invalid literal after null" );
#endif
}
#if defined( DAW_USE_EXCEPTIONS )
catch( daw::json::json_exception const &jex ) {
	std::cerr << "Exception thrown by parser: " << jex.reason( ) << '\n';
	exit( 1 );
}
#endif
