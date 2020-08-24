// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/daw_json_link
//

#pragma once

#include <string_view>

namespace daw::json {
	struct constexpr_exec_tag {
		static constexpr std::string_view name = "constexpr";
	};
	struct runtime_exec_tag : constexpr_exec_tag {
		static constexpr std::string_view name = "runtime";
	};
#if defined( DAW_ALLOW_SSE42 )
	struct sse42_exec_tag : runtime_exec_tag {
		static constexpr std::string_view name = "sse4.2";
	};
	using simd_exec_tag = sse42_exec_tag;
#elif defined( DAW_ALLOW_NEON )
	// TODO add Neon
	using simd_exec_tag = runtime_exec_tag;
#else
	using simd_exec_tag = runtime_exec_tag;
#endif

} // namespace daw::json
