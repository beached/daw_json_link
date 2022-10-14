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
// Should be updated when a potential ABI break is anticipated
#define DAW_JSON_VER v3_1_0
#else
#define DAW_JSON_VER DAW_JSON_VER_OVERRIDE
#endif

#if defined( __GNUC__ ) and not defined( __clang__ ) and \
  not defined( DAW_JSON_FLATTEN ) and not defined( DAW_NO_FLATTEN )
#define DAW_NO_FLATTEN
#endif

#if defined( DAW_USE_EXCEPTIONS )
#if defined( DAW_JSON_DONT_USE_EXCEPTIONS )
#error Conflicting defines DAW_USE_EXCEPTIONS and DAW_JSON_DONT_USE_EXCEPTIONS
#endif
#if defined( DAW_DONT_USE_EXCEPTIONS )
#error Conflicting defines DAW_USE_EXCEPTIONS and DAW_DONT_USE_EXCEPTIONS
#endif
#else
#if not defined( DAW_JSON_DONT_USE_EXCEPTIONS )
#define DAW_JSON_DONT_USE_EXCEPTIONS
#endif
#if not defined( DAW_DONT_USE_EXCEPTIONS )
#define DAW_JSON_DONT_USE_EXCEPTIONS
#endif
#endif
