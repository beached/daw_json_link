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

#include <cstdint>

#include "daw_bit.h"
#include "daw_endian.h"
#include "daw_exception.h"
#include "daw_move.h"

namespace daw {
	struct bit_queue_source_little_endian {};
	struct bit_queue_source_big_endian {};

	using bit_queue_source_native_endian =
	  typename std::conditional_t<daw::endian::native == daw::endian::little,
	                              bit_queue_source_little_endian,
	                              bit_queue_source_big_endian>;

	template<typename queue_type, typename value_type = uint8_t,
	         typename BitQueueLSB = bit_queue_source_native_endian>
	class basic_bit_queue {
		static_assert( std::is_unsigned_v<queue_type> and
		                 std::is_unsigned_v<value_type>,
		               "Only unsigned integral types are supported" );
		size_t m_size = 0;
		queue_type m_queue = 0;

	public:
		constexpr basic_bit_queue( ) = default;

		constexpr explicit basic_bit_queue( queue_type v ) noexcept
		  : m_size( sizeof( m_queue ) * 8 )
		  , m_queue( daw::move( v ) ) {}

		constexpr size_t size( ) const noexcept {
			return m_size;
		}

		constexpr bool can_pop( size_t const bits ) const noexcept {
			return m_size >= bits;
		}

		constexpr bool empty( ) const noexcept {
			return 0 == m_size;
		}

		constexpr size_t capacity( ) const noexcept {
			return sizeof( m_queue ) * 8;
		}

		constexpr bool full( ) const noexcept {
			return size( ) == capacity( );
		}

	private:
		template<typename T>
		auto source_to_native_endian( T value,
		                              bit_queue_source_little_endian ) noexcept {
			return daw::to_native_endian<daw::endian::little>( value );
		}

		template<typename T>
		auto source_to_native_endian( T value,
		                              bit_queue_source_big_endian ) noexcept {
			return daw::to_native_endian<daw::endian::big>( value );
		}

	public:
		void push_back( value_type value,
		                size_t const bits = sizeof( value_type ) * 8 ) {
			daw::exception::dbg_throw_on_false(
			  ( capacity( ) - m_size ) >= bits,
			  "Not enough bits to hold value pushed" );

			value &= get_left_mask<value_type>( sizeof( value_type ) * 8 - bits );
			m_queue <<= bits;
			m_queue |= source_to_native_endian( value, BitQueueLSB{} );
			m_size += bits;
		}

		value_type pop_front( size_t const bits ) noexcept {
			queue_type const mask = static_cast<queue_type>( ~( ( 2 << bits ) - 1 ) );
			auto result = static_cast<value_type>(
			  static_cast<value_type>( m_queue & mask ) >> ( capacity( ) - bits ) );
			m_queue <<= bits;
			m_size -= bits;
			return result;
		}

		value_type pop_back( size_t const bits ) {
			daw::exception::dbg_throw_on_false( m_size >= bits,
			                                    "Not enough bits to pop request" );

			auto const mask = get_left_mask<queue_type>( capacity( ) - bits );
			auto result = static_cast<value_type>( m_queue & mask );
			m_queue >>= bits;
			m_size -= bits;
			return result;
		}

		constexpr void clear( ) noexcept {
			m_queue = 0;
			m_size = 0;
		}

		constexpr value_type pop_all( ) noexcept {
			auto result = static_cast<value_type>( m_queue );
			clear( );
			return result;
		}

		constexpr queue_type const &value( ) const noexcept {
			return m_queue;
		}
	}; // basic_bit_queue

	using bit_queue = basic_bit_queue<uint16_t>;

	template<typename queue_type, typename value_type = uint8_t>
	class basic_nibble_queue {
		basic_bit_queue<queue_type, value_type> m_queue;

	public:
		constexpr basic_nibble_queue( ) noexcept
		  : m_queue{} {}

		constexpr explicit basic_nibble_queue( queue_type v ) noexcept
		  : m_queue{daw::move( v )} {}

		constexpr size_t capacity( ) const noexcept {
			return m_queue.capacity( ) / 4;
		}

		constexpr size_t size( ) const noexcept {
			return m_queue.size( ) / 4;
		}

		constexpr bool empty( ) const noexcept {
			return 0 == size( );
		}

		void push_back( value_type const &value ) {
			m_queue.push_back( value, 4 );
		}

		void push_back( value_type const &value, size_t const &num_nibbles ) {
			m_queue.push_back( value, num_nibbles * 4 );
		}

		constexpr bool can_pop( size_t num_nibbles = sizeof( value_type ) *
		                                             2 ) const noexcept {
			return m_queue.can_pop( num_nibbles * 4 );
		}

		constexpr bool full( ) const noexcept {
			return size( ) == capacity( );
		}

		value_type pop_front( size_t num_nibbles = sizeof( value_type ) *
		                                           2 ) noexcept {
			return m_queue.pop_front( num_nibbles * 4 );
		}

		constexpr void clear( ) noexcept {
			m_queue.clear( );
		}

		value_type pop_all( ) noexcept {
			return m_queue.pop_all( );
		}

		constexpr queue_type const &value( ) const noexcept {
			return m_queue.value( );
		}
	}; // basic_nibble_queue

	using nibble_queue = basic_nibble_queue<uint8_t>;
} // namespace daw
