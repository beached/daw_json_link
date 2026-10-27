// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/daw_json_link
//
// Decoding the escapes in a member name, so it can be compared with the
// mapped names, which are decoded text

#include "defines.h"

#include <daw/json/daw_json_link.h>

#include <string>
#include <string_view>
#include <tuple>
#include <variant>
#include <vector>

using namespace std::literals;

namespace {
	using checked_policy_t = daw::json::BasicParsePolicy<>;
	using unchecked_policy_t =
	  daw::json::BasicParsePolicy<daw::json::parse_options(
	    daw::json::options::CheckedParseMode::no )>;

	/// Decode raw, the name between its quotes, into a buffer of MaxSize chars
	template<std::size_t MaxSize, typename Policy = checked_policy_t>
	constexpr bool decodes_to( std::string_view raw, std::string_view expected ) {
		auto const parse_state =
		  Policy( std::data( raw ), std::data( raw ) + std::size( raw ) );
		auto buf = daw::json::json_details::name::name_buffer<MaxSize>{ };
		if( not daw::json::json_details::name::decode_name(
		      daw::string_view( std::data( raw ), std::size( raw ) ),
		      buf,
		      parse_state ) ) {
			return false;
		}
		auto const result = buf.view( );
		return std::string_view( std::data( result ), std::size( result ) ) ==
		       expected;
	}

	/// The decoded name does not fit in MaxSize chars
	template<std::size_t MaxSize, typename Policy = checked_policy_t>
	constexpr bool does_not_fit( std::string_view raw ) {
		auto const parse_state =
		  Policy( std::data( raw ), std::data( raw ) + std::size( raw ) );
		auto buf = daw::json::json_details::name::name_buffer<MaxSize>{ };
		return not daw::json::json_details::name::decode_name(
		  daw::string_view( std::data( raw ), std::size( raw ) ),
		  buf,
		  parse_state );
	}

	std::string decode_to_string( std::string_view raw ) {
		auto const parse_state =
		  checked_policy_t( std::data( raw ), std::data( raw ) + std::size( raw ) );
		auto result = std::string( );
		auto app = daw::json::json_details::name::name_string_appender<std::string>{
		  &result };
		daw_ensure( daw::json::json_details::name::decode_name(
		  daw::string_view( std::data( raw ), std::size( raw ) ),
		  app,
		  parse_state ) );
		return result;
	}

#if defined( DAW_USE_EXCEPTIONS )
	bool is_rejected( std::string_view raw ) {
		try {
			(void)decode_to_string( raw );
		} catch( daw::json::json_exception const & ) { return true; }
		return false;
	}
#endif

	// Names without escapes are copied
	static_assert( decodes_to<3>( "abc", "abc" ) );
	static_assert( decodes_to<1>( "", "" ) );
	static_assert( decodes_to<0>( "", "" ) );

	// The two char escapes
	static_assert( decodes_to<3>( R"(a\"b)", "a\"b" ) );
	static_assert( decodes_to<3>( R"(a\\b)", "a\\b" ) );
	static_assert( decodes_to<3>( R"(a\/b)", "a/b" ) );
	static_assert( decodes_to<5>( R"(\b\f\n\r\t)", "\b\f\n\r\t" ) );

	// \u escapes are UTF-8 encoded
	static_assert( decodes_to<2>( R"(\u0061b)", "ab" ) );
	static_assert( decodes_to<2>( R"(\u0061\u0062)", "ab" ) );
	static_assert( decodes_to<2>( R"(\u00e9)", "\xc3\xa9" ) );
	static_assert( decodes_to<2>( R"(\u00E9)", "\xc3\xa9" ) );
	static_assert( decodes_to<3>( R"(\u20ac)", "\xe2\x82\xac" ) );
	static_assert( decodes_to<4>( R"(\ud83d\ude00)", "\xf0\x9f\x98\x80" ) );
	static_assert( decodes_to<6>( R"(x\ud83d\ude00y)", "x\xf0\x9f\x98\x80y" ) );

	// The buffer bounds the decoded name, not the raw one
	static_assert( decodes_to<1>( R"(\u0061)", "a" ) );
	static_assert( does_not_fit<2>( "abc" ) );
	static_assert( does_not_fit<0>( "a" ) );
	static_assert( does_not_fit<2>( R"(ab\n)" ) );
	static_assert( does_not_fit<1>( R"(\u00e9)" ) );
	static_assert( does_not_fit<3>( R"(\ud83d\ude00)" ) );

	static_assert( decodes_to<3, unchecked_policy_t>( R"(a\"b)", "a\"b" ) );

	/// Mapped names are decoded text, the input can spell them with escapes
	struct EscapedNames {
		int quote;
		int backslash;
		int accent;
		int plain;
	};

	struct TaggedSwitcher {
		constexpr std::size_t operator( )( int tag ) const {
			return static_cast<std::size_t>( tag );
		}
	};

	/// The tag a tagged variant depends on has a name that must be escaped
	struct EscapedTag {
		std::variant<int, std::string> value;
	};
} // namespace

