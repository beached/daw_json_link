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

#include <cstdint>
#include <type_traits>
#include <utility>

namespace daw {
	enum class UInt64Buffer : std::uint64_t {};
	enum class UInt32Buffer : std::uint32_t {};
	enum class UInt16Buffer : std::uint16_t {};
	enum class UInt8Buffer : std::uint8_t {};

	constexpr UInt64Buffer &operator<<=( UInt64Buffer &b,
	                                     std::uint64_t shift ) noexcept {
		static_assert( std::is_unsigned_v<std::uint64_t> );
		auto tmp = static_cast<std::uint64_t>( b );
		tmp <<= static_cast<std::uint64_t>( shift );
		b = static_cast<UInt64Buffer>( tmp );
		return b;
	}

	constexpr UInt64Buffer &operator<<=( UInt64Buffer &b,
	                                     UInt64Buffer shift ) noexcept {
		auto tmp = static_cast<std::uint64_t>( b );
		tmp <<= static_cast<std::uint64_t>( shift );
		b = static_cast<UInt64Buffer>( tmp );
		return b;
	}

	constexpr UInt64Buffer &operator>>=( UInt64Buffer &b,
	                                     std::uint64_t shift ) noexcept {
		static_assert( std::is_unsigned_v<std::uint64_t> );
		auto tmp = static_cast<std::uint64_t>( b );
		tmp >>= static_cast<std::uint64_t>( shift );
		b = static_cast<UInt64Buffer>( tmp );
		return b;
	}

	constexpr UInt64Buffer &operator>>=( UInt64Buffer &b,
	                                     UInt64Buffer shift ) noexcept {
		auto tmp = static_cast<std::uint64_t>( b );
		tmp >>= static_cast<std::uint64_t>( shift );
		b = static_cast<UInt64Buffer>( tmp );
		return b;
	}

	constexpr UInt64Buffer operator<<( UInt64Buffer b,
	                                   std::uint64_t shift ) noexcept {
		static_assert( std::is_unsigned_v<std::uint64_t> );
		auto tmp = static_cast<std::uint64_t>( b );
		tmp <<= static_cast<std::uint64_t>( shift );
		return static_cast<UInt64Buffer>( tmp );
	}

	constexpr UInt64Buffer operator<<( UInt64Buffer b,
	                                   UInt64Buffer shift ) noexcept {
		auto tmp = static_cast<std::uint64_t>( b );
		tmp <<= static_cast<std::uint64_t>( shift );
		return static_cast<UInt64Buffer>( tmp );
	}

	constexpr std::uint64_t operator<<( std::uint64_t b,
	                                    UInt64Buffer shift ) noexcept {
		static_assert( std::is_unsigned_v<std::uint64_t> );
		b <<= static_cast<std::uint64_t>( shift );
		return b;
	}

	constexpr UInt64Buffer operator>>( UInt64Buffer b,
	                                   std::uint64_t shift ) noexcept {
		static_assert( std::is_unsigned_v<std::uint64_t> );
		auto tmp = static_cast<std::uint64_t>( b );
		tmp >>= static_cast<std::uint64_t>( shift );
		return static_cast<UInt64Buffer>( tmp );
	}

	constexpr UInt64Buffer operator>>( UInt64Buffer b,
	                                   UInt64Buffer shift ) noexcept {
		auto tmp = static_cast<std::uint64_t>( b );
		tmp >>= static_cast<std::uint64_t>( shift );
		return static_cast<UInt64Buffer>( tmp );
	}

	constexpr std::uint64_t operator>>( std::uint64_t b,
	                                    UInt64Buffer shift ) noexcept {
		static_assert( std::is_unsigned_v<std::uint64_t> );
		b >>= static_cast<std::uint64_t>( shift );
		return b;
	}

	constexpr UInt64Buffer &operator|=( UInt64Buffer &lhs,
	                                    std::uint64_t rhs ) noexcept {
		static_assert( std::is_unsigned_v<std::uint64_t> );
		auto tmp = static_cast<std::uint64_t>( lhs );
		tmp |= static_cast<std::uint64_t>( rhs );
		lhs = static_cast<UInt64Buffer>( tmp );
		return lhs;
	}

	constexpr UInt64Buffer &operator|=( UInt64Buffer &lhs,
	                                    UInt64Buffer rhs ) noexcept {
		auto tmp = static_cast<std::uint64_t>( lhs );
		tmp |= static_cast<std::uint64_t>( rhs );
		lhs = static_cast<UInt64Buffer>( tmp );
		return lhs;
	}

	constexpr UInt64Buffer &operator&=( UInt64Buffer &lhs,
	                                    std::uint64_t rhs ) noexcept {
		static_assert( std::is_unsigned_v<std::uint64_t> );
		auto tmp = static_cast<std::uint64_t>( lhs );
		tmp &= static_cast<std::uint64_t>( rhs );
		lhs = static_cast<UInt64Buffer>( tmp );
		return lhs;
	}

	constexpr UInt64Buffer &operator&=( UInt64Buffer &lhs,
	                                    UInt64Buffer rhs ) noexcept {
		auto tmp = static_cast<std::uint64_t>( lhs );
		tmp &= static_cast<std::uint64_t>( rhs );
		lhs = static_cast<UInt64Buffer>( tmp );
		return lhs;
	}

	constexpr UInt64Buffer &operator^=( UInt64Buffer &lhs,
	                                    std::uint64_t rhs ) noexcept {
		static_assert( std::is_unsigned_v<std::uint64_t> );
		auto tmp = static_cast<std::uint64_t>( lhs );
		tmp ^= static_cast<std::uint64_t>( rhs );
		lhs = static_cast<UInt64Buffer>( tmp );
		return lhs;
	}

	constexpr UInt64Buffer &operator^=( UInt64Buffer &lhs,
	                                    UInt64Buffer rhs ) noexcept {
		auto tmp = static_cast<std::uint64_t>( lhs );
		tmp ^= static_cast<std::uint64_t>( rhs );
		lhs = static_cast<UInt64Buffer>( tmp );
		return lhs;
	}

	constexpr UInt64Buffer operator|( UInt64Buffer lhs,
	                                  std::uint64_t rhs ) noexcept {
		static_assert( std::is_unsigned_v<std::uint64_t> );
		return static_cast<UInt64Buffer>( static_cast<std::uint64_t>( lhs ) |
		                                  static_cast<std::uint64_t>( rhs ) );
	}

	constexpr UInt64Buffer operator|( std::uint64_t lhs,
	                                  UInt64Buffer rhs ) noexcept {
		static_assert( std::is_unsigned_v<std::uint64_t> );
		return static_cast<UInt64Buffer>( static_cast<std::uint64_t>( lhs ) |
		                                  static_cast<std::uint64_t>( rhs ) );
	}

	constexpr UInt64Buffer operator&( UInt64Buffer lhs,
	                                  std::uint64_t rhs ) noexcept {
		static_assert( std::is_unsigned_v<std::uint64_t> );
		return static_cast<UInt64Buffer>( static_cast<std::uint64_t>( lhs ) &
		                                  static_cast<std::uint64_t>( rhs ) );
	}

	constexpr UInt64Buffer operator&( std::uint64_t lhs,
	                                  UInt64Buffer rhs ) noexcept {
		static_assert( std::is_unsigned_v<std::uint64_t> );
		return static_cast<UInt64Buffer>( static_cast<std::uint64_t>( lhs ) &
		                                  static_cast<std::uint64_t>( rhs ) );
	}

	constexpr UInt64Buffer operator^( UInt64Buffer lhs,
	                                  std::uint64_t rhs ) noexcept {
		static_assert( std::is_unsigned_v<std::uint64_t> );
		return static_cast<UInt64Buffer>( static_cast<std::uint64_t>( lhs ) ^
		                                  static_cast<std::uint64_t>( rhs ) );
	}

	constexpr UInt64Buffer operator^( std::uint64_t lhs,
	                                  UInt64Buffer rhs ) noexcept {
		static_assert( std::is_unsigned_v<std::uint64_t> );
		return static_cast<UInt64Buffer>( static_cast<std::uint64_t>( lhs ) ^
		                                  static_cast<std::uint64_t>( rhs ) );
	}

	constexpr bool operator==( UInt64Buffer lhs, std::uint64_t rhs ) noexcept {
		static_assert( std::is_unsigned_v<std::uint64_t> );
		return static_cast<std::uint64_t>( lhs ) ==
		       static_cast<std::uint64_t>( rhs );
	}

	constexpr bool operator==( std::uint64_t lhs, UInt64Buffer rhs ) noexcept {
		static_assert( std::is_unsigned_v<std::uint64_t> );
		return static_cast<std::uint64_t>( lhs ) ==
		       static_cast<std::uint64_t>( rhs );
	}

	constexpr bool operator!=( UInt64Buffer lhs, std::uint64_t rhs ) noexcept {
		static_assert( std::is_unsigned_v<std::uint64_t> );
		return static_cast<std::uint64_t>( lhs ) !=
		       static_cast<std::uint64_t>( rhs );
	}

