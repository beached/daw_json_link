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

#include <daw/daw_consteval.h>

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
// Use DAW_NO_CONSTEXPR_SCOPE_GUARD to disable constexpr scope guard.
// DAW_HAS_CONSTEXPR_SCOPE_GUARD is defined when it is enabled and in
// <daw/daw_scope_guard.h>
// This has performance impacts on MSVC as on_exit_success calls
// std::uncaught_exceptions( ) which is very slow there
#if not defined( DAW_NO_CONSTEXPR_SCOPE_GUARD ) and \
  not defined( DAW_JSON_ENABLE_FULL_RVO )
#define DAW_NO_CONSTEXPR_SCOPE_GUARD
#endif

// DAW_IS_CONSTANT_EVALUATED is defined when a builtin or
// std::is_constant_evaluated( ) is available in
// <daw/daw_is_constant_evaluated.h>

// Show extra diagnostic information like unmapped members when parsing
// by defining DAW_JSON_PARSER_DIAGNOSTICS

#if defined( __GNUC__ )
#define DAW_CAN_CONSTANT_EVAL( ... ) \
	( __builtin_constant_p( __VA_ARGS__ ) == 1 )
#else
#define DAW_CAN_CONSTANT_EVAL( ... ) true
#endif

// If the compiler supports CNTTP types allow for strings in json data
// contracts.  Both support passing local char const[], but the type is
// different.  To keep old behaviour when using C++20, define
// DAW_USE_CPP17_ABI
#if not defined( DAW_USE_CPP17_ABI )
#if defined( __cpp_nontype_template_parameter_class )
#if not defined( DAW_JSON_CNTTP_JSON_NAME )
#define DAW_JSON_CNTTP_JSON_NAME
#endif
#endif
#if defined( __cpp_nontype_template_args )
#if __cpp_nontype_template_args >= 201911L
#if not defined( DAW_JSON_CNTTP_JSON_NAME )
#define DAW_JSON_CNTTP_JSON_NAME
#endif
#endif
#endif
#if defined( __clang_major__ ) and __cplusplus >= 202002L
#if __clang_major__ >= 12
// Clang 12 supports enough of CNTTP string literals and compiles the tests
// successfully, but does not define the feature macro
#if not defined( DAW_JSON_CNTTP_JSON_NAME )
#define DAW_JSON_CNTTP_JSON_NAME
#endif
#endif
#endif
#endif

#if defined( __cpp_constexpr_dynamic_alloc )
#define CPP20CONSTEXPR constexpr
#else
// TODO: rename in v4
#define CPP20CONSTEXPR
#endif

// Fix bug in MSVC
#if defined( _MSC_VER ) and not defined( __clang__ )
#define DAW_JSON_MAKE_LOC_INFO_CONSTEVAL constexpr
#else
#define DAW_JSON_MAKE_LOC_INFO_CONSTEVAL DAW_CONSTEVAL
#endif

// Allow experimental SIMD paths, if available
// by defining DAW_ALLOW_SSE42 and using the parser policy ExecModeType simd

// Use strtod instead of from_chars when avialable by defining
// DAW_JSON_USE_STRTOD
#if not defined( DAW_JSON_USE_STRTOD ) and not defined( __cpp_lib_to_chars )
#define DAW_JSON_USE_STRTOD
#endif

// define DAW_JSON_DISABLE_RANDOM to disable creating random iterators when the
// size is known up front.  This is playing to the implementations prior to
// C++23 when range constructors are added to containers.  It is disabled on
// MSVC as that impl does not work with it
#if defined( _MSC_VER ) and not defined( __clang__ )
#if not defined( DAW_JSON_DISABLE_RANDOM )
#define DAW_JSON_DISABLE_RANDOM
#endif
#endif
