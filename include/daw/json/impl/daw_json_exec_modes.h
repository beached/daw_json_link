// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/daw_json_link
//

#pragma once

#include "version.h"

#include <daw/daw_scope_guard.h>

#include <string_view>

namespace daw::json DAW_ATTRIB_PUBLIC {
	inline namespace DAW_JSON_VER {
		struct constexpr_exec_tag {
			static constexpr std::string_view name = "constexpr";
#if defined( DAW_HAS_CONSTEXPR_SCOPE_GUARD )
			static constexpr bool always_rvo = true;
#else
			static constexpr bool always_rvo = false;
#endif
			static constexpr bool can_constexpr = true;
		};
		struct runtime_exec_tag : constexpr_exec_tag {
			static constexpr std::string_view name = "runtime";
			static constexpr bool always_rvo = true;
			static constexpr bool can_constexpr = false;
		};
#if defined( DAW_ALLOW_SSE42 )
		struct sse42_exec_tag : runtime_exec_tag {
			static constexpr std::string_view name = "sse4.2";
			static constexpr bool always_rvo = true;
			static constexpr bool can_constexpr = false;
		};
		using simd_exec_tag = sse42_exec_tag;
#else
		struct simd_exec_tag : runtime_exec_tag {};
#endif
		using default_exec_tag = constexpr_exec_tag;
	} // namespace DAW_JSON_VER
} // namespace daw::json
