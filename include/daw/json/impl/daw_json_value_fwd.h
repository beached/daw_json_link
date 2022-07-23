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
#include "daw_json_parse_options_impl.h"
#include "daw_json_parse_policy.h"

namespace daw::json {
	inline namespace DAW_JSON_VER {
		/// @brief A container for arbitrary JSON values
		/// @tparam ParseState see IteratorRange
		template<json_options_t PolicyFlags = json_details::default_policy_flag,
		         typename Allocator = json_details::NoAllocator>
		struct basic_json_value;

		/// @brief An untyped JSON value
		using json_value = basic_json_value<>;

		template<typename>
		inline constexpr bool is_a_basic_json_value = false;

		template<json_options_t PolicyFlags, typename Allocator>
		inline constexpr bool
		  is_a_basic_json_value<basic_json_value<PolicyFlags, Allocator>> = true;
	} // namespace DAW_JSON_VER
} // namespace daw::json
