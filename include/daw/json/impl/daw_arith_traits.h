// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/daw_json_link
//
#pragma once

#include <limits>

namespace daw {
	template<typename T>
	inline constexpr bool
	  is_floating_point_v = ( not std::numeric_limits<T>::is_integer ) and
	                        std::numeric_limits<T>::is_signed and
	                        ( not std::numeric_limits<T>::is_exact );

	template<typename T>
	inline constexpr bool is_integral_v = std::numeric_limits<T>::is_integer;

	template<typename T>
	inline constexpr bool
	  is_signed_v = (is_floating_point_v<T> or
	                 is_integral_v<T>)and std::numeric_limits<T>::is_signed;

	template<typename T>
	inline constexpr bool
	  is_unsigned_v = is_integral_v<T> and
	                  ( not std::numeric_limits<T>::is_signed );

	template<typename T>
	inline constexpr bool is_arithmetic_v =
	  is_integral_v<T> or is_floating_point_v<T>;
} // namespace daw
