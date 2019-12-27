// The MIT License (MIT)
//
// Copyright (c) 2019 Darrell Wright
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

#include <daw/daw_cxmath.h>

#include "daw_iterator_range.h"
#include "daw_json_assert.h"
#include "daw_json_parse_signed_int.h"
#include "daw_json_parse_unsigned_int.h"

namespace daw::json::impl {
	namespace {
		template<typename Result, typename First, typename Last,
		         bool IsTrustedInput>
		[[nodiscard]] static constexpr Result
		parse_real( IteratorRange<First, Last, IsTrustedInput> &rng ) noexcept {
			// [-]WHOLE[.FRACTION][(e|E)[+|-]EXPONENT]
			daw_json_assert_untrusted( rng.is_real_number_part( ),
			                           "Expected a real number" );
			auto const sign = [&] {
				if( rng.front( ) == '-' ) {
					rng.remove_prefix( );
					return -1;
				}
				return 1;
			}( );
			auto const whole_part =
			  static_cast<Result>( sign * parse_unsigned_integer<int64_t>( rng ) );

			Result fract_part = 0.0;
			if( rng.front( ) == '.' ) {
				rng.remove_prefix( );

				auto fract_tmp = parse_unsigned_integer2<uint64_t>( rng );
				fract_part = static_cast<Result>( fract_tmp.value );
				fract_part *= static_cast<Result>(
				  daw::cxmath::dpow10( -static_cast<int32_t>( fract_tmp.count ) ) );
				fract_part = daw::cxmath::copy_sign( fract_part, whole_part );
			}

			int32_t exp_part = 0;
			if( auto const frnt = rng.front( ); frnt == 'e' or frnt == 'E' ) {
				rng.remove_prefix( );
				exp_part = parse_integer<int32_t>( rng );
			}
			if constexpr( std::is_same_v<Result, float> ) {
				return ( whole_part + fract_part ) * daw::cxmath::fpow10( exp_part );

			} else {
				return ( whole_part + fract_part ) *
				       static_cast<Result>( daw::cxmath::dpow10( exp_part ) );
			}
		}
	} // namespace
} // namespace daw::json::impl