	constexpr bool operator!=( std::uint64_t lhs, UInt64Buffer rhs ) noexcept {
		static_assert( std::is_unsigned_v<std::uint64_t> );
		return static_cast<std::uint64_t>( lhs ) !=
		       static_cast<std::uint64_t>( rhs );
	}

	constexpr UInt64Buffer operator|( UInt64Buffer lhs,
	                                  UInt64Buffer rhs ) noexcept {
		return static_cast<UInt64Buffer>( static_cast<std::uint64_t>( lhs ) |
		                                  static_cast<std::uint64_t>( rhs ) );
	}

	constexpr UInt64Buffer operator&( UInt64Buffer lhs,
	                                  UInt64Buffer rhs ) noexcept {
		return static_cast<UInt64Buffer>( static_cast<std::uint64_t>( lhs ) &
		                                  static_cast<std::uint64_t>( rhs ) );
	}

	constexpr UInt64Buffer operator^( UInt64Buffer lhs,
	                                  UInt64Buffer rhs ) noexcept {
		return static_cast<UInt64Buffer>( static_cast<std::uint64_t>( lhs ) ^
		                                  static_cast<std::uint64_t>( rhs ) );
	}

	constexpr UInt32Buffer &operator<<=( UInt32Buffer &b,
	                                     std::uint32_t shift ) noexcept {
		static_assert( std::is_unsigned_v<std::uint32_t> );
		auto tmp = static_cast<std::uint32_t>( b );
		tmp <<= static_cast<std::uint32_t>( shift );
		b = static_cast<UInt32Buffer>( tmp );
		return b;
	}

	constexpr UInt32Buffer &operator<<=( UInt32Buffer &b,
	                                     UInt32Buffer shift ) noexcept {
		auto tmp = static_cast<std::uint32_t>( b );
		tmp <<= static_cast<std::uint32_t>( shift );
		b = static_cast<UInt32Buffer>( tmp );
		return b;
	}

	constexpr UInt32Buffer &operator>>=( UInt32Buffer &b,
	                                     std::uint32_t shift ) noexcept {
		static_assert( std::is_unsigned_v<std::uint32_t> );
		auto tmp = static_cast<std::uint32_t>( b );
		tmp >>= static_cast<std::uint32_t>( shift );
		b = static_cast<UInt32Buffer>( tmp );
		return b;
	}

	constexpr UInt32Buffer &operator>>=( UInt32Buffer &b,
	                                     UInt32Buffer shift ) noexcept {
		auto tmp = static_cast<std::uint32_t>( b );
		tmp >>= static_cast<std::uint32_t>( shift );
		b = static_cast<UInt32Buffer>( tmp );
		return b;
	}

	constexpr UInt32Buffer operator<<( UInt32Buffer b,
	                                   std::uint32_t shift ) noexcept {
		static_assert( std::is_unsigned_v<std::uint32_t> );
		auto tmp = static_cast<std::uint32_t>( b );
		tmp <<= static_cast<std::uint32_t>( shift );
		return static_cast<UInt32Buffer>( tmp );
	}

	constexpr UInt32Buffer operator<<( UInt32Buffer b,
	                                   UInt32Buffer shift ) noexcept {
		auto tmp = static_cast<std::uint32_t>( b );
		tmp <<= static_cast<std::uint32_t>( shift );
		return static_cast<UInt32Buffer>( tmp );
	}

	constexpr std::uint32_t operator<<( std::uint32_t b,
	                                    UInt32Buffer shift ) noexcept {
		static_assert( std::is_unsigned_v<std::uint32_t> );
		b <<= static_cast<std::uint32_t>( shift );
		return b;
	}

	constexpr UInt32Buffer operator>>( UInt32Buffer b,
	                                   std::uint32_t shift ) noexcept {
		static_assert( std::is_unsigned_v<std::uint32_t> );
		auto tmp = static_cast<std::uint32_t>( b );
		tmp >>= static_cast<std::uint32_t>( shift );
		return static_cast<UInt32Buffer>( tmp );
	}

	constexpr UInt32Buffer operator>>( UInt32Buffer b,
	                                   UInt32Buffer shift ) noexcept {
		auto tmp = static_cast<std::uint32_t>( b );
		tmp >>= static_cast<std::uint32_t>( shift );
		return static_cast<UInt32Buffer>( tmp );
	}

	constexpr std::uint32_t operator>>( std::uint32_t b,
	                                    UInt32Buffer shift ) noexcept {
		static_assert( std::is_unsigned_v<std::uint32_t> );
		b >>= static_cast<std::uint32_t>( shift );
		return b;
	}

	constexpr UInt32Buffer &operator|=( UInt32Buffer &lhs,
	                                    std::uint32_t rhs ) noexcept {
		static_assert( std::is_unsigned_v<std::uint32_t> );
		auto tmp = static_cast<std::uint32_t>( lhs );
		tmp |= static_cast<std::uint32_t>( rhs );
		lhs = static_cast<UInt32Buffer>( tmp );
		return lhs;
	}

	constexpr UInt32Buffer &operator|=( UInt32Buffer &lhs,
	                                    UInt32Buffer rhs ) noexcept {
		auto tmp = static_cast<std::uint32_t>( lhs );
		tmp |= static_cast<std::uint32_t>( rhs );
		lhs = static_cast<UInt32Buffer>( tmp );
		return lhs;
	}

	constexpr UInt32Buffer &operator&=( UInt32Buffer &lhs,
	                                    std::uint32_t rhs ) noexcept {
		static_assert( std::is_unsigned_v<std::uint32_t> );
		auto tmp = static_cast<std::uint32_t>( lhs );
		tmp &= static_cast<std::uint32_t>( rhs );
		lhs = static_cast<UInt32Buffer>( tmp );
		return lhs;
	}

	constexpr UInt32Buffer &operator&=( UInt32Buffer &lhs,
	                                    UInt32Buffer rhs ) noexcept {
		auto tmp = static_cast<std::uint32_t>( lhs );
		tmp &= static_cast<std::uint32_t>( rhs );
		lhs = static_cast<UInt32Buffer>( tmp );
		return lhs;
	}

	constexpr UInt32Buffer &operator^=( UInt32Buffer &lhs,
	                                    std::uint32_t rhs ) noexcept {
		static_assert( std::is_unsigned_v<std::uint32_t> );
		auto tmp = static_cast<std::uint32_t>( lhs );
		tmp ^= static_cast<std::uint32_t>( rhs );
		lhs = static_cast<UInt32Buffer>( tmp );
		return lhs;
	}

	constexpr UInt32Buffer &operator^=( UInt32Buffer &lhs,
	                                    UInt32Buffer rhs ) noexcept {
		auto tmp = static_cast<std::uint32_t>( lhs );
		tmp ^= static_cast<std::uint32_t>( rhs );
		lhs = static_cast<UInt32Buffer>( tmp );
		return lhs;
	}

	constexpr UInt32Buffer operator|( UInt32Buffer lhs,
	                                  std::uint32_t rhs ) noexcept {
		static_assert( std::is_unsigned_v<std::uint32_t> );
		return static_cast<UInt32Buffer>( static_cast<std::uint32_t>( lhs ) |
		                                  static_cast<std::uint32_t>( rhs ) );
	}

	constexpr UInt32Buffer operator|( std::uint32_t lhs,
	                                  UInt32Buffer rhs ) noexcept {
		static_assert( std::is_unsigned_v<std::uint32_t> );
		return static_cast<UInt32Buffer>( static_cast<std::uint32_t>( lhs ) |
		                                  static_cast<std::uint32_t>( rhs ) );
	}

	constexpr UInt32Buffer operator&( UInt32Buffer lhs,
	                                  std::uint32_t rhs ) noexcept {
		static_assert( std::is_unsigned_v<std::uint32_t> );
		return static_cast<UInt32Buffer>( static_cast<std::uint32_t>( lhs ) &
		                                  static_cast<std::uint32_t>( rhs ) );
	}

	constexpr UInt32Buffer operator&( std::uint32_t lhs,
	                                  UInt32Buffer rhs ) noexcept {
		static_assert( std::is_unsigned_v<std::uint32_t> );
		return static_cast<UInt32Buffer>( static_cast<std::uint32_t>( lhs ) &
		                                  static_cast<std::uint32_t>( rhs ) );
	}

	constexpr UInt32Buffer operator^( UInt32Buffer lhs,
	                                  std::uint32_t rhs ) noexcept {
		static_assert( std::is_unsigned_v<std::uint32_t> );
		return static_cast<UInt32Buffer>( static_cast<std::uint32_t>( lhs ) ^
		                                  static_cast<std::uint32_t>( rhs ) );
	}

	constexpr UInt32Buffer operator^( std::uint32_t lhs,
	                                  UInt32Buffer rhs ) noexcept {
		static_assert( std::is_unsigned_v<std::uint32_t> );
		return static_cast<UInt32Buffer>( static_cast<std::uint32_t>( lhs ) ^
		                                  static_cast<std::uint32_t>( rhs ) );
	}

	constexpr bool operator==( UInt32Buffer lhs, std::uint32_t rhs ) noexcept {
		static_assert( std::is_unsigned_v<std::uint32_t> );
		return static_cast<std::uint32_t>( lhs ) ==
		       static_cast<std::uint32_t>( rhs );
	}

