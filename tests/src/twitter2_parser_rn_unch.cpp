// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/
//

#include "twitter_test2_json.h"

#include <daw/json/daw_from_json.h>

namespace daw::json {
	template daw::twitter2::twitter_object_t
	from_json<daw::twitter2::twitter_object_t,
	          daw::json::SIMDNoCommentSkippingPolicyUnchecked<
	            daw::json::runtime_exec_tag>>( std::string_view json_data,
	                                           std::string_view path );

	template daw::twitter2::twitter_object_t
	from_json<daw::twitter2::twitter_object_t,
	          daw::json::SIMDNoCommentSkippingPolicyUnchecked<
	            daw::json::runtime_exec_tag>>( std::string_view json_data );
} // namespace daw::json
