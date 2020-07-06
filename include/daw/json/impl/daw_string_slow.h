// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/daw_json_link
//

#pragma once

#include <cstddef>

namespace daw::json::json_details {
	constexpr inline std::size_t set_slow_path( bool b ) noexcept {
		return static_cast<std::size_t>( b );
	}

	/***
	 * When skip_string encounters escaping, it will note this as a non-zero value
	 */
	template<typename Range>
	constexpr inline bool needs_slow_path( Range const &rng ) noexcept {
		return static_cast<bool>( rng.counter );
	}
} // namespace daw::json::json_details