	constexpr bool operator==( std::uint32_t lhs, UInt32Buffer rhs ) noexcept {
		static_assert( std::is_unsigned_v<std::uint32_t> );
		return static_cast<std::uint32_t>( lhs ) ==
		       static_cast<std::uint32_t>( rhs );
	}

	constexpr bool operator!=( UInt32Buffer lhs, std::uint32_t rhs ) noexcept {
		static_assert( std::is_unsigned_v<std::uint32_t> );
		return static_cast<std::uint32_t>( lhs ) !=
		       static_cast<std::uint32_t>( rhs );
	}

	constexpr bool operator!=( std::uint32_t lhs, UInt32Buffer rhs ) noexcept {
		static_assert( std::is_unsigned_v<std::uint32_t> );
		return static_cast<std::uint32_t>( lhs ) !=
		       static_cast<std::uint32_t>( rhs );
	}

	constexpr UInt32Buffer operator|( UInt32Buffer lhs,
	                                  UInt32Buffer rhs ) noexcept {
		return static_cast<UInt32Buffer>( static_cast<std::uint32_t>( lhs ) |
		                                  static_cast<std::uint32_t>( rhs ) );
	}

	constexpr UInt32Buffer operator&( UInt32Buffer lhs,
	                                  UInt32Buffer rhs ) noexcept {
		return static_cast<UInt32Buffer>( static_cast<std::uint32_t>( lhs ) &
		                                  static_cast<std::uint32_t>( rhs ) );
	}

	constexpr UInt32Buffer operator^( UInt32Buffer lhs,
	                                  UInt32Buffer rhs ) noexcept {
		return static_cast<UInt32Buffer>( static_cast<std::uint32_t>( lhs ) ^
		                                  static_cast<std::uint32_t>( rhs ) );
	}

	constexpr UInt16Buffer &operator<<=( UInt16Buffer &b,
	                                     std::uint16_t shift ) noexcept {
		static_assert( std::is_unsigned_v<std::uint16_t> );
		auto tmp = static_cast<std::uint16_t>( b );
		tmp <<= static_cast<std::uint16_t>( shift );
		b = static_cast<UInt16Buffer>( tmp );
		return b;
	}

	constexpr UInt16Buffer &operator<<=( UInt16Buffer &b,
	                                     UInt16Buffer shift ) noexcept {
		auto tmp = static_cast<std::uint16_t>( b );
		tmp <<= static_cast<std::uint16_t>( shift );
		b = static_cast<UInt16Buffer>( tmp );
		return b;
	}

	constexpr UInt16Buffer &operator>>=( UInt16Buffer &b,
	                                     std::uint16_t shift ) noexcept {
		static_assert( std::is_unsigned_v<std::uint16_t> );
		auto tmp = static_cast<std::uint16_t>( b );
		tmp >>= static_cast<std::uint16_t>( shift );
		b = static_cast<UInt16Buffer>( tmp );
		return b;
	}

	constexpr UInt16Buffer &operator>>=( UInt16Buffer &b,
	                                     UInt16Buffer shift ) noexcept {
		auto tmp = static_cast<std::uint16_t>( b );
		tmp >>= static_cast<std::uint16_t>( shift );
		b = static_cast<UInt16Buffer>( tmp );
		return b;
	}

	constexpr UInt16Buffer operator<<( UInt16Buffer b,
	                                   std::uint16_t shift ) noexcept {
		static_assert( std::is_unsigned_v<std::uint16_t> );
		auto tmp = static_cast<std::uint16_t>( b );
		tmp <<= static_cast<std::uint16_t>( shift );
		return static_cast<UInt16Buffer>( tmp );
	}

	constexpr UInt16Buffer operator<<( UInt16Buffer b,
	                                   UInt16Buffer shift ) noexcept {
		auto tmp = static_cast<std::uint16_t>( b );
		tmp <<= static_cast<std::uint16_t>( shift );
		return static_cast<UInt16Buffer>( tmp );
	}

	constexpr std::uint16_t operator<<( std::uint16_t b,
	                                    UInt16Buffer shift ) noexcept {
		static_assert( std::is_unsigned_v<std::uint16_t> );
		b <<= static_cast<std::uint16_t>( shift );
		return b;
	}

	constexpr UInt16Buffer operator>>( UInt16Buffer b,
	                                   std::uint16_t shift ) noexcept {
		static_assert( std::is_unsigned_v<std::uint16_t> );
		auto tmp = static_cast<std::uint16_t>( b );
		tmp >>= static_cast<std::uint16_t>( shift );
		return static_cast<UInt16Buffer>( tmp );
	}

	constexpr UInt16Buffer operator>>( UInt16Buffer b,
	                                   UInt16Buffer shift ) noexcept {
		auto tmp = static_cast<std::uint16_t>( b );
		tmp >>= static_cast<std::uint16_t>( shift );
		return static_cast<UInt16Buffer>( tmp );
	}

	constexpr std::uint16_t operator>>( std::uint16_t b,
	                                    UInt16Buffer shift ) noexcept {
		static_assert( std::is_unsigned_v<std::uint16_t> );
		b >>= static_cast<std::uint16_t>( shift );
		return b;
	}

	constexpr UInt16Buffer &operator|=( UInt16Buffer &lhs,
	                                    std::uint16_t rhs ) noexcept {
		static_assert( std::is_unsigned_v<std::uint16_t> );
		auto tmp = static_cast<std::uint16_t>( lhs );
		tmp |= static_cast<std::uint16_t>( rhs );
		lhs = static_cast<UInt16Buffer>( tmp );
		return lhs;
	}

	constexpr UInt16Buffer &operator|=( UInt16Buffer &lhs,
	                                    UInt16Buffer rhs ) noexcept {
		auto tmp = static_cast<std::uint16_t>( lhs );
		tmp |= static_cast<std::uint16_t>( rhs );
		lhs = static_cast<UInt16Buffer>( tmp );
		return lhs;
	}

	constexpr UInt16Buffer &operator&=( UInt16Buffer &lhs,
	                                    std::uint16_t rhs ) noexcept {
		static_assert( std::is_unsigned_v<std::uint16_t> );
		auto tmp = static_cast<std::uint16_t>( lhs );
		tmp &= static_cast<std::uint16_t>( rhs );
		lhs = static_cast<UInt16Buffer>( tmp );
		return lhs;
	}

	constexpr UInt16Buffer &operator&=( UInt16Buffer &lhs,
	                                    UInt16Buffer rhs ) noexcept {
		auto tmp = static_cast<std::uint16_t>( lhs );
		tmp &= static_cast<std::uint16_t>( rhs );
		lhs = static_cast<UInt16Buffer>( tmp );
		return lhs;
	}

	constexpr UInt16Buffer &operator^=( UInt16Buffer &lhs,
	                                    std::uint16_t rhs ) noexcept {
		static_assert( std::is_unsigned_v<std::uint16_t> );
		auto tmp = static_cast<std::uint16_t>( lhs );
		tmp ^= static_cast<std::uint16_t>( rhs );
		lhs = static_cast<UInt16Buffer>( tmp );
		return lhs;
	}

	constexpr UInt16Buffer &operator^=( UInt16Buffer &lhs,
	                                    UInt16Buffer rhs ) noexcept {
		auto tmp = static_cast<std::uint16_t>( lhs );
		tmp ^= static_cast<std::uint16_t>( rhs );
		lhs = static_cast<UInt16Buffer>( tmp );
		return lhs;
	}

	constexpr UInt16Buffer operator|( UInt16Buffer lhs,
	                                  std::uint16_t rhs ) noexcept {
		static_assert( std::is_unsigned_v<std::uint16_t> );
		return static_cast<UInt16Buffer>( static_cast<std::uint16_t>( lhs ) |
		                                  static_cast<std::uint16_t>( rhs ) );
	}

	constexpr UInt16Buffer operator|( std::uint16_t lhs,
	                                  UInt16Buffer rhs ) noexcept {
		static_assert( std::is_unsigned_v<std::uint16_t> );
		return static_cast<UInt16Buffer>( static_cast<std::uint16_t>( lhs ) |
		                                  static_cast<std::uint16_t>( rhs ) );
	}

	constexpr UInt16Buffer operator&( UInt16Buffer lhs,
	                                  std::uint16_t rhs ) noexcept {
		static_assert( std::is_unsigned_v<std::uint16_t> );
		return static_cast<UInt16Buffer>( static_cast<std::uint16_t>( lhs ) &
		                                  static_cast<std::uint16_t>( rhs ) );
	}

	constexpr UInt16Buffer operator&( std::uint16_t lhs,
	                                  UInt16Buffer rhs ) noexcept {
		static_assert( std::is_unsigned_v<std::uint16_t> );
		return static_cast<UInt16Buffer>( static_cast<std::uint16_t>( lhs ) &
		                                  static_cast<std::uint16_t>( rhs ) );
	}

	constexpr UInt16Buffer operator^( UInt16Buffer lhs,
	                                  std::uint16_t rhs ) noexcept {
		static_assert( std::is_unsigned_v<std::uint16_t> );
		return static_cast<UInt16Buffer>( static_cast<std::uint16_t>( lhs ) ^
		                                  static_cast<std::uint16_t>( rhs ) );
	}

