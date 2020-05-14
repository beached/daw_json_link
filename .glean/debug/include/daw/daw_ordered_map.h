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

#include <cstddef>
#include <memory>
#include <stdexcept>
#include <utility>
#include <vector>

#include "daw_algorithm.h"
#include "daw_enable_if.h"
#include "daw_utility.h"

namespace daw {
	namespace ordered_map_impl {
		template<typename Container>
		decltype( auto ) beginer( Container &&c ) noexcept {
			using std::begin;
			return begin( c );
		}

		template<typename Container>
		decltype( auto ) ender( Container &&c ) noexcept {
			using std::end;
			return end( c );
		}

		template<typename Container>
		decltype( auto ) cbeginer( Container &&c ) noexcept {
			using std::cbegin;
			return cbegin( c );
		}

		template<typename Container>
		decltype( auto ) cender( Container &&c ) noexcept {
			using std::cend;
			return cend( c );
		}

		template<typename Container>
		decltype( auto ) rbeginer( Container &&c ) noexcept {
			using std::rbegin;
			return rbegin( c );
		}

		template<typename Container>
		decltype( auto ) render( Container &&c ) noexcept {
			using std::rend;
			return rend( c );
		}

		template<typename Container>
		decltype( auto ) crbeginer( Container &&c ) noexcept {
			using std::crbegin;
			return crbegin( c );
		}

		template<typename Container>
		decltype( auto ) crender( Container &&c ) noexcept {
			using std::crend;
			return crend( c );
		}

		template<typename Container>
		decltype( auto ) emptyer( Container &&c ) noexcept {
			using std::empty;
			return empty( c );
		}

		template<typename Container>
		decltype( auto ) sizer( Container &&c ) noexcept {
			using std::size;
			return size( c );
		}
	} // namespace ordered_map_impl
	// Use linear searching for key, keep values in insertion order
	template<typename Key, typename Value, typename Compare = std::less<Key>,
	         typename Allocator = std::allocator<std::pair<Key, Value>>,
	         typename Container = std::vector<std::pair<Key, Value>, Allocator>>
	struct ordered_map {
		using key_type = Key;
		using mapped_type = Value;
		using value_type = std::pair<Key const, Value>;
		using values_type = Container;
		using size_type = typename values_type::size_type;
		using difference_type = typename values_type::difference_type;
		using key_compare = Compare;
		using allocator_type = Allocator;
		using reference = mapped_type &;
		using const_reference = mapped_type const &;
		using pointer = typename values_type::pointer;
		using const_pointer = typename values_type::const_pointer;
		using iterator = typename values_type::iterator;
		using const_iterator = typename values_type::const_iterator;
		using reverse_iterator = typename values_type::reverse_iterator;
		using const_reverse_iterator = typename values_type::const_reverse_iterator;

	private:
		values_type m_values{};
		key_compare m_compare{};

	public:
		constexpr iterator begin( ) noexcept {
			using namespace daw::ordered_map_impl;
			return beginer( m_values );
		}

		constexpr const_iterator begin( ) const noexcept {
			using namespace daw::ordered_map_impl;
			return beginer( m_values );
		}

		constexpr const_iterator cbegin( ) const noexcept {
			using namespace daw::ordered_map_impl;
			return cbeginer( m_values );
		}

		constexpr iterator rbegin( ) noexcept {
			using namespace daw::ordered_map_impl;
			return rbeginer( m_values );
		}

		constexpr const_iterator rbegin( ) const noexcept {
			using namespace daw::ordered_map_impl;
			return rbeginer( m_values );
		}

		constexpr const_iterator crbegin( ) const noexcept {
			using namespace daw::ordered_map_impl;
			return crbeginer( m_values );
		}

		constexpr iterator end( ) noexcept {
			using namespace daw::ordered_map_impl;
			return ender( m_values );
		}

		constexpr const_iterator end( ) const noexcept {
			using namespace daw::ordered_map_impl;
			return ender( m_values );
		}

		constexpr const_iterator cend( ) const noexcept {
			using namespace daw::ordered_map_impl;
			return cender( m_values );
		}

		constexpr iterator rend( ) noexcept {
			using namespace daw::ordered_map_impl;
			return render( m_values );
		}

		constexpr const_iterator rend( ) const noexcept {
			using namespace daw::ordered_map_impl;
			return render( m_values );
		}

		constexpr const_iterator crend( ) const noexcept {
			using namespace daw::ordered_map_impl;
			return crender( m_values );
		}

		constexpr decltype( auto ) front( ) noexcept {
			return m_values.front( );
		}

		constexpr decltype( auto ) front( ) const noexcept {
			return m_values.front( );
		}

		constexpr decltype( auto ) back( ) noexcept {
			return m_values.back( );
		}

