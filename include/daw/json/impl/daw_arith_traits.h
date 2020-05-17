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

	inline constexpr bool is_integer_v = std::numeric_limits<T>::is_integer;

	inline constexpr bool is_signed_v =
	  (is_floating_point_v<T> or
	   is_integer_v<T>)and std::numeric_limits<T>::is_signed;

	inline constexpr bool is_unsigned_v =
	  is_integer_v<T> and ( not std::numeric_limits<T>::is_signed );

	inline constexpr bool is_arithmetic_v =
	  is_integer_v<T> or is_floating_point_v<T>;
}