template<>
struct daw::json::json_data_contract<EscapedNames> {
	static constexpr char const quote[] = "a\"b";
	static constexpr char const backslash[] = "c\\d";
	static constexpr char const accent[] = "\xc3\xa9";
	static constexpr char const plain[] = "plain";
	using type =
	  json_member_list<json_number<quote, int>, json_number<backslash, int>,
	                   json_number<accent, int>, json_number<plain, int>>;

	static constexpr auto to_json_data( EscapedNames const &v ) {
		return std::forward_as_tuple( v.quote, v.backslash, v.accent, v.plain );
	}
};

template<>
struct daw::json::json_data_contract<EscapedTag> {
	static constexpr char const value[] = "value";
	static constexpr char const tag[] = "t\"ag";
	using type = json_member_list<
	  json_tagged_variant<value, std::variant<int, std::string>,
	                      json_number<tag, int>, TaggedSwitcher>>;
};

namespace {
	constexpr auto escaped_names_flags = daw::json::options::parse_flags<
	  daw::json::options::AllowEscapedNames::yes>;
	constexpr auto exact_escaped_names_flags = daw::json::options::parse_flags<
	  daw::json::options::AllowEscapedNames::yes,
	  daw::json::options::UseExactMappingsByDefault::yes>;

	constexpr bool is_1234( EscapedNames const &v ) {
		return v.quote == 1 and v.backslash == 2 and v.accent == 3 and
		       v.plain == 4;
	}

	DAW_CONSTEXPR bool parse_escaped_in_order( ) {
		return is_1234( daw::json::from_json<EscapedNames>(
		  R"({"a\"b":1,"c\\d":2,"\u00e9":3,"\u0070lain":4})",
		  escaped_names_flags ) );
	}

	void parse_escaped_names( ) {
		DAW_CONSTEXPR bool in_order = parse_escaped_in_order( );
		daw_ensure( in_order );

		// The names as written still match
		daw_ensure( is_1234( daw::json::from_json<EscapedNames>(
		  "{\"a\\\"b\":1,\"c\\\\d\":2,\"\xc3\xa9\":3,\"plain\":4}",
		  escaped_names_flags ) ) );

		// Out of order with unknown members.  The second unknown name decodes
		// to more chars than the longest mapped name
		daw_ensure( is_1234( daw::json::from_json<EscapedNames>(
		  R"({"plain":4,"x\ny":0,"\u00e9":3,"\u0061\u0061\u0061\u0061\u0061\u0061":0,"c\\d":2,"a\"b":1})",
		  escaped_names_flags ) ) );

		// Exact mappings with the decoded names
		daw_ensure( is_1234( daw::json::from_json<EscapedNames>(
		  R"({"c\\d":2,"a\"b":1,"plain":4,"\u00E9":3})",
		  exact_escaped_names_flags ) ) );

		// What the writer outputs is read back
		auto const json = daw::json::to_json(
		  EscapedNames{ 1, 2, 3, 4 },
		  daw::json::options::output_flags<
		    daw::json::options::AllowEscapedNames::yes> );
		daw_ensure( is_1234(
		  daw::json::from_json<EscapedNames>( json, escaped_names_flags ) ) );

		// The tag is found before and after the value it is for
		auto const tag_first = daw::json::from_json<EscapedTag>(
		  R"({"t\"ag":1,"value":"str"})", escaped_names_flags );
		daw_ensure( std::get<1>( tag_first.value ) == "str" );
		auto const tag_last = daw::json::from_json<EscapedTag>(
		  R"({"value":42,"t\u0022ag":0})", escaped_names_flags );
		daw_ensure( std::get<0>( tag_last.value ) == 42 );
	}

#if defined( DAW_USE_EXCEPTIONS )
	template<typename T>
	daw::json::ErrorReason parse_error( std::string_view json,
	                                     bool exact = false ) {
		try {
			if( exact ) {
				(void)daw::json::from_json<T>( json, exact_escaped_names_flags );
			} else {
				(void)daw::json::from_json<T>( json, escaped_names_flags );
			}
		} catch( daw::json::json_exception const &ex ) {
			return ex.reason_type( );
		}
		return daw::json::ErrorReason::Unknown;
	}

	void reject_escaped_names( ) {
		// Unknown members are errors with exact mappings, whether or not they
		// fit in the buffer
		daw_ensure(
		  parse_error<EscapedNames>(
		    R"({"a\"b":1,"c\\d":2,"\u00e9":3,"plain":4,"x\ny":0})", true ) ==
		  daw::json::ErrorReason::UnknownMember );
		daw_ensure(
		  parse_error<EscapedNames>(
		    R"({"\u0061\u0061\u0061\u0061\u0061\u0061":0,"a\"b":1,"c\\d":2,"\u00e9":3,"plain":4})",
		    true ) == daw::json::ErrorReason::UnknownMember );

		// Invalid escapes and unpaired surrogates in names
		daw_ensure( parse_error<EscapedNames>(
		              R"({"a\"b":1,"c\\d":2,"\ud800":3,"plain":4})" ) !=
		            daw::json::ErrorReason::Unknown );
		daw_ensure( parse_error<EscapedNames>(
		              R"({"a\"b":1,"c\\d":2,"\u00e9":3,"pl\qain":4})" ) !=
		            daw::json::ErrorReason::Unknown );
	}
