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
#ifndef NOSTRING
#include <string>
#endif
#include <type_traits>
#include <utility>

#include "cpp_17.h"
#include "daw_move.h"
#include "daw_traits.h"

namespace daw {
	template<size_t HashBytes>
	struct generic_hash_t;

	template<>
	struct generic_hash_t<2> {
		using hash_value_t = uint16_t;
		static constexpr size_t const hash_size = 2;
		static constexpr hash_value_t hash_init = 0;

	private:
		template<typename Result, typename T>
		static constexpr Result extract_value( T value,
		                                       size_t rshft_bits = 0 ) noexcept {
			static_assert( sizeof( Result ) <= sizeof( T ),
			               "result size must be <= to source size" );
			auto const mask = static_cast<T>( ~( Result{} & 0 ) ) << rshft_bits;
			value &= mask;
			value >>= rshft_bits;
			return static_cast<Result>( value );
		}

		template<typename Value>
		static constexpr hash_value_t
		append_hash_block( hash_value_t current_hash, Value value,
		                   size_t count = sizeof( Value ) ) noexcept {
			hash_value_t const JODY_HASH_CONSTANT = 0x1F5B;
			hash_value_t const JODY_HASH_SHIFT = 14;

			auto len = count / sizeof( hash_value_t );
			for( ; len > 0; --len ) {
				auto const element =
				  extract_value<hash_value_t>( value, len * sizeof( hash_value_t ) );
				current_hash += element;
				current_hash += JODY_HASH_CONSTANT;
				current_hash =
				  static_cast<hash_value_t>( current_hash << JODY_HASH_SHIFT ) |
				  current_hash >> ( sizeof( hash_value_t ) * 8 - JODY_HASH_SHIFT );
				current_hash ^= element;
				current_hash =
				  static_cast<hash_value_t>( current_hash << JODY_HASH_SHIFT ) |
				  current_hash >> ( sizeof( hash_value_t ) * 8 - JODY_HASH_SHIFT );
				current_hash ^= JODY_HASH_CONSTANT;
				current_hash += element;
			}
			return current_hash;
		}

	public:
		template<typename Value, std::enable_if_t<std::is_integral_v<Value>,
		                                          std::nullptr_t> = nullptr>
		static constexpr hash_value_t append_hash( hash_value_t current_hash,
		                                           Value const & ) noexcept {
			// TODO	static_assert( sizeof( Value ) >= sizeof( hash_value_t ), "Value
			// must be at least 2 bytes" );
			// TODO
			return current_hash;
		}
	};

	template<>
	struct generic_hash_t<4> {
		using hash_value_t = uint32_t;
		static constexpr size_t const hash_size = 4;
		static constexpr hash_value_t const hash_init = 2166136261UL;

		template<typename Value, std::enable_if_t<std::is_integral_v<Value>,
		                                          std::nullptr_t> = nullptr>
		static constexpr hash_value_t append_hash( hash_value_t current_hash,
		                                           Value const &value ) noexcept {
			hash_value_t const fnv1a_prime = 16777619UL;
			for( size_t n = 0; n < sizeof( Value ); ++n ) {
				current_hash ^= static_cast<hash_value_t>(
				  ( static_cast<uintmax_t>( value ) &
				    ( static_cast<uintmax_t>( 0xFF ) << ( n * 8u ) ) ) >>
				  ( n * 8u ) );
				current_hash *= fnv1a_prime;
			}
			return current_hash;
		}

		template<typename Iterator1, typename Iterator2,
		         std::enable_if_t<std::is_integral_v<
		                            typename std::iterator_traits<Iterator1>::type>,
		                          std::nullptr_t> = nullptr>
		constexpr hash_value_t operator( )( Iterator1 first,
		                                    Iterator2 const last ) const noexcept {
			auto hash = hash_init;
			while( first != last ) {
				hash = append_hash( hash, *first );
			}
			return hash;
		}
		template<
		  typename Member, size_t N,
		  std::enable_if_t<std::is_integral_v<Member>, std::nullptr_t> = nullptr>
		constexpr hash_value_t operator( )( Member const ( &member )[N] ) const
		  noexcept {
			return operator( )( member,
			                    std::next( member, static_cast<intmax_t>( N ) ) );
		}

