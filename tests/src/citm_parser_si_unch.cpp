// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/
//

#include "citm_test_json.h"
#include "json_firewall.h"

#include <daw/json/daw_json_link.h>

using result_t = daw::citm::citm_object_t;
using policy_t =
  daw::json::SIMDNoCommentSkippingPolicyUnchecked<daw::json::simd_exec_tag>;

template<>
result_t daw::parse_json_data<result_t, policy_t>( std::string_view json_doc,
                                                   std::string_view path ) {
	return daw::json::from_json<result_t, policy_t>( json_doc, path );
}