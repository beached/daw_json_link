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

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <ios>
#include <iterator>
#include <limits>
#include <stdexcept>
#ifndef NOSTRING
#include <string>
#endif
#include <utility>
#include <vector>

#include "impl/daw_string_impl.h"

#include "daw_algorithm.h"
#include "daw_bounded_vector.h"
#include "daw_container_algorithm.h"
#include "daw_exception.h"
#include "daw_fnv1a_hash.h"
#include "daw_generic_hash.h"
#include "daw_move.h"
#include "daw_string_view.h"
#include "daw_traits.h"
#include "iterator/daw_back_inserter.h"
#include "iterator/daw_iterator.h"

namespace daw {
	template<typename CharT, size_t Capacity>
	struct basic_bounded_string {
		using value_type = CharT;
		using pointer = CharT *;
		using const_pointer = value_type const *;
		using reference = value_type &;
		using const_reference = value_type const &;
		using const_iterator = const_pointer;
		using iterator = const_iterator;
		using reverse_iterator = std::reverse_iterator<iterator>;
		using const_reverse_iterator = std::reverse_iterator<const_iterator>;
		using size_type = size_t;
		using difference_type = std::ptrdiff_t;
		static constexpr size_t const extent = Capacity;

	private:
		daw::bounded_vector_t<CharT, Capacity + 1> m_data{};

	public:
		static constexpr size_type const npos =
		  std::numeric_limits<size_type>::max( );

		// Constructors

		constexpr basic_bounded_string( ) noexcept = default;

		constexpr basic_bounded_string( std::nullptr_t ) noexcept = delete;
		constexpr basic_bounded_string( std::nullptr_t,
		                                size_type ) noexcept = delete;

		constexpr basic_bounded_string( const_pointer str_ptr, size_type count )
		  : m_data( str_ptr, count ) {}

		template<size_t N>
		constexpr basic_bounded_string( basic_bounded_string<CharT, N> str,
		                                size_type count )
		  : m_data( str.data( ), daw::min( count, N ) ) {}

#ifndef NOSTRING
		template<typename Tr, typename Al>
		explicit basic_bounded_string(
		  std::basic_string<CharT, Tr, Al> const &str ) noexcept
		  : m_data( str.data( ), static_cast<size_type>( str.size( ) ) ) {}
#endif
		// TODO: determine if I want this or not
		// basic_bounded_string( std::basic_string<CharT> &&str ) noexcept =
		// delete;

		constexpr basic_bounded_string( const_pointer str_ptr ) noexcept
		  : m_data( str_ptr, details::strlen<size_type>( str_ptr ) ) {}

		template<size_t Len>
		constexpr basic_bounded_string( value_type ( &str_ptr )[Len] ) noexcept
		  : m_data( str_ptr, Len - 1 ) {}

		// End of Construtors
#ifndef NOSTRING
		explicit operator std::basic_string<CharT>( ) const {
			return to_string( );
		}
#endif
		template<typename Bound = daw::default_string_view_bounds_type>
		constexpr operator daw::basic_string_view<CharT, Bound>( ) const {
			return daw::basic_string_view<CharT, Bound>( m_data.data( ),
			                                             m_data.size( ) );
		}

		constexpr bool full( ) noexcept {
			return m_data.size( ) == capacity( );
		}

		constexpr bool has_room( size_type count ) noexcept {
			return m_data.has_room( count + 1 );
		}

		constexpr void push_back( value_type c ) {
			daw::exception::precondition_check<std::out_of_range>(
			  !full( ), "Attempt to push_back basic_bounded_string past end" );

			m_data.push_back( c );
			*m_data.end( ) = 0;
		}

		constexpr basic_bounded_string &append( value_type c ) {
			daw::exception::precondition_check<std::out_of_range>(
			  !m_data.full( ), "Attempt to append basic_bounded_string past end" );

			push_back( c );
			return *this;
		}

		constexpr basic_bounded_string &append( basic_string_view<CharT> sv ) {
			daw::exception::precondition_check<std::out_of_range>(
			  m_data.size( ) + sv.size( ) <= capacity( ),
			  "Attempt to append basic_bounded_string past end" );

			daw::container::copy( sv, daw::back_inserter( m_data ) );
			*m_data.end( ) = 0;
			return *this;
		}

#ifndef NOSTRING
		basic_bounded_string &append( std::basic_string<CharT> const &str ) {
			daw::exception::precondition_check<std::out_of_range>(
			  m_data.size( ) + str.size( ) <= capacity( ),
			  "Attempt to append basic_bounded_string past end" );
			daw::container::copy( str, daw::back_inserter( m_data ) );
			*m_data.end( ) = 0;
			return *this;
		}

		constexpr basic_bounded_string &append( std::basic_string<CharT> &&str ) {
			daw::exception::precondition_check<std::out_of_range>(
			  m_data.size( ) + str.size( ) <= capacity( ),
			  "Attempt to append basic_bounded_string past end" );
			daw::container::copy( str, daw::back_inserter( m_data ) );
			*m_data.end( ) = 0;
			return *this;
		}
#endif

		template<typename Iterator,
		         std::enable_if_t<traits::is_iterator_v<Iterator>, std::nullptr_t> =
		           nullptr>
		constexpr basic_bounded_string &append( Iterator first,
		                                        Iterator const last ) {

			daw::exception::precondition_check<std::out_of_range>(
			  m_data.size( ) + static_cast<size_t>( daw::distance( first, last ) ) <=
			    capacity( ),
			  "Attempt to append basic_bounded_string past end" );
			daw::algorithm::copy( first, last, daw::back_inserter( m_data ) );
			*m_data.end( ) = 0;
			return *this;
		}

		template<typename T>
		constexpr basic_bounded_string &
		append( T const &t, size_type pos,
		        size_type count = basic_bounded_string::npos ) {
			daw::basic_string_view<CharT> tmp = t;
			return append( tmp.substr( pos, count ) );
		}

#ifndef NOSTRING
		template<typename TrtsT, typename Allocator>
		basic_bounded_string &operator=(
		  std::basic_string<CharT, TrtsT, Allocator> const &str ) noexcept {
			m_data = basic_bounded_string( str.data( ), str.size( ) );
			return *this;
		}
#endif
		constexpr iterator begin( ) noexcept {
			return m_data.begin( );
		}

