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

namespace daw::uintbuff_details {
	template<typename Unsigned>
	struct uint_buffer_impl {
		static_assert( std::is_unsigned_v<Unsigned>,
		               "Only unsigned types are supported" );
		enum class type : Unsigned {};
	};
} // namespace daw::uintbuff_details

namespace daw {
	template<typename Unsigned>
	using UIntBuffer =
	  typename uintbuff_details::uint_buffer_impl<Unsigned>::type;
}

constexpr daw::UIntBuffer<std::uint64_t> &
operator<<=( daw::UIntBuffer<std::uint64_t> &b, std::uint64_t shift ) noexcept {
	static_assert( std::is_unsigned_v<std::uint64_t> );
	auto tmp = static_cast<std::uint64_t>( b );
	tmp <<= static_cast<std::uint64_t>( shift );
	b = static_cast<daw::UIntBuffer<std::uint64_t>>( tmp );
	return b;
}

constexpr daw::UIntBuffer<std::uint64_t> &
operator<<=( daw::UIntBuffer<std::uint64_t> &b,
             daw::UIntBuffer<std::uint64_t> shift ) noexcept {
	auto tmp = static_cast<std::uint64_t>( b );
	tmp <<= static_cast<std::uint64_t>( shift );
	b = static_cast<daw::UIntBuffer<std::uint64_t>>( tmp );
	return b;
}

constexpr daw::UIntBuffer<std::uint64_t> &
operator>>=( daw::UIntBuffer<std::uint64_t> &b, std::uint64_t shift ) noexcept {
	static_assert( std::is_unsigned_v<std::uint64_t> );
	auto tmp = static_cast<std::uint64_t>( b );
	tmp >>= static_cast<std::uint64_t>( shift );
	b = static_cast<daw::UIntBuffer<std::uint64_t>>( tmp );
	return b;
}

constexpr daw::UIntBuffer<std::uint64_t> &
operator>>=( daw::UIntBuffer<std::uint64_t> &b,
             daw::UIntBuffer<std::uint64_t> shift ) noexcept {
	auto tmp = static_cast<std::uint64_t>( b );
	tmp >>= static_cast<std::uint64_t>( shift );
	b = static_cast<daw::UIntBuffer<std::uint64_t>>( tmp );
	return b;
}

constexpr daw::UIntBuffer<std::uint64_t>
operator<<( daw::UIntBuffer<std::uint64_t> b, std::uint64_t shift ) noexcept {
	static_assert( std::is_unsigned_v<std::uint64_t> );
	auto tmp = static_cast<std::uint64_t>( b );
	tmp <<= static_cast<std::uint64_t>( shift );
	return static_cast<daw::UIntBuffer<std::uint64_t>>( tmp );
}

constexpr daw::UIntBuffer<std::uint64_t>
operator<<( daw::UIntBuffer<std::uint64_t> b,
            daw::UIntBuffer<std::uint64_t> shift ) noexcept {
	auto tmp = static_cast<std::uint64_t>( b );
	tmp <<= static_cast<std::uint64_t>( shift );
	return static_cast<daw::UIntBuffer<std::uint64_t>>( tmp );
}

constexpr std::uint64_t
operator<<( std::uint64_t b, daw::UIntBuffer<std::uint64_t> shift ) noexcept {
	static_assert( std::is_unsigned_v<std::uint64_t> );
	b <<= static_cast<std::uint64_t>( shift );
	return b;
}

constexpr daw::UIntBuffer<std::uint64_t>
operator>>( daw::UIntBuffer<std::uint64_t> b, std::uint64_t shift ) noexcept {
	static_assert( std::is_unsigned_v<std::uint64_t> );
	auto tmp = static_cast<std::uint64_t>( b );
	tmp >>= static_cast<std::uint64_t>( shift );
	return static_cast<daw::UIntBuffer<std::uint64_t>>( tmp );
}

constexpr daw::UIntBuffer<std::uint64_t>
operator>>( daw::UIntBuffer<std::uint64_t> b,
            daw::UIntBuffer<std::uint64_t> shift ) noexcept {
	auto tmp = static_cast<std::uint64_t>( b );
	tmp >>= static_cast<std::uint64_t>( shift );
	return static_cast<daw::UIntBuffer<std::uint64_t>>( tmp );
}

constexpr std::uint64_t
operator>>( std::uint64_t b, daw::UIntBuffer<std::uint64_t> shift ) noexcept {
	static_assert( std::is_unsigned_v<std::uint64_t> );
	b >>= static_cast<std::uint64_t>( shift );
	return b;
}

constexpr daw::UIntBuffer<std::uint64_t> &
operator|=( daw::UIntBuffer<std::uint64_t> &lhs, std::uint64_t rhs ) noexcept {
	static_assert( std::is_unsigned_v<std::uint64_t> );
	auto tmp = static_cast<std::uint64_t>( lhs );
	tmp |= static_cast<std::uint64_t>( rhs );
	lhs = static_cast<daw::UIntBuffer<std::uint64_t>>( tmp );
	return lhs;
}

constexpr daw::UIntBuffer<std::uint64_t> &
operator|=( daw::UIntBuffer<std::uint64_t> &lhs,
            daw::UIntBuffer<std::uint64_t> rhs ) noexcept {
	auto tmp = static_cast<std::uint64_t>( lhs );
	tmp |= static_cast<std::uint64_t>( rhs );
	lhs = static_cast<daw::UIntBuffer<std::uint64_t>>( tmp );
	return lhs;
}

constexpr daw::UIntBuffer<std::uint64_t> &
operator&=( daw::UIntBuffer<std::uint64_t> &lhs, std::uint64_t rhs ) noexcept {
	static_assert( std::is_unsigned_v<std::uint64_t> );
	auto tmp = static_cast<std::uint64_t>( lhs );
	tmp &= static_cast<std::uint64_t>( rhs );
	lhs = static_cast<daw::UIntBuffer<std::uint64_t>>( tmp );
	return lhs;
}

constexpr daw::UIntBuffer<std::uint64_t> &
operator&=( daw::UIntBuffer<std::uint64_t> &lhs,
            daw::UIntBuffer<std::uint64_t> rhs ) noexcept {
	auto tmp = static_cast<std::uint64_t>( lhs );
	tmp &= static_cast<std::uint64_t>( rhs );
	lhs = static_cast<daw::UIntBuffer<std::uint64_t>>( tmp );
	return lhs;
}

constexpr daw::UIntBuffer<std::uint64_t> &
operator^=( daw::UIntBuffer<std::uint64_t> &lhs, std::uint64_t rhs ) noexcept {
	static_assert( std::is_unsigned_v<std::uint64_t> );
	auto tmp = static_cast<std::uint64_t>( lhs );
	tmp ^= static_cast<std::uint64_t>( rhs );
	lhs = static_cast<daw::UIntBuffer<std::uint64_t>>( tmp );
	return lhs;
}

constexpr daw::UIntBuffer<std::uint64_t> &
operator^=( daw::UIntBuffer<std::uint64_t> &lhs,
            daw::UIntBuffer<std::uint64_t> rhs ) noexcept {
	auto tmp = static_cast<std::uint64_t>( lhs );
	tmp ^= static_cast<std::uint64_t>( rhs );
	lhs = static_cast<daw::UIntBuffer<std::uint64_t>>( tmp );
	return lhs;
}

constexpr daw::UIntBuffer<std::uint64_t>
operator|( daw::UIntBuffer<std::uint64_t> lhs, std::uint64_t rhs ) noexcept {
	static_assert( std::is_unsigned_v<std::uint64_t> );
	return static_cast<daw::UIntBuffer<std::uint64_t>>(
	  static_cast<std::uint64_t>( lhs ) | static_cast<std::uint64_t>( rhs ) );
}

constexpr daw::UIntBuffer<std::uint64_t>
operator|( std::uint64_t lhs, daw::UIntBuffer<std::uint64_t> rhs ) noexcept {
	static_assert( std::is_unsigned_v<std::uint64_t> );
	return static_cast<daw::UIntBuffer<std::uint64_t>>(
	  static_cast<std::uint64_t>( lhs ) | static_cast<std::uint64_t>( rhs ) );
}

constexpr daw::UIntBuffer<std::uint64_t>
operator&( daw::UIntBuffer<std::uint64_t> lhs, std::uint64_t rhs ) noexcept {
	static_assert( std::is_unsigned_v<std::uint64_t> );
	return static_cast<daw::UIntBuffer<std::uint64_t>>(
	  static_cast<std::uint64_t>( lhs ) & static_cast<std::uint64_t>( rhs ) );
}

constexpr daw::UIntBuffer<std::uint64_t>
operator&( std::uint64_t lhs, daw::UIntBuffer<std::uint64_t> rhs ) noexcept {
	static_assert( std::is_unsigned_v<std::uint64_t> );
	return static_cast<daw::UIntBuffer<std::uint64_t>>(
	  static_cast<std::uint64_t>( lhs ) & static_cast<std::uint64_t>( rhs ) );
}

constexpr daw::UIntBuffer<std::uint64_t>
operator^( daw::UIntBuffer<std::uint64_t> lhs, std::uint64_t rhs ) noexcept {
	static_assert( std::is_unsigned_v<std::uint64_t> );
	return static_cast<daw::UIntBuffer<std::uint64_t>>(
	  static_cast<std::uint64_t>( lhs ) ^ static_cast<std::uint64_t>( rhs ) );
}

constexpr daw::UIntBuffer<std::uint64_t>
operator^( std::uint64_t lhs, daw::UIntBuffer<std::uint64_t> rhs ) noexcept {
	static_assert( std::is_unsigned_v<std::uint64_t> );
	return static_cast<daw::UIntBuffer<std::uint64_t>>(
	  static_cast<std::uint64_t>( lhs ) ^ static_cast<std::uint64_t>( rhs ) );
}

constexpr bool operator==( daw::UIntBuffer<std::uint64_t> lhs,
                           std::uint64_t rhs ) noexcept {
	static_assert( std::is_unsigned_v<std::uint64_t> );
	return static_cast<std::uint64_t>( lhs ) == static_cast<std::uint64_t>( rhs );
}

constexpr bool operator==( std::uint64_t lhs,
                           daw::UIntBuffer<std::uint64_t> rhs ) noexcept {
	static_assert( std::is_unsigned_v<std::uint64_t> );
	return static_cast<std::uint64_t>( lhs ) == static_cast<std::uint64_t>( rhs );
}

constexpr bool operator!=( daw::UIntBuffer<std::uint64_t> lhs,
                           std::uint64_t rhs ) noexcept {
	static_assert( std::is_unsigned_v<std::uint64_t> );
	return static_cast<std::uint64_t>( lhs ) != static_cast<std::uint64_t>( rhs );
}

constexpr bool operator!=( std::uint64_t lhs,
                           daw::UIntBuffer<std::uint64_t> rhs ) noexcept {
	static_assert( std::is_unsigned_v<std::uint64_t> );
	return static_cast<std::uint64_t>( lhs ) != static_cast<std::uint64_t>( rhs );
}

