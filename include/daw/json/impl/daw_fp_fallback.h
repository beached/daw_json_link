// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/
//

#pragma once

#include "daw_json_assert.h"

#include <daw/daw_attributes.h>
#include <daw/daw_cpp_feature_check.h>

#if not defined( DAW_JSON_USE_STRTOD ) and defined( __cpp_lib_to_chars )
#include <charconv>
#endif

namespace daw::json {
	inline namespace DAW_JSON_VER {
		namespace json_details {

			template<typename Real>
			requires( std::is_floating_point_v<Real> ) //
			  DAW_ATTRIB_NOINLINE Real
			  parse_with_strtod( char const *first, char const *last ) {
#if not defined( DAW_JSON_USE_STRTOD ) and defined( __cpp_lib_to_chars )
				Real result;
				std::from_chars_result fc_res = std::from_chars( first, last, result );
				if( fc_res.ec == std::errc::result_out_of_range ) {
					if( *first == '-' ) {
						return -std::numeric_limits<Real>::infinity( );
					}
					return std::numeric_limits<Real>::infinity( );
				}
				daw_json_ensure( fc_res.ec == std::errc( ),
				                 ErrorReason::InvalidNumber );
				return result;
#else
				(void)last;
				char **end = nullptr;
				if constexpr( std::is_same_v<Real, float> ) {
					return static_cast<Real>( strtof( first, end ) );
				} else if( std::is_same_v<Real, double> ) {
					return static_cast<Real>( strtod( first, end ) );
				} else {
					return static_cast<Real>( strtold( first, end ) );
				}
#endif
			}
		} // namespace json_details
	}   // namespace DAW_JSON_VER
} // namespace daw::json
