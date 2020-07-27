// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/daw_json_link
//

#pragma once

#include "daw_json_assert.h"

#include <daw/daw_uint_buffer.h>

namespace daw::json::json_details::string_quote {
	template<std::size_t N>
	inline constexpr bool test_at_byte( UInt64Buffer b, char c ) {
		auto const lhs = static_cast<std::uint64_t>( b ) &
		                 ( static_cast<std::uint64_t>( 0xFF ) << ( N * 8U ) );
		auto const rhs =
		  static_cast<std::uint64_t>( static_cast<unsigned char>( c ) )
		  << ( N * 8U );
		return not( lhs - rhs );
	}

	inline constexpr void skip_to_first8( char const *&first,
	                                      char const *const last ) {
		bool keep_going = last - first >= 8;
		while( keep_going ) {
			auto buff = daw::to_uint64_buffer( first );
			auto const q7 = test_at_byte<7U>( buff, '"' );
			auto const q6 = test_at_byte<6U>( buff, '"' );
			auto const q5 = test_at_byte<5U>( buff, '"' );
			auto const q4 = test_at_byte<4U>( buff, '"' );
			auto const q3 = test_at_byte<3U>( buff, '"' );
			auto const q2 = test_at_byte<2U>( buff, '"' );
			auto const q1 = test_at_byte<1U>( buff, '"' );
			auto const q0 = test_at_byte<0U>( buff, '"' );
			auto const s7 = test_at_byte<7U>( buff, '\\' );
			auto const s6 = test_at_byte<6U>( buff, '\\' );
			auto const s5 = test_at_byte<5U>( buff, '\\' );
			auto const s4 = test_at_byte<4U>( buff, '\\' );
			auto const s3 = test_at_byte<3U>( buff, '\\' );
			auto const s2 = test_at_byte<2U>( buff, '\\' );
			auto const s1 = test_at_byte<1U>( buff, '\\' );
			auto const s0 = test_at_byte<0U>( buff, '\\' );
			keep_going = not( q0 | q1 | q2 | q3 | q4 | q5 | q6 | q7 | s0 | s1 | s2 |
			                  s3 | s4 | s5 | s6 | s7 );
			keep_going = keep_going & static_cast<bool>( last - ( first + 8 ) >= 8 );
			first += static_cast<int>( keep_going ) * 8;
		}
	}

	inline constexpr void skip_to_first4( char const *&first,
	                                      char const *const last ) {
		bool keep_going = last - first >= 4;
		while( keep_going ) {
			auto buff = daw::to_uint64_buffer( first );
			auto const q3 = test_at_byte<3U>( buff, '"' );
			auto const q2 = test_at_byte<2U>( buff, '"' );
			auto const q1 = test_at_byte<1U>( buff, '"' );
			auto const q0 = test_at_byte<0U>( buff, '"' );
			auto const s3 = test_at_byte<3U>( buff, '\\' );
			auto const s2 = test_at_byte<2U>( buff, '\\' );
			auto const s1 = test_at_byte<1U>( buff, '\\' );
			auto const s0 = test_at_byte<0U>( buff, '\\' );
			keep_going = not( q0 | q1 | q2 | q3 | s0 | s1 | s2 | s3 );
			keep_going = keep_going & static_cast<bool>( last - ( first + 4 ) >= 4 );
			first += static_cast<int>( keep_going ) * 4;
		}
	}

	struct string_quote_parser {
		template<typename Range>
		[[nodiscard]] static constexpr auto parse_nq( Range &rng )
		  -> std::enable_if_t<Range::is_unchecked_input, bool> {
			bool need_slow_path = false;
			char const *first = rng.first;
			if( char const *const last = rng.last; last - first >= 8 ) {
				skip_to_first8( first, last );
				first -= static_cast<int>( not( *( first - 1 ) - '\\' ) );
			} else if( last - first >= 4 ) {
				first -= static_cast<int>( not( *( first - 1 ) - '\\' ) );
				skip_to_first4( first, last );
			}
			while( *first != '"' ) {
				while( *first != '"' and *first != '\\' ) {
					++first;
				}
				if( DAW_JSON_UNLIKELY( *first == '\\' ) ) {
					need_slow_path = true;
					first += 2;
				} else {
					break;
				}
			}
			rng.first = first;
			return need_slow_path;
		}

		template<typename Range>
		[[nodiscard]] static constexpr auto parse_nq( Range &rng )
		  -> std::enable_if_t<not Range::is_unchecked_input, bool> {
			bool need_slow_path = false;
			char const *first = rng.first;
			char const *const last = rng.class_last;
			if( char const *const l = rng.last; l - first >= 8 ) {
				skip_to_first8( first, l );
				first -= static_cast<int>( not( *( first - 1 ) - '\\' ) );
			} else if( l - first >= 4 ) {
				skip_to_first4( first, l );
				first -= static_cast<int>( not( *( first - 1 ) - '\\' ) );
			}
			while( first < last and *first != '"' ) {
				while( first < last and *first != '"' and *first != '\\' ) {
					++first;
				}
				if( DAW_JSON_UNLIKELY( first < last and *first == '\\' ) ) {
					need_slow_path = true;
					first += 2;
				} else {
					break;
				}
			}
			daw_json_assert( first < last and *first == '"',
			                 "Expected a '\"' at end of string" );
			rng.first = first;
			return need_slow_path;
		}
	};
} // namespace daw::json::json_details::string_quote
