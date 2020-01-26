// The MIT License (MIT)
//
// Copyright (c) 2019-2020 Darrell Wright
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files( the "Software" ), to
// deal in the Software without restriction, including without limitation the
// rights to use, copy, modify, merge, publish, distribute, sublicense, and / or
// sell copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#pragma once

#include "daw_iterator_range.h"
#include "daw_json_assert.h"
#include "daw_json_parse_unsigned_int.h"

#include <daw/daw_cxmath.h>

namespace daw::json::json_details {
	template<typename Result, SIMDModes SIMDMode = SIMDModes::None,
	         typename First, typename Last, bool IsUnCheckedInput>
	[[nodiscard]] constexpr Result
	parse_real( IteratorRange<First, Last, IsUnCheckedInput> &rng ) {
		// [-]WHOLE[.FRACTION][(e|E)[+|-]EXPONENT]
		daw_json_assert_weak( rng.has_more( ) and rng.is_real_number_part( ),
		                      "Expected a real number" );

		int sign = 1;
		if( rng.front( ) == '-' ) {
			rng.remove_prefix( );
			sign = -1;
		} else if( rng.front( ) == '+' ) {
			rng.remove_prefix( );
		}
		daw_json_assert_weak( rng.is_number( ), "Expected a number" );
		auto const whole_part = static_cast<Result>(
		  sign *
		  parse_unsigned_integer<int64_t, JsonRangeCheck::Never, SIMDMode>( rng ) );

		Result fract_part = 0.0;
		if( rng.front( ) == '.' ) {
			rng.remove_prefix( );

			auto fract_tmp = parse_unsigned_integer2<std::uint64_t, SIMDMode>( rng );
			fract_part = static_cast<Result>( fract_tmp.value );
			fract_part *= static_cast<Result>(
			  daw::cxmath::dpow10( -static_cast<int32_t>( fract_tmp.count ) ) );
			fract_part = daw::cxmath::copy_sign( fract_part, whole_part );
		}

		int32_t exp_part = 0;
		if( auto const frnt = rng.front( ); frnt == 'e' or frnt == 'E' ) {
			rng.remove_prefix( );
			int32_t const exsign = [&]( ) -> int32_t {
				if( rng.front( ) == '-' ) {
					rng.remove_prefix( );
					return -1;
				} else if( rng.front( ) == '+' ) {
					rng.remove_prefix( );
				}
				return 1;
			}( );
			exp_part =
			  exsign *
			  parse_unsigned_integer<
			    int32_t, ( IsUnCheckedInput ? JsonRangeCheck::Never
			                                : JsonRangeCheck::CheckForNarrowing )>(
			    rng );
		}
		if constexpr( std::is_same_v<Result, float> ) {
			return ( whole_part + fract_part ) * daw::cxmath::fpow10( exp_part );

		} else {
			return ( whole_part + fract_part ) *
			       static_cast<Result>( daw::cxmath::dpow10( exp_part ) );
		}
	}

} // namespace daw::json::json_details