	constexpr UInt16Buffer operator^( std::uint16_t lhs,
	                                  UInt16Buffer rhs ) noexcept {
		static_assert( std::is_unsigned_v<std::uint16_t> );
		return static_cast<UInt16Buffer>( static_cast<std::uint16_t>( lhs ) ^
		                                  static_cast<std::uint16_t>( rhs ) );
	}

	constexpr bool operator==( UInt16Buffer lhs, std::uint16_t rhs ) noexcept {
		static_assert( std::is_unsigned_v<std::uint16_t> );
		return static_cast<std::uint16_t>( lhs ) ==
		       static_cast<std::uint16_t>( rhs );
	}

	constexpr bool operator==( std::uint16_t lhs, UInt16Buffer rhs ) noexcept {
		static_assert( std::is_unsigned_v<std::uint16_t> );
		return static_cast<std::uint16_t>( lhs ) ==
		       static_cast<std::uint16_t>( rhs );
	}

	constexpr bool operator!=( UInt16Buffer lhs, std::uint16_t rhs ) noexcept {
		static_assert( std::is_unsigned_v<std::uint16_t> );
		return static_cast<std::uint16_t>( lhs ) !=
		       static_cast<std::uint16_t>( rhs );
	}

	constexpr bool operator!=( std::uint16_t lhs, UInt16Buffer rhs ) noexcept {
		static_assert( std::is_unsigned_v<std::uint16_t> );
		return static_cast<std::uint16_t>( lhs ) !=
		       static_cast<std::uint16_t>( rhs );
	}

	constexpr UInt16Buffer operator|( UInt16Buffer lhs,
	                                  UInt16Buffer rhs ) noexcept {
		return static_cast<UInt16Buffer>( static_cast<std::uint16_t>( lhs ) |
		                                  static_cast<std::uint16_t>( rhs ) );
	}

	constexpr UInt16Buffer operator&( UInt16Buffer lhs,
	                                  UInt16Buffer rhs ) noexcept {
		return static_cast<UInt16Buffer>( static_cast<std::uint16_t>( lhs ) &
		                                  static_cast<std::uint16_t>( rhs ) );
	}

	constexpr UInt16Buffer operator^( UInt16Buffer lhs,
	                                  UInt16Buffer rhs ) noexcept {
		return static_cast<UInt16Buffer>( static_cast<std::uint16_t>( lhs ) ^
		                                  static_cast<std::uint16_t>( rhs ) );
	}
	//*********
	constexpr UInt64Buffer &operator+=( UInt64Buffer &lhs,
	                                    UInt64Buffer rhs ) noexcept {
		lhs = static_cast<UInt64Buffer>( static_cast<std::uint64_t>( lhs ) +
		                                 static_cast<std::uint64_t>( rhs ) );
		return lhs;
	}

	constexpr UInt64Buffer &operator+=( UInt64Buffer &lhs,
	                                    std::uint64_t rhs ) noexcept {
		lhs = static_cast<UInt64Buffer>( static_cast<std::uint64_t>( lhs ) + rhs );
		return lhs;
	}

	constexpr std::uint64_t &operator+=( std::uint64_t &lhs,
	                                     UInt64Buffer rhs ) noexcept {
		lhs += static_cast<std::uint64_t>( rhs );
		return lhs;
	}

	constexpr UInt64Buffer operator+( UInt64Buffer lhs,
	                                  UInt64Buffer rhs ) noexcept {
		lhs = static_cast<UInt64Buffer>( static_cast<std::uint64_t>( lhs ) +
		                                 static_cast<std::uint64_t>( rhs ) );
		return lhs;
	}

	constexpr UInt64Buffer operator+( UInt64Buffer lhs,
	                                  std::uint64_t rhs ) noexcept {
		lhs = static_cast<UInt64Buffer>( static_cast<std::uint64_t>( lhs ) + rhs );
		return lhs;
	}

	constexpr UInt64Buffer operator+( std::uint64_t lhs,
	                                  UInt64Buffer rhs ) noexcept {
		lhs += static_cast<std::uint64_t>( rhs );
		return static_cast<UInt64Buffer>( lhs );
	}

	constexpr UInt64Buffer &operator-=( UInt64Buffer &lhs,
	                                    UInt64Buffer rhs ) noexcept {
		lhs = static_cast<UInt64Buffer>( static_cast<std::uint64_t>( lhs ) -
		                                 static_cast<std::uint64_t>( rhs ) );
		return lhs;
	}

	constexpr UInt64Buffer &operator-=( UInt64Buffer &lhs,
	                                    std::uint64_t rhs ) noexcept {
		lhs = static_cast<UInt64Buffer>( static_cast<std::uint64_t>( lhs ) - rhs );
		return lhs;
	}

	constexpr std::uint64_t &operator-=( std::uint64_t &lhs,
	                                     UInt64Buffer rhs ) noexcept {
		lhs -= static_cast<std::uint64_t>( rhs );
		return lhs;
	}

	constexpr UInt64Buffer operator-( UInt64Buffer lhs,
	                                  UInt64Buffer rhs ) noexcept {
		lhs = static_cast<UInt64Buffer>( static_cast<std::uint64_t>( lhs ) -
		                                 static_cast<std::uint64_t>( rhs ) );
		return lhs;
	}

	constexpr UInt64Buffer operator-( UInt64Buffer lhs,
	                                  std::uint64_t rhs ) noexcept {
		lhs = static_cast<UInt64Buffer>( static_cast<std::uint64_t>( lhs ) - rhs );
		return lhs;
	}

	constexpr UInt64Buffer operator-( std::uint64_t lhs,
	                                  UInt64Buffer rhs ) noexcept {
		lhs -= static_cast<std::uint64_t>( rhs );
		return static_cast<UInt64Buffer>( lhs );
	}

	constexpr UInt64Buffer &operator*=( UInt64Buffer &lhs,
	                                    UInt64Buffer rhs ) noexcept {
		lhs = static_cast<UInt64Buffer>( static_cast<std::uint64_t>( lhs ) *
		                                 static_cast<std::uint64_t>( rhs ) );
		return lhs;
	}

	constexpr UInt64Buffer &operator*=( UInt64Buffer &lhs,
	                                    std::uint64_t rhs ) noexcept {
		lhs = static_cast<UInt64Buffer>( static_cast<std::uint64_t>( lhs ) * rhs );
		return lhs;
	}

	constexpr std::uint64_t &operator*=( std::uint64_t &lhs,
	                                     UInt64Buffer rhs ) noexcept {
		lhs *= static_cast<std::uint64_t>( rhs );
		return lhs;
	}

	constexpr UInt64Buffer operator*( UInt64Buffer lhs,
	                                  UInt64Buffer rhs ) noexcept {
		lhs = static_cast<UInt64Buffer>( static_cast<std::uint64_t>( lhs ) *
		                                 static_cast<std::uint64_t>( rhs ) );
		return lhs;
	}

	constexpr UInt64Buffer operator*( UInt64Buffer lhs,
	                                  std::uint64_t rhs ) noexcept {
		lhs = static_cast<UInt64Buffer>( static_cast<std::uint64_t>( lhs ) * rhs );
		return lhs;
	}

	constexpr UInt64Buffer operator*( std::uint64_t lhs,
	                                  UInt64Buffer rhs ) noexcept {
		lhs *= static_cast<std::uint64_t>( rhs );
		return static_cast<UInt64Buffer>( lhs );
	}

	constexpr UInt64Buffer &operator/=( UInt64Buffer &lhs,
	                                    UInt64Buffer rhs ) noexcept {
		lhs = static_cast<UInt64Buffer>( static_cast<std::uint64_t>( lhs ) /
		                                 static_cast<std::uint64_t>( rhs ) );
		return lhs;
	}

	constexpr UInt64Buffer &operator/=( UInt64Buffer &lhs,
	                                    std::uint64_t rhs ) noexcept {
		lhs = static_cast<UInt64Buffer>( static_cast<std::uint64_t>( lhs ) / rhs );
		return lhs;
	}

	constexpr std::uint64_t &operator/=( std::uint64_t &lhs,
	                                     UInt64Buffer rhs ) noexcept {
		lhs /= static_cast<std::uint64_t>( rhs );
		return lhs;
	}

	constexpr UInt64Buffer operator/( UInt64Buffer lhs,
	                                  UInt64Buffer rhs ) noexcept {
		lhs = static_cast<UInt64Buffer>( static_cast<std::uint64_t>( lhs ) /
		                                 static_cast<std::uint64_t>( rhs ) );
		return lhs;
	}

	constexpr UInt64Buffer operator/( UInt64Buffer lhs,
	                                  std::uint64_t rhs ) noexcept {
		lhs = static_cast<UInt64Buffer>( static_cast<std::uint64_t>( lhs ) / rhs );
		return lhs;
	}

	constexpr UInt64Buffer operator/( std::uint64_t lhs,
	                                  UInt64Buffer rhs ) noexcept {
		lhs /= static_cast<std::uint64_t>( rhs );
		return static_cast<UInt64Buffer>( lhs );
	}

