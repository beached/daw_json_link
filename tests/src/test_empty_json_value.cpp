// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/daw_json_link
//

#include <daw/json/daw_json_link.h>

#include <daw/daw_ensure.h>

#include <cstddef>

namespace {
	template<typename JsonValue>
	[[nodiscard]] constexpr bool is_empty_json_value( JsonValue const &value ) {
		return value.get_raw_state( ).empty( ) and
		       value.type( ) == daw::json::JsonBaseParseTypes::None and
		       not static_cast<bool>( value ) and value.begin( ) == value.end( );
	}

	template<typename JsonValue>
	[[nodiscard]] constexpr bool test_empty_constructors( ) {
		char const *const null_pointer = nullptr;
		char const *const empty_text = "";

		JsonValue const default_constructed{ };
		JsonValue const from_parse_state{
		  typename JsonValue::ParseState( null_pointer, null_pointer ) };
		JsonValue const from_empty_view{ daw::string_view{} };
		JsonValue const from_null_and_size{ null_pointer, std::size_t{ 0 } };
		JsonValue const from_null_and_nonzero_size{ null_pointer,
		                                            std::size_t{ 1 } };
		JsonValue const from_null_range{ null_pointer, null_pointer };
		JsonValue const from_empty_text_and_size{ empty_text, std::size_t{ 0 } };
		JsonValue const from_empty_text_range{ empty_text, empty_text };

		return is_empty_json_value( default_constructed ) and
		       is_empty_json_value( from_parse_state ) and
		       is_empty_json_value( from_empty_view ) and
		       is_empty_json_value( from_null_and_size ) and
		       is_empty_json_value( from_null_and_nonzero_size ) and
		       is_empty_json_value( from_null_range ) and
		       is_empty_json_value( from_empty_text_and_size ) and
		       is_empty_json_value( from_empty_text_range );
	}

	using checked_json_value = daw::json::json_value;
	using unchecked_json_value =
	  daw::json::basic_json_value<daw::json::parse_options(
	    daw::json::options::CheckedParseMode::no )>;

	static_assert( test_empty_constructors<checked_json_value>( ) );
	static_assert( test_empty_constructors<unchecked_json_value>( ) );

	[[nodiscard]] constexpr bool test_whitespace_only_value( ) {
		checked_json_value const value{ daw::string_view( " \t\r\n" ) };
		return is_empty_json_value( value );
	}

	static_assert( test_whitespace_only_value( ) );
} // namespace

int main( ) {
	daw_ensure( test_empty_constructors<checked_json_value>( ) );
	daw_ensure( test_empty_constructors<unchecked_json_value>( ) );
	daw_ensure( test_whitespace_only_value( ) );
}