		constexpr const_iterator begin( ) const noexcept {
			return m_data.begin( );
		}

		constexpr const_iterator cbegin( ) const noexcept {
			return m_data.cbegin( );
		}

		constexpr iterator end( ) noexcept {
			return m_data.end( );
		}

		constexpr const_iterator end( ) const noexcept {
			return m_data.end( );
		}

		constexpr const_iterator cend( ) const noexcept {
			return m_data.cend( );
		}

		constexpr reverse_iterator rbegin( ) noexcept {
			return reverse_iterator( m_data.end( ) );
		}

		constexpr const_reverse_iterator rbegin( ) const noexcept {
			return reverse_iterator( end( ) );
		}

		constexpr const_reverse_iterator crbegin( ) const noexcept {
			return const_reverse_iterator( cend( ) );
		}

		constexpr reverse_iterator rend( ) noexcept {
			return reverse_iterator( begin( ) );
		}

		constexpr const_reverse_iterator rend( ) const noexcept {
			return const_reverse_iterator( begin( ) );
		}

		constexpr const_reverse_iterator crend( ) const noexcept {
			return const_reverse_iterator( cbegin( ) );
		}

		constexpr reference operator[]( size_type const pos ) noexcept {
			return m_data[pos];
		}

		constexpr const_reference operator[]( size_type const pos ) const noexcept {
			return m_data[pos];
		}

		constexpr reference at( size_type const pos ) {
			return m_data.at( pos );
		}

		constexpr const_reference at( size_type const pos ) const {
			return m_data.at( pos );
		}

		constexpr reference front( ) noexcept {
			return m_data.front( );
		}

		constexpr const_reference front( ) const noexcept {
			return m_data.front( );
		}

		constexpr reference back( ) noexcept {
			return m_data.back( );
		}

		constexpr const_reference back( ) const noexcept {
			return m_data.back( );
		}

		constexpr pointer data( ) noexcept {
			return m_data.data( );
		}

		constexpr const_pointer data( ) const noexcept {
			return m_data.data( );
		}

		constexpr const_pointer c_str( ) const noexcept {
			return m_data.data( );
		}

		constexpr size_type capacity( ) const noexcept {
			return Capacity;
		}

		constexpr size_type size( ) const noexcept {
			return m_data.size( );
		}

		constexpr size_type length( ) const noexcept {
			return m_data.size( );
		}

		constexpr size_type max_size( ) const noexcept {
			return Capacity;
		}

		constexpr bool empty( ) const noexcept {
			return m_data.empty( );
		}

		constexpr explicit operator bool( ) const noexcept {
			return !m_data.empty( );
		}

		template<size_t NewSize>
		constexpr operator basic_bounded_string<CharT, NewSize>( ) const noexcept {
			return {data( ), size( )};
		}

		constexpr void remove_suffix( size_type n ) {
			m_data.resize( m_data.size( ) - n );
			*m_data.end( ) = 0;
		}

		constexpr void remove_suffix( ) {
			remove_suffix( 1 );
		}

		constexpr void clear( ) noexcept {
			m_data.clear( );
			*m_data.end( ) = 0;
		}

		constexpr CharT pop_back( ) noexcept {
			auto result = m_data.pop_back( );
			*m_data.end( ) = 0;
			return result;
		}

		constexpr void swap( basic_bounded_string &v ) noexcept {
			swap( m_data, v.m_data );
		}

		constexpr size_type copy( CharT *dest, size_type const count,
		                          size_type const pos = 0 ) const {
			daw::exception::precondition_check<std::out_of_range>(
			  pos < size( ), "Attempt to access basic_bounded_string past end" );

			size_type rlen = daw::min( count, size( ) - pos );
			daw::algorithm::copy_n( m_data.data( ) + pos, dest, rlen );
			return rlen;
		}

		constexpr basic_bounded_string
		substr( size_type const pos = 0, size_type const count = npos ) const {
			daw::exception::precondition_check<std::out_of_range>(
			  pos < size( ), "Attempt to access basic_bounded_string past end" );

			auto const rcount =
			  static_cast<size_type>( daw::min( count, size( ) - pos ) );
			return basic_bounded_string{&m_data[pos], rcount};
		}

		template<size_t CapL, size_t CapR>
		static constexpr int
		compare( basic_bounded_string<CharT, CapL> const &lhs,
		         basic_bounded_string<CharT, CapR> const &rhs ) noexcept {
			auto const str_compare = []( CharT const *p0, CharT const *p1,
			                             size_t len ) {
				auto const last = p0 + len;
				while( p0 != last ) {
					if( *p0 != *p1 ) {
						if( *p0 < *p1 ) {
							return -1;
						}
						return 1;
					}
					++p0;
					++p1;
				}
				return 0;
			};

			auto cmp = str_compare( lhs.data( ), rhs.data( ),
			                        daw::min( lhs.size( ), rhs.size( ) ) );
			if( cmp == 0 ) {
				if( lhs.size( ) < rhs.size( ) ) {
					return -1;
				}
				if( rhs.size( ) < lhs.size( ) ) {
					return 1;
				}
			}
			return cmp;
		}

		template<size_t CapL>
		static constexpr int compare( basic_bounded_string<CharT, CapL> const &lhs,
		                              CharT const *rhs_ptr,
		                              size_t rhs_size ) noexcept {
			auto const str_compare = []( CharT const *p0, CharT const *p1,
			                             size_t len ) {
				auto const last = p0 + len;
				while( p0 != last ) {
					if( *p0 != *p1 ) {
						if( *p0 < *p1 ) {
							return -1;
						}
						return 1;
					}
					++p0;
					++p1;
				}
				return 0;
			};

			auto cmp =
			  str_compare( lhs.data( ), rhs_ptr, daw::min( lhs.size( ), rhs_size ) );
			if( cmp == 0 ) {
				if( lhs.size( ) < rhs_size ) {
					return -1;
				}
				if( rhs_size < lhs.size( ) ) {
					return 1;
				}
			}
			return cmp;
		}

		template<size_t CapR>
		constexpr int
		compare( basic_bounded_string<CharT, CapR> const &rhs ) const {
			return compare( *this, rhs );
		}

