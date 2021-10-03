// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/daw_json_link
//

#pragma once

#include "daw_json_traits.h"
#include "version.h"

namespace daw::json DAW_ATTRIB_PUBLIC {
	inline namespace DAW_JSON_VER {
		/***
		 * Allows for alternate mappings of types.
		 * @tparam C Base type to map to
		 * @tparam Idx alternate mapping index
		 */
		template<typename C, std::size_t Idx = 0>
		struct json_alt {
			using type = C;
			static constexpr std::size_t index = Idx;
		};

		/***
		 * Default constructor for json_alt is to construct the base type
		 * @tparam T base type to construct
		 * @tparam I index of alternate mapping
		 */
		template<typename T, std::size_t I>
		struct default_constructor<json_alt<T, I>> : default_constructor<T> {};
	} // namespace DAW_JSON_VER
} // namespace daw::json
