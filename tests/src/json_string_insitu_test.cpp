// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/daw_json_link

#include "daw/json/daw_json_link.h"
#include "daw/json/daw_json_link_types.h"
#include "daw/json/daw_json_schema.h"

#include <daw/daw_ensure.h>

#include <array>
#include <iterator>
#include <memory>
#include <string>
#include <string_view>
#include <type_traits>
#include <utility>

namespace {
	using namespace daw::json;
	using mutable_policy =
	  BasicParsePolicy<parse_options( options::AllowStringMutation::yes )>;
	using readonly_policy = BasicParsePolicy<>;

	static_assert( std::is_same_v<mutable_policy::iterator, char *> );
	static_assert( std::is_same_v<readonly_policy::iterator, char const *> );
	static_assert( std::is_constructible_v<mutable_policy, char *, char *> );
	static_assert(
	  not std::is_constructible_v<mutable_policy, char const *, char const *> );
	static_assert(
	  not std::is_constructible_v<mutable_policy, char *, char const *> );
	static_assert(
	  not std::is_constructible_v<mutable_policy, char const *, char *> );
	static_assert( std::is_constructible_v<readonly_policy, char *, char *> );
	static_assert(
	  std::is_constructible_v<readonly_policy, char const *, char const *> );
	static_assert(
	  std::is_constructible_v<mutable_policy, char *, char *, char *, char *> );
	static_assert( not std::is_constructible_v<mutable_policy, char *, char *,
	                                           char const *, char const *> );
	static_assert( not std::is_constructible_v<mutable_policy, readonly_policy> );

	using allocator = std::allocator<char>;
	using allocated_policy = mutable_policy::with_allocator_type<allocator>;
	static_assert(
	  std::is_constructible_v<allocated_policy, char *, char *, allocator &> );
	static_assert( not std::is_constructible_v<allocated_policy, char const *,
	                                           char const *, allocator &> );
	static_assert( std::is_constructible_v<allocated_policy, char *, char *,
	                                       char *, char *, allocator const &> );
	static_assert(
	  not std::is_constructible_v<allocated_policy, char *, char *, char const *,
	                              char const *, allocator const &> );
	static_assert( std::is_constructible_v<mutable_policy, char *, char *, char *,
	                                       char *, std::size_t> );
	static_assert(
	  not std::is_constructible_v<mutable_policy, char const *, char const *,
	                              char const *, char const *, std::size_t> );

	template<typename String>
	constexpr bool accepts_mutable_input =
	  std::is_constructible_v<mutable_policy,
	                          decltype( std::data( std::declval<String &>( ) ) ),
	                          decltype( std::data( std::declval<String &>( ) ) )>;

	static_assert( accepts_mutable_input<std::string> );
	static_assert( accepts_mutable_input<char[8]> );
	static_assert( accepts_mutable_input<std::array<char, 8>> );
	static_assert( not accepts_mutable_input<std::string const> );
	static_assert( not accepts_mutable_input<char const[8]> );
	static_assert( not accepts_mutable_input<std::array<char, 8> const> );
	// A non-const view still exposes read-only characters.
	static_assert( not accepts_mutable_input<std::string_view> );
	static_assert( not accepts_mutable_input<std::string_view const> );

	constexpr bool policy_mutation_test( ) {
		char buffer[] = "123";
		auto state = mutable_policy( buffer, buffer + 3 );
		auto copy = state.copy( );
		*copy.begin( ) = '4';
		return buffer[0] == '4' and state.data( ) == buffer and
		       state.data_end( ) == buffer + 3;
	}
	static_assert( policy_mutation_test( ) );
} // namespace

namespace {
	using insitu_string = daw::json::json_string_insitu_no_name<>;
	constexpr bool decoded_view_test( ) {
		char buffer[] = R"("hello\nworld")";
		auto value = daw::json::from_json_insitu<insitu_string>( buffer );
		return value == "hello\nworld" and value.data( ) == buffer + 1;
	}
	static_assert( decoded_view_test( ) );
} // namespace

struct InsituRecord {
	std::string_view text;
	std::string_view other;
	int number;
	std::optional<std::string_view> optional;
};

struct MixedInsituRecord {
	InsituRecord nested;
	std::string owned;
};

