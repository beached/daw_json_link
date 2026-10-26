// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/daw_json_link
//
// Checked skipping of classes and arrays has to reject structurally invalid
// JSON, and the scalar and SIMD skips have to agree on every input

#include "defines.h"

#include "daw/json/daw_json_link.h"

#include <cstddef>
#include <iterator>
#include <optional>
#include <random>
#include <string>
#include <string_view>
#include <vector>

#if defined( DAW_USE_EXCEPTIONS )
struct A {
	int a;
};

// The mapped member is a string or an array, their parsers do not look at
// what follows the value
struct StringMember {
	std::string a;
};

struct ArrayMember {
	std::vector<int> a;
};

namespace daw::json {
	template<>
	struct json_data_contract<A> {
		static constexpr char const a[] = "a";
		using type = json_member_list<json_number<a, int>>;
	};

	template<>
	struct json_data_contract<StringMember> {
		static constexpr char const a[] = "a";
		using type = json_member_list<json_string<a>>;
	};

	template<>
	struct json_data_contract<ArrayMember> {
		static constexpr char const a[] = "a";
		using type = json_member_list<json_array<a, int>>;
	};
} // namespace daw::json

namespace {
	using namespace daw::json;
	using json_details::SkipBracketedType;
	using default_policy_t = BasicParsePolicy<>;
	using strict_policy_t = BasicParsePolicy<
	  options::details::make_parse_flags<options::AllowLeadingZeroPlus::no>( )
	    .value>;

	struct skip_outcome {
		std::optional<ErrorReason> error{ };
		std::ptrdiff_t end = 0;
		std::size_t counter = 0;
	};

	bool operator==( skip_outcome const &lhs, skip_outcome const &rhs ) {
		return lhs.error == rhs.error and lhs.end == rhs.end and
		       lhs.counter == rhs.counter;
	}

	template<SkipBracketedType Type, typename policy_t>
	skip_outcome scalar_skip( std::string_view json ) {
		auto state = policy_t( json.data( ), json.data( ) + json.size( ) );
		try {
			auto const r = state.template skip_bracketed_item_checked<Type>( );
			return { std::nullopt, state.first - json.data( ), r.counter };
		} catch( json_exception const &e ) { return { e.reason_type( ) }; }
	}

	template<SkipBracketedType Type, typename policy_t>
	skip_outcome simd_skip( std::string_view json ) {
#if defined( DAW_HAS_SIMD )
		auto state = policy_t( json.data( ), json.data( ) + json.size( ) );
		try {
			auto const r =
			  json_details::skip_bracketed_item_simd_validated<Type>( state );
			return { std::nullopt, state.first - json.data( ), r.counter };
		} catch( json_exception const &e ) { return { e.reason_type( ) }; }
#else
		return scalar_skip<Type, policy_t>( json );
#endif
	}

	template<SkipBracketedType Type, typename policy_t>
	void check_same( std::string_view json ) {
		auto const scalar = scalar_skip<Type, policy_t>( json );
		auto const simd = simd_skip<Type, policy_t>( json );
		test_assert( scalar == simd,
		             "scalar and SIMD skips differ on \"" + std::string( json ) +
		               "\" scalar " +
		               ( scalar.error
		                   ? std::string( reason_message( *scalar.error ) )
		                   : "end " + std::to_string( scalar.end ) ) +
		               " SIMD " +
		               ( simd.error ? std::string( reason_message( *simd.error ) )
		                            : "end " + std::to_string( simd.end ) ) );
	}

	void check_both( std::string_view json ) {
		check_same<SkipBracketedType::Class, default_policy_t>( json );
		check_same<SkipBracketedType::Array, default_policy_t>( json );
		check_same<SkipBracketedType::Class, strict_policy_t>( json );
		check_same<SkipBracketedType::Array, strict_policy_t>( json );
	}

	template<auto... PolicyFlags>
	void check_parse( std::string_view json, bool should_parse ) {
		bool parsed = true;
		try {
			(void)from_json<A>( json, options::parse_flags<PolicyFlags...> );
		} catch( json_exception const & ) { parsed = false; }
		test_assert( parsed == should_parse,
		             std::string( should_parse ? "expected to parse: "
		                                       : "expected an error: " ) +
		               std::string( json ) );
	}