		template<typename Integral, std::enable_if_t<std::is_integral_v<Integral>,
		                                             std::nullptr_t> = nullptr>
		constexpr hash_value_t operator( )( Integral const value ) const noexcept {
			return append_hash( hash_init, value );
		}
	};
	template<>
	struct generic_hash_t<8> {
		using hash_value_t = uint64_t;
		static constexpr size_t const hash_size = 8;
		static constexpr hash_value_t const hash_init = 14695981039346656037ULL;

		template<typename Value, std::enable_if_t<std::is_integral_v<Value>,
		                                          std::nullptr_t> = nullptr>
		static constexpr hash_value_t append_hash( hash_value_t current_hash,
		                                           Value const &value ) noexcept {
			hash_value_t const fnv1a_prime = 1099511628211ULL;
			for( size_t n = 0; n < sizeof( Value ); ++n ) {
				current_hash ^= static_cast<hash_value_t>(
				  ( static_cast<uintmax_t>( value ) &
				    ( static_cast<uintmax_t>( 0xFF ) << ( n * 8u ) ) ) >>
				  ( n * 8u ) );
				current_hash *= fnv1a_prime;
			}
			return current_hash;
		}

		template<typename Iterator1, typename Iterator2,
		         std::enable_if_t<std::is_integral_v<
		                            typename std::iterator_traits<Iterator1>::type>,
		                          std::nullptr_t> = nullptr>
		constexpr hash_value_t operator( )( Iterator1 first,
		                                    Iterator2 const last ) const noexcept {
			auto hash = hash_init;
			while( first != last ) {
				hash = append_hash( hash, *first );
			}
			return hash;
		}
		template<
		  typename Member, size_t N,
		  std::enable_if_t<std::is_integral_v<Member>, std::nullptr_t> = nullptr>
		constexpr hash_value_t operator( )( Member const ( &member )[N] ) const
		  noexcept {
			return operator( )( member,
			                    std::next( member, static_cast<intmax_t>( N ) ) );
		}

		template<typename Integral, std::enable_if_t<std::is_integral_v<Integral>,
		                                             std::nullptr_t> = nullptr>
		constexpr hash_value_t operator( )( Integral const value ) const noexcept {
			return append_hash( hash_init, value );
		}

		template<typename T>
		constexpr hash_value_t operator( )( T const *const ptr ) const noexcept {
			auto hash = hash_init;
			auto bptr = static_cast<uint8_t const *const>( ptr );
			for( size_t n = 0; n < sizeof( T ); ++n ) {
				append_hash( hash, bptr[n] );
			}
			return hash;
		}
	};

	template<size_t HashBytes = sizeof( size_t ), typename T,
	         std::enable_if_t<std::is_integral_v<T>, std::nullptr_t> = nullptr>
	constexpr auto generic_hash( T const value ) noexcept {
		return generic_hash_t<HashBytes>{}( value );
	}

	template<size_t HashBytes = sizeof( size_t ), typename Iterator,
	         typename IteratorL>
	constexpr auto generic_hash( Iterator first, IteratorL const last ) noexcept {
		using hash_t = generic_hash_t<HashBytes>;
		auto hash = hash_t::hash_init;
		while( first != last ) {
			hash = hash_t::append_hash( hash, *first );
			++first;
		}
		return hash;
	}

	template<size_t HashBytes = sizeof( size_t ), typename Iterator>
	constexpr auto generic_hash( Iterator first, size_t const len ) noexcept {
		using hash_t = generic_hash_t<HashBytes>;
		auto hash = hash_t::hash_init;
		for( size_t n = 0; n < len; ++n ) {
			hash = hash_t::append_hash( hash, *first );
			++first;
		}
		return hash;
	}

#ifndef NOSTRING
	template<size_t HashBytes = sizeof( size_t ), typename CharT, typename Traits,
	         typename Allocator>
	auto generic_hash(
	  std::basic_string<CharT, Traits, Allocator> const &str ) noexcept {
		return generic_hash<HashBytes>( str.data( ), str.size( ) );
	}

	template<size_t HashBytes = sizeof( size_t ), typename CharT, typename Traits,
	         typename Allocator>
	auto
	generic_hash( std::basic_string<CharT, Traits, Allocator> &&str ) noexcept {
		auto tmp = daw::move( str );
		return generic_hash<HashBytes>( tmp.data( ), tmp.size( ) );
	}
#endif
	template<size_t HashBytes = sizeof( size_t ), size_t N>
	constexpr auto generic_hash( char const ( &ptr )[N] ) noexcept {
		return generic_hash<HashBytes>( ptr, N );
	}
} // namespace daw
