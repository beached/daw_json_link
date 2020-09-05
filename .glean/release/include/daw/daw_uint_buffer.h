// The MIT License (MIT)
//
// Copyright (c) Darrell Wright
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

#include <cassert>
#include <climits>
#include <cstdint>
#include <limits>
#include <type_traits>
#include <utility>

#include <daw/daw_arith_traits.h>

namespace daw {
	struct InvalidUIntSize;
	enum class UInt64 : std::uint64_t {};
	enum class UInt32 : std::uint32_t {};
	enum class UInt16 : std::uint16_t {};
	enum class UInt8 : std::uint8_t {};

	template<typename Number>
	constexpr UInt64 to_uint64( Number num ) {
		static_assert( daw::is_arithmetic_v<Number> );
		return static_cast<UInt64>( num );
	}

	template<typename Number>
	constexpr UInt32 to_uint32( Number num ) {
		static_assert( daw::is_arithmetic_v<Number> );
		return static_cast<UInt32>( num );
	}

	template<typename Number>
	constexpr UInt16 to_uint16( Number num ) {
		static_assert( daw::is_arithmetic_v<Number> );
		return static_cast<UInt16>( num );
	}

	template<typename Number>
	constexpr UInt8 to_uint8( Number num ) {
		static_assert( daw::is_arithmetic_v<Number> );
		return static_cast<UInt8>( num );
	}

	template<unsigned Bits>
	using UInt = std::conditional_t<
	  ( Bits == 8 ), UInt8,
	  std::conditional_t<
	    ( Bits == 16 ), UInt16,
	    std::conditional_t<
	      ( Bits == 32 ), UInt32,
	      std::conditional_t<( Bits == 64 ), UInt64, InvalidUIntSize>>>>;

	constexpr UInt64 &operator<<=( UInt64 &b, std::uint64_t shift ) noexcept {
		auto tmp = static_cast<std::uint64_t>( b );
		tmp <<= static_cast<std::uint64_t>( shift );
		b = static_cast<UInt64>( tmp );
		return b;
	}

	constexpr UInt64 &operator<<=( UInt64 &b, UInt64 shift ) noexcept {
		auto tmp = static_cast<std::uint64_t>( b );
		tmp <<= static_cast<std::uint64_t>( shift );
		b = static_cast<UInt64>( tmp );
		return b;
	}

	constexpr UInt64 &operator<<=( UInt64 &b, UInt32 shift ) noexcept {
		auto tmp = static_cast<std::uint64_t>( b );
		tmp <<= static_cast<std::uint64_t>( shift );
		b = static_cast<UInt64>( tmp );
		return b;
	}

	constexpr UInt64 &operator<<=( UInt64 &b, UInt16 shift ) noexcept {
		auto tmp = static_cast<std::uint64_t>( b );
		tmp <<= static_cast<std::uint64_t>( shift );
		b = static_cast<UInt64>( tmp );
		return b;
	}

	constexpr UInt64 &operator<<=( UInt64 &b, UInt8 shift ) noexcept {
		auto tmp = static_cast<std::uint64_t>( b );
		tmp <<= static_cast<std::uint64_t>( shift );
		b = static_cast<UInt64>( tmp );
		return b;
	}

	constexpr UInt64 &operator>>=( UInt64 &b, std::uint64_t shift ) noexcept {
		auto tmp = static_cast<std::uint64_t>( b );
		tmp >>= static_cast<std::uint64_t>( shift );
		b = static_cast<UInt64>( tmp );
		return b;
	}

	constexpr UInt64 &operator>>=( UInt64 &b, UInt64 shift ) noexcept {
		auto tmp = static_cast<std::uint64_t>( b );
		tmp >>= static_cast<std::uint64_t>( shift );
		b = static_cast<UInt64>( tmp );
		return b;
	}

	constexpr UInt64 &operator>>=( UInt64 &b, UInt32 shift ) noexcept {
		auto tmp = static_cast<std::uint64_t>( b );
		tmp >>= static_cast<std::uint64_t>( shift );
		b = static_cast<UInt64>( tmp );
		return b;
	}

	constexpr UInt64 &operator>>=( UInt64 &b, UInt16 shift ) noexcept {
		auto tmp = static_cast<std::uint64_t>( b );
		tmp >>= static_cast<std::uint64_t>( shift );
		b = static_cast<UInt64>( tmp );
		return b;
	}

	constexpr UInt64 &operator>>=( UInt64 &b, UInt8 shift ) noexcept {
		auto tmp = static_cast<std::uint64_t>( b );
		tmp >>= static_cast<std::uint64_t>( shift );
		b = static_cast<UInt64>( tmp );
		return b;
	}

	constexpr UInt64 operator<<( UInt64 b, std::uint64_t shift ) noexcept {
		auto tmp = static_cast<std::uint64_t>( b );
		tmp <<= static_cast<std::uint64_t>( shift );
		return static_cast<UInt64>( tmp );
	}

	constexpr UInt64 operator<<( UInt64 b, UInt64 shift ) noexcept {
		auto tmp = static_cast<std::uint64_t>( b );
		tmp <<= static_cast<std::uint64_t>( shift );
		return static_cast<UInt64>( tmp );
	}

	constexpr UInt64 operator<<( UInt64 b, UInt32 shift ) noexcept {
		auto tmp = static_cast<std::uint64_t>( b );
		tmp <<= static_cast<std::uint64_t>( shift );
		return static_cast<UInt64>( tmp );
	}

	constexpr UInt64 operator<<( UInt64 b, UInt16 shift ) noexcept {
		auto tmp = static_cast<std::uint64_t>( b );
		tmp <<= static_cast<std::uint64_t>( shift );
		return static_cast<UInt64>( tmp );
	}

	constexpr UInt64 operator<<( UInt64 b, UInt8 shift ) noexcept {
		auto tmp = static_cast<std::uint64_t>( b );
		tmp <<= static_cast<std::uint64_t>( shift );
		return static_cast<UInt64>( tmp );
	}

	constexpr std::uint64_t operator<<( std::uint64_t b, UInt64 shift ) noexcept {
		b <<= static_cast<std::uint64_t>( shift );
		return b;
	}

	constexpr std::uint64_t operator<<( std::uint64_t b, UInt32 shift ) noexcept {
		b <<= static_cast<std::uint64_t>( shift );
		return b;
	}

	constexpr std::uint64_t operator<<( std::uint64_t b, UInt16 shift ) noexcept {
		b <<= static_cast<std::uint64_t>( shift );
		return b;
	}

	constexpr std::uint64_t operator<<( std::uint64_t b, UInt8 shift ) noexcept {
		b <<= static_cast<std::uint64_t>( shift );
		return b;
	}

	constexpr UInt64 operator>>( UInt64 b, std::uint64_t shift ) noexcept {
		auto tmp = static_cast<std::uint64_t>( b );
		tmp >>= static_cast<std::uint64_t>( shift );
		return static_cast<UInt64>( tmp );
	}

	constexpr UInt64 operator>>( UInt64 b, UInt64 shift ) noexcept {
		auto tmp = static_cast<std::uint64_t>( b );
		tmp >>= static_cast<std::uint64_t>( shift );
		return static_cast<UInt64>( tmp );
	}

	constexpr UInt64 operator>>( UInt64 b, UInt32 shift ) noexcept {
		auto tmp = static_cast<std::uint64_t>( b );
		tmp >>= static_cast<std::uint64_t>( shift );
		return static_cast<UInt64>( tmp );
	}

	constexpr UInt64 operator>>( UInt64 b, UInt16 shift ) noexcept {
		auto tmp = static_cast<std::uint64_t>( b );
		tmp >>= static_cast<std::uint64_t>( shift );
		return static_cast<UInt64>( tmp );
	}

	constexpr UInt64 operator>>( UInt64 b, UInt8 shift ) noexcept {
		auto tmp = static_cast<std::uint64_t>( b );
		tmp >>= static_cast<std::uint64_t>( shift );
		return static_cast<UInt64>( tmp );
	}

	constexpr std::uint64_t operator>>( std::uint64_t b, UInt64 shift ) noexcept {
		b >>= static_cast<std::uint64_t>( shift );
		return b;
	}

	constexpr std::uint64_t operator>>( std::uint64_t b, UInt32 shift ) noexcept {
		b >>= static_cast<std::uint64_t>( shift );
		return b;
	}

	constexpr std::uint64_t operator>>( std::uint64_t b, UInt16 shift ) noexcept {
		b >>= static_cast<std::uint64_t>( shift );
		return b;
	}

	constexpr std::uint64_t operator>>( std::uint64_t b, UInt8 shift ) noexcept {
		b >>= static_cast<std::uint64_t>( shift );
		return b;
	}

	constexpr UInt64 &operator|=( UInt64 &lhs, std::uint64_t rhs ) noexcept {
		auto tmp = static_cast<std::uint64_t>( lhs );
		tmp |= static_cast<std::uint64_t>( rhs );
		lhs = static_cast<UInt64>( tmp );
		return lhs;
	}

	constexpr UInt64 &operator|=( UInt64 &lhs, UInt64 rhs ) noexcept {
		auto tmp = static_cast<std::uint64_t>( lhs );
		tmp |= static_cast<std::uint64_t>( rhs );
		lhs = static_cast<UInt64>( tmp );
		return lhs;
	}

	constexpr UInt64 &operator|=( UInt64 &lhs, UInt32 rhs ) noexcept {
		auto tmp = static_cast<std::uint64_t>( lhs );
		tmp |= static_cast<std::uint64_t>( rhs );
		lhs = static_cast<UInt64>( tmp );
		return lhs;
	}

	constexpr UInt64 &operator|=( UInt64 &lhs, UInt16 rhs ) noexcept {
		auto tmp = static_cast<std::uint64_t>( lhs );
		tmp |= static_cast<std::uint64_t>( rhs );
		lhs = static_cast<UInt64>( tmp );
		return lhs;
	}

	constexpr UInt64 &operator|=( UInt64 &lhs, UInt8 rhs ) noexcept {
		auto tmp = static_cast<std::uint64_t>( lhs );
		tmp |= static_cast<std::uint64_t>( rhs );
		lhs = static_cast<UInt64>( tmp );
		return lhs;
	}

	constexpr UInt64 &operator&=( UInt64 &lhs, std::uint64_t rhs ) noexcept {
		auto tmp = static_cast<std::uint64_t>( lhs );
		tmp &= static_cast<std::uint64_t>( rhs );
		lhs = static_cast<UInt64>( tmp );
		return lhs;
	}

	constexpr UInt64 &operator&=( UInt64 &lhs, UInt64 rhs ) noexcept {
		auto tmp = static_cast<std::uint64_t>( lhs );
		tmp &= static_cast<std::uint64_t>( rhs );
		lhs = static_cast<UInt64>( tmp );
		return lhs;
	}

	constexpr UInt64 &operator&=( UInt64 &lhs, UInt32 rhs ) noexcept {
		auto tmp = static_cast<std::uint64_t>( lhs );
		tmp &= static_cast<std::uint64_t>( rhs );
		lhs = static_cast<UInt64>( tmp );
		return lhs;
	}

	constexpr UInt64 &operator&=( UInt64 &lhs, UInt16 rhs ) noexcept {
		auto tmp = static_cast<std::uint64_t>( lhs );
		tmp &= static_cast<std::uint64_t>( rhs );
		lhs = static_cast<UInt64>( tmp );
		return lhs;
	}

	constexpr UInt64 &operator&=( UInt64 &lhs, UInt8 rhs ) noexcept {
		auto tmp = static_cast<std::uint64_t>( lhs );
		tmp &= static_cast<std::uint64_t>( rhs );
		lhs = static_cast<UInt64>( tmp );
		return lhs;
	}

	constexpr UInt64 &operator^=( UInt64 &lhs, std::uint64_t rhs ) noexcept {
		auto tmp = static_cast<std::uint64_t>( lhs );
		tmp ^= static_cast<std::uint64_t>( rhs );
		lhs = static_cast<UInt64>( tmp );
		return lhs;
	}

	constexpr UInt64 &operator^=( UInt64 &lhs, UInt64 rhs ) noexcept {
		auto tmp = static_cast<std::uint64_t>( lhs );
		tmp ^= static_cast<std::uint64_t>( rhs );
		lhs = static_cast<UInt64>( tmp );
		return lhs;
	}

	constexpr UInt64 &operator^=( UInt64 &lhs, UInt32 rhs ) noexcept {
		auto tmp = static_cast<std::uint64_t>( lhs );
		tmp ^= static_cast<std::uint64_t>( rhs );
		lhs = static_cast<UInt64>( tmp );
		return lhs;
	}

	constexpr UInt64 &operator^=( UInt64 &lhs, UInt16 rhs ) noexcept {
		auto tmp = static_cast<std::uint64_t>( lhs );
		tmp ^= static_cast<std::uint64_t>( rhs );
		lhs = static_cast<UInt64>( tmp );
		return lhs;
	}

	constexpr UInt64 &operator^=( UInt64 &lhs, UInt8 rhs ) noexcept {
		auto tmp = static_cast<std::uint64_t>( lhs );
		tmp ^= static_cast<std::uint64_t>( rhs );
		lhs = static_cast<UInt64>( tmp );
		return lhs;
	}

	constexpr UInt64 operator|( UInt64 lhs, std::uint64_t rhs ) noexcept {
		return static_cast<UInt64>( static_cast<std::uint64_t>( lhs ) |
		                            static_cast<std::uint64_t>( rhs ) );
	}

	constexpr UInt64 operator|( std::uint64_t lhs, UInt64 rhs ) noexcept {
		return static_cast<UInt64>( static_cast<std::uint64_t>( lhs ) |
		                            static_cast<std::uint64_t>( rhs ) );
	}

	constexpr UInt64 operator&( UInt64 lhs, std::uint64_t rhs ) noexcept {
		return static_cast<UInt64>( static_cast<std::uint64_t>( lhs ) &
		                            static_cast<std::uint64_t>( rhs ) );
	}

	constexpr UInt64 operator&( std::uint64_t lhs, UInt64 rhs ) noexcept {
		return static_cast<UInt64>( static_cast<std::uint64_t>( lhs ) &
		                            static_cast<std::uint64_t>( rhs ) );
	}

	constexpr UInt64 operator^( UInt64 lhs, std::uint64_t rhs ) noexcept {
		return static_cast<UInt64>( static_cast<std::uint64_t>( lhs ) ^
		                            static_cast<std::uint64_t>( rhs ) );
	}

	constexpr UInt64 operator^( std::uint64_t lhs, UInt64 rhs ) noexcept {
		return static_cast<UInt64>( static_cast<std::uint64_t>( lhs ) ^
		                            static_cast<std::uint64_t>( rhs ) );
	}

	constexpr bool operator==( UInt64 lhs, std::uint64_t rhs ) noexcept {
		return static_cast<std::uint64_t>( lhs ) ==
		       static_cast<std::uint64_t>( rhs );
	}

	constexpr bool operator==( UInt64 lhs, UInt32 rhs ) noexcept {
		return static_cast<std::uint64_t>( lhs ) ==
		       static_cast<std::uint64_t>( rhs );
	}

	constexpr bool operator==( UInt64 lhs, UInt16 rhs ) noexcept {
		return static_cast<std::uint64_t>( lhs ) ==
		       static_cast<std::uint64_t>( rhs );
	}

	constexpr bool operator==( UInt64 lhs, UInt8 rhs ) noexcept {
		return static_cast<std::uint64_t>( lhs ) ==
		       static_cast<std::uint64_t>( rhs );
	}

	constexpr bool operator==( std::uint64_t lhs, UInt64 rhs ) noexcept {
		return static_cast<std::uint64_t>( lhs ) ==
		       static_cast<std::uint64_t>( rhs );
	}

	constexpr bool operator==( std::uint64_t lhs, UInt32 rhs ) noexcept {
		return static_cast<std::uint64_t>( lhs ) ==
		       static_cast<std::uint64_t>( rhs );
	}

	constexpr bool operator==( std::uint64_t lhs, UInt16 rhs ) noexcept {
		return static_cast<std::uint64_t>( lhs ) ==
		       static_cast<std::uint64_t>( rhs );
	}

	constexpr bool operator==( std::uint64_t lhs, UInt8 rhs ) noexcept {
		return static_cast<std::uint64_t>( lhs ) ==
		       static_cast<std::uint64_t>( rhs );
	}

	constexpr bool operator!=( UInt64 lhs, std::uint64_t rhs ) noexcept {
		return static_cast<std::uint64_t>( lhs ) !=
		       static_cast<std::uint64_t>( rhs );
	}

	constexpr bool operator!=( std::uint64_t lhs, UInt64 rhs ) noexcept {
		return static_cast<std::uint64_t>( lhs ) !=
		       static_cast<std::uint64_t>( rhs );
	}

	constexpr bool operator>( UInt64 lhs, std::uint64_t rhs ) noexcept {
		return static_cast<std::uint64_t>( lhs ) >
		       static_cast<std::uint64_t>( rhs );
	}

