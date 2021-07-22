// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/daw_json_link
//

#pragma once

#include <daw/cpp_17.h>

#include <tuple>
#include <type_traits>

namespace daw {
	template<typename T>
	using remove_rvalue_ref_t = std::conditional_t<std::is_rvalue_reference_v<T>,
	                                               daw::remove_cvref_t<T>, T>;
	/// Forwards arguments but stores rvalues.  This allows use to prevent
	/// dangling ref's
	template<typename... Ts>
	constexpr auto forward_nonrvalue_as_tuple( Ts &&...values ) {
		using tuple_t = std::tuple<remove_rvalue_ref_t<Ts>...>;

		return tuple_t{ DAW_FWD( values )... };
	}
} // namespace daw
