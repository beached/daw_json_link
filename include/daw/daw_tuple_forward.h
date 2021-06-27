// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/daw_json_link
//

#pragma once

#include <daw/cpp_17.h>

#include <type_traits>
#include <tuple>

namespace daw {
	/// Forwards arguments but stores rvalues.  This allows use to prevent dangling ref's
	template<typename... Ts>
	constexpr auto forward_nonrvalue_as_tuple( Ts &&...values ) {
		using tuple_t =
		  std::tuple<std::conditional_t<std::is_rvalue_reference_v<Ts>,
		                                daw::remove_cvref_t<Ts>, Ts>...>;
		return tuple_t{ DAW_FWD( values )... };
	}
} // namespace daw