	constexpr bool operator>( UInt32 lhs, std::uint64_t rhs ) noexcept {
		return static_cast<std::uint64_t>( lhs ) >
		       static_cast<std::uint64_t>( rhs );
	}

	constexpr bool operator>( UInt16 lhs, std::uint64_t rhs ) noexcept {
		return static_cast<std::uint64_t>( lhs ) >
		       static_cast<std::uint64_t>( rhs );
	}

	constexpr bool operator>( UInt8 lhs, std::uint64_t rhs ) noexcept {
		return static_cast<std::uint64_t>( lhs ) >
		       static_cast<std::uint64_t>( rhs );
	}

	constexpr bool operator>( std::uint64_t lhs, UInt64 rhs ) noexcept {
		return static_cast<std::uint64_t>( lhs ) >
		       static_cast<std::uint64_t>( rhs );
	}

	constexpr bool operator>( std::uint64_t lhs, UInt32 rhs ) noexcept {
		return static_cast<std::uint64_t>( lhs ) >
		       static_cast<std::uint64_t>( rhs );
	}

	constexpr bool operator>( std::uint64_t lhs, UInt16 rhs ) noexcept {
		return static_cast<std::uint64_t>( lhs ) >
		       static_cast<std::uint64_t>( rhs );
	}

	constexpr bool operator>( std::uint64_t lhs, UInt8 rhs ) noexcept {
		return static_cast<std::uint64_t>( lhs ) >
		       static_cast<std::uint64_t>( rhs );
	}

	constexpr bool operator>=( UInt64 lhs, std::uint64_t rhs ) noexcept {
		return static_cast<std::uint64_t>( lhs ) >=
		       static_cast<std::uint64_t>( rhs );
	}

	constexpr bool operator>=( UInt32 lhs, std::uint64_t rhs ) noexcept {
		return static_cast<std::uint64_t>( lhs ) >=
		       static_cast<std::uint64_t>( rhs );
	}

	constexpr bool operator>=( UInt16 lhs, std::uint64_t rhs ) noexcept {
		return static_cast<std::uint64_t>( lhs ) >=
		       static_cast<std::uint64_t>( rhs );
	}

	constexpr bool operator>=( UInt8 lhs, std::uint64_t rhs ) noexcept {
		return static_cast<std::uint64_t>( lhs ) >=
		       static_cast<std::uint64_t>( rhs );
	}

	constexpr bool operator>=( std::uint64_t lhs, UInt64 rhs ) noexcept {
		return static_cast<std::uint64_t>( lhs ) >=
		       static_cast<std::uint64_t>( rhs );
	}

	constexpr bool operator>=( std::uint64_t lhs, UInt32 rhs ) noexcept {
		return static_cast<std::uint64_t>( lhs ) >=
		       static_cast<std::uint64_t>( rhs );
	}

	constexpr bool operator>=( std::uint64_t lhs, UInt16 rhs ) noexcept {
		return static_cast<std::uint64_t>( lhs ) >=
		       static_cast<std::uint64_t>( rhs );
	}

	constexpr bool operator>=( std::uint64_t lhs, UInt8 rhs ) noexcept {
		return static_cast<std::uint64_t>( lhs ) >=
		       static_cast<std::uint64_t>( rhs );
	}

	constexpr bool operator<( UInt64 lhs, std::uint64_t rhs ) noexcept {
		return static_cast<std::uint64_t>( lhs ) <
		       static_cast<std::uint64_t>( rhs );
	}

	constexpr bool operator<( std::uint64_t lhs, UInt64 rhs ) noexcept {
		return static_cast<std::uint64_t>( lhs ) <
		       static_cast<std::uint64_t>( rhs );
	}

	constexpr bool operator<( std::uint64_t lhs, UInt32 rhs ) noexcept {
		return static_cast<std::uint64_t>( lhs ) <
		       static_cast<std::uint64_t>( rhs );
	}

	constexpr bool operator<( std::uint64_t lhs, UInt16 rhs ) noexcept {
		return static_cast<std::uint64_t>( lhs ) <
		       static_cast<std::uint64_t>( rhs );
	}

	constexpr bool operator<( std::uint64_t lhs, UInt8 rhs ) noexcept {
		return static_cast<std::uint64_t>( lhs ) <
		       static_cast<std::uint64_t>( rhs );
	}

	constexpr bool operator<=( UInt64 lhs, std::uint64_t rhs ) noexcept {
		return static_cast<std::uint64_t>( lhs ) <=
		       static_cast<std::uint64_t>( rhs );
	}

	constexpr bool operator<=( UInt32 lhs, std::uint64_t rhs ) noexcept {
		return static_cast<std::uint64_t>( lhs ) <=
		       static_cast<std::uint64_t>( rhs );
	}

	constexpr bool operator<=( UInt16 lhs, std::uint64_t rhs ) noexcept {
		return static_cast<std::uint64_t>( lhs ) <=
		       static_cast<std::uint64_t>( rhs );
	}

	constexpr bool operator<=( UInt8 lhs, std::uint64_t rhs ) noexcept {
		return static_cast<std::uint64_t>( lhs ) <=
		       static_cast<std::uint64_t>( rhs );
	}

	constexpr bool operator<=( std::uint64_t lhs, UInt64 rhs ) noexcept {
		return static_cast<std::uint64_t>( lhs ) <=
		       static_cast<std::uint64_t>( rhs );
	}

	constexpr bool operator<=( std::uint64_t lhs, UInt32 rhs ) noexcept {
		return static_cast<std::uint64_t>( lhs ) <=
		       static_cast<std::uint64_t>( rhs );
	}

	constexpr bool operator<=( std::uint64_t lhs, UInt16 rhs ) noexcept {
		return static_cast<std::uint64_t>( lhs ) <=
		       static_cast<std::uint64_t>( rhs );
	}

	constexpr bool operator<=( std::uint64_t lhs, UInt8 rhs ) noexcept {
		return static_cast<std::uint64_t>( lhs ) <=
		       static_cast<std::uint64_t>( rhs );
	}

	constexpr UInt64 operator|( UInt64 lhs, UInt64 rhs ) noexcept {
		return static_cast<UInt64>( static_cast<std::uint64_t>( lhs ) |
		                            static_cast<std::uint64_t>( rhs ) );
	}

	constexpr UInt64 operator|( UInt64 lhs, UInt32 rhs ) noexcept {
		return static_cast<UInt64>( static_cast<std::uint64_t>( lhs ) |
		                            static_cast<std::uint64_t>( rhs ) );
	}

	constexpr UInt64 operator|( UInt64 lhs, UInt16 rhs ) noexcept {
		return static_cast<UInt64>( static_cast<std::uint64_t>( lhs ) |
		                            static_cast<std::uint64_t>( rhs ) );
	}

	constexpr UInt64 operator|( UInt64 lhs, UInt8 rhs ) noexcept {
		return static_cast<UInt64>( static_cast<std::uint64_t>( lhs ) |
		                            static_cast<std::uint64_t>( rhs ) );
	}

	constexpr UInt64 operator|( UInt32 lhs, UInt64 rhs ) noexcept {
		return static_cast<UInt64>( static_cast<std::uint64_t>( lhs ) |
		                            static_cast<std::uint64_t>( rhs ) );
	}

	constexpr UInt64 operator|( UInt16 lhs, UInt64 rhs ) noexcept {
		return static_cast<UInt64>( static_cast<std::uint64_t>( lhs ) |
		                            static_cast<std::uint64_t>( rhs ) );
	}

	constexpr UInt64 operator|( UInt8 lhs, UInt64 rhs ) noexcept {
		return static_cast<UInt64>( static_cast<std::uint64_t>( lhs ) |
		                            static_cast<std::uint64_t>( rhs ) );
	}

	constexpr UInt64 operator&( UInt64 lhs, UInt64 rhs ) noexcept {
		return static_cast<UInt64>( static_cast<std::uint64_t>( lhs ) &
		                            static_cast<std::uint64_t>( rhs ) );
	}

	constexpr UInt64 operator&( UInt64 lhs, UInt32 rhs ) noexcept {
		return static_cast<UInt64>( static_cast<std::uint64_t>( lhs ) &
		                            static_cast<std::uint64_t>( rhs ) );
	}

	constexpr UInt64 operator&( UInt64 lhs, UInt16 rhs ) noexcept {
		return static_cast<UInt64>( static_cast<std::uint64_t>( lhs ) &
		                            static_cast<std::uint64_t>( rhs ) );
	}

	constexpr UInt64 operator&( UInt64 lhs, UInt8 rhs ) noexcept {
		return static_cast<UInt64>( static_cast<std::uint64_t>( lhs ) &
		                            static_cast<std::uint64_t>( rhs ) );
	}

	constexpr UInt64 operator&( UInt32 lhs, UInt64 rhs ) noexcept {
		return static_cast<UInt64>( static_cast<std::uint64_t>( lhs ) &
		                            static_cast<std::uint64_t>( rhs ) );
	}

	constexpr UInt64 operator&( UInt16 lhs, UInt64 rhs ) noexcept {
		return static_cast<UInt64>( static_cast<std::uint64_t>( lhs ) &
		                            static_cast<std::uint64_t>( rhs ) );
	}

	constexpr UInt64 operator&( UInt8 lhs, UInt64 rhs ) noexcept {
		return static_cast<UInt64>( static_cast<std::uint64_t>( lhs ) &
		                            static_cast<std::uint64_t>( rhs ) );
	}

	constexpr UInt64 operator^( UInt64 lhs, UInt64 rhs ) noexcept {
		return static_cast<UInt64>( static_cast<std::uint64_t>( lhs ) ^
		                            static_cast<std::uint64_t>( rhs ) );
	}

	constexpr UInt64 operator^( UInt64 lhs, UInt32 rhs ) noexcept {
		return static_cast<UInt64>( static_cast<std::uint64_t>( lhs ) ^
		                            static_cast<std::uint64_t>( rhs ) );
	}

	constexpr UInt64 operator^( UInt64 lhs, UInt16 rhs ) noexcept {
		return static_cast<UInt64>( static_cast<std::uint64_t>( lhs ) ^
		                            static_cast<std::uint64_t>( rhs ) );
	}

	constexpr UInt64 operator^( UInt64 lhs, UInt8 rhs ) noexcept {
		return static_cast<UInt64>( static_cast<std::uint64_t>( lhs ) ^
		                            static_cast<std::uint64_t>( rhs ) );
	}

	constexpr UInt64 operator^( UInt32 lhs, UInt64 rhs ) noexcept {
		return static_cast<UInt64>( static_cast<std::uint64_t>( lhs ) ^
		                            static_cast<std::uint64_t>( rhs ) );
	}

	constexpr UInt64 operator^( UInt16 lhs, UInt64 rhs ) noexcept {
		return static_cast<UInt64>( static_cast<std::uint64_t>( lhs ) ^
		                            static_cast<std::uint64_t>( rhs ) );
	}

	constexpr UInt64 operator^( UInt8 lhs, UInt64 rhs ) noexcept {
		return static_cast<UInt64>( static_cast<std::uint64_t>( lhs ) ^
		                            static_cast<std::uint64_t>( rhs ) );
	}

	template<unsigned bits>
	constexpr UInt64 rotate_left( UInt64 value ) {
		static_assert( bits <= 64 );
		return ( value << bits ) | ( value >> ( 64U - bits ) );
	}

	constexpr UInt64 rotate_left( UInt64 value, unsigned bits ) {
		return ( value << bits ) | ( value >> ( 64U - bits ) );
	}

	template<unsigned bits>
	constexpr UInt64 rotate_right( UInt64 value ) {
		static_assert( bits <= 64 );
		return ( value >> bits ) | ( value << ( 64U - bits ) );
	}

	constexpr UInt64 rotate_right( UInt64 value, unsigned bits ) {
		return ( value >> bits ) | ( value << ( 64U - bits ) );
	}

	constexpr UInt32 &operator<<=( UInt32 &b, std::uint32_t shift ) noexcept {

		auto tmp = static_cast<std::uint32_t>( b );
		tmp <<= static_cast<std::uint32_t>( shift );
		b = static_cast<UInt32>( tmp );
		return b;
	}

	constexpr UInt32 &operator<<=( UInt32 &b, UInt32 shift ) noexcept {
		auto tmp = static_cast<std::uint32_t>( b );
		tmp <<= static_cast<std::uint32_t>( shift );
		b = static_cast<UInt32>( tmp );
		return b;
	}

	constexpr UInt32 &operator<<=( UInt32 &b, UInt16 shift ) noexcept {
		auto tmp = static_cast<std::uint32_t>( b );
		tmp <<= static_cast<std::uint32_t>( shift );
		b = static_cast<UInt32>( tmp );
		return b;
	}

	constexpr UInt32 &operator<<=( UInt32 &b, UInt8 shift ) noexcept {
		auto tmp = static_cast<std::uint32_t>( b );
		tmp <<= static_cast<std::uint32_t>( shift );
		b = static_cast<UInt32>( tmp );
		return b;
	}

	constexpr UInt32 &operator>>=( UInt32 &b, std::uint32_t shift ) noexcept {

		auto tmp = static_cast<std::uint32_t>( b );
		tmp >>= static_cast<std::uint32_t>( shift );
		b = static_cast<UInt32>( tmp );
		return b;
	}

	constexpr UInt32 &operator>>=( UInt32 &b, UInt32 shift ) noexcept {
		auto tmp = static_cast<std::uint32_t>( b );
		tmp >>= static_cast<std::uint32_t>( shift );
		b = static_cast<UInt32>( tmp );
		return b;
	}

	constexpr UInt32 &operator>>=( UInt32 &b, UInt16 shift ) noexcept {
		auto tmp = static_cast<std::uint32_t>( b );
		tmp >>= static_cast<std::uint32_t>( shift );
		b = static_cast<UInt32>( tmp );
		return b;
	}

	constexpr UInt32 &operator>>=( UInt32 &b, UInt8 shift ) noexcept {
		auto tmp = static_cast<std::uint32_t>( b );
		tmp >>= static_cast<std::uint32_t>( shift );
		b = static_cast<UInt32>( tmp );
		return b;
	}

	constexpr UInt32 operator<<( UInt32 b, std::uint32_t shift ) noexcept {

		auto tmp = static_cast<std::uint32_t>( b );
		tmp <<= static_cast<std::uint32_t>( shift );
		return static_cast<UInt32>( tmp );
	}

	constexpr UInt32 operator<<( UInt32 b, UInt32 shift ) noexcept {
		auto tmp = static_cast<std::uint32_t>( b );
		tmp <<= static_cast<std::uint32_t>( shift );
		return static_cast<UInt32>( tmp );
	}

	constexpr UInt32 operator<<( UInt32 b, UInt16 shift ) noexcept {
		auto tmp = static_cast<std::uint32_t>( b );
		tmp <<= static_cast<std::uint32_t>( shift );
		return static_cast<UInt32>( tmp );
	}

	constexpr UInt32 operator<<( UInt32 b, UInt8 shift ) noexcept {
		auto tmp = static_cast<std::uint32_t>( b );
		tmp <<= static_cast<std::uint32_t>( shift );
		return static_cast<UInt32>( tmp );
	}

	constexpr std::uint32_t operator<<( std::uint32_t b, UInt32 shift ) noexcept {

		b <<= static_cast<std::uint32_t>( shift );
		return b;
	}

	constexpr UInt32 operator>>( UInt32 b, std::uint32_t shift ) noexcept {

		auto tmp = static_cast<std::uint32_t>( b );
		tmp >>= static_cast<std::uint32_t>( shift );
		return static_cast<UInt32>( tmp );
	}

	constexpr UInt32 operator>>( UInt32 b, UInt32 shift ) noexcept {
		auto tmp = static_cast<std::uint32_t>( b );
		tmp >>= static_cast<std::uint32_t>( shift );
		return static_cast<UInt32>( tmp );
	}
	constexpr UInt32 operator>>( UInt32 b, UInt16 shift ) noexcept {
		auto tmp = static_cast<std::uint32_t>( b );
		tmp >>= static_cast<std::uint32_t>( shift );
		return static_cast<UInt32>( tmp );
	}
	constexpr UInt32 operator>>( UInt32 b, UInt8 shift ) noexcept {
		auto tmp = static_cast<std::uint32_t>( b );
		tmp >>= static_cast<std::uint32_t>( shift );
		return static_cast<UInt32>( tmp );
	}

	constexpr std::uint32_t operator>>( std::uint32_t b, UInt32 shift ) noexcept {

		b >>= static_cast<std::uint32_t>( shift );
		return b;
	}

	constexpr std::uint32_t operator>>( std::uint32_t b, UInt16 shift ) noexcept {

		b >>= static_cast<std::uint32_t>( shift );
		return b;
	}

	constexpr std::uint32_t operator>>( std::uint32_t b, UInt8 shift ) noexcept {

		b >>= static_cast<std::uint32_t>( shift );
		return b;
	}

