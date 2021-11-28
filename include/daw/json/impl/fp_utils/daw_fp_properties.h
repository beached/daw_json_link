// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/
//

#pragma once

#include <cfloat>
#include <climits>
#include <cstdint>
#include <limits>
#include <type_traits>

namespace daw::json {
	DAW_JSON_INLINE_NS namespace DAW_JSON_VER {
		namespace fp_utils {
			template<typename Float>
			struct fp_properties;

			template<>
			struct fp_properties<float> {
				using unsigned_t = std::uint32_t;
				static_assert( sizeof( unsigned_t ) == sizeof( float ),
				               "Unexpected size of 'float' type." );

				static constexpr std::uint32_t bit_width = sizeof( unsigned_t ) * CHAR_BIT;

				static constexpr std::uint32_t mantissa_width = 23;
				static constexpr std::uint32_t exponent_width = 8;
				static constexpr unsigned_t mantissa_mask = ( unsigned_t{ 1U } << mantissa_width ) - 1U;
				static constexpr unsigned_t sign_mask = unsigned_t{ 1U }
				                                        << ( exponent_width + mantissa_width );
				static constexpr unsigned_t exponent_mask = ~( sign_mask | mantissa_mask );
				static constexpr std::uint32_t exponent_bias = 127U;

				// If a number x is a NAN, then it is a quiet NAN if:
				//   QuietNaN_mask & bits(x) != 0
				// Else, it is a signalling NAN.
				static constexpr unsigned_t quiet_NaN_mask = 0x00400000U;
			};

			template<>
			struct fp_properties<double> {
				using unsigned_t = std::uint64_t;
				static_assert( sizeof( unsigned_t ) == sizeof( double ),
				               "Unexpected size of 'double' type." );

				static constexpr std::uint32_t bit_width = sizeof( unsigned_t ) * CHAR_BIT;

				static constexpr std::uint32_t mantissa_width = 52;
				static constexpr std::uint32_t exponent_width = 11;
				static constexpr unsigned_t mantissa_mask = ( unsigned_t{ 1U } << mantissa_width ) - 1;
				static constexpr unsigned_t sign_mask = unsigned_t{ 1U }
				                                        << ( exponent_width + mantissa_width );
				static constexpr unsigned_t exponent_mask = ~( sign_mask | mantissa_mask );
				static constexpr std::uint32_t exponent_bias = 1023;

				// If a number x is a NAN, then it is a quiet NAN if:
				//   QuietNaN_mask & bits(x) != 0
				// Else, it is a signalling NAN.
				static constexpr unsigned_t quiet_NaN_mask = 0x0008000000000000ULL;
			};
		} // namespace fp_utils
	}
} // namespace daw::json
