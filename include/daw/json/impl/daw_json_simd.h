// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/daw_json_link
//

#pragma once

#include <daw/daw_cpp_feature_check.h>
#include <daw/daw_is_constant_evaluated.h>

#include "daw/json/impl/version.h"

#include <cstdint>
#include <type_traits>

#if defined( __BMI2__ ) and ( defined( __x86_64__ ) or defined( _M_X64 ) )
#include <immintrin.h>
#endif

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

#if defined( DAW_JSON_HAS_SIMD )

namespace daw::simd_impl {
	/**
	 * Pack the bits selected by selection into the low bits of the result while
	 * preserving their lane order.  This is the scalar interface used after a
	 * SIMD comparison mask has been materialized.
	 */
	[[nodiscard]] constexpr std::uint64_t
	compress_bits( std::uint64_t values, std::uint64_t selection ) noexcept {
#if defined( __BMI2__ ) and ( defined( __x86_64__ ) or defined( _M_X64 ) )
		if( not DAW_IS_CONSTANT_EVALUATED_COMPAT( ) ) {
			return _pext_u64( values, selection );
		}
#endif
		auto result = std::uint64_t{ 0 };
		auto output_bit = std::uint64_t{ 1 };
		while( selection != 0 ) {
			auto const selected_bit = selection & ( ~selection + 1U );
			if( ( values & selected_bit ) != 0 ) {
				result |= output_bit;
			}
			selection &= selection - 1U;
			output_bit <<= 1U;
		}
		return result;
	}

	/**
	 * Accept the SIMD mask itself.  Keeping this overload in simd_impl lets the
	 * DAW backends add native-mask overloads without exposing their register
	 * types to JSON code.  Standard SIMD deliberately uses its public mask
	 * conversion because it has no portable native-register accessor.
	 */
	template<typename Mask,
	         std::enable_if_t<not std::is_integral_v<Mask>, int> = 0>
	[[nodiscard]] DAW_JSON_SIMD_CONSTEXPR std::uint64_t
	compress_bits( Mask values, std::uint64_t selection ) noexcept {
		return compress_bits( values.to_ullong( ), selection );
	}

	template<typename Mask,
	         std::enable_if_t<not std::is_integral_v<Mask>, int> = 0>
	[[nodiscard]] DAW_JSON_SIMD_CONSTEXPR std::uint64_t
	compress_bits( Mask values, Mask selection ) noexcept {
		return compress_bits( values.to_ullong( ), selection.to_ullong( ) );
	}
} // namespace daw::simd_impl

namespace daw::json {
	inline namespace DAW_JSON_VER {
		namespace json_details {
			struct number_span {
				char const *first = nullptr;
				char const *last = nullptr;
				char const *decimal_point = nullptr;
				char const *exponent_marker = nullptr;
			};

			struct pending_number_span {
				char const *first = nullptr;
				char const *decimal_point = nullptr;
				char const *exponent_marker = nullptr;
			};

			struct integer_span {
				char const *first = nullptr;
				char const *last = nullptr;
			};

			struct pending_integer_span {
				char const *first = nullptr;
			};
		} // namespace json_details
	} // namespace DAW_JSON_VER
} // namespace daw::json

#endif
