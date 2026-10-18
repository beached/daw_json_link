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
#include <daw/daw_span.h>

#include "daw/json/impl/version.h"

#include <cstddef>
#include <cstdint>
#include <cstring>
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

#if defined( __AVX2__ ) or defined( _M_AVX2 )
#include "daw/json/impl/daw_json_simd_avx2.h"
#elif defined( __SSE4_2__ ) or defined( __AVX__ ) or defined( _M_X64 ) or \
  ( defined( _M_IX86_FP ) and _M_IX86_FP >= 2 )
#include "daw/json/impl/daw_json_simd_sse42.h"
#endif

#if defined( DAW_JSON_HAS_STD_SIMD )

#define DAW_JSON_HAS_SIMD 1
#define DAW_JSON_SIMD_CONSTEXPR constexpr

#if defined( DAW_JSON_SIMD_MAX_256 )
// vec<T> is at most 256 bits, so a build with AVX-512 enabled stays in 256-bit
// registers. 512-bit instructions lower the clock on some CPUs, mostly older
// Xeons; where that costs less than the wider blocks save, as on Tiger Lake
// and Zen 4, leave this undefined. Narrower native widths are kept
namespace daw::simd {
	using namespace std::simd;

	template<typename T>
	inline constexpr int max_256_size_v =
	  static_cast<int>( std::simd::vec<T>::size( ) ) <
	      static_cast<int>( 32 / sizeof( T ) )
	    ? static_cast<int>( std::simd::vec<T>::size( ) )
	    : static_cast<int>( 32 / sizeof( T ) );

	template<typename T, int N = max_256_size_v<T>>
	using vec = std::simd::vec<T, N>;
} // namespace daw::simd
#else
namespace daw {
	namespace simd = std::simd;
}
#endif

#elif defined( DAW_JSON_HAS_NEON_SIMD )

#define DAW_JSON_HAS_SIMD 1
#define DAW_JSON_SIMD_CONSTEXPR inline

namespace daw {
	namespace simd = simd_impl::neon;
}

#elif defined( DAW_JSON_HAS_AVX2_SIMD )

#define DAW_JSON_HAS_SIMD 1
#define DAW_JSON_SIMD_CONSTEXPR inline

namespace daw {
	namespace simd = simd_impl::avx2;
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

// Scans load whole aligned blocks, which can extend past either end of the
// range being scanned.  An aligned load never crosses a page so this cannot
// fault, but sanitizers report the bytes outside the range.  Under a
// sanitizer only the bytes inside the range are copied to an aligned local
// block and loaded from there.
#if defined( __SANITIZE_ADDRESS__ ) or defined( __SANITIZE_HWADDRESS__ )
#define DAW_JSON_SIMD_SAFE_ALIGNED_LOADS 1
#elif defined( __has_feature )
#if __has_feature( address_sanitizer ) or                \
  __has_feature( hwaddress_sanitizer ) or __has_feature( memory_sanitizer )
#define DAW_JSON_SIMD_SAFE_ALIGNED_LOADS 1
#endif
#endif

namespace daw::json {
	inline namespace DAW_JSON_VER {
		namespace json_details {
			/// Load the aligned block at block_addr.  The block must hold at least
			/// one byte of [first_addr, last_addr), lanes outside of it have
			/// unspecified values and must be masked off by the caller.
			template<typename Simd>
			[[nodiscard]] inline Simd
			load_aligned_block( std::uintptr_t block_addr,
			                    [[maybe_unused]] std::uintptr_t first_addr,
			                    [[maybe_unused]] std::uintptr_t last_addr ) {
				using value_t = typename Simd::value_type;
				static_assert( sizeof( value_t ) == 1 );
				constexpr std::size_t block_size = Simd::size( );
#if defined( DAW_JSON_SIMD_SAFE_ALIGNED_LOADS )
				alignas( block_size ) value_t buff[block_size] = { };
				auto const lo = block_addr < first_addr ? first_addr : block_addr;
				auto const hi = last_addr < block_addr + block_size
				                  ? last_addr
				                  : block_addr + block_size;
				std::memcpy( buff + ( lo - block_addr ),
				             reinterpret_cast<void const *>( lo ), hi - lo );
				value_t const *const ptr = buff;
#else
				auto const *const ptr = reinterpret_cast<value_t const *>( block_addr );
#endif
				return daw::simd::unchecked_load<Simd>( daw::span( ptr, block_size ),
				                                        daw::simd::flag_aligned );
			}
		} // namespace json_details
	} // namespace DAW_JSON_VER
} // namespace daw::json

#if defined( DAW_JSON_SIMD_SAFE_ALIGNED_LOADS )
#undef DAW_JSON_SIMD_SAFE_ALIGNED_LOADS
#endif

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
