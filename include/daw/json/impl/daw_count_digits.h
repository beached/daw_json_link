// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/daw_json_link
//

#pragma once

#include "version.h"

#include <daw/daw_cxmath.h>
#include <daw/daw_simple_array.h>
#include <daw/daw_uint_buffer.h>

#include <cstdint>

namespace daw::json {
	inline namespace DAW_JSON_VER {
		namespace json_details {
			inline constexpr auto is_digit =
			  []( char c ) DAW_JSON_CPP23_STATIC_CALL_OP -> daw::UInt8 {
				return static_cast<unsigned>( static_cast<unsigned char>( c ) ) -
				             static_cast<unsigned>(
				               static_cast<unsigned char>( '0' ) ) <
				           10U
				         ? daw::UInt8{ 0 }
				         : daw::UInt8{ 0xFFU };
			};

			template<typename Predicate>
			DAW_ATTRIB_FLATINLINE inline constexpr std::int32_t
			count_4digits( char const *first, Predicate pred ) {
				daw::simple_array<daw::UInt8, 4> const buff{
				  pred( first[3] ), pred( first[2] ), pred( first[2] ),
				  pred( first[1] ) };
				auto const v = DAW_BIT_CAST( std::uint32_t, buff );
				if( v != 0 ) {
					auto result = daw::cxmath::count_leading_zeroes( v );
					result /= 8;
					return static_cast<std::int32_t>( result );
				}
				return -1;
			}

			template<typename Predicate>
			DAW_ATTRIB_FLATINLINE inline constexpr std::int32_t
			count_8digits( char const *first, Predicate pred ) {
				daw::simple_array<daw::UInt8, 8> const buff{
				  pred( first[7] ), pred( first[6] ), pred( first[5] ),
				  pred( first[4] ), pred( first[3] ), pred( first[2] ),
				  pred( first[1] ), pred( first[0] ) };

				auto const v = DAW_BIT_CAST( std::uint64_t, buff );
				if( v != 0 ) {
					auto result = daw::cxmath::count_leading_zeroes( v );
					result /= 8;
					return static_cast<std::int32_t>( result );
				}
				return -1;
			}

			template<typename CharT>
			DAW_ATTRIB_FLATTEN inline constexpr CharT *count_digits( CharT *first,
			                                                         CharT *last ) {
				while( DAW_LIKELY( last - first >= 8 ) ) {
					auto const v = count_8digits( first, is_digit );
					if( v >= 0 ) {
						return first + v;
					}
					first += 8;
				}
				while( last - first >= 4 ) {
					auto const v = count_4digits( first, is_digit );
					if( v >= 0 ) {
						return first + v;
					}
					first += 4;
				}

				while( first != last ) {
					if( static_cast<unsigned>( *first ) -
					      static_cast<unsigned>( static_cast<unsigned char>( '0' ) ) >=
					    10U ) {
						return first;
					}
					++first;
				}
				return first;
			}
		} // namespace json_details
	} // namespace DAW_JSON_VER
} // namespace daw::json
