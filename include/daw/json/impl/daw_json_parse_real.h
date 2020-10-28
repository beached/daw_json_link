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

#include <ciso646>
#include <cstddef>
#include <cstdint>
#include <limits>
#include <type_traits>

namespace daw::json::json_details {
	namespace {
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
	}

	template<typename Result, typename Unsigned>
	DAW_ATTRIBUTE_FLATTEN static inline constexpr Result
	power10( constexpr_exec_tag const &, Result result, Unsigned p ) {
		// We only have a double table, of which float is a subset.  Long double
		// will be calculated in terms of that
		constexpr int max_dbl_exp = std::numeric_limits<double>::max_exponent10;
		constexpr int max_exp =
		  std::is_same_v<Result, float>
		    ? std::min( max_dbl_exp, std::numeric_limits<float>::max_exponent10 )
		    : max_dbl_exp;
		constexpr Result max_v = static_cast<Result>( dpow10_tbl[max_exp] );

		if( DAW_JSON_UNLIKELY( p > max_exp ) ) {
			Result exp2 = max_v;
			p -= max_exp;
			while( p > max_exp ) {
				exp2 *= max_v;
				p -= max_exp;
			}
			return static_cast<Result>( result ) *
			       ( exp2 * static_cast<Result>(
			                  dpow10_tbl[static_cast<std::size_t>( p )] ) );
		} else if( DAW_JSON_UNLIKELY( p < -max_exp ) ) {
			Result exp2 = max_v;
			p += max_exp;
			while( p < -max_exp ) {
				result /= max_v;
				p += max_exp;
			}
			return ( static_cast<Result>( result ) /
			         static_cast<Result>(
			           dpow10_tbl[static_cast<std::size_t>( -p )] ) ) /
			       exp2;
		}
		if( p < 0 ) {
			return static_cast<Result>( result ) /
			       static_cast<Result>( dpow10_tbl[static_cast<std::size_t>( -p )] );
		}
		return static_cast<Result>( result ) *
		       static_cast<Result>( dpow10_tbl[static_cast<std::size_t>( p )] );
	}

	template<typename Result, typename Unsigned>
	DAW_ATTRIBUTE_FLATTEN static inline constexpr Result
	power10( runtime_exec_tag const &, Result result, Unsigned p ) {
		if constexpr( std::is_same_v<Result, double> or
		              std::is_same_v<Result, float> ) {
			return power10( constexpr_exec_tag{ }, result,
			                static_cast<std::int32_t>( p ) );
		} else {
			// For long double and others fallback to the slower std::pow
			using std::pow;
			return result * pow( static_cast<Result>( 10.0 ), p );
		}
	}

	template<typename Result>
	DAW_ATTRIBUTE_FLATTEN static inline constexpr Result
	copy_sign( constexpr_exec_tag const &, Result to, Result from ) {
		return daw::cxmath::copy_sign( to, from );
	}

	template<typename Result>
	DAW_ATTRIBUTE_FLATTEN static inline Result
	copy_sign( runtime_exec_tag const &, Result to, Result from ) {
		return std::copysign( to, from );
	}

	template<typename Value>
	DAW_ATTRIBUTE_FLATTEN static inline constexpr void
	parse_digits( char const *first, char const *const last, Value &v ) {
		Value value = v;
		while( DAW_JSON_LIKELY( first < last ) ) {
			value *= 10U;
			value += parse_digit( *first );
			++first;
		}
		v = value;
	}

	template<bool is_unchecked_input, typename Unsigned>
	DAW_ATTRIBUTE_FLATTEN static inline constexpr char const *
	parse_real_digits_while_number( char const *first, char const *const last,
	                                Unsigned &v ) {
		auto value = v;
		unsigned dig = 0;
		while( ( is_unchecked_input or DAW_JSON_LIKELY( first < last ) ) and
		       ( dig = parse_digit( *first ) ) < 10 ) {
			value *= 10U;
			value += dig;
			++first;
		}
		v = value;
		return first;
	}

