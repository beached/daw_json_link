// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/daw_json_link
//

#pragma once

#include "daw_json_parse_policy.h"
#include "version.h"

namespace daw::json {
	inline namespace DAW_JSON_VER {
		using DefaultParsePolicy = NoCommentSkippingPolicyChecked;
	}
} // namespace daw::json
