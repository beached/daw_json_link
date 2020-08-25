// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/daw_json_link
//

#pragma once

#include "daw_exec_modes.h"

#include <daw/daw_hide.h>

#include <cstring>

namespace daw::json::json_details {
	template<bool is_unchecked_input, char... keys>
	DAW_ATTRIBUTE_FLATTEN static inline constexpr char const *
	mem_move_to_next_of( constexpr_exec_tag const &, char const *first,
	                     char const *last ) {
		constexpr auto is_eq = []( char c ) { return ( ( c == keys ) | ... ); };
		if constexpr( is_unchecked_input ) {
			while( not is_eq( *first ) ) {
				++first;
			}
		} else {
			while( first < last and not is_eq( *first ) ) {
				++first;
			}
		}
		return first;
	}

	template<bool is_unchecked_input, char... keys>
	DAW_ATTRIBUTE_FLATTEN static inline constexpr char const *
	mem_move_to_next_not_of( constexpr_exec_tag const &, char const *first,
	                         char const *last ) {
		constexpr auto is_eq = []( char c ) { return ( ( c == keys ) | ... ); };
		if constexpr( is_unchecked_input ) {
			while( is_eq( *first ) ) {
				++first;
			}
		} else {
			while( first < last and is_eq( *first ) ) {
				++first;
			}
		}
		return first;
	}

	template<bool is_unchecked_input, char... keys>
	DAW_ATTRIBUTE_FLATTEN static inline constexpr char const *
	mem_find_substr( constexpr_exec_tag const &, char const *first,
	                 char const *last ) {

		constexpr auto is_eq = []( char const *ptr ) {
			constexpr auto check = []( char const *&p, char c ) { return *p++ == c; };
			bool const r[]{ check( ptr, keys )... };
			bool const *bp = r;
			return ( ( (void)keys, ( *bp++ ) ) and ... );
		};
		if constexpr( is_unchecked_input ) {
			while( not is_eq( first ) ) {
				++first;
			}
		} else {
			while(
			  ( last - first > static_cast<std::ptrdiff_t>( sizeof...( keys ) ) ) and
			  not is_eq( first ) ) {
				++first;
			}
		}
		return first + sizeof...( keys );
	}

	template<bool is_unchecked_input>
	DAW_ATTRIBUTE_FLATTEN static inline constexpr char const *
	mem_chr( runtime_exec_tag const &, char const *first, char const *last,
	         char c ) {
		char const *ptr = reinterpret_cast<char const *>(
		  std::memchr( first, c, static_cast<std::size_t>( last - first ) ) );
		if constexpr( not is_unchecked_input ) {
			daw_json_assert( ptr != nullptr, "Expected token missing" );
		}
		return ptr;
	}

	template<bool is_unchecked_input>
	DAW_ATTRIBUTE_FLATTEN static inline constexpr char const *
	mem_chr( constexpr_exec_tag const &, char const *first, char const *last,
	         char c ) {
		if constexpr( is_unchecked_input ) {
			while( *first != c ) {
				++first;
			}
			return first;
		} else {
			while( first < last and *first != c ) {
				++first;
			}
			return first;
		}
	}
} // namespace daw::json::json_details