	template<typename Result, bool KnownRange, typename Range,
	         std::enable_if_t<KnownRange, std::nullptr_t> = nullptr>
	[[nodiscard]] DAW_ATTRIBUTE_FLATTEN static inline constexpr Result
	parse_real( Range &rng ) {
		// [-]WHOLE[.FRACTION][(e|E)[+|-]EXPONENT]
		daw_json_assert_weak(
		  rng.has_more( ) and parse_policy_details::is_number_start( rng.front( ) ),
		  ErrorReason::InvalidNumberStart, rng );

		char const *whole_first = rng.first;
		char const *whole_last = rng.class_first ? rng.class_first : rng.class_last;
		char const *fract_first = rng.class_first ? rng.class_first + 1 : nullptr;
		char const *fract_last = rng.class_last;
		char const *exp_first = rng.class_last ? rng.class_last + 1 : nullptr;
		char const *const exp_last = rng.last;

		if( rng.class_first == nullptr ) {
			if( rng.class_last == nullptr ) {
				whole_last = rng.last;
			} else {
				whole_last = rng.class_last;
			}
		} else if( rng.class_last == nullptr ) {
			fract_last = rng.last;
		}

		bool const sign = [&] {
			if( *whole_first == '-' ) {
				++whole_first;
				return true;
			}
			return false;
		}( );
		constexpr auto max_storage_digits = static_cast<std::ptrdiff_t>(
		  daw::numeric_limits<std::uint64_t>::digits10 );
		constexpr auto max_exponent = static_cast<std::ptrdiff_t>(
		  daw::numeric_limits<Result>::max_digits10 + 1 );
		using unsigned_t = std::conditional_t<max_storage_digits >= max_exponent,
		                                      std::uint64_t, Result>;

		std::ptrdiff_t whole_exponent_available = whole_last - whole_first;
		std::ptrdiff_t fract_exponent_available =
		  fract_first ? fract_last - fract_first : 0;
		std::ptrdiff_t exponent = 0;
		if( whole_exponent_available > max_exponent ) {
			whole_last = whole_first + max_exponent;
			whole_exponent_available -= max_exponent;
			fract_exponent_available = 0;
			fract_first = nullptr;
			exponent = whole_exponent_available;
		} else {
			whole_exponent_available = max_exponent - whole_exponent_available;
			fract_exponent_available =
			  std::min( fract_exponent_available, whole_exponent_available );
			exponent = -fract_exponent_available;
			fract_last = fract_first + fract_exponent_available;
		}

		unsigned_t significant_digits = 0;
		parse_digits( whole_first, whole_last, significant_digits );
		if( fract_first ) {
			parse_digits( fract_first, fract_last, significant_digits );
		}

		if( exp_first and ( exp_last - exp_first ) > 0 ) {
			int const exp_sign = [&] {
				switch( *exp_first ) {
				case '-':
					++exp_first;
					return -1;
				case '+':
					++exp_first;
					return 1;
				default:
					return 1;
				}
			}( );
			exponent += exp_sign * [&] {
				std::ptrdiff_t r = 0;
				while( exp_first < exp_last ) {
					r *= static_cast<std::ptrdiff_t>( 10 );
					r += static_cast<std::ptrdiff_t>( parse_digit( *exp_first ) );
					++exp_first;
				}
				return r;
			}( );
		}
		if( sign ) {
			return -power10<Result>(
			  Range::exec_tag, static_cast<Result>( significant_digits ), exponent );
		}
		return power10<Result>(
		  Range::exec_tag, static_cast<Result>( significant_digits ), exponent );
	}

