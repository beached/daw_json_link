// The MIT License (MIT)
//
// Copyright (c) 2014-2020 Darrell Wright
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
#include <cassert>
#include <iterator>
#include <numeric>
#include <vector>

#include "daw_algorithm.h"
#include "daw_traits.h"
#include "daw_utility.h"

namespace daw {
	namespace range {
		template<typename Iterator>
		struct Range {
			using value_type = typename std::iterator_traits<Iterator>::value_type;
			using reference = typename std::iterator_traits<Iterator>::reference;
			using const_reference = const reference;
			using iterator = Iterator;
			using const_iterator = const iterator;
			using difference_type =
			  typename std::iterator_traits<Iterator>::difference_type;

		private:
			iterator m_begin;
			iterator m_end;

		public:
			Range( ) = default;
			Range( Range const & ) = default;
			Range( Range && ) = default;
			~Range( ) = default;
			Range &operator=( Range const & ) = default;
			Range &operator=( Range && ) = default;

			Range( Iterator First, Iterator Last )
			  : m_begin( First )
			  , m_end( Last ) {}

			Range &move_next( ) {
				assert( m_begin != m_end );
				++m_begin;
				return *this;
			}

			Range &move_back( ) {
				--m_begin;
				return *this;
			}

			Range &move_back( Iterator start ) {
				assert( m_begin > start );
				--m_begin;
				return *this;
			}

			bool at_end( ) const {
				return !( m_begin != m_end );
			}

			bool empty( ) const {
				return !( m_begin != m_end );
			}

			explicit operator bool( ) const {
				return !empty( );
			}

			iterator begin( ) {
				return m_begin;
			}

			void advance( difference_type n ) {
				daw::advance( m_begin, n );
			}

			void set_begin( iterator i ) {
				m_begin = i;
			}

			iterator end( ) {
				return m_end;
			}

			void set_end( iterator i ) {
				m_end = i;
			}

			void set( iterator First, iterator Last ) {
				m_begin = First;
				m_end = Last;
			}

			const_iterator begin( ) const {
				return m_begin;
			}

			const_iterator end( ) const {
				return m_end;
			}

			const_iterator cbegin( ) const {
				return m_begin;
			}

			const_iterator cend( ) const {
				return m_end;
			}

			reference front( ) {
				return *m_begin;
			}

			const_reference front( ) const {
				return *m_begin;
			}

			reference back( ) {
				auto it = m_begin;
				daw::advance( it, size( ) - 1 );
				return *it;
			}

			const_reference back( ) const {
				auto it = m_begin;
				daw::advance( it, size( ) - 1 );
				return *it;
			}

			reference operator*( ) {
				return *m_begin;
			}

			const_reference operator*( ) const {
				return *m_begin;
			}

			size_t size( ) const {
				return static_cast<size_t>( daw::distance( m_begin, m_end ) );
			}

			reference operator[]( size_t pos ) {
				return *( m_begin + pos );
			}

			const_reference operator[]( size_t pos ) const {
				return *( m_begin + pos );
			}

			bool operator==( Range const &other ) const {
				return std::equal( m_begin, m_end, other.m_begin, other.m_end );
			}

			bool operator!=( Range const &other ) const {
				return !std::equal( m_begin, m_end, other.m_begin, other.m_end );
			}

			auto sort( ) {
				return make_range_reference( *this ).sort( );
			}

			template<typename UnaryPredicate>
			auto sort( UnaryPredicate predicate ) {
				return make_range_reference( *this ).sort( predicate );
			}

			auto unique( ) {
				return make_range_reference( *this ).unique( );
			}

			template<typename UnaryPredicate>
			auto unique( UnaryPredicate predicate ) {
				return make_range_reference( *this ).unique( predicate );
			}

			auto stable_sort( ) {
				return make_range_reference( *this ).stable_sort( );
			}

			template<typename UnaryPredicate>
			auto stable_sort( UnaryPredicate predicate ) {
				return make_range_reference( *this ).stable_sort( predicate );
			}

			template<typename Value>
			iterator find( Value const &value ) const {
				return std::find( m_begin, m_end, value );
			}

			template<typename UnaryPredicate>
			iterator find_if( UnaryPredicate predicate ) const {
				return std::find_if( m_begin, m_end, predicate );
			}

			template<typename Value>
			auto where_equal_to( Value const &value ) {
				return make_range_reference( *this ).where_equal_to( value );
			}

			template<typename UnaryPredicate>
			auto where( UnaryPredicate predicate ) {
				return make_range_reference( *this ).where( predicate );
			}

			template<typename Value>
			auto erase_where_equal_to( Value const &value ) {
				return make_range_reference( *this ).erase_equal_to( value );
			}

			template<typename UnaryPredicate>
			auto erase( UnaryPredicate predicate ) {
				return make_range_reference( *this ).erase( predicate );
			}

