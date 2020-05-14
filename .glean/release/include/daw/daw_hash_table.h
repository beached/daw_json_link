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

#include <algorithm>
#include <functional>
#include <iterator>
#include <list>
#include <numeric>
#include <string>
#include <utility>

#include "daw_exception.h"
#include "daw_exchange.h"
#include "daw_fnv1a_hash.h"
#include "daw_heap_array.h"
#include "daw_move.h"
#include "daw_swap.h"
#include "daw_traits.h"
#include "daw_utility.h"

namespace daw {
	namespace impl {
		template<typename ValueType>
		struct hash_table;
		template<typename ValueType>
		struct hash_table_item;

		template<typename ValueType>
		struct hash_table_item {
			enum Sentinals : size_t {
				sentinal_empty,   // empty, nothing there
				sentinal_removed, // has been removed so when looking for a hash just
				                  // skip and don't treat as unfound
				SentinalsSize     // must be last.  Marker to get number of Sentinals
			};
			using value_type = typename daw::traits::root_type_t<ValueType>;
			size_t hash; // 0 is the sentinel to mark unused
			value_type value;

			void clear( ) {
				hash = Sentinals::sentinal_removed;
				value = value_type{};
			}

			bool empty( ) const noexcept {
				return Sentinals::sentinal_empty == hash;
			}

			bool removed( ) const noexcept {
				return Sentinals::sentinal_removed == hash;
			}

			bool good( ) const noexcept {
				return hash >= Sentinals::SentinalsSize;
			}

			explicit operator bool( ) const {
				return hash >= Sentinals::SentinalsSize;
			}

			hash_table_item( )
			  : hash{Sentinals::sentinal_empty}
			  , value{} {}

			~hash_table_item( ) = default;

			hash_table_item( hash_table_item const &other )
			  : hash{( other.hash == Sentinals::sentinal_removed )
			           ? Sentinals::sentinal_empty
			           : other.hash}
			  , value{other.value} {}

			hash_table_item( hash_table_item &&other ) noexcept
			  : hash{daw::move( other.hash )}
			  , value{daw::move( other.value )} {}

			hash_table_item &operator=( hash_table_item const &rhs ) {
				if( this != &rhs ) {
					hash = ( rhs.hash == Sentinals::sentinal_removed )
					         ? Sentinals::sentinal_empty
					         : rhs.hash;
					hash = rhs.hash;
					value = rhs.value;
				}
				return *this;
			}

			hash_table_item &operator=( hash_table_item &&rhs ) noexcept {
				if( this != &rhs ) {
					hash = daw::exchange( rhs.hash, Sentinals::sentinal_empty );
					value = daw::move( rhs.value );
				}
				return *this;
			}
		}; // hash_table_item
	}    // namespace impl