	constexpr UInt32 &operator|=( UInt32 &lhs, std::uint32_t rhs ) noexcept {

		auto tmp = static_cast<std::uint32_t>( lhs );
		tmp |= static_cast<std::uint32_t>( rhs );
		lhs = static_cast<UInt32>( tmp );
		return lhs;
	}

	constexpr UInt32 &operator|=( UInt32 &lhs, UInt32 rhs ) noexcept {
		auto tmp = static_cast<std::uint32_t>( lhs );
		tmp |= static_cast<std::uint32_t>( rhs );
		lhs = static_cast<UInt32>( tmp );
		return lhs;
	}

	constexpr UInt32 &operator|=( UInt32 &lhs, UInt16 rhs ) noexcept {
		auto tmp = static_cast<std::uint32_t>( lhs );
		tmp |= static_cast<std::uint32_t>( rhs );
		lhs = static_cast<UInt32>( tmp );
		return lhs;
	}

	constexpr UInt32 &operator|=( UInt32 &lhs, UInt8 rhs ) noexcept {
		auto tmp = static_cast<std::uint32_t>( lhs );
		tmp |= static_cast<std::uint32_t>( rhs );
		lhs = static_cast<UInt32>( tmp );
		return lhs;
	}

	constexpr UInt32 &operator&=( UInt32 &lhs, std::uint32_t rhs ) noexcept {

		auto tmp = static_cast<std::uint32_t>( lhs );
		tmp &= static_cast<std::uint32_t>( rhs );
		lhs = static_cast<UInt32>( tmp );
		return lhs;
	}

	constexpr UInt32 &operator&=( UInt32 &lhs, UInt32 rhs ) noexcept {
		auto tmp = static_cast<std::uint32_t>( lhs );
		tmp &= static_cast<std::uint32_t>( rhs );
		lhs = static_cast<UInt32>( tmp );
		return lhs;
	}

	constexpr UInt32 &operator&=( UInt32 &lhs, UInt16 rhs ) noexcept {
		auto tmp = static_cast<std::uint32_t>( lhs );
		tmp &= static_cast<std::uint32_t>( rhs );
		lhs = static_cast<UInt32>( tmp );
		return lhs;
	}

	constexpr UInt32 &operator&=( UInt32 &lhs, UInt8 rhs ) noexcept {
		auto tmp = static_cast<std::uint32_t>( lhs );
		tmp &= static_cast<std::uint32_t>( rhs );
		lhs = static_cast<UInt32>( tmp );
		return lhs;
	}

	constexpr UInt32 &operator^=( UInt32 &lhs, std::uint32_t rhs ) noexcept {

		auto tmp = static_cast<std::uint32_t>( lhs );
		tmp ^= static_cast<std::uint32_t>( rhs );
		lhs = static_cast<UInt32>( tmp );
		return lhs;
	}

	constexpr UInt32 &operator^=( UInt32 &lhs, UInt32 rhs ) noexcept {
		auto tmp = static_cast<std::uint32_t>( lhs );
		tmp ^= static_cast<std::uint32_t>( rhs );
		lhs = static_cast<UInt32>( tmp );
		return lhs;
	}

	constexpr UInt32 &operator^=( UInt32 &lhs, UInt16 rhs ) noexcept {
		auto tmp = static_cast<std::uint32_t>( lhs );
		tmp ^= static_cast<std::uint32_t>( rhs );
		lhs = static_cast<UInt32>( tmp );
		return lhs;
	}

	constexpr UInt32 &operator^=( UInt32 &lhs, UInt8 rhs ) noexcept {
		auto tmp = static_cast<std::uint32_t>( lhs );
		tmp ^= static_cast<std::uint32_t>( rhs );
		lhs = static_cast<UInt32>( tmp );
		return lhs;
	}

	constexpr bool operator>=( UInt32 lhs, std::uint32_t rhs ) noexcept {

		return static_cast<std::uint32_t>( lhs ) >=
		       static_cast<std::uint32_t>( rhs );
	}

	constexpr bool operator>=( std::uint32_t lhs, UInt32 rhs ) noexcept {

		return static_cast<std::uint32_t>( lhs ) >=
		       static_cast<std::uint32_t>( rhs );
	}

	constexpr bool operator<( UInt32 lhs, std::uint32_t rhs ) noexcept {

		return static_cast<std::uint32_t>( lhs ) <
		       static_cast<std::uint32_t>( rhs );
	}

	constexpr bool operator<( std::uint32_t lhs, UInt32 rhs ) noexcept {

		return static_cast<std::uint32_t>( lhs ) <
		       static_cast<std::uint32_t>( rhs );
	}

	constexpr bool operator<=( UInt32 lhs, std::uint32_t rhs ) noexcept {

		return static_cast<std::uint32_t>( lhs ) <=
		       static_cast<std::uint32_t>( rhs );
	}

	constexpr bool operator<=( std::uint32_t lhs, UInt32 rhs ) noexcept {

		return static_cast<std::uint32_t>( lhs ) <=
		       static_cast<std::uint32_t>( rhs );
	}

	constexpr UInt32 operator|( UInt32 lhs, std::uint32_t rhs ) noexcept {

		return static_cast<UInt32>( static_cast<std::uint32_t>( lhs ) |
		                            static_cast<std::uint32_t>( rhs ) );
	}

	constexpr UInt32 operator|( std::uint32_t lhs, UInt32 rhs ) noexcept {

		return static_cast<UInt32>( static_cast<std::uint32_t>( lhs ) |
		                            static_cast<std::uint32_t>( rhs ) );
	}

	constexpr UInt32 operator&( UInt32 lhs, std::uint32_t rhs ) noexcept {

		return static_cast<UInt32>( static_cast<std::uint32_t>( lhs ) &
		                            static_cast<std::uint32_t>( rhs ) );
	}

	constexpr UInt32 operator&( std::uint32_t lhs, UInt32 rhs ) noexcept {

		return static_cast<UInt32>( static_cast<std::uint32_t>( lhs ) &
		                            static_cast<std::uint32_t>( rhs ) );
	}

	constexpr UInt32 operator^( UInt32 lhs, std::uint32_t rhs ) noexcept {

		return static_cast<UInt32>( static_cast<std::uint32_t>( lhs ) ^
		                            static_cast<std::uint32_t>( rhs ) );
	}

	constexpr UInt32 operator^( std::uint32_t lhs, UInt32 rhs ) noexcept {

		return static_cast<UInt32>( static_cast<std::uint32_t>( lhs ) ^
		                            static_cast<std::uint32_t>( rhs ) );
	}

	constexpr bool operator==( UInt32 lhs, std::uint32_t rhs ) noexcept {

		return static_cast<std::uint32_t>( lhs ) ==
		       static_cast<std::uint32_t>( rhs );
	}

	constexpr bool operator==( std::uint32_t lhs, UInt32 rhs ) noexcept {

		return static_cast<std::uint32_t>( lhs ) ==
		       static_cast<std::uint32_t>( rhs );
	}

	constexpr bool operator!=( UInt32 lhs, std::uint32_t rhs ) noexcept {

		return static_cast<std::uint32_t>( lhs ) !=
		       static_cast<std::uint32_t>( rhs );
	}

	constexpr bool operator!=( std::uint32_t lhs, UInt32 rhs ) noexcept {

		return static_cast<std::uint32_t>( lhs ) !=
		       static_cast<std::uint32_t>( rhs );
	}

	constexpr UInt32 operator|( UInt32 lhs, UInt32 rhs ) noexcept {
		return static_cast<UInt32>( static_cast<std::uint32_t>( lhs ) |
		                            static_cast<std::uint32_t>( rhs ) );
	}

	constexpr UInt32 operator|( UInt32 lhs, UInt16 rhs ) noexcept {
		return static_cast<UInt32>( static_cast<std::uint32_t>( lhs ) |
		                            static_cast<std::uint32_t>( rhs ) );
	}

	constexpr UInt32 operator|( UInt32 lhs, UInt8 rhs ) noexcept {
		return static_cast<UInt32>( static_cast<std::uint32_t>( lhs ) |
		                            static_cast<std::uint32_t>( rhs ) );
	}

	constexpr UInt32 operator|( UInt16 lhs, UInt32 rhs ) noexcept {
		return static_cast<UInt32>( static_cast<std::uint32_t>( lhs ) |
		                            static_cast<std::uint32_t>( rhs ) );
	}

	constexpr UInt32 operator|( UInt8 lhs, UInt32 rhs ) noexcept {
		return static_cast<UInt32>( static_cast<std::uint32_t>( lhs ) |
		                            static_cast<std::uint32_t>( rhs ) );
	}

	constexpr UInt32 operator&( UInt32 lhs, UInt32 rhs ) noexcept {
		return static_cast<UInt32>( static_cast<std::uint32_t>( lhs ) &
		                            static_cast<std::uint32_t>( rhs ) );
	}

	constexpr UInt32 operator&( UInt32 lhs, UInt16 rhs ) noexcept {
		return static_cast<UInt32>( static_cast<std::uint32_t>( lhs ) &
		                            static_cast<std::uint32_t>( rhs ) );
	}

	constexpr UInt32 operator&( UInt32 lhs, UInt8 rhs ) noexcept {
		return static_cast<UInt32>( static_cast<std::uint32_t>( lhs ) &
		                            static_cast<std::uint32_t>( rhs ) );
	}

	constexpr UInt32 operator&( UInt16 lhs, UInt32 rhs ) noexcept {
		return static_cast<UInt32>( static_cast<std::uint32_t>( lhs ) &
		                            static_cast<std::uint32_t>( rhs ) );
	}

	constexpr UInt32 operator&( UInt8 lhs, UInt32 rhs ) noexcept {
		return static_cast<UInt32>( static_cast<std::uint32_t>( lhs ) &
		                            static_cast<std::uint32_t>( rhs ) );
	}

	constexpr UInt32 operator^( UInt32 lhs, UInt32 rhs ) noexcept {
		return static_cast<UInt32>( static_cast<std::uint32_t>( lhs ) ^
		                            static_cast<std::uint32_t>( rhs ) );
	}

	constexpr UInt32 operator^( UInt32 lhs, UInt16 rhs ) noexcept {
		return static_cast<UInt32>( static_cast<std::uint32_t>( lhs ) ^
		                            static_cast<std::uint32_t>( rhs ) );
	}

	constexpr UInt32 operator^( UInt32 lhs, UInt8 rhs ) noexcept {
		return static_cast<UInt32>( static_cast<std::uint32_t>( lhs ) ^
		                            static_cast<std::uint32_t>( rhs ) );
	}

	constexpr UInt32 operator^( UInt16 lhs, UInt32 rhs ) noexcept {
		return static_cast<UInt32>( static_cast<std::uint32_t>( lhs ) ^
		                            static_cast<std::uint32_t>( rhs ) );
	}

	constexpr UInt32 operator^( UInt8 lhs, UInt32 rhs ) noexcept {
		return static_cast<UInt32>( static_cast<std::uint32_t>( lhs ) ^
		                            static_cast<std::uint32_t>( rhs ) );
	}

	template<unsigned bits>
	constexpr UInt32 rotate_left( UInt32 value ) {
		static_assert( bits <= 32 );
		return ( value << bits ) | ( value >> ( 32U - bits ) );
	}

	constexpr UInt32 rotate_left( UInt32 value, unsigned bits ) {
		return ( value << bits ) | ( value >> ( 32U - bits ) );
	}

	template<unsigned bits>
	constexpr UInt32 rotate_right( UInt32 value ) {
		static_assert( bits <= 32 );
		return ( value >> bits ) | ( value << ( 32U - bits ) );
	}

	constexpr UInt32 rotate_right( UInt32 value, unsigned bits ) {
		return ( value >> bits ) | ( value << ( 32U - bits ) );
	}

	constexpr UInt16 &operator<<=( UInt16 &b, std::uint16_t shift ) noexcept {

		auto tmp = static_cast<std::uint32_t>( b );
		tmp <<= static_cast<std::uint32_t>( shift );
		b = static_cast<UInt16>( tmp );
		return b;
	}

	constexpr UInt16 &operator<<=( UInt16 &b, UInt16 shift ) noexcept {
		auto tmp = static_cast<std::uint32_t>( b );
		tmp <<= static_cast<std::uint32_t>( shift );
		b = static_cast<UInt16>( tmp );
		return b;
	}

	constexpr UInt16 &operator<<=( UInt16 &b, UInt8 shift ) noexcept {
		auto tmp = static_cast<std::uint32_t>( b );
		tmp <<= static_cast<std::uint32_t>( shift );
		b = static_cast<UInt16>( tmp );
		return b;
	}

	constexpr UInt16 &operator>>=( UInt16 &b, std::uint16_t shift ) noexcept {

		auto tmp = static_cast<std::uint32_t>( b );
		tmp >>= static_cast<std::uint32_t>( shift );
		b = static_cast<UInt16>( tmp );
		return b;
	}

	constexpr UInt16 &operator>>=( UInt16 &b, UInt16 shift ) noexcept {
		auto tmp = static_cast<std::uint32_t>( b );
		tmp >>= static_cast<std::uint32_t>( shift );
		b = static_cast<UInt16>( tmp );
		return b;
	}

	constexpr UInt16 &operator>>=( UInt16 &b, UInt8 shift ) noexcept {
		auto tmp = static_cast<std::uint32_t>( b );
		tmp >>= static_cast<std::uint32_t>( shift );
		b = static_cast<UInt16>( tmp );
		return b;
	}

	constexpr UInt16 operator<<( UInt16 b, std::uint16_t shift ) noexcept {

		auto tmp = static_cast<std::uint32_t>( b );
		tmp <<= static_cast<std::uint32_t>( shift );
		return static_cast<UInt16>( tmp );
	}

	constexpr UInt16 operator<<( UInt16 b, UInt16 shift ) noexcept {
		auto tmp = static_cast<std::uint32_t>( b );
		tmp <<= static_cast<std::uint32_t>( shift );
		return static_cast<UInt16>( tmp );
	}

	constexpr UInt16 operator<<( UInt16 b, UInt8 shift ) noexcept {
		auto tmp = static_cast<std::uint32_t>( b );
		tmp <<= static_cast<std::uint32_t>( shift );
		return static_cast<UInt16>( tmp );
	}

	constexpr std::uint16_t operator<<( std::uint16_t b, UInt16 shift ) noexcept {

		b <<= static_cast<std::uint32_t>( shift );
		return b;
	}

	constexpr UInt16 operator>>( UInt16 b, std::uint16_t shift ) noexcept {

		auto tmp = static_cast<std::uint32_t>( b );
		tmp >>= static_cast<std::uint32_t>( shift );
		return static_cast<UInt16>( tmp );
	}

	constexpr UInt16 operator>>( UInt16 b, UInt16 shift ) noexcept {
		auto tmp = static_cast<std::uint32_t>( b );
		tmp >>= static_cast<std::uint32_t>( shift );
		return static_cast<UInt16>( tmp );
	}

	constexpr std::uint16_t operator>>( std::uint16_t b, UInt16 shift ) noexcept {

		b >>= static_cast<std::uint32_t>( shift );
		return b;
	}

	constexpr UInt16 &operator|=( UInt16 &lhs, std::uint16_t rhs ) noexcept {

		auto tmp = static_cast<std::uint32_t>( lhs );
		tmp |= static_cast<std::uint32_t>( rhs );
		lhs = static_cast<UInt16>( tmp );
		return lhs;
	}

	constexpr UInt16 &operator|=( UInt16 &lhs, UInt16 rhs ) noexcept {
		auto tmp = static_cast<std::uint32_t>( lhs );
		tmp |= static_cast<std::uint32_t>( rhs );
		lhs = static_cast<UInt16>( tmp );
		return lhs;
	}

	constexpr UInt16 &operator|=( UInt16 &lhs, UInt8 rhs ) noexcept {
		auto tmp = static_cast<std::uint32_t>( lhs );
		tmp |= static_cast<std::uint32_t>( rhs );
		lhs = static_cast<UInt16>( tmp );
		return lhs;
	}

	constexpr UInt16 &operator&=( UInt16 &lhs, std::uint16_t rhs ) noexcept {

		auto tmp = static_cast<std::uint32_t>( lhs );
		tmp &= static_cast<std::uint32_t>( rhs );
		lhs = static_cast<UInt16>( tmp );
		return lhs;
	}

	constexpr UInt16 &operator&=( UInt16 &lhs, UInt16 rhs ) noexcept {
		auto tmp = static_cast<std::uint32_t>( lhs );
		tmp &= static_cast<std::uint32_t>( rhs );
		lhs = static_cast<UInt16>( tmp );
		return lhs;
	}

	constexpr UInt16 &operator&=( UInt16 &lhs, UInt8 rhs ) noexcept {
		auto tmp = static_cast<std::uint32_t>( lhs );
		tmp &= static_cast<std::uint32_t>( rhs );
		lhs = static_cast<UInt16>( tmp );
		return lhs;
	}