	constexpr bool operator!( UInt64Buffer value ) {
		return !static_cast<std::uint64_t>( value );
	}

	constexpr UInt64Buffer operator+( UInt64Buffer value ) {
		return value;
	}

	constexpr UInt64Buffer operator-( UInt64Buffer value ) {
		return static_cast<UInt64Buffer>( -static_cast<std::uint64_t>( value ) );
	}

	constexpr UInt64Buffer &operator++( UInt64Buffer &value ) {
		value =
		  static_cast<UInt64Buffer>( static_cast<std::uint64_t>( value ) + 1 );
		return value;
	}

	constexpr UInt64Buffer operator++( UInt64Buffer &value, int ) {
		auto result = value;
		value =
		  static_cast<UInt64Buffer>( static_cast<std::uint64_t>( value ) + 1 );
		return result;
	}

	constexpr UInt64Buffer &operator--( UInt64Buffer &value ) {
		value =
		  static_cast<UInt64Buffer>( static_cast<std::uint64_t>( value ) - 1 );
		return value;
	}

	constexpr UInt64Buffer operator--( UInt64Buffer &value, int ) {
		auto result = value;
		value =
		  static_cast<UInt64Buffer>( static_cast<std::uint64_t>( value ) - 1 );
		return result;
	}

	constexpr UInt64Buffer operator~( UInt64Buffer value ) {
		return static_cast<UInt64Buffer>( ~static_cast<std::uint64_t>( value ) );
	}
	//*********
	constexpr UInt32Buffer &operator+=( UInt32Buffer &lhs,
	                                    UInt32Buffer rhs ) noexcept {
		lhs = static_cast<UInt32Buffer>( static_cast<std::uint32_t>( lhs ) +
		                                 static_cast<std::uint32_t>( rhs ) );
		return lhs;
	}

	constexpr UInt32Buffer &operator+=( UInt32Buffer &lhs,
	                                    std::uint32_t rhs ) noexcept {
		lhs = static_cast<UInt32Buffer>( static_cast<std::uint32_t>( lhs ) + rhs );
		return lhs;
	}

	constexpr std::uint32_t &operator+=( std::uint32_t &lhs,
	                                     UInt32Buffer rhs ) noexcept {
		lhs += static_cast<std::uint32_t>( rhs );
		return lhs;
	}

	constexpr UInt32Buffer operator+( UInt32Buffer lhs,
	                                  UInt32Buffer rhs ) noexcept {
		lhs = static_cast<UInt32Buffer>( static_cast<std::uint32_t>( lhs ) +
		                                 static_cast<std::uint32_t>( rhs ) );
		return lhs;
	}

	constexpr UInt32Buffer operator+( UInt32Buffer lhs,
	                                  std::uint32_t rhs ) noexcept {
		lhs = static_cast<UInt32Buffer>( static_cast<std::uint32_t>( lhs ) + rhs );
		return lhs;
	}

	constexpr UInt32Buffer operator+( std::uint32_t lhs,
	                                  UInt32Buffer rhs ) noexcept {
		lhs += static_cast<std::uint32_t>( rhs );
		return static_cast<UInt32Buffer>( lhs );
	}

	constexpr UInt32Buffer &operator-=( UInt32Buffer &lhs,
	                                    UInt32Buffer rhs ) noexcept {
		lhs = static_cast<UInt32Buffer>( static_cast<std::uint32_t>( lhs ) -
		                                 static_cast<std::uint32_t>( rhs ) );
		return lhs;
	}

	constexpr UInt32Buffer &operator-=( UInt32Buffer &lhs,
	                                    std::uint32_t rhs ) noexcept {
		lhs = static_cast<UInt32Buffer>( static_cast<std::uint32_t>( lhs ) - rhs );
		return lhs;
	}

	constexpr std::uint32_t &operator-=( std::uint32_t &lhs,
	                                     UInt32Buffer rhs ) noexcept {
		lhs -= static_cast<std::uint32_t>( rhs );
		return lhs;
	}

	constexpr UInt32Buffer operator-( UInt32Buffer lhs,
	                                  UInt32Buffer rhs ) noexcept {
		lhs = static_cast<UInt32Buffer>( static_cast<std::uint32_t>( lhs ) -
		                                 static_cast<std::uint32_t>( rhs ) );
		return lhs;
	}

	constexpr UInt32Buffer operator-( UInt32Buffer lhs,
	                                  std::uint32_t rhs ) noexcept {
		lhs = static_cast<UInt32Buffer>( static_cast<std::uint32_t>( lhs ) - rhs );
		return lhs;
	}

	constexpr UInt32Buffer operator-( std::uint32_t lhs,
	                                  UInt32Buffer rhs ) noexcept {
		lhs -= static_cast<std::uint32_t>( rhs );
		return static_cast<UInt32Buffer>( lhs );
	}

	constexpr UInt32Buffer &operator*=( UInt32Buffer &lhs,
	                                    UInt32Buffer rhs ) noexcept {
		lhs = static_cast<UInt32Buffer>( static_cast<std::uint32_t>( lhs ) *
		                                 static_cast<std::uint32_t>( rhs ) );
		return lhs;
	}

	constexpr UInt32Buffer &operator*=( UInt32Buffer &lhs,
	                                    std::uint32_t rhs ) noexcept {
		lhs = static_cast<UInt32Buffer>( static_cast<std::uint32_t>( lhs ) * rhs );
		return lhs;
	}

	constexpr std::uint32_t &operator*=( std::uint32_t &lhs,
	                                     UInt32Buffer rhs ) noexcept {
		lhs *= static_cast<std::uint32_t>( rhs );
		return lhs;
	}

	constexpr UInt32Buffer operator*( UInt32Buffer lhs,
	                                  UInt32Buffer rhs ) noexcept {
		lhs = static_cast<UInt32Buffer>( static_cast<std::uint32_t>( lhs ) *
		                                 static_cast<std::uint32_t>( rhs ) );
		return lhs;
	}

	constexpr UInt32Buffer operator*( UInt32Buffer lhs,
	                                  std::uint64_t rhs ) noexcept {
		lhs = static_cast<UInt32Buffer>( static_cast<std::uint32_t>( lhs ) * rhs );
		return lhs;
	}

	constexpr UInt32Buffer operator*( std::uint32_t lhs,
	                                  UInt64Buffer rhs ) noexcept {
		lhs *= static_cast<std::uint32_t>( rhs );
		return static_cast<UInt32Buffer>( lhs );
	}

	constexpr UInt32Buffer &operator/=( UInt32Buffer &lhs,
	                                    UInt32Buffer rhs ) noexcept {
		lhs = static_cast<UInt32Buffer>( static_cast<std::uint32_t>( lhs ) /
		                                 static_cast<std::uint64_t>( rhs ) );
		return lhs;
	}

	constexpr UInt32Buffer &operator/=( UInt32Buffer &lhs,
	                                    std::uint64_t rhs ) noexcept {
		lhs = static_cast<UInt32Buffer>( static_cast<std::uint32_t>( lhs ) / rhs );
		return lhs;
	}

	constexpr std::uint32_t &operator/=( std::uint32_t &lhs,
	                                     UInt64Buffer rhs ) noexcept {
		lhs /= static_cast<std::uint32_t>( rhs );
		return lhs;
	}

	constexpr UInt32Buffer operator/( UInt32Buffer lhs,
	                                  UInt32Buffer rhs ) noexcept {
		lhs = static_cast<UInt32Buffer>( static_cast<std::uint32_t>( lhs ) /
		                                 static_cast<std::uint64_t>( rhs ) );
		return lhs;
	}

	constexpr UInt32Buffer operator/( UInt32Buffer lhs,
	                                  std::uint64_t rhs ) noexcept {
		lhs = static_cast<UInt32Buffer>( static_cast<std::uint32_t>( lhs ) / rhs );
		return lhs;
	}

	constexpr UInt32Buffer operator/( std::uint32_t lhs,
	                                  UInt64Buffer rhs ) noexcept {
		lhs /= static_cast<std::uint32_t>( rhs );
		return static_cast<UInt32Buffer>( lhs );
	}

	constexpr bool operator!( UInt32Buffer value ) {
		return !static_cast<std::uint32_t>( value );
	}

	constexpr UInt32Buffer operator+( UInt32Buffer value ) {
		return value;
	}

	constexpr UInt32Buffer operator-( UInt32Buffer value ) {
		return static_cast<UInt32Buffer>( -static_cast<std::uint32_t>( value ) );
	}

	constexpr UInt32Buffer &operator++( UInt32Buffer &value ) {
		value =
		  static_cast<UInt32Buffer>( static_cast<std::uint32_t>( value ) + 1 );
		return value;
	}

	constexpr UInt32Buffer operator++( UInt32Buffer &value, int ) {
		auto result = value;
		value =
		  static_cast<UInt32Buffer>( static_cast<std::uint32_t>( value ) + 1 );
		return result;
	}

	constexpr UInt32Buffer &operator--( UInt32Buffer &value ) {
		value =
		  static_cast<UInt32Buffer>( static_cast<std::uint32_t>( value ) - 1 );
		return value;
	}