		constexpr int compare( basic_string_view<CharT> rhs ) const {
			return compare( *this, rhs.data( ), rhs.size( ) );
		}

#ifndef NOSTRING
		int compare( std::basic_string<CharT> const &rhs ) const {
			return compare( *this, rhs.data( ), rhs.size( ) );
		}
#endif

		template<size_t CapR>
		constexpr int compare( size_type const pos1, size_type const count1,
		                       basic_bounded_string<CharT, CapR> const v ) const {
			return substr( pos1, count1 ).compare( v );
		}

		template<size_t CapR>
		constexpr int compare( size_type const pos1, size_type const count1,
		                       basic_bounded_string<CharT, CapR> const v,
		                       size_type const pos2,
		                       size_type const count2 ) const {
			return substr( pos1, count1 ).compare( v.substr( pos2, count2 ) );
		}

		constexpr int compare( const_pointer s ) const noexcept {
			// TODO fix not best way
			basic_string_view<CharT> rhs( s );
			return basic_bounded_string::compare( *this, rhs.data( ), rhs.size( ) );
		}

		constexpr int compare( size_type const pos1, size_type const count1,
		                       const_pointer s ) const {
			return substr( pos1, count1 ).compare( s );
		}

		constexpr int compare( size_type const pos1, size_type const count1,
		                       const_pointer s, size_type const count2 ) const {
			return basic_bounded_string::compare( substr( pos1, count1 ), s, count2 );
		}

		constexpr size_type find( basic_bounded_string const v,
		                          size_type const pos = 0 ) const noexcept {
			return daw::string_view{data( ), size( )}.find(
			  daw::string_view{v.data( ), v.size( )}, pos );
		}

		constexpr size_type find( value_type const c,
		                          size_type const pos = 0 ) const noexcept {
			return find( basic_bounded_string{&c, 1}, pos );
		}

		constexpr size_type find( const_pointer s, size_type const pos,
		                          size_type const count ) const noexcept {
			return find( basic_bounded_string{s, count}, pos );
		}

		constexpr size_type find( const_pointer s, size_type const pos = 0 ) const
		  noexcept {
			return find( basic_bounded_string{s}, pos );
		}

		constexpr size_type rfind( basic_bounded_string const v,
		                           size_type pos = npos ) const noexcept {
			return daw::string_view{data( ), size( )}.rfind(
			  daw::string_view{v.data( ), v.size( )}, pos );
		}

		constexpr size_type rfind( value_type const c,
		                           size_type const pos = npos ) const noexcept {
			return rfind( basic_bounded_string{&c, 1}, pos );
		}

		constexpr size_type rfind( const_pointer s, size_type const pos,
		                           size_type const count ) const noexcept {
			return rfind( basic_bounded_string{s, count}, pos );
		}

		constexpr size_type rfind( const_pointer s,
		                           size_type const pos = npos ) const noexcept {
			return rfind( basic_bounded_string{s}, pos );
		}

		constexpr size_type find_first_of( basic_bounded_string const v,
		                                   size_type const pos = 0 ) const
		  noexcept {
			return daw::string_view{data( ), size( )}.find_first_of(
			  daw::string_view{v.data( ), v.size( )}, pos );
		}

		constexpr size_type search( basic_bounded_string const v,
		                            size_type const pos = 0 ) const noexcept {
			return daw::string_view{data( ), size( )}.search(
			  daw::string_view{v.data( ), v.size( )}, pos );
		}

		constexpr size_type search_last( basic_bounded_string const v,
		                                 size_type const pos = 0 ) const noexcept {
			return daw::string_view{data( ), size( )}.search_last(
			  daw::string_view{v.data( ), v.size( )}, pos );
		}

		template<typename UnaryPredicate>
		constexpr size_type find_first_of_if( UnaryPredicate pred,
		                                      size_type const pos = 0 ) const
		  noexcept( noexcept(
		    std::declval<UnaryPredicate>( )( std::declval<value_type>( ) ) ) ) {

			static_assert( traits::is_unary_predicate_v<UnaryPredicate, CharT>,
			               "UnaryPredicate p does not fullfill the requires of a "
			               "unary predicate concept.  See "
			               "http://en.cppreference.com/w/cpp/concept/Predicate" );

			return daw::string_view{data( ), size( )}.find_first_of_if( pred, pos );
		}

		template<typename UnaryPredicate>
		constexpr size_type find_first_not_of_if( UnaryPredicate pred,
		                                          size_type const pos = 0 ) const
		  noexcept( noexcept(
		    std::declval<UnaryPredicate>( )( std::declval<value_type>( ) ) ) ) {

			static_assert( traits::is_unary_predicate_v<UnaryPredicate, CharT>,
			               "UnaryPredicate p does not fullfill the requires of a "
			               "unary predicate concept.  See "
			               "http://en.cppreference.com/w/cpp/concept/Predicate" );

			return daw::string_view{data( ), size( )}.find_first_not_of_if( pred,
			                                                                pos );
		}

		constexpr size_type find_first_of( value_type c,
		                                   size_type const pos = 0 ) const
		  noexcept {
			return find_first_of( basic_bounded_string{&c, 1}, pos );
		}

		constexpr size_type find_first_of( const_pointer s, size_type pos,
		                                   size_type const count ) const noexcept {
			return find_first_of( basic_bounded_string{s, count}, pos );
		}

		constexpr size_type find_first_of( const_pointer s,
		                                   size_type const pos = 0 ) const
		  noexcept {
			return find_first_of( basic_bounded_string{s}, pos );
		}

#ifndef NOSTRING
		std::basic_string<value_type> to_string( ) const {
			std::basic_string<value_type> result;
			result.reserve( size( ) );
			daw::algorithm::copy_n( data( ), daw::back_inserter( result ), size( ) );
			return result;
		}
#endif
	private:
		constexpr size_type reverse_distance( const_reverse_iterator first,
		                                      const_reverse_iterator last ) const
		  noexcept {
			// Portability note here: std::distance is not NOEXCEPT, but calling it
			// with a bounded_string::reverse_iterator will not throw.
			return ( size( ) - 1u ) -
			       static_cast<size_t>( daw::distance( first, last ) );
		}

