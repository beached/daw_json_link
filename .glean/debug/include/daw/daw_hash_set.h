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

#include <functional>
#include <optional>
#include <vector>

#include "daw_algorithm.h"

namespace daw {
	template<typename Key, typename Hash = std::hash<Key>>
	class hash_set_t {
		std::vector<std::optional<Key>> m_indices;

		static constexpr size_t scale_hash( size_t hash, size_t range_size ) {
			size_t const prime_a = 18446744073709551557u;
			size_t const prime_b = 18446744073709551533u;
			return ( hash * prime_a + prime_b ) % range_size;
		}

		std::optional<size_t> find_index( size_t hash, Key const &key ) const {
			size_t const scaled_hash = scale_hash( hash, m_indices.size( ) );

			for( size_t n = scaled_hash; n < m_indices.size( ); ++n ) {
				if( !m_indices[n] ) {
					return n;
				}
				if( *m_indices[n] == key ) {
					return n;
				}
			}
			for( size_t n = 0; n < scaled_hash; ++n ) {
				if( !m_indices[n] ) {
					return n;
				}
				if( *m_indices[n] == key ) {
					return n;
				}
			}
			return {};
		}

	public:
		hash_set_t( size_t range_size ) noexcept
		  : m_indices( range_size, std::nullopt ) {}

		size_t insert( Key const &key ) {
			auto const hash = Hash{}( key );
			auto const index = find_index( hash, key );
			if( !index ) {
				throw std::out_of_range( "Hash table is full" );
			}
			m_indices[*index] = key;
			return *index;
		}

		std::optional<size_t> erase( Key const &key ) {
			auto const hash = Hash{}( key );
			auto const index = find_index( hash, key );
			if( !index ) {
				return {};
			}
			m_indices[index] = std::nullopt;
			return index;
		}

		bool exists( Key const &key ) const noexcept {
			auto const hash = Hash{}( key );
			auto const index = find_index( hash, key );
			return static_cast<bool>( index ) and
			       static_cast<bool>( m_indices[*index] );
		}

		bool count( Key const &key ) const noexcept {
			return exists( key ) ? 1 : 0;
		}

		size_t capacity( ) const noexcept {
			return m_indices.size( );
		}

		size_t size( ) const noexcept {
			return daw::algorithm::accumulate(
			  std::begin( m_indices ), std::end( m_indices ), 0ULL,
			  []( auto const &opt ) {
				  return static_cast<bool>( opt ) ? 1ULL : 0ULL;
			  } );
		}
	};
} // namespace daw
