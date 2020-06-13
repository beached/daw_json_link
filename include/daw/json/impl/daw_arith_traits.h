// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/daw_json_link
//
#pragma once

/***
 * Arithmetic Traits
 * This allows us to use the numeric_limits traits to build up the number traits
 * from traits.  If a user supplied type can do what an arithmetic type can, it
 * should work
 */

#include <limits>
#include <type_traits>

namespace daw {
	template<typename T>
	using is_integral = std::bool_constant<std::numeric_limits<T>::is_integer>;

	template<typename T>
	inline constexpr bool is_integral_v = is_integral<T>::value;

	static_assert( is_integral_v<int> );
	static_assert( not is_integral_v<float> );

	namespace arith_traits_details {
		template<typename T>
		using limits_is_signed =
		  std::bool_constant<std::numeric_limits<T>::is_signed>;

		template<typename T>
		using limits_is_exact =
		  std::bool_constant<std::numeric_limits<T>::is_exact>;
	} // namespace arith_traits_details

	template<typename T>
	using is_floating_point =
	  std::conjunction<std::negation<is_integral<T>>,
	                   arith_traits_details::limits_is_signed<T>,
	                   std::negation<arith_traits_details::limits_is_exact<T>>>;

	template<typename T>
	inline constexpr bool is_floating_point_v = is_floating_point<T>::value;

	static_assert( is_floating_point_v<float> );
	static_assert( not is_floating_point_v<int> );

	template<typename T>
	using is_number = std::disjunction<is_integral<T>, is_floating_point<T>>;

	template<typename T>
	inline constexpr bool is_number_v = is_number<T>::value;

	static_assert( is_number_v<float> );
	static_assert( is_number_v<int> );
	static_assert( not is_number_v<is_integral<int>> );

	template<typename T>
	using is_signed =
	  std::conjunction<is_number<T>, arith_traits_details::limits_is_signed<T>>;

	template<typename T>
	inline constexpr bool is_signed_v = is_signed<T>::value;

	static_assert( is_signed_v<int> );
	static_assert( is_signed_v<float> );
	static_assert( not is_signed_v<unsigned> );

	template<typename T>
	using is_unsigned =
	  std::conjunction<is_integral<T>,
	                   std::negation<arith_traits_details::limits_is_signed<T>>>;

	template<typename T>
	inline constexpr bool is_unsigned_v = is_unsigned<T>::value;

	static_assert( not is_unsigned_v<int> );
	static_assert( not is_unsigned_v<float> );
	static_assert( is_unsigned_v<unsigned> );

	template<typename T>
	using is_arithmetic =
	  std::disjunction<is_number<T>, std::is_enum<T>,
	                   std::conditional_t<std::numeric_limits<T>::is_specialized,
	                                      std::true_type, std::false_type>>;

	template<typename T>
	inline constexpr bool is_arithmetic_v = is_arithmetic<T>::value;
} // namespace daw
