// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/daw_json_link
//

#pragma once

#include <daw/daw_cpp_feature_check.h>

/// Disable concepts on gcc < 13.3.
/// See https://github.com/beached/daw_json_link/issues/454
#if DAW_HAS_GCC_VER_LT( 13, 3 )
#define DAW_JSON_DONT_USE_CONCEPTS
#endif

#if not defined( DAW_JSON_DONT_USE_CONCEPTS ) and defined( __cpp_lib_concepts )
#if __cpp_lib_concepts >= 202002L
#define DAW_JSON_USE_CONCEPTS
#endif
#endif

// Allow customizable trait
#define DAW_JSON_MAKE_REQ_TRAIT_CUSTOM( Name, /*ReqExpression*/... ) \
	template<typename, typename = void>                                \
	inline constexpr bool Name = false;                                \
                                                                     \
	template<typename T>                                               \
	inline constexpr bool Name<T, std::void_t<decltype( __VA_ARGS__ )>> = true

#if defined( DAW_JSON_USE_CONCEPTS )
#define DAW_JSON_MAKE_REQ_TRAIT( Name, /*ReqExpression*/... ) \
	template<typename T>                                        \
	inline constexpr bool Name = requires {                     \
		__VA_ARGS__;                                              \
	}

#else
#define DAW_JSON_MAKE_REQ_TRAIT( Name, /*ReqExpression*/... ) \
	DAW_JSON_MAKE_REQ_TRAIT_CUSTOM( Name, __VA_ARGS__ )
#endif

#if defined( DAW_JSON_USE_CONCEPTS )
#define DAW_JSON_MAKE_REQ_TRAIT2( Name, /*ReqExpression*/... ) \
	template<typename T, typename U>                             \
	concept Name = requires {                                    \
		__VA_ARGS__;                                               \
	}
#else
#define DAW_JSON_MAKE_REQ_TRAIT2( Name, /*ReqExpression*/... )             \
	template<typename, typename, typename = void>                            \
	inline constexpr bool Name = false;                                      \
                                                                           \
	template<typename T, typename U>                                         \
	inline constexpr bool Name<T, U, std::void_t<decltype( __VA_ARGS__ )>> = \
	  true
#endif

#if defined( DAW_JSON_USE_CONCEPTS )
#define DAW_JSON_MAKE_REQ_TYPE_ALIAS_TRAIT_NT( Name, /*MemberTypeAlias*/... ) \
	template<typename T>                                                        \
	concept Name = requires {                                                   \
		typename __VA_ARGS__;                                                     \
	}
#else
#define DAW_JSON_MAKE_REQ_TYPE_ALIAS_TRAIT_NT( Name, /*MemberTypeAlias*/... ) \
	template<typename, typename = void>                                         \
	inline constexpr bool Name = false;                                         \
                                                                              \
	template<typename T>                                                        \
	inline constexpr bool Name<T, std::void_t<__VA_ARGS__>> = true
#endif

#define DAW_JSON_MAKE_REQ_TYPE_ALIAS_TRAIT_CUSTOM( Name,                    \
                                                   /*MemberTypeAlias*/... ) \
	template<typename, typename = void>                                       \
	inline constexpr bool Name = false;                                       \
                                                                            \
	template<typename T>                                                      \
	inline constexpr bool Name<T, std::void_t<typename __VA_ARGS__>> = true

#if defined( DAW_JSON_USE_CONCEPTS )
#define DAW_JSON_MAKE_REQ_TYPE_ALIAS_TRAIT( Name, /*MemberTypeAlias*/... ) \
	template<typename T>                                                     \
	concept Name = requires {                                                \
		typename __VA_ARGS__;                                                  \
	}
#else
#define DAW_JSON_MAKE_REQ_TYPE_ALIAS_TRAIT( Name, /*MemberTypeAlias*/... ) \
	DAW_JSON_MAKE_REQ_TYPE_ALIAS_TRAIT_CUSTOM( Name, __VA_ARGS__ )
#endif

#if defined( DAW_JSON_USE_CONCEPTS )
#define DAW_JSON_MAKE_REQ_TYPE_ALIAS_TRAIT2( Name, /*MemberTypeAlias*/... ) \
	template<typename T, typename U>                                          \
	concept Name = requires {                                                 \
		__VA_ARGS__;                                                            \
	}
#else
#define DAW_JSON_MAKE_REQ_TYPE_ALIAS_TRAIT2( Name, /*MemberTypeAlias*/... ) \
	template<typename, typename, typename = void>                             \
	inline constexpr bool Name = false;                                       \
                                                                            \
	template<typename T, typename U>                                          \
	inline constexpr bool Name<T, U, std::void_t<__VA_ARGS__>> = true
#endif
