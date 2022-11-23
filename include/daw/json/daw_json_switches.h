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

#if defined( _MSC_VER ) and not defined( __clang__ )
#define DAW_JSON_COMPILER_MSVC
#define DAW_JSON_COMPILER_MSVC_COMPAT
#elif defined( _MSC_VER ) and defined( __clang__ )
#define DAW_JSON_COMPILER_CLANG
#define DAW_JSON_COMPILER_CLANGCL
#define DAW_JSON_COMPILER_MSVC_COMPAT
#define DAW_JSON_COMPILER_CLANG_COMPAT
#elif defined( __GNUC__ ) and not defined( __clang__ )
#define DAW_JSON_COMPILER_GCC
#define DAW_JSON_COMPILER_GCC_COMPAT
#elif defined( __clang__ )
#define DAW_JSON_COMPILER_CLANG
#define DAW_JSON_COMPILER_CLANG_COMPAT
#define DAW_JSON_COMPILER_GCC_COMPAT
#endif

/// DAW_NO_FLATTEN disables any flatten attributes in code.  This is disabled in
/// GCC by default as it cannot handle it
#if defined( DAW_JSON_COMPILER_GCC ) and not defined( DAW_JSON_FLATTEN )
#if not defined( DAW_NO_FLATTEN )
#define DAW_NO_FLATTEN
#endif
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

// DAW_CAN_CONSTANT_EVAL is used to test if we are in a constant expression
#if defined( DAW_JSON_COMPILER_GCC_COMPAT )
#define DAW_CAN_CONSTANT_EVAL( ... ) \
	( __builtin_constant_p( __VA_ARGS__ ) == 1 )
#else
#define DAW_CAN_CONSTANT_EVAL( ... ) true
#endif

// If the compiler supports CNTTP types allow for strings in json data
// contracts.  Both support passing local char const[], but the type is
// different.  To keep old behaviour when using C++20, define
// DAW_USE_CPP17_ABI
#define DAW_USE_CPP17_ABI
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
#if defined( DAW_JSON_COMPILER_MSVC )
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
#if defined( DAW_JSON_COMPILER_MSVC )
#if not defined( DAW_JSON_DISABLE_RANDOM )
#define DAW_JSON_DISABLE_RANDOM
#endif
#endif

// DAW_JSON_HAS_BUILTIN_UADD is used to switch to a constexpr method of overflow
// addition when available
#if( defined( __GNUC__ ) and __GNUC__ >= 8 ) or defined( __clang__ ) or \
  ( DAW_HAS_BUILTIN( __builtin_uadd_overflow ) and                      \
    DAW_HAS_BUILTIN( __builtin_uaddl_overflow ) and                     \
    DAW_HAS_BUILTIN( __builtin_uaddll_overflow ) )
#define DAW_JSON_HAS_BUILTIN_UADD
#endif

// DAW_JSON_BUGFIX_FROM_JSON_001
// Defined for MSVC as it has been ICE'ing on daw_json_ensure in from_json
#if defined( DAW_JSON_COMPILER_MSVC )
#define DAW_JSON_BUGFIX_FROM_JSON_001
#endif

// DAW_JSON_BUGFIX_MSVC_EVAL_ORDER_002
// MSVC cannot always evaluate in the correct order as defined by the standard.
// JSON Link uses the left->right guarantees in places like constructors so that
// less state is needed.  In MSVC one must keep more state in places like
// parse_tuple_value
#if defined( DAW_JSON_COMPILER_MSVC )
#define DAW_JSON_BUGFIX_MSVC_EVAL_ORDER_002
#endif

// DAW_JSON_BUGFIX_MSVC_KNOWN_LOC_ICE_003
// MSVC in C++20 mode will ICE when known locations is evaluated at compile time
#if( defined( DAW_JSON_COMPILER_MSVC ) and __cpp_constexpr > 201700L )
#define DAW_JSON_BUGFIX_MSVC_KNOWN_LOC_ICE_003
#endif

/// This is in addition to the parse policy.  Always do a full name match
/// instead of sometimes relying on the hash.  This was enabled in MSVC due to
/// an issue that should be rechecked
#if not defined( NDEBUG ) or defined( DEBUG ) or \
  defined( DAW_JSON_PARSER_DIAGNOSTICS ) or defined( DAW_JSON_COMPILER_MSVC )
#if not defined( DAW_JSON_ALWAYS_FULL_NAME_MATCH )
#define DAW_JSON_ALWAYS_FULL_NAME_MATCH
#endif
#endif

// Allows pack expansions with get<Is> without separate methods.  This should
// improve symbol sizes
#if defined( __cpp_generic_lambdas )
#if __cpp_generic_lambdas >= 201707L
#define DAW_JSON_USE_GENERIC_LAMBDAS
#endif
#endif