	template<typename T>
	void check_parse_as( std::string_view json, bool should_parse ) {
		bool parsed = true;
		try {
			(void)from_json<T>( json );
		} catch( json_exception const & ) { parsed = false; }
		test_assert( parsed == should_parse,
		             std::string( should_parse ? "expected to parse: "
		                                       : "expected an error: " ) +
		               std::string( json ) );
	}
} // namespace

int main( ) {
	// Members that are not mapped, before and after the mapped one
	check_parse( R"({"b":[{]},"a":1})", false );
	check_parse( R"({"b":{[}],"a":1})", false );
	check_parse( R"({"b":[1,2,],"a":1})", true );
	check_parse( R"({"b":{"x" 1},"a":1})", false );
	check_parse( R"({"b":{"x":},"a":1})", false );
	check_parse( R"({"b":[1 2],"a":1})", false );
	check_parse( R"({"b":[,],"a":1})", false );
	check_parse( R"({"b":[]],"a":1})", false );
	check_parse( R"({"a":1,"b":[{]}})", false );
	check_parse( R"({"a":1,"b":{"x" 1}})", false );
	check_parse( R"({"a":1,"b" 1})", false );
	check_parse( R"({"a":1,"b":]})", false );
	check_parse( R"({"a":1,"b":[}]})", false );
	check_parse( R"({"a":1 "b":1})", false );
	check_parse( R"({"a":1,})", true );
	check_parse( R"({"a":1,"b":{"c":[1,{"d":"}]"}],"e":null},"f":[]})", true );
	check_parse( R"({"b":{"c":"\"{["},"a":1})", true );

	// A comma has to follow a value that is not the last
	check_parse( R"({"b":1 "a":1})", false );
	check_parse( R"({"b":"x" "a":1})", false );
	check_parse( R"({"b":[1] "a":1})", false );
	check_parse( R"({"b":{} "a":1})", false );
	check_parse( R"({"b":true "a":1})", false );
	check_parse( R"({"a":1,"b":1 "c":2})", false );
	check_parse( R"({"b":1 ,"a":1 })", true );

	// Literals and numbers inside skipped classes and arrays
	check_parse( R"({"b":[1e+5,-0.5E-3,0,-0,true,false,null],"a":1})", true );
	check_parse( R"({"b":[tru],"a":1})", false );
	check_parse( R"({"b":{"c":nul},"a":1})", false );
	check_parse( R"({"b":[falsey],"a":1})", false );
	check_parse( R"({"b":[1.],"a":1})", false );
	check_parse( R"({"b":[.5],"a":1})", false );
	check_parse( R"({"b":[-],"a":1})", false );
	check_parse( R"({"b":[1e],"a":1})", false );
	check_parse( R"({"b":[1x],"a":1})", false );
	check_parse( R"({"a":1,"b":[truex]})", false );
	check_parse( R"({"a":1,"b":{"c":1.5.5}})", false );
	check_parse( R"({"b":[+],"a":1})", false );
	check_parse( R"({"b":[0x1],"a":1})", false );

	// A leading plus and leading zeros are allowed in skipped numbers unless
	// AllowLeadingZeroPlus is turned off
	for( std::string_view json : { R"({"b":[01],"a":1})",
	                               R"({"b":[+1],"a":1})",
	                               R"({"b":{"c":-00.5e+1},"a":1})",
	                               R"({"b":01,"a":1})",
	                               R"({"a":1,"b":[007]})" } ) {
		check_parse( json, true );
		check_parse<options::AllowLeadingZeroPlus::no>( json, false );
	}

	// After the mapped members, the rest of the class is a comma separated list
	// of members
	check_parse( R"({"a":1, xyz "b":2})", false );
	check_parse( R"({"a":1, xyz})", false );
	check_parse( R"({"a":1,,"b":2})", false );
	check_parse( R"({"a":1,"b":2,})", true );
	check_parse_as<StringMember>( R"({"a":"x" "b":1})", false );
	check_parse_as<StringMember>( R"({"a":"x" xyz})", false );
	check_parse_as<StringMember>( R"({"a":"x",,"b":1})", false );
	check_parse_as<StringMember>( R"({"a":"x"})", true );
	check_parse_as<StringMember>( R"({"a":"x" , "b":[1]})", true );
	check_parse_as<StringMember>( R"({"a":"x"])", false );
	check_parse_as<ArrayMember>( R"({"a":[1] "b":1})", false );
	check_parse_as<ArrayMember>( R"({"a":[1] xyz})", false );
	check_parse_as<ArrayMember>( R"({"a":[1],"b":{"c":2}})", true );

	// Skipped classes and arrays are checked the same way when comments are
	// allowed, and comments can be where whitespace can
	using options::PolicyCommentTypes;
	check_parse<PolicyCommentTypes::cpp>( R"({"b":[1 2],"a":1})", false );
	check_parse<PolicyCommentTypes::cpp>( R"({"b":[{]},"a":1})", false );
	check_parse<PolicyCommentTypes::cpp>( R"({"b":{"x" 1},"a":1})", false );
	check_parse<PolicyCommentTypes::cpp>( R"({"b":[tru],"a":1})", false );
	check_parse<PolicyCommentTypes::cpp>( R"({"b":[1 /x],"a":1})", false );
	check_parse<PolicyCommentTypes::cpp>( R"({"b":[1,/*c],"a":1})", false );
	check_parse<PolicyCommentTypes::cpp>( R"({"b":[1,/*c*/2],"a":1})", true );
	check_parse<PolicyCommentTypes::cpp>( "{\"b\":[1/*c*/,2]//x\n,\"a\":1}",
	                                      true );
	check_parse<PolicyCommentTypes::cpp>( R"({"b":{"x"/*c*/:/*d*/1},"a":1})",
	                                      true );
	check_parse<PolicyCommentTypes::cpp>( R"({"b":["/*s*/"],"a":1})", true );
	check_parse<PolicyCommentTypes::hash>( R"({"b":[1 2],"a":1})", false );
	check_parse<PolicyCommentTypes::hash>( R"({"b":[{]},"a":1})", false );
	check_parse<PolicyCommentTypes::hash>( "{\"b\":[1 #c\n2],\"a\":1}", false );
	check_parse<PolicyCommentTypes::hash>( "{\"b\":[1, #c\n2],\"a\":1}", true );
	check_parse<PolicyCommentTypes::hash>( "{\"b\":[1#c\n,2],\"a\":1}", true );
	check_parse<PolicyCommentTypes::hash>( R"({"b":["#s"],"a":1})", true );

	// Starting at the bracket and part way through, as the class and array
	// cleanups do
	for( std::string_view json : { "{}",
	                               "[]",
	                               R"({"a":1})",
	                               "[1,2]",
	                               "{,}",
	                               "[,]",
	                               R"({"a":})",
	                               R"({"a" 1})",
	                               "[1 2]",
	                               "[{]}",
	                               R"(,"a":1})",
	                               R"("a":1})",
	                               R"(})",
	                               ",1]",
	                               "1]",
	                               "]",
	                               R"(,"a")",
	                               "[[[[]]]]",
	                               "[[[[]]]",
	                               R"({"a":[1,{"b":2}]})",
	                               R"({"a":"\\"})",
	                               R"({"a":"\"})",
	                               R"([\,1])",
	                               R"([\ 1])",
	                               R"({"a":\})",
	                               "[1,]",
	                               R"({"a":1,})",
	                               "[tru]",
	                               "[true]",
	                               "[nul",
	                               "[01]",
	                               "[-0]",
	                               "[+1]",
	                               "[+]",
	                               "[-01.5]",
	                               "[00]",
	                               "[1.5e+3]",
	                               "[1.]",
	                               R"([1\"])",
	                               "[nullx]",
	                               R"([1"a"])",
	                               "[1e",
	                               "[-",
	                               "[true" } ) {
		check_both( json );
	}

	// Random JSON like pieces, every outcome has to match
	constexpr std::string_view pieces[] = {
	  "{",
	  "}",
	  "[",
	  "]",
	  ",",
	  ":",
	  "\"",
	  "\\",
	  " ",
	  "\t",
	  "1",
	  "0",
	  "-",
	  ".",
	  "e",
	  "+",
	  "a",
	  "true",
	  "tru",
	  "false",
	  "null",
	  // The control characters that match a structural by their low four bits
	  "\x0c",
	  "\x1a" };
	auto engine = std::mt19937( 7 );
	auto pick =
	  std::uniform_int_distribution<std::size_t>( 0, std::size( pieces ) - 1U );
	auto length = std::uniform_int_distribution<std::size_t>( 0, 60 );
	for( int n = 0; n < 200000; ++n ) {
		auto json = std::string( );
		for( auto count = length( engine ); count > 0; --count ) {
			json += pieces[pick( engine )];
		}
		check_both( json );
		check_both( "{" + json );
		check_both( "[" + json );
	}
}
#else
int main( ) {}
#endif