	constexpr UInt16 &operator^=( UInt16 &lhs, std::uint16_t rhs ) noexcept {

		auto tmp = static_cast<std::uint32_t>( lhs );
		tmp ^= static_cast<std::uint32_t>( rhs );
		lhs = static_cast<UInt16>( tmp );
		return lhs;
	}

	constexpr UInt16 &operator^=( UInt16 &lhs, UInt16 rhs ) noexcept {
		auto tmp = static_cast<std::uint32_t>( lhs );
		tmp ^= static_cast<std::uint32_t>( rhs );
		lhs = static_cast<UInt16>( tmp );
		return lhs;
	}

	constexpr UInt16 &operator^=( UInt16 &lhs, UInt8 rhs ) noexcept {
		auto tmp = static_cast<std::uint32_t>( lhs );
		tmp ^= static_cast<std::uint32_t>( rhs );
		lhs = static_cast<UInt16>( tmp );
		return lhs;
	}

	constexpr UInt16 operator|( UInt16 lhs, std::uint16_t rhs ) noexcept {

		return static_cast<UInt16>( static_cast<std::uint32_t>( lhs ) |
		                            static_cast<std::uint32_t>( rhs ) );
	}

	constexpr UInt16 operator|( std::uint16_t lhs, UInt16 rhs ) noexcept {

		return static_cast<UInt16>( static_cast<std::uint32_t>( lhs ) |
		                            static_cast<std::uint32_t>( rhs ) );
	}

	constexpr UInt16 operator&( UInt16 lhs, std::uint16_t rhs ) noexcept {

		return static_cast<UInt16>( static_cast<std::uint32_t>( lhs ) &
		                            static_cast<std::uint32_t>( rhs ) );
	}

	constexpr UInt16 operator&( std::uint16_t lhs, UInt16 rhs ) noexcept {

		return static_cast<UInt16>( static_cast<std::uint32_t>( lhs ) &
		                            static_cast<std::uint32_t>( rhs ) );
	}

	constexpr UInt16 operator^( UInt16 lhs, std::uint16_t rhs ) noexcept {

		return static_cast<UInt16>( static_cast<std::uint32_t>( lhs ) ^
		                            static_cast<std::uint32_t>( rhs ) );
	}

	constexpr UInt16 operator^( std::uint16_t lhs, UInt16 rhs ) noexcept {

		return static_cast<UInt16>( static_cast<std::uint32_t>( lhs ) ^
		                            static_cast<std::uint32_t>( rhs ) );
	}

	constexpr bool operator==( UInt16 lhs, std::uint16_t rhs ) noexcept {

		return static_cast<std::uint32_t>( lhs ) ==
		       static_cast<std::uint32_t>( rhs );
	}

	constexpr bool operator==( std::uint16_t lhs, UInt16 rhs ) noexcept {

		return static_cast<std::uint32_t>( lhs ) ==
		       static_cast<std::uint32_t>( rhs );
	}

	constexpr bool operator!=( UInt16 lhs, std::uint16_t rhs ) noexcept {

		return static_cast<std::uint32_t>( lhs ) !=
		       static_cast<std::uint32_t>( rhs );
	}

	constexpr bool operator!=( std::uint16_t lhs, UInt16 rhs ) noexcept {

		return static_cast<std::uint32_t>( lhs ) !=
		       static_cast<std::uint32_t>( rhs );
	}

	constexpr bool operator>( UInt16 lhs, std::uint16_t rhs ) noexcept {

		return static_cast<std::uint32_t>( lhs ) >
		       static_cast<std::uint32_t>( rhs );
	}

	constexpr bool operator>( std::uint16_t lhs, UInt16 rhs ) noexcept {

		return static_cast<std::uint32_t>( lhs ) >
		       static_cast<std::uint32_t>( rhs );
	}

	constexpr bool operator>=( UInt16 lhs, std::uint16_t rhs ) noexcept {

		return static_cast<std::uint32_t>( lhs ) >=
		       static_cast<std::uint32_t>( rhs );
	}

	constexpr bool operator>=( std::uint16_t lhs, UInt16 rhs ) noexcept {

		return static_cast<std::uint32_t>( lhs ) >=
		       static_cast<std::uint32_t>( rhs );
	}

	constexpr bool operator<( UInt16 lhs, std::uint16_t rhs ) noexcept {

		return static_cast<std::uint32_t>( lhs ) <
		       static_cast<std::uint32_t>( rhs );
	}

	constexpr bool operator<( std::uint16_t lhs, UInt16 rhs ) noexcept {

		return static_cast<std::uint32_t>( lhs ) <
		       static_cast<std::uint32_t>( rhs );
	}

	constexpr bool operator<=( UInt16 lhs, std::uint16_t rhs ) noexcept {

		return static_cast<std::uint32_t>( lhs ) <=
		       static_cast<std::uint32_t>( rhs );
	}

	constexpr bool operator<=( std::uint16_t lhs, UInt16 rhs ) noexcept {

		return static_cast<std::uint32_t>( lhs ) <=
		       static_cast<std::uint32_t>( rhs );
	}

	constexpr UInt16 operator|( UInt16 lhs, UInt16 rhs ) noexcept {
		return static_cast<UInt16>( static_cast<std::uint32_t>( lhs ) |
		                            static_cast<std::uint32_t>( rhs ) );
	}

	constexpr UInt16 operator|( UInt16 lhs, UInt8 rhs ) noexcept {
		return static_cast<UInt16>( static_cast<std::uint32_t>( lhs ) |
		                            static_cast<std::uint32_t>( rhs ) );
	}

	constexpr UInt16 operator&( UInt16 lhs, UInt16 rhs ) noexcept {
		return static_cast<UInt16>( static_cast<std::uint32_t>( lhs ) &
		                            static_cast<std::uint32_t>( rhs ) );
	}

	constexpr UInt16 operator&( UInt16 lhs, UInt8 rhs ) noexcept {
		return static_cast<UInt16>( static_cast<std::uint32_t>( lhs ) &
		                            static_cast<std::uint32_t>( rhs ) );
	}

	constexpr UInt16 operator^( UInt16 lhs, UInt16 rhs ) noexcept {
		return static_cast<UInt16>( static_cast<std::uint32_t>( lhs ) ^
		                            static_cast<std::uint32_t>( rhs ) );
	}

	constexpr UInt16 operator^( UInt16 lhs, UInt8 rhs ) noexcept {
		return static_cast<UInt16>( static_cast<std::uint32_t>( lhs ) ^
		                            static_cast<std::uint32_t>( rhs ) );
	}
	//*********
	constexpr UInt64 &operator+=( UInt64 &lhs, UInt64 rhs ) noexcept {
		lhs = static_cast<UInt64>( static_cast<std::uint64_t>( lhs ) +
		                           static_cast<std::uint64_t>( rhs ) );
		return lhs;
	}

	constexpr UInt64 &operator+=( UInt64 &lhs, UInt32 rhs ) noexcept {
		lhs = static_cast<UInt64>( static_cast<std::uint64_t>( lhs ) +
		                           static_cast<std::uint64_t>( rhs ) );
		return lhs;
	}

	constexpr UInt64 &operator+=( UInt64 &lhs, UInt16 rhs ) noexcept {
		lhs = static_cast<UInt64>( static_cast<std::uint64_t>( lhs ) +
		                           static_cast<std::uint64_t>( rhs ) );
		return lhs;
	}

	constexpr UInt64 &operator+=( UInt64 &lhs, UInt8 rhs ) noexcept {
		lhs = static_cast<UInt64>( static_cast<std::uint64_t>( lhs ) +
		                           static_cast<std::uint64_t>( rhs ) );
		return lhs;
	}

	constexpr UInt64 &operator+=( UInt64 &lhs, std::uint64_t rhs ) noexcept {
		lhs = static_cast<UInt64>( static_cast<std::uint64_t>( lhs ) + rhs );
		return lhs;
	}

	constexpr std::uint64_t &operator+=( std::uint64_t &lhs,
	                                     UInt64 rhs ) noexcept {
		lhs += static_cast<std::uint64_t>( rhs );
		return lhs;
	}

	constexpr UInt64 operator+( UInt64 lhs, UInt64 rhs ) noexcept {
		return static_cast<UInt64>( static_cast<std::uint64_t>( lhs ) +
		                            static_cast<std::uint64_t>( rhs ) );
	}
	constexpr UInt64 operator+( UInt64 lhs, UInt32 rhs ) noexcept {
		return static_cast<UInt64>( static_cast<std::uint64_t>( lhs ) +
		                            static_cast<std::uint64_t>( rhs ) );
	}
	constexpr UInt64 operator+( UInt64 lhs, UInt16 rhs ) noexcept {
		return static_cast<UInt64>( static_cast<std::uint64_t>( lhs ) +
		                            static_cast<std::uint64_t>( rhs ) );
	}
	constexpr UInt64 operator+( UInt64 lhs, UInt8 rhs ) noexcept {
		return static_cast<UInt64>( static_cast<std::uint64_t>( lhs ) +
		                            static_cast<std::uint64_t>( rhs ) );
	}
	constexpr UInt64 operator+( UInt32 lhs, UInt64 rhs ) noexcept {
		return static_cast<UInt64>( static_cast<std::uint64_t>( lhs ) +
		                            static_cast<std::uint64_t>( rhs ) );
	}
	constexpr UInt64 operator+( UInt16 lhs, UInt64 rhs ) noexcept {
		return static_cast<UInt64>( static_cast<std::uint64_t>( lhs ) +
		                            static_cast<std::uint64_t>( rhs ) );
	}
	constexpr UInt64 operator+( UInt8 lhs, UInt64 rhs ) noexcept {
		return static_cast<UInt64>( static_cast<std::uint64_t>( lhs ) +
		                            static_cast<std::uint64_t>( rhs ) );
	}

	constexpr UInt64 operator+( UInt64 lhs, std::uint64_t rhs ) noexcept {
		lhs = static_cast<UInt64>( static_cast<std::uint64_t>( lhs ) + rhs );
		return lhs;
	}

	constexpr UInt64 operator+( std::uint64_t lhs, UInt64 rhs ) noexcept {
		lhs += static_cast<std::uint64_t>( rhs );
		return static_cast<UInt64>( lhs );
	}

	constexpr UInt64 &operator-=( UInt64 &lhs, UInt64 rhs ) noexcept {
		lhs = static_cast<UInt64>( static_cast<std::uint64_t>( lhs ) -
		                           static_cast<std::uint64_t>( rhs ) );
		return lhs;
	}
	constexpr UInt64 &operator-=( UInt64 &lhs, UInt32 rhs ) noexcept {
		lhs = static_cast<UInt64>( static_cast<std::uint64_t>( lhs ) -
		                           static_cast<std::uint64_t>( rhs ) );
		return lhs;
	}
	constexpr UInt64 &operator-=( UInt64 &lhs, UInt16 rhs ) noexcept {
		lhs = static_cast<UInt64>( static_cast<std::uint64_t>( lhs ) -
		                           static_cast<std::uint64_t>( rhs ) );
		return lhs;
	}
	constexpr UInt64 &operator-=( UInt64 &lhs, UInt8 rhs ) noexcept {
		lhs = static_cast<UInt64>( static_cast<std::uint64_t>( lhs ) -
		                           static_cast<std::uint64_t>( rhs ) );
		return lhs;
	}

	constexpr UInt64 &operator-=( UInt64 &lhs, std::uint64_t rhs ) noexcept {
		lhs = static_cast<UInt64>( static_cast<std::uint64_t>( lhs ) - rhs );
		return lhs;
	}

	constexpr std::uint64_t &operator-=( std::uint64_t &lhs,
	                                     UInt64 rhs ) noexcept {
		lhs -= static_cast<std::uint64_t>( rhs );
		return lhs;
	}

	constexpr UInt64 operator-( UInt64 lhs, UInt64 rhs ) noexcept {
		lhs = static_cast<UInt64>( static_cast<std::uint64_t>( lhs ) -
		                           static_cast<std::uint64_t>( rhs ) );
		return lhs;
	}
	constexpr UInt64 operator-( UInt64 lhs, UInt32 rhs ) noexcept {
		lhs = static_cast<UInt64>( static_cast<std::uint64_t>( lhs ) -
		                           static_cast<std::uint64_t>( rhs ) );
		return lhs;
	}
	constexpr UInt64 operator-( UInt64 lhs, UInt16 rhs ) noexcept {
		lhs = static_cast<UInt64>( static_cast<std::uint64_t>( lhs ) -
		                           static_cast<std::uint64_t>( rhs ) );
		return lhs;
	}
	constexpr UInt64 operator-( UInt64 lhs, UInt8 rhs ) noexcept {
		lhs = static_cast<UInt64>( static_cast<std::uint64_t>( lhs ) -
		                           static_cast<std::uint64_t>( rhs ) );
		return lhs;
	}
	constexpr UInt64 operator-( UInt32 lhs, UInt64 rhs ) noexcept {
		return static_cast<UInt64>( static_cast<std::uint64_t>( lhs ) -
		                            static_cast<std::uint64_t>( rhs ) );
	}
	constexpr UInt64 operator-( UInt16 lhs, UInt64 rhs ) noexcept {
		return static_cast<UInt64>( static_cast<std::uint64_t>( lhs ) -
		                            static_cast<std::uint64_t>( rhs ) );
	}
	constexpr UInt64 operator-( UInt8 lhs, UInt64 rhs ) noexcept {
		return static_cast<UInt64>( static_cast<std::uint64_t>( lhs ) -
		                            static_cast<std::uint64_t>( rhs ) );
	}

	constexpr UInt64 operator-( UInt64 lhs, std::uint64_t rhs ) noexcept {
		lhs = static_cast<UInt64>( static_cast<std::uint64_t>( lhs ) - rhs );
		return lhs;
	}

	constexpr UInt64 operator-( std::uint64_t lhs, UInt64 rhs ) noexcept {
		lhs -= static_cast<std::uint64_t>( rhs );
		return static_cast<UInt64>( lhs );
	}

	constexpr UInt64 &operator*=( UInt64 &lhs, UInt64 rhs ) noexcept {
		lhs = static_cast<UInt64>( static_cast<std::uint64_t>( lhs ) *
		                           static_cast<std::uint64_t>( rhs ) );
		return lhs;
	}

	constexpr UInt64 &operator*=( UInt64 &lhs, UInt32 rhs ) noexcept {
		lhs = static_cast<UInt64>( static_cast<std::uint64_t>( lhs ) *
		                           static_cast<std::uint64_t>( rhs ) );
		return lhs;
	}

	constexpr UInt64 &operator*=( UInt64 &lhs, UInt16 rhs ) noexcept {
		lhs = static_cast<UInt64>( static_cast<std::uint64_t>( lhs ) *
		                           static_cast<std::uint64_t>( rhs ) );
		return lhs;
	}

	constexpr UInt64 &operator*=( UInt64 &lhs, UInt8 rhs ) noexcept {
		lhs = static_cast<UInt64>( static_cast<std::uint64_t>( lhs ) *
		                           static_cast<std::uint64_t>( rhs ) );
		return lhs;
	}

	constexpr UInt64 &operator*=( UInt64 &lhs, std::uint64_t rhs ) noexcept {
		lhs = static_cast<UInt64>( static_cast<std::uint64_t>( lhs ) * rhs );
		return lhs;
	}

	constexpr std::uint64_t &operator*=( std::uint64_t &lhs,
	                                     UInt64 rhs ) noexcept {
		lhs *= static_cast<std::uint64_t>( rhs );
		return lhs;
	}

	constexpr UInt64 operator*( UInt64 lhs, UInt64 rhs ) noexcept {
		lhs = static_cast<UInt64>( static_cast<std::uint64_t>( lhs ) *
		                           static_cast<std::uint64_t>( rhs ) );
		return lhs;
	}

	constexpr UInt64 operator*( UInt64 lhs, UInt32 rhs ) noexcept {
		lhs = static_cast<UInt64>( static_cast<std::uint64_t>( lhs ) *
		                           static_cast<std::uint64_t>( rhs ) );
		return lhs;
	}

	constexpr UInt64 operator*( UInt64 lhs, UInt16 rhs ) noexcept {
		lhs = static_cast<UInt64>( static_cast<std::uint64_t>( lhs ) *
		                           static_cast<std::uint64_t>( rhs ) );
		return lhs;
	}

	constexpr UInt64 operator*( UInt64 lhs, UInt8 rhs ) noexcept {
		lhs = static_cast<UInt64>( static_cast<std::uint64_t>( lhs ) *
		                           static_cast<std::uint64_t>( rhs ) );
		return lhs;
	}

	constexpr UInt64 operator*( UInt32 lhs, UInt64 rhs ) noexcept {
		return static_cast<UInt64>( static_cast<std::uint64_t>( lhs ) *
		                            static_cast<std::uint64_t>( rhs ) );
	}

	constexpr UInt64 operator*( UInt16 lhs, UInt64 rhs ) noexcept {
		return static_cast<UInt64>( static_cast<std::uint64_t>( lhs ) *
		                            static_cast<std::uint64_t>( rhs ) );
	}

