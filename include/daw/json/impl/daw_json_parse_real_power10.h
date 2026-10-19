// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/daw_json_link
//

#pragma once

#include "daw/json/impl/version.h"

#include "daw/json/impl/daw_json_exec_modes.h"
#include "daw/json/impl/daw_json_parse_real_eisellemire.h"

#include <daw/daw_arith_traits.h>
#include <daw/daw_cpp_feature_check.h>
#include <daw/daw_is_constant_evaluated.h>
#include <daw/daw_likely.h>

#include <cmath>
#include <cstddef>
#include <cstdint>
#include <limits>
#if defined( DAW_JSON_HAS_FLOAT128 ) and defined( __STDCPP_FLOAT128_T__ )
#include <stdfloat>
#endif
#include <type_traits>

namespace daw::json {
	inline namespace DAW_JSON_VER {
		namespace json_details {
			inline constexpr double dpow10_tbl[] = {
			  1e0,   1e1,   1e2,   1e3,   1e4,   1e5,   1e6,   1e7,   1e8,   1e9,
			  1e10,  1e11,  1e12,  1e13,  1e14,  1e15,  1e16,  1e17,  1e18,  1e19,
			  1e20,  1e21,  1e22,  1e23,  1e24,  1e25,  1e26,  1e27,  1e28,  1e29,
			  1e30,  1e31,  1e32,  1e33,  1e34,  1e35,  1e36,  1e37,  1e38,  1e39,
			  1e40,  1e41,  1e42,  1e43,  1e44,  1e45,  1e46,  1e47,  1e48,  1e49,
			  1e50,  1e51,  1e52,  1e53,  1e54,  1e55,  1e56,  1e57,  1e58,  1e59,
			  1e60,  1e61,  1e62,  1e63,  1e64,  1e65,  1e66,  1e67,  1e68,  1e69,
			  1e70,  1e71,  1e72,  1e73,  1e74,  1e75,  1e76,  1e77,  1e78,  1e79,
			  1e80,  1e81,  1e82,  1e83,  1e84,  1e85,  1e86,  1e87,  1e88,  1e89,
			  1e90,  1e91,  1e92,  1e93,  1e94,  1e95,  1e96,  1e97,  1e98,  1e99,
			  1e100, 1e101, 1e102, 1e103, 1e104, 1e105, 1e106, 1e107, 1e108, 1e109,
			  1e110, 1e111, 1e112, 1e113, 1e114, 1e115, 1e116, 1e117, 1e118, 1e119,
			  1e120, 1e121, 1e122, 1e123, 1e124, 1e125, 1e126, 1e127, 1e128, 1e129,
			  1e130, 1e131, 1e132, 1e133, 1e134, 1e135, 1e136, 1e137, 1e138, 1e139,
			  1e140, 1e141, 1e142, 1e143, 1e144, 1e145, 1e146, 1e147, 1e148, 1e149,
			  1e150, 1e151, 1e152, 1e153, 1e154, 1e155, 1e156, 1e157, 1e158, 1e159,
			  1e160, 1e161, 1e162, 1e163, 1e164, 1e165, 1e166, 1e167, 1e168, 1e169,
			  1e170, 1e171, 1e172, 1e173, 1e174, 1e175, 1e176, 1e177, 1e178, 1e179,
			  1e180, 1e181, 1e182, 1e183, 1e184, 1e185, 1e186, 1e187, 1e188, 1e189,
			  1e190, 1e191, 1e192, 1e193, 1e194, 1e195, 1e196, 1e197, 1e198, 1e199,
			  1e200, 1e201, 1e202, 1e203, 1e204, 1e205, 1e206, 1e207, 1e208, 1e209,
			  1e210, 1e211, 1e212, 1e213, 1e214, 1e215, 1e216, 1e217, 1e218, 1e219,
			  1e220, 1e221, 1e222, 1e223, 1e224, 1e225, 1e226, 1e227, 1e228, 1e229,
			  1e230, 1e231, 1e232, 1e233, 1e234, 1e235, 1e236, 1e237, 1e238, 1e239,
			  1e240, 1e241, 1e242, 1e243, 1e244, 1e245, 1e246, 1e247, 1e248, 1e249,
			  1e250, 1e251, 1e252, 1e253, 1e254, 1e255, 1e256, 1e257, 1e258, 1e259,
			  1e260, 1e261, 1e262, 1e263, 1e264, 1e265, 1e266, 1e267, 1e268, 1e269,
			  1e270, 1e271, 1e272, 1e273, 1e274, 1e275, 1e276, 1e277, 1e278, 1e279,
			  1e280, 1e281, 1e282, 1e283, 1e284, 1e285, 1e286, 1e287, 1e288, 1e289,
			  1e290, 1e291, 1e292, 1e293, 1e294, 1e295, 1e296, 1e297, 1e298, 1e299,
			  1e300, 1e301, 1e302, 1e303, 1e304, 1e305, 1e306, 1e307, 1e308 };

			// 10^0 .. 10^27 are exactly representable in 80-bit long double
			// (5^27 < 2^64), and 10^0 .. 10^48 in binary128 (5^48 < 2^113).
			// Entries above 22 must not be sourced from the double table.
			inline constexpr long double ldpow10_tbl[] = {
			  1e0L,  1e1L,  1e2L,  1e3L,  1e4L,  1e5L,  1e6L,
			  1e7L,  1e8L,  1e9L,  1e10L, 1e11L, 1e12L, 1e13L,
			  1e14L, 1e15L, 1e16L, 1e17L, 1e18L, 1e19L, 1e20L,
			  1e21L, 1e22L, 1e23L, 1e24L, 1e25L, 1e26L, 1e27L,
			  1e28L, 1e29L, 1e30L, 1e31L, 1e32L, 1e33L, 1e34L,
			  1e35L, 1e36L, 1e37L, 1e38L, 1e39L, 1e40L, 1e41L,
			  1e42L, 1e43L, 1e44L, 1e45L, 1e46L, 1e47L, 1e48L,
			};
			inline constexpr int max_ld_table_exp = 48;

			inline constexpr int max_dbl_exp =
			  std::numeric_limits<double>::max_exponent10;

			template<typename T>
			[[nodiscard]] DAW_ATTRIB_INLINE constexpr T const &
			Min( T const &l, T const &r ) noexcept {
				if( l <= r ) {
					return l;
				}
				return r;
			}

			template<typename Result>
			inline constexpr int max_exp =
			  std::is_same_v<Result, float>
			    ? Min( max_dbl_exp, std::numeric_limits<float>::max_exponent10 )
			    : max_dbl_exp;

			template<typename Result, typename Unsigned>
			[[nodiscard]] DAW_ATTRIB_FLATINLINE constexpr Result
			power10_constexpr( Result result, Unsigned p ) {
				// For extended long double use the exact LD table where available;
				// the double table loses precision for p above 22.
				if constexpr( is_double_sized_long_double_v<Result> == false and
				              std::is_same_v<Result, long double> ) {
					DAW_CPP23_STATIC_LOCAL constexpr int max_exact_exp =
					  std::numeric_limits<long double>::digits == 113
					    ? max_ld_table_exp
					    : 27;
					if( p >= -max_exact_exp and p <= max_exact_exp ) {
						if( p < 0 ) {
							return result /
							       ldpow10_tbl[static_cast<std::size_t>( -p )];
						}
						return result * ldpow10_tbl[static_cast<std::size_t>( p )];
					}
				}

				DAW_CPP23_STATIC_LOCAL constexpr auto max_v =
				  static_cast<Result>( dpow10_tbl[max_exp<Result>] );

				if( DAW_UNLIKELY( p > max_exp<Result> ) ) {
					if constexpr( std::is_floating_point_v<Result> ) {
						if( DAW_UNLIKELY( p >
						                  daw::numeric_limits<Result>::max_exponent ) ) {
							return std::numeric_limits<Result>::infinity( );
						}
					}
					Result exp2 = max_v;
					p -= max_exp<Result>;

					for( ; p > max_exp<Result>; p -= max_exp<Result> ) {
						exp2 *= max_v;
					}
					return static_cast<Result>( result ) *
					       ( exp2 * static_cast<Result>(
					                  dpow10_tbl[static_cast<std::size_t>( p )] ) );
				}
				if( DAW_UNLIKELY( p < -max_exp<Result> ) ) {
					if constexpr( std::is_floating_point_v<Result> ) {
						if( DAW_UNLIKELY( p <
						                  daw::numeric_limits<Result>::min_exponent ) ) {
							return Result{ 0 };
						}
					}
					Result exp2 = max_v;
					p += max_exp<Result>;

					for( ; p < -max_exp<Result>; p += max_exp<Result> ) {
						result /= max_v;
					}
					return ( static_cast<Result>( result ) /
					         static_cast<Result>(
					           dpow10_tbl[static_cast<std::size_t>( -p )] ) ) /
					       exp2;
				}
				if( p < 0 ) {
					return static_cast<Result>( result ) /
					       static_cast<Result>(
					         dpow10_tbl[static_cast<std::size_t>( -p )] );
				}
				return static_cast<Result>( result ) *
				       static_cast<Result>( dpow10_tbl[static_cast<std::size_t>( p )] );
			}

			namespace power10_details {
				/// Largest power of ten that is exact as a double, 5^22 < 2^53
				inline constexpr std::uint32_t max_exact_double_power10 = 22;

				/// 10^n computed in Result.  Only the first 23 entries of the double
				/// table are exact, so larger powers are 10^(n % 22) * (10^22)^(n / 22)
				/// with the second part by repeated squaring.  Every intermediate
				/// value is a power of ten no larger than 10^n, so the result is exact
				/// whenever 10^n is representable in Result, e.g. up to 10^48 for
				/// binary128 and always for decimal types.  Otherwise each multiply
				/// adds at most half an ulp of Result.
				template<typename Result>
				[[nodiscard]] constexpr Result power10_value( std::uint32_t n ) {
					auto power =
					  static_cast<Result>( dpow10_tbl[n % max_exact_double_power10] );
					auto q = n / max_exact_double_power10;
					if( q != 0 ) {
						auto base =
						  static_cast<Result>( dpow10_tbl[max_exact_double_power10] );
						while( true ) {
							if( ( q & 1U ) != 0 ) {
								power = power * base;
							}
							q >>= 1U;
							if( q == 0 ) {
								break;
							}
							base = base * base;
						}
					}
					return power;
				}
			} // namespace power10_details

			/// @brief result * 10^p computed in Result, for when the double table
			/// cannot do it precisely: user defined types with more precision than
			/// double, and constant evaluation of extended floating point types where
			/// from_chars is not available.  It needs no allocation, no non-constexpr
			/// pow, and no table beyond the exact part of the double table.  Negative
			/// exponents divide by 10^-p, which can be exact, rather than multiplying
			/// by 10^p, which never is in binary.  result must not be negative; the
			/// parsers apply the sign afterwards.
			template<typename Result>
			[[nodiscard]] constexpr Result power10_by_squaring( Result result,
			                                                    std::int64_t p ) {
				using limits = std::numeric_limits<Result>;
				// Apply at most 10^chunk at a time so the power itself cannot overflow
				// when result * 10^p is still representable
				DAW_CPP23_STATIC_LOCAL constexpr std::uint32_t chunk =
				  limits::max_exponent10 > 0
				    ? static_cast<std::uint32_t>( limits::max_exponent10 )
				    : power10_details::max_exact_double_power10;
				// After this many chunks any finite result is already 0 or infinity
				DAW_CPP23_STATIC_LOCAL constexpr std::uint64_t max_chunks =
				  static_cast<std::uint64_t>(
				    ( limits::max_exponent10 - limits::min_exponent10 ) > 0
				      ? ( limits::max_exponent10 - limits::min_exponent10 )
				      : 0 ) /
				    chunk +
				  2U;

				bool const negative = p < 0;
				auto n = negative ? std::uint64_t{ 0 } - static_cast<std::uint64_t>( p )
				                  : static_cast<std::uint64_t>( p );
				// Overflowing to infinity is not allowed in a constant expression, so
				// check before multiplying and return the overflow value directly
				bool overflowed = false;
				auto const scale = [&]( Result value, Result const &power ) {
					if( negative ) {
						return value / power;
					}
					if( DAW_UNLIKELY( value > limits::max( ) / power ) ) {
						overflowed = true;
						if constexpr( limits::has_infinity ) {
							return limits::infinity( );
						} else {
							return limits::max( );
						}
					}
					return value * power;
				};
				if( DAW_UNLIKELY( n > chunk ) ) {
					auto const chunk_power =
					  power10_details::power10_value<Result>( chunk );
					std::uint64_t chunks = 0;
					while( n > chunk and chunks < max_chunks ) {
						result = scale( result, chunk_power );
						if( overflowed ) {
							return result;
						}
						n -= chunk;
						++chunks;
					}
					if( n > chunk ) {
						n = chunk;
					}
				}
				return scale( result,
				              power10_details::power10_value<Result>(
				                static_cast<std::uint32_t>( n ) ) );
			}

#if defined( DAW_JSON_HAS_FLOAT128 )
				namespace power10_details {
					inline constexpr int float128_max_exponent = 16384;
					inline constexpr int float128_min_exponent = -16381;
#if defined( __STDCPP_FLOAT128_T__ )
					using float128_t = std::float128_t;
#else
				using float128_t = __float128;
#endif
				inline constexpr int float128_digits = 113;

				/// (sig_hi * 2^64 + sig_lo) * 2^exp2, exactly.  This lets the table be
				/// written without a literal suffix, as std::float128_t and __float128
				/// do not share one
				[[nodiscard]] constexpr float128_t make_float128( bool negative,
				                                                  std::uint64_t sig_hi,
				                                                  std::uint64_t sig_lo,
				                                                  int exp2 ) {
					auto value = static_cast<float128_t>( sig_hi ) *
					               static_cast<float128_t>( 18446744073709551616.0 ) +
					             static_cast<float128_t>( sig_lo );
					auto base = static_cast<float128_t>( exp2 < 0 ? 0.5 : 2.0 );
					auto n = static_cast<unsigned>( exp2 < 0 ? -exp2 : exp2 );
					while( n != 0 ) {
						if( ( n & 1U ) != 0 ) {
							value = value * base;
						}
						n >>= 1U;
						if( n != 0 ) {
							base = base * base;
						}
					}
					return negative ? -value : value;
				}

				/// A float128 value plus its rounding error, about 226 bits
				struct float128_pair {
					float128_t hi;
					float128_t lo;
				};

				/// The exact error of the rounded product p = a * b.  A hardware fma
				/// when there is one, otherwise Dekker's product
				[[nodiscard]] constexpr float128_t
				product_error( float128_t a, float128_t b,
				               [[maybe_unused]] float128_t p ) {
#if defined( __FP_FAST_FMAF128 )
					// Only with a hardware fma.  Without one, e.g. x86-64, fmaf128 is
					// done in software and is slower than the product below
					if( not DAW_IS_CONSTANT_EVALUATED_COMPAT( ) ) {
#if defined( __STDCPP_FLOAT128_T__ )
						return std::fma( a, b, -p );
#elif DAW_HAS_BUILTIN( __builtin_fmaf128 )
						return __builtin_fmaf128( a, b, -p );
#endif
					}
#endif
					// Veltkamp split, 2^57 + 1 for a 113 bit significand.  Large
					// operands are scaled down first so the split cannot overflow;
					// scaling by a power of two is exact
					DAW_CPP23_STATIC_LOCAL constexpr auto splitter =
					  static_cast<float128_t>( std::uint64_t{ 144115188075855873ULL } );
					DAW_CPP23_STATIC_LOCAL constexpr float128_t big =
					  make_float128( false, 0, 1, 16000 );
					DAW_CPP23_STATIC_LOCAL constexpr float128_t down =
					  make_float128( false, 0, 1, -80 );
					DAW_CPP23_STATIC_LOCAL constexpr float128_t up =
					  make_float128( false, 0, 1, 80 );
					auto scale = static_cast<float128_t>( 1 );
					if( a > big or a < -big ) {
						a = a * down;
						scale = scale * up;
					}
					if( b > big or b < -big ) {
						b = b * down;
						scale = scale * up;
					}
					auto const split = []( float128_t x ) {
						auto const c = splitter * x;
						auto const hi = c - ( c - x );
						return float128_pair{ hi, x - hi };
					};
					auto const sa = split( a );
					auto const sb = split( b );
					auto const scaled_p = a * b;
					return ( ( ( ( sa.hi * sb.hi - scaled_p ) + sa.hi * sb.lo ) +
					           sa.lo * sb.hi ) +
					         sa.lo * sb.lo ) *
					       scale;
				}

				[[nodiscard]] constexpr float128_pair multiply( float128_pair a,
				                                                float128_pair b ) {
					auto const p = a.hi * b.hi;
					auto const e =
					  product_error( a.hi, b.hi, p ) + ( a.hi * b.lo + a.lo * b.hi );
					auto const s = p + e;
					return { s, e - ( s - p ) };
				}

				[[nodiscard]] constexpr float128_pair divide( float128_pair a,
				                                              float128_pair b ) {
					auto const q1 = a.hi / b.hi;
					auto const p = q1 * b.hi;
					auto const r =
					  ( ( a.hi - p ) - product_error( q1, b.hi, p ) + a.lo ) - q1 * b.lo;
					auto const q2 = r / b.hi;
					auto const s = q1 + q2;
					return { s, q2 - ( s - q1 ) };
				}

				/// 10^(16 * 2^k) = hi + lo for k = 0..8, from exact integer arithmetic.
				/// 10^16 and 10^32 are exact in binary128
				inline constexpr float128_t power10_hi[] = {
				  static_cast<float128_t>( 1e16 ),
				  static_cast<float128_t>( 1e16 ) * static_cast<float128_t>( 1e16 ),
				  make_float128( false, 0x184f03e93ff9f, 0x4daa797ed6e38ed6, 100 ),
				  make_float128( false, 0x127748f9301d3, 0x19bf8cde66d86d62, 313 ),
				  make_float128( false, 0x154fdd7f73bf3, 0xbd1bbb77203731fe, 738 ),
				  make_float128( false, 0x1c633415d4c1d, 0x238d98cab8a978a1, 1588 ),
				  make_float128( false, 0x192eceb0d02ea, 0x182eca1a7a51e317, 3289 ),
				  make_float128( false, 0x13d1676bb8a7a, 0xbbc94e9a519c6536, 6691 ),
				  make_float128( false, 0x188c0a4051441, 0x2f3592982a7f0095, 13494 ) };
				inline constexpr float128_t power10_lo[] = {
				  static_cast<float128_t>( 0 ),
				  static_cast<float128_t>( 0 ),
				  make_float128( false, 0x12fda87c04000, 0x0000000000000000, -14 ),
				  make_float128( false, 0x1370234f3fd7b, 0x08dd390bc3c54e3f, 194 ),
				  make_float128( true, 0x15487ae1b340c, 0x21d06755187f3801, 623 ),
				  make_float128( true, 0x1383b1cd3e33f, 0x17659a2dee9638ae, 1474 ),
				  make_float128( true, 0x1798d6ca33bec, 0x56db2781485a496c, 3175 ),
				  make_float128( true, 0x18d6cae734220, 0xa0e7e584475598ea, 6577 ),
				  make_float128( true, 0x1ab6cd44f026d, 0xdd8eecc1151841b6, 13380 ) };

				/// 10^n as hi + lo, for n < 8192
				[[nodiscard]] constexpr float128_pair power10_pair( std::uint32_t n ) {
					auto power =
					  float128_pair{ static_cast<float128_t>( dpow10_tbl[n & 15U] ),
					                 static_cast<float128_t>( 0 ) };
					auto bits = n >> 4U;
					for( std::size_t k = 0; bits != 0; ++k, bits >>= 1U ) {
						if( ( bits & 1U ) != 0 ) {
							power = multiply( power, { power10_hi[k], power10_lo[k] } );
						}
					}
					return power;
				}

				/// value * 10^p with a single rounding at the end.  value must not
				/// be negative.  Returns false when the result overflows, as that is
				/// not allowed in a constant expression
				[[nodiscard]] constexpr bool scale_float128( float128_t &value,
				                                             std::int64_t p ) {
					// 10^4932 is the largest power below the binary128 maximum
					DAW_CPP23_STATIC_LOCAL constexpr std::uint32_t chunk = 4932;
					DAW_CPP23_STATIC_LOCAL constexpr float128_t max_value = make_float128(
					  false, 0x1ffffffffffff, 0xffffffffffffffff, 16383 - 112 );
					bool const negative = p < 0;
					auto n = negative
					           ? std::uint64_t{ 0 } - static_cast<std::uint64_t>( p )
					           : static_cast<std::uint64_t>( p );
					// 10^n is exact in binary128 up to 10^48, but the double table is
					// only exact up to 10^22, so 10^22 * 10^22 bounds this.  A single
					// multiply or divide by an exact power is correctly rounded
					if( n <= 44 ) {
						auto const power =
						  n <= 22 ? static_cast<float128_t>( dpow10_tbl[n] )
						          : static_cast<float128_t>( dpow10_tbl[22] ) *
						              static_cast<float128_t>( dpow10_tbl[n - 22] );
						if( negative ) {
							value = value / power;
							return true;
						}
						if( value > max_value / power ) {
							return false;
						}
						value = value * power;
						return true;
					}
					auto v = float128_pair{ value, static_cast<float128_t>( 0 ) };
					auto const apply = [&]( float128_pair const &power ) {
						if( negative ) {
							v = divide( v, power );
							return true;
						}
						if( v.hi > max_value / power.hi ) {
							return false;
						}
						v = multiply( v, power );
						return true;
					};
					// Three chunks take any finite value past the binary128 range
					for( int chunks = 0; n > chunk and chunks < 3; ++chunks ) {
						if( not apply( power10_pair( chunk ) ) ) {
							return false;
						}
						n -= chunk;
					}
					if( n > chunk ) {
						n = chunk;
					}
					if( not apply( power10_pair( static_cast<std::uint32_t>( n ) ) ) ) {
						return false;
					}
					value = v.hi + v.lo;
					return true;
				}

#if defined( __STDCPP_FLOAT128_T__ ) and defined( __SIZEOF_FLOAT128__ )
				/// std::float128_t and __float128 are both binary128 but distinct
				/// types, and an explicit conversion to one will not convert on to the
				/// other, so a Result may convert with either
				using other_float128_t = __float128;
#else
				using other_float128_t = float128_t;
#endif

				template<typename Result>
				inline constexpr bool converts_from_float128_v =
				  std::is_constructible_v<Result, float128_t> or
				  std::is_constructible_v<Result, other_float128_t>;

				template<typename Result>
				inline constexpr bool converts_to_float128_v =
				  std::is_constructible_v<float128_t, Result> or
				  std::is_constructible_v<other_float128_t, Result>;

				/// Result is scaled in binary128 when it is a binary type wider than
				/// double but no wider than binary128 in precision or exponent range,
				/// and converts to and from binary128
				template<typename Result>
				inline constexpr bool uses_float128_v =
				  ( std::numeric_limits<Result>::radix == 2 ) and
				  ( std::numeric_limits<Result>::digits >
				    std::numeric_limits<double>::digits ) and
				  ( std::numeric_limits<Result>::digits <= float128_digits ) and
				  ( std::numeric_limits<Result>::max_exponent <=
				    float128_max_exponent ) and
				  ( std::numeric_limits<Result>::min_exponent >=
				    float128_min_exponent ) and
				  converts_from_float128_v<Result> and converts_to_float128_v<Result>;

				template<typename Result>
				[[nodiscard]] constexpr float128_t to_float128( Result const &value ) {
					if constexpr( std::is_constructible_v<float128_t, Result> ) {
						return static_cast<float128_t>( value );
					} else {
						return static_cast<float128_t>(
						  static_cast<other_float128_t>( value ) );
					}
				}

				template<typename Result>
				[[nodiscard]] constexpr Result from_float128( float128_t value ) {
					if constexpr( std::is_constructible_v<Result, float128_t> ) {
						return static_cast<Result>( value );
					} else {
						return static_cast<Result>(
						  static_cast<other_float128_t>( value ) );
					}
				}
			} // namespace power10_details
#endif

			/// @brief result * 10^p for types wider than double, which the double
			/// table cannot scale precisely.  Uses the binary128 hi + lo table when
			/// available and Result converts to and from binary128, otherwise
			/// power10_by_squaring.  result must not be negative.
			template<typename Result>
			[[nodiscard]] constexpr Result power10_wide( Result result,
			                                             std::int64_t p ) {
#if defined( DAW_JSON_HAS_FLOAT128 )
				if constexpr( power10_details::uses_float128_v<Result> ) {
					auto value = power10_details::to_float128( result );
					if( DAW_LIKELY( power10_details::scale_float128( value, p ) ) ) {
						return power10_details::from_float128<Result>( value );
					}
					using limits = std::numeric_limits<Result>;
					if constexpr( limits::has_infinity ) {
						return limits::infinity( );
					} else {
						return limits::max( );
					}
				} else {
					return power10_by_squaring( result, p );
				}
#else
				return power10_by_squaring( result, p );
#endif
			}

			template<typename Result, typename Unsigned>
			[[nodiscard]] DAW_ATTRIB_FLATINLINE constexpr Result
			power10_runtime( Result result, Unsigned p ) {
				if constexpr( std::is_same_v<Result, double> or
				              std::is_same_v<Result, float> ) {
					return power10_constexpr( result, static_cast<std::int32_t>( p ) );
				} else if constexpr( is_double_sized_long_double_v<Result> ) {
					// long double here has no more precision than double, so the
					// double-precision table used by power10_constexpr loses nothing.
					return static_cast<Result>( power10_constexpr(
					  static_cast<double>( result ), static_cast<std::int32_t>( p ) ) );
				} else {
					// Extended long double: power10_constexpr uses the exact LD table
					// for all exponents the precise parser routes here.
					return power10_constexpr( result, static_cast<std::int32_t>( p ) );
				}
			}

			template<typename Result, typename ExecTag, typename Unsigned>
			[[nodiscard]] DAW_ATTRIB_FLATINLINE constexpr Result
			power10( ExecTag, Result result, Unsigned p ) {
				if( use_constexpr_exec_mode<ExecTag>( ) ) {
					return power10_constexpr<Result>( result, p );
				}
				return power10_runtime<Result>( result, p );
			}
		} // namespace json_details
	} // namespace DAW_JSON_VER
} // namespace daw::json