#endif

	void json_pair_decoded_name( ) {
		auto const jv = daw::json::json_value(
		  R"({"a\"b":1,"\u00e9":2,"plain":3,"\ud83d\ude00":4})" );
		auto names = std::vector<std::string>( );
		for( auto jp : jv ) {
			auto const decoded = jp.decoded_name( );
			daw_ensure( decoded.has_value( ) );
			names.push_back( *decoded );
		}
		daw_ensure( names.size( ) == 4 );
		daw_ensure( names[0] == "a\"b" );
		daw_ensure( names[1] == "\xc3\xa9" );
		daw_ensure( names[2] == "plain" );
		daw_ensure( names[3] == "\xf0\x9f\x98\x80" );

		// name stays as written
		auto const first = *jv.begin( );
		daw_ensure( first.name == std::optional<std::string_view>( R"(a\"b)" ) );

		// A json_value made from JSON text decodes names when parsed, a
		// json_value made from a parse state has its options
		static_assert( daw::json::json_value::ParseState::allow_escaped_names );
		static_assert( not decltype( daw::json::basic_json_value(
		  daw::json::BasicParsePolicy<>( nullptr, nullptr ) ) )::ParseState::
		                 allow_escaped_names );
		daw_ensure( is_1234( daw::json::from_json<EscapedNames>(
		  daw::json::json_value(
		    R"({"a\"b":1,"c\\d":2,"\u00e9":3,"\u0070lain":4})" ) ) ) );

		// Array elements have no name
		auto const arr = daw::json::json_value( "[1]" );
		daw_ensure( not ( *arr.begin( ) ).decoded_name( ).has_value( ) );
	}

#if defined( DAW_USE_EXCEPTIONS )
	template<typename F>
	bool throws_json_exception( F &&f ) {
		try {
			(void)f( );
		} catch( daw::json::json_exception const & ) { return true; }
		return false;
	}

	void reject_invalid_escapes( ) {
		daw_ensure( throws_json_exception( [] {
			auto const jv = daw::json::json_value( R"({"a\qb":1})" );
			return ( *jv.begin( ) ).decoded_name( );
		} ) );

		// String values reject escapes that are not in JSON too
		daw_ensure( throws_json_exception(
		  [] { return daw::json::from_json<std::string>( R"("a\qb")" ); } ) );
		daw_ensure( throws_json_exception(
		  [] { return daw::json::from_json<std::string>( R"("a\'b")" ); } ) );
		daw_ensure( daw::json::from_json<std::string>(
		              R"("\"\\\/\b\f\n\r\t\u0061")" ) == "\"\\/\b\f\n\r\ta" );
	}
#endif
} // namespace

int main( )
#if defined( DAW_USE_EXCEPTIONS )
  try
#endif
{
	daw_ensure( decode_to_string( "abc" ) == "abc" );
	daw_ensure( decode_to_string( R"(a\"b\\c)" ) == "a\"b\\c" );
	daw_ensure( decode_to_string( R"(\ud83d\ude00\u0061)" ) ==
	            "\xf0\x9f\x98\x80"
	            "a" );

	// Unchecked input stops at a truncated escape instead of reading past the
	// name.  Constant evaluation always checks, so these are at runtime
	daw_ensure( does_not_fit<8, unchecked_policy_t>( R"(a\u00)" ) );
	daw_ensure( does_not_fit<8, unchecked_policy_t>( R"(\ud83d)" ) );
	daw_ensure( does_not_fit<8, unchecked_policy_t>( R"(a\)" ) );

	parse_escaped_names( );
	json_pair_decoded_name( );

#if defined( DAW_USE_EXCEPTIONS )
	// Invalid escapes and unpaired surrogates are errors
	daw_ensure( is_rejected( R"(a\qb)" ) );
	daw_ensure( is_rejected( R"(a\)" ) );
	daw_ensure( is_rejected( R"(\u00)" ) );
	daw_ensure( is_rejected( R"(\u00g1)" ) );
	daw_ensure( is_rejected( R"(\ude00)" ) );
	daw_ensure( is_rejected( R"(\ud83d)" ) );
	daw_ensure( is_rejected( R"(\ud83dabcdef)" ) );
	daw_ensure( is_rejected( R"(\ud83d\u0061)" ) );
	reject_escaped_names( );
	reject_invalid_escapes( );
#endif
}
#if defined( DAW_USE_EXCEPTIONS )
catch( daw::json::json_exception const &jex ) {
	std::cerr << "Exception thrown by parser: " << jex.reason( ) << '\n';
	exit( 1 );
}
#endif
