// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/daw_json_link
//

#pragma once

namespace daw::json {
	struct NoCommentSkippingPolicy {
		static constexpr bool has_alternate_parse_tokens = false;
	};
} // namespace daw::json
