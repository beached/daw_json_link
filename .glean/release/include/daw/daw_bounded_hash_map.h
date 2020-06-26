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

#include <memory>
#include <numeric>
#include <utility>

#include "cpp_17.h"
#include "daw_algorithm.h"
#include "daw_optional.h"
#include "daw_traits.h"

namespace daw {
	template<typename Key, typename Value>
	struct key_value_t {
		Key key{ };
		Value value{ };

		constexpr key_value_t( ) = default;

		template<typename K, typename V>
		constexpr key_value_t( K &&k, V &&v )
		  : key( std::forward<K>( k ) )
		  , value( std::forward<V>( v ) ) {}
	};

	template<typename Key, typename Value>
	struct bounded_hash_map_item_t {
		key_value_t<Key, Value> kv{ };
		bool has_value = false;

		constexpr bounded_hash_map_item_t( ) = default;

		constexpr bounded_hash_map_item_t( key_value_t<Key, Value> &&kv )
		  : kv( std::move( kv ) )
		  , has_value( true ) {}

		constexpr bounded_hash_map_item_t( key_value_t<Key, Value> const &kv )
		  : kv( kv )
		  , has_value( true ) {}

		template<typename K, typename V>
		constexpr bounded_hash_map_item_t( K &&k, V &&v )
		  : kv( std::forward<K>( k ), std::forward<V>( v ) )
		  , has_value( true ) {}

		explicit constexpr operator bool( ) const {
			return has_value;
		}
	};
	template<typename Key, typename Value>
	bounded_hash_map_item_t( Key &&key, Value &&value )
	  -> bounded_hash_map_item_t<daw::remove_cvref_t<Key>,
	                             daw::remove_cvref_t<Value>>;

	template<typename Key, typename Value>
	struct bounded_hash_map_iterator;

	template<typename Key, typename Value>
	struct const_bounded_hash_map_iterator {
		using difference_type = std::ptrdiff_t;
		using value_type = key_value_t<Key, Value>;
		using pointer = value_type const *;
		using const_pointer = value_type const *;
		using reference = value_type const &;
		using const_reference = value_type const &;
		using iterator_category = std::bidirectional_iterator_tag;

	private:
		bounded_hash_map_item_t<Key, Value> const *const m_first;
		bounded_hash_map_item_t<Key, Value> const *const m_last;
		bounded_hash_map_item_t<Key, Value> const *m_position;

	public:
		constexpr const_bounded_hash_map_iterator(
		  bounded_hash_map_item_t<Key, Value> const *first,
		  bounded_hash_map_item_t<Key, Value> const *last,
		  bounded_hash_map_item_t<Key, Value> const *pos )
		  : m_first( first )
		  , m_last( last )
		  , m_position( pos ) {}

		constexpr const_reference operator*( ) const {
			return m_position->kv;
		}

		constexpr const_pointer operator->( ) const {
			return std::addressof( m_position->kv );
		}

		constexpr const_bounded_hash_map_iterator &operator++( ) {
			++m_position;
			while( !m_position->has_value and m_position < m_last ) {
				++m_position;
			}
			return *this;
		}

		constexpr const_bounded_hash_map_iterator operator++( int ) {
			auto result = *this;
			operator++( );
			return result;
		}

		constexpr const_bounded_hash_map_iterator &operator--( ) {
			--m_position;
			while( !m_position->has_value and m_position > m_first ) {
				--m_position;
			}
			return *this;
		}

		constexpr const_bounded_hash_map_iterator operator--( int ) {
			auto result = *this;
			operator--( );
			return result;
		}

		friend struct bounded_hash_map_iterator<Key, Value>;
		constexpr int compare( const_bounded_hash_map_iterator const &rhs ) const {
			if( m_position == rhs.m_position ) {
				return 0;
			}
			if( m_position < rhs.m_position ) {
				return -1;
			}
			return 1;
		}

		constexpr int
		compare( bounded_hash_map_iterator<Key, Value> const &rhs ) const {
			if( m_position == rhs.m_position ) {
				return 0;
			}
			if( m_position < rhs.m_position ) {
				return -1;
			}
			return 1;
		}
	};