	constexpr UInt32Buffer operator--( UInt32Buffer &value, int ) {
		auto result = value;
		value =
		  static_cast<UInt32Buffer>( static_cast<std::uint32_t>( value ) - 1 );
		return result;
	}

	constexpr UInt32Buffer operator~( UInt32Buffer value ) {
		return static_cast<UInt32Buffer>( ~static_cast<std::uint32_t>( value ) );
	}
	//*********
	constexpr UInt16Buffer &operator+=( UInt16Buffer &lhs,
	                                    UInt16Buffer rhs ) noexcept {
		lhs = static_cast<UInt16Buffer>( static_cast<std::uint16_t>( lhs ) +
		                                 static_cast<std::uint16_t>( rhs ) );
		return lhs;
	}

	constexpr UInt16Buffer &operator+=( UInt16Buffer &lhs,
	                                    std::uint16_t rhs ) noexcept {
		lhs = static_cast<UInt16Buffer>( static_cast<std::uint16_t>( lhs ) + rhs );
		return lhs;
	}

	constexpr std::uint16_t &operator+=( std::uint16_t &lhs,
	                                     UInt16Buffer rhs ) noexcept {
		lhs += static_cast<std::uint16_t>( rhs );
		return lhs;
	}

	constexpr UInt16Buffer operator+( UInt16Buffer lhs,
	                                  UInt16Buffer rhs ) noexcept {
		lhs = static_cast<UInt16Buffer>( static_cast<std::uint16_t>( lhs ) +
		                                 static_cast<std::uint16_t>( rhs ) );
		return lhs;
	}

	constexpr UInt16Buffer operator+( UInt16Buffer lhs,
	                                  std::uint16_t rhs ) noexcept {
		lhs = static_cast<UInt16Buffer>( static_cast<std::uint16_t>( lhs ) + rhs );
		return lhs;
	}

	constexpr UInt16Buffer operator+( std::uint16_t lhs,
	                                  UInt16Buffer rhs ) noexcept {
		lhs += static_cast<std::uint16_t>( rhs );
		return static_cast<UInt16Buffer>( lhs );
	}

	constexpr UInt16Buffer &operator-=( UInt16Buffer &lhs,
	                                    UInt16Buffer rhs ) noexcept {
		lhs = static_cast<UInt16Buffer>( static_cast<std::uint16_t>( lhs ) -
		                                 static_cast<std::uint16_t>( rhs ) );
		return lhs;
	}

	constexpr UInt16Buffer &operator-=( UInt16Buffer &lhs,
	                                    std::uint16_t rhs ) noexcept {
		lhs = static_cast<UInt16Buffer>( static_cast<std::uint16_t>( lhs ) - rhs );
		return lhs;
	}

	constexpr std::uint16_t &operator-=( std::uint16_t &lhs,
	                                     UInt16Buffer rhs ) noexcept {
		lhs -= static_cast<std::uint16_t>( rhs );
		return lhs;
	}

	constexpr UInt16Buffer operator-( UInt16Buffer lhs,
	                                  UInt16Buffer rhs ) noexcept {
		lhs = static_cast<UInt16Buffer>( static_cast<std::uint16_t>( lhs ) -
		                                 static_cast<std::uint16_t>( rhs ) );
		return lhs;
	}

	constexpr UInt16Buffer operator-( UInt16Buffer lhs,
	                                  std::uint16_t rhs ) noexcept {
		lhs = static_cast<UInt16Buffer>( static_cast<std::uint16_t>( lhs ) - rhs );
		return lhs;
	}

	constexpr UInt16Buffer operator-( std::uint16_t lhs,
	                                  UInt16Buffer rhs ) noexcept {
		lhs -= static_cast<std::uint16_t>( rhs );
		return static_cast<UInt16Buffer>( lhs );
	}

	constexpr UInt16Buffer &operator*=( UInt16Buffer &lhs,
	                                    UInt16Buffer rhs ) noexcept {
		lhs = static_cast<UInt16Buffer>( static_cast<std::uint16_t>( lhs ) *
		                                 static_cast<std::uint16_t>( rhs ) );
		return lhs;
	}

	constexpr UInt16Buffer &operator*=( UInt16Buffer &lhs,
	                                    std::uint16_t rhs ) noexcept {
		lhs = static_cast<UInt16Buffer>( static_cast<std::uint16_t>( lhs ) * rhs );
		return lhs;
	}

	constexpr std::uint16_t &operator*=( std::uint16_t &lhs,
	                                     UInt16Buffer rhs ) noexcept {
		lhs *= static_cast<std::uint16_t>( rhs );
		return lhs;
	}

	constexpr UInt16Buffer operator*( UInt16Buffer lhs,
	                                  UInt16Buffer rhs ) noexcept {
		lhs = static_cast<UInt16Buffer>( static_cast<std::uint16_t>( lhs ) *
		                                 static_cast<std::uint16_t>( rhs ) );
		return lhs;
	}

	constexpr UInt16Buffer operator*( UInt16Buffer lhs,
	                                  std::uint64_t rhs ) noexcept {
		lhs = static_cast<UInt16Buffer>( static_cast<std::uint16_t>( lhs ) * rhs );
		return lhs;
	}

	constexpr UInt16Buffer operator*( std::uint16_t lhs,
	                                  UInt64Buffer rhs ) noexcept {
		lhs *= static_cast<std::uint16_t>( rhs );
		return static_cast<UInt16Buffer>( lhs );
	}

	constexpr UInt16Buffer &operator/=( UInt16Buffer &lhs,
	                                    UInt16Buffer rhs ) noexcept {
		lhs = static_cast<UInt16Buffer>( static_cast<std::uint16_t>( lhs ) /
		                                 static_cast<std::uint64_t>( rhs ) );
		return lhs;
	}

	constexpr UInt16Buffer &operator/=( UInt16Buffer &lhs,
	                                    std::uint64_t rhs ) noexcept {
		lhs = static_cast<UInt16Buffer>( static_cast<std::uint16_t>( lhs ) / rhs );
		return lhs;
	}

	constexpr std::uint16_t &operator/=( std::uint16_t &lhs,
	                                     UInt64Buffer rhs ) noexcept {
		lhs /= static_cast<std::uint16_t>( rhs );
		return lhs;
	}

	constexpr UInt16Buffer operator/( UInt16Buffer lhs,
	                                  UInt16Buffer rhs ) noexcept {
		lhs = static_cast<UInt16Buffer>( static_cast<std::uint16_t>( lhs ) /
		                                 static_cast<std::uint64_t>( rhs ) );
		return lhs;
	}

	constexpr UInt16Buffer operator/( UInt16Buffer lhs,
	                                  std::uint64_t rhs ) noexcept {
		lhs = static_cast<UInt16Buffer>( static_cast<std::uint16_t>( lhs ) / rhs );
		return lhs;
	}

	constexpr UInt16Buffer operator/( std::uint16_t lhs,
	                                  UInt64Buffer rhs ) noexcept {
		lhs /= static_cast<std::uint16_t>( rhs );
		return static_cast<UInt16Buffer>( lhs );
	}

	constexpr bool operator!( UInt16Buffer value ) {
		return !static_cast<std::uint16_t>( value );
	}

	constexpr UInt16Buffer operator+( UInt16Buffer value ) {
		return value;
	}

	constexpr UInt16Buffer operator-( UInt16Buffer value ) {
		return static_cast<UInt16Buffer>( -static_cast<std::uint16_t>( value ) );
	}

	constexpr UInt16Buffer &operator++( UInt16Buffer &value ) {
		value =
		  static_cast<UInt16Buffer>( static_cast<std::uint16_t>( value ) + 1 );
		return value;
	}

	constexpr UInt16Buffer operator++( UInt16Buffer &value, int ) {
		auto result = value;
		value =
		  static_cast<UInt16Buffer>( static_cast<std::uint16_t>( value ) + 1 );
		return result;
	}

	constexpr UInt16Buffer &operator--( UInt16Buffer &value ) {
		value =
		  static_cast<UInt16Buffer>( static_cast<std::uint16_t>( value ) - 1 );
		return value;
	}

	constexpr UInt16Buffer operator--( UInt16Buffer &value, int ) {
		auto result = value;
		value =
		  static_cast<UInt16Buffer>( static_cast<std::uint16_t>( value ) - 1 );
		return result;
	}

	constexpr UInt16Buffer operator~( UInt16Buffer value ) {
		return static_cast<UInt16Buffer>( ~static_cast<std::uint16_t>( value ) );
	}
	//*********
	constexpr UInt8Buffer &operator+=( UInt8Buffer &lhs,
	                                   UInt8Buffer rhs ) noexcept {
		lhs = static_cast<UInt8Buffer>( static_cast<std::uint8_t>( lhs ) +
		                                static_cast<std::uint8_t>( rhs ) );
		return lhs;
	}

	constexpr UInt8Buffer &operator+=( UInt8Buffer &lhs,
	                                   std::uint8_t rhs ) noexcept {
		lhs = static_cast<UInt8Buffer>( static_cast<std::uint8_t>( lhs ) + rhs );
		return lhs;
	}

