// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/daw_json_link
//

#pragma once

#include "version.h"

#include "daw_json_enums.h"
#include "daw_json_parse_iso8601_utils.h"

#include <daw/daw_not_null.h>
#include <daw/daw_string_view.h>

#include <chrono>
#include <ciso646>
#include <optional>

namespace daw::json {
	DAW_JSON_INLINE_NS namespace DAW_JSON_VER {
		template<JsonNullable>
		struct construct_from_iso8601_timestamp {
			using result_type = std::optional<
			  std::chrono::time_point<std::chrono::system_clock, std::chrono::milliseconds>>;

			[[maybe_unused, nodiscard]] inline constexpr result_type operator( )( ) const {
				return { };
			}

			[[maybe_unused, nodiscard]] inline constexpr result_type
			operator( )( daw::not_null<char const *> ptr, std::size_t sz ) const {
				return datetime::parse_iso8601_timestamp( daw::string_view( ptr, sz ) );
			}
		};

		template<>
		struct construct_from_iso8601_timestamp<JsonNullable::MustExist> {
			using result_type =
			  std::chrono::time_point<std::chrono::system_clock, std::chrono::milliseconds>;

			[[maybe_unused, nodiscard]] inline constexpr result_type
			operator( )( daw::not_null<char const *> ptr, std::size_t sz ) const {
				return datetime::parse_iso8601_timestamp( daw::string_view( ptr, sz ) );
			}
		};
	} // namespace DAW_JSON_VER
} // namespace daw::json
