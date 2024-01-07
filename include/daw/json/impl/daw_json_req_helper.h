// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/daw_json_link
//

#pragma once

#if defined( __cpp_lib_concepts )
#define DAW_JSON_MAKE_REQ_TRAIT( Name, /*ReqExpression*/... ) \
	template<typename T, typename = void>                       \
	inline constexpr bool Name = requires {                     \
		__VA_ARGS__;                                              \
	};
#else
#define DAW_JSON_MAKE_REQ_TRAIT( Name, /*ReqExpression*/... ) \
	template<typename, typename = void>                         \
	inline constexpr bool Name = false;                         \
                                                              \
	template<typename T>                                        \
	inline constexpr bool Name<T, std::void_t<decltype( __VA_ARGS__ )>> = true;
#endif

#if defined( __cpp_lib_concepts )
#define DAW_JSON_MAKE_REQ_TYPE_ALIAS_TRAIT( Name, /*MemberTypeAlias*/... ) \
	template<typename T, typename = void>                                    \
	inline constexpr bool Name = requires {                                  \
		typename __VA_ARGS__;                                                  \
	}
#else
#define DAW_JSON_MAKE_REQ_TYPE_ALIAS_TRAIT( Name, /*MemberTypeAlias*/... ) \
	template<typename, typename = void>                                      \
	inline constexpr bool Name = false;                                      \
                                                                           \
	template<typename T>                                                     \
	inline constexpr bool Name<T, std::void_t<typename __VA_ARGS__>> = true
#endif