	public:
		constexpr size_type find_last_of( basic_bounded_string s,
		                                  size_type pos = npos ) const noexcept {
			return daw::string_view{data( ), size( )}.find_last_of(
			  daw::string_view{s.data( ), s.size( )}, pos );
		}

		constexpr size_type find_last_of( value_type const c,
		                                  size_type pos = npos ) const noexcept {
			return daw::string_view{data( ), size( )}.find_last_of( c, pos );
		}

		template<size_type N>
		constexpr size_type find_last_of( CharT const ( &s )[N],
		                                  size_type pos ) noexcept {
			return find_last_of( basic_bounded_string{s, N - 1}, pos );
		}

		template<size_type N>
		constexpr size_type find_last_of( CharT const ( &s )[N] ) noexcept {
			return find_last_of( basic_bounded_string{s, N - 1}, npos );
		}

		constexpr size_type find_last_of( const_pointer s, size_type pos,
		                                  size_type count ) const noexcept {
			return find_last_of( basic_bounded_string{s, count}, pos );
		}

		constexpr size_type find_last_of( const_pointer s, size_type pos ) const
		  noexcept {
			return find_last_of( basic_bounded_string{s}, pos );
		}

		constexpr size_type find_last_of( const_pointer s ) const noexcept {
			return find_last_of( basic_bounded_string{s}, npos );
		}

		constexpr size_type find_first_not_of( basic_bounded_string v,
		                                       size_type pos = 0 ) const noexcept {
			return daw::string_view{data( ), size( )}.find_first_not_of(
			  daw::string_view{v.data( ), v.size( )}, pos );
		}

		constexpr size_type find_first_not_of( value_type c,
		                                       size_type pos = 0 ) const noexcept {
			return find_first_not_of( basic_bounded_string{&c, 1}, pos );
		}

		constexpr size_type find_first_not_of( const_pointer s, size_type pos,
		                                       size_type count ) const noexcept {
			return find_first_not_of( basic_bounded_string{s, count}, pos );
		}

		constexpr size_type find_first_not_of( const_pointer s,
		                                       size_type pos = 0 ) const noexcept {
			return find_first_not_of( basic_bounded_string{s}, pos );
		}

		constexpr size_type find_last_not_of( basic_bounded_string v,
		                                      size_type pos = npos ) const
		  noexcept {
			return daw::string_view{data( ), size( )}.find_last_not_of(
			  daw::string_view{v.data( ), v.size( )}, pos );
		}

		constexpr size_type find_last_not_of( value_type c,
		                                      size_type pos = npos ) const
		  noexcept {
			return find_last_not_of( basic_bounded_string{&c, 1}, pos );
		}

		constexpr size_type find_last_not_of( const_pointer s, size_type pos,
		                                      size_type count ) const noexcept {
			return find_last_not_of( basic_bounded_string{s, count}, pos );
		}

		constexpr size_type find_last_not_of( const_pointer s,
		                                      size_type pos = npos ) const
		  noexcept {
			return find_last_not_of( basic_bounded_string{s}, pos );
		}

		constexpr bool starts_with( value_type c ) const noexcept {
			if( empty( ) ) {
				return false;
			}
			return front( ) == c;
		}

		constexpr bool starts_with( basic_bounded_string s ) const noexcept {
			return daw::string_view{data( ), size( )}.starts_with(
			  daw::string_view{s.data( ), s.size( )} );
		}

		constexpr bool starts_with( const_pointer s ) const noexcept {
			return starts_with( basic_bounded_string{s} );
		}

		constexpr bool ends_with( value_type c ) const noexcept {
			if( empty( ) ) {
				return false;
			}
			return back( ) == c;
		}

		constexpr bool ends_with( basic_bounded_string s ) const noexcept {
			return daw::string_view{data( ), size( )}.ends_with(
			  daw::string_view{s.data( ), s.size( )} );
		}

		constexpr bool ends_with( const_pointer s ) const noexcept {
			return ends_with( basic_bounded_string{s} );
		}

		constexpr iterator erase( const_iterator pos ) {
			return m_data.erase( pos );
		}

		constexpr iterator erase( const_iterator first, const_iterator last ) {
			return m_data.erase( first, last );
		}

		constexpr basic_bounded_string &erase( size_type index = 0,
		                                       size_type count = npos ) {

			auto const first =
			  std::next( begin( ), static_cast<difference_type>( index ) );
			auto const last =
			  count == npos
			    ? end( )
			    : std::next( first, static_cast<difference_type>( count ) );

			m_data.erase( first, last );
			return *this;
		}
	}; // basic_bounded_string

	template<typename CharT, size_t N>
	basic_bounded_string( CharT const ( & )[N] )->basic_bounded_string<CharT, N>;

	template<typename... Chars,
	         std::enable_if_t<daw::traits::all_same_v<char, Chars...>,
	                          std::nullptr_t> = nullptr>
	basic_bounded_string( Chars... )
	  ->basic_bounded_string<char, sizeof...( Chars )>;

	template<typename CharT, size_t N>
	constexpr void swap( basic_bounded_string<CharT, N> &lhs,
	                     basic_bounded_string<CharT, N> &rhs ) noexcept {
		lhs.swap( rhs );
	}

	using bounded_string = basic_bounded_string<char, 100>;
	using wbounded_string = basic_bounded_string<wchar_t, 100>;
	using u16bounded_string = basic_bounded_string<char16_t, 100>;
	using u32bounded_string = basic_bounded_string<char32_t, 100>;

	template<size_t Capacity = 100, typename CharT>
	constexpr auto make_bounded_string_it( CharT const *first,
	                                       CharT const *last ) noexcept {
		return basic_bounded_string<CharT, Capacity>{
		  first, static_cast<size_t>( last - first )};
	}

	template<
	  size_t Capacity = 100, typename Iterator,
	  typename CharT = std::decay_t<decltype( *std::declval<Iterator>( ) )>>
	constexpr auto make_bounded_string_it( Iterator first,
	                                       Iterator last ) noexcept {
		using sv_t = basic_bounded_string<CharT, Capacity>;
		using size_type = typename sv_t::size_type;
		return sv_t{&( *first ),
		            static_cast<size_type>( daw::distance( first, last ) )};
	}

