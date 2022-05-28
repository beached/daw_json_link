// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/daw_json_link
//

#pragma once

#include "daw_json_allocator_wrapper.h"
#include "daw_json_option_bits.h"
#include "daw_json_parse_policy.h"

namespace daw::json {
	inline namespace DAW_JSON_VER {
		/// @brief A container for arbitrary JSON values
		/// @tparam ParseState see IteratorRange
		template<json_options_t PolicyFlags = json_details::default_policy_flag,
		         typename Allocator = json_details::NoAllocator>
		class basic_json_value;
	} // namespace DAW_JSON_VER
} // namespace daw::json
