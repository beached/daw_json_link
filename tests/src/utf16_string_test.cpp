// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/daw_json_link
//

#include <daw/json/daw_json_link.h>
#include <daw/utf8/unchecked.h>

#include <cstddef>
#include <string>

/***
 * This example shows how to parse a utf8 JSON string with the result being a
 * std::u16string. This applies equally to std::wstring on windows, I believe.
 */

struct ToConverter {
	std::u16string operator( )( ) const {
		return { };
	}

	std::u16string operator( )( std::string_view sv ) const {
		char const *first = std::data( sv );
		char const *last = daw::data_end( sv );
		auto result = std::u16string( );
		result.resize( sv.size( ) );
		auto *olast =
		  daw::utf8::unchecked::utf8to16( first, last, std::data( result ) );
		result.resize(
		  static_cast<std::size_t>( std::distance( std::data( result ), olast ) ) );
		return result;
	}
};

struct FromConverter {
	std::string operator( )( std::u16string const &str ) const {
		char16_t const *first = std::data( str );
		char16_t const *last = daw::data_end( str );
		auto result = std::string( );
		result.resize( str.size( ) * 3 );
		auto *olast =
		  daw::utf8::unchecked::utf16to8( first, last, std::data( result ) );
		result.resize(
		  static_cast<std::size_t>( std::distance( std::data( result ), olast ) ) );
		return result;
	}
};

using json_u16string =
  daw::json::json_custom_no_name<std::u16string, ToConverter, FromConverter>;

int main( int, char ** ) {
	std::string in_str = R"json(["testing🎉", "🙈monkey"])json";

	puts( "Before" );
	puts( in_str.c_str( ) );

	std::vector<std::u16string> ary =
	  daw::json::from_json_array<json_u16string>( in_str );

	assert( ary.size( ) == 2 );

	auto out_str = daw::json::to_json_array<json_u16string>( ary );

	puts( "After" );
	puts( out_str.c_str( ) );
}
