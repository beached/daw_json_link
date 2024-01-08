// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/daw_json_link
//

#pragma once

#include "version.h"

#include "daw_json_parse_iso8601_utils.h"

#include <daw/daw_string_view.h>

#include <chrono>

namespace daw::json {
	inline namespace DAW_JSON_VER {
		template<typename TP>
		struct construct_from_iso8601_timestamp;

		template<typename Clock, typename Duration>
		struct construct_from_iso8601_timestamp<
		  std::chrono::time_point<Clock, Duration>> {
			using result_type = std::chrono::time_point<Clock, Duration>;
#if defined( DAW_JSON_HAS_STATIC_CALL_OP ) and DAW_HAS_CLANG_VER_GTE( 17, 0 )
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wc++23-extensions"
#endif
			[[nodiscard]] DAW_JSON_CPP23_STATIC_CALL_OP inline constexpr result_type
			operator( )( char const *ptr,
			             std::size_t sz ) DAW_JSON_CPP23_STATIC_CALL_OP_CONST {
				return datetime::parse_iso8601_timestamp<result_type>(
				  daw::string_view( ptr, sz ) );
			}
#if defined( DAW_JSON_HAS_STATIC_CALL_OP ) and DAW_HAS_CLANG_VER_GTE( 17, 0 )
#pragma clang diagnostic pop
#endif
		};
	} // namespace DAW_JSON_VER
} // namespace daw::json