namespace daw::json {
	template<>
	struct json_data_contract<InsituRecord> {
		static constexpr char text[] = "text";
		static constexpr char other[] = "other";
		static constexpr char number[] = "number";
		static constexpr char optional[] = "optional";
		using type =
		  json_member_list<json_string_insitu<text>, json_string_insitu<other>,
		                   json_number<number, int>,
		                   json_string_insitu_null<optional>>;
		static auto to_json_data( InsituRecord const &v ) {
			return std::tie( v.text, v.other, v.number, v.optional );
		}
	};
	template<>
	struct json_data_contract<MixedInsituRecord> {
		static constexpr char nested[] = "nested";
		static constexpr char owned[] = "owned";
		using type =
		  json_member_list<json_class<nested, InsituRecord>, json_string<owned>>;
	};

} // namespace daw::json

namespace {
	template<auto... Flags>
	void
	string_mapping_tests( daw::json::options::parse_flags_t<Flags...> flags ) {
		using namespace daw::json;
		struct test_case {
			std::string_view encoded;
			std::string_view decoded;
		};
		constexpr test_case cases[] = {
		  { R"("")", "" },
		  { R"("plain text")", "plain text" },
		  { R"("\"\\\/\b\f\n\r\t")", "\"\\/\b\f\n\r\t" },
		  { R"("\u0000after")", std::string_view( "\0after", 6 ) },
		  { R"("\u007f\u0080\u07ff\u0800\uffff")",
		    "\x7f\xc2\x80\xdf\xbf\xe0\xa0\x80\xef\xbf\xbf" },
		  { R"("\uD800\uDC00\uDBFF\uDFFF")", "\xf0\x90\x80\x80\xf4\x8f\xbf\xbf" },
		  { R"("\uD83D\uDE00")", "\xf0\x9f\x98\x80" },
		  { R"("\u0041\u00e9\u20AC")", "A\xc3\xa9\xe2\x82\xac" },
		  { "\"\xc3\xa9\"", "\xc3\xa9" },
		  { R"("prefix\nsuffix with enough bytes to overlap the source")",
		    "prefix\nsuffix with enough bytes to overlap the source" },
		  { R"("\\n")", "\\n" },
		};
		for( auto const &tc : cases ) {
			std::string input( tc.encoded );
			auto const view = from_json_insitu<insitu_string>( input, flags );
			daw_ensure( view == tc.decoded );
			daw_ensure( view.data( ) == input.data( ) + 1 );
			daw_ensure( daw::data_end( view ) <= daw::data_end( input ) );
			daw_ensure( input.front( ) == '"' and *daw::data_end( view ) == '"' );
			if( tc.encoded.find( '\\' ) == std::string_view::npos ) {
				daw_ensure( input == tc.encoded );
			}
			auto const serialized = to_json<insitu_string>( view );
			daw_ensure( from_json<std::string>( serialized ) == tc.decoded );
		}
		// Saved locations must preserve the original encoded bounds, even when
		// fields are decoded in a different order from their input order.
		for(
		  std::string input :
		  { R"({"text":"a\"b","other":"c\nd","number":42,"optional":"e\tf"})",
		    R"({"optional":"e\tf","number":42,"other":"c\nd","text":"a\"b"})" } ) {
			auto const text_pos = input.find( "a\\\"b" );
			auto const other_pos = input.find( "c\\nd" );
			auto const value = from_json_insitu<InsituRecord>( input, flags );
			daw_ensure( value.text == "a\"b" and value.other == "c\nd" and
			            value.number == 42 );
			daw_ensure( value.optional and *value.optional == "e\tf" );
			daw_ensure( value.text.data( ) == input.data( ) + text_pos );
			daw_ensure( value.other.data( ) == input.data( ) + other_pos );
			auto serialized = to_json( value );
			auto const restored = from_json_insitu<InsituRecord>( serialized, flags );
			daw_ensure( restored.text == value.text and
			            restored.other == value.other );
		}
		for( std::string input :
		     { R"({"text":"a","other":"b","number":42,"optional":null})",
		       R"({"text":"a","other":"b","number":42})" } ) {
			daw_ensure( not from_json_insitu<InsituRecord>( input, flags ).optional );
		}
		std::string array = R"(["a\nb","","c\"d"])";
		using array_mapping = json_array_no_name<insitu_string>;
		auto const values = from_json_insitu<array_mapping>( array, flags );
		daw_ensure( values.size( ) == 3 and values[0] == "a\nb" and
		            values[1].empty( ) and values[2] == "c\"d" );
	}

	void additional_mapping_tests( ) {
		using namespace daw::json;
		std::string input =
		  R"({"owned":"own\u00e9d","unknown":[{"skip":"q\\r"}],"nested":{"text":"a\nb","other":"c","number":42}})";
		auto const mixed = from_json_insitu<MixedInsituRecord>( input );
		daw_ensure( mixed.owned ==
		              "own\xc3\xa9"
		              "d" and
		            mixed.nested.text == "a\nb" );
		std::string other_view_input = R"("a\tb")";
		auto const other_view =
		  from_json_insitu<json_string_insitu_no_name<daw::string_view>>(
		    other_view_input );
		daw_ensure( other_view == "a\tb" and
		            other_view.data( ) == other_view_input.data( ) + 1 );
		std::string optional_input = "null";
		daw_ensure( not from_json_insitu<json_string_insitu_null_no_name<>>(
		  optional_input ) );
		std::string ascii_input = R"("a\n\u00e9")";
		using ascii_mapping =
		  json_string_insitu_no_name<std::string_view,
		                             options::string_opt(
		                               options::EightBitModes::DisallowHigh )>;
		auto const ascii = from_json_insitu<ascii_mapping>( ascii_input );
		daw_ensure( ascii == "a\n\xc3\xa9" );
		daw_ensure( to_json<ascii_mapping>( ascii ) == R"("a\n\u00E9")" );
		auto const schema = to_json_schema<InsituRecord>( "", "InsituRecord" );
		daw_ensure( schema.find( R"("type":"string")" ) != std::string::npos );
	}

