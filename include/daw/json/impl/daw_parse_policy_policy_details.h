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
	[[nodiscard]] DAW_ATTRIBUTE_FLATTEN inline constexpr bool in( char lhs,
	                                                              char rhs ) {
		return lhs == rhs;
	}

	template<JSONNAMETYPE Set>
	[[nodiscard]] DAW_ATTRIBUTE_FLATTEN inline constexpr bool in( char c ) {
		constexpr daw::string_view set = Set;
		unsigned result = 0;
		daw::algorithm::do_n_arg<set.size( )>( [&]( std::size_t n ) {
			result |= static_cast<unsigned>( set[n] == c );
		} );
		return static_cast<bool>( result );
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
	is_signed_number_part( char c ) {
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
		case '+':
		case '-':
			return true;
		}
		return false;
	}

	[[nodiscard]] DAW_ATTRIBUTE_FLATTEN inline constexpr bool
	is_real_number_part( char c ) {
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
		case 'e':
		case 'E':
		case '+':
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
