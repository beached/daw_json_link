// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/daw_json_link
//

#pragma once

#include "version.h"

#include "daw_json_parse_iso8601_utils.h"

#include <daw/daw_string_view.h>

#include <chrono>
#include <ciso646>

namespace daw::json {
	inline namespace DAW_JSON_VER {
		struct construct_from_iso8601_timestamp {
			using result_type = std::chrono::time_point<std::chrono::system_clock,
			                                            std::chrono::milliseconds>;

			[[nodiscard]] DAW_JSON_CPP23_STATIC_CALL_OP inline constexpr result_type
			operator( )( char const *ptr,
			             std::size_t sz ) DAW_JSON_CPP23_STATIC_CALL_OP_CONST {
				return datetime::parse_iso8601_timestamp( daw::string_view( ptr, sz ) );
			}
		};
	} // namespace DAW_JSON_VER
} // namespace daw::json
