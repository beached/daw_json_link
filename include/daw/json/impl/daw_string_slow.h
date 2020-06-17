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

	constexpr inline bool needs_slow_path( std::size_t value ) noexcept {
		return static_cast<bool>( value );
	}
} // namespace daw::json::json_details