	template<typename Key, typename Value>
	struct bounded_hash_map_iterator {
		using difference_type = std::ptrdiff_t;
		using value_type = key_value_t<Key, Value>;
		using pointer = value_type *;
		using const_pointer = value_type const *;
		using reference = value_type &;
		using const_reference = value_type const &;
		using iterator_category = std::bidirectional_iterator_tag;

	private:
		bounded_hash_map_item_t<Key, Value> const *const m_first;
		bounded_hash_map_item_t<Key, Value> const *const m_last;
		bounded_hash_map_item_t<Key, Value> *m_position;

	public:
		constexpr bounded_hash_map_iterator(
		  bounded_hash_map_item_t<Key, Value> const *first,
		  bounded_hash_map_item_t<Key, Value> const *last,
		  bounded_hash_map_item_t<Key, Value> *pos )
		  : m_first( first )
		  , m_last( last )
		  , m_position( pos ) {}

		constexpr reference operator*( ) {
			return m_position->kv;
		}

		constexpr const_reference operator*( ) const {
			return m_position->kv;
		}

		constexpr pointer operator->( ) {
			return std::addressof( m_position->kv );
		}

		constexpr const_pointer operator->( ) const {
			return std::addressof( m_position->kv );
		}

		constexpr bounded_hash_map_iterator &operator++( ) {
			++m_position;
			while( !m_position->has_value and m_position < m_last ) {
				++m_position;
			}
			return *this;
		}

		constexpr bounded_hash_map_iterator operator++( int ) {
			auto result = *this;
			operator++( );
			return result;
		}

		constexpr bounded_hash_map_iterator &operator--( ) {
			--m_position;
			while( !m_position->has_value and m_position > m_first ) {
				--m_position;
			}
			return *this;
		}

		constexpr bounded_hash_map_iterator operator--( int ) {
			auto result = *this;
			operator--( );
			return result;
		}

		constexpr operator const_bounded_hash_map_iterator<Key, Value>( ) const {
			return { m_first, m_last, m_position };
		}

		friend struct const_bounded_hash_map_iterator<Key, Value>;
		constexpr int
		compare( const_bounded_hash_map_iterator<Key, Value> const &rhs ) const {
			if( m_position == rhs.m_position ) {
				return 0;
			}
			if( m_position < rhs.m_position ) {
				return -1;
			}
			return 1;
		}

		constexpr int compare( bounded_hash_map_iterator const &rhs ) const {
			if( m_position == rhs.m_position ) {
				return 0;
			}
			if( m_position < rhs.m_position ) {
				return -1;
			}
			return 1;
		}
	};

	template<typename Key, typename Value>
	constexpr bool
	operator==( bounded_hash_map_iterator<Key, Value> const &lhs,
	            bounded_hash_map_iterator<Key, Value> const &rhs ) {
		return lhs.compare( rhs ) == 0;
	}

	template<typename Key, typename Value>
	constexpr bool
	operator==( const_bounded_hash_map_iterator<Key, Value> const &lhs,
	            const_bounded_hash_map_iterator<Key, Value> const &rhs ) {
		return lhs.compare( rhs ) == 0;
	}

	template<typename Key, typename Value>
	constexpr bool
	operator==( bounded_hash_map_iterator<Key, Value> const &lhs,
	            const_bounded_hash_map_iterator<Key, Value> const &rhs ) {
		return lhs.compare( rhs ) == 0;
	}

	template<typename Key, typename Value>
	constexpr bool
	operator==( const_bounded_hash_map_iterator<Key, Value> const &lhs,
	            bounded_hash_map_iterator<Key, Value> const &rhs ) {
		return lhs.compare( rhs ) == 0;
	}

	template<typename Key, typename Value>
	constexpr bool
	operator!=( bounded_hash_map_iterator<Key, Value> const &lhs,
	            bounded_hash_map_iterator<Key, Value> const &rhs ) {
		return lhs.compare( rhs ) != 0;
	}

