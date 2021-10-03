// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/daw_json_link
//

#pragma once

#include "daw_json_enums.h"
#include "daw_json_parse_digit.h"
#include "daw_json_parse_iso8601_utils.h"
#include "daw_json_serialize_impl.h"
#include "version.h"

#include <daw/daw_string_view.h>
#include <daw/daw_utility.h>

#include <chrono>
#include <ciso646>
#include <optional>
#include <string_view>

namespace daw::json {
	inline namespace DAW_JSON_VER DAW_ATTRIB_PUBLIC {
		template<JsonNullable>
		struct construct_from_iso8601_timestamp {
			using result_type =
			  std::optional<std::chrono::time_point<std::chrono::system_clock,
			                                        std::chrono::milliseconds>>;

			[[maybe_unused, nodiscard]] inline constexpr result_type
			operator( )( ) const {
				return { };
			}

			[[maybe_unused, nodiscard]] inline constexpr result_type
			operator( )( char const *ptr, std::size_t sz ) const {
				return datetime::parse_iso8601_timestamp( daw::string_view( ptr, sz ) );
			}
		};

		template<>
		struct construct_from_iso8601_timestamp<JsonNullable::MustExist> {
			using result_type = std::chrono::time_point<std::chrono::system_clock,
			                                            std::chrono::milliseconds>;

			[[maybe_unused, nodiscard]] inline constexpr result_type
			operator( )( char const *ptr, std::size_t sz ) const {
				return datetime::parse_iso8601_timestamp( daw::string_view( ptr, sz ) );
			}
		};

	} // namespace DAW_JSON_VER
} // namespace daw::json
