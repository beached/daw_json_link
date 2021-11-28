// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/
//

#pragma once

#include "daw_fp_properties.h"

#include <daw/daw_bit_cast.h>

#include <limits>
#include <type_traits>

namespace daw::json {
	DAW_JSON_INLINE_NS namespace DAW_JSON_VER {
		namespace fp_utils {
			template<typename Float>
			struct fp_data {
				static_assert( std::is_floating_point_v<Float>, "Only floating point types are supported" );

				// Reinterpreting bits as an integer value and interpreting the bits of
				// an integer value as a floating point value is used in tests. So, a
				// convenient type is provided for such reinterpretations.
				using fp_prop = fp_properties<Float>;
				using unsigned_t = typename fp_prop::unsigned_t;

				unsigned_t bits{ };

				constexpr void set_mantissa( unsigned_t mant_val ) noexcept {
					mant_val &= ( fp_prop::mantissa_mask );
					bits &= ~( fp_prop::mantissa_mask );
					bits |= mant_val;
				}

				constexpr unsigned_t get_mantissa( ) const noexcept {
					return bits & fp_prop::mantissa_mask;
				}

				constexpr void set_unbiased_exponent( unsigned_t expVal ) noexcept {
					expVal = ( expVal << ( fp_prop::mantissa_width ) ) & fp_prop::exponent_mask;
					bits &= ~( fp_prop::exponent_mask );
					bits |= expVal;
				}

				constexpr uint16_t get_unbiased_exponent( ) const noexcept {
					return uint16_t( ( bits & fp_prop::exponent_mask ) >> ( fp_prop::mantissa_width ) );
				}

				constexpr void set_sign( bool sign_val ) noexcept {
					bits &= ~( fp_prop::sign_mask );
					unsigned_t sign = unsigned_t( sign_val ) << ( fp_prop::bit_width - 1 );
					bits |= sign;
				}

				constexpr bool get_sign( ) const noexcept {
					return ( ( bits & fp_prop::sign_mask ) >> ( fp_prop::bit_width - 1 ) );
				}

				static_assert( sizeof( Float ) == sizeof( unsigned_t ),
				               "Data type and integral representation have different sizes." );

				static constexpr int exponent_bias =
				  ( 1 << ( fp_prop::template exponent_width<Float>::value - 1 ) ) - 1;
				static constexpr int max_exponent =
				  ( 1 << fp_prop::template exponent_width<Float>::value ) - 1;

				static constexpr unsigned_t min_subnormal = unsigned_t{ 1U };
				static constexpr unsigned_t max_subnormal =
				  ( unsigned_t{ 1U } << fp_prop::template mantissa_width<Float>::value ) - 1;
				static constexpr unsigned_t min_normal =
				  ( unsigned_t{ 1U } << fp_prop::template mantissa_width<Float>::value );
				static constexpr unsigned_t max_normal =
				  ( ( unsigned_t( max_exponent ) - 1 ) << fp_prop::template mantissa_width<Float>::value ) |
				  max_subnormal;

				// Use SFINAE to prevent accidental type promotions/conversions
				template<typename F, std::enable_if_t<std::is_same_v<Float, F>, std::nullptr_t> = nullptr>
				explicit constexpr fp_data( F float_value ) noexcept
				  : bits( DAW_BIT_CAST( Float, float_value ) ) {}

				template<typename U,
				         std::enable_if_t<std::is_same_v<unsigned_t, U>, std::nullptr_t> = nullptr>
				explicit constexpr fp_data( U unsigned_value ) noexcept
				  : bits( unsigned_value ) {}

				explicit constexpr fp_data( ) = default;

				constexpr unsigned_t unsigned_value( ) const noexcept {
					return bits;
				}

				constexpr unsigned_t float_value( ) const noexcept {
					return DAW_BIT_CAST( Float, bits );
				}

				constexpr int get_exponent( ) const noexcept {
					return int( get_unbiased_exponent( ) ) - exponent_bias;
				}

				constexpr bool is_zero( ) const noexcept {
					return get_mantissa( ) == 0 and get_unbiased_exponent( ) == 0;
				}

				constexpr bool is_inf( ) const noexcept {
					return get_mantissa( ) == 0 and get_unbiased_exponent( ) == max_exponent;
				}

				constexpr bool is_NaN( ) const noexcept {
					return get_unbiased_exponent( ) == max_exponent and get_mantissa( ) != 0;
				}

				constexpr bool is_inf_or_NaN( ) const noexcept {
					return get_unbiased_exponent( ) == max_exponent;
				}

				static constexpr fp_data zero( ) noexcept {
					return fp_data( );
				}

				static constexpr fp_data neg_zero( ) noexcept {
					return fp_data( unsigned_t{ 1U } << ( sizeof( unsigned_t ) * CHAR_BIT - 1U ) );
				}

				static constexpr fp_data inf( ) noexcept {
					auto bits = fp_data{ };
					bits.setUnbiased_exponent( max_exponent );
					return bits;
				}

				static constexpr fp_data neg_inf( ) noexcept {
					auto bits = inf( );
					bits.set_sign( 1 );
					return bits;
				}

				static constexpr Float build_NaN( unsigned_t v ) noexcept {
					auto bits = inf( );
					bits.set_mantissa( v );
					return bits.float_value( );
				}
			};
		} // namespace fp_utils
	}
} // namespace daw::json