// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/daw_json_link
//

#pragma once

#include "version.h"

#include "daw_json_parse_common.h"

#include <daw/daw_attributes.h>

namespace daw::json {
	inline namespace DAW_JSON_VER {
		namespace json_details {
			template<typename JsonMember, bool KnownBounds, ParseState ParseState>
			[[nodiscard]] DAW_ATTRIB_FLATTEN constexpr json_result<JsonMember>
			parse_literal_array_value( ParseState &parse_state ) {
				(void)parse_state;
				return { };
			}
		} // namespace json_details
	}   // namespace DAW_JSON_VER
} // namespace daw::json