	template<size_t Capacity = 100, typename CharT>
	auto make_bounded_string( std::vector<CharT> const &v ) noexcept {
		return basic_bounded_string<CharT, Capacity>{v.data( ), v.size( )};
	}

#ifndef NOSTRING
	template<size_t Capacity = 100, typename CharT>
	daw::basic_bounded_string<CharT, Capacity>
	make_bounded_string( std::basic_string<CharT> const &str ) {
		return daw::basic_bounded_string<CharT, Capacity>{str};
	}
#endif

	template<size_t Capacity = 100, typename CharT, size_t N>
	daw::basic_bounded_string<CharT, Capacity>
	make_bounded_string( CharT const ( &str )[N] ) {
		return daw::basic_bounded_string<CharT, Capacity>{str, N};
	}

	// basic_bounded_string / basic_bounded_string
	//
	template<typename CharT, size_t CapacityL, size_t CapacityR>
	constexpr bool
	operator==( basic_bounded_string<CharT, CapacityL> const &lhs,
	            basic_bounded_string<CharT, CapacityR> const &rhs ) noexcept {

		return lhs.compare( rhs ) == 0;
	}

	template<typename CharT, size_t CapacityL, size_t CapacityR>
	constexpr bool
	operator!=( basic_bounded_string<CharT, CapacityL> const &lhs,
	            basic_bounded_string<CharT, CapacityR> const &rhs ) noexcept {

		return lhs.compare( rhs ) != 0;
	}

	template<typename CharT, size_t CapacityL, size_t CapacityR>
	constexpr bool
	operator<( basic_bounded_string<CharT, CapacityL> const &lhs,
	           basic_bounded_string<CharT, CapacityR> const &rhs ) noexcept {

		return lhs.compare( rhs ) < 0;
	}

	template<typename CharT, size_t CapacityL, size_t CapacityR>
	constexpr bool
	operator<=( basic_bounded_string<CharT, CapacityL> const &lhs,
	            basic_bounded_string<CharT, CapacityR> const &rhs ) noexcept {

		return lhs.compare( rhs ) <= 0;
	}

	template<typename CharT, size_t CapacityL, size_t CapacityR>
	constexpr bool
	operator>( basic_bounded_string<CharT, CapacityL> const &lhs,
	           basic_bounded_string<CharT, CapacityR> const &rhs ) noexcept {

		return lhs.compare( rhs ) > 0;
	}

	template<typename CharT, size_t CapacityL, size_t CapacityR>
	constexpr bool
	operator>=( basic_bounded_string<CharT, CapacityL> const &lhs,
	            basic_bounded_string<CharT, CapacityR> const &rhs ) noexcept {

		return lhs.compare( rhs ) >= 0;
	}

	// basic_bounded_string / basic_string_view
	//
	template<typename CharT, size_t Capacity>
	constexpr bool
	operator==( daw::basic_bounded_string<CharT, Capacity> const &lhs,
	            daw::basic_string_view<CharT> rhs ) noexcept {

		return lhs.compare( rhs ) == 0;
	}

	template<typename CharT, size_t Capacity>
	constexpr bool
	operator!=( daw::basic_bounded_string<CharT, Capacity> const &lhs,
	            daw::basic_string_view<CharT> rhs ) noexcept {

		return lhs.compare( rhs ) != 0;
	}

	template<typename CharT, size_t Capacity>
	constexpr bool
	operator<( daw::basic_bounded_string<CharT, Capacity> const &lhs,
	           daw::basic_string_view<CharT> rhs ) noexcept {

		return lhs.compare( rhs ) < 0;
	}

	template<typename CharT, size_t Capacity>
	constexpr bool
	operator<=( daw::basic_bounded_string<CharT, Capacity> const &lhs,
	            daw::basic_string_view<CharT> rhs ) noexcept {

		return lhs.compare( rhs ) <= 0;
	}

	template<typename CharT, size_t Capacity>
	constexpr bool
	operator>( daw::basic_bounded_string<CharT, Capacity> const &lhs,
	           daw::basic_string_view<CharT> rhs ) noexcept {

		return lhs.compare( rhs ) > 0;
	}

	template<typename CharT, size_t Capacity>
	constexpr bool
	operator>=( daw::basic_bounded_string<CharT, Capacity> const &lhs,
	            daw::basic_string_view<CharT> rhs ) noexcept {

		return lhs.compare( rhs ) >= 0;
	}

	// daw::basic_string_view / daw::basic_bounded_string
	//
	template<typename CharT, size_t Capacity>
	constexpr bool
	operator==( daw::basic_string_view<CharT> const &lhs,
	            daw::basic_bounded_string<CharT, Capacity> const &rhs ) noexcept {

		return rhs.compare( lhs ) == 0;
	}

	template<typename CharT, size_t Capacity>
	constexpr bool
	operator!=( daw::basic_string_view<CharT> const &lhs,
	            daw::basic_bounded_string<CharT, Capacity> const &rhs ) noexcept {

		return rhs.compare( lhs ) != 0;
	}

	template<typename CharT, size_t Capacity>
	constexpr bool
	operator<( daw::basic_string_view<CharT> const &lhs,
	           daw::basic_bounded_string<CharT, Capacity> const &rhs ) noexcept {

		return rhs.compare( lhs ) > 0;
	}

	template<typename CharT, size_t Capacity>
	constexpr bool
	operator<=( daw::basic_string_view<CharT> const &lhs,
	            daw::basic_bounded_string<CharT, Capacity> const &rhs ) noexcept {

		return rhs.compare( lhs ) >= 0;
	}

	template<typename CharT, size_t Capacity>
	constexpr bool
	operator>( daw::basic_string_view<CharT> const &lhs,
	           daw::basic_bounded_string<CharT, Capacity> const &rhs ) noexcept {

		return rhs.compare( lhs ) < 0;
	}

	template<typename CharT, size_t Capacity>
	constexpr bool
	operator>=( daw::basic_string_view<CharT> const &lhs,
	            daw::basic_bounded_string<CharT, Capacity> const &rhs ) noexcept {

		return rhs.compare( lhs ) <= 0;
	}

#ifndef NOSTRING
	// basic_bounded_string / basic_string
	//
	template<typename CharT, size_t Capacity>
	bool operator==( daw::basic_bounded_string<CharT, Capacity> const &lhs,
	                 std::basic_string<CharT> const &rhs ) noexcept {

		return lhs.compare( rhs ) == 0;
	}