constexpr daw::UIntBuffer<std::uint64_t>
operator|( daw::UIntBuffer<std::uint64_t> lhs,
           daw::UIntBuffer<std::uint64_t> rhs ) noexcept {
	return static_cast<daw::UIntBuffer<std::uint64_t>>(
	  static_cast<std::uint64_t>( lhs ) | static_cast<std::uint64_t>( rhs ) );
}

constexpr daw::UIntBuffer<std::uint64_t>
operator&( daw::UIntBuffer<std::uint64_t> lhs,
           daw::UIntBuffer<std::uint64_t> rhs ) noexcept {
	return static_cast<daw::UIntBuffer<std::uint64_t>>(
	  static_cast<std::uint64_t>( lhs ) & static_cast<std::uint64_t>( rhs ) );
}

constexpr daw::UIntBuffer<std::uint64_t>
operator^( daw::UIntBuffer<std::uint64_t> lhs,
           daw::UIntBuffer<std::uint64_t> rhs ) noexcept {
	return static_cast<daw::UIntBuffer<std::uint64_t>>(
	  static_cast<std::uint64_t>( lhs ) ^ static_cast<std::uint64_t>( rhs ) );
}

constexpr daw::UIntBuffer<std::uint32_t> &
operator<<=( daw::UIntBuffer<std::uint32_t> &b, std::uint32_t shift ) noexcept {
	static_assert( std::is_unsigned_v<std::uint32_t> );
	auto tmp = static_cast<std::uint32_t>( b );
	tmp <<= static_cast<std::uint32_t>( shift );
	b = static_cast<daw::UIntBuffer<std::uint32_t>>( tmp );
	return b;
}

constexpr daw::UIntBuffer<std::uint32_t> &
operator<<=( daw::UIntBuffer<std::uint32_t> &b,
             daw::UIntBuffer<std::uint32_t> shift ) noexcept {
	auto tmp = static_cast<std::uint32_t>( b );
	tmp <<= static_cast<std::uint32_t>( shift );
	b = static_cast<daw::UIntBuffer<std::uint32_t>>( tmp );
	return b;
}

constexpr daw::UIntBuffer<std::uint32_t> &
operator>>=( daw::UIntBuffer<std::uint32_t> &b, std::uint32_t shift ) noexcept {
	static_assert( std::is_unsigned_v<std::uint32_t> );
	auto tmp = static_cast<std::uint32_t>( b );
	tmp >>= static_cast<std::uint32_t>( shift );
	b = static_cast<daw::UIntBuffer<std::uint32_t>>( tmp );
	return b;
}

constexpr daw::UIntBuffer<std::uint32_t> &
operator>>=( daw::UIntBuffer<std::uint32_t> &b,
             daw::UIntBuffer<std::uint32_t> shift ) noexcept {
	auto tmp = static_cast<std::uint32_t>( b );
	tmp >>= static_cast<std::uint32_t>( shift );
	b = static_cast<daw::UIntBuffer<std::uint32_t>>( tmp );
	return b;
}

constexpr daw::UIntBuffer<std::uint32_t>
operator<<( daw::UIntBuffer<std::uint32_t> b, std::uint32_t shift ) noexcept {
	static_assert( std::is_unsigned_v<std::uint32_t> );
	auto tmp = static_cast<std::uint32_t>( b );
	tmp <<= static_cast<std::uint32_t>( shift );
	return static_cast<daw::UIntBuffer<std::uint32_t>>( tmp );
}

constexpr daw::UIntBuffer<std::uint32_t>
operator<<( daw::UIntBuffer<std::uint32_t> b,
            daw::UIntBuffer<std::uint32_t> shift ) noexcept {
	auto tmp = static_cast<std::uint32_t>( b );
	tmp <<= static_cast<std::uint32_t>( shift );
	return static_cast<daw::UIntBuffer<std::uint32_t>>( tmp );
}

constexpr std::uint32_t
operator<<( std::uint32_t b, daw::UIntBuffer<std::uint32_t> shift ) noexcept {
	static_assert( std::is_unsigned_v<std::uint32_t> );
	b <<= static_cast<std::uint32_t>( shift );
	return b;
}

constexpr daw::UIntBuffer<std::uint32_t>
operator>>( daw::UIntBuffer<std::uint32_t> b, std::uint32_t shift ) noexcept {
	static_assert( std::is_unsigned_v<std::uint32_t> );
	auto tmp = static_cast<std::uint32_t>( b );
	tmp >>= static_cast<std::uint32_t>( shift );
	return static_cast<daw::UIntBuffer<std::uint32_t>>( tmp );
}

constexpr daw::UIntBuffer<std::uint32_t>
operator>>( daw::UIntBuffer<std::uint32_t> b,
            daw::UIntBuffer<std::uint32_t> shift ) noexcept {
	auto tmp = static_cast<std::uint32_t>( b );
	tmp >>= static_cast<std::uint32_t>( shift );
	return static_cast<daw::UIntBuffer<std::uint32_t>>( tmp );
}

constexpr std::uint32_t
operator>>( std::uint32_t b, daw::UIntBuffer<std::uint32_t> shift ) noexcept {
	static_assert( std::is_unsigned_v<std::uint32_t> );
	b >>= static_cast<std::uint32_t>( shift );
	return b;
}

constexpr daw::UIntBuffer<std::uint32_t> &
operator|=( daw::UIntBuffer<std::uint32_t> &lhs, std::uint32_t rhs ) noexcept {
	static_assert( std::is_unsigned_v<std::uint32_t> );
	auto tmp = static_cast<std::uint32_t>( lhs );
	tmp |= static_cast<std::uint32_t>( rhs );
	lhs = static_cast<daw::UIntBuffer<std::uint32_t>>( tmp );
	return lhs;
}

constexpr daw::UIntBuffer<std::uint32_t> &
operator|=( daw::UIntBuffer<std::uint32_t> &lhs,
            daw::UIntBuffer<std::uint32_t> rhs ) noexcept {
	auto tmp = static_cast<std::uint32_t>( lhs );
	tmp |= static_cast<std::uint32_t>( rhs );
	lhs = static_cast<daw::UIntBuffer<std::uint32_t>>( tmp );
	return lhs;
}

constexpr daw::UIntBuffer<std::uint32_t> &
operator&=( daw::UIntBuffer<std::uint32_t> &lhs, std::uint32_t rhs ) noexcept {
	static_assert( std::is_unsigned_v<std::uint32_t> );
	auto tmp = static_cast<std::uint32_t>( lhs );
	tmp &= static_cast<std::uint32_t>( rhs );
	lhs = static_cast<daw::UIntBuffer<std::uint32_t>>( tmp );
	return lhs;
}

constexpr daw::UIntBuffer<std::uint32_t> &
operator&=( daw::UIntBuffer<std::uint32_t> &lhs,
            daw::UIntBuffer<std::uint32_t> rhs ) noexcept {
	auto tmp = static_cast<std::uint32_t>( lhs );
	tmp &= static_cast<std::uint32_t>( rhs );
	lhs = static_cast<daw::UIntBuffer<std::uint32_t>>( tmp );
	return lhs;
}

constexpr daw::UIntBuffer<std::uint32_t> &
operator^=( daw::UIntBuffer<std::uint32_t> &lhs, std::uint32_t rhs ) noexcept {
	static_assert( std::is_unsigned_v<std::uint32_t> );
	auto tmp = static_cast<std::uint32_t>( lhs );
	tmp ^= static_cast<std::uint32_t>( rhs );
	lhs = static_cast<daw::UIntBuffer<std::uint32_t>>( tmp );
	return lhs;
}

constexpr daw::UIntBuffer<std::uint32_t> &
operator^=( daw::UIntBuffer<std::uint32_t> &lhs,
            daw::UIntBuffer<std::uint32_t> rhs ) noexcept {
	auto tmp = static_cast<std::uint32_t>( lhs );
	tmp ^= static_cast<std::uint32_t>( rhs );
	lhs = static_cast<daw::UIntBuffer<std::uint32_t>>( tmp );
	return lhs;
}

constexpr daw::UIntBuffer<std::uint32_t>
operator|( daw::UIntBuffer<std::uint32_t> lhs, std::uint32_t rhs ) noexcept {
	static_assert( std::is_unsigned_v<std::uint32_t> );
	return static_cast<daw::UIntBuffer<std::uint32_t>>(
	  static_cast<std::uint32_t>( lhs ) | static_cast<std::uint32_t>( rhs ) );
}

constexpr daw::UIntBuffer<std::uint32_t>
operator|( std::uint32_t lhs, daw::UIntBuffer<std::uint32_t> rhs ) noexcept {
	static_assert( std::is_unsigned_v<std::uint32_t> );
	return static_cast<daw::UIntBuffer<std::uint32_t>>(
	  static_cast<std::uint32_t>( lhs ) | static_cast<std::uint32_t>( rhs ) );
}

constexpr daw::UIntBuffer<std::uint32_t>
operator&( daw::UIntBuffer<std::uint32_t> lhs, std::uint32_t rhs ) noexcept {
	static_assert( std::is_unsigned_v<std::uint32_t> );
	return static_cast<daw::UIntBuffer<std::uint32_t>>(
	  static_cast<std::uint32_t>( lhs ) & static_cast<std::uint32_t>( rhs ) );
}

constexpr daw::UIntBuffer<std::uint32_t>
operator&( std::uint32_t lhs, daw::UIntBuffer<std::uint32_t> rhs ) noexcept {
	static_assert( std::is_unsigned_v<std::uint32_t> );
	return static_cast<daw::UIntBuffer<std::uint32_t>>(
	  static_cast<std::uint32_t>( lhs ) & static_cast<std::uint32_t>( rhs ) );
}

constexpr daw::UIntBuffer<std::uint32_t>
operator^( daw::UIntBuffer<std::uint32_t> lhs, std::uint32_t rhs ) noexcept {
	static_assert( std::is_unsigned_v<std::uint32_t> );
	return static_cast<daw::UIntBuffer<std::uint32_t>>(
	  static_cast<std::uint32_t>( lhs ) ^ static_cast<std::uint32_t>( rhs ) );
}

constexpr daw::UIntBuffer<std::uint32_t>
operator^( std::uint32_t lhs, daw::UIntBuffer<std::uint32_t> rhs ) noexcept {
	static_assert( std::is_unsigned_v<std::uint32_t> );
	return static_cast<daw::UIntBuffer<std::uint32_t>>(
	  static_cast<std::uint32_t>( lhs ) ^ static_cast<std::uint32_t>( rhs ) );
}

constexpr bool operator==( daw::UIntBuffer<std::uint32_t> lhs,
                           std::uint32_t rhs ) noexcept {
	static_assert( std::is_unsigned_v<std::uint32_t> );
	return static_cast<std::uint32_t>( lhs ) == static_cast<std::uint32_t>( rhs );
}

constexpr bool operator==( std::uint32_t lhs,
                           daw::UIntBuffer<std::uint32_t> rhs ) noexcept {
	static_assert( std::is_unsigned_v<std::uint32_t> );
	return static_cast<std::uint32_t>( lhs ) == static_cast<std::uint32_t>( rhs );
}

