// The MIT License (MIT)
//
// Copyright (c) 2019-2020 Darrell Wright
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

#include "daw/daw_bounded_array.h"
#include "daw/daw_bounded_vector.h"
#include "daw/daw_fnv1a_hash.h"
#include "daw_algorithm.h"
#include "daw_sort_n.h"
#include "daw_swap.h"

#include <cstddef>
#include <functional>
#include <limits>
#include <new>
#include <type_traits>
#include <utility>

namespace daw {
	namespace mph_impl {
		template<size_t Bits>
		constexpr auto determine_opt_sz( ) {
			constexpr size_t sz = [] {
				std::pair<uint8_t, size_t> sizes[] = {{8U, Bits % 8U},
				                                      {16U, Bits % 16U},
				                                      {32U, Bits % 32U},
				                                      {64U, Bits % 64U}};
				size_t min_pos = 0;
				daw::algorithm::do_n_arg<3>( [&]( size_t n ) {
					++n;
					if( sizes[n].second < sizes[min_pos].second ) {
						min_pos = n;
					} else if( sizes[n].second == sizes[min_pos].second and
					           sizes[n].first > sizes[min_pos].first ) {
						min_pos = n;
					}
				} );
				return sizes[min_pos].first;
			}( );
			if constexpr( sz == 8U ) {
				return uint8_t{};
			} else if constexpr( sz == 16U ) {
				return uint16_t{};
			} else if constexpr( sz == 32U ) {
				return uint32_t{};
			} else if constexpr( sz == 64U ) {
				return uint64_t{};
			};
		}

		template<size_t Bits>
		using opt_type_t =
		  std::remove_reference_t<decltype( determine_opt_sz<Bits>( ) )>;

		template<size_t Bits>
		class static_bitset {
			using values_type = opt_type_t<Bits>;
			inline static constexpr size_t m_bits_per_bin =
			  sizeof( values_type ) * 8U;
			inline static constexpr size_t m_bins = Bits / m_bits_per_bin;

			daw::array<values_type, m_bins> m_values{};

		public:
			constexpr static_bitset( ) noexcept = default;

			constexpr void set_bit( size_t idx ) noexcept {
				auto const bin = idx / m_bits_per_bin;
				auto const item = idx - ( bin * m_bits_per_bin );
				auto const mask = 1U << item;
				m_values[bin] |= mask;
			}

			constexpr bool operator[]( size_t idx ) const noexcept {
				auto const bin = idx / m_bits_per_bin;
				auto const item = idx - ( bin * m_bits_per_bin );
				auto const mask = ~( 1U << item );
				return ( m_values[bin] & mask ) != 0;
			}
		};
		template<size_t N>
		[[nodiscard]] constexpr size_t next_pow2( ) {
			size_t ret = 1U;

			for( size_t sz = N; sz > 0; sz >>= 1U ) {
				ret <<= 1U;
			}
			return ret;
		}

		template<size_t N>
		[[nodiscard]] constexpr size_t get_bucket_count( ) {
			static_assert( N > 0 );
			return next_pow2<N>( ) >> 1U;
		}

		template<typename hash_result>
		[[nodiscard]] static constexpr hash_result
		hash_combine( hash_result seed, hash_result hash ) noexcept {

			uintmax_t const s = seed + 1;
			uintmax_t const h = hash;
			return static_cast<hash_result>(
			  s ^ ( h + 0x9e3779b9ULL + ( s << 6ULL ) + ( s >> 2ULL ) ) );
		}
	} // namespace mph_impl

