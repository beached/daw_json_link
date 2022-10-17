// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/
//

#pragma once

#include "../impl/version.h"

#include <daw/cpp_17.h>

#include <array>
#include <cstddef>
#include <iterator>
#include <type_traits>

namespace daw::json {
	inline namespace DAW_JSON_VER {
		namespace concepts {
			/// @brief Concept to help deduce container types.
			template<typename>
			struct container_traits : std::false_type {};
		} // namespace concepts
	}   // namespace DAW_JSON_VER
} // namespace daw::json