constexpr bool operator!=( daw::UIntBuffer<std::uint32_t> lhs,
                           std::uint32_t rhs ) noexcept {
	static_assert( std::is_unsigned_v<std::uint32_t> );
	return static_cast<std::uint32_t>( lhs ) != static_cast<std::uint32_t>( rhs );
}

constexpr bool operator!=( std::uint32_t lhs,
                           daw::UIntBuffer<std::uint32_t> rhs ) noexcept {
	static_assert( std::is_unsigned_v<std::uint32_t> );
	return static_cast<std::uint32_t>( lhs ) != static_cast<std::uint32_t>( rhs );
}

constexpr daw::UIntBuffer<std::uint32_t>
operator|( daw::UIntBuffer<std::uint32_t> lhs,
           daw::UIntBuffer<std::uint32_t> rhs ) noexcept {
	return static_cast<daw::UIntBuffer<std::uint32_t>>(
	  static_cast<std::uint32_t>( lhs ) | static_cast<std::uint32_t>( rhs ) );
}

constexpr daw::UIntBuffer<std::uint32_t>
operator&( daw::UIntBuffer<std::uint32_t> lhs,
           daw::UIntBuffer<std::uint32_t> rhs ) noexcept {
	return static_cast<daw::UIntBuffer<std::uint32_t>>(
	  static_cast<std::uint32_t>( lhs ) & static_cast<std::uint32_t>( rhs ) );
}

constexpr daw::UIntBuffer<std::uint32_t>
operator^( daw::UIntBuffer<std::uint32_t> lhs,
           daw::UIntBuffer<std::uint32_t> rhs ) noexcept {
	return static_cast<daw::UIntBuffer<std::uint32_t>>(
	  static_cast<std::uint32_t>( lhs ) ^ static_cast<std::uint32_t>( rhs ) );
}

constexpr daw::UIntBuffer<std::uint16_t> &
operator<<=( daw::UIntBuffer<std::uint16_t> &b, std::uint16_t shift ) noexcept {
	static_assert( std::is_unsigned_v<std::uint16_t> );
	auto tmp = static_cast<std::uint16_t>( b );
	tmp <<= static_cast<std::uint16_t>( shift );
	b = static_cast<daw::UIntBuffer<std::uint16_t>>( tmp );
	return b;
}

constexpr daw::UIntBuffer<std::uint16_t> &
operator<<=( daw::UIntBuffer<std::uint16_t> &b,
             daw::UIntBuffer<std::uint16_t> shift ) noexcept {
	auto tmp = static_cast<std::uint16_t>( b );
	tmp <<= static_cast<std::uint16_t>( shift );
	b = static_cast<daw::UIntBuffer<std::uint16_t>>( tmp );
	return b;
}

constexpr daw::UIntBuffer<std::uint16_t> &
operator>>=( daw::UIntBuffer<std::uint16_t> &b, std::uint16_t shift ) noexcept {
	static_assert( std::is_unsigned_v<std::uint16_t> );
	auto tmp = static_cast<std::uint16_t>( b );
	tmp >>= static_cast<std::uint16_t>( shift );
	b = static_cast<daw::UIntBuffer<std::uint16_t>>( tmp );
	return b;
}

constexpr daw::UIntBuffer<std::uint16_t> &
operator>>=( daw::UIntBuffer<std::uint16_t> &b,
             daw::UIntBuffer<std::uint16_t> shift ) noexcept {
	auto tmp = static_cast<std::uint16_t>( b );
	tmp >>= static_cast<std::uint16_t>( shift );
	b = static_cast<daw::UIntBuffer<std::uint16_t>>( tmp );
	return b;
}

constexpr daw::UIntBuffer<std::uint16_t>
operator<<( daw::UIntBuffer<std::uint16_t> b, std::uint16_t shift ) noexcept {
	static_assert( std::is_unsigned_v<std::uint16_t> );
	auto tmp = static_cast<std::uint16_t>( b );
	tmp <<= static_cast<std::uint16_t>( shift );
	return static_cast<daw::UIntBuffer<std::uint16_t>>( tmp );
}

constexpr daw::UIntBuffer<std::uint16_t>
operator<<( daw::UIntBuffer<std::uint16_t> b,
            daw::UIntBuffer<std::uint16_t> shift ) noexcept {
	auto tmp = static_cast<std::uint16_t>( b );
	tmp <<= static_cast<std::uint16_t>( shift );
	return static_cast<daw::UIntBuffer<std::uint16_t>>( tmp );
}

constexpr std::uint16_t
operator<<( std::uint16_t b, daw::UIntBuffer<std::uint16_t> shift ) noexcept {
	static_assert( std::is_unsigned_v<std::uint16_t> );
	b <<= static_cast<std::uint16_t>( shift );
	return b;
}

constexpr daw::UIntBuffer<std::uint16_t>
operator>>( daw::UIntBuffer<std::uint16_t> b, std::uint16_t shift ) noexcept {
	static_assert( std::is_unsigned_v<std::uint16_t> );
	auto tmp = static_cast<std::uint16_t>( b );
	tmp >>= static_cast<std::uint16_t>( shift );
	return static_cast<daw::UIntBuffer<std::uint16_t>>( tmp );
}

constexpr daw::UIntBuffer<std::uint16_t>
operator>>( daw::UIntBuffer<std::uint16_t> b,
            daw::UIntBuffer<std::uint16_t> shift ) noexcept {
	auto tmp = static_cast<std::uint16_t>( b );
	tmp >>= static_cast<std::uint16_t>( shift );
	return static_cast<daw::UIntBuffer<std::uint16_t>>( tmp );
}

constexpr std::uint16_t
operator>>( std::uint16_t b, daw::UIntBuffer<std::uint16_t> shift ) noexcept {
	static_assert( std::is_unsigned_v<std::uint16_t> );
	b >>= static_cast<std::uint16_t>( shift );
	return b;
}

constexpr daw::UIntBuffer<std::uint16_t> &
operator|=( daw::UIntBuffer<std::uint16_t> &lhs, std::uint16_t rhs ) noexcept {
	static_assert( std::is_unsigned_v<std::uint16_t> );
	auto tmp = static_cast<std::uint16_t>( lhs );
	tmp |= static_cast<std::uint16_t>( rhs );
	lhs = static_cast<daw::UIntBuffer<std::uint16_t>>( tmp );
	return lhs;
}

constexpr daw::UIntBuffer<std::uint16_t> &
operator|=( daw::UIntBuffer<std::uint16_t> &lhs,
            daw::UIntBuffer<std::uint16_t> rhs ) noexcept {
	auto tmp = static_cast<std::uint16_t>( lhs );
	tmp |= static_cast<std::uint16_t>( rhs );
	lhs = static_cast<daw::UIntBuffer<std::uint16_t>>( tmp );
	return lhs;
}

constexpr daw::UIntBuffer<std::uint16_t> &
operator&=( daw::UIntBuffer<std::uint16_t> &lhs, std::uint16_t rhs ) noexcept {
	static_assert( std::is_unsigned_v<std::uint16_t> );
	auto tmp = static_cast<std::uint16_t>( lhs );
	tmp &= static_cast<std::uint16_t>( rhs );
	lhs = static_cast<daw::UIntBuffer<std::uint16_t>>( tmp );
	return lhs;
}

constexpr daw::UIntBuffer<std::uint16_t> &
operator&=( daw::UIntBuffer<std::uint16_t> &lhs,
            daw::UIntBuffer<std::uint16_t> rhs ) noexcept {
	auto tmp = static_cast<std::uint16_t>( lhs );
	tmp &= static_cast<std::uint16_t>( rhs );
	lhs = static_cast<daw::UIntBuffer<std::uint16_t>>( tmp );
	return lhs;
}

constexpr daw::UIntBuffer<std::uint16_t> &
operator^=( daw::UIntBuffer<std::uint16_t> &lhs, std::uint16_t rhs ) noexcept {
	static_assert( std::is_unsigned_v<std::uint16_t> );
	auto tmp = static_cast<std::uint16_t>( lhs );
	tmp ^= static_cast<std::uint16_t>( rhs );
	lhs = static_cast<daw::UIntBuffer<std::uint16_t>>( tmp );
	return lhs;
}

constexpr daw::UIntBuffer<std::uint16_t> &
operator^=( daw::UIntBuffer<std::uint16_t> &lhs,
            daw::UIntBuffer<std::uint16_t> rhs ) noexcept {
	auto tmp = static_cast<std::uint16_t>( lhs );
	tmp ^= static_cast<std::uint16_t>( rhs );
	lhs = static_cast<daw::UIntBuffer<std::uint16_t>>( tmp );
	return lhs;
}

constexpr daw::UIntBuffer<std::uint16_t>
operator|( daw::UIntBuffer<std::uint16_t> lhs, std::uint16_t rhs ) noexcept {
	static_assert( std::is_unsigned_v<std::uint16_t> );
	return static_cast<daw::UIntBuffer<std::uint16_t>>(
	  static_cast<std::uint16_t>( lhs ) | static_cast<std::uint16_t>( rhs ) );
}

constexpr daw::UIntBuffer<std::uint16_t>
operator|( std::uint16_t lhs, daw::UIntBuffer<std::uint16_t> rhs ) noexcept {
	static_assert( std::is_unsigned_v<std::uint16_t> );
	return static_cast<daw::UIntBuffer<std::uint16_t>>(
	  static_cast<std::uint16_t>( lhs ) | static_cast<std::uint16_t>( rhs ) );
}

constexpr daw::UIntBuffer<std::uint16_t>
operator&( daw::UIntBuffer<std::uint16_t> lhs, std::uint16_t rhs ) noexcept {
	static_assert( std::is_unsigned_v<std::uint16_t> );
	return static_cast<daw::UIntBuffer<std::uint16_t>>(
	  static_cast<std::uint16_t>( lhs ) & static_cast<std::uint16_t>( rhs ) );
}

constexpr daw::UIntBuffer<std::uint16_t>
operator&( std::uint16_t lhs, daw::UIntBuffer<std::uint16_t> rhs ) noexcept {
	static_assert( std::is_unsigned_v<std::uint16_t> );
	return static_cast<daw::UIntBuffer<std::uint16_t>>(
	  static_cast<std::uint16_t>( lhs ) & static_cast<std::uint16_t>( rhs ) );
}

constexpr daw::UIntBuffer<std::uint16_t>
operator^( daw::UIntBuffer<std::uint16_t> lhs, std::uint16_t rhs ) noexcept {
	static_assert( std::is_unsigned_v<std::uint16_t> );
	return static_cast<daw::UIntBuffer<std::uint16_t>>(
	  static_cast<std::uint16_t>( lhs ) ^ static_cast<std::uint16_t>( rhs ) );
}

constexpr daw::UIntBuffer<std::uint16_t>
operator^( std::uint16_t lhs, daw::UIntBuffer<std::uint16_t> rhs ) noexcept {
	static_assert( std::is_unsigned_v<std::uint16_t> );
	return static_cast<daw::UIntBuffer<std::uint16_t>>(
	  static_cast<std::uint16_t>( lhs ) ^ static_cast<std::uint16_t>( rhs ) );
}

