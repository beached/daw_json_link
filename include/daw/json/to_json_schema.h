// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/daw_json_link
//

#pragma once

#include "daw_json_link_types.h"

namespace daw::json {
	inline namespace DAW_JSON_VER {
		template<typename T, typename OutputIterator>
		constexpr OutputIterator to_json_schema( OutputIterator out_it ) {
			using json_type = json_link_no_name<T>;

		}
	} // namespace DAW_JSON_VER
} // namespace daw::json
