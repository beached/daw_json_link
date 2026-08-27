// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/daw_json_link
//

#pragma once

#include <daw/daw_cpp_feature_check.h>

#if __has_include( <simd> )
#include <simd>
#endif

#if defined( __cpp_lib_simd ) or defined( __glibcxx_simd )

#define DAW_JSON_HAS_STD_SIMD 1

#endif

#if ( defined( __ARM_NEON ) or defined( __ARM_NEON__ ) )
#include "daw/json/impl/daw_json_simd_neon.h"
#endif

#if defined( __SSE4_2__ ) or defined( __AVX__ ) or defined( __AVX2__ )
#include "daw/json/impl/daw_json_simd_sse42.h"
#endif

#if defined( DAW_JSON_HAS_STD_SIMD )

#define DAW_JSON_HAS_SIMD 1
#define DAW_JSON_SIMD_CONSTEXPR constexpr

namespace daw {
	namespace simd = std::simd;
}

#elif defined( DAW_JSON_HAS_NEON_SIMD )

#define DAW_JSON_HAS_SIMD 1
#define DAW_JSON_SIMD_CONSTEXPR inline

namespace daw {
	namespace simd = simd_impl::neon;
}

#elif defined( DAW_JSON_HAS_SSE42_SIMD )

#define DAW_JSON_HAS_SIMD 1
#define DAW_JSON_SIMD_CONSTEXPR inline

namespace daw {
	namespace simd = simd_impl::sse42;
}

#endif