constexpr bool operator==( daw::UIntBuffer<std::uint16_t> lhs,
                           std::uint16_t rhs ) noexcept {
	static_assert( std::is_unsigned_v<std::uint16_t> );
	return static_cast<std::uint16_t>( lhs ) == static_cast<std::uint16_t>( rhs );
}

constexpr bool operator==( std::uint16_t lhs,
                           daw::UIntBuffer<std::uint16_t> rhs ) noexcept {
	static_assert( std::is_unsigned_v<std::uint16_t> );
	return static_cast<std::uint16_t>( lhs ) == static_cast<std::uint16_t>( rhs );
}

constexpr bool operator!=( daw::UIntBuffer<std::uint16_t> lhs,
                           std::uint16_t rhs ) noexcept {
	static_assert( std::is_unsigned_v<std::uint16_t> );
	return static_cast<std::uint16_t>( lhs ) != static_cast<std::uint16_t>( rhs );
}

constexpr bool operator!=( std::uint16_t lhs,
                           daw::UIntBuffer<std::uint16_t> rhs ) noexcept {
	static_assert( std::is_unsigned_v<std::uint16_t> );
	return static_cast<std::uint16_t>( lhs ) != static_cast<std::uint16_t>( rhs );
}

constexpr daw::UIntBuffer<std::uint16_t>
operator|( daw::UIntBuffer<std::uint16_t> lhs,
           daw::UIntBuffer<std::uint16_t> rhs ) noexcept {
	return static_cast<daw::UIntBuffer<std::uint16_t>>(
	  static_cast<std::uint16_t>( lhs ) | static_cast<std::uint16_t>( rhs ) );
}

constexpr daw::UIntBuffer<std::uint16_t>
operator&( daw::UIntBuffer<std::uint16_t> lhs,
           daw::UIntBuffer<std::uint16_t> rhs ) noexcept {
	return static_cast<daw::UIntBuffer<std::uint16_t>>(
	  static_cast<std::uint16_t>( lhs ) & static_cast<std::uint16_t>( rhs ) );
}

constexpr daw::UIntBuffer<std::uint16_t>
operator^( daw::UIntBuffer<std::uint16_t> lhs,
           daw::UIntBuffer<std::uint16_t> rhs ) noexcept {
	return static_cast<daw::UIntBuffer<std::uint16_t>>(
	  static_cast<std::uint16_t>( lhs ) ^ static_cast<std::uint16_t>( rhs ) );
}
//*********
constexpr daw::UIntBuffer<std::uint64_t> &
operator+=( daw::UIntBuffer<std::uint64_t> &lhs,
            daw::UIntBuffer<std::uint64_t> rhs ) noexcept {
	lhs = static_cast<daw::UIntBuffer<std::uint64_t>>(
	  static_cast<std::uint64_t>( lhs ) + static_cast<std::uint64_t>( rhs ) );
	return lhs;
}

constexpr daw::UIntBuffer<std::uint64_t> &
operator+=( daw::UIntBuffer<std::uint64_t> &lhs, std::uint64_t rhs ) noexcept {
	lhs = static_cast<daw::UIntBuffer<std::uint64_t>>(
	  static_cast<std::uint64_t>( lhs ) + rhs );
	return lhs;
}

constexpr std::uint64_t &
operator+=( std::uint64_t &lhs, daw::UIntBuffer<std::uint64_t> rhs ) noexcept {
	lhs += static_cast<std::uint64_t>( rhs );
	return lhs;
}

constexpr daw::UIntBuffer<std::uint64_t>
operator+( daw::UIntBuffer<std::uint64_t> lhs,
           daw::UIntBuffer<std::uint64_t> rhs ) noexcept {
	lhs = static_cast<daw::UIntBuffer<std::uint64_t>>(
	  static_cast<std::uint64_t>( lhs ) + static_cast<std::uint64_t>( rhs ) );
	return lhs;
}

constexpr daw::UIntBuffer<std::uint64_t>
operator+( daw::UIntBuffer<std::uint64_t> lhs, std::uint64_t rhs ) noexcept {
	lhs = static_cast<daw::UIntBuffer<std::uint64_t>>(
	  static_cast<std::uint64_t>( lhs ) + rhs );
	return lhs;
}

constexpr daw::UIntBuffer<std::uint64_t>
operator+( std::uint64_t lhs, daw::UIntBuffer<std::uint64_t> rhs ) noexcept {
	lhs += static_cast<std::uint64_t>( rhs );
	return static_cast<daw::UIntBuffer<std::uint64_t>>( lhs );
}

constexpr daw::UIntBuffer<std::uint64_t> &
operator-=( daw::UIntBuffer<std::uint64_t> &lhs,
            daw::UIntBuffer<std::uint64_t> rhs ) noexcept {
	lhs = static_cast<daw::UIntBuffer<std::uint64_t>>(
	  static_cast<std::uint64_t>( lhs ) - static_cast<std::uint64_t>( rhs ) );
	return lhs;
}

constexpr daw::UIntBuffer<std::uint64_t> &
operator-=( daw::UIntBuffer<std::uint64_t> &lhs, std::uint64_t rhs ) noexcept {
	lhs = static_cast<daw::UIntBuffer<std::uint64_t>>(
	  static_cast<std::uint64_t>( lhs ) - rhs );
	return lhs;
}

constexpr std::uint64_t &
operator-=( std::uint64_t &lhs, daw::UIntBuffer<std::uint64_t> rhs ) noexcept {
	lhs -= static_cast<std::uint64_t>( rhs );
	return lhs;
}

constexpr daw::UIntBuffer<std::uint64_t>
operator-( daw::UIntBuffer<std::uint64_t> lhs,
           daw::UIntBuffer<std::uint64_t> rhs ) noexcept {
	lhs = static_cast<daw::UIntBuffer<std::uint64_t>>(
	  static_cast<std::uint64_t>( lhs ) - static_cast<std::uint64_t>( rhs ) );
	return lhs;
}

constexpr daw::UIntBuffer<std::uint64_t>
operator-( daw::UIntBuffer<std::uint64_t> lhs, std::uint64_t rhs ) noexcept {
	lhs = static_cast<daw::UIntBuffer<std::uint64_t>>(
	  static_cast<std::uint64_t>( lhs ) - rhs );
	return lhs;
}

constexpr daw::UIntBuffer<std::uint64_t>
operator-( std::uint64_t lhs, daw::UIntBuffer<std::uint64_t> rhs ) noexcept {
	lhs -= static_cast<std::uint64_t>( rhs );
	return static_cast<daw::UIntBuffer<std::uint64_t>>( lhs );
}

constexpr daw::UIntBuffer<std::uint64_t> &
operator*=( daw::UIntBuffer<std::uint64_t> &lhs,
            daw::UIntBuffer<std::uint64_t> rhs ) noexcept {
	lhs = static_cast<daw::UIntBuffer<std::uint64_t>>(
	  static_cast<std::uint64_t>( lhs ) * static_cast<std::uint64_t>( rhs ) );
	return lhs;
}

constexpr daw::UIntBuffer<std::uint64_t> &
operator*=( daw::UIntBuffer<std::uint64_t> &lhs, std::uint64_t rhs ) noexcept {
	lhs = static_cast<daw::UIntBuffer<std::uint64_t>>(
	  static_cast<std::uint64_t>( lhs ) * rhs );
	return lhs;
}

constexpr std::uint64_t &
operator*=( std::uint64_t &lhs, daw::UIntBuffer<std::uint64_t> rhs ) noexcept {
	lhs *= static_cast<std::uint64_t>( rhs );
	return lhs;
}

constexpr daw::UIntBuffer<std::uint64_t>
operator*( daw::UIntBuffer<std::uint64_t> lhs,
           daw::UIntBuffer<std::uint64_t> rhs ) noexcept {
	lhs = static_cast<daw::UIntBuffer<std::uint64_t>>(
	  static_cast<std::uint64_t>( lhs ) * static_cast<std::uint64_t>( rhs ) );
	return lhs;
}

constexpr daw::UIntBuffer<std::uint64_t>
operator*( daw::UIntBuffer<std::uint64_t> lhs, std::uint64_t rhs ) noexcept {
	lhs = static_cast<daw::UIntBuffer<std::uint64_t>>(
	  static_cast<std::uint64_t>( lhs ) * rhs );
	return lhs;
}

constexpr daw::UIntBuffer<std::uint64_t>
operator*( std::uint64_t lhs, daw::UIntBuffer<std::uint64_t> rhs ) noexcept {
	lhs *= static_cast<std::uint64_t>( rhs );
	return static_cast<daw::UIntBuffer<std::uint64_t>>( lhs );
}

constexpr daw::UIntBuffer<std::uint64_t> &
operator/=( daw::UIntBuffer<std::uint64_t> &lhs,
            daw::UIntBuffer<std::uint64_t> rhs ) noexcept {
	lhs = static_cast<daw::UIntBuffer<std::uint64_t>>(
	  static_cast<std::uint64_t>( lhs ) / static_cast<std::uint64_t>( rhs ) );
	return lhs;
}

constexpr daw::UIntBuffer<std::uint64_t> &
operator/=( daw::UIntBuffer<std::uint64_t> &lhs, std::uint64_t rhs ) noexcept {
	lhs = static_cast<daw::UIntBuffer<std::uint64_t>>(
	  static_cast<std::uint64_t>( lhs ) / rhs );
	return lhs;
}

constexpr std::uint64_t &
operator/=( std::uint64_t &lhs, daw::UIntBuffer<std::uint64_t> rhs ) noexcept {
	lhs /= static_cast<std::uint64_t>( rhs );
	return lhs;
}

constexpr daw::UIntBuffer<std::uint64_t>
operator/( daw::UIntBuffer<std::uint64_t> lhs,
           daw::UIntBuffer<std::uint64_t> rhs ) noexcept {
	lhs = static_cast<daw::UIntBuffer<std::uint64_t>>(
	  static_cast<std::uint64_t>( lhs ) / static_cast<std::uint64_t>( rhs ) );
	return lhs;
}

constexpr daw::UIntBuffer<std::uint64_t>
operator/( daw::UIntBuffer<std::uint64_t> lhs, std::uint64_t rhs ) noexcept {
	lhs = static_cast<daw::UIntBuffer<std::uint64_t>>(
	  static_cast<std::uint64_t>( lhs ) / rhs );
	return lhs;
}

constexpr daw::UIntBuffer<std::uint64_t>
operator/( std::uint64_t lhs, daw::UIntBuffer<std::uint64_t> rhs ) noexcept {
	lhs /= static_cast<std::uint64_t>( rhs );
	return static_cast<daw::UIntBuffer<std::uint64_t>>( lhs );
}

constexpr bool operator!( daw::UIntBuffer<std::uint64_t> value ) {
	return !static_cast<std::uint64_t>( value );
}

constexpr daw::UIntBuffer<std::uint64_t>
operator+( daw::UIntBuffer<std::uint64_t> value ) {
	return value;
}

constexpr daw::UIntBuffer<std::uint64_t>
operator-( daw::UIntBuffer<std::uint64_t> value ) {
	return static_cast<daw::UIntBuffer<std::uint64_t>>(
	  -static_cast<std::uint64_t>( value ) );
}

