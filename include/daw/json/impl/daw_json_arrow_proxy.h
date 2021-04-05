// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/daw_json_link
//
#pragma once

#include "namespace.h"

namespace DAW_JSON_NS::json_details {
	template<typename T>
	struct arrow_proxy {
		T value;

		[[nodiscard]] constexpr T *operator->( ) && {
			return &value;
		}
	};
} // namespace DAW_JSON_NS::json_details
