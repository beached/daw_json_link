// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/daw_json_link
//

#pragma once

/// The version string used in namespace definitions.  Must be a valid namespace
/// name.
#if not defined( DAW_JSON_VER_OVERRIDE )
#define DAW_JSON_VER v3_0_0_beta
#else
#define DAW_JSON_VER DAW_JSON_VER_OVERRIDE
#endif
#if not defined( DAW_JSON_VER_INLINE_OVERRIDE )
#define DAW_JSON_INLINE_NS inline
#else
#define DAW_JSON_INLINE_NS
#endif

#if defined( __GNUC__ ) and not defined( __clang__ ) and \
  not defined( DAW_JSON_FLATTEN ) and not defined( DAW_NO_FLATTEN )
#define DAW_NO_FLATTEN
#endif
