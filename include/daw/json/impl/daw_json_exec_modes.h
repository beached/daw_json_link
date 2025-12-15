// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/daw_json_link
//

#pragma once

#include "version.h"

#include <daw/daw_is_constant_evaluated.h>

#include <string_view>
#include <type_traits>

namespace daw::json {
	inline namespace DAW_JSON_VER {
		struct constexpr_exec_tag {
			static constexpr std::string_view name = "constexpr";
			static constexpr bool can_constexpr = true;
		};
		struct runtime_exec_tag : constexpr_exec_tag {
			static constexpr std::string_view name = "runtime";
			static constexpr bool can_constexpr = false;
		};
#if defined( DAW_ALLOW_SSE42 )
		struct sse42_exec_tag : runtime_exec_tag {
			static constexpr std::string_view name = "sse4.2";
			static constexpr bool can_constexpr = false;
		};
		using simd_exec_tag = sse42_exec_tag;
#else
		struct simd_exec_tag : runtime_exec_tag {};
#endif
		using default_exec_tag = constexpr_exec_tag;

		namespace json_details {
			template<typename ExecTag>
			DAW_ATTRIB_INLINE constexpr bool use_constexpr_exec_mode( ) {
				return not DAW_IS_CONSTANT_EVALUATED_COMPAT( ) or
				       std::is_base_of_v<runtime_exec_tag, ExecTag>;
			}
		} // namespace json_details
	} // namespace DAW_JSON_VER
} // namespace daw::json
