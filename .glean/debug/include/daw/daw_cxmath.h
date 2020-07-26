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

#include <cstdint>
#include <limits>
#include <optional>

#include "daw_do_n.h"
#include "daw_enable_if.h"

namespace daw::cxmath {
	[[nodiscard]] constexpr std::optional<std::int16_t>
	fexp2( float const f ) noexcept;
	constexpr float fpow2( int32_t exp ) noexcept;

	namespace cxmath_impl {
		template<typename Float>
		inline constexpr auto sqrt2 = static_cast<Float>(
		  1.4142135623730950488016887242096980785696718753769480L );

		template<typename Float>
		inline constexpr auto sqrt0_5 = static_cast<Float>(
		  0.7071067811865475244008443621048490392848359376884740L );

		// Based on code from
		// https://graphics.stanford.edu/~seander/bithacks.html
		[[nodiscard]] constexpr std::uint32_t
		count_leading_zeroes( std::uint64_t v ) noexcept {
			char const bit_position[64] = {
			  0,  1,  2,  7,  3,  13, 8,  19, 4,  25, 14, 28, 9,  34, 20, 40,
			  5,  17, 26, 38, 15, 46, 29, 48, 10, 31, 35, 54, 21, 50, 41, 57,
			  63, 6,  12, 18, 24, 27, 33, 39, 16, 37, 45, 47, 30, 53, 49, 56,
			  62, 11, 23, 32, 36, 44, 52, 55, 61, 22, 43, 51, 60, 42, 59, 58};

			v |= v >> 1U; // first round down to one less than a power of 2
			v |= v >> 2U;
			v |= v >> 4U;
			v |= v >> 8U;
			v |= v >> 16U;
			v |= v >> 32U;
			v = ( v >> 1U ) + 1U;

			return 63U -
			       static_cast<std::uint32_t>(
			         bit_position[( v * 0x021'8a39'2cd3'd5dbf ) >> 58U] ); // [3]
		}

		[[nodiscard]] constexpr float pow( float b, int32_t exp ) noexcept {
			auto result = 1.0f;
			while( exp < 0 ) {
				result /= b;
				exp++;
			}
			while( exp > 0 ) {
				result *= b;
				exp--;
			}
			return result;
		}

		class float_parts_t {
			std::uint32_t m_raw_value{};
			float m_float_value{};

		public:
			static constexpr std::uint32_t const PosInf = 0x7F80'0000;
			static constexpr std::uint32_t const NegInf = 0xFF80'0000;
			static constexpr std::uint32_t const NaN = 0x7FC0'0000;

			constexpr float_parts_t( std::uint32_t i, float f ) noexcept
			  : m_raw_value( i )
			  , m_float_value( f ) {}

			[[nodiscard]] constexpr std::uint32_t raw_value( ) const noexcept {
				return m_raw_value;
			}

			[[nodiscard]] constexpr float float_value( ) const noexcept {
				return m_float_value;
			}

			[[nodiscard]] constexpr bool sign_bit( ) const noexcept {
				return ( m_raw_value >> 31U ) == 0U; // (-1)^S  0=pos, 1=neg
			}

			[[nodiscard]] constexpr bool is_positive( ) const noexcept {
				return sign_bit( );
			}

			[[nodiscard]] constexpr bool is_negative( ) const noexcept {
				return !sign_bit( );
			}

			[[nodiscard]] constexpr uint8_t raw_exponent( ) const noexcept {
				return static_cast<uint8_t>(
				  ( 0b0111'1111'1000'0000'0000'0000'0000'0000 & m_raw_value ) >> 23U );
			}

			[[nodiscard]] constexpr std::int16_t exponent( ) const noexcept {
				std::int16_t const bias = 127;
				return static_cast<std::int16_t>( raw_exponent( ) ) - bias;
			}

			[[nodiscard]] constexpr std::uint32_t raw_significand( ) const noexcept {
				return 0b0000'0000'0111'1111'1111'1111'1111'1111 & m_raw_value;
			}

			[[nodiscard]] constexpr float significand( ) const noexcept {
				float result = m_float_value;
				if( m_float_value < 0.0f ) {
					result = -result;
				}
				auto const e = exponent( );
				if( e < 0 ) {
					return result * daw::cxmath::fpow2( -e );
				}
				return result / daw::cxmath::fpow2( -e );
			}

			[[nodiscard]] constexpr bool is_pos_inf( ) const noexcept {
				return m_raw_value == PosInf;
			}

			[[nodiscard]] constexpr bool is_neg_inf( ) const noexcept {
				return m_raw_value == NegInf;
			}

			[[nodiscard]] constexpr bool is_inf( ) const noexcept {
				return is_pos_inf( ) or is_neg_inf( );
			}

			[[nodiscard]] constexpr bool is_nan( ) const noexcept {
				return m_raw_value == NaN;
			}
		};

		// From: http://brnz.org/hbr/?p=1518
		[[nodiscard]] constexpr float_parts_t bits( float const f ) noexcept {
			// Once c++20 use bit_cast
			if( f == 0.0f ) {
				return {0, f}; // also matches -0.0f and gives wrong result
			} else if( f > std::numeric_limits<float>::max( ) ) {
				// infinity
				return {0x7f80'0000, f};
			} else if( f < -std::numeric_limits<float>::max( ) ) {
				// negative infinity
				return {0xff800000, f};
			} else if( f != f ) {
				// NaN
				return {0x7fc0'0000, f};
			}
			bool sign = f < 0.0f;
			float abs_f = sign ? -f : f;
			int32_t exponent = 254;

			while( abs_f < 0x1p87f ) {
				abs_f *= 0x1p41f;
				exponent -= 41;
			}

			auto const a = static_cast<std::uint64_t>( abs_f * 0x1p-64f );
			auto lz = static_cast<int32_t>( count_leading_zeroes( a ) );
			exponent -= lz;

			if( exponent <= 0 ) {
				exponent = 0;
				lz = 8 - 1;
			}

			std::uint32_t significand = ( a << ( lz + 1 ) ) >> ( 64 - 23 ); // [3]
			return {( static_cast<std::uint32_t>( sign ? 1U : 0U ) << 31U ) |
			          ( static_cast<std::uint32_t>( exponent ) << 23U ) | significand,
			        f};
		}

		template<typename Float>
		[[nodiscard]] constexpr Float pow2_impl2( intmax_t exp ) noexcept {
			bool is_neg = exp < 0;
			exp = is_neg ? -exp : exp;
			auto const max_shft = daw::min(
			  static_cast<size_t>( std::numeric_limits<Float>::max_exponent10 ),
			  ( sizeof( size_t ) * 8ULL ) );
			Float result = 1.0;

			while( static_cast<size_t>( exp ) >= max_shft ) {
				result *= static_cast<Float>( std::numeric_limits<size_t>::max( ) );
				exp -= max_shft;
			}
			if( exp > 0 ) {
				result *= static_cast<Float>( 1ULL << static_cast<size_t>( exp ) );
			}
			if( is_neg && result != 0.0 ) {
				result = static_cast<Float>( 1.0 ) / result;
			}
			return result;
		}

		[[nodiscard]] constexpr size_t pow10_impl( intmax_t exp ) noexcept {
			// exp is < floor( log10( numeric_limits<size_t>::max( ) ) )
			size_t result = 1ULL;
			while( exp-- > 0 ) {
				result *= 10ULL;
			}
			return result;
		}

		template<intmax_t exp>
		[[nodiscard]] constexpr size_t pow10_impl( ) noexcept {
			return pow10_impl( exp );
		}

		template<typename Float>
		[[nodiscard]] constexpr Float fpow10_impl2( intmax_t exp ) noexcept {
			bool is_neg = exp < 0;
			exp = is_neg ? -exp : exp;

			auto const max_spow =
			  daw::min( std::numeric_limits<Float>::max_exponent10,
			            std::numeric_limits<size_t>::digits10 );
			Float result = 1.0;

			while( exp >= max_spow ) {
				result *= static_cast<Float>( pow10_impl<max_spow>( ) );
				exp -= max_spow;
			}
			if( exp > 0 ) {
				result *= static_cast<Float>( pow10_impl( exp ) );
			}
			if( is_neg && result != 0.0 ) {
				result = static_cast<Float>( 1.0 ) / result;
			}
			return result;
		}

		template<typename Integer>
		[[nodiscard]] constexpr Integer pow10_impl2( intmax_t exp ) noexcept {
			Integer result = 1;
			while( exp-- > 0 ) {
				result *= 10;
			}
			return result;
		}

		template<typename Float>
		[[nodiscard]] constexpr auto calc_pow2s( ) noexcept {
			intmax_t const min_e = std::numeric_limits<Float>::min_exponent10;
			intmax_t const max_e = std::numeric_limits<Float>::max_exponent10;
			std::array<Float, max_e - min_e> result{};
			intmax_t n = max_e - min_e;
			while( n-- > 0 ) {
				result[static_cast<size_t>( n )] = pow2_impl2<Float>( n + min_e );
			}
			return result;
		}

		template<typename Float>
		[[nodiscard]] constexpr auto calc_fpow10s( ) noexcept {
			intmax_t const min_e = std::numeric_limits<Float>::min_exponent10;
			intmax_t const max_e = std::numeric_limits<Float>::max_exponent10;
			std::array<Float, max_e - min_e> result{};
			intmax_t n = max_e - min_e;
			while( n-- > 0 ) {
				result[static_cast<size_t>( n )] = fpow10_impl2<Float>( n + min_e );
			}
			return result;
		}

		template<typename Integer>
		[[nodiscard]] constexpr auto calc_pow10s( ) noexcept {
			std::array<Integer, std::numeric_limits<Integer>::digits10> result{};
			intmax_t n = std::numeric_limits<Integer>::digits10;
			while( n-- > 0 ) {
				result[static_cast<size_t>( n )] = pow10_impl2<Integer>( n );
			}
			return result;
		}

		template<typename Float>
		class [[nodiscard]] pow2_t {
			static constexpr std::array const m_tbl = calc_pow2s<Float>( );

		public:
			template<typename Result = Float>
			[[nodiscard]] static constexpr Result get( intmax_t pos ) noexcept {
				auto const zero = static_cast<intmax_t>( m_tbl.size( ) / 2ULL );
				return static_cast<Result>( m_tbl[static_cast<size_t>( zero + pos )] );
			}
		};

		template<typename Float>
		class [[nodiscard]] fpow10_t {
			static constexpr std::array const m_tbl = calc_fpow10s<Float>( );

		public:
			template<typename Result = Float>
			[[nodiscard]] static constexpr Result get( intmax_t pos ) noexcept {
				auto const zero = static_cast<intmax_t>( m_tbl.size( ) / 2ULL );
				return static_cast<Result>( m_tbl[static_cast<size_t>( zero + pos )] );
			}
		};

		template<typename Integer>
		class [[nodiscard]] pow10_t {
			static constexpr std::array const m_tbl = calc_pow10s<Integer>( );

		public:
			template<typename Result = Integer>
			[[nodiscard]] static constexpr Result get( size_t pos ) noexcept {
				return static_cast<Result>( m_tbl[pos] );
			}
		};

		[[nodiscard]] constexpr float fexp3( float X, std::int16_t exponent,
		                                     std::int16_t old_exponent ) noexcept {
			auto const exp_diff = exponent - old_exponent;
			if( exp_diff > 0 ) {
				return fpow2( exp_diff ) * X;
			}
			return X / fpow2( -exp_diff );
		}
	} // namespace cxmath_impl

	template<int32_t exp>
	constexpr float fpow2_v = cxmath_impl::pow2_t<double>::get<float>( exp );

	template<int32_t exp>
	constexpr double dpow2_v = cxmath_impl::pow2_t<double>::get( exp );

	template<int32_t exp>
	constexpr float fpow10_v = cxmath_impl::fpow10_t<double>::get<float>( exp );

	template<int32_t exp>
	constexpr double dpow10_v = cxmath_impl::fpow10_t<double>::get( exp );

	template<size_t exp>
	constexpr uintmax_t pow10_v = cxmath_impl::pow10_t<uintmax_t>::get( exp );

	constexpr float fpow2( int32_t exp ) noexcept {
		return cxmath_impl::pow2_t<double>::get<float>( exp );
	}

	[[nodiscard]] constexpr double dpow2( int32_t exp ) noexcept {
		return cxmath_impl::pow2_t<double>::get( exp );
	}

	[[nodiscard]] constexpr float fpow10( int32_t exp ) noexcept {
		return cxmath_impl::fpow10_t<double>::get<float>( exp );
	}

	[[nodiscard]] constexpr double dpow10( int32_t exp ) noexcept {
		return cxmath_impl::fpow10_t<double>::get( exp );
	}

	[[nodiscard]] constexpr uintmax_t pow10( size_t exp ) noexcept {
		return cxmath_impl::pow10_t<uintmax_t>::get( exp );
	}

	[[nodiscard]] constexpr float fexp2( float X, std::int16_t exponent ) noexcept {
		auto const exp_diff = exponent - *fexp2( X );
		if( exp_diff > 0 ) {
			return fpow2( exp_diff ) * X;
		}
		return X / fpow2( -exp_diff );
	}

	[[nodiscard]] constexpr std::optional<std::int16_t>
	fexp2( float const f ) noexcept {
		// Once c++20 use bit_cast
		if( f == 0.0f ) {
			return static_cast<std::int16_t>( 0 );
		}
		if( f > std::numeric_limits<float>::max( ) ) {
			// inf
			return std::nullopt;
		}
		if( f < -std::numeric_limits<float>::max( ) ) {
			// -inf
			return std::nullopt;
		}
		if( f != f ) {
			// NaN
			return std::nullopt;
		}
		int32_t exponent = 254;
		float abs_f = f < 0 ? -f : f;

		while( abs_f < 0x1p87f ) {
			abs_f *= 0x1p41f;
			exponent -= 41;
		}

		auto const a = static_cast<std::uint64_t>( abs_f * 0x1p-64f );
		auto lz = static_cast<int32_t>( cxmath_impl::count_leading_zeroes( a ) );
		exponent -= lz;

		if( exponent >= 0 ) {
			return static_cast<std::int16_t>( exponent - 127 );
		}
		// return -127;
		return std::nullopt;
	}

	template<typename Integer,
	         daw::enable_when_t<std::is_integral_v<Integer>> = nullptr>
	[[nodiscard]] constexpr bool is_odd( Integer i ) noexcept {
		return ( static_cast<std::uint32_t>( i ) & 1U ) == 1U;
	}

	template<typename Integer,
	         daw::enable_when_t<std::is_integral_v<Integer>> = nullptr>
	[[nodiscard]] constexpr bool is_even( Integer i ) noexcept {
		return ( static_cast<std::uint32_t>( i ) & 1U ) == 0U;
	}

	template<typename Float,
	         daw::enable_when_t<std::is_floating_point_v<Float>> = nullptr>
	[[nodiscard]] constexpr Float abs( Float f ) noexcept {
		if( f < 0.0f ) {
			return -f;
		}
		return f;
	}

	[[nodiscard]] constexpr float sqrt( float const x ) noexcept {
		if( x < 0.0f ) {
			return std::numeric_limits<float>::quiet_NaN( );
		}
		// TODO: use bit_cast to get std::uint32_t of float, extract exponent,
		// set it to zero and bit_cast back to a float
		auto const exp = fexp2( x );
		if( !exp ) {
			return x;
		}
		auto const N = *exp;
		auto const f = cxmath_impl::fexp3( x, 0, N );

		auto const y0 = ( 0.41731f + ( 0.59016f * f ) );
		auto const z = ( y0 + ( f / y0 ) );
		auto const y2 = ( 0.25f * z ) + ( f / z );
		auto y = 0.5f * ( y2 + ( f / y2 ) );

		if( is_odd( N ) ) {
			y /= cxmath_impl::sqrt2<float>;
			return y * fpow2( ( N + 1 ) / 2 );
		}
		return y * fpow2( N / 2 );
	}

	template<typename Number, typename Number2,
	         daw::enable_when_t<std::is_arithmetic_v<Number>,
	                            std::is_arithmetic_v<Number2>> = nullptr>
	[[nodiscard]] constexpr Number copy_sign( Number x, Number2 s ) noexcept {
		if( s < 0 ) {
			if( x < 0 ) {
				return x;
			}
			return -x;
		}
		if( x < 0 ) {
			return -x;
		}
		return x;
	}

	template<typename Number,
	         daw::enable_when_t<std::is_signed_v<Number>> = nullptr>
	[[nodiscard]] constexpr bool signbit( Number n ) noexcept {
		return n < 0;
	}

	template<typename Number,
	         daw::enable_when_t<!std::is_signed_v<Number>> = nullptr>
	[[nodiscard]] constexpr bool signbit( Number n ) noexcept {
		return false;
	}
} // namespace daw::cxmath
