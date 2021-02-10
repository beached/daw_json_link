// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/
//

#pragma once

#if defined( __GNUC__ ) or defined( __clang__ )
#define DAW_JSON_LIKELY( Bool ) ( __builtin_expect( !!( Bool ), 1 ) )
#define DAW_JSON_UNLIKELY( Bool ) ( __builtin_expect( !!( Bool ), 0 ) )
#define DAW_JSON_NOINLINE [[gnu::noinline]]
// __attribute__( ( noinline ) )
#elif defined( _MSC_VER )
#define DAW_JSON_LIKELY( Bool ) !!( Bool )
#define DAW_JSON_UNLIKELY( Bool ) !!( Bool )
#define DAW_JSON_NOINLINE __declspec( noinline )
#else
#define DAW_JSON_LIKELY( Bool ) !!( Bool )
#define DAW_JSON_UNLIKELY( Bool ) !!( Bool )
#define DAW_JSON_NOINLINE [[gnu::noinline]]
#endif

#if not defined( DAW_NO_FLATTEN ) and not defined( _MSC_VER )
#define DAW_ONLY_FLATTEN [[gnu::flatten]]
#define DAW_ONLY_INLINE [[gnu::always_inline]]
#else
#define DAW_ONLY_FLATTEN
#define DAW_ONLY_INLINE
#endif