	template<typename CharT, size_t Capacity>
	bool operator!=( daw::basic_bounded_string<CharT, Capacity> const &lhs,
	                 std::basic_string<CharT> const &rhs ) noexcept {

		return lhs.compare( rhs ) != 0;
	}

	template<typename CharT, size_t Capacity>
	bool operator<( daw::basic_bounded_string<CharT, Capacity> const &lhs,
	                std::basic_string<CharT> const &rhs ) noexcept {

		return lhs.compare( rhs ) < 0;
	}

	template<typename CharT, size_t Capacity>
	bool operator<=( daw::basic_bounded_string<CharT, Capacity> const &lhs,
	                 std::basic_string<CharT> const &rhs ) noexcept {

		return lhs.compare( rhs ) <= 0;
	}

	template<typename CharT, size_t Capacity>
	bool operator>( daw::basic_bounded_string<CharT, Capacity> const &lhs,
	                std::basic_string<CharT> const &rhs ) noexcept {

		return lhs.compare( rhs ) > 0;
	}

	template<typename CharT, size_t Capacity>
	bool operator>=( daw::basic_bounded_string<CharT, Capacity> const &lhs,
	                 std::basic_string<CharT> const &rhs ) noexcept {

		return lhs.compare( rhs ) >= 0;
	}

	// std::basic_string / daw::basic_bounded_string
	//
	template<typename CharT, size_t Capacity>
	constexpr bool
	operator==( std::basic_string<CharT> const &lhs,
	            daw::basic_bounded_string<CharT, Capacity> const &rhs ) noexcept {

		return rhs.compare( lhs ) == 0;
	}

	template<typename CharT, size_t Capacity>
	constexpr bool
	operator!=( std::basic_string<CharT> const &lhs,
	            daw::basic_bounded_string<CharT, Capacity> const &rhs ) noexcept {

		return rhs.compare( lhs ) != 0;
	}

	template<typename CharT, size_t Capacity>
	constexpr bool
	operator<( std::basic_string<CharT> const &lhs,
	           daw::basic_bounded_string<CharT, Capacity> const &rhs ) noexcept {

		return rhs.compare( lhs ) > 0;
	}

	template<typename CharT, size_t Capacity>
	constexpr bool
	operator<=( std::basic_string<CharT> const &lhs,
	            daw::basic_bounded_string<CharT, Capacity> const &rhs ) noexcept {

		return rhs.compare( lhs ) >= 0;
	}

	template<typename CharT, size_t Capacity>
	constexpr bool
	operator>( std::basic_string<CharT> const &lhs,
	           daw::basic_bounded_string<CharT, Capacity> const &rhs ) noexcept {

		return rhs.compare( lhs ) < 0;
	}

	template<typename CharT, size_t Capacity>
	constexpr bool
	operator>=( std::basic_string<CharT> const &lhs,
	            daw::basic_bounded_string<CharT, Capacity> const &rhs ) noexcept {

		return rhs.compare( lhs ) <= 0;
	}
#endif
	// daw::basic_bounded_string / char const *
	//
	template<typename CharT, size_t Capacity>
	constexpr bool
	operator==( daw::basic_bounded_string<CharT, Capacity> const &lhs,
	            typename daw::basic_bounded_string<CharT, Capacity>::const_pointer
	              rhs ) noexcept {
		return daw::basic_string_view<CharT>{lhs.data( ), lhs.size( )}.compare(
		         rhs ) == 0;
	}

	template<typename CharT, size_t Capacity>
	constexpr bool
	operator!=( daw::basic_bounded_string<CharT, Capacity> const &lhs,
	            typename daw::basic_bounded_string<CharT, Capacity>::const_pointer
	              rhs ) noexcept {
		return daw::basic_string_view<CharT>{lhs.data( ), lhs.size( )}.compare(
		         rhs ) != 0;
	}

	template<typename CharT, size_t Capacity>
	constexpr bool
	operator<( daw::basic_bounded_string<CharT, Capacity> const &lhs,
	           typename daw::basic_bounded_string<CharT, Capacity>::const_pointer
	             rhs ) noexcept {
		return daw::basic_string_view<CharT>{lhs.data( ), lhs.size( )}.compare(
		         rhs ) < 0;
	}

	template<typename CharT, size_t Capacity>
	constexpr bool
	operator<=( daw::basic_bounded_string<CharT, Capacity> const &lhs,
	            typename daw::basic_bounded_string<CharT, Capacity>::const_pointer
	              rhs ) noexcept {
		return daw::basic_string_view<CharT>{lhs.data( ), lhs.size( )}.compare(
		         rhs ) <= 0;
	}

	template<typename CharT, size_t Capacity>
	constexpr bool
	operator>( daw::basic_bounded_string<CharT, Capacity> const &lhs,
	           typename daw::basic_bounded_string<CharT, Capacity>::const_pointer
	             rhs ) noexcept {
		return daw::basic_string_view<CharT>{lhs.data( ), lhs.size( )}.compare(
		         rhs ) > 0;
	}

	template<typename CharT, size_t Capacity>
	constexpr bool
	operator>=( daw::basic_bounded_string<CharT, Capacity> lhs,
	            typename daw::basic_bounded_string<CharT, Capacity>::const_pointer
	              rhs ) noexcept {
		return daw::basic_string_view<CharT>{lhs.data( ), lhs.size( )}.compare(
		         rhs ) >= 0;
	}

	// char const * / daw::basic_bounded_string
	//
	template<typename CharT, size_t Capacity>
	constexpr bool operator==(
	  typename daw::basic_bounded_string<CharT, Capacity>::const_pointer lhs,
	  daw::basic_bounded_string<CharT, Capacity> const &rhs ) noexcept {
		using sv_t = daw::basic_string_view<CharT>;
		return sv_t( lhs ).compare( static_cast<sv_t>( rhs ) ) == 0;
	}

	template<typename CharT, size_t Capacity>
	constexpr bool operator!=(
	  typename daw::basic_bounded_string<CharT, Capacity>::const_pointer lhs,
	  daw::basic_bounded_string<CharT, Capacity> const &rhs ) noexcept {
		using sv_t = daw::basic_string_view<CharT>;
		return sv_t( lhs ).compare( static_cast<sv_t>( rhs ) ) != 0;
	}

