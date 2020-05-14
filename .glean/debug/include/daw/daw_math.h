// The MIT License (MIT)
//
// Copyright (c) 2014-2020 Darrell Wright
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
#ifdef max
#undef max
#endif // max

#include <cassert>
#include <cfenv>
#include <cmath>
#include <cstddef>
#include <limits>
#include <memory>
#include <utility>

#include "daw/cpp_17.h"
#include "daw/daw_algorithm.h"
#include "daw/daw_scope_guard.h"
#include "daw/daw_traits.h"
#include "daw/daw_utility.h"
#include "daw/impl/daw_math_impl.h"

namespace daw {
	namespace math {
		template<typename Float>
		struct int_for_float {
			using type = intmax_t;
		};

		template<>
		struct int_for_float<float> {
			using type = int32_t;
		};

		template<typename Float>
		using int_for_float_t = typename int_for_float<Float>::type;

		template<typename T>
		constexpr T const PI = T(
		  3.14159265358979323846264338327950288419716939937510582097494459230781640628620899 );

		template<typename Result = int_for_float_t<float>>
		constexpr Result round( float d ) noexcept {
			static_assert( std::is_integral_v<Result>,
			               "Result type must be integral" );
			static_assert( std::is_floating_point_v<float>,
			               "Argument must be a floating point type" );
			return static_cast<Result>( d + 0.5 );
		}

		template<typename Result = int_for_float_t<double>>
		constexpr Result round( double d ) noexcept {
			static_assert( std::is_integral_v<Result>,
			               "Result type must be integral" );
			static_assert( std::is_floating_point_v<double>,
			               "Argument must be a floating point type" );
			return static_cast<Result>( d + 0.5 );
		}

		template<typename Result = int_for_float_t<float>>
		constexpr Result floor( float d ) noexcept {
			static_assert( std::is_integral_v<Result>,
			               "Result type must be integral" );
			static_assert( std::is_floating_point_v<float>,
			               "Argument must be a floating point type" );
			return static_cast<Result>( d );
		}

		template<typename Result = int_for_float_t<double>>
		constexpr Result floor( double d ) noexcept {
			static_assert( std::is_integral_v<Result>,
			               "Result type must be integral" );
			static_assert( std::is_floating_point_v<double>,
			               "Argument must be a doubleing point type" );
			return static_cast<Result>( d );
		}

		template<typename Result = int_for_float_t<float>>
		constexpr Result ceil( float d ) noexcept {
			static_assert( std::is_integral_v<Result>,
			               "Result type must be integral" );
			static_assert( std::is_floating_point_v<float>,
			               "Argument must be a floating point type" );
			return static_cast<Result>( d + 1.0 );
		}

		template<typename Result = int_for_float_t<double>>
		constexpr Result ceil( double d ) noexcept {
			static_assert( std::is_integral_v<Result>,
			               "Result type must be integral" );
			static_assert( std::is_floating_point_v<double>,
			               "Argument must be a doubleing point type" );
			return static_cast<Result>( d + 1.0 );
		}

		template<typename T>
		constexpr T round_by( T const value, double const rnd_by ) noexcept {
			static_assert( std::is_arithmetic_v<T>,
			               "value must be an arithmetic type" );
			auto const rnd =
			  static_cast<double>( round( static_cast<double>( value ) / rnd_by ) );
			return static_cast<T>( rnd * rnd_by );
		}

		template<typename T>
		constexpr T floor_by( T const value, double const rnd_by ) noexcept {
			static_assert( std::is_arithmetic_v<T>,
			               "value must be an arithmetic type" );
			auto const rnd =
			  static_cast<double>( floor( static_cast<double>( value ) / rnd_by ) );
			return static_cast<T>( rnd * rnd_by );
		}

		template<typename T>
		constexpr T ceil_by( T const value, double const rnd_by ) noexcept {
			static_assert( std::is_arithmetic_v<T>,
			               "value must be an arithmetic type" );
			auto const rnd =
			  static_cast<double>( ceil( static_cast<double>( value ) / rnd_by ) );
			return static_cast<T>( rnd * rnd_by );
		}

		template<typename T, std::enable_if_t<not std::is_integral_v<T>,
		                                      std::nullptr_t> = nullptr>
		constexpr T abs( T t ) noexcept {
			return t < static_cast<T>( 0 ) ? -t : t;
		}

		template<typename T = uintmax_t>
		constexpr T pow2( T n ) noexcept {
			T result = 1;
			while( n-- > 0 ) {
				result *= 2ULL;
			}
			return result;
		}