constexpr daw::UIntBuffer<std::uint64_t> &
operator++( daw::UIntBuffer<std::uint64_t> &value ) {
	value = static_cast<daw::UIntBuffer<std::uint64_t>>(
	  static_cast<std::uint64_t>( value ) + 1 );
	return value;
}

constexpr daw::UIntBuffer<std::uint64_t>
operator++( daw::UIntBuffer<std::uint64_t> &value, int ) {
	auto result = value;
	value = static_cast<daw::UIntBuffer<std::uint64_t>>(
	  static_cast<std::uint64_t>( value ) + 1 );
	return result;
}

constexpr daw::UIntBuffer<std::uint64_t> &
operator--( daw::UIntBuffer<std::uint64_t> &value ) {
	value = static_cast<daw::UIntBuffer<std::uint64_t>>(
	  static_cast<std::uint64_t>( value ) - 1 );
	return value;
}

constexpr daw::UIntBuffer<std::uint64_t>
operator--( daw::UIntBuffer<std::uint64_t> &value, int ) {
	auto result = value;
	value = static_cast<daw::UIntBuffer<std::uint64_t>>(
	  static_cast<std::uint64_t>( value ) - 1 );
	return result;
}

constexpr daw::UIntBuffer<std::uint64_t>
operator~( daw::UIntBuffer<std::uint64_t> value ) {
	return static_cast<daw::UIntBuffer<std::uint64_t>>(
	  ~static_cast<std::uint64_t>( value ) );
}
//*********
constexpr daw::UIntBuffer<std::uint32_t> &
operator+=( daw::UIntBuffer<std::uint32_t> &lhs,
            daw::UIntBuffer<std::uint32_t> rhs ) noexcept {
	lhs = static_cast<daw::UIntBuffer<std::uint32_t>>(
	  static_cast<std::uint32_t>( lhs ) + static_cast<std::uint32_t>( rhs ) );
	return lhs;
}

constexpr daw::UIntBuffer<std::uint32_t> &
operator+=( daw::UIntBuffer<std::uint32_t> &lhs, std::uint32_t rhs ) noexcept {
	lhs = static_cast<daw::UIntBuffer<std::uint32_t>>(
	  static_cast<std::uint32_t>( lhs ) + rhs );
	return lhs;
}

constexpr std::uint32_t &
operator+=( std::uint32_t &lhs, daw::UIntBuffer<std::uint32_t> rhs ) noexcept {
	lhs += static_cast<std::uint32_t>( rhs );
	return lhs;
}

constexpr daw::UIntBuffer<std::uint32_t>
operator+( daw::UIntBuffer<std::uint32_t> lhs,
           daw::UIntBuffer<std::uint32_t> rhs ) noexcept {
	lhs = static_cast<daw::UIntBuffer<std::uint32_t>>(
	  static_cast<std::uint32_t>( lhs ) + static_cast<std::uint32_t>( rhs ) );
	return lhs;
}

constexpr daw::UIntBuffer<std::uint32_t>
operator+( daw::UIntBuffer<std::uint32_t> lhs, std::uint32_t rhs ) noexcept {
	lhs = static_cast<daw::UIntBuffer<std::uint32_t>>(
	  static_cast<std::uint32_t>( lhs ) + rhs );
	return lhs;
}

constexpr daw::UIntBuffer<std::uint32_t>
operator+( std::uint32_t lhs, daw::UIntBuffer<std::uint32_t> rhs ) noexcept {
	lhs += static_cast<std::uint32_t>( rhs );
	return static_cast<daw::UIntBuffer<std::uint32_t>>( lhs );
}

constexpr daw::UIntBuffer<std::uint32_t> &
operator-=( daw::UIntBuffer<std::uint32_t> &lhs,
            daw::UIntBuffer<std::uint32_t> rhs ) noexcept {
	lhs = static_cast<daw::UIntBuffer<std::uint32_t>>(
	  static_cast<std::uint32_t>( lhs ) - static_cast<std::uint32_t>( rhs ) );
	return lhs;
}

constexpr daw::UIntBuffer<std::uint32_t> &
operator-=( daw::UIntBuffer<std::uint32_t> &lhs, std::uint32_t rhs ) noexcept {
	lhs = static_cast<daw::UIntBuffer<std::uint32_t>>(
	  static_cast<std::uint32_t>( lhs ) - rhs );
	return lhs;
}

constexpr std::uint32_t &
operator-=( std::uint32_t &lhs, daw::UIntBuffer<std::uint32_t> rhs ) noexcept {
	lhs -= static_cast<std::uint32_t>( rhs );
	return lhs;
}

constexpr daw::UIntBuffer<std::uint32_t>
operator-( daw::UIntBuffer<std::uint32_t> lhs,
           daw::UIntBuffer<std::uint32_t> rhs ) noexcept {
	lhs = static_cast<daw::UIntBuffer<std::uint32_t>>(
	  static_cast<std::uint32_t>( lhs ) - static_cast<std::uint32_t>( rhs ) );
	return lhs;
}

constexpr daw::UIntBuffer<std::uint32_t>
operator-( daw::UIntBuffer<std::uint32_t> lhs, std::uint32_t rhs ) noexcept {
	lhs = static_cast<daw::UIntBuffer<std::uint32_t>>(
	  static_cast<std::uint32_t>( lhs ) - rhs );
	return lhs;
}

constexpr daw::UIntBuffer<std::uint32_t>
operator-( std::uint32_t lhs, daw::UIntBuffer<std::uint32_t> rhs ) noexcept {
	lhs -= static_cast<std::uint32_t>( rhs );
	return static_cast<daw::UIntBuffer<std::uint32_t>>( lhs );
}

constexpr daw::UIntBuffer<std::uint32_t> &
operator*=( daw::UIntBuffer<std::uint32_t> &lhs,
            daw::UIntBuffer<std::uint32_t> rhs ) noexcept {
	lhs = static_cast<daw::UIntBuffer<std::uint32_t>>(
	  static_cast<std::uint32_t>( lhs ) * static_cast<std::uint32_t>( rhs ) );
	return lhs;
}

constexpr daw::UIntBuffer<std::uint32_t> &
operator*=( daw::UIntBuffer<std::uint32_t> &lhs, std::uint32_t rhs ) noexcept {
	lhs = static_cast<daw::UIntBuffer<std::uint32_t>>(
	  static_cast<std::uint32_t>( lhs ) * rhs );
	return lhs;
}

constexpr std::uint32_t &
operator*=( std::uint32_t &lhs, daw::UIntBuffer<std::uint32_t> rhs ) noexcept {
	lhs *= static_cast<std::uint32_t>( rhs );
	return lhs;
}

constexpr daw::UIntBuffer<std::uint32_t>
operator*( daw::UIntBuffer<std::uint32_t> lhs,
           daw::UIntBuffer<std::uint32_t> rhs ) noexcept {
	lhs = static_cast<daw::UIntBuffer<std::uint32_t>>(
	  static_cast<std::uint32_t>( lhs ) * static_cast<std::uint32_t>( rhs ) );
	return lhs;
}

constexpr daw::UIntBuffer<std::uint32_t>
operator*( daw::UIntBuffer<std::uint32_t> lhs, std::uint64_t rhs ) noexcept {
	lhs = static_cast<daw::UIntBuffer<std::uint32_t>>(
	  static_cast<std::uint32_t>( lhs ) * rhs );
	return lhs;
}

constexpr daw::UIntBuffer<std::uint32_t>
operator*( std::uint32_t lhs, daw::UIntBuffer<std::uint64_t> rhs ) noexcept {
	lhs *= static_cast<std::uint32_t>( rhs );
	return static_cast<daw::UIntBuffer<std::uint32_t>>( lhs );
}

constexpr daw::UIntBuffer<std::uint32_t> &
operator/=( daw::UIntBuffer<std::uint32_t> &lhs,
            daw::UIntBuffer<std::uint32_t> rhs ) noexcept {
	lhs = static_cast<daw::UIntBuffer<std::uint32_t>>(
	  static_cast<std::uint32_t>( lhs ) / static_cast<std::uint64_t>( rhs ) );
	return lhs;
}

constexpr daw::UIntBuffer<std::uint32_t> &
operator/=( daw::UIntBuffer<std::uint32_t> &lhs, std::uint64_t rhs ) noexcept {
	lhs = static_cast<daw::UIntBuffer<std::uint32_t>>(
	  static_cast<std::uint32_t>( lhs ) / rhs );
	return lhs;
}

constexpr std::uint32_t &
operator/=( std::uint32_t &lhs, daw::UIntBuffer<std::uint64_t> rhs ) noexcept {
	lhs /= static_cast<std::uint32_t>( rhs );
	return lhs;
}

constexpr daw::UIntBuffer<std::uint32_t>
operator/( daw::UIntBuffer<std::uint32_t> lhs,
           daw::UIntBuffer<std::uint32_t> rhs ) noexcept {
	lhs = static_cast<daw::UIntBuffer<std::uint32_t>>(
	  static_cast<std::uint32_t>( lhs ) / static_cast<std::uint64_t>( rhs ) );
	return lhs;
}

constexpr daw::UIntBuffer<std::uint32_t>
operator/( daw::UIntBuffer<std::uint32_t> lhs, std::uint64_t rhs ) noexcept {
	lhs = static_cast<daw::UIntBuffer<std::uint32_t>>(
	  static_cast<std::uint32_t>( lhs ) / rhs );
	return lhs;
}

constexpr daw::UIntBuffer<std::uint32_t>
operator/( std::uint32_t lhs, daw::UIntBuffer<std::uint64_t> rhs ) noexcept {
	lhs /= static_cast<std::uint32_t>( rhs );
	return static_cast<daw::UIntBuffer<std::uint32_t>>( lhs );
}

constexpr bool operator!( daw::UIntBuffer<std::uint32_t> value ) {
	return !static_cast<std::uint32_t>( value );
}

constexpr daw::UIntBuffer<std::uint32_t>
operator+( daw::UIntBuffer<std::uint32_t> value ) {
	return value;
}

constexpr daw::UIntBuffer<std::uint32_t>
operator-( daw::UIntBuffer<std::uint32_t> value ) {
	return static_cast<daw::UIntBuffer<std::uint32_t>>(
	  -static_cast<std::uint32_t>( value ) );
}

constexpr daw::UIntBuffer<std::uint32_t> &
operator++( daw::UIntBuffer<std::uint32_t> &value ) {
	value = static_cast<daw::UIntBuffer<std::uint32_t>>(
	  static_cast<std::uint32_t>( value ) + 1 );
	return value;
}

constexpr daw::UIntBuffer<std::uint32_t>
operator++( daw::UIntBuffer<std::uint32_t> &value, int ) {
	auto result = value;
	value = static_cast<daw::UIntBuffer<std::uint32_t>>(
	  static_cast<std::uint32_t>( value ) + 1 );
	return result;
}

constexpr daw::UIntBuffer<std::uint32_t> &
operator--( daw::UIntBuffer<std::uint32_t> &value ) {
	value = static_cast<daw::UIntBuffer<std::uint32_t>>(
	  static_cast<std::uint32_t>( value ) - 1 );
	return value;
}