	template<typename CharT, size_t Capacity>
	constexpr bool operator<(
	  typename daw::basic_bounded_string<CharT, Capacity>::const_pointer lhs,
	  daw::basic_bounded_string<CharT, Capacity> const &rhs ) noexcept {
		using sv_t = daw::basic_string_view<CharT>;
		return sv_t( lhs ).compare( static_cast<sv_t>( rhs ) ) < 0;
	}

	template<typename CharT, size_t Capacity>
	constexpr bool operator<=(
	  typename daw::basic_bounded_string<CharT, Capacity>::const_pointer lhs,
	  daw::basic_bounded_string<CharT, Capacity> const &rhs ) noexcept {
		using sv_t = daw::basic_string_view<CharT>;
		return sv_t( lhs ).compare( static_cast<sv_t>( rhs ) ) <= 0;
	}

	template<typename CharT, size_t Capacity>
	constexpr bool operator>(
	  typename daw::basic_bounded_string<CharT, Capacity>::const_pointer lhs,
	  daw::basic_bounded_string<CharT, Capacity> const &rhs ) noexcept {
		using sv_t = daw::basic_string_view<CharT>;
		return sv_t( lhs ).compare( static_cast<sv_t>( rhs ) ) > 0;
	}

	template<typename CharT, size_t Capacity>
	constexpr bool operator>=(
	  typename daw::basic_bounded_string<CharT, Capacity>::const_pointer lhs,
	  daw::basic_bounded_string<CharT, Capacity> const &rhs ) noexcept {
		using sv_t = daw::basic_string_view<CharT>;
		return sv_t( lhs ).compare( static_cast<sv_t>( rhs ) ) >= 0;
	}

	// daw::basic_bounded_string / char const[N]
	//
	template<typename CharT, size_t Capacity, size_t N>
	constexpr bool
	operator==( daw::basic_bounded_string<CharT, Capacity> const &lhs,
	            CharT const ( &rhs )[N] ) noexcept {
		return daw::basic_string_view<CharT>{lhs.data( ), lhs.size( )}.compare(
		         rhs ) == 0;
	}

	template<typename CharT, size_t Capacity, size_t N>
	constexpr bool
	operator!=( daw::basic_bounded_string<CharT, Capacity> const &lhs,
	            CharT const ( &rhs )[N] ) noexcept {
		return daw::basic_string_view<CharT>{lhs.data( ), lhs.size( )}.compare(
		         rhs ) != 0;
	}

	template<typename CharT, size_t Capacity, size_t N>
	constexpr bool
	operator<( daw::basic_bounded_string<CharT, Capacity> const &lhs,
	           CharT const ( &rhs )[N] ) noexcept {
		return daw::basic_string_view<CharT>{lhs.data( ), lhs.size( )}.compare(
		         rhs ) < 0;
	}

	template<typename CharT, size_t Capacity, size_t N>
	constexpr bool
	operator<=( daw::basic_bounded_string<CharT, Capacity> const &lhs,
	            CharT const ( &rhs )[N] ) noexcept {
		return daw::basic_string_view<CharT>{lhs.data( ), lhs.size( )}.compare(
		         rhs ) <= 0;
	}

	template<typename CharT, size_t Capacity, size_t N>
	constexpr bool
	operator>( daw::basic_bounded_string<CharT, Capacity> const &lhs,
	           CharT const ( &rhs )[N] ) noexcept {
		return daw::basic_string_view<CharT>{lhs.data( ), lhs.size( )}.compare(
		         rhs ) > 0;
	}

	template<typename CharT, size_t Capacity, size_t N>
	constexpr bool
	operator>=( daw::basic_bounded_string<CharT, Capacity> const &lhs,
	            CharT const ( &rhs )[N] ) noexcept {
		return daw::basic_string_view<CharT>{lhs.data( ), lhs.size( )}.compare(
		         rhs ) >= 0;
	}

	// char const[N] / daw::basic_bounded_string
	//
	template<typename CharT, size_t Capacity, size_t N>
	constexpr bool
	operator==( CharT const ( &lhs )[N],
	            daw::basic_bounded_string<CharT, Capacity> const &rhs ) noexcept {
		return daw::basic_string_view<CharT>( lhs, N - 1 )
		         .compare( static_cast<daw::basic_string_view<CharT>>( rhs ) ) == 0;
	}

	template<typename CharT, size_t Capacity, size_t N>
	constexpr bool
	operator!=( CharT const ( &lhs )[N],
	            daw::basic_bounded_string<CharT, Capacity> const &rhs ) noexcept {
		return daw::basic_string_view<CharT>( lhs ).compare( rhs ) != 0;
	}

	template<typename CharT, size_t Capacity, size_t N>
	constexpr bool
	operator<( CharT const ( &lhs )[N],
	           daw::basic_bounded_string<CharT, Capacity> const &rhs ) noexcept {
		return daw::basic_string_view<CharT>( lhs ).compare( rhs ) < 0;
	}

	template<typename CharT, size_t Capacity, size_t N>
	constexpr bool
	operator<=( CharT const ( &lhs )[N],
	            daw::basic_bounded_string<CharT, Capacity> const &rhs ) noexcept {
		return daw::basic_string_view<CharT>( lhs ).compare( rhs ) <= 0;
	}

	template<typename CharT, size_t Capacity, size_t N>
	constexpr bool
	operator>( CharT const ( &lhs )[N],
	           daw::basic_bounded_string<CharT, Capacity> const &rhs ) noexcept {
		return daw::basic_string_view<CharT>( lhs ).compare( rhs ) > 0;
	}

	template<typename CharT, size_t Capacity, size_t N>
	constexpr bool
	operator>=( CharT const ( &lhs )[N],
	            daw::basic_bounded_string<CharT, Capacity> const &rhs ) noexcept {
		return daw::basic_string_view<CharT>( lhs ).compare( rhs ) >= 0;
	}