	constexpr UInt64 operator*( UInt8 lhs, UInt64 rhs ) noexcept {
		return static_cast<UInt64>( static_cast<std::uint64_t>( lhs ) *
		                            static_cast<std::uint64_t>( rhs ) );
	}

	constexpr UInt64 operator*( UInt64 lhs, std::uint64_t rhs ) noexcept {
		lhs = static_cast<UInt64>( static_cast<std::uint64_t>( lhs ) * rhs );
		return lhs;
	}

	constexpr UInt64 operator*( std::uint64_t lhs, UInt64 rhs ) noexcept {
		lhs *= static_cast<std::uint64_t>( rhs );
		return static_cast<UInt64>( lhs );
	}

	constexpr UInt64 &operator/=( UInt64 &lhs, UInt64 rhs ) noexcept {
		lhs = static_cast<UInt64>( static_cast<std::uint64_t>( lhs ) /
		                           static_cast<std::uint64_t>( rhs ) );
		return lhs;
	}

	constexpr UInt64 &operator/=( UInt64 &lhs, UInt32 rhs ) noexcept {
		lhs = static_cast<UInt64>( static_cast<std::uint64_t>( lhs ) /
		                           static_cast<std::uint64_t>( rhs ) );
		return lhs;
	}

	constexpr UInt64 &operator/=( UInt64 &lhs, UInt16 rhs ) noexcept {
		lhs = static_cast<UInt64>( static_cast<std::uint64_t>( lhs ) /
		                           static_cast<std::uint64_t>( rhs ) );
		return lhs;
	}

	constexpr UInt64 &operator/=( UInt64 &lhs, UInt8 rhs ) noexcept {
		lhs = static_cast<UInt64>( static_cast<std::uint64_t>( lhs ) /
		                           static_cast<std::uint64_t>( rhs ) );
		return lhs;
	}

	constexpr UInt64 &operator/=( UInt64 &lhs, std::uint64_t rhs ) noexcept {
		lhs = static_cast<UInt64>( static_cast<std::uint64_t>( lhs ) / rhs );
		return lhs;
	}

	constexpr std::uint64_t &operator/=( std::uint64_t &lhs,
	                                     UInt64 rhs ) noexcept {
		lhs /= static_cast<std::uint64_t>( rhs );
		return lhs;
	}

	constexpr std::uint64_t &operator/=( std::uint64_t &lhs,
	                                     UInt32 rhs ) noexcept {
		lhs /= static_cast<std::uint64_t>( rhs );
		return lhs;
	}

	constexpr std::uint64_t &operator/=( std::uint64_t &lhs,
	                                     UInt16 rhs ) noexcept {
		lhs /= static_cast<std::uint64_t>( rhs );
		return lhs;
	}

	constexpr std::uint64_t &operator/=( std::uint64_t &lhs,
	                                     UInt8 rhs ) noexcept {
		lhs /= static_cast<std::uint64_t>( rhs );
		return lhs;
	}

	constexpr UInt64 operator/( UInt64 lhs, UInt64 rhs ) noexcept {
		return static_cast<UInt64>( static_cast<std::uint64_t>( lhs ) /
		                            static_cast<std::uint64_t>( rhs ) );
	}

	constexpr UInt64 operator/( UInt64 lhs, UInt32 rhs ) noexcept {
		lhs = static_cast<UInt64>( static_cast<std::uint64_t>( lhs ) /
		                           static_cast<std::uint64_t>( rhs ) );
		return lhs;
	}

	constexpr UInt64 operator/( UInt64 lhs, UInt16 rhs ) noexcept {
		lhs = static_cast<UInt64>( static_cast<std::uint64_t>( lhs ) /
		                           static_cast<std::uint64_t>( rhs ) );
		return lhs;
	}

	constexpr UInt64 operator/( UInt64 lhs, UInt8 rhs ) noexcept {
		lhs = static_cast<UInt64>( static_cast<std::uint64_t>( lhs ) /
		                           static_cast<std::uint64_t>( rhs ) );
		return lhs;
	}

	constexpr UInt64 operator/( UInt32 lhs, UInt64 rhs ) noexcept {
		return static_cast<UInt64>( static_cast<std::uint64_t>( lhs ) /
		                            static_cast<std::uint64_t>( rhs ) );
	}

	constexpr UInt64 operator/( UInt16 lhs, UInt64 rhs ) noexcept {
		return static_cast<UInt64>( static_cast<std::uint64_t>( lhs ) /
		                            static_cast<std::uint64_t>( rhs ) );
	}

	constexpr UInt64 operator/( UInt8 lhs, UInt64 rhs ) noexcept {
		return static_cast<UInt64>( static_cast<std::uint64_t>( lhs ) /
		                            static_cast<std::uint64_t>( rhs ) );
	}

	constexpr UInt64 operator/( UInt64 lhs, std::uint64_t rhs ) noexcept {
		lhs = static_cast<UInt64>( static_cast<std::uint64_t>( lhs ) / rhs );
		return lhs;
	}

	constexpr UInt64 operator/( std::uint64_t lhs, UInt64 rhs ) noexcept {
		lhs /= static_cast<std::uint64_t>( rhs );
		return static_cast<UInt64>( lhs );
	}

	constexpr UInt64 operator/( std::uint64_t lhs, UInt32 rhs ) noexcept {
		lhs /= static_cast<std::uint64_t>( rhs );
		return static_cast<UInt64>( lhs );
	}

	constexpr UInt64 operator/( std::uint64_t lhs, UInt16 rhs ) noexcept {
		lhs /= static_cast<std::uint64_t>( rhs );
		return static_cast<UInt64>( lhs );
	}

	constexpr UInt64 operator/( std::uint64_t lhs, UInt8 rhs ) noexcept {
		lhs /= static_cast<std::uint64_t>( rhs );
		return static_cast<UInt64>( lhs );
	}

	constexpr bool operator!( UInt64 value ) {
		return !static_cast<std::uint64_t>( value );
	}

	constexpr UInt64 operator+( UInt64 value ) {
		return value;
	}

	constexpr UInt64 operator-( UInt64 value ) {
		return static_cast<UInt64>( -static_cast<std::uint64_t>( value ) );
	}

	constexpr UInt64 &operator++( UInt64 &value ) {
		value = static_cast<UInt64>( static_cast<std::uint64_t>( value ) + 1 );
		return value;
	}

	constexpr UInt64 operator++( UInt64 &value, int ) {
		auto result = value;
		value = static_cast<UInt64>( static_cast<std::uint64_t>( value ) + 1 );
		return result;
	}

	constexpr UInt64 &operator--( UInt64 &value ) {
		value = static_cast<UInt64>( static_cast<std::uint64_t>( value ) - 1 );
		return value;
	}

	constexpr UInt64 operator--( UInt64 &value, int ) {
		auto result = value;
		value = static_cast<UInt64>( static_cast<std::uint64_t>( value ) - 1 );
		return result;
	}

	constexpr UInt64 operator~( UInt64 value ) {
		return static_cast<UInt64>( ~static_cast<std::uint64_t>( value ) );
	}
	//*********
	constexpr UInt32 &operator+=( UInt32 &lhs, UInt32 rhs ) noexcept {
		lhs = static_cast<UInt32>( static_cast<std::uint32_t>( lhs ) +
		                           static_cast<std::uint32_t>( rhs ) );
		return lhs;
	}

	constexpr UInt32 &operator+=( UInt32 &lhs, UInt16 rhs ) noexcept {
		lhs = static_cast<UInt32>( static_cast<std::uint32_t>( lhs ) +
		                           static_cast<std::uint32_t>( rhs ) );
		return lhs;
	}

	constexpr UInt32 &operator+=( UInt32 &lhs, UInt8 rhs ) noexcept {
		lhs = static_cast<UInt32>( static_cast<std::uint32_t>( lhs ) +
		                           static_cast<std::uint32_t>( rhs ) );
		return lhs;
	}

	constexpr UInt32 &operator+=( UInt32 &lhs, std::uint32_t rhs ) noexcept {
		lhs = static_cast<UInt32>( static_cast<std::uint32_t>( lhs ) + rhs );
		return lhs;
	}

	constexpr std::uint32_t &operator+=( std::uint32_t &lhs,
	                                     UInt32 rhs ) noexcept {
		lhs += static_cast<std::uint32_t>( rhs );
		return lhs;
	}

	constexpr std::uint32_t &operator+=( std::uint32_t &lhs,
	                                     UInt16 rhs ) noexcept {
		lhs += static_cast<std::uint32_t>( rhs );
		return lhs;
	}

	constexpr std::uint32_t &operator+=( std::uint32_t &lhs,
	                                     UInt8 rhs ) noexcept {
		lhs += static_cast<std::uint32_t>( rhs );
		return lhs;
	}

	constexpr UInt32 operator+( UInt32 lhs, UInt32 rhs ) noexcept {
		lhs = static_cast<UInt32>( static_cast<std::uint32_t>( lhs ) +
		                           static_cast<std::uint32_t>( rhs ) );
		return lhs;
	}

	constexpr UInt32 operator+( UInt32 lhs, UInt16 rhs ) noexcept {
		lhs = static_cast<UInt32>( static_cast<std::uint32_t>( lhs ) +
		                           static_cast<std::uint32_t>( rhs ) );
		return lhs;
	}

	constexpr UInt32 operator+( UInt32 lhs, UInt8 rhs ) noexcept {
		lhs = static_cast<UInt32>( static_cast<std::uint32_t>( lhs ) +
		                           static_cast<std::uint32_t>( rhs ) );
		return lhs;
	}

	constexpr UInt32 operator+( UInt16 lhs, UInt32 rhs ) noexcept {
		return static_cast<UInt32>( static_cast<std::uint32_t>( lhs ) +
		                            static_cast<std::uint32_t>( rhs ) );
	}

	constexpr UInt32 operator+( UInt8 lhs, UInt32 rhs ) noexcept {
		return static_cast<UInt32>( static_cast<std::uint32_t>( lhs ) +
		                            static_cast<std::uint32_t>( rhs ) );
	}

	constexpr UInt32 operator+( UInt32 lhs, std::uint32_t rhs ) noexcept {
		lhs = static_cast<UInt32>( static_cast<std::uint32_t>( lhs ) + rhs );
		return lhs;
	}

	constexpr UInt32 operator+( UInt16 lhs, std::uint32_t rhs ) noexcept {
		return static_cast<UInt32>( static_cast<std::uint32_t>( lhs ) + rhs );
	}

	constexpr UInt32 operator+( UInt8 lhs, std::uint32_t rhs ) noexcept {
		return static_cast<UInt32>( static_cast<std::uint32_t>( lhs ) + rhs );
	}

	constexpr UInt32 operator+( std::uint32_t lhs, UInt32 rhs ) noexcept {
		return static_cast<UInt32>( static_cast<std::uint32_t>( lhs ) +
		                            static_cast<std::uint32_t>( rhs ) );
	}

	constexpr UInt32 operator+( std::uint32_t lhs, UInt16 rhs ) noexcept {
		return static_cast<UInt32>( static_cast<std::uint32_t>( lhs ) +
		                            static_cast<std::uint32_t>( rhs ) );
	}

	constexpr UInt32 operator+( std::uint32_t lhs, UInt8 rhs ) noexcept {
		return static_cast<UInt32>( static_cast<std::uint32_t>( lhs ) +
		                            static_cast<std::uint32_t>( rhs ) );
	}

	constexpr UInt32 &operator-=( UInt32 &lhs, UInt32 rhs ) noexcept {
		lhs = static_cast<UInt32>( static_cast<std::uint32_t>( lhs ) -
		                           static_cast<std::uint32_t>( rhs ) );
		return lhs;
	}

	constexpr UInt32 &operator-=( UInt32 &lhs, UInt16 rhs ) noexcept {
		lhs = static_cast<UInt32>( static_cast<std::uint32_t>( lhs ) -
		                           static_cast<std::uint32_t>( rhs ) );
		return lhs;
	}

	constexpr UInt32 &operator-=( UInt32 &lhs, UInt8 rhs ) noexcept {
		lhs = static_cast<UInt32>( static_cast<std::uint32_t>( lhs ) -
		                           static_cast<std::uint32_t>( rhs ) );
		return lhs;
	}

	constexpr UInt32 &operator-=( UInt32 &lhs, std::uint32_t rhs ) noexcept {
		lhs = static_cast<UInt32>( static_cast<std::uint32_t>( lhs ) - rhs );
		return lhs;
	}

	constexpr std::uint32_t &operator-=( std::uint32_t &lhs,
	                                     UInt32 rhs ) noexcept {
		lhs -= static_cast<std::uint32_t>( rhs );
		return lhs;
	}

	constexpr std::uint32_t &operator-=( std::uint32_t &lhs,
	                                     UInt16 rhs ) noexcept {
		lhs -= static_cast<std::uint32_t>( rhs );
		return lhs;
	}

	constexpr std::uint32_t &operator-=( std::uint32_t &lhs,
	                                     UInt8 rhs ) noexcept {
		lhs -= static_cast<std::uint32_t>( rhs );
		return lhs;
	}

	constexpr UInt32 operator-( UInt32 lhs, UInt32 rhs ) noexcept {
		lhs = static_cast<UInt32>( static_cast<std::uint32_t>( lhs ) -
		                           static_cast<std::uint32_t>( rhs ) );
		return lhs;
	}

	constexpr UInt32 operator-( UInt32 lhs, UInt16 rhs ) noexcept {
		lhs = static_cast<UInt32>( static_cast<std::uint32_t>( lhs ) -
		                           static_cast<std::uint32_t>( rhs ) );
		return lhs;
	}

	constexpr UInt32 operator-( UInt32 lhs, UInt8 rhs ) noexcept {
		lhs = static_cast<UInt32>( static_cast<std::uint32_t>( lhs ) -
		                           static_cast<std::uint32_t>( rhs ) );
		return lhs;
	}

	constexpr UInt32 operator-( UInt16 lhs, UInt32 rhs ) noexcept {
		return static_cast<UInt32>( static_cast<std::uint32_t>( lhs ) -
		                            static_cast<std::uint32_t>( rhs ) );
	}

	constexpr UInt32 operator-( UInt8 lhs, UInt32 rhs ) noexcept {
		return static_cast<UInt32>( static_cast<std::uint32_t>( lhs ) -
		                            static_cast<std::uint32_t>( rhs ) );
	}

	constexpr UInt32 operator-( UInt32 lhs, std::uint32_t rhs ) noexcept {
		lhs = static_cast<UInt32>( static_cast<std::uint32_t>( lhs ) - rhs );
		return lhs;
	}

	constexpr UInt32 operator-( std::uint32_t lhs, UInt32 rhs ) noexcept {
		lhs -= static_cast<std::uint32_t>( rhs );
		return static_cast<UInt32>( lhs );
	}

	constexpr UInt32 operator-( std::uint32_t lhs, UInt16 rhs ) noexcept {
		lhs -= static_cast<std::uint32_t>( rhs );
		return static_cast<UInt32>( lhs );
	}

	constexpr UInt32 operator-( std::uint32_t lhs, UInt8 rhs ) noexcept {
		lhs -= static_cast<std::uint32_t>( rhs );
		return static_cast<UInt32>( lhs );
	}

	constexpr UInt32 &operator*=( UInt32 &lhs, UInt32 rhs ) noexcept {
		lhs = static_cast<UInt32>( static_cast<std::uint32_t>( lhs ) *
		                           static_cast<std::uint32_t>( rhs ) );
		return lhs;
	}

	constexpr UInt32 &operator*=( UInt32 &lhs, UInt16 rhs ) noexcept {
		lhs = static_cast<UInt32>( static_cast<std::uint32_t>( lhs ) *
		                           static_cast<std::uint32_t>( rhs ) );
		return lhs;
	}

	constexpr UInt32 &operator*=( UInt32 &lhs, UInt8 rhs ) noexcept {
		lhs = static_cast<UInt32>( static_cast<std::uint32_t>( lhs ) *
		                           static_cast<std::uint32_t>( rhs ) );
		return lhs;
	}

	constexpr UInt32 &operator*=( UInt32 &lhs, std::uint32_t rhs ) noexcept {
		lhs = static_cast<UInt32>( static_cast<std::uint32_t>( lhs ) * rhs );
		return lhs;
	}

	constexpr std::uint32_t &operator*=( std::uint32_t &lhs,
	                                     UInt32 rhs ) noexcept {
		lhs *= static_cast<std::uint32_t>( rhs );
		return lhs;
	}

	constexpr std::uint32_t &operator*=( std::uint32_t &lhs,
	                                     UInt16 rhs ) noexcept {
		lhs *= static_cast<std::uint32_t>( rhs );
		return lhs;
	}

	constexpr std::uint32_t &operator*=( std::uint32_t &lhs,
	                                     UInt8 rhs ) noexcept {
		lhs *= static_cast<std::uint32_t>( rhs );
		return lhs;
	}

