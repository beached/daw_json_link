// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/
//

#pragma once

#include <daw/json/impl/daw_json_parse_policy.h>

#include <string_view>

/***
 * These methods are used to speed up duplicate compiles and provide a common
 * way to exclude headers and forward declare
 */
namespace daw {
	/***
	 * Allow for faster compiles and hide json parsing in single TU for this type
	 * @tparam Result result type
	 * @tparam ParsePolicy parse policy for parsing
	 * @param json_doc json document to parse
	 * @param path optional path to json member
	 * @return A reified object of Result typed
	 */
	template<typename Result,
	         typename ParsePolicy = daw::json::SIMDNoCommentSkippingPolicyChecked<
	           daw::json::constexpr_exec_tag>>
	Result parse_json_data( std::string_view json_doc,
	                        std::string_view path = std::string_view{ } );
} // namespace daw