	template<typename ValueType>
	struct hash_table_item_iterator
	  : public std::iterator<
	      std::bidirectional_iterator_tag,
	      typename impl::hash_table_item<ValueType>::value_type, std::ptrdiff_t,
	      impl::hash_table_item<ValueType> *,
	      typename impl::hash_table_item<ValueType>::value_type &> {
		using pointer = impl::hash_table_item<ValueType> *;

	private:
		pointer m_begin;
		pointer m_position;
		pointer m_end;

	public:
		hash_table_item_iterator( pointer first, pointer position, pointer last )
		  : m_begin{first}
		  , m_position{position}
		  , m_end{last} {}

		template<typename T,
		         typename = std::enable_if_t<std::is_convertible_v<T, pointer>>>
		hash_table_item_iterator( hash_table_item_iterator<T> const &other )
		  : m_begin( other.m_begin )
		  , m_position( other.m_position )
		  , m_end( other.m_end ) {}

		template<typename T,
		         typename = std::enable_if_t<std::is_convertible_v<T, pointer>>>
		hash_table_item_iterator &
		operator=( hash_table_item_iterator<T> const &rhs ) {
			if( this != rhs ) {
				hash_table_item_iterator tmp{rhs};
				using std::swap;
				swap( *this, tmp );
			}
			return *this;
		}

		void swap( hash_table_item_iterator &rhs ) noexcept {
			using std::swap;
			swap( m_begin, rhs.m_begin );
			swap( m_position, rhs.m_position );
			swap( m_end, rhs.m_end );
		}

		hash_table_item_iterator( ) = delete;

		~hash_table_item_iterator( ) = default;

		hash_table_item_iterator( hash_table_item_iterator const &other ) noexcept
		  : m_begin{other.m_begin}
		  , m_position{other.m_position}
		  , m_end{other.m_end} {}

		hash_table_item_iterator( hash_table_item_iterator &&other ) noexcept
		  : m_begin{daw::move( other.m_begin )}
		  , m_position{daw::move( other.m_position )}
		  , m_end{daw::move( other.m_end )} {}

		hash_table_item_iterator &
		operator=( hash_table_item_iterator const &rhs ) noexcept {
			if( this != &rhs ) {
				m_begin = rhs.m_begin;
				m_position = rhs.m_position;
				m_end = rhs.m_end;
			}
			return *this;
		}

		hash_table_item_iterator &
		operator=( hash_table_item_iterator &&rhs ) noexcept {
			if( this != &rhs ) {
				m_begin = daw::move( rhs.m_begin );
				m_position = daw::move( rhs.m_position );
				m_end = daw::move( rhs.m_end );
			}
			return *this;
		}

		hash_table_item_iterator &operator++( ) {
			++m_position;
			m_position = std::find_if(
			  m_position, m_end, []( auto const &val ) { return val.good( ); } );
			return *this;
		}

		bool at_end( ) const {
			return m_position != m_end;
		}

		bool valid( ) const {
			return at_end( ) or !m_position->empty( );
		}

		hash_table_item_iterator operator++( int ) {
			hash_table_item_iterator result{*this};
			++( *this );
			return result;
		}

		hash_table_item_iterator &operator--( ) {
			--m_position;
			while( m_position > m_begin and !m_position->good( ) ) {
				--m_position;
			}
			return *this;
		}

		hash_table_item_iterator operator--( int ) {
			hash_table_item_iterator result{*this};
			--( *this );
			return result;
		}

		template<typename T, typename U>
		friend bool operator==( hash_table_item_iterator<T> const &lhs,
		                        hash_table_item_iterator<U> const &rhs );

		template<typename T, typename U>
		friend bool operator!=( hash_table_item_iterator<T> const &lhs,
		                        hash_table_item_iterator<U> const &rhs );

		template<typename T, typename U>
		friend bool operator>( hash_table_item_iterator<T> const &lhs,
		                       hash_table_item_iterator<U> const &rhs );

		template<typename T, typename U>
		friend bool operator>=( hash_table_item_iterator<T> const &lhs,
		                        hash_table_item_iterator<U> const &rhs );

		template<typename T, typename U>
		friend bool operator<( hash_table_item_iterator<T> const &lhs,
		                       hash_table_item_iterator<U> const &rhs );

		template<typename T, typename U>
		friend bool operator<=( hash_table_item_iterator<T> const &lhs,
		                        hash_table_item_iterator<U> const &rhs );

		auto operator*( ) {
			return m_position->value;
		}

		auto const &operator*( ) const {
			return m_position->value;
		}
	}; // hash_table_item_iterator

	template<typename T, typename U>
	bool operator==( hash_table_item_iterator<T> const &lhs,
	                 hash_table_item_iterator<U> const &rhs ) {
		return lhs.m_position == rhs.m_position;
	}

	template<typename T, typename U>
	bool operator!=( hash_table_item_iterator<T> const &lhs,
	                 hash_table_item_iterator<U> const &rhs ) {
		return lhs.m_position != rhs.m_position;
	}

	template<typename T, typename U>
	bool operator>( hash_table_item_iterator<T> const &lhs,
	                hash_table_item_iterator<U> const &rhs ) {
		return lhs.m_position > rhs.m_position;
	}

	template<typename T, typename U>
	bool operator>=( hash_table_item_iterator<T> const &lhs,
	                 hash_table_item_iterator<U> const &rhs ) {
		return lhs.m_position >= rhs.m_position;
	}