		constexpr decltype( auto ) back( ) const noexcept {
			return m_values.back( );
		}

		constexpr bool empty( ) const noexcept {
			using namespace daw::ordered_map_impl;
			return emptyer( m_values );
		}

		constexpr size_type size( ) const noexcept {
			using namespace daw::ordered_map_impl;
			return static_cast<size_type>( sizer( m_values ) );
		}

		constexpr auto max_size( ) const noexcept
		  -> decltype( std::declval<values_type const>( ).max_size( ) ) {
			return m_values.max_size( );
		}

		constexpr void clear( ) {
			m_values.clear( );
		}
		template<typename K>
		constexpr iterator find( K const &key ) {
			return daw::algorithm::find_if( begin( ), end( ),
			                                [&]( auto const &item ) {
				                                return !m_compare( item.first, key ) and
				                                       !m_compare( key, item.first );
			                                } );
		}

		template<typename K>
		constexpr const_iterator find( K const &key ) const {
			return daw::algorithm::find_if( begin( ), end( ),
			                                [&]( auto const &item ) {
				                                return !m_compare( item.first, key ) and
				                                       !m_compare( key, item.first );
			                                } );
		}

		constexpr ordered_map( ) = default;

		constexpr ordered_map( key_compare const &comp,
		                       allocator_type const &alloc )
		  : m_values( alloc )
		  , m_compare( comp ) {}

		template<typename InputIterator>
		constexpr ordered_map( InputIterator first, InputIterator last,
		                       key_compare const &comp = key_compare{},
		                       allocator_type const &alloc = allocator_type{} )
		  : m_compare( comp )
		  , m_values( alloc ) {

			while( first != last ) {
				if( auto pos = find( *first ); pos == end( ) ) {
					m_values.insert( pos, *first );
				}
				++first;
			}
		}

		template<typename InputIterator>
		constexpr ordered_map( InputIterator first, InputIterator last,
		                       allocator_type const &alloc = allocator_type{} )
		  : m_values( alloc ) {

			while( first != last ) {
				if( auto pos = find( *first ); pos == end( ) ) {
					m_values.insert( pos, *first );
				}
				++first;
			}
		}

		constexpr ordered_map( std::initializer_list<value_type> init,
		                       key_compare const &comp,
		                       allocator_type const &alloc )
		  : m_values( alloc )
		  , m_compare( comp ) {

			for( auto const &value : init ) {
				if( auto pos = find( value ); pos == end( ) ) {
					m_values.insert( pos, value );
				}
			}
		}

		constexpr ordered_map( std::initializer_list<value_type> init,
		                       allocator_type const &alloc )
		  : m_values( alloc ) {

			for( auto const &value : init ) {
				if( auto pos = find( value ); pos == end( ) ) {
					m_values.insert( pos, value );
				}
			}
		}

		template<typename K>
		constexpr iterator lower_bound( K const &x ) {
			return daw::algorithm::lower_bound(
			  begin( ), end( ), [&]( auto const &lhs, auto const &rhs ) {
				  return m_compare( lhs.first, rhs.first );
			  } );
		}

		template<typename K>
		constexpr const_iterator lower_bound( K const &x ) const {
			return daw::algorithm::lower_bound(
			  begin( ), end( ), [&]( auto const &lhs, auto const &rhs ) {
				  return m_compare( lhs.first, rhs.first );
			  } );
		}

		template<typename P, daw::enable_when_t<
		                       std::is_constructible_v<value_type, P &&>> = nullptr>
		constexpr std::pair<iterator, bool> insert( P &&value ) {
			auto pos = find( std::get<0>( value ) );
			if( pos == end( ) ) {
				pos = m_values.insert(
				  pos, daw::construct_a<value_type>( std::forward<P>( value ) ) );
				return {pos, true};
			}
			return {pos, false};
		}

		constexpr std::pair<iterator, bool> insert( value_type const &value ) {
			auto pos = find( value.first );
			if( pos == end( ) ) {
				pos = m_values.insert( pos, value );
				return {pos, true};
			}
			return {pos, false};
		}

		template<typename K>
		constexpr reference operator[]( K const &key ) {
			auto pos = find( key );
			if( pos == end( ) ) {
				pos = insert( value_type( key, mapped_type{} ) ).first;
			}
			return pos->second;
		}

		template<typename K>
		constexpr reference at( K const &key ) {
			auto pos = find( key );
			if( pos == end( ) ) {
				throw std::out_of_range( "key" );
			}
			return pos->second;
		}

		template<typename K>
		constexpr const_reference at( K const &key ) const {
			auto pos = find( key );
			if( pos == end( ) ) {
				throw std::out_of_range( "key" );
			}
			return pos->second;
		}
	};
} // namespace daw