	template<typename Key, typename Value>
	constexpr bool
	operator!=( const_bounded_hash_map_iterator<Key, Value> const &lhs,
	            const_bounded_hash_map_iterator<Key, Value> const &rhs ) {
		return lhs.compare( rhs ) != 0;
	}

	template<typename Key, typename Value>
	constexpr bool
	operator!=( bounded_hash_map_iterator<Key, Value> const &lhs,
	            const_bounded_hash_map_iterator<Key, Value> const &rhs ) {
		return lhs.compare( rhs ) != 0;
	}

	template<typename Key, typename Value>
	constexpr bool
	operator!=( const_bounded_hash_map_iterator<Key, Value> const &lhs,
	            bounded_hash_map_iterator<Key, Value> const &rhs ) {
		return lhs.compare( rhs ) != 0;
	}

	template<typename Key, typename Value, size_t N,
	         typename Hash = std::hash<Key>,
	         typename KeyEqual = std::equal_to<Key>>
	struct bounded_hash_map {
		using key_type = Key;
		using mapped_type = Value;
		using size_type = size_t;
		using difference_type = ptrdiff_t;
		using hasher = Hash;
		using key_equal = KeyEqual;
		using value_type = key_value_t<Key, Value>;
		using reference = value_type &;
		using const_reference = value_type const &;
		using pointer = value_type *;
		using const_pointer = value_type const *;
		using iterator = bounded_hash_map_iterator<Key, Value>;
		using const_iterator = const_bounded_hash_map_iterator<Key, Value>;

	private:
		std::array<bounded_hash_map_item_t<Key, Value>, N + 1> m_data{ };

		static constexpr size_t scale_hash( size_t hash, size_t range_size ) {
			size_t const prime_a = 18446744073709551557u;
			size_t const prime_b = 18446744073709551533u;
			return ( hash * prime_a + prime_b ) % range_size;
		}

		constexpr daw::optional<size_type> find_index( Key const &key ) const {
			size_type const hash = Hash{ }( key );
			size_type const scaled_hash = scale_hash( hash, capacity( ) );

			for( size_type n = scaled_hash; n < capacity( ); ++n ) {
				if( !m_data[n] or key_equal{ }( m_data[n].kv.key, key ) ) {
					return n;
				}
			}
			for( size_type n = 0; n < scaled_hash; ++n ) {
				if( !m_data[n] or key_equal{ }( m_data[n].kv.key, key ) ) {
					return n;
				}
			}
			return { };
		}

		constexpr void insert( value_type &&item ) {
			auto const index = find_index( item.key );
			m_data[*index] =
			  daw::bounded_hash_map_item_t<Key, Value>( std::move( item ) );
		}

	public:
		constexpr bounded_hash_map( ) = default;

		template<size_type ItemCount>
		constexpr bounded_hash_map(
		  std::pair<Key, Value> const ( &init_values )[ItemCount] ) {

			static_assert( ItemCount <= N );
			for( auto const &kv : init_values ) {
				insert( kv.first, kv.second );
			}
		}

		template<typename Iterator>
		constexpr bounded_hash_map( Iterator first, Iterator last ) {
			for( ; first != last; ++first ) {
				insert( *first );
			}
		}

		template<typename K, typename V,
		         std::enable_if_t<
		           daw::all_true_v<std::is_same_v<Key, daw::remove_cvref_t<K>>,
		                           std::is_same_v<Value, daw::remove_cvref_t<V>>>,
		           std::nullptr_t> = nullptr>
		constexpr void insert( K &&key, V &&value ) {
			auto const index = find_index( key );
			m_data[*index] = bounded_hash_map_item_t( std::forward<K>( key ),
			                                          std::forward<V>( value ) );
		}

		constexpr void insert( std::pair<Key const, Value> const &item ) {
			auto const index = find_index( item.first );
			m_data[*index] = bounded_hash_map_item_t( item.first, item.second );
		}

		constexpr bool exists( Key const &key ) const {
			auto idx = find_index( key );
			if( idx ) {
				auto result = static_cast<bool>( m_data[*idx] );
				return result;
			}
			return false;
		}

