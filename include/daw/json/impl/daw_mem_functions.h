// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/daw_json_link
//

#pragma once

#include "daw_exec_modes.h"
#include "daw_mem_functions_common.h"
#include "daw_mem_functions_sse42.h"

#include <daw/daw_hide.h>
#include <daw/daw_uint_buffer.h>

#include <cstring>

namespace daw::json::json_details {
	DAW_ATTRIBUTE_FLATTEN static inline constexpr bool
	is_escaped( char const *ptr, char const *min_ptr ) {
		if( *( ptr - 1 ) != '\\' ) {
			return false;
		}
		if( ( ptr - min_ptr ) < 2 ) {
			return false;
		}
		return *( ptr - 2 ) != '\\';
	}

	namespace {
		template<std::size_t N, char c>
		inline constexpr bool test_at_byte( UInt64Buffer b ) {
			auto const lhs = b & ( static_cast<std::uint64_t>( 0xFF ) << ( N * 8U ) );
			auto const rhs =
			  static_cast<std::uint64_t>( static_cast<unsigned char>( c ) )
			  << ( N * 8U );
			return not( lhs - rhs );
		}

		template<std::size_t N, char c>
		inline constexpr bool test_at_byte( UInt32Buffer b ) {
			auto const lhs = b & ( static_cast<std::uint32_t>( 0xFF ) << ( N * 8U ) );
			auto const rhs =
			  static_cast<std::uint32_t>( static_cast<unsigned char>( c ) )
			  << ( N * 8U );
			return not( lhs - rhs );
		}
		inline constexpr void skip_to_first8( char const *&first,
		                                      char const *const last ) {
			bool keep_going = last - first >= 8;
			while( keep_going ) {
				auto buff = daw::to_uint64_buffer( first );
				auto const q7 = test_at_byte<7U, '"'>( buff );
				auto const q6 = test_at_byte<6U, '"'>( buff );
				auto const q5 = test_at_byte<5U, '"'>( buff );
				auto const q4 = test_at_byte<4U, '"'>( buff );
				auto const q3 = test_at_byte<3U, '"'>( buff );
				auto const q2 = test_at_byte<2U, '"'>( buff );
				auto const q1 = test_at_byte<1U, '"'>( buff );
				auto const q0 = test_at_byte<0U, '"'>( buff );
				auto const s7 = test_at_byte<7U, '\\'>( buff );
				auto const s6 = test_at_byte<6U, '\\'>( buff );
				auto const s5 = test_at_byte<5U, '\\'>( buff );
				auto const s4 = test_at_byte<4U, '\\'>( buff );
				auto const s3 = test_at_byte<3U, '\\'>( buff );
				auto const s2 = test_at_byte<2U, '\\'>( buff );
				auto const s1 = test_at_byte<1U, '\\'>( buff );
				auto const s0 = test_at_byte<0U, '\\'>( buff );
				keep_going = not( q0 | q1 | q2 | q3 | q4 | q5 | q6 | q7 | s0 | s1 | s2 |
				                  s3 | s4 | s5 | s6 | s7 );
				keep_going =
				  keep_going & static_cast<bool>( last - ( first + 8 ) >= 8 );
				first += static_cast<int>( keep_going ) * 8;
			}
			first -= *( first - 1 ) == '\\' ? 1 : 0;
		}

		inline constexpr void skip_to_first4( char const *&first,
		                                      char const *const last ) {
			bool keep_going = last - first >= 4;
			while( keep_going ) {
				// Need to look for escapes as this is fast path
				auto buff = daw::to_uint32_buffer( first );
				auto const q3 = test_at_byte<3U, '"'>( buff );
				auto const q2 = test_at_byte<2U, '"'>( buff );
				auto const q1 = test_at_byte<1U, '"'>( buff );
				auto const q0 = test_at_byte<0U, '"'>( buff );
				auto const s3 = test_at_byte<3U, '\\'>( buff );
				auto const s2 = test_at_byte<2U, '\\'>( buff );
				auto const s1 = test_at_byte<1U, '\\'>( buff );
				auto const s0 = test_at_byte<0U, '\\'>( buff );
				keep_going = not( q0 | q1 | q2 | q3 | s0 | s1 | s2 | s3 );
				keep_going =
				  keep_going & static_cast<bool>( last - ( first + 4 ) >= 4 );
				first += static_cast<int>( keep_going ) * 4;
			}
			first -= *( first - 1 ) == '\\' ? 1 : 0;
		}
	} // namespace

	template<typename Range>
	DAW_ATTRIBUTE_FLATTEN static inline constexpr void
	mem_skip_string( constexpr_exec_tag const &, Range &rng ) {
		char const *first = rng.first;
		char const *const last = rng.last;
		if( last - first >= 8 ) {
			skip_to_first8( first, last );
		} else if( last - first >= 4 ) {
			skip_to_first4( first, last );
		}
		constexpr auto is_eq = []( char c ) {
			return ( c == '\\' ) | ( c == '"' );
		};
		if constexpr( Range::is_unchecked_input ) {
			while( not is_eq( *first ) ) {
				++first;
			}
		} else {
			while( first < last and not is_eq( *first ) ) {
				++first;
			}
		}
		rng.first = first;
	}

	template<typename Tag, typename Range>
	DAW_ATTRIBUTE_FLATTEN static inline constexpr void
	mem_skip_string( Tag const &tag, Range &rng ) {
		rng.first = mem_move_to_next_of<Range::is_unchecked_input, '"', '\\'>(
		  tag, rng.first, rng.last );
	}

	template<bool is_unchecked_input, typename Tag>
	DAW_ATTRIBUTE_FLATTEN static inline constexpr char const *
	mem_skip_until_end_of_string( Tag const &tag, char const *first,
	                              char const *const last ) {
		while( first < last ) {
			auto ptr =
			  mem_move_to_next_of<is_unchecked_input, '"'>( tag, first, last );
			if( DAW_JSON_LIKELY( not is_escaped( ptr, first ) ) ) {
				return ptr;
			} else {
				first = ptr + 1;
				continue;
			}
		}
		return first;
	}
} // namespace daw::json::json_details
