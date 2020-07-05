// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/daw_json_link
//

#pragma once

#include <daw/daw_do_n.h>
#include <daw/daw_hide.h>

namespace daw::json::parse_policy_details {
	template<JSONNAMETYPE Set>
	[[nodiscard]] DAW_ATTRIBUTE_FLATTEN inline constexpr bool in( char c ) {
		constexpr daw::string_view set = Set;
		constexpr char const *ptr = std::data( set );
		bool result = false;
		daw::algorithm::do_n_arg<std::size( set )>(
		  [&]( std::size_t n ) { result |= ptr[n] == c; } );
		return result;
	}

	[[nodiscard]] DAW_ATTRIBUTE_FLATTEN inline constexpr bool
	at_end_of_item( char c ) {
		return static_cast<bool>(
		  static_cast<unsigned>( c == ',' ) | static_cast<unsigned>( c == '}' ) |
		  static_cast<unsigned>( c == ']' ) | static_cast<unsigned>( c == ':' ) |
		  static_cast<unsigned>( c <= 0x20 ) );
	}

	[[nodiscard]] DAW_ATTRIBUTE_FLATTEN inline constexpr bool
	is_number( char c ) {
		return static_cast<unsigned>( c ) - static_cast<unsigned>( '0' ) < 10U;
	}

	[[nodiscard]] DAW_ATTRIBUTE_FLATTEN inline constexpr bool
	is_number_start( char c ) {
		switch( c ) {
		case '0':
		case '1':
		case '2':
		case '3':
		case '4':
		case '5':
		case '6':
		case '7':
		case '8':
		case '9':
		case '-':
			return true;
		}
		return false;
	}

	[[nodiscard]] DAW_ATTRIBUTE_FLATTEN inline constexpr bool
	is_space_unchecked( char c ) {
		return c <= 0x20;
	}
} // namespace daw::json::parse_policy_details
