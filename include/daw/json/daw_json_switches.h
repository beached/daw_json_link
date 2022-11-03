// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/daw_json_link
//

// This file will have all the preprocessor based switches that users can
// switch. Many are controllable from cmake via commandline options too.

#pragma once

#if defined( __GNUC__ ) and not defined( __clang__ ) and \
  not defined( DAW_JSON_FLATTEN ) and not defined( DAW_NO_FLATTEN )
#define DAW_NO_FLATTEN
#endif

// Does json_link throw itself.  Other things like constructors for the user
// types may.
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

// Disable using guaranteed RVO to clean up after return value construction.
#if defined( DAW_JSON_DISABLE_ALWAYS_RVO )
#if defined( DAW_JSON_ENABLE_ALWAYS_RVO )
#error Conflicting defines DAW_JSON_DISABLE_ALWAYS_RVO and DAW_JSON_ENABLE_ALWAYS_RVO
#endif
#else
#if defined( _MSC_VER ) and not defined( DAW_JSON_ENABLE_ALWAYS_RVO )
// This is needed on MSVC as it's std::uncaught_exceptions( ) is expensive
#define DAW_JSON_DISABLE_ALWAYS_RVO
#elif defined( DAW_HAS_CONSTEXPR_SCOPE_GUARD )
#define DAW_JSON_ENABLE_ALWAYS_RVO
#endif
#endif