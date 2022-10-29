// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/daw_json_link
//

#pragma once

namespace daw::json::inline DAW_JSON_VER {
	template<typename T>
	inline constexpr bool is_parse_state_v = false;

	template<typename T>
	concept ParseState = is_parse_state_v<T>;
} // namespace daw::json::inline DAW_JSON_VER