constexpr daw::UIntBuffer<std::uint32_t>
operator--( daw::UIntBuffer<std::uint32_t> &value, int ) {
	auto result = value;
	value = static_cast<daw::UIntBuffer<std::uint32_t>>(
	  static_cast<std::uint32_t>( value ) - 1 );
	return result;
}

constexpr daw::UIntBuffer<std::uint32_t>
operator~( daw::UIntBuffer<std::uint32_t> value ) {
	return static_cast<daw::UIntBuffer<std::uint32_t>>(
	  ~static_cast<std::uint32_t>( value ) );
}
//*********
constexpr daw::UIntBuffer<std::uint16_t> &
operator+=( daw::UIntBuffer<std::uint16_t> &lhs,
            daw::UIntBuffer<std::uint16_t> rhs ) noexcept {
	lhs = static_cast<daw::UIntBuffer<std::uint16_t>>(
	  static_cast<std::uint16_t>( lhs ) + static_cast<std::uint16_t>( rhs ) );
	return lhs;
}

constexpr daw::UIntBuffer<std::uint16_t> &
operator+=( daw::UIntBuffer<std::uint16_t> &lhs, std::uint16_t rhs ) noexcept {
	lhs = static_cast<daw::UIntBuffer<std::uint16_t>>(
	  static_cast<std::uint16_t>( lhs ) + rhs );
	return lhs;
}

constexpr std::uint16_t &
operator+=( std::uint16_t &lhs, daw::UIntBuffer<std::uint16_t> rhs ) noexcept {
	lhs += static_cast<std::uint16_t>( rhs );
	return lhs;
}

constexpr daw::UIntBuffer<std::uint16_t>
operator+( daw::UIntBuffer<std::uint16_t> lhs,
           daw::UIntBuffer<std::uint16_t> rhs ) noexcept {
	lhs = static_cast<daw::UIntBuffer<std::uint16_t>>(
	  static_cast<std::uint16_t>( lhs ) + static_cast<std::uint16_t>( rhs ) );
	return lhs;
}

constexpr daw::UIntBuffer<std::uint16_t>
operator+( daw::UIntBuffer<std::uint16_t> lhs, std::uint16_t rhs ) noexcept {
	lhs = static_cast<daw::UIntBuffer<std::uint16_t>>(
	  static_cast<std::uint16_t>( lhs ) + rhs );
	return lhs;
}

constexpr daw::UIntBuffer<std::uint16_t>
operator+( std::uint16_t lhs, daw::UIntBuffer<std::uint16_t> rhs ) noexcept {
	lhs += static_cast<std::uint16_t>( rhs );
	return static_cast<daw::UIntBuffer<std::uint16_t>>( lhs );
}

constexpr daw::UIntBuffer<std::uint16_t> &
operator-=( daw::UIntBuffer<std::uint16_t> &lhs,
            daw::UIntBuffer<std::uint16_t> rhs ) noexcept {
	lhs = static_cast<daw::UIntBuffer<std::uint16_t>>(
	  static_cast<std::uint16_t>( lhs ) - static_cast<std::uint16_t>( rhs ) );
	return lhs;
}

constexpr daw::UIntBuffer<std::uint16_t> &
operator-=( daw::UIntBuffer<std::uint16_t> &lhs, std::uint16_t rhs ) noexcept {
	lhs = static_cast<daw::UIntBuffer<std::uint16_t>>(
	  static_cast<std::uint16_t>( lhs ) - rhs );
	return lhs;
}

constexpr std::uint16_t &
operator-=( std::uint16_t &lhs, daw::UIntBuffer<std::uint16_t> rhs ) noexcept {
	lhs -= static_cast<std::uint16_t>( rhs );
	return lhs;
}

constexpr daw::UIntBuffer<std::uint16_t>
operator-( daw::UIntBuffer<std::uint16_t> lhs,
           daw::UIntBuffer<std::uint16_t> rhs ) noexcept {
	lhs = static_cast<daw::UIntBuffer<std::uint16_t>>(
	  static_cast<std::uint16_t>( lhs ) - static_cast<std::uint16_t>( rhs ) );
	return lhs;
}

constexpr daw::UIntBuffer<std::uint16_t>
operator-( daw::UIntBuffer<std::uint16_t> lhs, std::uint16_t rhs ) noexcept {
	lhs = static_cast<daw::UIntBuffer<std::uint16_t>>(
	  static_cast<std::uint16_t>( lhs ) - rhs );
	return lhs;
}

constexpr daw::UIntBuffer<std::uint16_t>
operator-( std::uint16_t lhs, daw::UIntBuffer<std::uint16_t> rhs ) noexcept {
	lhs -= static_cast<std::uint16_t>( rhs );
	return static_cast<daw::UIntBuffer<std::uint16_t>>( lhs );
}

constexpr daw::UIntBuffer<std::uint16_t> &
operator*=( daw::UIntBuffer<std::uint16_t> &lhs,
            daw::UIntBuffer<std::uint16_t> rhs ) noexcept {
	lhs = static_cast<daw::UIntBuffer<std::uint16_t>>(
	  static_cast<std::uint16_t>( lhs ) * static_cast<std::uint16_t>( rhs ) );
	return lhs;
}

constexpr daw::UIntBuffer<std::uint16_t> &
operator*=( daw::UIntBuffer<std::uint16_t> &lhs, std::uint16_t rhs ) noexcept {
	lhs = static_cast<daw::UIntBuffer<std::uint16_t>>(
	  static_cast<std::uint16_t>( lhs ) * rhs );
	return lhs;
}

constexpr std::uint16_t &
operator*=( std::uint16_t &lhs, daw::UIntBuffer<std::uint16_t> rhs ) noexcept {
	lhs *= static_cast<std::uint16_t>( rhs );
	return lhs;
}

constexpr daw::UIntBuffer<std::uint16_t>
operator*( daw::UIntBuffer<std::uint16_t> lhs,
           daw::UIntBuffer<std::uint16_t> rhs ) noexcept {
	lhs = static_cast<daw::UIntBuffer<std::uint16_t>>(
	  static_cast<std::uint16_t>( lhs ) * static_cast<std::uint16_t>( rhs ) );
	return lhs;
}

constexpr daw::UIntBuffer<std::uint16_t>
operator*( daw::UIntBuffer<std::uint16_t> lhs, std::uint64_t rhs ) noexcept {
	lhs = static_cast<daw::UIntBuffer<std::uint16_t>>(
	  static_cast<std::uint16_t>( lhs ) * rhs );
	return lhs;
}

constexpr daw::UIntBuffer<std::uint16_t>
operator*( std::uint16_t lhs, daw::UIntBuffer<std::uint64_t> rhs ) noexcept {
	lhs *= static_cast<std::uint16_t>( rhs );
	return static_cast<daw::UIntBuffer<std::uint16_t>>( lhs );
}

constexpr daw::UIntBuffer<std::uint16_t> &
operator/=( daw::UIntBuffer<std::uint16_t> &lhs,
            daw::UIntBuffer<std::uint16_t> rhs ) noexcept {
	lhs = static_cast<daw::UIntBuffer<std::uint16_t>>(
	  static_cast<std::uint16_t>( lhs ) / static_cast<std::uint64_t>( rhs ) );
	return lhs;
}

constexpr daw::UIntBuffer<std::uint16_t> &
operator/=( daw::UIntBuffer<std::uint16_t> &lhs, std::uint64_t rhs ) noexcept {
	lhs = static_cast<daw::UIntBuffer<std::uint16_t>>(
	  static_cast<std::uint16_t>( lhs ) / rhs );
	return lhs;
}

constexpr std::uint16_t &
operator/=( std::uint16_t &lhs, daw::UIntBuffer<std::uint64_t> rhs ) noexcept {
	lhs /= static_cast<std::uint16_t>( rhs );
	return lhs;
}

constexpr daw::UIntBuffer<std::uint16_t>
operator/( daw::UIntBuffer<std::uint16_t> lhs,
           daw::UIntBuffer<std::uint16_t> rhs ) noexcept {
	lhs = static_cast<daw::UIntBuffer<std::uint16_t>>(
	  static_cast<std::uint16_t>( lhs ) / static_cast<std::uint64_t>( rhs ) );
	return lhs;
}

constexpr daw::UIntBuffer<std::uint16_t>
operator/( daw::UIntBuffer<std::uint16_t> lhs, std::uint64_t rhs ) noexcept {
	lhs = static_cast<daw::UIntBuffer<std::uint16_t>>(
	  static_cast<std::uint16_t>( lhs ) / rhs );
	return lhs;
}

constexpr daw::UIntBuffer<std::uint16_t>
operator/( std::uint16_t lhs, daw::UIntBuffer<std::uint64_t> rhs ) noexcept {
	lhs /= static_cast<std::uint16_t>( rhs );
	return static_cast<daw::UIntBuffer<std::uint16_t>>( lhs );
}

constexpr bool operator!( daw::UIntBuffer<std::uint16_t> value ) {
	return !static_cast<std::uint16_t>( value );
}

constexpr daw::UIntBuffer<std::uint16_t>
operator+( daw::UIntBuffer<std::uint16_t> value ) {
	return value;
}

constexpr daw::UIntBuffer<std::uint16_t>
operator-( daw::UIntBuffer<std::uint16_t> value ) {
	return static_cast<daw::UIntBuffer<std::uint16_t>>(
	  -static_cast<std::uint16_t>( value ) );
}

constexpr daw::UIntBuffer<std::uint16_t> &
operator++( daw::UIntBuffer<std::uint16_t> &value ) {
	value = static_cast<daw::UIntBuffer<std::uint16_t>>(
	  static_cast<std::uint16_t>( value ) + 1 );
	return value;
}

constexpr daw::UIntBuffer<std::uint16_t>
operator++( daw::UIntBuffer<std::uint16_t> &value, int ) {
	auto result = value;
	value = static_cast<daw::UIntBuffer<std::uint16_t>>(
	  static_cast<std::uint16_t>( value ) + 1 );
	return result;
}

constexpr daw::UIntBuffer<std::uint16_t> &
operator--( daw::UIntBuffer<std::uint16_t> &value ) {
	value = static_cast<daw::UIntBuffer<std::uint16_t>>(
	  static_cast<std::uint16_t>( value ) - 1 );
	return value;
}

constexpr daw::UIntBuffer<std::uint16_t>
operator--( daw::UIntBuffer<std::uint16_t> &value, int ) {
	auto result = value;
	value = static_cast<daw::UIntBuffer<std::uint16_t>>(
	  static_cast<std::uint16_t>( value ) - 1 );
	return result;
}

constexpr daw::UIntBuffer<std::uint16_t>
operator~( daw::UIntBuffer<std::uint16_t> value ) {
	return static_cast<daw::UIntBuffer<std::uint16_t>>(
	  ~static_cast<std::uint16_t>( value ) );
}
//*********
constexpr daw::UIntBuffer<std::uint8_t> &
operator+=( daw::UIntBuffer<std::uint8_t> &lhs,
            daw::UIntBuffer<std::uint8_t> rhs ) noexcept {
	lhs = static_cast<daw::UIntBuffer<std::uint8_t>>(
	  static_cast<std::uint8_t>( lhs ) + static_cast<std::uint8_t>( rhs ) );
	return lhs;
}

