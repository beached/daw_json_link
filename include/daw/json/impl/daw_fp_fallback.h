// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/
//

#pragma once

#include <daw/daw_bit_cast.h>

#include <boost/multiprecision/cpp_bin_float.hpp>
#include <boost/multiprecision/cpp_dec_float.hpp>
#include <boost/multiprecision/cpp_int.hpp>
#include <cstdint>

namespace daw::json {
	inline namespace DAW_JSON_VER {
		namespace json_details {
			namespace math {
				using bigfloat_t = boost::multiprecision::cpp_dec_float<1000>;

				template<unsigned Bits>
				using basic_bigint_t =
				  boost::multiprecision::number<boost::multiprecision::cpp_int_backend<
				    Bits, Bits, boost::multiprecision::signed_magnitude,
				    boost::multiprecision::unchecked, void>>;

				// Enough for 1000 digits (log(2^bits)/log(10)) or dig*log(10)/log(2)
				using bigint_t = basic_bigint_t<3328U>;

				inline constexpr std::int32_t min3( std::int32_t a, std::int32_t b,
				                                    std::int32_t c ) {
					if( a > b ) {
						a = b;
					}
					if( a > c ) {
						a = c;
					}
					return a;
				}

				constexpr std::int32_t get_msb( bigint_t const &num ) {
					std::size_t const limb_count = num.backend( ).size( );
					if( limb_count == 0 ) {
						return 0;
					}
					constexpr std::size_t limb_bits =
					  sizeof( boost::multiprecision::limb_type ) * CHAR_BIT;
					constexpr std::size_t int_bits = sizeof( int ) * CHAR_BIT;
					static_assert( limb_bits >= int_bits );

					std::size_t bit_count = ( limb_count - 1U ) * limb_bits;
					// TODO: replace with more generic function
					auto last_limb =
					  num.backend( )
					    .limbs( )[static_cast<std::size_t>( limb_count ) - 1U];
					last_limb >>= limb_bits - int_bits;
					auto const lz_count =
					  __builtin_clz( static_cast<unsigned>( last_limb ) );
					bit_count +=
					  ( limb_bits - 1U ) - static_cast<std::size_t>( lz_count );
					return static_cast<std::int32_t>( bit_count );
				}
			} // namespace math

			template<typename Real, std::enable_if_t<std::is_floating_point_v<Real>,
			                                         std::nullptr_t> = nullptr>
			inline Real parse_with_strtod( char const *orig_first ) {
				char **end = nullptr;
				if constexpr( std::is_same_v<Real, float> ) {
					return strtof( orig_first, end );
				} else if( std::is_same_v<Real, double> ) {
					return strtod( orig_first, end );
				} else {
					return static_cast<Real>( strtold( orig_first, end ) );
				}
			}

		} // namespace json_details
	}   // namespace DAW_JSON_VER
} // namespace daw::json
