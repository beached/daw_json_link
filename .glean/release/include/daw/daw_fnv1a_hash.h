// The MIT License (MIT)
//
// Copyright (c) 2016-2020 Darrell Wright
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
#include <cstddef>
#include <cstdint>
#include <iterator>
#ifndef NOSTRING
#include <string>
#endif
#include <type_traits>

#include "daw_string_view_fwd.h"
#include "daw_traits.h"

namespace daw {
	namespace impl {
		constexpr bool is_64bit_v = sizeof( size_t ) == sizeof( uint64_t );

		[[nodiscard]] constexpr size_t fnv_prime( ) noexcept {
			return is_64bit_v ? 1099511628211ULL : 16777619UL;
		}

		[[nodiscard]] constexpr size_t fnv_offset( ) noexcept {
			return is_64bit_v ? 14695981039346656037ULL : 2166136261UL;
		}

	} // namespace impl

	struct fnv1a_hash_t {
		// TODO: check for UB if values are signed
		template<typename Value, std::enable_if_t<std::is_integral_v<Value>,
		                                          std::nullptr_t> = nullptr>
		[[nodiscard]] static constexpr size_t
		append_hash( size_t current_hash, Value const &value ) noexcept {
			for( size_t n = 0; n < sizeof( Value ); ++n ) {
				current_hash ^= static_cast<size_t>(
				  ( static_cast<size_t>( value ) &
				    ( static_cast<size_t>( 0xFF ) << ( n * 8u ) ) ) >>
				  ( n * 8u ) );
				current_hash *= impl::fnv_prime( );
			}
			return current_hash;
		}

		template<typename Iterator1, typename Iterator2,
		         std::enable_if_t<std::is_integral_v<
		                            typename std::iterator_traits<Iterator1>::type>,
		                          std::nullptr_t> = nullptr>
		[[nodiscard]] constexpr size_t
		operator( )( Iterator1 first, Iterator2 const last ) const noexcept {
			auto hash = impl::fnv_offset( );
			while( first != last ) {
				hash = append_hash( hash, *first );
				++first;
			}
			return hash;
		}

		template<typename CharT, typename BoundsType, ptrdiff_t Extent>
		[[nodiscard]] constexpr size_t operator( )(
		  daw::basic_string_view<CharT, BoundsType, Extent> sv ) const noexcept {
			auto hash = impl::fnv_offset( );
			for( char c : sv ) {
				hash = append_hash( hash, c );
			}
			return hash;
		}

		template<
		  typename Member, size_t N,
		  std::enable_if_t<std::is_integral_v<Member>, std::nullptr_t> = nullptr>
		[[nodiscard]] constexpr size_t
		operator( )( Member const ( &member )[N] ) const noexcept {
			return operator( )( member,
			                    std::next( member, static_cast<intmax_t>( N ) ) );
		}

		template<typename Integral, std::enable_if_t<std::is_integral_v<Integral>,
		                                             std::nullptr_t> = nullptr>
		[[nodiscard]] constexpr size_t
		operator( )( Integral const value ) const noexcept {
			return append_hash( impl::fnv_offset( ), value );
		}

		[[nodiscard]] constexpr size_t
		operator( )( char const *ptr ) const noexcept {
			auto hash = impl::fnv_offset( );
			while( *ptr != '\0' ) {
				hash = hash ^ static_cast<size_t>( *ptr );
				hash *= impl::fnv_prime( );
				++ptr;
			}
			return hash;
		}

		template<typename T>
		[[nodiscard]] constexpr size_t
		operator( )( T const *const ptr ) const noexcept {
			auto hash = impl::fnv_offset( );
			auto bptr = static_cast<uint8_t const *const>( ptr );
			for( size_t n = 0; n < sizeof( T ); ++n ) {
				hash = hash ^ static_cast<size_t>( bptr[n] );
				hash *= impl::fnv_prime( );
			}
			return hash;
		}
	};

	template<typename T,
	         std::enable_if_t<std::is_integral_v<T>, std::nullptr_t> = nullptr>
	[[nodiscard]] constexpr size_t fnv1a_hash( T const value ) noexcept {
		return fnv1a_hash_t{ }( value );
	}

	[[nodiscard]] constexpr size_t fnv1a_hash( char const *ptr ) noexcept {
		auto hash = impl::fnv_offset( );
		while( *ptr != 0 ) {
			hash = fnv1a_hash_t::append_hash( hash, *ptr );
			++ptr;
		}
		return hash;
	}

	template<typename CharT>
	[[nodiscard]] constexpr size_t fnv1a_hash( CharT const *ptr,
	                                           size_t const len ) noexcept {
		auto hash = impl::fnv_offset( );
		auto const last = std::next( ptr, static_cast<intmax_t>( len ) );
		while( ptr != last ) {
			hash = fnv1a_hash_t::append_hash( hash, *ptr );
			++ptr;
		}
		return hash;
	}

#ifndef NOSTRING
	template<typename CharT, typename Traits, typename Allocator>
	[[nodiscard]] size_t
	fnv1a_hash( std::basic_string<CharT, Traits, Allocator> const &str ) {
		return fnv1a_hash( str.data( ), str.size( ) );
	}

	template<typename CharT, typename Traits, typename Allocator>
	[[nodiscard]] size_t
	fnv1a_hash( std::basic_string<CharT, Traits, Allocator> &&str ) {
		return fnv1a_hash( str.data( ), str.size( ) );
	}
#endif
	template<size_t N>
	[[nodiscard]] constexpr size_t fnv1a_hash( char const ( &ptr )[N] ) noexcept {
		return fnv1a_hash( ptr, N );
	}
} // namespace daw