	constexpr UInt32 operator*( UInt32 lhs, UInt32 rhs ) noexcept {
		return static_cast<UInt32>( static_cast<std::uint32_t>( lhs ) *
		                            static_cast<std::uint32_t>( rhs ) );
	}

	constexpr UInt32 operator*( UInt32 lhs, UInt16 rhs ) noexcept {
		return static_cast<UInt32>( static_cast<std::uint32_t>( lhs ) *
		                            static_cast<std::uint32_t>( rhs ) );
	}

	constexpr UInt32 operator*( UInt32 lhs, UInt8 rhs ) noexcept {
		return static_cast<UInt32>( static_cast<std::uint32_t>( lhs ) *
		                            static_cast<std::uint32_t>( rhs ) );
	}

	constexpr UInt32 operator*( UInt16 lhs, UInt32 rhs ) noexcept {
		return static_cast<UInt32>( static_cast<std::uint32_t>( lhs ) *
		                            static_cast<std::uint32_t>( rhs ) );
	}

	constexpr UInt32 operator*( UInt8 lhs, UInt32 rhs ) noexcept {
		return static_cast<UInt32>( static_cast<std::uint32_t>( lhs ) *
		                            static_cast<std::uint32_t>( rhs ) );
	}

	constexpr UInt32 operator*( UInt32 lhs, std::uint32_t rhs ) noexcept {
		return static_cast<UInt32>( static_cast<std::uint32_t>( lhs ) * rhs );
	}

	constexpr UInt32 operator*( std::uint32_t lhs, UInt32 rhs ) noexcept {
		lhs *= static_cast<std::uint32_t>( rhs );
		return static_cast<UInt32>( lhs );
	}

	constexpr UInt32 operator*( std::uint32_t lhs, UInt16 rhs ) noexcept {
		lhs *= static_cast<std::uint32_t>( rhs );
		return static_cast<UInt32>( lhs );
	}

	constexpr UInt32 operator*( std::uint32_t lhs, UInt8 rhs ) noexcept {
		lhs *= static_cast<std::uint32_t>( rhs );
		return static_cast<UInt32>( lhs );
	}

	constexpr UInt32 &operator/=( UInt32 &lhs, UInt32 rhs ) noexcept {
		lhs = static_cast<UInt32>( static_cast<std::uint32_t>( lhs ) /
		                           static_cast<std::uint64_t>( rhs ) );
		return lhs;
	}

	constexpr UInt32 &operator/=( UInt32 &lhs, UInt16 rhs ) noexcept {
		lhs = static_cast<UInt32>( static_cast<std::uint32_t>( lhs ) /
		                           static_cast<std::uint64_t>( rhs ) );
		return lhs;
	}

	constexpr UInt32 &operator/=( UInt32 &lhs, UInt8 rhs ) noexcept {
		lhs = static_cast<UInt32>( static_cast<std::uint32_t>( lhs ) /
		                           static_cast<std::uint64_t>( rhs ) );
		return lhs;
	}

	constexpr UInt32 &operator/=( UInt32 &lhs, std::uint32_t rhs ) noexcept {
		lhs = static_cast<UInt32>( static_cast<std::uint32_t>( lhs ) / rhs );
		return lhs;
	}

	constexpr std::uint32_t &operator/=( std::uint32_t &lhs,
	                                     UInt32 rhs ) noexcept {
		lhs /= static_cast<std::uint32_t>( rhs );
		return lhs;
	}

	constexpr UInt32 operator/( UInt32 lhs, UInt32 rhs ) noexcept {
		return static_cast<UInt32>( static_cast<std::uint32_t>( lhs ) /
		                            static_cast<std::uint64_t>( rhs ) );
	}

	constexpr UInt32 operator/( UInt32 lhs, UInt16 rhs ) noexcept {
		return static_cast<UInt32>( static_cast<std::uint32_t>( lhs ) /
		                            static_cast<std::uint64_t>( rhs ) );
	}

	constexpr UInt32 operator/( UInt32 lhs, UInt8 rhs ) noexcept {
		return static_cast<UInt32>( static_cast<std::uint32_t>( lhs ) /
		                            static_cast<std::uint64_t>( rhs ) );
	}

	constexpr UInt32 operator/( UInt16 lhs, UInt32 rhs ) noexcept {
		return static_cast<UInt32>( static_cast<std::uint32_t>( lhs ) /
		                            static_cast<std::uint64_t>( rhs ) );
	}

	constexpr UInt32 operator/( UInt8 lhs, UInt32 rhs ) noexcept {
		return static_cast<UInt32>( static_cast<std::uint32_t>( lhs ) /
		                            static_cast<std::uint64_t>( rhs ) );
	}

	constexpr UInt32 operator/( UInt32 lhs, std::uint32_t rhs ) noexcept {
		lhs = static_cast<UInt32>( static_cast<std::uint32_t>( lhs ) / rhs );
		return lhs;
	}

	constexpr UInt32 operator/( std::uint32_t lhs, UInt32 rhs ) noexcept {
		lhs /= static_cast<std::uint32_t>( rhs );
		return static_cast<UInt32>( lhs );
	}

	constexpr UInt32 operator/( std::uint32_t lhs, UInt16 rhs ) noexcept {
		lhs /= static_cast<std::uint32_t>( rhs );
		return static_cast<UInt32>( lhs );
	}

	constexpr UInt32 operator/( std::uint32_t lhs, UInt8 rhs ) noexcept {
		lhs /= static_cast<std::uint32_t>( rhs );
		return static_cast<UInt32>( lhs );
	}

	constexpr bool operator!( UInt32 value ) {
		return !static_cast<std::uint32_t>( value );
	}

	constexpr UInt32 operator+( UInt32 value ) {
		return value;
	}

	constexpr UInt32 operator-( UInt32 value ) {
		return static_cast<UInt32>( -static_cast<std::uint32_t>( value ) );
	}

	constexpr UInt32 &operator++( UInt32 &value ) {
		value = static_cast<UInt32>( static_cast<std::uint32_t>( value ) + 1 );
		return value;
	}

	constexpr UInt32 operator++( UInt32 &value, int ) {
		auto result = value;
		value = static_cast<UInt32>( static_cast<std::uint32_t>( value ) + 1 );
		return result;
	}

	constexpr UInt32 &operator--( UInt32 &value ) {
		value = static_cast<UInt32>( static_cast<std::uint32_t>( value ) - 1 );
		return value;
	}

	constexpr UInt32 operator--( UInt32 &value, int ) {
		auto result = value;
		value = static_cast<UInt32>( static_cast<std::uint32_t>( value ) - 1 );
		return result;
	}

	constexpr UInt32 operator~( UInt32 value ) {
		return static_cast<UInt32>( ~static_cast<std::uint32_t>( value ) );
	}
	//*********
	constexpr UInt16 &operator+=( UInt16 &lhs, UInt16 rhs ) noexcept {
		lhs = static_cast<UInt16>( static_cast<std::uint32_t>( lhs ) +
		                           static_cast<std::uint32_t>( rhs ) );
		return lhs;
	}

	constexpr UInt16 &operator+=( UInt16 &lhs, UInt8 rhs ) noexcept {
		lhs = static_cast<UInt16>( static_cast<std::uint32_t>( lhs ) +
		                           static_cast<std::uint32_t>( rhs ) );
		return lhs;
	}

	constexpr UInt16 &operator+=( UInt16 &lhs, std::uint16_t rhs ) noexcept {
		lhs = static_cast<UInt16>( static_cast<std::uint32_t>( lhs ) + rhs );
		return lhs;
	}

	constexpr std::uint16_t &operator+=( std::uint16_t &lhs,
	                                     UInt16 rhs ) noexcept {
		lhs += static_cast<std::uint32_t>( rhs );
		return lhs;
	}

	constexpr std::uint16_t &operator+=( std::uint16_t &lhs,
	                                     UInt8 rhs ) noexcept {
		lhs += static_cast<std::uint32_t>( rhs );
		return lhs;
	}

	constexpr UInt16 operator+( UInt16 lhs, UInt16 rhs ) noexcept {
		lhs = static_cast<UInt16>( static_cast<std::uint32_t>( lhs ) +
		                           static_cast<std::uint32_t>( rhs ) );
		return lhs;
	}

	constexpr UInt16 operator+( UInt16 lhs, UInt8 rhs ) noexcept {
		lhs = static_cast<UInt16>( static_cast<std::uint32_t>( lhs ) +
		                           static_cast<std::uint32_t>( rhs ) );
		return lhs;
	}

	constexpr UInt16 operator+( UInt8 lhs, UInt16 rhs ) noexcept {
		return static_cast<UInt16>( static_cast<std::uint32_t>( lhs ) +
		                            static_cast<std::uint32_t>( rhs ) );
	}

	constexpr UInt16 operator+( UInt16 lhs, std::uint16_t rhs ) noexcept {
		lhs = static_cast<UInt16>( static_cast<std::uint32_t>( lhs ) + rhs );
		return lhs;
	}

	constexpr UInt16 operator+( UInt8 lhs, std::uint16_t rhs ) noexcept {
		return static_cast<UInt16>( static_cast<std::uint32_t>( lhs ) + rhs );
	}

	constexpr UInt16 operator+( std::uint16_t lhs, UInt16 rhs ) noexcept {
		return static_cast<UInt16>( static_cast<std::uint16_t>( lhs ) +
		                            static_cast<std::uint16_t>( rhs ) );
	}

	constexpr UInt16 &operator-=( UInt16 &lhs, UInt16 rhs ) noexcept {
		lhs = static_cast<UInt16>( static_cast<std::uint32_t>( lhs ) -
		                           static_cast<std::uint32_t>( rhs ) );
		return lhs;
	}

	constexpr UInt16 &operator-=( UInt16 &lhs, UInt8 rhs ) noexcept {
		lhs = static_cast<UInt16>( static_cast<std::uint16_t>( lhs ) -
		                           static_cast<std::uint16_t>( rhs ) );
		return lhs;
	}

	constexpr UInt16 &operator-=( UInt16 &lhs, std::uint16_t rhs ) noexcept {
		lhs = static_cast<UInt16>( static_cast<std::uint16_t>( lhs ) - rhs );
		return lhs;
	}

	constexpr std::uint16_t &operator-=( std::uint16_t &lhs,
	                                     UInt16 rhs ) noexcept {
		lhs -= static_cast<std::uint16_t>( rhs );
		return lhs;
	}

	constexpr std::uint16_t &operator-=( std::uint16_t &lhs,
	                                     UInt8 rhs ) noexcept {
		lhs -= static_cast<std::uint16_t>( rhs );
		return lhs;
	}

	constexpr UInt16 operator-( UInt16 lhs, UInt16 rhs ) noexcept {
		return static_cast<UInt16>( static_cast<std::uint16_t>( lhs ) -
		                            static_cast<std::uint16_t>( rhs ) );
	}

	constexpr UInt16 operator-( UInt16 lhs, UInt8 rhs ) noexcept {
		return static_cast<UInt16>( static_cast<std::uint16_t>( lhs ) -
		                            static_cast<std::uint16_t>( rhs ) );
	}

	constexpr UInt16 operator-( UInt8 lhs, UInt16 rhs ) noexcept {
		return static_cast<UInt16>( static_cast<std::uint16_t>( lhs ) -
		                            static_cast<std::uint16_t>( rhs ) );
	}

	constexpr UInt16 operator-( UInt16 lhs, std::uint16_t rhs ) noexcept {
		return static_cast<UInt16>( static_cast<std::uint16_t>( lhs ) - rhs );
	}

	constexpr UInt16 operator-( UInt8 lhs, std::uint16_t rhs ) noexcept {
		return static_cast<UInt16>( static_cast<std::uint16_t>( lhs ) - rhs );
	}

	constexpr UInt16 operator-( std::uint16_t lhs, UInt16 rhs ) noexcept {
		return static_cast<UInt16>( static_cast<std::uint16_t>( lhs ) -
		                            static_cast<std::uint16_t>( rhs ) );
	}

	constexpr UInt16 operator-( std::uint16_t lhs, UInt8 rhs ) noexcept {
		return static_cast<UInt16>( static_cast<std::uint16_t>( lhs ) -
		                            static_cast<std::uint16_t>( rhs ) );
	}

	constexpr UInt16 &operator*=( UInt16 &lhs, UInt16 rhs ) noexcept {
		lhs = static_cast<UInt16>( static_cast<std::uint32_t>( lhs ) *
		                           static_cast<std::uint32_t>( rhs ) );
		return lhs;
	}

	constexpr UInt16 &operator*=( UInt16 &lhs, UInt8 rhs ) noexcept {
		lhs = static_cast<UInt16>( static_cast<std::uint32_t>( lhs ) *
		                           static_cast<std::uint32_t>( rhs ) );
		return lhs;
	}

	constexpr UInt16 &operator*=( UInt16 &lhs, std::uint16_t rhs ) noexcept {
		lhs = static_cast<UInt16>( static_cast<std::uint32_t>( lhs ) * rhs );
		return lhs;
	}

	constexpr std::uint16_t &operator*=( std::uint16_t &lhs,
	                                     UInt16 rhs ) noexcept {
		lhs *= static_cast<std::uint32_t>( rhs );
		return lhs;
	}

	constexpr std::uint16_t &operator*=( std::uint16_t &lhs,
	                                     UInt8 rhs ) noexcept {
		lhs *= static_cast<std::uint32_t>( rhs );
		return lhs;
	}

	constexpr UInt16 operator*( UInt16 lhs, UInt16 rhs ) noexcept {
		return static_cast<UInt16>( static_cast<std::uint32_t>( lhs ) *
		                            static_cast<std::uint32_t>( rhs ) );
	}

	constexpr UInt16 operator*( UInt16 lhs, UInt8 rhs ) noexcept {
		return static_cast<UInt16>( static_cast<std::uint32_t>( lhs ) *
		                            static_cast<std::uint32_t>( rhs ) );
	}
	constexpr UInt16 operator*( UInt8 lhs, UInt16 rhs ) noexcept {
		return static_cast<UInt16>( static_cast<std::uint32_t>( lhs ) *
		                            static_cast<std::uint32_t>( rhs ) );
	}

	constexpr UInt16 operator*( UInt16 lhs, std::uint16_t rhs ) noexcept {
		return static_cast<UInt16>( static_cast<std::uint32_t>( lhs ) *
		                            static_cast<std::uint32_t>( rhs ) );
	}

	constexpr UInt16 operator*( UInt8 lhs, std::uint16_t rhs ) noexcept {
		return static_cast<UInt16>( static_cast<std::uint32_t>( lhs ) *
		                            static_cast<std::uint32_t>( rhs ) );
	}

	constexpr UInt16 operator*( std::uint16_t lhs, UInt16 rhs ) noexcept {
		return static_cast<UInt16>( static_cast<std::uint32_t>( lhs ) *
		                            static_cast<std::uint32_t>( rhs ) );
	}

	constexpr UInt16 &operator/=( UInt16 &lhs, UInt16 rhs ) noexcept {
		lhs = static_cast<UInt16>( static_cast<std::uint32_t>( lhs ) /
		                           static_cast<std::uint32_t>( rhs ) );
		return lhs;
	}

	constexpr UInt16 &operator/=( UInt16 &lhs, UInt8 rhs ) noexcept {
		lhs = static_cast<UInt16>( static_cast<std::uint32_t>( lhs ) /
		                           static_cast<std::uint32_t>( rhs ) );
		return lhs;
	}

	constexpr UInt16 &operator/=( UInt16 &lhs, std::uint16_t rhs ) noexcept {
		lhs = static_cast<UInt16>( static_cast<std::uint16_t>( lhs ) / rhs );
		return lhs;
	}

	constexpr std::uint16_t &operator/=( std::uint16_t &lhs,
	                                     UInt16 rhs ) noexcept {
		lhs /= static_cast<std::uint32_t>( rhs );
		return lhs;
	}

	constexpr std::uint16_t &operator/=( std::uint16_t &lhs,
	                                     UInt8 rhs ) noexcept {
		lhs /= static_cast<std::uint16_t>( rhs );
		return lhs;
	}

	constexpr UInt16 operator/( UInt16 lhs, UInt16 rhs ) noexcept {
		return static_cast<UInt16>( static_cast<std::uint32_t>( lhs ) /
		                            static_cast<std::uint32_t>( rhs ) );
	}

	constexpr UInt16 operator/( UInt16 lhs, UInt8 rhs ) noexcept {
		return static_cast<UInt16>( static_cast<std::uint32_t>( lhs ) /
		                            static_cast<std::uint32_t>( rhs ) );
	}

	constexpr UInt16 operator/( UInt8 lhs, UInt16 rhs ) noexcept {
		return static_cast<UInt16>( static_cast<std::uint32_t>( lhs ) /
		                            static_cast<std::uint32_t>( rhs ) );
	}

	constexpr UInt16 operator/( UInt16 lhs, std::uint16_t rhs ) noexcept {
		return static_cast<UInt16>( static_cast<std::uint32_t>( lhs ) /
		                            static_cast<std::uint32_t>( rhs ) );
	}