	template<typename T, typename U>
	bool operator<( hash_table_item_iterator<T> const &lhs,
	                hash_table_item_iterator<U> const &rhs ) {
		return lhs.m_position < rhs.m_position;
	}

	template<typename T, typename U>
	bool operator<=( hash_table_item_iterator<T> const &lhs,
	                 hash_table_item_iterator<U> const &rhs ) {
		return lhs.m_position <= rhs.m_position;
	}

	template<typename T>
	void swap( hash_table_item_iterator<T> &lhs,
	           hash_table_item_iterator<T> &rhs ) noexcept {
		lhs.swap( rhs );
	}

	template<typename Value>
	struct hash_table {
		using value_type = daw::traits::root_type_t<Value>;
		using mapped_type = daw::traits::root_type_t<Value>;
		using size_type = size_t;
		using reference = value_type &;
		using const_reference = value_type const &;
		using iterator = hash_table_item_iterator<value_type>;
		using const_iterator = hash_table_item_iterator<value_type const>;

	private:
		using values_type = daw::heap_array<impl::hash_table_item<value_type>>;
		using priv_iterator = typename values_type::iterator;
		double m_resize_ratio;
		size_t m_load;
		size_t m_growth_counter;
		size_t m_max_load;
		values_type m_values;

		auto priv_begin( ) {
			return m_values.begin( );
		}

		auto priv_begin( ) const {
			return m_values.begin( );
		}

		auto priv_end( ) {
			return m_values.end( );
		}

		auto priv_end( ) const {
			return m_values.end( );
		}

	public:
		hash_table( size_t start_size, double resize_ratio = 2.0,
		            size_t max_load_percent = 70 )
		  : m_resize_ratio{resize_ratio}
		  , m_load{0}
		  , m_growth_counter{0}
		  , m_max_load{max_load_percent}
		  , m_values{start_size} {

			daw::exception::daw_throw_on_false( start_size > 0 );
		}

		hash_table( )
		  : hash_table( 7, 2.6, 50 ) {}

		~hash_table( ) = default;

		hash_table( hash_table && ) noexcept = default;

		hash_table( hash_table const & ) = default;

		hash_table &operator=( hash_table && ) noexcept = default;

		hash_table &operator=( hash_table const & ) = default;

		iterator begin( ) {
			auto result = iterator{priv_begin( ), priv_begin( ), priv_end( )};
			++result;
			return result;
		}

		const_iterator begin( ) const {
			auto result = const_iterator{priv_begin( ), priv_begin( ), priv_end( )};
			++result;
			return result;
		}

		const_iterator cbegin( ) const {
			auto result = const_iterator{priv_begin( ), priv_begin( ), priv_end( )};
			++result;
			return result;
		}

		iterator end( ) {
			return iterator{priv_begin( ), priv_end( ), priv_end( )};
		}

		const_iterator end( ) const {
			return iterator{priv_begin( ), priv_end( ), priv_end( )};
		}

		const_iterator cend( ) const {
			return iterator{priv_begin( ), priv_end( ), priv_end( )};
		}

	private:
		template<typename KeyType>
		static size_t hash_fn( KeyType &&key ) {
			static const auto s_hash = []( auto &&k ) {
				size_t result =
				  ( daw::fnv1a_hash( std::forward<KeyType>( k ) ) %
				    ( std::numeric_limits<size_t>::max( ) -
				      impl::hash_table_item<value_type>::SentinalsSize ) ) +
				  impl::hash_table_item<value_type>::SentinalsSize; // Guarantee we
				                                                    // cannot be zero
				return result;
			};
			return s_hash( std::forward<KeyType>( key ) );
		}

		static size_t hash_fn( char const *c_str ) {
			std::string value = c_str;
			return hash_fn( value );
		}

		static size_t scale_hash( size_t hash, size_t table_size ) {
			// Scale value to capacity using MAD(Multiply-Add-Divide) compression
			// Use the two largest Prime's that fit in a 64bit unsigned integral
			daw::exception::daw_throw_on_false(
			  table_size <
			  max_size( ) ); // Table size must be less than max of ptrdiff_t as we
			                 // use the value 0 as a sentinel.  This should be rare
			daw::exception::daw_throw_on_false(
			  hash >=
			  impl::hash_table_item<
			    value_type>::SentinalsSize ); // zero is a sentinel for no value
			static const size_t prime_a = 18446744073709551557u;
			static const size_t prime_b = 18446744073709551533u;
			return ( hash * prime_a + prime_b ) % table_size;
		}

