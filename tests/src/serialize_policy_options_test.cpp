// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/daw_json_link
//
//  Covers serialization options that no other existing test sets:
//  options::IndentationType (non-default values), options::NewLineDelimiter,
//  options::OutputTrailingComma, and options::RestrictedStringOutput::None /
//  ::ErrorInvalidUTF8.
//

#include <daw/json/daw_json_link.h>
#include <daw/json/daw_json_writer.h>

#include <daw/daw_ensure.h>

#include <cstdlib>
#include <iostream>
#include <string>
#include <vector>

struct TrailingCommaHolder {
	std::vector<int> b;
};

namespace daw::json {
	template<>
	struct json_data_contract<TrailingCommaHolder> {
		static constexpr char b[] = "b";
		using type = json_member_list<json_array<b, int>>;

		static auto to_json_data( TrailingCommaHolder const &v ) {
			return std::forward_as_tuple( v.b );
		}
	};
} // namespace daw::json

struct RestrictedOutputHolder {
	std::string s;
};

namespace daw::json {
	template<>
	struct json_data_contract<RestrictedOutputHolder> {
		static constexpr char s[] = "s";
		using type = json_member_list<json_string<s>>;

		static auto to_json_data( RestrictedOutputHolder const &v ) {
			return std::forward_as_tuple( v.s );
		}
	};
} // namespace daw::json

int main( )
#if defined( DAW_USE_EXCEPTIONS )
  try
#endif
{
	using namespace daw::json;

	// IndentationType::Tab
	{
		auto out = std::string{ };
		{
			auto w = json_writer<options::SerializationFormat::Pretty,
			                     options::IndentationType::Tab>( out );
			w.open_object( );
			w.write_key_value( "a", 42 );
			w.write_key_value( "b", { 1, 2, 3 } );
		}
		daw_ensure( out == "{\n\t\"a\": 42,\n\t\"b\": [\n\t\t1,\n\t\t2,\n\t\t3\n\t]\n}" );
	}

	// IndentationType::Space4 (a non-default value; Space2 is already covered
	// elsewhere)
	{
		auto out = std::string{ };
		{
			auto w = json_writer<options::SerializationFormat::Pretty,
			                     options::IndentationType::Space4>( out );
			w.open_object( );
			w.write_key_value( "a", 42 );
		}
		daw_ensure( out == "{\n    \"a\": 42\n}" );
	}

	// NewLineDelimiter::rn
	{
		auto out = std::string{ };
		{
			auto w = json_writer<options::SerializationFormat::Pretty,
			                     options::NewLineDelimiter::rn>( out );
			w.open_object( );
			w.write_key_value( "a", 42 );
		}
		daw_ensure( out == "{\r\n  \"a\": 42\r\n}" );
	}

	// OutputTrailingComma::Yes
	//
	// Note: this option is only honored by the automatic class/array
	// serializer (json_data_contract-based to_json), not by the manual
	// json_writer API -- daw_json_writer.h's open_object/close_object and
	// open_array/close_array never consult it, so a manual-writer-based test
	// here would silently test nothing.
	{
		auto const v = TrailingCommaHolder{ { 1, 2, 3 } };
		auto const out = to_json(
		  v,
		  options::output_flags<options::SerializationFormat::Pretty,
		                        options::OutputTrailingComma::Yes> );
		daw_ensure( out == "{\n  \"b\": [\n    1,\n    2,\n    3,\n  ],\n}" );
	}

	// RestrictedStringOutput::None tolerates invalid UTF-8 without throwing.
	// (Note: despite the "default: None" comment on the enum's declaration in
	// daw_json_serialize_options.h, the actual compiled-in default is
	// ErrorInvalidUTF8 -- see default_json_option_value<RestrictedStringOutput>
	// in daw_json_serialize_options_impl.h, which matches what this cookbook
	// page documents.
	//
	// This must go through the automatic class serializer (to_json on a
	// json_data_contract type with output_flags<...>), not the manual
	// json_writer API: json_writer_t::write_value ends up calling
	// to_json<JsonClass>(value, m_writer.get()) (daw_json_writer.h:121),
	// and m_writer.get() strips the PolicyFlags the writer was constructed
	// with, so the *value*-formatting flags (RestrictedStringOutput among
	// them) silently fall back to their compiled-in defaults there -- only
	// the structural flags used directly via m_writer (indentation, newline,
	// used by add_indent/del_indent/next_member) actually apply. A test built
	// on the manual writer would therefore test nothing.)
	{
		std::string bad = "ok ";
		bad.push_back( static_cast<char>( 0xFF ) );
		auto const v = RestrictedOutputHolder{ bad };
		auto const out =
		  to_json( v, options::output_flags<options::RestrictedStringOutput::None> );
		daw_ensure( not out.empty( ) );
	}

	// RestrictedStringOutput::OnlyAllow7bitStrings permits the full 7-bit
	// range, including DEL (0x7F).
	{
		std::string value = "ok ";
		value.push_back( static_cast<char>( 0x7F ) );
		auto const out = to_json(
		  RestrictedOutputHolder{ value },
		  options::output_flags<
		    options::RestrictedStringOutput::OnlyAllow7bitStrings> );
		auto expected = std::string{ R"({"s":"ok )" };
		expected.push_back( static_cast<char>( 0x7F ) );
		expected += R"("})";
		daw_ensure( out == expected );
	}
	{
		using disallow_high_string = json_string_no_name<
		  std::string,
		  options::string_opt( options::EightBitModes::DisallowHigh )>;
		std::string_view const value = "\x7F";
		std::string_view const expected = "\"\x7F\"";
		daw_ensure( to_json<disallow_high_string>( value ) == expected );
	}

#if defined( DAW_USE_EXCEPTIONS )
	// RestrictedStringOutput::ErrorInvalidUTF8 throws on invalid UTF-8.
	{
		std::string bad = "ok ";
		bad.push_back( static_cast<char>( 0xFF ) );
		auto const v = RestrictedOutputHolder{ bad };
		bool threw = false;
		try {
			(void)to_json(
			  v,
			  options::output_flags<options::RestrictedStringOutput::ErrorInvalidUTF8> );
		} catch( daw::json::json_exception const & ) {
			threw = true;
		}
		daw_ensure( threw );
	}
#endif
}
#if defined( DAW_USE_EXCEPTIONS )
catch( daw::json::json_exception const &jex ) {
	std::cerr << "Exception thrown by parser: " << jex.reason( ) << '\n';
	exit( 1 );
}
#endif
