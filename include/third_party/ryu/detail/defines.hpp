// Copyright 2018 Ulf Adams
//
// The contents of this file may be used under the terms of the Apache License,
// Version 2.0.
//
//    (See accompanying file LICENSE-Apache or copy at
//     http://www.apache.org/licenses/LICENSE-2.0)
//
// Alternatively, the contents of this file may be used under the terms of
// the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE-Boost or copy at
//     https://www.boost.org/LICENSE_1_0.txt)
//
// Unless required by applicable law or agreed to in writing, this software
// is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
// KIND, either express or implied.

// Runtime compiler options:
// -DRYU_ONLY_64_BIT_OPS Avoid using uint128_t or 64-bit intrinsics. Slower,
//     depending on your compiler.
//
// -DRYU_OPTIMIZE_SIZE Use smaller lookup tables. Instead of storing every
//     required power of 5, only store every 26th entry, and compute
//     intermediate values with a multiplication. This reduces the lookup table
//     size by about 10x (only one case, and only double) at the cost of some
//     performance. Currently requires MSVC intrinsics.

/*
    This is a derivative work
*/

#pragma once

// copied from ABSL but allowing the define to be forced
#if defined( DAW_HAS_INT128 )
using uint128_t = __uint128_t;
#elif defined( __SIZEOF_INT128__ )
#if( defined( __clang__ ) && !defined( _WIN32 ) ) ||                           \
  ( defined( __CUDACC__ ) && __CUDACC_VER_MAJOR__ >= 9 ) ||                    \
  ( defined( __GNUC__ ) && !defined( __clang__ ) && !defined( __CUDACC__ ) )
#define HAS_UINT128
using uint128_t = __uint128_t;
#elif defined( __CUDACC__ )
// __CUDACC_VER__ is a full version number before CUDA 9, and is defined to a
// string explaining that it has been removed starting with CUDA 9. We use
// nested #ifs because there is no short-circuiting in the preprocessor.
// NOTE: `__CUDACC__` could be undefined while `__CUDACC_VER__` is defined.
#if __CUDACC_VER__ >= 70000
#define HAS_UINT128
using uint128_t = __uint128_t;
#endif // __CUDACC_VER__ >= 70000
#endif // defined(__CUDACC__)
#endif

#if not defined( HAS_UINT128 ) and defined( __SIZEOF_INT128__ ) and            \
  not defined( _MSC_VER )
#elif defined( _MSC_VER ) and not defined( RYU_ONLY_64_BIT_OPS ) and           \
  defined( _M_X64 )
#define DAW_JSON_RYU_HAS_64_BIT_INTRINSICS
#endif