	/// Appending
#ifndef NOSTRING
	template<typename CharT, size_t Capacity, typename Allocator>
	auto operator+( std::basic_string<CharT, Allocator> lhs,
	                daw::basic_bounded_string<CharT, Capacity> const &rhs ) {
		lhs.append( rhs.cbegin( ), rhs.cend( ) );
		return daw::move( lhs );
	}
#endif

	template<typename CharT, size_t N, size_t Capacity>
	auto operator+( CharT ( &lhs )[N],
	                daw::basic_bounded_string<CharT, Capacity> const &rhs ) {
		static_assert( N > Capacity, "Not enough space to perform append" );
		daw::basic_bounded_string<CharT, Capacity> result{lhs, N};
		result.append( rhs.cbegin( ), rhs.cend( ) );
		return result;
	}

	template<typename CharT, size_t Capacity>
	auto operator+( CharT const *lhs,
	                daw::basic_bounded_string<CharT, Capacity> const &rhs ) {
		daw::basic_bounded_string<CharT, Capacity> result{lhs};
		result.append( rhs.cbegin( ), rhs.cend( ) );
		return result;
	}

#ifndef NOSTRING
	template<typename CharT, size_t Capacity, typename Allocator>
	auto operator+( daw::basic_bounded_string<CharT, Capacity> lhs,
	                std::basic_string<CharT, Allocator> const &rhs ) {
		lhs.append( rhs );
		return daw::move( lhs );
	}

	template<typename CharT, size_t Capacity, typename Allocator>
	auto operator+( daw::basic_bounded_string<CharT, Capacity> lhs,
	                std::basic_string<CharT, Allocator> &&rhs ) {
		lhs.append( daw::move( rhs ) );
		return daw::move( lhs );
	}
#endif

	template<typename CharT, size_t Capacity, size_t N>
	auto operator+( daw::basic_bounded_string<CharT, Capacity> lhs,
	                CharT ( &rhs )[N] ) {
		static_assert( Capacity < ( N - 1 ), "Not enough space to perform append" );
		lhs.append( daw::basic_string_view<CharT>( rhs ) );
		return daw::move( lhs );
	}

	template<typename CharT, size_t Capacity>
	auto operator+( daw::basic_bounded_string<CharT, Capacity> lhs,
	                CharT const *rhs ) {
		lhs.append( daw::basic_string_view<CharT>( rhs ) );
		return daw::move( lhs );
	}

	template<typename CharT, size_t N, typename UnaryPredicate>
	auto split( daw::basic_bounded_string<CharT, N> const &str,
	            UnaryPredicate pred ) {
		static_assert( traits::is_unary_predicate_v<UnaryPredicate, CharT>,
		               "UnaryPredicate p does not fullfill the requires of a unary "
		               "predicate concept.  See "
		               "http://en.cppreference.com/w/cpp/concept/Predicate" );

		return split( daw::string_view{str.data( ), str.size( )},
		              daw::move( pred ) );
	}

	template<typename CharT, size_t N, typename UnaryPredicate>
	auto split( daw::basic_bounded_string<CharT, N> &&str,
	            UnaryPredicate pred ) = delete;

	template<typename CharT, size_t Capacity>
	auto split( daw::basic_bounded_string<CharT, Capacity> const &str,
	            CharT const delemiter ) {
		return split( str,
		              [delemiter]( CharT c ) noexcept { return c == delemiter; } );
	}

	template<typename CharT, size_t Capacity>
	auto split( daw::basic_bounded_string<CharT, Capacity> &&str,
	            CharT const delemiter ) = delete;

	template<typename CharT, size_t Capacity, size_t N>
	auto split( daw::basic_bounded_string<CharT, Capacity> const &str,
	            CharT const ( &delemiter )[N] ) {
		static_assert( N == 2,
		               "string literal used as delemiter.  One 1 value is "
		               "supported (e.g. \",\" )" );
		return split(
		  str, [delemiter]( CharT c ) noexcept { return c == delemiter[0]; } );
	}

	template<typename CharT, size_t Capacity, size_t N>
	auto split( daw::basic_bounded_string<CharT, Capacity> &&str,
	            CharT const ( &delemiter )[N] ) = delete;

	template<typename CharT, size_t Capacity>
	constexpr size_t
	fnv1a_hash( daw::basic_bounded_string<CharT, Capacity> const &str ) noexcept {
		return fnv1a_hash( str.data( ), str.size( ) );
	}

	template<size_t HashSize = sizeof( size_t ), typename CharT, size_t Capacity>
	constexpr size_t generic_hash(
	  daw::basic_bounded_string<CharT, Capacity> const &str ) noexcept {
		return generic_hash<HashSize>( str.data( ), str.size( ) );
	}

	namespace detectors {
		template<typename String>
		using can_be_bounded_string =
		  decltype( daw::bounded_string{std::declval<String>( )} );
	}

	template<typename String>
	constexpr bool can_be_bounded_string =
	  daw::is_detected_v<detectors::can_be_bounded_string, String>;

	template<typename OStream, typename CharT, size_t Capacity,
	         std::enable_if_t<daw::traits::is_ostream_like_v<OStream, CharT>,
	                          std::nullptr_t> = nullptr>
	OStream &operator<<( OStream &os,
	                     daw::basic_bounded_string<CharT, Capacity> const &str ) {
		if( os.good( ) ) {
			auto const size = static_cast<ptrdiff_t>( str.size( ) );
			auto const w = static_cast<ptrdiff_t>( os.width( ) );
			if( w <= size ) {
				os.write( str.data( ), static_cast<std::streamsize>( size ) );
			} else {
				daw::details::sv_insert_aligned( os, str );
			}
			os.width( 0 );
		}
		return os;
	}

	template<size_t HashSize = sizeof( size_t ), typename CharT, size_t Capacity>
	constexpr size_t
	generic_hash( daw::basic_bounded_string<CharT, Capacity> sv ) noexcept {
		return generic_hash<HashSize>( sv.data( ), sv.size( ) );
	}

} // namespace daw

namespace std {
	// TODO use same function as string without killing performance of creating a
	// string
	template<typename CharT, size_t Capacity>
	struct hash<daw::basic_bounded_string<CharT, Capacity>> {
		constexpr size_t
		operator( )( daw::basic_bounded_string<CharT, Capacity> const &str ) const
		  noexcept {
			return daw::fnv1a_hash( str );
		}
	};
} // namespace std
