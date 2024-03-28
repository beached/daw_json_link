// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/daw_json_link
//

#pragma once

#include <daw/json/daw_json_switches.h>

#include <daw/ciso646.h>

/// The version string used in namespace definitions.  Must be a valid namespace
/// name.
#if not defined( DAW_JSON_VER_OVERRIDE )
// Should be updated when a potential ABI break is anticipated
#if defined( DEBUG ) or not defined( NDEBUG )
#define DAW_JSON_VER v3_24_0d
#else
#define DAW_JSON_VER v3_24_0
#endif
#else
#define DAW_JSON_VER DAW_JSON_VER_OVERRIDE
#endif
