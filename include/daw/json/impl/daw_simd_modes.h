// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/daw_json_link
//

#pragma once

namespace daw::json {
	enum class SIMDModes {
		None,
#ifdef DAW_ALLOW_SSE42
		SSE42
#endif
	};

	template<SIMDModes mode>
	using SIMDConst = std::integral_constant<SIMDModes, mode>;

	template<SIMDModes mode>
	inline constexpr auto SIMDConst_v = SIMDConst<mode>{ };

} // namespace daw::json