	//*********************************************************************
	// Minimal Perfect Hash Table
	// Adapted from
	// https://blog.demofox.org/2015/12/14/o1-data-lookups-with-minimal-perfect-hashing/
	// and https://blog.gopheracademy.com/advent-2017/mphf/
	template<size_t N, typename Key, typename Value,
	         typename Hasher = std::hash<Key>,
	         typename KeyEqual = std::equal_to<>>
	struct perfect_hash_table {
		static_assert( std::is_default_constructible_v<Key> );
		static_assert( std::is_default_constructible_v<Value> );
		static_assert( std::is_trivially_copyable_v<Key> );
		static_assert( std::is_trivially_copyable_v<Value> );
		using key_type = Key;
		using mapped_type = Value;
		using size_type = std::size_t;
		using difference_type = std::ptrdiff_t;
		using reference = std::pair<key_type const &, mapped_type &>;
		using const_reference = std::pair<key_type const &, mapped_type const &>;
		using key_equal = KeyEqual;
		using key_type_pointer = key_type const *;
		using mapped_type_pointer = mapped_type *;
		using const_mapped_type_pointer = mapped_type const *;
		using hash_result = daw::remove_cvref_t<std::invoke_result_t<Hasher, Key>>;
		using salt_type = intmax_t;
		static size_t constexpr m_data_size = mph_impl::next_pow2<N>( );
		static_assert( m_data_size <= static_cast<size_t>(
		                                std::numeric_limits<salt_type>::max( ) ) );

		/***
		 * Construct a perfect_hash_table from a range of pair like items that have
		 * the key in their first member and the value in their second member
		 *
		 * @tparam ForwardIterator points to a range of items with a std::pair like
		 * type
		 * @param first Start of range, inclusive
		 * @param last End of range, exclusive
		 */
		template<typename ForwardIterator>
		constexpr perfect_hash_table( ForwardIterator first,
		                              ForwardIterator last ) {
			daw::array<bucket_t<ForwardIterator>, m_num_buckets> buckets{};

			static_assert( std::is_default_constructible_v<ForwardIterator> );

			daw::algorithm::iota(
			  buckets.begin( ), buckets.end( ), static_cast<size_type>( 0 ),
			  []( auto &i, size_type num ) { i.bucket_index = num; } );

			for( auto it = first; it != last; ++it ) {
				auto const bucket = scale_hash<m_num_buckets>( call_hash( it->first ) );
				buckets[bucket].items.push_back( it );
			}

			daw::sort( buckets.begin( ), buckets.end( ),
			           []( auto const &lhs, auto const &rhs ) {
				           return lhs.items.size( ) > rhs.items.size( );
			           } );

			daw::array<bool, m_data_size> slots_claimed{};

			daw::algorithm::find_if( buckets.cbegin( ), buckets.cend( ),
			                         [&]( auto const &bucket ) {
				                         if( bucket.items.empty( ) ) {
					                         return true;
				                         }
				                         find_salt_for_bucket( bucket, slots_claimed );
				                         return false;
			                         } );
		}

		explicit constexpr perfect_hash_table(
		  std::pair<Key, Value> const ( &data )[N] )
		  : perfect_hash_table( data, data + static_cast<ptrdiff_t>( N ) ) {}

		[[nodiscard]] constexpr const_mapped_type_pointer
		find( Key const &key ) const {
			size_type const pos = find_data_index( key );
			if( key_equal{}( key, m_keys[pos] ) ) {
				return &m_values[pos];
			}
			return nullptr;
		}

		[[nodiscard]] constexpr size_type size( ) const noexcept {
			return N;
		}

		[[nodiscard]] constexpr bool contains( Key const &key ) const noexcept {
			size_type const pos = find_data_index( key );
			return key_equal{}( key, m_keys[pos] );
		}

		[[nodiscard]] constexpr mapped_type_pointer find( Key const &key ) {
			size_type const pos = find_data_index( key );
			if( key_equal{}( key, m_keys[pos] ) ) {
				return &m_values[pos];
			}
			return nullptr;
		}

		[[nodiscard]] constexpr mapped_type &operator[]( Key const &key ) {
			auto const idx = find_data_index( key );
			return m_values[idx];
		}

		[[nodiscard]] constexpr mapped_type const &
		operator[]( Key const &key ) const {
			auto const idx = find_data_index( key );
			return m_values[idx];
		}

