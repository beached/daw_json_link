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

#include "daw_exception.h"
#include "daw_fnv1a_hash.h"
#include "daw_heap_array.h"
#include "daw_move.h"
#include "daw_swap.h"
#include "daw_traits.h"

namespace daw {
	namespace impl {
		namespace sentinals {
			enum sentinals : size_t { empty = 0, removed, sentinals_size };
		} // namespace sentinals

		template<typename KeyType>
		struct s_hash_fn_t {
			constexpr size_t operator( )( KeyType const &k ) noexcept {
				size_t result =
				  ( daw::fnv1a_hash( k ) % ( std::numeric_limits<size_t>::max( ) -
				                             impl::sentinals::sentinals_size ) ) +
				  impl::sentinals::sentinals_size;
				return result;
			}

			constexpr size_t operator( )( KeyType const *k ) noexcept {
				size_t result =
				  ( daw::fnv1a_hash( k ) % ( std::numeric_limits<size_t>::max( ) -
				                             impl::sentinals::sentinals_size ) ) +
				  impl::sentinals::sentinals_size;
				return result;
			}
		};
	} // namespace impl

	namespace resize_policies {
		struct golden_ratio {
			constexpr size_t operator( )( size_t current_size ) const {
				return static_cast<size_t>(
				  static_cast<double>( current_size ) *
				  1.61803398875 ); // assumes 2^53(9 Petabytes) will be enough for
				                   // anybody :)
			}
		};
	} // namespace resize_policies

	template<typename Value, size_t m_initial_size = 11,
	         uint8_t resize_ratio = 80,
	         typename ResizePolicy = resize_policies::golden_ratio>
	struct hash_table {
		static_assert( m_initial_size > 0,
		               "Must supply a positive initial_size larger than 0" );
		using value_type = daw::traits::root_type_t<Value>;
		using reference = value_type &;
		using const_reference = value_type const &;

	private:
		daw::heap_array<size_t> m_hashes;
		daw::heap_array<value_type> m_values;

		static constexpr size_t max_size( ) noexcept {
			return static_cast<size_t>( std::numeric_limits<ptrdiff_t>::max( ) - 1 );
		}

		template<typename KeyType>
		static constexpr size_t hash_fn( KeyType const &key ) noexcept {
			return impl::s_hash_fn_t<KeyType>{}( key );
		}

		static constexpr size_t scale_hash( size_t hash, size_t table_size ) {
			// Scale value to capacity using MAD(Multiply-Add-Divide) compression
			// Use the two largest Prime's that fit in a 64bit unsigned integral
			daw::exception::daw_throw_on_false( table_size > 0 );
			daw::exception::daw_throw_on_false(
			  table_size <
			  max_size( ) ); // Table size must be less than max of ptrdiff_t as we
			                 // use the value 0 as a sentinel.  This should be rare
			daw::exception::daw_throw_on_false( hash >=
			                                    impl::sentinals::sentinals_size );

			size_t const prime_a = 18446744073709551557u;
			size_t const prime_b = 18446744073709551533u;
			return ( hash * prime_a + prime_b ) % table_size;
		}

		constexpr auto lookup( size_t const hash ) const {
			struct lookup_result_t {
				size_t position;
				size_t lookup_cost;
				bool found;

				lookup_result_t( )
				  : position{0}
				  , lookup_cost{0}
				  , found{false} {}
				operator bool( ) const noexcept {
					return found;
				}
			};
			lookup_result_t result;
			auto const s_hash = scale_hash( hash, m_hashes.size( ) );
			result.position = s_hash;
			size_t removed_found =
			  std::numeric_limits<size_t>::max( ); // Need a check and this will be
			                                       // rare
			for( ; result.position != m_hashes.size( ); ++result.position ) {
				if( m_hashes[result.position] == hash ) {
					result.found = true;
					result.lookup_cost = result.position - s_hash;
					return result;
				} else if( m_hashes[result.position] == impl::sentinals::empty ) {
					result.lookup_cost = result.position - s_hash;
					return result;
				} else if( m_hashes[result.position] == impl::sentinals::removed &&
				           result.position < removed_found ) {
					removed_found = result.position;
				}
			}
			result.position = 0;
			for( ; result.position != s_hash; ++result.position ) {
				if( m_hashes[result.position] == hash ) {
					result.found = true;
					result.lookup_cost =
					  ( m_hashes.size( ) - s_hash ) + result.position - s_hash;
					return result;
				} else if( m_hashes[result.position] == impl::sentinals::empty ) {
					result.lookup_cost =
					  ( m_hashes.size( ) - s_hash ) + result.position - s_hash;
					return result;
				} else if( m_hashes[result.position] == impl::sentinals::removed &&
				           result.position < removed_found ) {
					removed_found = result.position;
				}
			}
			if( removed_found < std::numeric_limits<size_t>::max( ) ) {
				result.lookup_cost = m_hashes.size( );
				result.position = removed_found;
				return result;
			}
			result.position = m_hashes.size( ); // Indicate that there are no empty or
			                                    // removed cells, table is full
			return result;
		}

		void resize_tables( size_t new_size ) {
			hash_table new_tbl{new_size};
			for( size_t n = 0; n < m_hashes.size( ); ++n ) {
				if( m_hashes[n] >= impl::sentinals::sentinals_size ) {
					new_tbl[m_hashes[n]] = daw::move( m_values[n] );
				}
			}
			daw::cswap( *this, new_tbl );
		}

		void resize_tables( ) {
			resize_tables( ResizePolicy{}( m_hashes.size( ) ) );
		}

		static constexpr bool should_resize( size_t lookup_cost,
		                                     size_t current_size ) {
			daw::exception::daw_throw_on_false( current_size > 0 );
			return ( ( lookup_cost * 100 ) / current_size ) >= resize_ratio;
		}

	public:
		hash_table( )
		  : m_hashes{m_initial_size, impl::sentinals::empty}
		  , m_values{m_initial_size} {

			daw::exception::daw_throw_on_false( m_hashes.size( ) > 0 );
			daw::exception::daw_throw_on_false( m_values.size( ) > 0 );
		}

		hash_table( size_t initial_size )
		  : m_hashes{initial_size, impl::sentinals::empty}
		  , m_values( initial_size ) {

			daw::exception::daw_throw_on_false( m_hashes.size( ) > 0 );
			daw::exception::daw_throw_on_false( m_values.size( ) > 0 );
		}

		void swap( hash_table &rhs ) noexcept {
			daw::cswap( m_hashes, rhs.m_hashes );
			daw::cswap( m_values, rhs.m_values );
		}

		template<typename Key>
		const_reference operator[]( Key const &key ) const {
			auto const hash = hash_fn<Key>( key );
			auto const is_found = lookup( hash );

			daw::exception::precondition_check<std::out_of_range>(
			  is_found, "Attempt to access an undefined key" );

			return m_values[is_found.position];
		}

		template<typename Key>
		reference operator[]( Key const &key ) {
			auto const hash = hash_fn<Key>( key );
			auto is_found = lookup( hash );
			if( ( !is_found and is_found.position == m_hashes.size( ) ) or
			    should_resize( is_found.lookup_cost, m_hashes.size( ) ) ) {
				resize_tables( );
				is_found = lookup( hash );
			}
			return m_values[is_found.position];
		}

		void shrink_to_fit( ) {
			size_t item_count = 0;
			for( auto const &hash : m_hashes ) {
				if( hash >= impl::sentinals::sentinals_size ) {
					++item_count;
				}
			}
			resize_tables( item_count );
		}
	};
} // namespace daw