#if defined( DAW_USE_EXCEPTIONS )
	void invalid_string_tests( ) {
		using namespace daw::string_view_literals;
		DAW_CPP23_STATIC_LOCAL constexpr std::array tests = {
		  R"("\uDC00")"_sv,
		  R"("\uD800")"_sv,
		  R"("\u12")"_sv,
		  R"("\uGGGG")"_sv,
		  R"("\uD800\u0041")"_sv,
		  R"("\uD800xuDC00")"_sv,
		  R"("\q")"_sv,
		  R"("unterminated)"_sv,
		  R"("trailing\)"_sv,
		  "\"raw\nnewline\""_sv };
		for( std::size_t n = 0; n < tests.size( ); ++n ) {
			auto const input_src = tests[n];
			auto input = static_cast<std::string>( input_src );
			bool rejected = false;
			try {
				(void)daw::json::from_json_insitu<insitu_string>(
				  input, daw::json::ConformancePolicy );
			} catch( daw::json::json_exception const &ex ) {
				(void)ex;
				rejected = true;
			}
			if( not rejected ) {
				std::cerr << "Failed to reject: " << input << '\n';
			}
			daw_ensure( rejected );
		}
	}
#endif
} // namespace

int main( ) {
	additional_mapping_tests( );
	string_mapping_tests( daw::json::options::parse_flags<> );
	string_mapping_tests( daw::json::options::parse_flags<
	                      daw::json::options::ExecModeTypes::runtime> );
	string_mapping_tests(
	  daw::json::options::parse_flags<daw::json::options::ExecModeTypes::simd> );
	string_mapping_tests(
	  daw::json::options::parse_flags<daw::json::options::CheckedParseMode::no> );
#if defined( DAW_USE_EXCEPTIONS )
	invalid_string_tests( );
#endif

	daw_ensure( decoded_view_test( ) );
	std::string buffer = "42 ";
	daw_ensure( daw::json::from_json_insitu<int>( buffer ) == 42 );
	char boolean[] = "true ";
	daw_ensure( daw::json::from_json_insitu<bool>( boolean ) );
	std::array<char, 2> number = { '4', '2' };
	daw_ensure( daw::json::from_json_insitu<int>( number ) == 42 );
	daw_ensure( daw::json::from_json_insitu<int>(
	              buffer,
	              daw::json::options::parse_flags<
	                daw::json::options::ExecModeTypes::compile_time> ) == 42 );
	daw_ensure( ( daw::json::from_json_insitu<int, true>( number ) == 42 ) );
	daw_ensure( daw::json::from_json<int>( std::string_view( buffer ) ) == 42 );
	// The wrapper must preserve other flags and always enable mutation.
	daw_ensure( daw::json::from_json_insitu<int>(
	              number,
	              daw::json::options::parse_flags<
	                daw::json::options::AllowStringMutation::no> ) == 42 );
#if defined( DAW_USE_EXCEPTIONS )
	std::string trailing = "42 false";
	bool rejected_trailing = false;
	try {
		(void)daw::json::from_json_insitu<int>(
		  trailing,
		  daw::json::options::parse_flags<
		    daw::json::options::MustVerifyEndOfDataIsValid::yes> );
	} catch( daw::json::json_exception const & ) { rejected_trailing = true; }
	daw_ensure( rejected_trailing );
#endif
}
