// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/daw_json_link
//

#pragma once

#include "namespace.h"

#include <cstddef>

namespace DAW_JSON_NS::json_details {
	/***
	 * When skip_string encounters escaping, it will note this as a non-zero value
	 */
	template<typename ParseState>
	constexpr inline bool needs_slow_path( ParseState const &parse_state ) {
		return static_cast<std::ptrdiff_t>( parse_state.counter ) >= 0;
	}
} // namespace DAW_JSON_NS::json_details
