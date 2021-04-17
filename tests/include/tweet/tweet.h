// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/daw_json_link
//

#pragma once

#include "twitter_user.h"

#include <cstdint>
#include <string_view>

namespace daw::twitter {
	struct tweet {
		std::uint32_t retweet_count;
		twitter_user user;
		std::string_view created_at;
		std::uint64_t in_reply_to_status_id;
		std::uint32_t favorite_count;
		std::uint64_t id;
		std::string_view text;
	};
} // namespace daw::twitter