	private:
		template<typename ForwardIterator>
		struct bucket_t {
			size_type bucket_index = 0;
			daw::bounded_vector_t<ForwardIterator, m_data_size> items{};

			constexpr void swap( bucket_t &rhs ) {
				daw::cswap( bucket_index, rhs.bucket_index );
				daw::cswap( items, rhs.items );
			}
		};

		static constexpr size_type m_num_buckets = mph_impl::get_bucket_count<N>( );

		//***************
		daw::array<salt_type, m_num_buckets> m_salts{};
		daw::array<mapped_type, m_data_size> m_values{};
		daw::array<key_type, m_data_size> m_keys{};
		//***************

		template<size_type Space = m_data_size>
		[[nodiscard]] static constexpr size_type
		scale_hash( hash_result hash ) noexcept {
			return static_cast<hash_result>( hash % Space );
		}

		[[nodiscard]] static constexpr hash_result call_hash( Key const &key ) {
			return Hasher{}( key );
		}

		[[nodiscard]] static constexpr hash_result call_hash( Key const &key,
		                                                      salt_type seed ) {
			return mph_impl::hash_combine( static_cast<hash_result>( seed ),
			                               Hasher{}( key ) );
		}

		template<typename ForwardIterator>
		constexpr void
		find_salt_for_bucket( bucket_t<ForwardIterator> const &bucket,
		                      daw::array<bool, m_data_size> &slots_claimed ) {
			if( bucket.items.size( ) == 1 ) {
				auto const pos = daw::algorithm::find_index_of(
				  slots_claimed.cbegin( ), slots_claimed.cend( ), false );
				if( pos == slots_claimed.size( ) ) {
					// Should never happen, there should always be at least one unclaimed
					// slot
					std::abort( );
				}
				slots_claimed[pos] = true;
				m_salts[bucket.bucket_index] = -( static_cast<salt_type>( pos ) + 1 );
				m_keys[pos] = bucket.items[0]->first;
				m_values[pos] = bucket.items[0]->second;
			}

			for( salt_type salt = 1;; ++salt ) {
				daw::bounded_vector_t<hash_result, m_data_size> slots_this_bucket{};

				auto const success = daw::algorithm::all_of(
				  bucket.items.begin( ), bucket.items.end( ),
				  [&]( auto const *const item ) -> bool {
					  hash_result const slot_wanted =
					    scale_hash( call_hash( item->first, salt ) );
					  if( slots_claimed[slot_wanted] or
					      ( daw::algorithm::find(
					          slots_this_bucket.begin( ), slots_this_bucket.end( ),
					          slot_wanted ) != slots_this_bucket.end( ) ) ) {
						  return false;
					  }
					  slots_this_bucket.push_back( slot_wanted );
					  return true;
				  } );

				if( success ) {
					m_salts[bucket.bucket_index] = salt;
					for( size_type i = 0; i < bucket.items.size( ); ++i ) {
						auto const pos = static_cast<size_t>( slots_this_bucket[i] );
						m_keys[pos] = bucket.items[i]->first;
						m_values[pos] = bucket.items[i]->second;
						slots_claimed[pos] = true;
					}
					return;
				}
			}
		}

		[[nodiscard]] constexpr size_type find_data_index( Key const &key ) const {
			size_type const hash = call_hash( key );
			auto const salt = m_salts[scale_hash<m_num_buckets>( hash )];

			if( salt < 0 ) {
				return static_cast<size_type>( ( -salt ) - 1 );
			}
			return scale_hash( mph_impl::hash_combine<hash_result>(
			  static_cast<hash_result>( salt ), hash ) );
		}

	}; // namespace daw

	template<typename Hasher, typename Key, typename Value, size_t N>
	constexpr auto
	make_perfect_hash_table( std::pair<Key, Value> const ( &data )[N] ) {
		return perfect_hash_table<N, Key, Value, Hasher>(
		  data, data + static_cast<ptrdiff_t>( N ) );
	}
} // namespace daw