		template<typename Result = uintmax_t, typename SignedInteger,
		         std::enable_if_t<all_true_v<std::is_integral_v<SignedInteger>,
		                                     std::is_signed_v<SignedInteger>>,
		                          std::nullptr_t> = nullptr>
		constexpr Result abs( SignedInteger v ) noexcept {
			// This accounts for when negating the number is out of range
			if( static_cast<intmax_t>( v ) ==
			    std::numeric_limits<intmax_t>::min( ) ) {
				return pow2<Result>( bsizeof<intmax_t> - 1 );
			}
			if( v < 0 ) {
				v = -v;
			}
			return static_cast<Result>( v );
		}

		template<typename UnsignedInteger,
		         std::enable_if_t<all_true_v<std::is_integral_v<UnsignedInteger>,
		                                     not std::is_signed_v<UnsignedInteger>>,
		                          std::nullptr_t> = nullptr>
		constexpr UnsignedInteger abs( UnsignedInteger &&v ) noexcept {
			return std::forward<UnsignedInteger>( v );
		}

		template<typename T>
		constexpr uintmax_t factorial( T t ) noexcept {
			uintmax_t count = abs( t );
			uintmax_t result = 1;
			for( ; count > 0; --count ) {
				result *= count;
			}
			return result;
		}

		namespace impl {
			template<typename T>
			constexpr T pow_impl( T base, size_t exponent ) noexcept {
				for( size_t k = 0; k < exponent; ++k ) {
					base *= base;
				}
				return base;
			}

		} // namespace impl

		template<typename T>
		constexpr T pow( T base, size_t exponent ) noexcept {
			return base == 0 ? 0
			                 : exponent > 0 ? impl::pow_impl( base, exponent ) : 1;
		}

		namespace impl {
			struct constants {
				constexpr static double const tol = 0.001;
			};

			template<typename T>
			constexpr auto cube( T x ) noexcept {
				return x * x * x;
			}

			// Based on the triple-angle formula: sin 3x = 3 sin x - 4 sin ^3 x
			template<typename R>
			constexpr R sin_helper( R x ) noexcept {
				return x < constants::tol
				         ? x
				         : static_cast<R>( 3.0 ) *
				               ( sin_helper( x / static_cast<R>( 3.0 ) ) ) -
				             static_cast<R>( 4.0 ) *
				               cube( sin_helper( x / static_cast<R>( 3.0 ) ) );
			}
		} // namespace impl

		template<typename R>
		constexpr R sin( R x ) noexcept {
			return impl::sin_helper( x < 0 ? PI<R> - x : x );
		}

		template<typename R>
		constexpr R cos( R x ) noexcept {
			if( 0.0 == x ) {
				return 1.0;
			}
			return sin( ( PI<R> / 2.0 ) - x );
		}

		template<typename T>
		constexpr auto sqr( T const &value ) noexcept {
			static_assert( std::is_arithmetic_v<T>,
			               "Template parameter must be an arithmetic type" );
			return value * value;
		}

		///
		/// <summary>Set the minimum value allowed</summary>
		///
		template<typename T, typename U>
		constexpr auto value_or_min( T const &value, U const &min_value ) noexcept {
			if( min_value > value ) {
				return min_value;
			}
			return value;
		}

		template<typename T, std::enable_if_t<not std::is_floating_point_v<T>,
		                                      std::nullptr_t> = nullptr>
		constexpr bool nearly_equal( T const &a, T const &b ) noexcept {
			return a == b;
		}

#ifdef __clang__
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wfloat-equal"
#endif
		template<typename T, std::enable_if_t<std::is_floating_point_v<T>,
		                                      std::nullptr_t> = nullptr>
		constexpr bool
		nearly_equal( T a, T b,
		              T epsilon = std::numeric_limits<T>::epsilon( ) ) noexcept {
			if( a != a ) {
				if( b != b ) {
					return true; // both NaN
				}
				return false;
			}
			if( b != b ) {
				return false;
			}
			// Code from http://floating-point-gui.de/errors/comparison/
			auto absA = daw::math::abs( a );
			auto absB = daw::math::abs( b );
			auto diff = daw::math::abs( a - b );
			if( diff != diff ) {
				// both numbers where either max or min
				return true;
			}

			if( a == b ) { // shortcut, handles infinities
				return true;
			}
			if( a == 0 or b == 0 or diff < std::numeric_limits<T>::min_exponent ) {
				// a or b is zero or both are extremely close to it
				// 			// relative error is less meaningful here
				return diff < ( epsilon * std::numeric_limits<T>::min_exponent );
			}
			// use relative error
			return diff /
			         daw::min( ( absA + absB ), std::numeric_limits<T>::max( ) ) <
			       epsilon;
		}
#ifdef __clang__
#pragma clang diagnostic pop
#endif
	} // namespace math
} // namespace daw
