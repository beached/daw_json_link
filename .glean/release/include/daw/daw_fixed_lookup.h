// The MIT License (MIT)
//
// Copyright (c) 2017-2020 Darrell Wright
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

#include <stdexcept>
#include <type_traits>

#include "daw_bounded_array.h"
#include "daw_exception.h"
#include "daw_generic_hash.h"
#include "daw_move.h"
#include "daw_traits.h"
#include "daw_utility.h"

namespace daw {
	namespace impl {
		namespace sentinals {
			enum sentinals { empty = 0, removed, sentinals_size };
		} // namespace sentinals

		template<size_t ValueSize>
		struct largest_primes;

		template<>
		struct largest_primes<8> {
			static constexpr uint64_t const a = 18446744073709551557ULL;
			static constexpr uint64_t const b = 18446744073709551533ULL;
		};

		template<>
		struct largest_primes<4> {
			static constexpr uint32_t const a = 4093082899UL;
			static constexpr uint32_t const b = 3367900313UL;
		};
	} // namespace impl

	///
	/// A fixed lookup table.  Only indices returned by insert or get_existing are
	/// defined.  A reference does not exist after a copy/move operation but the
	/// indices will remain valid
	/// In some testing the average distance to actual value was 0.147 when size
	/// is double needed items
	template<typename Value, size_t N, size_t HashSize = sizeof( size_t )>
	struct fixed_lookup {
		static_assert( std::is_default_constructible_v<Value>,
		               "Value must be default constructible" );
		static_assert( N > 0, "Must supply a positive initial_size larger than 0" );

		using value_t = daw::traits::root_type_t<Value>;
		using hash_value_t = typename daw::generic_hash_t<HashSize>::hash_value_t;
		static_assert(
		  N <= std::numeric_limits<hash_value_t>::max( ),
		  "Cannot allocate more values than can be addressed by hash value" );

		using reference = value_t &;
		using const_reference = value_t const &;

	private:
		daw::bounded_array_t<hash_value_t, N> m_hashes;
		daw::bounded_array_t<value_t, N> m_values;

	public:
		static constexpr hash_value_t capacity( ) noexcept {
			return N;
		}

		constexpr hash_value_t size( ) const noexcept {
			hash_value_t count = 0;
			for( auto hash : m_hashes ) {
				count += hash >= impl::sentinals::sentinals_size ? 1 : 0;
			}
			return count;
		}

	private:
		template<typename KeyType>
		static constexpr hash_value_t hash_fn( KeyType &&key ) noexcept {
			auto const hash =
			  daw::generic_hash<HashSize>( std::forward<KeyType>( key ) );
			auto const divisor = std::numeric_limits<hash_value_t>::max( ) -
			                     impl::sentinals::sentinals_size;
			return ( hash % divisor ) + impl::sentinals::sentinals_size;
		}

		constexpr hash_value_t scale_hash( hash_value_t const hash ) const {
			// Scale value to capacity using MAD(Multiply-Add-Divide) compression
			// Use the two largest Prime's that fit in a hash_value_t
			return ( ( hash * impl::largest_primes<HashSize>::a ) +
			         impl::largest_primes<HashSize>::b ) %
			       capacity( );
		}

		constexpr auto lookup( hash_value_t const hash ) const noexcept {
			struct lookup_result_t {
				hash_value_t position;
				bool found;

				constexpr lookup_result_t( hash_value_t pos, bool is_found ) noexcept
				  : position{pos}
				  , found{is_found} {}

				constexpr operator bool( ) const noexcept {
					return found;
				}
			};

			auto const hash_pos = scale_hash( hash );
			for( hash_value_t n = hash_pos; n != m_hashes.size( ); ++n ) {
				if( hash == m_hashes[n] ) {
					return lookup_result_t{n, true};
				} else if( impl::sentinals::empty == m_hashes[n] ) {
					return lookup_result_t{n, false};
				}
			}
			for( hash_value_t n = 0; n != hash_pos; ++n ) {
				if( m_hashes[n] == hash ) {
					return lookup_result_t{n, true};
				} else if( m_hashes[n] == impl::sentinals::empty ) {
					return lookup_result_t{n, false};
				}
			}
			return lookup_result_t{m_hashes.size( ), false};
		}

	public:
		constexpr fixed_lookup( ) noexcept(
		  noexcept( std::is_nothrow_default_constructible_v<value_t> ) ) = default;

		template<typename Key>
		constexpr hash_value_t find_existing( Key &&key ) const {
			auto const hash = hash_fn( std::forward<Key>( key ) );
			auto const is_found = lookup( hash );
			daw::exception::daw_throw_on_false(
			  is_found, "Attempt to access an undefined key" );
			return is_found.position;
		}

		template<typename Key>
		constexpr hash_value_t insert( Key &&key, Value value ) {
			auto const hash = hash_fn( std::forward<Key>( key ) );
			auto const is_found = lookup( hash );
			daw::exception::daw_throw_on_true(
			  !is_found and is_found.position == m_hashes.size( ),
			  "Fixed hash table does not have enough space to allocate all entries" );
			m_hashes[is_found.position] = hash;
			m_values[is_found.position] = daw::move( value );
			return is_found.position;
		}

		constexpr reference get_existing( hash_value_t position ) {
			return m_values[position];
		}

		constexpr const_reference get_existing( hash_value_t position ) const {
			return m_values[position];
		}

		template<typename Key>
		constexpr const_reference operator[]( Key &&key ) const {
			return m_values[find_existing( std::forward<Key>( key ) )];
		}

		template<typename Key>
		constexpr reference operator[]( Key &&key ) {
			auto const hash = hash_fn( std::forward<Key>( key ) );
			auto const is_found = lookup( hash );
			daw::exception::daw_throw_on_true(
			  !is_found and is_found.position == m_hashes.size( ),
			  "Fixed hash table does not have enough space to allocate all entries" );
			m_hashes[is_found.position] = hash;
			return m_values[is_found.position];
		}

		template<typename Key>
		constexpr bool exists( Key &&key ) const noexcept {
			auto const hash = hash_fn( std::forward<Key>( key ) );
			return static_cast<bool>( lookup( hash ) );
		}
	};

	template<typename Value, size_t HashSize = sizeof( size_t ), typename... Keys>
	constexpr auto make_fixed_lookup( Keys &&... keys ) noexcept {
		fixed_lookup<Value, sizeof...( Keys )> result{};
		auto const lst = {( result[keys] = Value{}, 0 )...};
		Unused( lst );
		return result;
	}
} // namespace daw
