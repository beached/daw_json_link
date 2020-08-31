// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/daw_json_link
//

#pragma once

#include "daw_json_assert.h"
#include "daw_json_iterator_range.h"
#include "daw_json_parse_unsigned_int.h"

#include <daw/daw_cxmath.h>

namespace daw::json::json_details {
	template<typename Result>
	constexpr Result power10( constexpr_exec_tag const &, std::int32_t p ) {
		if constexpr( std::is_same_v<Result, float> ) {
			return daw::cxmath::fpow10( p );
		} else {
			return static_cast<Result>( daw::cxmath::dpow10( p ) );
		}
	}

	template<typename Result>
	constexpr Result copy_sign( constexpr_exec_tag const &, Result to,
	                            Result from ) {
		return daw::cxmath::copy_sign( to, from );
	}
	template<typename Result>
	inline Result copy_sign( runtime_exec_tag const &, Result to, Result from ) {
		return std::copysign( to, from );
	}

	template<typename Result, bool KnownRange, typename Range,
	         std::enable_if_t<KnownRange, std::nullptr_t> = nullptr>
	[[nodiscard]] inline constexpr Result parse_real( Range const &rng ) {
		// [-]WHOLE[.FRACTION][(e|E)[+|-]EXPONENT]
		daw_json_assert_weak(
		  rng.has_more( ) and parse_policy_details::is_number_start( rng.front( ) ),
		  "Expected a real number" );

		auto whole_rng = Range( rng.first, rng.class_first );
		auto fract_rng = Range( rng.class_first, rng.class_last );
		auto exp_rng = Range( rng.class_last, rng.last );
		if( rng.class_first == nullptr ) {
			if( rng.class_last == nullptr ) {
				whole_rng.last = rng.last;
			} else {
				whole_rng.last = rng.class_last;
			}
		} else if( rng.class_last == nullptr ) {
			fract_rng.last = rng.last;
		}

		std::int32_t sign = 1;
		if( whole_rng.front( ) == '-' ) {
			whole_rng.remove_prefix( );
			sign = -1;
		}
		auto const whole_part = static_cast<Result>(
		  sign * unsigned_parser<int64_t, JsonRangeCheck::Never, true>(
		           Range::exec_tag, whole_rng ) );

		Result fract_part = 0.0;
		if( fract_rng.first != nullptr ) {
			fract_rng.remove_prefix( );
			auto const fract_pow = power10<Result>(
			  Range::exec_tag,
			  -static_cast<std::int32_t>( fract_rng.last - fract_rng.first ) );
			fract_part = static_cast<Result>(
			  unsigned_parser<uint64_t, JsonRangeCheck::Never, true>( Range::exec_tag,
			                                                          fract_rng ) );
			fract_part *= fract_pow;
			fract_part = copy_sign( Range::exec_tag, fract_part, whole_part );
		}

		int32_t exp_part = 0;
		if( exp_rng.first != nullptr ) {
			exp_rng.remove_prefix( );
			int32_t exsign = 1;
			daw_json_assert_weak( exp_rng.has_more( ), "Unexpected end of stream" );
			if( exp_rng.front( ) == '-' ) {
				exp_rng.remove_prefix( );
				exsign = -1;
			} else if( exp_rng.front( ) == '+' ) {
				exp_rng.remove_prefix( );
			}
			daw_json_assert_weak( exp_rng.has_more( ), "Unexpected end of stream" );
			exp_part =
			  exsign * unsigned_parser<int32_t,
			                           ( Range::is_unchecked_input
			                               ? JsonRangeCheck::Never
			                               : JsonRangeCheck::CheckForNarrowing ),
			                           true>( Range::exec_tag, exp_rng );
		}
		return ( whole_part + fract_part ) *
		       power10<Result>( Range::exec_tag, exp_part );
	}

	template<typename Result, bool KnownRange, typename Range,
	         std::enable_if_t<not KnownRange, std::nullptr_t> = nullptr>
	[[nodiscard]] inline constexpr Result parse_real( Range &rng ) {
		// [-]WHOLE[.FRACTION][(e|E)[+|-]EXPONENT]
		daw_json_assert_weak(
		  rng.has_more( ) and parse_policy_details::is_number_start( rng.front( ) ),
		  "Expected a real number" );

		std::int32_t sign = 1;
		if( rng.front( ) == '-' ) {
			rng.remove_prefix( );
			sign = -1;
		}
		auto const whole_part = static_cast<Result>(
		  sign * unsigned_parser<int64_t, JsonRangeCheck::Never, false>(
		           Range::exec_tag, rng ) );

		Result fract_part = 0.0;
		if( rng.front( ) == '.' ) {
			rng.remove_prefix( );

			char const *orig_first = rng.first;
			fract_part = static_cast<Result>(
			  unsigned_parser<uint64_t, JsonRangeCheck::Never, false>(
			    Range::exec_tag, rng ) );
			fract_part *= static_cast<Result>( power10<Result>(
			  Range::exec_tag, -static_cast<int32_t>( rng.first - orig_first ) ) );
			fract_part = copy_sign( Range::exec_tag, fract_part, whole_part );
		}

		int32_t exp_part = 0;
		if( rng.is_exponent_checked( ) ) {
			rng.remove_prefix( );
			int32_t exsign = 1;
			daw_json_assert_weak( rng.has_more( ), "Unexpected end of stream" );
			if( rng.front( ) == '-' ) {
				rng.remove_prefix( );
				exsign = -1;
			} else if( rng.front( ) == '+' ) {
				rng.remove_prefix( );
			}
			if( rng.has_more( ) ) {
				exp_part =
				  exsign * unsigned_parser<int32_t,
				                           ( Range::is_unchecked_input
				                               ? JsonRangeCheck::Never
				                               : JsonRangeCheck::CheckForNarrowing ),
				                           false>( Range::exec_tag, rng );
			}
		}
		return ( whole_part + fract_part ) *
		       power10<Result>( Range::exec_tag, exp_part );
	}
} // namespace daw::json::json_details