constexpr daw::UIntBuffer<std::uint8_t> &
operator+=( daw::UIntBuffer<std::uint8_t> &lhs, std::uint8_t rhs ) noexcept {
	lhs = static_cast<daw::UIntBuffer<std::uint8_t>>(
	  static_cast<std::uint8_t>( lhs ) + rhs );
	return lhs;
}

constexpr std::uint8_t &
operator+=( std::uint8_t &lhs, daw::UIntBuffer<std::uint8_t> rhs ) noexcept {
	lhs += static_cast<std::uint8_t>( rhs );
	return lhs;
}

constexpr daw::UIntBuffer<std::uint8_t>
operator+( daw::UIntBuffer<std::uint8_t> lhs,
           daw::UIntBuffer<std::uint8_t> rhs ) noexcept {
	lhs = static_cast<daw::UIntBuffer<std::uint8_t>>(
	  static_cast<std::uint8_t>( lhs ) + static_cast<std::uint8_t>( rhs ) );
	return lhs;
}

constexpr daw::UIntBuffer<std::uint8_t>
operator+( daw::UIntBuffer<std::uint8_t> lhs, std::uint8_t rhs ) noexcept {
	lhs = static_cast<daw::UIntBuffer<std::uint8_t>>(
	  static_cast<std::uint8_t>( lhs ) + rhs );
	return lhs;
}

constexpr daw::UIntBuffer<std::uint8_t>
operator+( std::uint8_t lhs, daw::UIntBuffer<std::uint8_t> rhs ) noexcept {
	lhs += static_cast<std::uint8_t>( rhs );
	return static_cast<daw::UIntBuffer<std::uint8_t>>( lhs );
}

constexpr daw::UIntBuffer<std::uint8_t> &
operator-=( daw::UIntBuffer<std::uint8_t> &lhs,
            daw::UIntBuffer<std::uint8_t> rhs ) noexcept {
	lhs = static_cast<daw::UIntBuffer<std::uint8_t>>(
	  static_cast<std::uint8_t>( lhs ) - static_cast<std::uint8_t>( rhs ) );
	return lhs;
}

constexpr daw::UIntBuffer<std::uint8_t> &
operator-=( daw::UIntBuffer<std::uint8_t> &lhs, std::uint8_t rhs ) noexcept {
	lhs = static_cast<daw::UIntBuffer<std::uint8_t>>(
	  static_cast<std::uint8_t>( lhs ) - rhs );
	return lhs;
}

constexpr std::uint8_t &
operator-=( std::uint8_t &lhs, daw::UIntBuffer<std::uint8_t> rhs ) noexcept {
	lhs -= static_cast<std::uint8_t>( rhs );
	return lhs;
}

constexpr daw::UIntBuffer<std::uint8_t>
operator-( daw::UIntBuffer<std::uint8_t> lhs,
           daw::UIntBuffer<std::uint8_t> rhs ) noexcept {
	lhs = static_cast<daw::UIntBuffer<std::uint8_t>>(
	  static_cast<std::uint8_t>( lhs ) - static_cast<std::uint8_t>( rhs ) );
	return lhs;
}

constexpr daw::UIntBuffer<std::uint8_t>
operator-( daw::UIntBuffer<std::uint8_t> lhs, std::uint8_t rhs ) noexcept {
	lhs = static_cast<daw::UIntBuffer<std::uint8_t>>(
	  static_cast<std::uint8_t>( lhs ) - rhs );
	return lhs;
}

constexpr daw::UIntBuffer<std::uint8_t>
operator-( std::uint8_t lhs, daw::UIntBuffer<std::uint8_t> rhs ) noexcept {
	lhs -= static_cast<std::uint8_t>( rhs );
	return static_cast<daw::UIntBuffer<std::uint8_t>>( lhs );
}

constexpr daw::UIntBuffer<std::uint8_t> &
operator*=( daw::UIntBuffer<std::uint8_t> &lhs,
            daw::UIntBuffer<std::uint8_t> rhs ) noexcept {
	lhs = static_cast<daw::UIntBuffer<std::uint8_t>>(
	  static_cast<std::uint8_t>( lhs ) * static_cast<std::uint8_t>( rhs ) );
	return lhs;
}

constexpr daw::UIntBuffer<std::uint8_t> &
operator*=( daw::UIntBuffer<std::uint8_t> &lhs, std::uint8_t rhs ) noexcept {
	lhs = static_cast<daw::UIntBuffer<std::uint8_t>>(
	  static_cast<std::uint8_t>( lhs ) * rhs );
	return lhs;
}

constexpr std::uint8_t &
operator*=( std::uint8_t &lhs, daw::UIntBuffer<std::uint8_t> rhs ) noexcept {
	lhs *= static_cast<std::uint8_t>( rhs );
	return lhs;
}

constexpr daw::UIntBuffer<std::uint8_t>
operator*( daw::UIntBuffer<std::uint8_t> lhs,
           daw::UIntBuffer<std::uint8_t> rhs ) noexcept {
	lhs = static_cast<daw::UIntBuffer<std::uint8_t>>(
	  static_cast<std::uint8_t>( lhs ) * static_cast<std::uint8_t>( rhs ) );
	return lhs;
}

constexpr daw::UIntBuffer<std::uint8_t>
operator*( daw::UIntBuffer<std::uint8_t> lhs, std::uint64_t rhs ) noexcept {
	lhs = static_cast<daw::UIntBuffer<std::uint8_t>>(
	  static_cast<std::uint8_t>( lhs ) * rhs );
	return lhs;
}

constexpr daw::UIntBuffer<std::uint8_t>
operator*( std::uint8_t lhs, daw::UIntBuffer<std::uint64_t> rhs ) noexcept {
	lhs *= static_cast<std::uint8_t>( rhs );
	return static_cast<daw::UIntBuffer<std::uint8_t>>( lhs );
}

constexpr daw::UIntBuffer<std::uint8_t> &
operator/=( daw::UIntBuffer<std::uint8_t> &lhs,
            daw::UIntBuffer<std::uint8_t> rhs ) noexcept {
	lhs = static_cast<daw::UIntBuffer<std::uint8_t>>(
	  static_cast<std::uint8_t>( lhs ) / static_cast<std::uint64_t>( rhs ) );
	return lhs;
}

constexpr daw::UIntBuffer<std::uint8_t> &
operator/=( daw::UIntBuffer<std::uint8_t> &lhs, std::uint64_t rhs ) noexcept {
	lhs = static_cast<daw::UIntBuffer<std::uint8_t>>(
	  static_cast<std::uint8_t>( lhs ) / rhs );
	return lhs;
}

constexpr std::uint8_t &
operator/=( std::uint8_t &lhs, daw::UIntBuffer<std::uint64_t> rhs ) noexcept {
	lhs /= static_cast<std::uint8_t>( rhs );
	return lhs;
}

constexpr daw::UIntBuffer<std::uint8_t>
operator/( daw::UIntBuffer<std::uint8_t> lhs,
           daw::UIntBuffer<std::uint8_t> rhs ) noexcept {
	lhs = static_cast<daw::UIntBuffer<std::uint8_t>>(
	  static_cast<std::uint8_t>( lhs ) / static_cast<std::uint64_t>( rhs ) );
	return lhs;
}

constexpr daw::UIntBuffer<std::uint8_t>
operator/( daw::UIntBuffer<std::uint8_t> lhs, std::uint64_t rhs ) noexcept {
	lhs = static_cast<daw::UIntBuffer<std::uint8_t>>(
	  static_cast<std::uint8_t>( lhs ) / rhs );
	return lhs;
}

constexpr daw::UIntBuffer<std::uint8_t>
operator/( std::uint8_t lhs, daw::UIntBuffer<std::uint64_t> rhs ) noexcept {
	lhs /= static_cast<std::uint8_t>( rhs );
	return static_cast<daw::UIntBuffer<std::uint8_t>>( lhs );
}

constexpr bool operator!( daw::UIntBuffer<std::uint8_t> value ) {
	return !static_cast<std::uint8_t>( value );
}

constexpr daw::UIntBuffer<std::uint8_t>
operator+( daw::UIntBuffer<std::uint8_t> value ) {
	return value;
}

constexpr daw::UIntBuffer<std::uint8_t>
operator-( daw::UIntBuffer<std::uint8_t> value ) {
	return static_cast<daw::UIntBuffer<std::uint8_t>>(
	  -static_cast<std::uint8_t>( value ) );
}

constexpr daw::UIntBuffer<std::uint8_t> &
operator++( daw::UIntBuffer<std::uint8_t> &value ) {
	value = static_cast<daw::UIntBuffer<std::uint8_t>>(
	  static_cast<std::uint8_t>( value ) + 1 );
	return value;
}

constexpr daw::UIntBuffer<std::uint8_t>
operator++( daw::UIntBuffer<std::uint8_t> &value, int ) {
	auto result = value;
	value = static_cast<daw::UIntBuffer<std::uint8_t>>(
	  static_cast<std::uint8_t>( value ) + 1 );
	return result;
}

constexpr daw::UIntBuffer<std::uint8_t> &
operator--( daw::UIntBuffer<std::uint8_t> &value ) {
	value = static_cast<daw::UIntBuffer<std::uint8_t>>(
	  static_cast<std::uint8_t>( value ) - 1 );
	return value;
}

constexpr daw::UIntBuffer<std::uint8_t>
operator--( daw::UIntBuffer<std::uint8_t> &value, int ) {
	auto result = value;
	value = static_cast<daw::UIntBuffer<std::uint8_t>>(
	  static_cast<std::uint8_t>( value ) - 1 );
	return result;
}

constexpr daw::UIntBuffer<std::uint8_t>
operator~( daw::UIntBuffer<std::uint8_t> value ) {
	return static_cast<daw::UIntBuffer<std::uint8_t>>(
	  ~static_cast<std::uint8_t>( value ) );
}
constexpr daw::UIntBuffer<std::uint8_t> &
operator<<=( daw::UIntBuffer<std::uint8_t> &b, std::uint8_t shift ) noexcept {
	static_assert( std::is_unsigned_v<std::uint8_t> );
	auto tmp = static_cast<std::uint8_t>( b );
	tmp <<= static_cast<std::uint8_t>( shift );
	b = static_cast<daw::UIntBuffer<std::uint8_t>>( tmp );
	return b;
}

constexpr daw::UIntBuffer<std::uint8_t> &
operator<<=( daw::UIntBuffer<std::uint8_t> &b,
             daw::UIntBuffer<std::uint8_t> shift ) noexcept {
	auto tmp = static_cast<std::uint8_t>( b );
	tmp <<= static_cast<std::uint8_t>( shift );
	b = static_cast<daw::UIntBuffer<std::uint8_t>>( tmp );
	return b;
}