		template<typename K>
		constexpr mapped_type &operator[]( K &&key ) {
			static_assert( std::is_convertible_v<std::remove_reference_t<K>, Key>,
			               "Incompatable key passed" );
			decltype( auto ) item = m_data[*find_index( key )];
			if( !item ) {
				item.has_value = true;
				item.kv.key = std::forward<K>( key );
				item.kv.value = Value{ };
			}
			return item.kv.value;
		}

		constexpr mapped_type const &operator[]( Key const &key ) const {
			decltype( auto ) item = m_data[*find_index( key )];
			if( !item ) {
				std::abort( );
			}
			return item.kv.value;
		}

		constexpr daw::optional<mapped_type const &>
		try_get( Key const &key ) const {
			auto idx = find_index( key );
			if( !idx ) {
				return { };
			}
			decltype( auto ) item = m_data[*idx];
			if( !item ) {
				return { };
			}
			return item.kv.value;
		}

		constexpr size_type count( Key const &key ) const {
			if( exists( key ) ) {
				return 1U;
			}
			return 0U;
		}

		static constexpr size_type capacity( ) {
			return N;
		}

		constexpr size_type size( ) const {
			using std::cbegin;
			using std::cend;
			return daw::algorithm::accumulate(
			  cbegin( m_data ), cend( m_data ), 0ULL, []( auto &&init, auto &&v ) {
				  if( v ) {
					  return std::forward<decltype( init )>( init ) + 1ULL;
				  }
				  return std::forward<decltype( init )>( init );
			  } );
		}

		constexpr size_type empty( ) const {
			return daw::algorithm::find_if( std::cbegin( m_data ),
			                                std::cend( m_data ),
			                                []( auto const &value ) {
				                                return value;
			                                } ) == std::cend( m_data );
		}

		constexpr iterator begin( ) {
			auto it = m_data.data( );
			auto last = m_data.data( ) + capacity( );
			while( it != last and !it->has_value ) {
				++it;
			}
			return { m_data.data( ), last, it };
		}

		constexpr const_iterator begin( ) const {
			auto it = m_data.data( );
			auto last = m_data.data( ) + capacity( );
			while( it != last and !it->has_value ) {
				++it;
			}
			return { m_data.data( ), last, it };
		}

		constexpr const_iterator cbegin( ) const {
			auto it = m_data.data( );
			auto last = m_data.data( ) + capacity( );
			while( it != last and !it->has_value ) {
				++it;
			}
			return { m_data.data( ), last, it };
		}

		constexpr iterator end( ) {
			return { m_data.data( ), m_data.data( ) + capacity( ),
			         m_data.data( ) + capacity( ) };
		}

		constexpr const_iterator end( ) const {
			return { m_data.data( ), m_data.data( ) + capacity( ),
			         m_data.data( ) + capacity( ) };
		}

		constexpr const_iterator cend( ) const {
			return { m_data.data( ), m_data.data( ) + capacity( ),
			         m_data.data( ) + capacity( ) };
		}

		constexpr iterator find( Key const &key ) {
			auto idx = find_index( key );
			if( !idx ) {
				return end( );
			}
			return { m_data.data( ), m_data.data( ) + capacity( ), &m_data[*idx] };
		}

		constexpr const_iterator find( Key const &key ) const {
			auto idx = find_index( key );
			if( !idx ) {
				return end( );
			}
			return { m_data.data( ), m_data.data( ) + capacity( ), &m_data[*idx] };
		}

		constexpr void erase( Key const &key ) {
			auto idx = find_index( key );
			if( idx ) {
				m_data[*idx] = bounded_hash_map_item_t<Key, Value>( );
			}
		}
	};

	template<typename Key, typename Value, typename Hash = std::hash<Key>,
	         size_t N>
	constexpr auto
	make_bounded_hash_map( std::pair<Key, Value> const ( &items )[N] ) {
		return bounded_hash_map<Key, Value, N, Hash>( items );
	}
} // namespace daw
