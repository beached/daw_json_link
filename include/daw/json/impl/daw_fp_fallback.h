// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/
//

#pragma once

#include "version.h"

#include "daw_json_assert.h"

#include <daw/daw_attributes.h>
#include <daw/daw_cpp_feature_check.h>

#if not defined( DAW_JSON_USE_STRTOD )
#include <charconv>
#endif

namespace daw::json {
	inline namespace DAW_JSON_VER {
		namespace json_details {
			/// @brief When normal FP parsing cannot handle the value, this method
			/// used as a fallback for std floating point types.  For others, it
			/// provides either a customization point or will call the overload found
			/// via ADL
			template<typename Real>
			DAW_ATTRIB_NOINLINE [[nodiscard]] Real
			parse_with_strtod( char const *first, char const *last ) {
				static_assert( std::is_floating_point_v<Real>,
				               "Execpected type passed to parse_with_strtod" );
#if defined( DAW_JSON_USE_STRTOD )
				(void)last;
				char **end = nullptr;
				if constexpr( std::is_same_v<Real, float> ) {
					return static_cast<Real>( strtof( first, end ) );
				} else if( std::is_same_v<Real, double> ) {
					return static_cast<Real>( strtod( first, end ) );
				} else {
					return static_cast<Real>( strtold( first, end ) );
				}
#else
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
#endif
			}
		} // namespace json_details
	}   // namespace DAW_JSON_VER
} // namespace daw::json