			template<typename UnaryPredicate>
			auto partition( UnaryPredicate predicate ) {
				return make_range_reference( *this ).partition( predicate );
			}

			template<typename UnaryPredicate>
			auto stable_partition( UnaryPredicate predicate ) {
				return make_range_reference( *this ).stable_partition( predicate );
			}

			template<typename T>
			T accumulate( T &&init ) {
				return std::accumulate( m_begin, m_end, std::forward<T>( init ) );
			}

			template<typename T, typename BinaryOperator>
			T accumulate( T &&init, BinaryOperator oper ) {
				return std::accumulate( m_begin, m_end, std::forward<T>( init ), oper );
			}

			template<typename UnaryOperator>
			auto
			map( UnaryOperator oper ) { // TODO verify result shouldn't be ref range
				using v_t = decltype( oper( *begin( ) ) );
				using result_t = std::vector<v_t>;
				result_t result;
				std::transform( begin( ), end( ), std::back_inserter( result ), oper );
				return result;
			}

			template<typename Value>
			bool contains( Value const &value ) const {
				return std::find( m_begin, m_end, value ) != m_end;
			}

			template<typename Value, typename UnaryPredicate>
			bool contains( Value const &value, UnaryPredicate predicate ) {
				auto predicate2 = [&value, &predicate]( Value const &val ) {
					return predicate( value, val );
				};
				return std::find_if( m_begin, m_end, predicate2 ) != m_end;
			}

			auto shuffle( ) {
				return make_range_reference( *this ).shuffle( );
			}

			template<typename UniformRandomNumberGenerator>
			auto shuffle( UniformRandomNumberGenerator &&urng ) {
				return make_range_reference( *this ).shuffle(
				  std::forward<UniformRandomNumberGenerator>( urng ) );
			}

			auto slice( size_t first_pos ) {
				using daw::next;
				return Range{next( m_begin, first_pos ), next( m_begin, size( ) )};
			}

			auto slice( size_t first_pos ) const {
				using daw::next;
				return Range{next( m_begin, first_pos ), next( m_begin, size( ) )};
			}

			auto slice( size_t first_pos, size_t last_pos ) {
				using daw::next;
				return Range{next( m_begin, first_pos ), next( m_begin, last_pos )};
			}

			auto slice( size_t first_pos, size_t last_pos ) const {
				using daw::next;
				return Range{next( m_begin, first_pos ), next( m_begin, last_pos )};
			}

			auto shrink( size_t new_size ) {
				using daw::next;
				return Range( m_begin, next( m_begin, new_size + 1 ) );
			}

			auto shrink( size_t new_size ) const {
				using daw::next;
				return Range( m_begin, next( m_begin, new_size + 1 ) );
			}

			std::vector<value_type> as_vector( ) const {
				std::vector<value_type> result;
				std::copy( m_begin, m_end, std::back_inserter( result ) );
				return result;
			}

			std::string to_hex_string( ) const {
				std::string result;
				daw::hex( m_begin, m_end, std::back_inserter( result ) );
				return result;
			}
		}; // struct Range

		template<typename Iterator>
		Range<Iterator> make_range( Iterator first, Iterator last ) {
			using iterator = typename std::decay_t<Iterator>;
			return Range<iterator>{first, last};
		}

		template<
		  typename Container,
		  typename std::enable_if<daw::traits::is_container_not_string_v<Container>,
		                          long>::type = 0>
		auto make_range( Container &container ) {
			using Iterator = std::decay_t<decltype( std::begin( container ) )>;
			return Range<Iterator>( std::begin( container ), std::end( container ) );
		}

		template<
		  typename Container,
		  typename std::enable_if<daw::traits::is_container_not_string_v<Container>,
		                          long>::type = 0>
		auto make_crange( Container const &container ) {
			using Iterator = std::decay_t<decltype( std::begin( container ) )>;
			return Range<Iterator>( std::begin( container ), std::end( container ) );
		}

		template<typename Iterator>
		void safe_advance(
		  Range<Iterator> &range,
		  typename std::iterator_traits<Iterator>::difference_type count ) {
			assert( 0 <= count );
			if( daw::distance( range.begin( ), range.end( ) ) >= count ) {
				range.advance( count );
			} else {
				range.set_begin( range.end( ) );
			}
		}

		template<typename Iterator>
		bool
		contains( Range<Iterator> const &range,
		          typename std::iterator_traits<Iterator>::value_type const &key ) {
			return std::find( range.begin( ), range.end( ), key ) != range.end( );
		}

		template<typename Iterator>
		bool at_end( Range<Iterator> const &range ) {
			return range.begin( ) == range.end( );
		}

		template<typename Iterator>
		auto next( Range<Iterator> rng, size_t n = 1 ) {
			rng.advance( n );
			return rng;
		}
	} // namespace range
} // namespace daw
