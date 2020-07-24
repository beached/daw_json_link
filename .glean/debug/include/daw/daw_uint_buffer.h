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
	namespace uintbuff_details {
		template<typename Unsigned>
		struct uint_buffer_impl {
			static_assert( std::is_unsigned_v<Unsigned>,
			               "Only unsigned types are supported" );
			enum class type : Unsigned {};
		};
	} // namespace uintbuff_details

	template<typename Unsigned>
	using UIntBuffer =
	  typename uintbuff_details::uint_buffer_impl<Unsigned>::type;

	using UInt64Buffer = UIntBuffer<std::uint64_t>;
	using UInt32Buffer = UIntBuffer<std::uint32_t>;
	using UInt16Buffer = UIntBuffer<std::uint16_t>;

	template<typename Unsigned>
	constexpr UInt64Buffer &operator<<=( UInt64Buffer &b,
	                                     Unsigned shift ) noexcept {
		static_assert( std::is_unsigned_v<Unsigned> );
		auto tmp = static_cast<std::uint64_t>( b );
		tmp <<= shift;
		b = static_cast<UInt64Buffer>( tmp );
		return b;
	}

	template<typename Unsigned>
	constexpr UInt64Buffer &operator>>=( UInt64Buffer &b,
	                                     Unsigned shift ) noexcept {
		static_assert( std::is_unsigned_v<Unsigned> );
		auto tmp = static_cast<std::uint64_t>( b );
		tmp >>= shift;
		b = static_cast<UInt64Buffer>( tmp );
		return b;
	}

	template<typename Unsigned>
	constexpr UInt64Buffer operator<<( UInt64Buffer b, unsigned shift ) noexcept {
		static_assert( std::is_unsigned_v<Unsigned> );
		auto tmp = static_cast<std::uint64_t>( b );
		tmp <<= shift;
		return static_cast<UInt64Buffer>( tmp );
	}

	template<typename Unsigned>
	constexpr UInt64Buffer operator>>( UInt64Buffer b, Unsigned shift ) noexcept {
		static_assert( std::is_unsigned_v<Unsigned> );
		auto tmp = static_cast<std::uint64_t>( b );
		tmp >>= shift;
		return static_cast<UInt64Buffer>( tmp );
	}

	template<typename Unsigned>
	constexpr UInt64Buffer &operator|=( UInt64Buffer &lhs,
	                                    Unsigned rhs ) noexcept {
		static_assert( std::is_unsigned_v<Unsigned> );
		auto tmp = static_cast<std::uint64_t>( lhs );
		tmp |= static_cast<std::uint64_t>( rhs );
		lhs = static_cast<UInt64Buffer>( tmp );
		return lhs;
	}

	template<typename Unsigned>
	constexpr UInt64Buffer &operator&=( UInt64Buffer &lhs,
	                                    Unsigned rhs ) noexcept {
		static_assert( std::is_unsigned_v<Unsigned> );
		auto tmp = static_cast<std::uint64_t>( lhs );
		tmp &= static_cast<std::uint64_t>( rhs );
		lhs = static_cast<UInt64Buffer>( tmp );
		return lhs;
	}

	template<typename Unsigned>
	constexpr UInt64Buffer &operator^=( UInt64Buffer &lhs,
	                                    Unsigned rhs ) noexcept {
		static_assert( std::is_unsigned_v<Unsigned> );
		auto tmp = static_cast<std::uint64_t>( lhs );
		tmp ^= static_cast<std::uint64_t>( rhs );
		lhs = static_cast<UInt64Buffer>( tmp );
		return lhs;
	}

	template<typename Unsigned>
	constexpr UInt64Buffer operator|( UInt64Buffer lhs, Unsigned rhs ) noexcept {
		static_assert( std::is_unsigned_v<Unsigned> );
		return static_cast<UInt64Buffer>( static_cast<std::uint64_t>( lhs ) |
		                                  static_cast<std::uint64_t>( rhs ) );
	}

	template<typename Unsigned>
	constexpr UInt64Buffer operator|( Unsigned lhs, UInt64Buffer rhs ) noexcept {
		static_assert( std::is_unsigned_v<Unsigned> );
		return static_cast<UInt64Buffer>( static_cast<std::uint64_t>( lhs ) |
		                                  static_cast<std::uint64_t>( rhs ) );
	}

	template<typename Unsigned>
	constexpr UInt64Buffer operator&( UInt64Buffer lhs, Unsigned rhs ) noexcept {
		static_assert( std::is_unsigned_v<Unsigned> );
		return static_cast<UInt64Buffer>( static_cast<std::uint64_t>( lhs ) &
		                                  static_cast<std::uint64_t>( rhs ) );
	}

	template<typename Unsigned>
	constexpr UInt64Buffer operator&( Unsigned lhs, UInt64Buffer rhs ) noexcept {
		static_assert( std::is_unsigned_v<Unsigned> );
		return static_cast<UInt64Buffer>( static_cast<std::uint64_t>( lhs ) &
		                                  static_cast<std::uint64_t>( rhs ) );
	}

	template<typename Unsigned>
	constexpr UInt64Buffer operator^( UInt64Buffer lhs, Unsigned rhs ) noexcept {
		static_assert( std::is_unsigned_v<Unsigned> );
		return static_cast<UInt64Buffer>( static_cast<std::uint64_t>( lhs ) ^
		                                  static_cast<std::uint64_t>( rhs ) );
	}

	template<typename Unsigned>
	constexpr UInt64Buffer operator^( Unsigned lhs, UInt64Buffer rhs ) noexcept {
		static_assert( std::is_unsigned_v<Unsigned> );
		return static_cast<UInt64Buffer>( static_cast<std::uint64_t>( lhs ) ^
		                                  static_cast<std::uint64_t>( rhs ) );
	}

	template<typename Unsigned>
	constexpr bool operator==( UInt64Buffer lhs, Unsigned rhs ) noexcept {
		static_assert( std::is_unsigned_v<Unsigned> );
		return static_cast<std::uint64_t>( lhs ) ==
		       static_cast<std::uint64_t>( rhs );
	}

	template<typename Unsigned>
	constexpr bool operator==( Unsigned lhs, UInt64Buffer rhs ) noexcept {
		static_assert( std::is_unsigned_v<Unsigned> );
		return static_cast<std::uint64_t>( lhs ) ==
		       static_cast<std::uint64_t>( rhs );
	}

	template<typename Unsigned>
	constexpr bool operator!=( UInt64Buffer lhs, Unsigned rhs ) noexcept {
		static_assert( std::is_unsigned_v<Unsigned> );
		return static_cast<std::uint64_t>( lhs ) !=
		       static_cast<std::uint64_t>( rhs );
	}

	template<typename Unsigned>
	constexpr bool operator!=( Unsigned lhs, UInt64Buffer rhs ) noexcept {
		static_assert( std::is_unsigned_v<Unsigned> );
		return static_cast<std::uint64_t>( lhs ) !=
		       static_cast<std::uint64_t>( rhs );
	}

	template<typename Unsigned>
	constexpr UInt32Buffer &operator<<=( UInt32Buffer &b,
	                                     Unsigned shift ) noexcept {
		static_assert( std::is_unsigned_v<Unsigned> );
		auto tmp = static_cast<std::uint32_t>( b );
		tmp <<= shift;
		b = static_cast<UInt32Buffer>( tmp );
		return b;
	}

	template<typename Unsigned>
	constexpr UInt32Buffer &operator>>=( UInt32Buffer &b,
	                                     Unsigned shift ) noexcept {
		static_assert( std::is_unsigned_v<Unsigned> );
		auto tmp = static_cast<std::uint32_t>( b );
		tmp >>= shift;
		b = static_cast<UInt32Buffer>( tmp );
		return b;
	}

	template<typename Unsigned>
	constexpr UInt32Buffer operator<<( UInt32Buffer b, unsigned shift ) noexcept {
		static_assert( std::is_unsigned_v<Unsigned> );
		auto tmp = static_cast<std::uint32_t>( b );
		tmp <<= shift;
		return static_cast<UInt32Buffer>( tmp );
	}

	template<typename Unsigned>
	constexpr UInt32Buffer operator>>( UInt32Buffer b, Unsigned shift ) noexcept {
		static_assert( std::is_unsigned_v<Unsigned> );
		auto tmp = static_cast<std::uint32_t>( b );
		tmp >>= shift;
		return static_cast<UInt32Buffer>( tmp );
	}

	template<typename Unsigned>
	constexpr UInt32Buffer &operator|=( UInt32Buffer &lhs,
	                                    Unsigned rhs ) noexcept {
		static_assert( std::is_unsigned_v<Unsigned> );
		auto tmp = static_cast<std::uint32_t>( lhs );
		tmp |= static_cast<std::uint32_t>( rhs );
		lhs = static_cast<UInt32Buffer>( tmp );
		return lhs;
	}

	template<typename Unsigned>
	constexpr UInt32Buffer &operator&=( UInt32Buffer &lhs,
	                                    Unsigned rhs ) noexcept {
		static_assert( std::is_unsigned_v<Unsigned> );
		auto tmp = static_cast<std::uint32_t>( lhs );
		tmp &= static_cast<std::uint32_t>( rhs );
		lhs = static_cast<UInt32Buffer>( tmp );
		return lhs;
	}

	template<typename Unsigned>
	constexpr UInt32Buffer &operator^=( UInt32Buffer &lhs,
	                                    Unsigned rhs ) noexcept {
		static_assert( std::is_unsigned_v<Unsigned> );
		auto tmp = static_cast<std::uint32_t>( lhs );
		tmp ^= static_cast<std::uint32_t>( rhs );
		lhs = static_cast<UInt32Buffer>( tmp );
		return lhs;
	}

	template<typename Unsigned>
	constexpr UInt32Buffer operator|( UInt32Buffer lhs, Unsigned rhs ) noexcept {
		static_assert( std::is_unsigned_v<Unsigned> );
		return static_cast<UInt32Buffer>( static_cast<std::uint32_t>( lhs ) |
		                                  static_cast<std::uint32_t>( rhs ) );
	}

	template<typename Unsigned>
	constexpr UInt32Buffer operator|( Unsigned lhs, UInt32Buffer rhs ) noexcept {
		static_assert( std::is_unsigned_v<Unsigned> );
		return static_cast<UInt32Buffer>( static_cast<std::uint32_t>( lhs ) |
		                                  static_cast<std::uint32_t>( rhs ) );
	}

	template<typename Unsigned>
	constexpr UInt32Buffer operator&( UInt32Buffer lhs, Unsigned rhs ) noexcept {
		static_assert( std::is_unsigned_v<Unsigned> );
		return static_cast<UInt32Buffer>( static_cast<std::uint32_t>( lhs ) &
		                                  static_cast<std::uint32_t>( rhs ) );
	}

	template<typename Unsigned>
	constexpr UInt32Buffer operator&( Unsigned lhs, UInt32Buffer rhs ) noexcept {
		static_assert( std::is_unsigned_v<Unsigned> );
		return static_cast<UInt32Buffer>( static_cast<std::uint32_t>( lhs ) &
		                                  static_cast<std::uint32_t>( rhs ) );
	}

	template<typename Unsigned>
	constexpr UInt32Buffer operator^( UInt32Buffer lhs, Unsigned rhs ) noexcept {
		static_assert( std::is_unsigned_v<Unsigned> );
		return static_cast<UInt32Buffer>( static_cast<std::uint32_t>( lhs ) ^
		                                  static_cast<std::uint32_t>( rhs ) );
	}

	template<typename Unsigned>
	constexpr UInt32Buffer operator^( Unsigned lhs, UInt32Buffer rhs ) noexcept {
		static_assert( std::is_unsigned_v<Unsigned> );
		return static_cast<UInt32Buffer>( static_cast<std::uint32_t>( lhs ) ^
		                                  static_cast<std::uint32_t>( rhs ) );
	}

	template<typename Unsigned>
	constexpr bool operator==( UInt32Buffer lhs, Unsigned rhs ) noexcept {
		static_assert( std::is_unsigned_v<Unsigned> );
		return static_cast<std::uint32_t>( lhs ) ==
		       static_cast<std::uint32_t>( rhs );
	}

	template<typename Unsigned>
	constexpr bool operator==( Unsigned lhs, UInt32Buffer rhs ) noexcept {
		static_assert( std::is_unsigned_v<Unsigned> );
		return static_cast<std::uint32_t>( lhs ) ==
		       static_cast<std::uint32_t>( rhs );
	}

	template<typename Unsigned>
	constexpr bool operator!=( UInt32Buffer lhs, Unsigned rhs ) noexcept {
		static_assert( std::is_unsigned_v<Unsigned> );
		return static_cast<std::uint32_t>( lhs ) !=
		       static_cast<std::uint32_t>( rhs );
	}

	template<typename Unsigned>
	constexpr bool operator!=( Unsigned lhs, UInt32Buffer rhs ) noexcept {
		static_assert( std::is_unsigned_v<Unsigned> );
		return static_cast<std::uint32_t>( lhs ) !=
		       static_cast<std::uint32_t>( rhs );
	}

	template<typename Unsigned>
	constexpr UInt16Buffer &operator<<=( UInt16Buffer &b,
	                                     Unsigned shift ) noexcept {
		static_assert( std::is_unsigned_v<Unsigned> );
		auto tmp = static_cast<std::uint16_t>( b );
		tmp <<= shift;
		b = static_cast<UInt16Buffer>( tmp );
		return b;
	}

	template<typename Unsigned>
	constexpr UInt16Buffer &operator>>=( UInt16Buffer &b,
	                                     Unsigned shift ) noexcept {
		static_assert( std::is_unsigned_v<Unsigned> );
		auto tmp = static_cast<std::uint16_t>( b );
		tmp >>= shift;
		b = static_cast<UInt16Buffer>( tmp );
		return b;
	}

	template<typename Unsigned>
	constexpr UInt16Buffer operator<<( UInt16Buffer b, unsigned shift ) noexcept {
		static_assert( std::is_unsigned_v<Unsigned> );
		auto tmp = static_cast<std::uint16_t>( b );
		tmp <<= shift;
		return static_cast<UInt16Buffer>( tmp );
	}

	template<typename Unsigned>
	constexpr UInt16Buffer operator>>( UInt16Buffer b, Unsigned shift ) noexcept {
		static_assert( std::is_unsigned_v<Unsigned> );
		auto tmp = static_cast<std::uint16_t>( b );
		tmp >>= shift;
		return static_cast<UInt16Buffer>( tmp );
	}

	template<typename Unsigned>
	constexpr UInt16Buffer &operator|=( UInt16Buffer &lhs,
	                                    Unsigned rhs ) noexcept {
		static_assert( std::is_unsigned_v<Unsigned> );
		auto tmp = static_cast<std::uint16_t>( lhs );
		tmp |= static_cast<std::uint16_t>( rhs );
		lhs = static_cast<UInt16Buffer>( tmp );
		return lhs;
	}

	template<typename Unsigned>
	constexpr UInt16Buffer &operator&=( UInt16Buffer &lhs,
	                                    Unsigned rhs ) noexcept {
		static_assert( std::is_unsigned_v<Unsigned> );
		auto tmp = static_cast<std::uint16_t>( lhs );
		tmp &= static_cast<std::uint16_t>( rhs );
		lhs = static_cast<UInt16Buffer>( tmp );
		return lhs;
	}

	template<typename Unsigned>
	constexpr UInt16Buffer &operator^=( UInt16Buffer &lhs,
	                                    Unsigned rhs ) noexcept {
		static_assert( std::is_unsigned_v<Unsigned> );
		auto tmp = static_cast<std::uint16_t>( lhs );
		tmp ^= static_cast<std::uint16_t>( rhs );
		lhs = static_cast<UInt16Buffer>( tmp );
		return lhs;
	}

	template<typename Unsigned>
	constexpr UInt16Buffer operator|( UInt16Buffer lhs, Unsigned rhs ) noexcept {
		static_assert( std::is_unsigned_v<Unsigned> );
		return static_cast<UInt16Buffer>( static_cast<std::uint16_t>( lhs ) |
		                                  static_cast<std::uint16_t>( rhs ) );
	}

	template<typename Unsigned>
	constexpr UInt16Buffer operator|( Unsigned lhs, UInt16Buffer rhs ) noexcept {
		static_assert( std::is_unsigned_v<Unsigned> );
		return static_cast<UInt16Buffer>( static_cast<std::uint16_t>( lhs ) |
		                                  static_cast<std::uint16_t>( rhs ) );
	}

	template<typename Unsigned>
	constexpr UInt16Buffer operator&( UInt16Buffer lhs, Unsigned rhs ) noexcept {
		static_assert( std::is_unsigned_v<Unsigned> );
		return static_cast<UInt16Buffer>( static_cast<std::uint16_t>( lhs ) &
		                                  static_cast<std::uint16_t>( rhs ) );
	}

	template<typename Unsigned>
	constexpr UInt16Buffer operator&( Unsigned lhs, UInt16Buffer rhs ) noexcept {
		static_assert( std::is_unsigned_v<Unsigned> );
		return static_cast<UInt16Buffer>( static_cast<std::uint16_t>( lhs ) &
		                                  static_cast<std::uint16_t>( rhs ) );
	}

	template<typename Unsigned>
	constexpr UInt16Buffer operator^( UInt16Buffer lhs, Unsigned rhs ) noexcept {
		static_assert( std::is_unsigned_v<Unsigned> );
		return static_cast<UInt16Buffer>( static_cast<std::uint16_t>( lhs ) ^
		                                  static_cast<std::uint16_t>( rhs ) );
	}

	template<typename Unsigned>
	constexpr UInt16Buffer operator^( Unsigned lhs, UInt16Buffer rhs ) noexcept {
		static_assert( std::is_unsigned_v<Unsigned> );
		return static_cast<UInt16Buffer>( static_cast<std::uint16_t>( lhs ) ^
		                                  static_cast<std::uint16_t>( rhs ) );
	}

	template<typename Unsigned>
	constexpr bool operator==( UInt16Buffer lhs, Unsigned rhs ) noexcept {
		static_assert( std::is_unsigned_v<Unsigned> );
		return static_cast<std::uint16_t>( lhs ) ==
		       static_cast<std::uint16_t>( rhs );
	}

	template<typename Unsigned>
	constexpr bool operator==( Unsigned lhs, UInt16Buffer rhs ) noexcept {
		static_assert( std::is_unsigned_v<Unsigned> );
		return static_cast<std::uint16_t>( lhs ) ==
		       static_cast<std::uint16_t>( rhs );
	}

	template<typename Unsigned>
	constexpr bool operator!=( UInt16Buffer lhs, Unsigned rhs ) noexcept {
		static_assert( std::is_unsigned_v<Unsigned> );
		return static_cast<std::uint16_t>( lhs ) !=
		       static_cast<std::uint16_t>( rhs );
	}

	template<typename Unsigned>
	constexpr bool operator!=( Unsigned lhs, UInt16Buffer rhs ) noexcept {
		static_assert( std::is_unsigned_v<Unsigned> );
		return static_cast<std::uint16_t>( lhs ) !=
		       static_cast<std::uint16_t>( rhs );
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