	constexpr std::uint8_t &operator+=( std::uint8_t &lhs,
	                                    UInt8Buffer rhs ) noexcept {
		lhs += static_cast<std::uint8_t>( rhs );
		return lhs;
	}

	constexpr UInt8Buffer operator+( UInt8Buffer lhs, UInt8Buffer rhs ) noexcept {
		lhs = static_cast<UInt8Buffer>( static_cast<std::uint8_t>( lhs ) +
		                                static_cast<std::uint8_t>( rhs ) );
		return lhs;
	}

	constexpr UInt8Buffer operator+( UInt8Buffer lhs,
	                                 std::uint8_t rhs ) noexcept {
		lhs = static_cast<UInt8Buffer>( static_cast<std::uint8_t>( lhs ) + rhs );
		return lhs;
	}

	constexpr UInt8Buffer operator+( std::uint8_t lhs,
	                                 UInt8Buffer rhs ) noexcept {
		lhs += static_cast<std::uint8_t>( rhs );
		return static_cast<UInt8Buffer>( lhs );
	}

	constexpr UInt8Buffer &operator-=( UInt8Buffer &lhs,
	                                   UInt8Buffer rhs ) noexcept {
		lhs = static_cast<UInt8Buffer>( static_cast<std::uint8_t>( lhs ) -
		                                static_cast<std::uint8_t>( rhs ) );
		return lhs;
	}

	constexpr UInt8Buffer &operator-=( UInt8Buffer &lhs,
	                                   std::uint8_t rhs ) noexcept {
		lhs = static_cast<UInt8Buffer>( static_cast<std::uint8_t>( lhs ) - rhs );
		return lhs;
	}

	constexpr std::uint8_t &operator-=( std::uint8_t &lhs,
	                                    UInt8Buffer rhs ) noexcept {
		lhs -= static_cast<std::uint8_t>( rhs );
		return lhs;
	}

	constexpr UInt8Buffer operator-( UInt8Buffer lhs, UInt8Buffer rhs ) noexcept {
		lhs = static_cast<UInt8Buffer>( static_cast<std::uint8_t>( lhs ) -
		                                static_cast<std::uint8_t>( rhs ) );
		return lhs;
	}

	constexpr UInt8Buffer operator-( UInt8Buffer lhs,
	                                 std::uint8_t rhs ) noexcept {
		lhs = static_cast<UInt8Buffer>( static_cast<std::uint8_t>( lhs ) - rhs );
		return lhs;
	}

	constexpr UInt8Buffer operator-( std::uint8_t lhs,
	                                 UInt8Buffer rhs ) noexcept {
		lhs -= static_cast<std::uint8_t>( rhs );
		return static_cast<UInt8Buffer>( lhs );
	}

	constexpr UInt8Buffer &operator*=( UInt8Buffer &lhs,
	                                   UInt8Buffer rhs ) noexcept {
		lhs = static_cast<UInt8Buffer>( static_cast<std::uint8_t>( lhs ) *
		                                static_cast<std::uint8_t>( rhs ) );
		return lhs;
	}

	constexpr UInt8Buffer &operator*=( UInt8Buffer &lhs,
	                                   std::uint8_t rhs ) noexcept {
		lhs = static_cast<UInt8Buffer>( static_cast<std::uint8_t>( lhs ) * rhs );
		return lhs;
	}

	constexpr std::uint8_t &operator*=( std::uint8_t &lhs,
	                                    UInt8Buffer rhs ) noexcept {
		lhs *= static_cast<std::uint8_t>( rhs );
		return lhs;
	}

	constexpr UInt8Buffer operator*( UInt8Buffer lhs, UInt8Buffer rhs ) noexcept {
		lhs = static_cast<UInt8Buffer>( static_cast<std::uint8_t>( lhs ) *
		                                static_cast<std::uint8_t>( rhs ) );
		return lhs;
	}

	constexpr UInt8Buffer operator*( UInt8Buffer lhs,
	                                 std::uint64_t rhs ) noexcept {
		lhs = static_cast<UInt8Buffer>( static_cast<std::uint8_t>( lhs ) * rhs );
		return lhs;
	}

	constexpr UInt8Buffer operator*( std::uint8_t lhs,
	                                 UInt64Buffer rhs ) noexcept {
		lhs *= static_cast<std::uint8_t>( rhs );
		return static_cast<UInt8Buffer>( lhs );
	}

	constexpr UInt8Buffer &operator/=( UInt8Buffer &lhs,
	                                   UInt8Buffer rhs ) noexcept {
		lhs = static_cast<UInt8Buffer>( static_cast<std::uint8_t>( lhs ) /
		                                static_cast<std::uint64_t>( rhs ) );
		return lhs;
	}

	constexpr UInt8Buffer &operator/=( UInt8Buffer &lhs,
	                                   std::uint64_t rhs ) noexcept {
		lhs = static_cast<UInt8Buffer>( static_cast<std::uint8_t>( lhs ) / rhs );
		return lhs;
	}

	constexpr std::uint8_t &operator/=( std::uint8_t &lhs,
	                                    UInt64Buffer rhs ) noexcept {
		lhs /= static_cast<std::uint8_t>( rhs );
		return lhs;
	}

	constexpr UInt8Buffer operator/( UInt8Buffer lhs, UInt8Buffer rhs ) noexcept {
		lhs = static_cast<UInt8Buffer>( static_cast<std::uint8_t>( lhs ) /
		                                static_cast<std::uint64_t>( rhs ) );
		return lhs;
	}

	constexpr UInt8Buffer operator/( UInt8Buffer lhs,
	                                 std::uint64_t rhs ) noexcept {
		lhs = static_cast<UInt8Buffer>( static_cast<std::uint8_t>( lhs ) / rhs );
		return lhs;
	}

	constexpr UInt8Buffer operator/( std::uint8_t lhs,
	                                 UInt64Buffer rhs ) noexcept {
		lhs /= static_cast<std::uint8_t>( rhs );
		return static_cast<UInt8Buffer>( lhs );
	}

	constexpr bool operator!( UInt8Buffer value ) {
		return !static_cast<std::uint8_t>( value );
	}

	constexpr UInt8Buffer operator+( UInt8Buffer value ) {
		return value;
	}

	constexpr UInt8Buffer operator-( UInt8Buffer value ) {
		return static_cast<UInt8Buffer>( -static_cast<std::uint8_t>( value ) );
	}

	constexpr UInt8Buffer &operator++( UInt8Buffer &value ) {
		value = static_cast<UInt8Buffer>( static_cast<std::uint8_t>( value ) + 1 );
		return value;
	}

	constexpr UInt8Buffer operator++( UInt8Buffer &value, int ) {
		auto result = value;
		value = static_cast<UInt8Buffer>( static_cast<std::uint8_t>( value ) + 1 );
		return result;
	}

	constexpr UInt8Buffer &operator--( UInt8Buffer &value ) {
		value = static_cast<UInt8Buffer>( static_cast<std::uint8_t>( value ) - 1 );
		return value;
	}

	constexpr UInt8Buffer operator--( UInt8Buffer &value, int ) {
		auto result = value;
		value = static_cast<UInt8Buffer>( static_cast<std::uint8_t>( value ) - 1 );
		return result;
	}

	constexpr UInt8Buffer operator~( UInt8Buffer value ) {
		return static_cast<UInt8Buffer>( ~static_cast<std::uint8_t>( value ) );
	}
	constexpr UInt8Buffer &operator<<=( UInt8Buffer &b,
	                                    std::uint8_t shift ) noexcept {
		static_assert( std::is_unsigned_v<std::uint8_t> );
		auto tmp = static_cast<std::uint8_t>( b );
		tmp <<= static_cast<std::uint8_t>( shift );
		b = static_cast<UInt8Buffer>( tmp );
		return b;
	}

	constexpr UInt8Buffer &operator<<=( UInt8Buffer &b,
	                                    UInt8Buffer shift ) noexcept {
		auto tmp = static_cast<std::uint8_t>( b );
		tmp <<= static_cast<std::uint8_t>( shift );
		b = static_cast<UInt8Buffer>( tmp );
		return b;
	}

	constexpr UInt8Buffer &operator>>=( UInt8Buffer &b,
	                                    std::uint8_t shift ) noexcept {
		static_assert( std::is_unsigned_v<std::uint8_t> );
		auto tmp = static_cast<std::uint8_t>( b );
		tmp >>= static_cast<std::uint8_t>( shift );
		b = static_cast<UInt8Buffer>( tmp );
		return b;
	}

	constexpr UInt8Buffer &operator>>=( UInt8Buffer &b,
	                                    UInt8Buffer shift ) noexcept {
		auto tmp = static_cast<std::uint8_t>( b );
		tmp >>= static_cast<std::uint8_t>( shift );
		b = static_cast<UInt8Buffer>( tmp );
		return b;
	}

	constexpr UInt8Buffer operator<<( UInt8Buffer b,
	                                  std::uint8_t shift ) noexcept {
		static_assert( std::is_unsigned_v<std::uint8_t> );
		auto tmp = static_cast<std::uint8_t>( b );
		tmp <<= static_cast<std::uint8_t>( shift );
		return static_cast<UInt8Buffer>( tmp );
	}