constexpr daw::UIntBuffer<std::uint8_t> &
operator>>=( daw::UIntBuffer<std::uint8_t> &b, std::uint8_t shift ) noexcept {
	static_assert( std::is_unsigned_v<std::uint8_t> );
	auto tmp = static_cast<std::uint8_t>( b );
	tmp >>= static_cast<std::uint8_t>( shift );
	b = static_cast<daw::UIntBuffer<std::uint8_t>>( tmp );
	return b;
}

constexpr daw::UIntBuffer<std::uint8_t> &
operator>>=( daw::UIntBuffer<std::uint8_t> &b,
             daw::UIntBuffer<std::uint8_t> shift ) noexcept {
	auto tmp = static_cast<std::uint8_t>( b );
	tmp >>= static_cast<std::uint8_t>( shift );
	b = static_cast<daw::UIntBuffer<std::uint8_t>>( tmp );
	return b;
}

constexpr daw::UIntBuffer<std::uint8_t>
operator<<( daw::UIntBuffer<std::uint8_t> b, std::uint8_t shift ) noexcept {
	static_assert( std::is_unsigned_v<std::uint8_t> );
	auto tmp = static_cast<std::uint8_t>( b );
	tmp <<= static_cast<std::uint8_t>( shift );
	return static_cast<daw::UIntBuffer<std::uint8_t>>( tmp );
}

constexpr daw::UIntBuffer<std::uint8_t>
operator<<( daw::UIntBuffer<std::uint8_t> b,
            daw::UIntBuffer<std::uint8_t> shift ) noexcept {
	auto tmp = static_cast<std::uint8_t>( b );
	tmp <<= static_cast<std::uint8_t>( shift );
	return static_cast<daw::UIntBuffer<std::uint8_t>>( tmp );
}

constexpr std::uint8_t
operator<<( std::uint8_t b, daw::UIntBuffer<std::uint8_t> shift ) noexcept {
	static_assert( std::is_unsigned_v<std::uint8_t> );
	b <<= static_cast<std::uint8_t>( shift );
	return b;
}

constexpr daw::UIntBuffer<std::uint8_t>
operator>>( daw::UIntBuffer<std::uint8_t> b, std::uint8_t shift ) noexcept {
	static_assert( std::is_unsigned_v<std::uint8_t> );
	auto tmp = static_cast<std::uint8_t>( b );
	tmp >>= static_cast<std::uint8_t>( shift );
	return static_cast<daw::UIntBuffer<std::uint8_t>>( tmp );
}

constexpr daw::UIntBuffer<std::uint8_t>
operator>>( daw::UIntBuffer<std::uint8_t> b,
            daw::UIntBuffer<std::uint8_t> shift ) noexcept {
	auto tmp = static_cast<std::uint8_t>( b );
	tmp >>= static_cast<std::uint8_t>( shift );
	return static_cast<daw::UIntBuffer<std::uint8_t>>( tmp );
}

constexpr std::uint8_t
operator>>( std::uint8_t b, daw::UIntBuffer<std::uint8_t> shift ) noexcept {
	static_assert( std::is_unsigned_v<std::uint8_t> );
	b >>= static_cast<std::uint8_t>( shift );
	return b;
}

constexpr daw::UIntBuffer<std::uint8_t> &
operator|=( daw::UIntBuffer<std::uint8_t> &lhs, std::uint8_t rhs ) noexcept {
	static_assert( std::is_unsigned_v<std::uint8_t> );
	auto tmp = static_cast<std::uint8_t>( lhs );
	tmp |= static_cast<std::uint8_t>( rhs );
	lhs = static_cast<daw::UIntBuffer<std::uint8_t>>( tmp );
	return lhs;
}

constexpr daw::UIntBuffer<std::uint8_t> &
operator|=( daw::UIntBuffer<std::uint8_t> &lhs,
            daw::UIntBuffer<std::uint8_t> rhs ) noexcept {
	auto tmp = static_cast<std::uint8_t>( lhs );
	tmp |= static_cast<std::uint8_t>( rhs );
	lhs = static_cast<daw::UIntBuffer<std::uint8_t>>( tmp );
	return lhs;
}

constexpr daw::UIntBuffer<std::uint8_t> &
operator&=( daw::UIntBuffer<std::uint8_t> &lhs, std::uint8_t rhs ) noexcept {
	static_assert( std::is_unsigned_v<std::uint8_t> );
	auto tmp = static_cast<std::uint8_t>( lhs );
	tmp &= static_cast<std::uint8_t>( rhs );
	lhs = static_cast<daw::UIntBuffer<std::uint8_t>>( tmp );
	return lhs;
}

constexpr daw::UIntBuffer<std::uint8_t> &
operator&=( daw::UIntBuffer<std::uint8_t> &lhs,
            daw::UIntBuffer<std::uint8_t> rhs ) noexcept {
	auto tmp = static_cast<std::uint8_t>( lhs );
	tmp &= static_cast<std::uint8_t>( rhs );
	lhs = static_cast<daw::UIntBuffer<std::uint8_t>>( tmp );
	return lhs;
}

constexpr daw::UIntBuffer<std::uint8_t> &
operator^=( daw::UIntBuffer<std::uint8_t> &lhs, std::uint8_t rhs ) noexcept {
	static_assert( std::is_unsigned_v<std::uint8_t> );
	auto tmp = static_cast<std::uint8_t>( lhs );
	tmp ^= static_cast<std::uint8_t>( rhs );
	lhs = static_cast<daw::UIntBuffer<std::uint8_t>>( tmp );
	return lhs;
}

constexpr daw::UIntBuffer<std::uint8_t> &
operator^=( daw::UIntBuffer<std::uint8_t> &lhs,
            daw::UIntBuffer<std::uint8_t> rhs ) noexcept {
	auto tmp = static_cast<std::uint8_t>( lhs );
	tmp ^= static_cast<std::uint8_t>( rhs );
	lhs = static_cast<daw::UIntBuffer<std::uint8_t>>( tmp );
	return lhs;
}

constexpr daw::UIntBuffer<std::uint8_t>
operator|( daw::UIntBuffer<std::uint8_t> lhs, std::uint8_t rhs ) noexcept {
	static_assert( std::is_unsigned_v<std::uint8_t> );
	return static_cast<daw::UIntBuffer<std::uint8_t>>(
	  static_cast<std::uint8_t>( lhs ) | static_cast<std::uint8_t>( rhs ) );
}

constexpr daw::UIntBuffer<std::uint8_t>
operator|( std::uint8_t lhs, daw::UIntBuffer<std::uint8_t> rhs ) noexcept {
	static_assert( std::is_unsigned_v<std::uint8_t> );
	return static_cast<daw::UIntBuffer<std::uint8_t>>(
	  static_cast<std::uint8_t>( lhs ) | static_cast<std::uint8_t>( rhs ) );
}

constexpr daw::UIntBuffer<std::uint8_t>
operator&( daw::UIntBuffer<std::uint8_t> lhs, std::uint8_t rhs ) noexcept {
	static_assert( std::is_unsigned_v<std::uint8_t> );
	return static_cast<daw::UIntBuffer<std::uint8_t>>(
	  static_cast<std::uint8_t>( lhs ) & static_cast<std::uint8_t>( rhs ) );
}

constexpr daw::UIntBuffer<std::uint8_t>
operator&( std::uint8_t lhs, daw::UIntBuffer<std::uint8_t> rhs ) noexcept {
	static_assert( std::is_unsigned_v<std::uint8_t> );
	return static_cast<daw::UIntBuffer<std::uint8_t>>(
	  static_cast<std::uint8_t>( lhs ) & static_cast<std::uint8_t>( rhs ) );
}

constexpr daw::UIntBuffer<std::uint8_t>
operator^( daw::UIntBuffer<std::uint8_t> lhs, std::uint8_t rhs ) noexcept {
	static_assert( std::is_unsigned_v<std::uint8_t> );
	return static_cast<daw::UIntBuffer<std::uint8_t>>(
	  static_cast<std::uint8_t>( lhs ) ^ static_cast<std::uint8_t>( rhs ) );
}

constexpr daw::UIntBuffer<std::uint8_t>
operator^( std::uint8_t lhs, daw::UIntBuffer<std::uint8_t> rhs ) noexcept {
	static_assert( std::is_unsigned_v<std::uint8_t> );
	return static_cast<daw::UIntBuffer<std::uint8_t>>(
	  static_cast<std::uint8_t>( lhs ) ^ static_cast<std::uint8_t>( rhs ) );
}

constexpr bool operator==( daw::UIntBuffer<std::uint8_t> lhs,
                           std::uint8_t rhs ) noexcept {
	static_assert( std::is_unsigned_v<std::uint8_t> );
	return static_cast<std::uint8_t>( lhs ) == static_cast<std::uint8_t>( rhs );
}

constexpr bool operator==( std::uint8_t lhs,
                           daw::UIntBuffer<std::uint8_t> rhs ) noexcept {
	static_assert( std::is_unsigned_v<std::uint8_t> );
	return static_cast<std::uint8_t>( lhs ) == static_cast<std::uint8_t>( rhs );
}

constexpr bool operator!=( daw::UIntBuffer<std::uint8_t> lhs,
                           std::uint8_t rhs ) noexcept {
	static_assert( std::is_unsigned_v<std::uint8_t> );
	return static_cast<std::uint8_t>( lhs ) != static_cast<std::uint8_t>( rhs );
}

constexpr bool operator!=( std::uint8_t lhs,
                           daw::UIntBuffer<std::uint8_t> rhs ) noexcept {
	static_assert( std::is_unsigned_v<std::uint8_t> );
	return static_cast<std::uint8_t>( lhs ) != static_cast<std::uint8_t>( rhs );
}

constexpr daw::UIntBuffer<std::uint8_t>
operator|( daw::UIntBuffer<std::uint8_t> lhs,
           daw::UIntBuffer<std::uint8_t> rhs ) noexcept {
	return static_cast<daw::UIntBuffer<std::uint8_t>>(
	  static_cast<std::uint8_t>( lhs ) | static_cast<std::uint8_t>( rhs ) );
}

constexpr daw::UIntBuffer<std::uint8_t>
operator&( daw::UIntBuffer<std::uint8_t> lhs,
           daw::UIntBuffer<std::uint8_t> rhs ) noexcept {
	return static_cast<daw::UIntBuffer<std::uint8_t>>(
	  static_cast<std::uint8_t>( lhs ) & static_cast<std::uint8_t>( rhs ) );
}

constexpr daw::UIntBuffer<std::uint8_t>
operator^( daw::UIntBuffer<std::uint8_t> lhs,
           daw::UIntBuffer<std::uint8_t> rhs ) noexcept {
	return static_cast<daw::UIntBuffer<std::uint8_t>>(
	  static_cast<std::uint8_t>( lhs ) ^ static_cast<std::uint8_t>( rhs ) );
}
//*********
namespace daw {
	using UInt64Buffer = UIntBuffer<std::uint64_t>;
	using UInt32Buffer = UIntBuffer<std::uint32_t>;
	using UInt16Buffer = UIntBuffer<std::uint16_t>;
	using UInt8Buffer = UIntBuffer<std::uint8_t>;

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