	template<typename Result, bool KnownRange, typename Range,
	         std::enable_if_t<not KnownRange, std::nullptr_t> = nullptr>
	[[nodiscard]] DAW_ATTRIBUTE_FLATTEN inline constexpr Result
	parse_real( Range &rng ) {
		// [-]WHOLE[.FRACTION][(e|E)[+|-]EXPONENT]
		daw_json_assert_weak(
		  rng.has_more( ) and parse_policy_details::is_number_start( rng.front( ) ),
		  ErrorReason::InvalidNumberStart, rng );

		Result const sign = [&] {
			if( rng.front( ) == '-' ) {
				rng.remove_prefix( );
				return static_cast<Result>( -1 );
			}
			return static_cast<Result>( 1 );
		}( );
		constexpr auto max_storage_digits = static_cast<std::ptrdiff_t>(
		  daw::numeric_limits<std::uint64_t>::digits10 );
		constexpr auto max_exponent = static_cast<std::ptrdiff_t>(
		  daw::numeric_limits<Result>::max_digits10 + 1 );
		using unsigned_t = std::conditional_t<max_storage_digits >= max_exponent,
		                                      std::uint64_t, Result>;
		using signed_t =
		  std::conditional_t<max_storage_digits >= max_exponent, int, Result>;

		char const *first = rng.first;
		char const *const whole_last =
		  rng.first + std::min( rng.last - rng.first, max_exponent );

		unsigned_t significant_digits = 0;
		char const *last_char =
		  parse_real_digits_while_number<Range::is_unchecked_input>(
		    first, whole_last, significant_digits );

		signed_t exponent = [&] {
			if( last_char >= whole_last ) {
				// We have sig digits we cannot parse because there isn't enough room in
				// a std::uint64_t
				char const *ptr =
				  skip_digits<Range::is_unchecked_input>( last_char, rng.last );
				auto const diff = ptr - last_char;
				last_char = ptr;
				return static_cast<signed_t>( diff );
			}
			return static_cast<signed_t>( 0 );
		}( );
		if( significant_digits == 0 ) {
			exponent = 0;
		}
		first = last_char;
		if( ( ( Range::is_unchecked_input or
		        DAW_JSON_LIKELY( first < rng.last ) ) and
		      *first == '.' ) ) {
			++first;
			if( exponent != 0 ) {
				first = skip_digits<Range::is_unchecked_input>( first, rng.last );
			} else {
				char const *fract_last =
				  first +
				  std::min( rng.last - first, max_exponent - ( first - rng.first ) );

				last_char = parse_real_digits_while_number<Range::is_unchecked_input>(
				  first, fract_last, significant_digits );
				exponent -= last_char - first;
				first = last_char;
				if( first >= fract_last ) {
					first = skip_digits<Range::is_unchecked_input>( first, rng.last );
				}
			}
		}

		exponent += [&] {
			if( ( Range::is_unchecked_input or first < rng.last ) and
			    ( ( *first | 0x20 ) == 'e' ) ) {
				++first;
				bool const exp_sign = [&] {
					daw_json_assert_weak(
					  first < rng.last, ErrorReason::UnexpectedEndOfData,
					  Range( first, rng.last, rng.class_first, rng.class_last ) );
					switch( *first ) {
					case '+':
						++first;
						return false;
					case '-':
						++first;
						return true;
					default:
						return false;
					}
				}( );
				daw_json_assert_weak( rng.has_more( ), ErrorReason::UnexpectedEndOfData,
				                      rng );
				unsigned_t exp_tmp = 0;
				last_char = parse_real_digits_while_number<Range::is_unchecked_input>(
				  first, rng.last, exp_tmp );
				first = last_char;
				if( exp_sign ) {
					return -static_cast<unsigned_t>( exp_tmp );
				}
				return static_cast<unsigned_t>( exp_tmp );
			}
			return static_cast<unsigned_t>( 0 );
		}( );
		rng.first = first;
		return sign * power10<Result>( Range::exec_tag,
		                               static_cast<Result>( significant_digits ),
		                               exponent );
	}
} // namespace daw::json::json_details