		static size_t resize_table( values_type &old_table, size_t new_size ) {
			values_type new_hash_table{new_size};
			size_t load = 0;
			for( auto &&current_item : old_table ) {
				if( current_item ) {
					++load;
					auto pos = find_item_by_hash( current_item.hash, new_hash_table );
					daw::exception::daw_throw_on_false( pos != new_hash_table.cend( ) );
					daw::cswap( *pos, current_item );
				}
			}
			daw::cswap( old_table, new_hash_table );
			return load;
		}

		static auto find_item_by_hash( size_t hash, values_type const &tbl,
		                               bool skip_removed = true ) {
			auto const scaled_hash =
			  static_cast<ptrdiff_t>( scale_hash( hash, tbl.size( ) ) );
			auto const start_it = std::next( tbl.begin( ), scaled_hash );
			// loop through all values until an empty spot is found(at end start at
			// beginning)
			auto const is_here = [skip_removed, hash]( auto const &item ) {
				// hash can be empty, removed or have a value.
				if( hash == item.hash ) {
					return true;
				}
				if( item.empty( ) ) {
					return true;
				}
				// Only option left is that item is removed
				return !skip_removed;
			};
			auto pos = std::find_if( start_it, tbl.cend( ), is_here );
			if( tbl.cend( ) == pos ) {
				pos = std::find_if( tbl.begin( ), start_it, is_here );
				if( start_it == pos ) {
					pos = tbl.cend( );
				}
			}
			return const_cast<priv_iterator>( pos );
		}

		static auto find_item_by_hash( size_t hash, hash_table const &tbl ) {
			return find_item_by_hash( hash, tbl.m_values );
		}

		static priv_iterator find_item_by_hash_or_create( size_t hash,
		                                                  hash_table &tbl ) {
			if( tbl.empty( ) or
			    ( ( tbl.m_load * 100 ) / tbl.m_values.size( ) ) > tbl.m_max_load ) {
				tbl.grow_table( );
			}
			auto pos = find_item_by_hash( hash, tbl.m_values );
			if( pos == tbl.priv_end( ) ) {
				pos = find_item_by_hash( hash, tbl.m_values, false );
				if( pos == tbl.priv_end( ) ) {
					tbl.grow_table( );
					pos = find_item_by_hash( hash, tbl );
					daw::exception::daw_throw_on_true( pos == tbl.priv_end( ) );
					// This is a WAG.  It be wrong but I had to pick something
					// without evidence to support it.
				}
			}
			if( !pos ) {
				++tbl.m_load;
				pos->hash = hash;
			}
			return pos;
		}

		static auto insert_into( impl::hash_table_item<value_type> &&item,
		                         hash_table &tbl ) {
			auto pos = find_item_by_hash_or_create( item.hash, tbl );
			*pos == daw::move( item );
		}

		static auto insert_into( size_t hash, value_type value, hash_table &tbl ) {
			auto pos = find_item_by_hash_or_create( hash, tbl );
			pos->value = daw::move( value );
		}

		void grow_table( ) {
			m_load = resize_table(
			  m_values,
			  static_cast<size_t>( std::ceil(
			    static_cast<double>( m_values.size( ) ) * m_resize_ratio ) ) );
			++m_growth_counter;
		}

	public:
		static size_t max_size( ) {
			return static_cast<size_t>( std::numeric_limits<ptrdiff_t>::max( ) - 1 );
		}

		template<typename Key>
		iterator insert( Key const &key, value_type value ) {
			auto hash = hash_fn( key );
			auto pos = find_item_by_hash_or_create( hash, *this );
			pos->value = daw::move( value );
			return iterator{priv_begin( ), pos, priv_end( )};
		}

		template<typename Key>
		auto &operator[]( Key const &key ) {
			auto hash = hash_fn( key );
			auto pos = find_item_by_hash_or_create( hash, *this );
			return pos->value;
		}