	constexpr UInt16 operator/( std::uint16_t lhs, UInt16 rhs ) noexcept {
		lhs /= static_cast<std::uint16_t>( rhs );
		return static_cast<UInt16>( lhs );
	}

	constexpr UInt16 operator/( std::uint16_t lhs, UInt8 rhs ) noexcept {
		lhs /= static_cast<std::uint16_t>( rhs );
		return static_cast<UInt16>( lhs );
	}

	constexpr bool operator!( UInt16 value ) {
		return !static_cast<std::uint32_t>( value );
	}

	constexpr UInt16 operator+( UInt16 value ) {
		return value;
	}

	constexpr UInt16 operator-( UInt16 value ) {
		return static_cast<UInt16>( -static_cast<std::uint32_t>( value ) );
	}

	constexpr UInt16 &operator++( UInt16 &value ) {
		value = static_cast<UInt16>( static_cast<std::uint32_t>( value ) + 1 );
		return value;
	}

	constexpr UInt16 operator++( UInt16 &value, int ) {
		auto result = value;
		value = static_cast<UInt16>( static_cast<std::uint32_t>( value ) + 1 );
		return result;
	}

	constexpr UInt16 &operator--( UInt16 &value ) {
		value = static_cast<UInt16>( static_cast<std::uint32_t>( value ) - 1 );
		return value;
	}

	constexpr UInt16 operator--( UInt16 &value, int ) {
		auto result = value;
		value = static_cast<UInt16>( static_cast<std::uint32_t>( value ) - 1 );
		return result;
	}

	constexpr UInt16 operator~( UInt16 value ) {
		return static_cast<UInt16>( ~static_cast<std::uint32_t>( value ) );
	}

	template<unsigned bits>
	constexpr UInt16 rotate_left( UInt16 value ) {
		static_assert( bits <= 16 );
		return ( value << bits ) | ( value >> ( 16U - bits ) );
	}

	constexpr UInt16 rotate_left( UInt16 value, unsigned bits ) {
		return ( value << bits ) | ( value >> ( 16U - bits ) );
	}

	template<unsigned bits>
	constexpr UInt16 rotate_right( UInt16 value ) {
		static_assert( bits <= 16 );
		return ( value >> bits ) | ( value << ( 16U - bits ) );
	}

	constexpr UInt16 rotate_right( UInt16 value, unsigned bits ) {
		return ( value >> bits ) | ( value << ( 16U - bits ) );
	}

	constexpr UInt8 &operator+=( UInt8 &lhs, UInt8 rhs ) noexcept {
		lhs = static_cast<UInt8>( static_cast<std::uint32_t>( lhs ) +
		                          static_cast<std::uint32_t>( rhs ) );
		return lhs;
	}

	constexpr UInt8 &operator+=( UInt8 &lhs, std::uint8_t rhs ) noexcept {
		lhs = static_cast<UInt8>( static_cast<std::uint32_t>( lhs ) + rhs );
		return lhs;
	}

	constexpr std::uint8_t &operator+=( std::uint8_t &lhs, UInt8 rhs ) noexcept {
		lhs += static_cast<std::uint32_t>( rhs );
		return lhs;
	}

	constexpr UInt8 operator+( UInt8 lhs, UInt8 rhs ) noexcept {
		lhs = static_cast<UInt8>( static_cast<std::uint32_t>( lhs ) +
		                          static_cast<std::uint32_t>( rhs ) );
		return lhs;
	}

	constexpr UInt8 operator+( UInt8 lhs, std::uint8_t rhs ) noexcept {
		lhs = static_cast<UInt8>( static_cast<std::uint32_t>( lhs ) + rhs );
		return lhs;
	}

	constexpr UInt8 operator+( std::uint8_t lhs, UInt8 rhs ) noexcept {
		lhs += static_cast<std::uint32_t>( rhs );
		return static_cast<UInt8>( lhs );
	}

	constexpr UInt8 &operator-=( UInt8 &lhs, UInt8 rhs ) noexcept {
		lhs = static_cast<UInt8>( static_cast<std::uint32_t>( lhs ) -
		                          static_cast<std::uint32_t>( rhs ) );
		return lhs;
	}

	constexpr UInt8 &operator-=( UInt8 &lhs, std::uint8_t rhs ) noexcept {
		lhs = static_cast<UInt8>( static_cast<std::uint32_t>( lhs ) - rhs );
		return lhs;
	}

	constexpr std::uint8_t &operator-=( std::uint8_t &lhs, UInt8 rhs ) noexcept {
		lhs -= static_cast<std::uint32_t>( rhs );
		return lhs;
	}

	constexpr UInt8 operator-( UInt8 lhs, UInt8 rhs ) noexcept {
		lhs = static_cast<UInt8>( static_cast<std::uint32_t>( lhs ) -
		                          static_cast<std::uint32_t>( rhs ) );
		return lhs;
	}

	constexpr UInt8 operator-( UInt8 lhs, std::uint8_t rhs ) noexcept {
		lhs = static_cast<UInt8>( static_cast<std::uint32_t>( lhs ) - rhs );
		return lhs;
	}

	constexpr UInt8 operator-( std::uint8_t lhs, UInt8 rhs ) noexcept {
		lhs -= static_cast<std::uint32_t>( rhs );
		return static_cast<UInt8>( lhs );
	}

	constexpr UInt8 &operator*=( UInt8 &lhs, UInt8 rhs ) noexcept {
		lhs = static_cast<UInt8>( static_cast<std::uint32_t>( lhs ) *
		                          static_cast<std::uint32_t>( rhs ) );
		return lhs;
	}

	constexpr UInt8 &operator*=( UInt8 &lhs, std::uint8_t rhs ) noexcept {
		lhs = static_cast<UInt8>( static_cast<std::uint32_t>( lhs ) * rhs );
		return lhs;
	}

	constexpr std::uint8_t &operator*=( std::uint8_t &lhs, UInt8 rhs ) noexcept {
		lhs *= static_cast<std::uint32_t>( rhs );
		return lhs;
	}

	constexpr UInt8 operator*( UInt8 lhs, UInt8 rhs ) noexcept {
		lhs = static_cast<UInt8>( static_cast<std::uint32_t>( lhs ) *
		                          static_cast<std::uint32_t>( rhs ) );
		return lhs;
	}

	constexpr UInt8 operator*( UInt8 lhs, std::uint64_t rhs ) noexcept {
		lhs = static_cast<UInt8>( static_cast<std::uint32_t>( lhs ) * rhs );
		return lhs;
	}

	constexpr UInt8 operator*( std::uint8_t lhs, UInt64 rhs ) noexcept {
		lhs *= static_cast<std::uint32_t>( rhs );
		return static_cast<UInt8>( lhs );
	}

	constexpr UInt8 &operator/=( UInt8 &lhs, UInt8 rhs ) noexcept {
		lhs = static_cast<UInt8>( static_cast<std::uint32_t>( lhs ) /
		                          static_cast<std::uint64_t>( rhs ) );
		return lhs;
	}

	constexpr UInt8 &operator/=( UInt8 &lhs, std::uint64_t rhs ) noexcept {
		lhs = static_cast<UInt8>( static_cast<std::uint32_t>( lhs ) / rhs );
		return lhs;
	}

	constexpr std::uint8_t &operator/=( std::uint8_t &lhs, UInt64 rhs ) noexcept {
		lhs /= static_cast<std::uint32_t>( rhs );
		return lhs;
	}

	constexpr UInt8 operator/( UInt8 lhs, UInt8 rhs ) noexcept {
		lhs = static_cast<UInt8>( static_cast<std::uint32_t>( lhs ) /
		                          static_cast<std::uint64_t>( rhs ) );
		return lhs;
	}

	constexpr UInt8 operator/( UInt8 lhs, std::uint64_t rhs ) noexcept {
		lhs = static_cast<UInt8>( static_cast<std::uint32_t>( lhs ) / rhs );
		return lhs;
	}

	constexpr UInt8 operator/( std::uint8_t lhs, UInt64 rhs ) noexcept {
		lhs /= static_cast<std::uint32_t>( rhs );
		return static_cast<UInt8>( lhs );
	}

	constexpr bool operator!( UInt8 value ) {
		return !static_cast<std::uint32_t>( value );
	}

	constexpr UInt8 operator+( UInt8 value ) {
		return value;
	}

	constexpr UInt8 operator-( UInt8 value ) {
		return static_cast<UInt8>( -static_cast<std::uint32_t>( value ) );
	}

	constexpr UInt8 &operator++( UInt8 &value ) {
		value = static_cast<UInt8>( static_cast<std::uint32_t>( value ) + 1 );
		return value;
	}

	constexpr UInt8 operator++( UInt8 &value, int ) {
		auto result = value;
		value = static_cast<UInt8>( static_cast<std::uint32_t>( value ) + 1 );
		return result;
	}

	constexpr UInt8 &operator--( UInt8 &value ) {
		value = static_cast<UInt8>( static_cast<std::uint32_t>( value ) - 1 );
		return value;
	}

	constexpr UInt8 operator--( UInt8 &value, int ) {
		auto result = value;
		value = static_cast<UInt8>( static_cast<std::uint32_t>( value ) - 1 );
		return result;
	}

	constexpr UInt8 operator~( UInt8 value ) {
		return static_cast<UInt8>( ~static_cast<std::uint32_t>( value ) );
	}
	constexpr UInt8 &operator<<=( UInt8 &b, std::uint8_t shift ) noexcept {

		auto tmp = static_cast<std::uint32_t>( b );
		tmp <<= static_cast<std::uint32_t>( shift );
		b = static_cast<UInt8>( tmp );
		return b;
	}

	constexpr UInt8 &operator<<=( UInt8 &b, UInt8 shift ) noexcept {
		auto tmp = static_cast<std::uint32_t>( b );
		tmp <<= static_cast<std::uint32_t>( shift );
		b = static_cast<UInt8>( tmp );
		return b;
	}

	constexpr UInt8 &operator>>=( UInt8 &b, std::uint8_t shift ) noexcept {

		auto tmp = static_cast<std::uint32_t>( b );
		tmp >>= static_cast<std::uint32_t>( shift );
		b = static_cast<UInt8>( tmp );
		return b;
	}

	constexpr UInt8 &operator>>=( UInt8 &b, UInt8 shift ) noexcept {
		auto tmp = static_cast<std::uint32_t>( b );
		tmp >>= static_cast<std::uint32_t>( shift );
		b = static_cast<UInt8>( tmp );
		return b;
	}

	constexpr UInt8 operator<<( UInt8 b, std::uint8_t shift ) noexcept {

		auto tmp = static_cast<std::uint32_t>( b );
		tmp <<= static_cast<std::uint32_t>( shift );
		return static_cast<UInt8>( tmp );
	}

	constexpr UInt8 operator<<( UInt8 b, UInt8 shift ) noexcept {
		auto tmp = static_cast<std::uint32_t>( b );
		tmp <<= static_cast<std::uint32_t>( shift );
		return static_cast<UInt8>( tmp );
	}

	constexpr std::uint8_t operator<<( std::uint8_t b, UInt8 shift ) noexcept {

		b <<= static_cast<std::uint32_t>( shift );
		return b;
	}

	constexpr UInt8 operator>>( UInt8 b, std::uint8_t shift ) noexcept {

		auto tmp = static_cast<std::uint32_t>( b );
		tmp >>= static_cast<std::uint32_t>( shift );
		return static_cast<UInt8>( tmp );
	}

	constexpr UInt8 operator>>( UInt8 b, UInt8 shift ) noexcept {
		auto tmp = static_cast<std::uint32_t>( b );
		tmp >>= static_cast<std::uint32_t>( shift );
		return static_cast<UInt8>( tmp );
	}

	constexpr std::uint8_t operator>>( std::uint8_t b, UInt8 shift ) noexcept {

		b >>= static_cast<std::uint32_t>( shift );
		return b;
	}

	constexpr UInt8 &operator|=( UInt8 &lhs, std::uint8_t rhs ) noexcept {

		auto tmp = static_cast<std::uint32_t>( lhs );
		tmp |= static_cast<std::uint32_t>( rhs );
		lhs = static_cast<UInt8>( tmp );
		return lhs;
	}

	constexpr UInt8 &operator|=( UInt8 &lhs, UInt8 rhs ) noexcept {
		auto tmp = static_cast<std::uint32_t>( lhs );
		tmp |= static_cast<std::uint32_t>( rhs );
		lhs = static_cast<UInt8>( tmp );
		return lhs;
	}

	constexpr UInt8 &operator&=( UInt8 &lhs, std::uint8_t rhs ) noexcept {

		auto tmp = static_cast<std::uint32_t>( lhs );
		tmp &= static_cast<std::uint32_t>( rhs );
		lhs = static_cast<UInt8>( tmp );
		return lhs;
	}

	constexpr UInt8 &operator&=( UInt8 &lhs, UInt8 rhs ) noexcept {
		auto tmp = static_cast<std::uint32_t>( lhs );
		tmp &= static_cast<std::uint32_t>( rhs );
		lhs = static_cast<UInt8>( tmp );
		return lhs;
	}

	constexpr UInt8 &operator^=( UInt8 &lhs, std::uint8_t rhs ) noexcept {

		auto tmp = static_cast<std::uint32_t>( lhs );
		tmp ^= static_cast<std::uint32_t>( rhs );
		lhs = static_cast<UInt8>( tmp );
		return lhs;
	}

	constexpr UInt8 &operator^=( UInt8 &lhs, UInt8 rhs ) noexcept {
		auto tmp = static_cast<std::uint32_t>( lhs );
		tmp ^= static_cast<std::uint32_t>( rhs );
		lhs = static_cast<UInt8>( tmp );
		return lhs;
	}

	constexpr bool operator>( UInt8 lhs, std::uint8_t rhs ) noexcept {

		return static_cast<std::uint32_t>( lhs ) >
		       static_cast<std::uint32_t>( rhs );
	}

	constexpr bool operator>( std::uint8_t lhs, UInt8 rhs ) noexcept {

		return static_cast<std::uint32_t>( lhs ) >
		       static_cast<std::uint32_t>( rhs );
	}

	constexpr bool operator>=( UInt8 lhs, std::uint8_t rhs ) noexcept {

		return static_cast<std::uint32_t>( lhs ) >=
		       static_cast<std::uint32_t>( rhs );
	}

	constexpr bool operator>=( std::uint8_t lhs, UInt8 rhs ) noexcept {

		return static_cast<std::uint32_t>( lhs ) >=
		       static_cast<std::uint32_t>( rhs );
	}

	constexpr bool operator<( UInt8 lhs, std::uint8_t rhs ) noexcept {

		return static_cast<std::uint32_t>( lhs ) <
		       static_cast<std::uint32_t>( rhs );
	}

	constexpr bool operator<( std::uint8_t lhs, UInt8 rhs ) noexcept {

		return static_cast<std::uint32_t>( lhs ) <
		       static_cast<std::uint32_t>( rhs );
	}

	constexpr bool operator<=( UInt8 lhs, std::uint8_t rhs ) noexcept {

		return static_cast<std::uint32_t>( lhs ) <=
		       static_cast<std::uint32_t>( rhs );
	}

	constexpr bool operator<=( std::uint8_t lhs, UInt8 rhs ) noexcept {

		return static_cast<std::uint32_t>( lhs ) <=
		       static_cast<std::uint32_t>( rhs );
	}

	constexpr UInt8 operator|( UInt8 lhs, std::uint8_t rhs ) noexcept {

		return static_cast<UInt8>( static_cast<std::uint32_t>( lhs ) |
		                           static_cast<std::uint32_t>( rhs ) );
	}

	constexpr UInt8 operator|( std::uint8_t lhs, UInt8 rhs ) noexcept {

		return static_cast<UInt8>( static_cast<std::uint32_t>( lhs ) |
		                           static_cast<std::uint32_t>( rhs ) );
	}

	constexpr UInt8 operator&( UInt8 lhs, std::uint8_t rhs ) noexcept {

		return static_cast<UInt8>( static_cast<std::uint32_t>( lhs ) &
		                           static_cast<std::uint32_t>( rhs ) );
	}

	constexpr UInt8 operator&( std::uint8_t lhs, UInt8 rhs ) noexcept {

		return static_cast<UInt8>( static_cast<std::uint32_t>( lhs ) &
		                           static_cast<std::uint32_t>( rhs ) );
	}

	constexpr UInt8 operator^( UInt8 lhs, std::uint8_t rhs ) noexcept {

		return static_cast<UInt8>( static_cast<std::uint32_t>( lhs ) ^
		                           static_cast<std::uint32_t>( rhs ) );
	}

	constexpr UInt8 operator^( std::uint8_t lhs, UInt8 rhs ) noexcept {

		return static_cast<UInt8>( static_cast<std::uint32_t>( lhs ) ^
		                           static_cast<std::uint32_t>( rhs ) );
	}

	constexpr bool operator==( UInt8 lhs, std::uint8_t rhs ) noexcept {

		return static_cast<std::uint32_t>( lhs ) ==
		       static_cast<std::uint32_t>( rhs );
	}

