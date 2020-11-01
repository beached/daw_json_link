// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/daw_json_link
//

#pragma once

#include <cstdint>
#include <string_view>

namespace daw::twitter {
	struct twitter_user {
		std::string_view screen_name;
		std::uint64_t id;
	};
} // namespace daw::twitter
