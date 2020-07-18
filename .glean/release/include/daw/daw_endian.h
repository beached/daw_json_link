// The MIT License (MIT)
//
// Copyright (c) 2018-2020 Darrell Wright
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files( the "Software" ), to
// deal in the Software without restriction, including without limitation the
// rights to use, copy, modify, merge, publish, distribute, sublicense, and / or
// sell copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#pragma once

#include "cpp_17.h"

#include <cstddef>
#include <cstdint>
#include <type_traits>
#include <utility>

namespace daw {
	enum class endian {
#if defined( _MSC_VER ) and not defined( __clang__ )
		little = 1234,
		big = 4321,
		native = little
#else
		little = __ORDER_LITTLE_ENDIAN__,
		big = __ORDER_BIG_ENDIAN__,
		native = __BYTE_ORDER__
#endif
	};

	namespace endian_details {
		template<typename T>
		constexpr T
		swap_bytes( T value,
		            std::integral_constant<size_t, sizeof( uint8_t )> ) noexcept {
			return value;
		}

		template<typename T>
		constexpr T
		swap_bytes( T value,
		            std::integral_constant<size_t, sizeof( uint16_t )> ) noexcept {
			return static_cast<T>(
			  static_cast<uint16_t>(
			    ( static_cast<uint16_t>( value ) >> static_cast<uint16_t>( 8U ) ) &
			    static_cast<uint16_t>( 0x00FFU ) ) |
			  static_cast<uint16_t>(
			    ( static_cast<uint16_t>( value ) << static_cast<uint16_t>( 8U ) ) &
			    static_cast<uint16_t>( 0xFF00U ) ) );
		}

		template<typename T>
		constexpr T
		swap_bytes( T value,
		            std::integral_constant<size_t, sizeof( uint32_t )> ) noexcept {
			return static_cast<T>(
			  static_cast<uint32_t>(
			    ( static_cast<uint32_t>( value ) >> static_cast<uint32_t>( 24U ) ) &
			    static_cast<uint32_t>( 0x0000'00FFU ) ) |
			  static_cast<uint32_t>(
			    ( static_cast<uint32_t>( value ) >> static_cast<uint32_t>( 8U ) ) &
			    static_cast<uint32_t>( 0x0000'FF00U ) ) |
			  static_cast<uint32_t>(
			    ( static_cast<uint32_t>( value ) << static_cast<uint32_t>( 8U ) ) &
			    static_cast<uint32_t>( 0x00FF'0000U ) ) |
			  static_cast<uint32_t>(
			    ( static_cast<uint32_t>( value ) << static_cast<uint32_t>( 24U ) ) &
			    static_cast<uint32_t>( 0xFF00'0000U ) ) );
		}

		template<typename T>
		constexpr T
		swap_bytes( T value,
		            std::integral_constant<size_t, sizeof( uint64_t )> ) noexcept {
			return static_cast<T>(
			  static_cast<uint64_t>(
			    ( static_cast<uint64_t>( value ) >> static_cast<uint64_t>( 56U ) ) &
			    static_cast<uint64_t>( 0x0000'0000'0000'00FFU ) ) |
			  static_cast<uint64_t>(
			    ( static_cast<uint64_t>( value ) >> static_cast<uint64_t>( 40U ) ) &
			    static_cast<uint64_t>( 0x0000'0000'0000'FF00U ) ) |
			  static_cast<uint64_t>(
			    ( static_cast<uint64_t>( value ) >> static_cast<uint64_t>( 24U ) ) &
			    static_cast<uint64_t>( 0x0000'0000'00FF'0000U ) ) |
			  static_cast<uint64_t>(
			    ( static_cast<uint64_t>( value ) >> static_cast<uint64_t>( 8U ) ) &
			    static_cast<uint64_t>( 0x0000'0000'FF00'0000U ) ) |
			  static_cast<uint64_t>(
			    ( static_cast<uint64_t>( value ) << static_cast<uint64_t>( 8U ) ) &
			    static_cast<uint64_t>( 0x0000'00FF'0000'0000U ) ) |
			  static_cast<uint64_t>(
			    ( static_cast<uint64_t>( value ) << static_cast<uint64_t>( 24U ) ) &
			    static_cast<uint64_t>( 0x0000'FF00'0000'0000U ) ) |
			  static_cast<uint64_t>(
			    ( static_cast<uint64_t>( value ) << static_cast<uint64_t>( 40U ) ) &
			    static_cast<uint64_t>( 0x00FF'0000'0000'0000U ) ) |
			  static_cast<uint64_t>(
			    ( static_cast<uint64_t>( value ) << static_cast<uint64_t>( 56U ) ) &
			    static_cast<uint64_t>( 0xFF00'0000'0000'0000U ) ) );
		}
	} // namespace endian_details

	template<typename T,
	         std::enable_if_t<std::is_integral_v<T>, std::nullptr_t> = nullptr>
	constexpr T to_little_endian( T value ) noexcept {
		if constexpr( endian::native == endian::little ) {
			return value;
		} else {
			return endian_details::swap_bytes(
			  value, std::integral_constant<size_t, sizeof( T )>{} );
		}
	}

	template<typename T,
	         std::enable_if_t<std::is_integral_v<T>, std::nullptr_t> = nullptr>
	constexpr T to_big_endian( T value ) noexcept {
		if constexpr( endian::native == endian::big ) {
			return value;
		} else {
			return endian_details::swap_bytes(
			  value, std::integral_constant<size_t, sizeof( T )>{} );
		}
	}

	template<typename T,
	         std::enable_if_t<std::is_integral_v<T>, std::nullptr_t> = nullptr>
	constexpr T network_to_host_endian( T value ) noexcept {
		if constexpr( endian::native == endian::big ) {
			return value;
		} else {
			return to_little_endian( value );
		}
	}

	template<endian SourceEndian, typename T,
	         std::enable_if_t<std::is_integral_v<T>, std::nullptr_t> = nullptr>
	constexpr T to_native_endian( T value ) noexcept {
		if constexpr( SourceEndian == endian::native ) {
			return value;
		} else if constexpr( endian::native == endian::little ) {
			return to_little_endian( value );
		} else {
			return to_big_endian( value );
		}
	}
} // namespace daw