		template<typename Key>
		auto const &operator[]( Key const &key ) const {
			return at( key );
		}

		template<typename Key>
		auto const &at( Key const &key ) const {
			auto hash = hash_fn( key );
			auto pos = find_item_by_hash( hash, *this );

			daw::exception::precondition_check<std::out_of_range>(
			  priv_end( ) != pos, "Key does not already exist" );

			return pos->value;
		}

		template<typename Key>
		auto &at( Key const &key ) {
			auto hash = hash_fn( key );
			auto pos = find_item_by_hash( hash, *this );

			daw::exception::precondition_check<std::out_of_range>(
			  priv_end( ) != pos, "Key does not already exist" );

			return pos->value;
		}

		template<typename Key>
		size_t erase( Key &&key ) {
			auto hash = hash_fn( key );
			auto pos = find_item_by_hash( hash, m_values );
			if( pos != m_values.cend( ) ) {
				pos->clear( );
				--m_load;
				return 1;
			}
			return 0;
		}

		size_t erase( iterator it ) {
			if( !it.at_end( ) ) {
				it->clear( );
				--m_load;
				return 1;
			}
			return 0;
		}

		void shrink_to_fit( ) {
			resize_table( m_values, m_load );
			++m_growth_counter;
		}

		size_t occupied( ) const {
			return m_load;
		}

		size_t capacity( ) const {
			return m_values.size( );
		}

		bool empty( ) const {
			return 0 == m_load;
		}

		void clear( ) {
			m_values.clear( );
			m_load = 0;
			++m_growth_counter;
		}

		size_t growth_counter( ) const {
			return m_growth_counter;
		}

		void swap( hash_table &rhs ) noexcept {
			daw::cswap( m_values, rhs.m_values );
			daw::cswap( m_load, rhs.m_load );
			daw::cswap( m_growth_counter, rhs.m_growth_counter );
			daw::cswap( m_resize_ratio, rhs.m_resize_ratio );
			daw::cswap( m_max_load, rhs.m_max_load );
		}

		template<typename Key>
		bool key_exists( Key const &key ) const {
			auto hash = hash_fn( key );
			auto pos = find_item_by_hash( hash, m_values );
			return pos != m_values.priv_end( );
		}

		template<typename Key>
		iterator find( Key const &key ) {
			auto hash = hash_fn( key );
			auto pos = find_item_by_hash( hash, m_values );
			return iterator{priv_begin( ), pos, priv_end( )};
		}

		template<typename Key>
		const_iterator find( Key const &key ) const {
			auto hash = hash_fn( key );
			auto pos = find_item_by_hash( hash, m_values );
			return const_iterator{priv_begin( ), pos, priv_end( )};
		}

		template<typename Key>
		size_t count( Key const &key ) const {
			return key_exists( key ) ? 1 : 0;
		}

	private:
		bool hash_exists( size_t hash ) const {
			auto pos = find_item_by_hash( hash, m_values );
			return pos != m_values.priv_end( );
		}

	public:
		friend bool operator==( hash_table const &lhs, hash_table const &rhs ) {
			if( lhs.m_load == rhs.m_load ) {
				for( auto const &value : lhs.m_values ) {
					if( !rhs.hash_exists( value.hash ) ) {
						return false;
					}
				}
				return true;
			}
			return false;
		}

		friend bool operator!=( hash_table const &lhs, hash_table const &rhs ) {
			if( lhs.m_load == rhs.m_load ) {
				for( auto const &value : lhs.m_values ) {
					if( !rhs.hash_exists( value.hash ) ) {
						return true;
					}
				}
				return false;
			}
			return true;
		}

	}; // struct hash_table

	template<typename Key, typename Value>
	auto create_hash_table(
	  std::initializer_list<std::pair<Key, Value>> const &values ) {
		hash_table<Value> result;
		for( auto const &p : values ) {
			result.insert( p.first, p.second );
		}
		return result;
	}

	template<typename T>
	void swap( hash_table<T> &lhs, hash_table<T> &rhs ) noexcept {
		lhs.swap( rhs );
	}
} // namespace daw
