// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/daw_json_link
//

#include "twitter_test_json.h"

#include <daw/json/daw_from_json.h>

#include <string_view>

namespace daw::json {
	template daw::twitter::twitter_object_t
	from_json<daw::twitter::twitter_object_t,
	          SIMDNoCommentSkippingPolicyUnchecked<constexpr_exec_tag>,
	          false,
	          daw::twitter::twitter_object_t>( std::string_view const &json_data,
	                                           std::string_view path );

	template daw::twitter::twitter_object_t
	from_json<daw::twitter::twitter_object_t,
	          SIMDNoCommentSkippingPolicyUnchecked<constexpr_exec_tag>,
	          false,
	          daw::twitter::twitter_object_t>( std::string_view const &json_data );
} // namespace daw::json