	constexpr bool operator==( std::uint8_t lhs, UInt8 rhs ) noexcept {

		return static_cast<std::uint32_t>( lhs ) ==
		       static_cast<std::uint32_t>( rhs );
	}

	constexpr bool operator!=( UInt8 lhs, std::uint8_t rhs ) noexcept {

		return static_cast<std::uint32_t>( lhs ) !=
		       static_cast<std::uint32_t>( rhs );
	}

	constexpr bool operator!=( std::uint8_t lhs, UInt8 rhs ) noexcept {

		return static_cast<std::uint32_t>( lhs ) !=
		       static_cast<std::uint32_t>( rhs );
	}

	constexpr UInt8 operator|( UInt8 lhs, UInt8 rhs ) noexcept {
		return static_cast<UInt8>( static_cast<std::uint32_t>( lhs ) |
		                           static_cast<std::uint32_t>( rhs ) );
	}

	constexpr UInt8 operator&( UInt8 lhs, UInt8 rhs ) noexcept {
		return static_cast<UInt8>( static_cast<std::uint32_t>( lhs ) &
		                           static_cast<std::uint32_t>( rhs ) );
	}

	constexpr UInt8 operator^( UInt8 lhs, UInt8 rhs ) noexcept {
		return static_cast<UInt8>( static_cast<std::uint32_t>( lhs ) ^
		                           static_cast<std::uint32_t>( rhs ) );
	}

	template<unsigned N>
	inline constexpr void set_bit( UInt64 &value, bool state ) {
		static_assert( N <= 64 );
		value =
		  ( value & ( ~( 1U << N ) ) ) | ( static_cast<UInt64>( state ) << N );
	}

	template<unsigned N>
	inline constexpr UInt64 get_bit( UInt64 value ) {
		static_assert( N <= 64 );
		return ( value >> N ) & 1U;
	}

	template<unsigned N>
	inline constexpr void set_bit( UInt32 &value, bool state ) {
		static_assert( N <= 32 );
		value =
		  ( value & ( ~( 1U << N ) ) ) | ( static_cast<UInt32>( state ) << N );
	}

	template<unsigned N>
	inline constexpr UInt32 get_bit( UInt32 value ) {
		static_assert( N <= 32 );
		return ( value >> N ) & 1U;
	}

	template<unsigned N>
	inline constexpr void set_bit( UInt16 &value, bool state ) {
		static_assert( N <= 16 );
		value =
		  ( value & ( ~( 1U << N ) ) ) | ( static_cast<UInt16>( state ) << N );
	}

	template<unsigned N>
	inline constexpr UInt16 get_bit( UInt16 value ) {
		static_assert( N <= 16 );
		return ( value >> N ) & 1U;
	}

	template<unsigned N>
	inline constexpr void set_bit( UInt8 &value, bool state ) {
		static_assert( N <= 8 );
		value = ( value & ( ~( 1U << N ) ) ) | ( static_cast<UInt8>( state ) << N );
	}

	template<unsigned N>
	inline constexpr UInt8 get_bit( UInt8 value ) {
		static_assert( N <= 8 );
		return ( value >> N ) & 1U;
	}

	template<unsigned bits>
	constexpr UInt8 rotate_left( UInt8 value ) {
		static_assert( bits <= 8 );
		return ( value << bits ) | ( value >> ( 8U - bits ) );
	}

	constexpr UInt8 rotate_left( UInt8 value, unsigned bits ) {
		return ( value << bits ) | ( value >> ( 8U - bits ) );
	}

	template<unsigned bits>
	constexpr UInt8 rotate_right( UInt8 value ) {
		static_assert( bits <= 8 );
		return ( value >> bits ) | ( value << ( 8U - bits ) );
	}

	constexpr UInt8 rotate_right( UInt8 value, unsigned bits ) {
		return ( value >> bits ) | ( value << ( 8U - bits ) );
	}

	template<unsigned N>
	inline constexpr UInt64 mask_from_lsb64 = [] {
		static_assert( N <= 64 );
		if constexpr( N == 0 ) {
			return UInt64( );
		} else {
			auto const One = ~to_uint64( 1U );
			return ~( One << ( N - 1 ) );
		}
	}( );

	template<unsigned N>
	inline constexpr UInt32 mask_from_lsb32 = [] {
		static_assert( N <= 32 );
		if constexpr( N == 0 ) {
			return UInt32( );
		} else {
			auto const One = ~to_uint32( 1U );
			return ~( One << ( N - 1 ) );
		}
	}( );

	template<unsigned N>
	inline constexpr UInt16 mask_from_lsb16 = [] {
		static_assert( N <= 16 );
		if constexpr( N == 0 ) {
			return UInt16( );
		} else {
			auto const One = ~to_uint16( 1U );
			return ~( One << ( N - 1 ) );
		}
	}( );

	template<unsigned N>
	inline constexpr UInt8 mask_from_lsb8 = [] {
		static_assert( N <= 8 );
		if constexpr( N == 0 ) {
			return UInt8( );
		} else {
			auto const One = ~to_uint8( 1U );
			return ~( One << ( N - 1 ) );
		}
	}( );

	constexpr UInt64 to_uint64_buffer( char const *ptr ) noexcept {
		std::uint64_t result = 0;
		result |= static_cast<std::uint64_t>( static_cast<unsigned char>( ptr[7] ) )
		          << 56U;
		result |= static_cast<std::uint64_t>( static_cast<unsigned char>( ptr[6] ) )
		          << 48U;
		result |= static_cast<std::uint64_t>( static_cast<unsigned char>( ptr[5] ) )
		          << 40U;
		result |= static_cast<std::uint64_t>( static_cast<unsigned char>( ptr[4] ) )
		          << 32U;
		result |= static_cast<std::uint64_t>( static_cast<unsigned char>( ptr[3] ) )
		          << 24U;
		result |= static_cast<std::uint64_t>( static_cast<unsigned char>( ptr[2] ) )
		          << 16U;
		result |= static_cast<std::uint64_t>( static_cast<unsigned char>( ptr[1] ) )
		          << 8U;
		result |=
		  static_cast<std::uint64_t>( static_cast<unsigned char>( ptr[0] ) );
		return static_cast<UInt64>( result );
	}

	constexpr UInt32 to_uint32_buffer( char const *ptr ) noexcept {
		std::uint32_t result = 0;
		result |= static_cast<std::uint32_t>( static_cast<unsigned char>( ptr[3] ) )
		          << 24U;
		result |= static_cast<std::uint32_t>( static_cast<unsigned char>( ptr[2] ) )
		          << 16U;
		result |= static_cast<std::uint32_t>( static_cast<unsigned char>( ptr[1] ) )
		          << 8U;
		result |=
		  static_cast<std::uint32_t>( static_cast<unsigned char>( ptr[0] ) );
		return static_cast<UInt32>( result );
	}

	constexpr UInt16 to_uint16_buffer( char const *ptr ) noexcept {
		std::uint16_t result = 0;
		result |= static_cast<std::uint32_t>( static_cast<unsigned char>( ptr[1] ) )
		          << 8U;
		result |=
		  static_cast<std::uint32_t>( static_cast<unsigned char>( ptr[0] ) );
		return static_cast<UInt16>( result );
	}

	constexpr UInt64 operator"" _u64( unsigned long long value ) {
		return static_cast<UInt64>( value );
	}

	constexpr UInt32 operator"" _u32( unsigned long long value ) {
		assert( ( value <= static_cast<unsigned long long>(
		                     daw::numeric_limits<std::uint32_t>::max( ) ) ) );
		return static_cast<UInt32>( value );
	}

	constexpr UInt16 operator"" _u16( unsigned long long value ) {
		assert( ( value < static_cast<unsigned long long>(
		                    std::numeric_limits<std::uint16_t>::max( ) ) ) );
		return static_cast<UInt16>( value );
	}

	constexpr UInt8 operator"" _u8( unsigned long long value ) {
		assert( ( value < static_cast<unsigned long long>(
		                    std::numeric_limits<std::uint8_t>::max( ) ) ) );
		return static_cast<UInt8>( value );
	}
} // namespace daw

namespace std {
	template<>
	struct numeric_limits<daw::UInt64> {
		static constexpr bool is_specialized = true;
		static constexpr bool is_signed = false;
		static constexpr bool is_integer = true;
		static constexpr bool is_exact = true;
		static constexpr bool has_infinity = false;
		static constexpr bool has_quiet_NaN = false;
		static constexpr bool has_signaling_NaN = false;
		static constexpr bool has_denorm = false;
		static constexpr bool has_denorm_loss = false;
		static constexpr std::float_round_style round_style =
		  std::round_toward_zero;
		static constexpr bool is_iec559 = false;
		static constexpr bool is_bounded = true;
		static constexpr bool is_modulo = true;
		static constexpr int digits =
		  static_cast<int>( sizeof( std::uint64_t ) * CHAR_BIT - is_signed );
		static constexpr int digits10 = digits * 3 / 10;
		static constexpr int max_digits10 = 0;
		static constexpr int radix = 2;
		static constexpr int min_exponent = 0;
		static constexpr int min_exponent10 = 0;
		static constexpr int max_exponent = 0;
		static constexpr int max_exponent10 = 0;
		// Cannot reasonibly define
		// static constexpr bool traps = true;
		static constexpr bool tinyness_before = false;

		static constexpr daw::UInt64 min( ) noexcept {
			return daw::UInt64( );
		}

		static constexpr daw::UInt64 lowest( ) noexcept {
			return daw::UInt64( );
		}

		static constexpr daw::UInt64 max( ) noexcept {
			return static_cast<daw::UInt64>(
			  std::numeric_limits<std::uint64_t>::max( ) );
		}

		static constexpr daw::UInt64 epsilon( ) noexcept {
			return daw::UInt64( );
		}

		static constexpr daw::UInt64 round_error( ) noexcept {
			return daw::UInt64( );
		}

		static constexpr daw::UInt64 infinity( ) noexcept {
			return daw::UInt64( );
		}

		static constexpr daw::UInt64 quiet_NaN( ) noexcept {
			return daw::UInt64( );
		}

		static constexpr daw::UInt64 signalling_NaN( ) noexcept {
			return daw::UInt64( );
		}

		static constexpr daw::UInt64 denorm_min( ) noexcept {
			return daw::UInt64( );
		}
	};

	template<>
	struct numeric_limits<daw::UInt32> {
		static constexpr bool is_specialized = true;
		static constexpr bool is_signed = false;
		static constexpr bool is_integer = true;
		static constexpr bool is_exact = true;
		static constexpr bool has_infinity = false;
		static constexpr bool has_quiet_NaN = false;
		static constexpr bool has_signaling_NaN = false;
		static constexpr bool has_denorm = false;
		static constexpr bool has_denorm_loss = false;
		static constexpr std::float_round_style round_style =
		  std::round_toward_zero;
		static constexpr bool is_iec559 = false;
		static constexpr bool is_bounded = true;
		static constexpr bool is_modulo = true;
		static constexpr int digits =
		  static_cast<int>( sizeof( std::uint32_t ) * CHAR_BIT - is_signed );
		static constexpr int digits10 = digits * 3 / 10;
		static constexpr int max_digits10 = 0;
		static constexpr int radix = 2;
		static constexpr int min_exponent = 0;
		static constexpr int min_exponent10 = 0;
		static constexpr int max_exponent = 0;
		static constexpr int max_exponent10 = 0;
		// Cannot reasonibly define
		// static constexpr bool traps = true;
		static constexpr bool tinyness_before = false;

		static constexpr daw::UInt32 min( ) noexcept {
			return daw::UInt32( );
		}

		static constexpr daw::UInt32 lowest( ) noexcept {
			return daw::UInt32( );
		}

		static constexpr daw::UInt32 max( ) noexcept {
			return static_cast<daw::UInt32>(
			  std::numeric_limits<std::uint32_t>::max( ) );
		}

		static constexpr daw::UInt32 epsilon( ) noexcept {
			return daw::UInt32( );
		}

		static constexpr daw::UInt32 round_error( ) noexcept {
			return daw::UInt32( );
		}

		static constexpr daw::UInt32 infinity( ) noexcept {
			return daw::UInt32( );
		}

		static constexpr daw::UInt32 quiet_NaN( ) noexcept {
			return daw::UInt32( );
		}

		static constexpr daw::UInt32 signalling_NaN( ) noexcept {
			return daw::UInt32( );
		}

		static constexpr daw::UInt32 denorm_min( ) noexcept {
			return daw::UInt32( );
		}
	};

	template<>
	struct numeric_limits<daw::UInt16> {
		static constexpr bool is_specialized = true;
		static constexpr bool is_signed = false;
		static constexpr bool is_integer = true;
		static constexpr bool is_exact = true;
		static constexpr bool has_infinity = false;
		static constexpr bool has_quiet_NaN = false;
		static constexpr bool has_signaling_NaN = false;
		static constexpr bool has_denorm = false;
		static constexpr bool has_denorm_loss = false;
		static constexpr std::float_round_style round_style =
		  std::round_toward_zero;
		static constexpr bool is_iec559 = false;
		static constexpr bool is_bounded = true;
		static constexpr bool is_modulo = true;
		static constexpr int digits =
		  static_cast<int>( sizeof( std::uint16_t ) * CHAR_BIT - is_signed );
		static constexpr int digits10 = digits * 3 / 10;
		static constexpr int max_digits10 = 0;
		static constexpr int radix = 2;
		static constexpr int min_exponent = 0;
		static constexpr int min_exponent10 = 0;
		static constexpr int max_exponent = 0;
		static constexpr int max_exponent10 = 0;
		// Cannot reasonibly define
		// static constexpr bool traps = true;
		static constexpr bool tinyness_before = false;

		static constexpr daw::UInt16 min( ) noexcept {
			return daw::UInt16( );
		}

		static constexpr daw::UInt16 lowest( ) noexcept {
			return daw::UInt16( );
		}

		static constexpr daw::UInt16 max( ) noexcept {
			return static_cast<daw::UInt16>(
			  std::numeric_limits<std::uint16_t>::max( ) );
		}

		static constexpr daw::UInt16 epsilon( ) noexcept {
			return daw::UInt16( );
		}

		static constexpr daw::UInt16 round_error( ) noexcept {
			return daw::UInt16( );
		}

		static constexpr daw::UInt16 infinity( ) noexcept {
			return daw::UInt16( );
		}

		static constexpr daw::UInt16 quiet_NaN( ) noexcept {
			return daw::UInt16( );
		}

		static constexpr daw::UInt16 signalling_NaN( ) noexcept {
			return daw::UInt16( );
		}

		static constexpr daw::UInt16 denorm_min( ) noexcept {
			return daw::UInt16( );
		}
	};

	template<>
	struct numeric_limits<daw::UInt8> {
		static constexpr bool is_specialized = true;
		static constexpr bool is_signed = false;
		static constexpr bool is_integer = true;
		static constexpr bool is_exact = true;
		static constexpr bool has_infinity = false;
		static constexpr bool has_quiet_NaN = false;
		static constexpr bool has_signaling_NaN = false;
		static constexpr bool has_denorm = false;
		static constexpr bool has_denorm_loss = false;
		static constexpr std::float_round_style round_style =
		  std::round_toward_zero;
		static constexpr bool is_iec559 = false;
		static constexpr bool is_bounded = true;
		static constexpr bool is_modulo = true;
		static constexpr int digits =
		  static_cast<int>( sizeof( std::uint8_t ) * CHAR_BIT - is_signed );
		static constexpr int digits10 = digits * 3 / 10;
		static constexpr int max_digits10 = 0;
		static constexpr int radix = 2;
		static constexpr int min_exponent = 0;
		static constexpr int min_exponent10 = 0;
		static constexpr int max_exponent = 0;
		static constexpr int max_exponent10 = 0;
		// Cannot reasonibly define
		// static constexpr bool traps = true;
		static constexpr bool tinyness_before = false;

		static constexpr daw::UInt8 min( ) noexcept {
			return daw::UInt8( );
		}

		static constexpr daw::UInt8 lowest( ) noexcept {
			return daw::UInt8( );
		}

		static constexpr daw::UInt8 max( ) noexcept {
			return static_cast<daw::UInt8>(
			  std::numeric_limits<std::uint8_t>::max( ) );
		}

		static constexpr daw::UInt8 epsilon( ) noexcept {
			return daw::UInt8( );
		}

		static constexpr daw::UInt8 round_error( ) noexcept {
			return daw::UInt8( );
		}

		static constexpr daw::UInt8 infinity( ) noexcept {
			return daw::UInt8( );
		}

		static constexpr daw::UInt8 quiet_NaN( ) noexcept {
			return daw::UInt8( );
		}

		static constexpr daw::UInt8 signalling_NaN( ) noexcept {
			return daw::UInt8( );
		}

		static constexpr daw::UInt8 denorm_min( ) noexcept {
			return daw::UInt8( );
		}
	};
} // namespace std