	constexpr UInt8Buffer operator<<( UInt8Buffer b,
	                                  UInt8Buffer shift ) noexcept {
		auto tmp = static_cast<std::uint8_t>( b );
		tmp <<= static_cast<std::uint8_t>( shift );
		return static_cast<UInt8Buffer>( tmp );
	}

	constexpr std::uint8_t operator<<( std::uint8_t b,
	                                   UInt8Buffer shift ) noexcept {
		static_assert( std::is_unsigned_v<std::uint8_t> );
		b <<= static_cast<std::uint8_t>( shift );
		return b;
	}

	constexpr UInt8Buffer operator>>( UInt8Buffer b,
	                                  std::uint8_t shift ) noexcept {
		static_assert( std::is_unsigned_v<std::uint8_t> );
		auto tmp = static_cast<std::uint8_t>( b );
		tmp >>= static_cast<std::uint8_t>( shift );
		return static_cast<UInt8Buffer>( tmp );
	}

	constexpr UInt8Buffer operator>>( UInt8Buffer b,
	                                  UInt8Buffer shift ) noexcept {
		auto tmp = static_cast<std::uint8_t>( b );
		tmp >>= static_cast<std::uint8_t>( shift );
		return static_cast<UInt8Buffer>( tmp );
	}

	constexpr std::uint8_t operator>>( std::uint8_t b,
	                                   UInt8Buffer shift ) noexcept {
		static_assert( std::is_unsigned_v<std::uint8_t> );
		b >>= static_cast<std::uint8_t>( shift );
		return b;
	}

	constexpr UInt8Buffer &operator|=( UInt8Buffer &lhs,
	                                   std::uint8_t rhs ) noexcept {
		static_assert( std::is_unsigned_v<std::uint8_t> );
		auto tmp = static_cast<std::uint8_t>( lhs );
		tmp |= static_cast<std::uint8_t>( rhs );
		lhs = static_cast<UInt8Buffer>( tmp );
		return lhs;
	}

	constexpr UInt8Buffer &operator|=( UInt8Buffer &lhs,
	                                   UInt8Buffer rhs ) noexcept {
		auto tmp = static_cast<std::uint8_t>( lhs );
		tmp |= static_cast<std::uint8_t>( rhs );
		lhs = static_cast<UInt8Buffer>( tmp );
		return lhs;
	}

	constexpr UInt8Buffer &operator&=( UInt8Buffer &lhs,
	                                   std::uint8_t rhs ) noexcept {
		static_assert( std::is_unsigned_v<std::uint8_t> );
		auto tmp = static_cast<std::uint8_t>( lhs );
		tmp &= static_cast<std::uint8_t>( rhs );
		lhs = static_cast<UInt8Buffer>( tmp );
		return lhs;
	}

	constexpr UInt8Buffer &operator&=( UInt8Buffer &lhs,
	                                   UInt8Buffer rhs ) noexcept {
		auto tmp = static_cast<std::uint8_t>( lhs );
		tmp &= static_cast<std::uint8_t>( rhs );
		lhs = static_cast<UInt8Buffer>( tmp );
		return lhs;
	}

	constexpr UInt8Buffer &operator^=( UInt8Buffer &lhs,
	                                   std::uint8_t rhs ) noexcept {
		static_assert( std::is_unsigned_v<std::uint8_t> );
		auto tmp = static_cast<std::uint8_t>( lhs );
		tmp ^= static_cast<std::uint8_t>( rhs );
		lhs = static_cast<UInt8Buffer>( tmp );
		return lhs;
	}

	constexpr UInt8Buffer &operator^=( UInt8Buffer &lhs,
	                                   UInt8Buffer rhs ) noexcept {
		auto tmp = static_cast<std::uint8_t>( lhs );
		tmp ^= static_cast<std::uint8_t>( rhs );
		lhs = static_cast<UInt8Buffer>( tmp );
		return lhs;
	}

	constexpr UInt8Buffer operator|( UInt8Buffer lhs,
	                                 std::uint8_t rhs ) noexcept {
		static_assert( std::is_unsigned_v<std::uint8_t> );
		return static_cast<UInt8Buffer>( static_cast<std::uint8_t>( lhs ) |
		                                 static_cast<std::uint8_t>( rhs ) );
	}

	constexpr UInt8Buffer operator|( std::uint8_t lhs,
	                                 UInt8Buffer rhs ) noexcept {
		static_assert( std::is_unsigned_v<std::uint8_t> );
		return static_cast<UInt8Buffer>( static_cast<std::uint8_t>( lhs ) |
		                                 static_cast<std::uint8_t>( rhs ) );
	}

	constexpr UInt8Buffer operator&( UInt8Buffer lhs,
	                                 std::uint8_t rhs ) noexcept {
		static_assert( std::is_unsigned_v<std::uint8_t> );
		return static_cast<UInt8Buffer>( static_cast<std::uint8_t>( lhs ) &
		                                 static_cast<std::uint8_t>( rhs ) );
	}

	constexpr UInt8Buffer operator&( std::uint8_t lhs,
	                                 UInt8Buffer rhs ) noexcept {
		static_assert( std::is_unsigned_v<std::uint8_t> );
		return static_cast<UInt8Buffer>( static_cast<std::uint8_t>( lhs ) &
		                                 static_cast<std::uint8_t>( rhs ) );
	}

	constexpr UInt8Buffer operator^( UInt8Buffer lhs,
	                                 std::uint8_t rhs ) noexcept {
		static_assert( std::is_unsigned_v<std::uint8_t> );
		return static_cast<UInt8Buffer>( static_cast<std::uint8_t>( lhs ) ^
		                                 static_cast<std::uint8_t>( rhs ) );
	}

	constexpr UInt8Buffer operator^( std::uint8_t lhs,
	                                 UInt8Buffer rhs ) noexcept {
		static_assert( std::is_unsigned_v<std::uint8_t> );
		return static_cast<UInt8Buffer>( static_cast<std::uint8_t>( lhs ) ^
		                                 static_cast<std::uint8_t>( rhs ) );
	}

	constexpr bool operator==( UInt8Buffer lhs, std::uint8_t rhs ) noexcept {
		static_assert( std::is_unsigned_v<std::uint8_t> );
		return static_cast<std::uint8_t>( lhs ) == static_cast<std::uint8_t>( rhs );
	}

	constexpr bool operator==( std::uint8_t lhs, UInt8Buffer rhs ) noexcept {
		static_assert( std::is_unsigned_v<std::uint8_t> );
		return static_cast<std::uint8_t>( lhs ) == static_cast<std::uint8_t>( rhs );
	}

	constexpr bool operator!=( UInt8Buffer lhs, std::uint8_t rhs ) noexcept {
		static_assert( std::is_unsigned_v<std::uint8_t> );
		return static_cast<std::uint8_t>( lhs ) != static_cast<std::uint8_t>( rhs );
	}

	constexpr bool operator!=( std::uint8_t lhs, UInt8Buffer rhs ) noexcept {
		static_assert( std::is_unsigned_v<std::uint8_t> );
		return static_cast<std::uint8_t>( lhs ) != static_cast<std::uint8_t>( rhs );
	}

	constexpr UInt8Buffer operator|( UInt8Buffer lhs, UInt8Buffer rhs ) noexcept {
		return static_cast<UInt8Buffer>( static_cast<std::uint8_t>( lhs ) |
		                                 static_cast<std::uint8_t>( rhs ) );
	}

	constexpr UInt8Buffer operator&( UInt8Buffer lhs, UInt8Buffer rhs ) noexcept {
		return static_cast<UInt8Buffer>( static_cast<std::uint8_t>( lhs ) &
		                                 static_cast<std::uint8_t>( rhs ) );
	}

	constexpr UInt8Buffer operator^( UInt8Buffer lhs, UInt8Buffer rhs ) noexcept {
		return static_cast<UInt8Buffer>( static_cast<std::uint8_t>( lhs ) ^
		                                 static_cast<std::uint8_t>( rhs ) );
	}

	constexpr UInt64Buffer to_uint64_buffer( char const *ptr ) noexcept {
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
		return static_cast<UInt64Buffer>( result );
	}

	constexpr UInt32Buffer to_uint32_buffer( char const *ptr ) noexcept {
		std::uint32_t result = 0;
		result |= static_cast<std::uint32_t>( static_cast<unsigned char>( ptr[3] ) )
		          << 24U;
		result |= static_cast<std::uint32_t>( static_cast<unsigned char>( ptr[2] ) )
		          << 16U;
		result |= static_cast<std::uint32_t>( static_cast<unsigned char>( ptr[1] ) )
		          << 8U;
		result |=
		  static_cast<std::uint32_t>( static_cast<unsigned char>( ptr[0] ) );
		return static_cast<UInt32Buffer>( result );
	}

	constexpr UInt16Buffer to_uint16_buffer( char const *ptr ) noexcept {
		std::uint16_t result = 0;
		result |= static_cast<std::uint16_t>( static_cast<unsigned char>( ptr[1] ) )
		          << 8U;
		result |=
		  static_cast<std::uint16_t>( static_cast<unsigned char>( ptr[0] ) );
		return static_cast<UInt16Buffer>( result );
	}
} // namespace daw
