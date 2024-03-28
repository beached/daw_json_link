// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/daw_json_link
//

#pragma once

#include "version.h"

#include "daw_json_parse_common.h"
#include "daw_json_traits.h"

#include <cstddef>

namespace daw::json {
	inline namespace DAW_JSON_VER {
		///
		/// @brief Allows for alternate/multiple mappings of types that already have
		/// mappings.
		///@tparam C Base type to map to
		///@tparam Idx alternate mapping index
		///
		template<typename C, std::size_t = 0>
		struct json_alt {
			using type = C;
		};

		///
		/// @brief Default constructor for json_alt is to construct the base type
		/// @tparam T base type to construct
		/// @tparam I index of alternate mapping
		///
		template<typename T, std::size_t I>
		struct default_constructor<json_alt<T, I>>
		  : default_constructor<
		      json_details::json_result_t<json_details::json_deduced_type<T>>> {};

	} // namespace DAW_JSON_VER
} // namespace daw::json
