// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/daw_json_link
//

#pragma once

#include "daw_iterator_range.h"
#include "daw_json_assert.h"
#include "daw_json_parse_unsigned_int.h"

#include <daw/daw_cxmath.h>

namespace daw::json::json_details {
	template<typename Result, typename Range>
	[[nodiscard]] inline constexpr Result parse_real( Range &rng ) {
		// [-]WHOLE[.FRACTION][(e|E)[+|-]EXPONENT]
		daw_json_assert_weak(
		  rng.has_more( ) and
		    parse_policy_details::is_real_number_part( rng.front( ) ),
		  "Expected a real number" );

		std::int32_t sign = 1;
		if( rng.front( ) == '-' ) {
			rng.remove_prefix( );
			sign = -1;
		} else if( rng.front( ) == '+' ) {
			rng.remove_prefix( );
		}
		daw_json_assert_weak( rng.is_number( ), "Expected a number" );
		auto const whole_part = static_cast<Result>(
		  sign * unsigned_parser<int64_t, JsonRangeCheck::Never>(
		           SIMDConst_v<Range::simd_mode>, rng ) );

		Result fract_part = 0.0;
		if( rng.front( ) == '.' ) {
			rng.remove_prefix( );

			char const *orig_first = rng.first;
			fract_part = unsigned_parser<Result, JsonRangeCheck::Never>(
			  SIMDConst_v<Range::simd_mode>, rng );
			fract_part *= static_cast<Result>( daw::cxmath::dpow10(
			  -static_cast<int32_t>( rng.first - orig_first ) ) );
			fract_part = daw::cxmath::copy_sign( fract_part, whole_part );
		}

		int32_t exp_part = 0;
		if( parse_policy_details::in( rng.front( ), "eE" ) ) {
			rng.remove_prefix( );
			int32_t exsign = 1;
			if( rng.front( ) == '-' ) {
				rng.remove_prefix( );
				exsign = -1;
			} else if( rng.front( ) == '+' ) {
				rng.remove_prefix( );
			}
			exp_part =
			  exsign *
			  unsigned_parser<int32_t, ( Range::is_unchecked_input
			                               ? JsonRangeCheck::Never
			                               : JsonRangeCheck::CheckForNarrowing )>(
			    SIMDConst_v<Range::simd_mode>, rng );
		}
		if constexpr( std::is_same_v<Result, float> ) {
			return ( whole_part + fract_part ) * daw::cxmath::fpow10( exp_part );

		} else {
			return ( whole_part + fract_part ) *
			       static_cast<Result>( daw::cxmath::dpow10( exp_part ) );
		}
	}
} // namespace daw::json::json_details
