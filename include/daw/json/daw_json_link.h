// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, version 1.0. (see accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/daw_json_link
//

#pragma once

#include "impl/version.h"

#include "daw_from_json.h"
#include "daw_json_apply.h"
#include "daw_json_exception.h"
#include "daw_json_link_types.h"
#include "daw_to_json.h"
#include "impl/daw_json_alternate_mapping.h"
#include "impl/daw_json_value.h"

namespace daw::json::literals {
	/***
	 * Construct a json_value from a string literal
	 * @return A json_value representing the json document
	 */
	constexpr daw::json::json_value operator""_dawjson( char const *ptr,
	                                                    std::size_t sz ) {
		return daw::json::json_value( ptr, sz );
	}
} // namespace daw::json::literals

namespace daw::json {
	inline namespace DAW_JSON_VER {
		DAW_CONSTEVAL std::string_view json_link_version( ) {
#define DAW_XSTR( x ) DAW_STR( x )
#define DAW_STR( x ) std::string_view( #x )
			return DAW_XSTR( DAW_JSON_VER );
#undef DAW_STR
#undef DAW_XSTR
		}
	} // namespace DAW_JSON_VER
} // namespace daw::json
