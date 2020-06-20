// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/daw_json_link
//

#pragma once

#include "daw_parse_policy_hash_comments.h"
#include "daw_parse_policy_no_comments.h"

namespace daw::json {
	using DefaultParsePolicy = NoCommentSkippingPolicyChecked;
}
