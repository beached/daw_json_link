// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/daw_json_link
//

#pragma once

#include <daw/json/daw_from_json_fwd.h>

#include <cstdint>
#include <string_view>
#include <vector>

namespace apache_builds {
	struct jobs_t {
		std::string_view name;
		std::string_view url;
		std::string_view color;
	};

	struct views_t {
		std::string_view name;
		std::string_view url;
	};

	struct apache_builds {
		std::string_view mode;
		std::string_view nodeDescription;
		std::string_view nodeName;
		int64_t numExecutors;
		std::string_view description;
		std::vector<jobs_t> jobs;
		views_t primaryView;
		bool quietingDown;
		int64_t slaveAgentPort;
		bool useCrumbs;
		bool useSecurity;
		std::vector<views_t> views;
	};
} // namespace apache_builds

#if defined( DAW_JSON_TEST_EXTERN_TEMPLATE )

namespace daw::json {
	extern template apache_builds::apache_builds
	from_json<apache_builds::apache_builds,
	          SIMDNoCommentSkippingPolicyChecked<constexpr_exec_tag>,
	          false,
	          apache_builds::apache_builds>( std::string_view const &json_data,
	                                         std::string_view json_path );

	extern template apache_builds::apache_builds
	from_json<apache_builds::apache_builds,
	          SIMDNoCommentSkippingPolicyUnchecked<constexpr_exec_tag>,
	          false,
	          apache_builds::apache_builds>( std::string_view const &json_data,
	                                         std::string_view json_path );

	extern template apache_builds::apache_builds
	from_json<apache_builds::apache_builds,
	          SIMDNoCommentSkippingPolicyChecked<runtime_exec_tag>,
	          false,
	          apache_builds::apache_builds>( std::string_view const &json_data,
	                                         std::string_view json_path );

	extern template apache_builds::apache_builds
	from_json<apache_builds::apache_builds,
	          SIMDNoCommentSkippingPolicyUnchecked<runtime_exec_tag>,
	          false,
	          apache_builds::apache_builds>( std::string_view const &json_data,
	                                         std::string_view json_path );

	extern template apache_builds::apache_builds
	from_json<apache_builds::apache_builds,
	          SIMDNoCommentSkippingPolicyChecked<simd_exec_tag>,
	          false,
	          apache_builds::apache_builds>( std::string_view const &json_data,
	                                         std::string_view json_path );

	extern template apache_builds::apache_builds
	from_json<apache_builds::apache_builds,
	          SIMDNoCommentSkippingPolicyUnchecked<simd_exec_tag>,
	          false,
	          apache_builds::apache_builds>( std::string_view const &json_data,
	                                         std::string_view json_path );

	extern template apache_builds::apache_builds
	from_json<apache_builds::apache_builds,
	          SIMDNoCommentSkippingPolicyChecked<constexpr_exec_tag>,
	          false,
	          apache_builds::apache_builds>( std::string_view const &json_data );

	extern template apache_builds::apache_builds
	from_json<apache_builds::apache_builds,
	          SIMDNoCommentSkippingPolicyUnchecked<constexpr_exec_tag>,
	          false,
	          apache_builds::apache_builds>( std::string_view const &json_data );

	extern template apache_builds::apache_builds
	from_json<apache_builds::apache_builds,
	          SIMDNoCommentSkippingPolicyChecked<runtime_exec_tag>,
	          false,
	          apache_builds::apache_builds>( std::string_view const &json_data );

	extern template apache_builds::apache_builds
	from_json<apache_builds::apache_builds,
	          SIMDNoCommentSkippingPolicyUnchecked<runtime_exec_tag>,
	          false,
	          apache_builds::apache_builds>( std::string_view const &json_data );

	extern template apache_builds::apache_builds
	from_json<apache_builds::apache_builds,
	          SIMDNoCommentSkippingPolicyChecked<simd_exec_tag>,
	          false,
	          apache_builds::apache_builds>( std::string_view const &json_data );

	extern template apache_builds::apache_builds
	from_json<apache_builds::apache_builds,
	          SIMDNoCommentSkippingPolicyUnchecked<simd_exec_tag>,
	          false,
	          apache_builds::apache_builds>( std::string_view const &json_data );
} // namespace daw::json
#endif
