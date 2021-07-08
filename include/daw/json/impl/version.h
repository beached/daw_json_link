// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/daw_json_link
//

#pragma once

/// The version string used in namespace definitions.  Must be a valid namespace
/// name
#define DAW_JSON_VER v3_0_0

#if defined( __GNUC__ ) and not defined( __clang__ ) and not defined( DAW_JSON_FLATTEN )
#define DAW_NO_FLATTEN
#endif
