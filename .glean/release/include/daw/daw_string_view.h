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

#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <iterator>
#include <limits>
#include <stdexcept>

#ifndef NOSTRING
#include <string>
#if defined( __cpp_lib_string_view )
#include <string_view>
#endif
#endif

#include <daw/iterator/daw_back_inserter.h>
#include <vector>

#include "daw_string_view_fwd.h"

#include "daw_algorithm.h"
#include "daw_exception.h"
#include "daw_fnv1a_hash.h"
#include "daw_generic_hash.h"
#include "daw_math.h"
#include "daw_move.h"
#include "daw_swap.h"
#include "daw_traits.h"
#include "impl/daw_string_impl.h"
#include "iterator/daw_iterator.h"

namespace daw {
	template<typename T>
	constexpr std::size_t find_predicate_result_size( T ) {
		return 1;
	}

	namespace string_view_details {
		template<typename T>
		constexpr bool is_dynamic_sv_v = T::extent == dynamic_string_size;
	} // namespace string_view_details

	template<typename CharT, typename BoundsType, ptrdiff_t Extent>
	struct basic_string_view {
		using value_type = CharT;
		using pointer = CharT *;
		using const_pointer = std::add_const_t<CharT> *;
		using reference = std::add_lvalue_reference_t<CharT>;
		using const_reference =
		  std::add_lvalue_reference_t<std::add_const_t<CharT>>;
		using const_iterator = const_pointer;
		using iterator = const_iterator;
		using reverse_iterator = std::reverse_iterator<iterator>;
		using const_reverse_iterator = std::reverse_iterator<const_iterator>;
		using size_type = std::size_t;
		using difference_type = std::ptrdiff_t;
		static constexpr ptrdiff_t extent = Extent;

		template<typename, typename, ptrdiff_t>
		friend struct ::daw::basic_string_view;

	private:
		static_assert(
		  is_string_view_bounds_type_v<BoundsType>,
		  "Invalid BoundType.  Often this is caused by using char_triats" );

		static constexpr auto bp_eq = []( CharT l, CharT r ) { return l == r; };

		template<typename B>
		static constexpr bool is_last_a_pointer_v =
		  std::is_same_v<B, StringViewBoundsPointer>;

		using last_type = std::conditional_t<is_last_a_pointer_v<BoundsType>,
		                                     const_pointer, size_type>;
		using last_difference_type =
		  std::conditional_t<is_last_a_pointer_v<BoundsType>, difference_type,
		                     size_type>;

		template<typename Bounds>
		static constexpr last_type make_last( const_pointer f,
		                                      const_pointer l ) noexcept {
			if constexpr( is_last_a_pointer_v<Bounds> ) {
				return l;
			} else {
				return static_cast<last_type>( l - f );
			}
		}

		template<typename Bounds>
		static constexpr last_type make_last( const_pointer f,
		                                      size_type s ) noexcept {
			if constexpr( is_last_a_pointer_v<Bounds> ) {
				return f + static_cast<difference_type>( s );
			} else {
				return s;
			}
		}

		template<typename Bounds>
		constexpr const_pointer last_pointer( ) const {
			if constexpr( is_last_a_pointer_v<Bounds> ) {
				return m_last;
			} else {
				return m_first + static_cast<difference_type>( m_last );
			}
		}

		template<typename Bounds>
		static constexpr size_type size( const_pointer f, last_type l ) {
			if constexpr( is_last_a_pointer_v<Bounds> ) {
				return static_cast<size_type>( l - f );
			} else {
				return l;
			}
		}

		template<typename Bounds>
		constexpr void dec_front( size_type n ) {
			if constexpr( is_last_a_pointer_v<Bounds> ) {
				m_first += static_cast<difference_type>( n );
			} else {
				m_first += static_cast<difference_type>( n );
				m_last -= n;
			}
		}

		template<typename Bounds>
		constexpr void dec_back( size_type n ) {
			if constexpr( is_last_a_pointer_v<Bounds> ) {
				m_last -= static_cast<difference_type>( n );
			} else {
				m_last -= n;
			}
		}

		const_pointer m_first;
		last_type m_last;

		template<typename ForwardIterator>
		static constexpr ForwardIterator find_not_of( ForwardIterator first,
		                                              ForwardIterator const last,
		                                              basic_string_view sv ) {
			traits::is_forward_access_iterator_test<ForwardIterator>( );
			traits::is_input_iterator_test<ForwardIterator>( );

			while( first != last ) {
				for( std::size_t n = 0; n < sv.size( ); ++n ) {
					if( *first == sv[0] ) {
						return first;
					}
				}
				++first;
			}
			return last;
		}

	public:
		static constexpr size_type const npos =
		  std::numeric_limits<size_type>::max( );

		// constructors
		constexpr basic_string_view( ) noexcept
		  : m_first( nullptr )
		  , m_last( make_last<BoundsType>( nullptr, nullptr ) ) {}

		constexpr basic_string_view( std::nullptr_t ) noexcept
		  : m_first( nullptr )
		  , m_last( make_last<BoundsType>( nullptr, nullptr ) ) {}

		constexpr basic_string_view( std::nullptr_t, size_type ) noexcept
		  : m_first( nullptr )
		  , m_last( make_last<BoundsType>( nullptr, nullptr ) ) {}

		constexpr basic_string_view( const_pointer s,
		                             size_type count = npos ) noexcept
		  : m_first( s )
		  , m_last( make_last<BoundsType>(
		      s, details::sstrlen<size_type>( s, count, npos ) ) ) {}

		template<typename B, difference_type E>
		constexpr basic_string_view( basic_string_view<CharT, B, E> sv,
		                             size_type count ) noexcept
		  : m_first( sv.begin( ) )
		  , m_last( make_last<BoundsType>( sv.begin( ),
		                                   std::min( sv.size( ), count ) ) ) {}

		template<std::size_t N>
		constexpr basic_string_view( CharT const ( &cstr )[N] ) noexcept
		  : m_first( cstr )
		  , m_last( make_last<BoundsType>( N - 1 ) ) {}

#ifndef NOSTRING
		template<typename Traits, typename Allocator>
		basic_string_view(
		  std::basic_string<CharT, Traits, Allocator> const &str ) noexcept
		  : m_first( str.data( ) )
		  , m_last( make_last<BoundsType>( str.data( ), str.size( ) ) ) {}

#if defined( __cpp_lib_string_view )
		template<typename Traits>
		constexpr basic_string_view(
		  std::basic_string_view<CharT, Traits> sv ) noexcept
		  : m_first( sv.data( ) )
		  , m_last( make_last<BoundsType>( sv.data( ), sv.size( ) ) ) {}
#endif
#endif

#if not defined( _MSC_VER ) or defined( __clang__ ) 
		template<typename Bounds, ptrdiff_t Ex>
		constexpr auto
		operator=( basic_string_view<CharT, Bounds, Ex> rhs ) noexcept
		  -> std::enable_if_t<
		    ( string_view_details::is_dynamic_sv_v<basic_string_view> and
		      Ex != extent ),
		    basic_string_view &> {

			m_first = rhs.data( );
			m_last = make_last<BoundsType>( rhs.data( ), rhs.size( ) );
			return *this;
		}
#endif

		constexpr basic_string_view( CharT const *first,
		                             CharT const *last ) noexcept
		  : m_first( first )
		  , m_last( make_last<BoundsType>( first, last ) ) {}

	private:
		/// If you really want to do this, use to_string_view as storing the address
		/// of temporaries is often a mistake
#ifndef NOSTRING
		template<typename Traits, typename Allocator>
		basic_string_view(
		  std::basic_string<CharT, Traits, Allocator> &&str ) noexcept
		  : m_first( str.data( ) )
		  , m_last( make_last<BoundsType>( str.data( ), str.size( ) ) ) {}

		template<typename Traits, typename Allocator>
		friend basic_string_view to_string_view(
		  std::basic_string<CharT, Traits, Allocator> &&str ) noexcept;
#endif
		//
		// END OF constructors
	public:
		template<
		  typename Bounds, ptrdiff_t Ex,
		  std::enable_if_t<( Ex == daw::dynamic_string_size and Ex != Extent ),
		                   std::nullptr_t> = nullptr>
		constexpr operator basic_string_view<CharT, Bounds, Ex>( ) noexcept {
			return {m_first, m_last};
		}

#ifndef NOSTRING
		template<typename Traits, typename Allocator>
		operator std::basic_string<CharT, Traits, Allocator>( ) const {
			return to_string( );
		}

#if defined( __cpp_lib_string_view )
		template<typename Traits>
		constexpr operator std::basic_string_view<CharT, Traits>( ) const {
			return {data( ), size( )};
		}
#endif
		template<typename Traits, typename Allocator>
		basic_string_view &operator=(
		  std::basic_string<CharT, Traits, Allocator> const &str ) noexcept {
			m_first = str.data( );
			m_last = make_last<BoundsType>( str.data( ), str.size( ) );
			return *this;
		}
#endif

		[[nodiscard]] constexpr const_iterator begin( ) const {
			return m_first;
		}

		[[nodiscard]] constexpr const_iterator cbegin( ) const {
			return m_first;
		}

		[[nodiscard]] constexpr const_iterator end( ) const {
			return last_pointer<BoundsType>( );
		}

		[[nodiscard]] constexpr const_iterator cend( ) const {
			return last_pointer<BoundsType>( );
		}

		[[nodiscard]] constexpr const_reverse_iterator rbegin( ) const {
			return const_reverse_iterator( end( ) );
		}

		[[nodiscard]] constexpr const_reverse_iterator crbegin( ) const {
			return const_reverse_iterator( cend( ) );
		}

		[[nodiscard]] constexpr const_reverse_iterator rend( ) const {
			return const_reverse_iterator( begin( ) );
		}

		[[nodiscard]] constexpr const_reverse_iterator crend( ) const {
			return const_reverse_iterator( cbegin( ) );
		}

		[[nodiscard]] constexpr const_reference
		operator[]( size_type const pos ) const {
			return m_first[pos];
		}

		[[nodiscard]] constexpr const_reference at( size_type const pos ) const {
			daw::exception::precondition_check<std::out_of_range>(
			  pos < size( ), "Attempt to access basic_string_view past end" );
			return *std::next( m_first, static_cast<ptrdiff_t>( pos ) );
		}

		[[nodiscard]] constexpr const_reference front( ) const {
			return *m_first;
		}

		[[nodiscard]] constexpr const_reference back( ) const {
			return *std::prev( end( ) );
		}

		[[nodiscard]] constexpr const_pointer data( ) const {
			return m_first;
		}

		// Do not use, use either data( ) if you are sure it is zero terminated or
		// copy to a string
		[[nodiscard, deprecated]] constexpr const_pointer c_str( ) const {
			return m_first;
		}

		[[nodiscard]] constexpr size_type size( ) const {
			return size<BoundsType>( m_first, m_last );
			/*
			if( m_first == nullptr or m_last == nullptr ) {
			  return 0U;
			}
			auto const result = distance( m_first, m_last );
			return static_cast<size_t>( result );*/
		}

		[[nodiscard]] constexpr size_type length( ) const {
			return size( );
		}

		[[nodiscard]] constexpr size_type max_size( ) const {
			return size( );
		}

		[[nodiscard]] constexpr bool empty( ) const {
			return size( ) == 0;
		}

		[[nodiscard]] constexpr explicit operator bool( ) const {
			return not empty( );
		}

		constexpr void remove_prefix( size_type n ) {
			dec_front<BoundsType>( std::min( n, size( ) ) );
		}

		constexpr void remove_prefix( ) {
			remove_prefix( 1 );
		}

		constexpr void remove_suffix( size_type n ) {
			dec_back<BoundsType>( std::min( n, size( ) ) );
		}

		constexpr void remove_suffix( ) {
			remove_suffix( 1 );
		}

		constexpr void clear( ) {
			m_first = nullptr;
			m_last = make_last<BoundsType>( nullptr, 0 );
		}

		[[nodiscard]] constexpr CharT pop_front( ) {
			auto result = front( );
			remove_prefix( );
			return result;
		}

		/// @brief create a substr of the first count characters and remove them
		/// from beggining
		/// @param count number of characters to remove and return
		/// @return a substr of size count starting at begin
		[[nodiscard]] constexpr basic_string_view pop_front( std::size_t count ) {
			basic_string_view result = substr( 0, count );
			remove_prefix( count );
			return result;
		}

		/// @brief searches for where, returns substring between front and where,
		/// then pops off the substring and the where string
		/// @param where string to split on and remove from front
		/// @return substring from beginning to where string
		[[nodiscard]] constexpr basic_string_view
		pop_front( basic_string_view where ) {
			auto pos = find( where );
			auto result = pop_front( pos );
			remove_prefix( where.size( ) );
			return result;
		}

		/// @brief creates a substr of the substr from begin to position reported
		/// true by predicate
		/// @tparam UnaryPredicate a unary predicate type that accepts a char and
		/// indicates with true when to stop
		/// @param pred predicate to determine where to split
		/// @return substring from beginning to position marked by predicate
		template<
		  typename UnaryPredicate,
		  std::enable_if_t<traits::is_unary_predicate_v<UnaryPredicate, CharT>,
		                   std::nullptr_t> = nullptr>
		[[nodiscard]] constexpr basic_string_view pop_front( UnaryPredicate pred ) {

			auto pos = find_first_of_if( daw::move( pred ) );
			auto result = pop_front( pos );
			remove_prefix( find_predicate_result_size( pred ) );
			return result;
		}

		[[nodiscard]] constexpr CharT pop_back( ) {
			auto result = back( );
			remove_suffix( );
			return result;
		}

		/// @brief create a substr of the last count characters and remove them from
		/// end
		/// @param count number of characters to remove and return
		/// @return a substr of size count ending at end of string_view
		[[nodiscard]] constexpr basic_string_view pop_back( std::size_t count ) {
			count = std::min( count, size( ) );
			basic_string_view result = substr( size( ) - count, npos );
			remove_suffix( count );
			return result;
		}

		/// @brief searches for last where, returns substring between where and end,
		/// then pops off the substring and the where string
		/// @param where string to split on and remove from back
		/// @return substring from end of where string to end of string
		[[nodiscard]] constexpr basic_string_view
		pop_back( basic_string_view where ) {
			auto pos = rfind( where );
			if( pos == npos ) {
				auto result{*this};
				remove_prefix( npos );
				return result;
			}
			auto result = substr( pos + where.size( ) );
			remove_suffix( size( ) - pos );
			return result;
		}

		/// @brief searches for last position UnaryPredicate would be true, returns
		/// substring between pred and end, then pops off the substring and the pred
		/// specified string
		/// @tparam UnaryPredicate a unary predicate type that accepts a char and
		/// indicates with true when to stop
		/// @param pred predicate to determine where to split
		/// @return substring from last position marked by predicate to end
		template<
		  typename UnaryPredicate,
		  std::enable_if_t<traits::is_unary_predicate_v<UnaryPredicate, CharT>,
		                   std::nullptr_t> = nullptr>
		[[nodiscard]] constexpr basic_string_view pop_back( UnaryPredicate pred ) {

			auto pos = find_last_of_if( daw::move( pred ) );
			if( pos == npos ) {
				auto result = *this;
				remove_prefix( npos );
				return result;
			}
			auto result = substr( pos + find_predicate_result_size( pred ) );
			remove_suffix( size( ) - pos );
			return result;
		}

		/// @brief searches for where, returns substring between front and where,
		/// then pops off the substring and the where string. Do nothing if where is
		/// not found
		/// @param where string to split on and remove from front
		/// @return substring from beginning to where string
		[[nodiscard]] constexpr basic_string_view
		try_pop_front( basic_string_view where ) {
			auto pos = find( where );
			if( pos == npos ) {
				return basic_string_view<CharT, BoundsType>( );
			}
			auto result = pop_front( pos );
			remove_prefix( where.size( ) );
			return result;
		}

		/// @brief searches for last where, returns substring between where and end,
		/// then pops off the substring and the where string.  If where is not
		/// found, nothing is done
		/// @param where string to split on and remove from back
		/// @return substring from end of where string to end of string
		[[nodiscard]] constexpr basic_string_view
		try_pop_back( basic_string_view where ) {
			auto pos = rfind( where );
			if( pos == npos ) {
				return basic_string_view<CharT, BoundsType>( );
			}
			auto result = substr( pos + where.size( ) );
			remove_suffix( size( ) - pos );
			return result;
		}

		/// @brief searches for where, and disgregards everything until the end of
		/// that
		/// @param where string to find and consume
		/// @return substring with everything up until the end of where removed
		[[nodiscard]] constexpr basic_string_view &
		consume_front( basic_string_view where ) {
			auto pos = find( where );
			remove_prefix( pos );
			remove_prefix( where.size( ) );
			return *this;
		}

		constexpr void resize( std::size_t const n ) {
			daw::exception::precondition_check<std::out_of_range>( n < size( ) );
			m_last = std::next( m_first, static_cast<ptrdiff_t>( n ) );
		}

		constexpr void swap( basic_string_view &v ) {
			daw::cswap( m_first, v.m_first );
			daw::cswap( m_last, v.m_last );
		}

		size_type copy( CharT *dest, size_type const count,
		                size_type const pos = 0 ) const {
			daw::exception::precondition_check<std::out_of_range>(
			  pos <= size( ), "Attempt to access basic_string_view past end" );

			size_type const rlen = std::min( count, size( ) - pos );
			if( rlen > 0 ) {
				auto const f =
				  std::next( begin( ), static_cast<difference_type>( pos ) );
				auto const l = std::next( f, static_cast<difference_type>( rlen ) );
				daw::algorithm::copy( f, l, dest );
			}
			return rlen;
		}

		[[nodiscard]] constexpr basic_string_view
		substr( size_type const pos = 0, size_type const count = npos ) const {
			daw::exception::precondition_check<std::out_of_range>(
			  pos <= size( ), "Attempt to access basic_string_view past end" );
			auto const rcount =
			  static_cast<size_type>( std::min( count, size( ) - pos ) );
			return {m_first + pos, m_first + pos + rcount};
		}

	public:
		template<typename BL, std::ptrdiff_t ExL, typename BR, std::ptrdiff_t ExR>
		[[nodiscard]] static constexpr int
		compare( basic_string_view<CharT, BL, ExL> lhs,
		         basic_string_view<CharT, BR, ExR> rhs ) {
			auto const str_compare = []( CharT const *p0, CharT const *p1,
			                             std::size_t len ) {
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
			                        std::min( lhs.size( ), rhs.size( ) ) );
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

		template<typename Bounds, std::ptrdiff_t Ex,
		         std::enable_if_t<( not std::is_same_v<BoundsType, Bounds> or
		                            ( Ex != Extent ) ),
		                          std::nullptr_t> = nullptr>
		[[nodiscard]] constexpr int
		compare( basic_string_view<CharT, Bounds, Ex> const rhs ) const {
			basic_string_view lhs = *this;
			return compare( lhs, rhs );
		}

		[[nodiscard]] constexpr int compare( basic_string_view const rhs ) const {
			basic_string_view lhs = *this;
			return compare( lhs, rhs );
		}

		[[nodiscard]] constexpr int compare( const_pointer str ) const {
			basic_string_view lhs = *this;
			return compare( lhs, basic_string_view<CharT, BoundsType>( str ) );
		}

		constexpr int compare( size_type const pos1, size_type const count1,
		                       basic_string_view const v ) const {
			return compare( substr( pos1, count1 ), v );
		}

		template<typename Bounds, std::ptrdiff_t Ex>
		[[nodiscard]] constexpr int
		compare( size_type const pos1, size_type const count1,
		         basic_string_view<CharT, Bounds, Ex> const v, size_type const pos2,
		         size_type const count2 ) const {
			return compare( substr( pos1, count1 ), v.substr( pos2, count2 ) );
		}

		[[nodiscard]] constexpr int compare( size_type const pos1,
		                                     size_type const count1,
		                                     const_pointer s ) const {
			return compare( substr( pos1, count1 ),
			                basic_string_view<CharT, BoundsType>( s ) );
		}

		[[nodiscard]] constexpr int compare( size_type const pos1,
		                                     size_type const count1,
		                                     const_pointer s,
		                                     size_type const count2 ) const {
			return compare( substr( pos1, count1 ),
			                basic_string_view<CharT, BoundsType>( s, count2 ) );
		}

		template<typename Bounds, std::ptrdiff_t Ex>
		[[nodiscard]] constexpr size_type
		find( basic_string_view<CharT, Bounds, Ex> const v,
		      size_type const pos = 0 ) const {

			if( size( ) < v.size( ) ) {
				return npos;
			}
			if( v.empty( ) ) {
				return pos;
			}
			auto result =
			  details::search( begin( ) + pos, end( ), v.begin( ), v.end( ) );
			if( end( ) == result ) {
				return npos;
			}
			return static_cast<size_type>( result - begin( ) );
		}

		[[nodiscard]] constexpr size_type find( CharT const c,
		                                        size_type const pos = 0 ) const {
			return find(
			  basic_string_view<CharT, BoundsType>( std::addressof( c ), 1 ), pos );
		}

		[[nodiscard]] constexpr size_type
		find( const_pointer s, size_type const pos, size_type const count ) const {
			return find( basic_string_view<CharT, BoundsType>( s, count ), pos );
		}

		[[nodiscard]] constexpr size_type find( const_pointer s,
		                                        size_type const pos = 0 ) const {
			return find( basic_string_view<CharT, BoundsType>( s ), pos );
		}

		template<typename Bounds, std::ptrdiff_t Ex>
		[[nodiscard]] constexpr size_type
		rfind( basic_string_view<CharT, Bounds, Ex> const v,
		       size_type pos = npos ) const {

			if( size( ) < v.size( ) ) {
				return npos;
			}
			pos = std::min( pos, size( ) - v.size( ) );
			if( v.empty( ) ) {
				return pos;
			}
			for( auto cur = begin( ) + pos;; --cur ) {
				if( details::compare( cur, v.begin( ), v.size( ) ) == 0 ) {
					return static_cast<size_type>( cur - begin( ) );
				}
				if( cur == begin( ) ) {
					return npos;
				}
			}
		}

		[[nodiscard]] constexpr size_type
		rfind( CharT const c, size_type const pos = npos ) const {
			return rfind(
			  basic_string_view<CharT, BoundsType>( std::addressof( c ), 1 ), pos );
		}

		[[nodiscard]] constexpr size_type
		rfind( const_pointer s, size_type const pos, size_type const count ) const {
			return rfind( basic_string_view<CharT, BoundsType>( s, count ), pos );
		}

		[[nodiscard]] constexpr size_type
		rfind( const_pointer s, size_type const pos = npos ) const {
			return rfind( basic_string_view<CharT, BoundsType>( s ), pos );
		}

		/// Find the first item in v that is in string
		/// \param v A range of characters to look for
		/// \param pos Starting position to start searching
		/// \return position of first item in v or npos
		template<typename Bounds, std::ptrdiff_t Ex>
		[[nodiscard]] constexpr size_type
		find_first_of( basic_string_view<CharT, Bounds, Ex> v,
		               size_type pos = 0 ) const {
			if( pos >= size( ) or v.empty( ) ) {
				return npos;
			}
			auto const iter = details::find_first_of( begin( ) + pos, end( ),
			                                          v.begin( ), v.end( ), bp_eq );

			if( end( ) == iter ) {
				return npos;
			}
			return static_cast<size_type>( std::distance( begin( ), iter ) );
		}

		[[nodiscard]] constexpr size_type find_first_of( const_pointer str,
		                                                 size_type pos = 0 ) const {
			return find_first_of( basic_string_view<CharT, BoundsType>( str ), pos );
		}

		template<typename Bounds, std::ptrdiff_t Ex>
		[[nodiscard]] constexpr size_type
		search( basic_string_view<CharT, Bounds, Ex> const v,
		        size_type const pos = 0 ) const {
			if( pos + v.size( ) >= size( ) or v.empty( ) ) {
				return npos;
			}
			auto const iter =
			  details::search( begin( ) + pos, end( ), v.begin( ), v.end( ) );
			if( cend( ) == iter ) {
				return npos;
			}
			return static_cast<size_type>( std::distance( begin( ), iter ) );
		}

		[[nodiscard]] constexpr size_t search( const_pointer str,
		                                       size_type pos = 0 ) const {
			return search( basic_string_view<CharT, BoundsType>( str ), pos );
		}

		template<typename Bounds, std::ptrdiff_t Ex>
		[[nodiscard]] constexpr size_type
		search_last( basic_string_view<CharT, Bounds, Ex> const v,
		             size_type const pos = 0 ) const {
			if( pos + v.size( ) >= size( ) or v.empty( ) ) {
				return npos;
			}
			auto last_pos = pos;
			auto fpos = search( v, pos );
			while( fpos != npos ) {
				last_pos = fpos;
				fpos = search( v, fpos );
				if( fpos == last_pos ) {
					break;
				}
			}
			return last_pos;
		}

		[[nodiscard]] constexpr size_t search_last( const_pointer str,
		                                            size_type pos = 0 ) const {
			return search_last( basic_string_view<CharT, BoundsType>( str ), pos );
		}

		template<typename UnaryPredicate>
		[[nodiscard]] constexpr size_type
		find_first_of_if( UnaryPredicate pred, size_type const pos = 0 ) const {

			traits::is_unary_predicate_test<UnaryPredicate, CharT>( );

			if( pos >= size( ) ) {
				return npos;
			}
			auto const iter =
			  details::find_first_of_if( cbegin( ) + pos, cend( ), pred );
			if( cend( ) == iter ) {
				return npos;
			}
			return static_cast<size_type>( iter - cbegin( ) );
		}

		template<typename UnaryPredicate>
		[[nodiscard]] constexpr size_type
		find_first_not_of_if( UnaryPredicate pred, size_type const pos = 0 ) const {

			traits::is_unary_predicate_test<UnaryPredicate, CharT>( );

			if( pos >= size( ) ) {
				return npos;
			}
			auto const iter =
			  details::find_first_not_of_if( begin( ) + pos, end( ), pred );
			if( end( ) == iter ) {
				return npos;
			}
			return static_cast<size_type>( std::distance( begin( ), iter ) );
		}

		[[nodiscard]] constexpr size_type
		find_first_of( CharT c, size_type const pos = 0 ) const {
			return find_first_of(
			  basic_string_view<CharT, BoundsType>( std::addressof( c ), 1U ), pos );
		}

		[[nodiscard]] constexpr size_type
		find_first_of( const_pointer s, size_type pos,
		               size_type const count ) const {
			return find_first_of( basic_string_view<CharT, BoundsType>( s, count ),
			                      pos );
		}

#ifndef NOSTRING
		template<typename Traits = std::char_traits<CharT>,
		         typename Allocator = std::allocator<CharT>>
		[[nodiscard]] std::basic_string<CharT, Traits, Allocator>
		to_string( ) const {
			std::basic_string<CharT, Traits, Allocator> result;
			result.reserve( size( ) );
			daw::algorithm::copy_n( begin( ), std::back_inserter( result ), size( ) );
			return result;
		}
#endif
	private:
		[[nodiscard]] constexpr size_type
		reverse_distance( const_reverse_iterator first,
		                  const_reverse_iterator last ) const {
			// Portability note here: std::distance is not NOEXCEPT, but calling it
			// with a string_view::reverse_iterator will not throw.
			return ( size( ) - 1u ) -
			       static_cast<size_t>( std::distance( first, last ) );
		}

	public:
		template<typename Bounds, std::ptrdiff_t Ex>
		[[nodiscard]] constexpr size_type
		find_last_of( basic_string_view<CharT, Bounds, Ex> s,
		              size_type pos = npos ) const {
			if( s.size( ) == 0U ) {
				return npos;
			}
			if( pos >= size( ) ) {
				pos = 0;
			} else {
				pos = size( ) - ( pos + 1U );
			}
			auto iter = std::find_first_of(
			  std::next( rbegin( ), static_cast<difference_type>( pos ) ), rend( ),
			  s.rbegin( ), s.rend( ) );
			return iter == rend( ) ? npos : reverse_distance( rbegin( ), iter );
		}

		template<typename UnaryPredicate>
		[[nodiscard]] constexpr size_type
		find_last_of_if( UnaryPredicate pred, size_type pos = npos ) const {

			traits::is_unary_predicate_test<UnaryPredicate, CharT>( );

			if( pos >= size( ) ) {
				pos = 0;
			} else {
				pos = size( ) - ( pos + 1 );
			}
			auto iter = std::find_if(
			  crbegin( ) + static_cast<difference_type>( pos ), crend( ), pred );
			return iter == crend( ) ? npos : reverse_distance( crbegin( ), iter );
		}

		[[nodiscard]] constexpr size_type
		find_last_of( CharT const c, size_type pos = npos ) const {
			if( pos >= size( ) ) {
				pos = 0;
			}
			auto first = std::prev( m_last );
			auto const last = std::next( m_first, pos );
			while( first != last ) {
				if( *first == c ) {
					return static_cast<size_type>( std::distance( m_first, first ) );
				}
				--first;
			}
			return npos;
		}

		template<size_type N>
		[[nodiscard]] constexpr size_type find_last_of( CharT const ( &s )[N],
		                                                size_type pos ) {
			return find_last_of( basic_string_view<CharT, BoundsType>( s, N - 1 ),
			                     pos );
		}

		template<size_type N>
		[[nodiscard]] constexpr size_type find_last_of( CharT const ( &s )[N] ) {
			return find_last_of( basic_string_view<CharT, BoundsType>( s, N - 1 ),
			                     npos );
		}

		[[nodiscard]] constexpr size_type
		find_last_of( const_pointer s, size_type pos, size_type count ) const {
			return find_last_of( basic_string_view<CharT, BoundsType>( s, count ),
			                     pos );
		}

		[[nodiscard]] constexpr size_type find_last_of( const_pointer s,
		                                                size_type pos ) const {
			return find_last_of( basic_string_view<CharT, BoundsType>( s ), pos );
		}

		[[nodiscard]] constexpr size_type find_last_of( const_pointer s ) const {
			return find_last_of( basic_string_view<CharT, BoundsType>( s ), npos );
		}

		template<typename Bounds, std::ptrdiff_t Ex>
		[[nodiscard]] constexpr size_type
		find_first_not_of( basic_string_view<CharT, Bounds, Ex> v,
		                   size_type pos = 0 ) const {
			if( pos >= size( ) ) {
				return npos;
			}
			if( v.empty( ) ) {
				return pos;
			}

			const_iterator iter = details::find_first_not_of(
			  begin( ) + pos, end( ), v.begin( ),
			  std::next( v.begin( ), static_cast<ptrdiff_t>( v.size( ) ) ), bp_eq );
			if( end( ) == iter ) {
				return npos;
			}

			return static_cast<size_type>( std::distance( begin( ), iter ) );
		}

		[[nodiscard]] constexpr size_type
		find_first_not_of( CharT c, size_type pos = 0 ) const {
			return find_first_not_of(
			  basic_string_view<CharT, BoundsType>( std::addressof( c ), 1U ), pos );
		}

		[[nodiscard]] constexpr size_type
		find_first_not_of( const_pointer s, size_type pos, size_type count ) const {
			return find_first_not_of(
			  basic_string_view<CharT, BoundsType>( s, count ), pos );
		}

		[[nodiscard]] constexpr size_type
		find_first_not_of( const_pointer s, size_type pos = 0U ) const {
			return find_first_not_of( basic_string_view<CharT, BoundsType>( s ),
			                          pos );
		}

		template<typename Bounds, std::ptrdiff_t Ex>
		[[nodiscard]] constexpr size_type
		find_last_not_of( basic_string_view<CharT, Bounds, Ex> v,
		                  size_type pos = npos ) const {
			if( pos >= size( ) ) {
				pos = size( ) - 1;
			}
			if( v.empty( ) ) {
				return pos;
			}
			pos = size( ) - ( pos + 1U );
			const_reverse_iterator iter =
			  find_not_of( rbegin( ) + static_cast<intmax_t>( pos ), rend( ), v );
			if( rend( ) == iter ) {
				return npos;
			}
			return reverse_distance( rbegin( ), iter );
		}

		[[nodiscard]] constexpr size_type
		find_last_not_of( CharT c, size_type pos = npos ) const {
			return find_last_not_of(
			  basic_string_view<CharT, BoundsType>( std::addressof( c ), 1U ), pos );
		}

		[[nodiscard]] constexpr size_type
		find_last_not_of( const_pointer s, size_type pos, size_type count ) const {
			return find_last_not_of( basic_string_view<CharT, BoundsType>( s, count ),
			                         pos );
		}

		[[nodiscard]] constexpr size_type
		find_last_not_of( const_pointer s, size_type pos = npos ) const {
			return find_last_not_of( basic_string_view<CharT, BoundsType>( s ), pos );
		}

		[[nodiscard]] constexpr bool starts_with( CharT c ) const {
			if( empty( ) ) {
				return false;
			}
			return front( ) == c;
		}

		template<typename Bounds, std::ptrdiff_t Ex>
		[[nodiscard]] constexpr bool
		starts_with( basic_string_view<CharT, Bounds, Ex> s ) const {
			if( s.size( ) > size( ) ) {
				return false;
			}
			auto lhs = begin( );
			while( not s.empty( ) ) {
				if( *lhs++ != s.front( ) ) {
					return false;
				}
				s.remove_prefix( );
			}
			return true;
		}

		[[nodiscard]] constexpr bool starts_with( const_pointer s ) const {
			return starts_with( basic_string_view<CharT, BoundsType>( s ) );
		}

		[[nodiscard]] constexpr bool ends_with( CharT c ) const {
			if( empty( ) ) {
				return false;
			}
			return back( ) == c;
		}

		template<typename Bounds, std::ptrdiff_t Ex>
		[[nodiscard]] constexpr bool
		ends_with( basic_string_view<CharT, Bounds, Ex> s ) const {
			if( s.size( ) > size( ) ) {
				return false;
			}
			auto lhs = rbegin( );
			while( not s.empty( ) ) {
				if( *lhs++ != s.back( ) ) {
					return false;
				}
				s.remove_suffix( );
			}
			return true;
		}

		[[nodiscard]] constexpr bool ends_with( const_pointer s ) const {
			return ends_with( basic_string_view<CharT, BoundsType>( s ) );
		}
	}; // basic_string_view

	// CTAD
	template<typename CharT>
	basic_string_view( CharT const *s, std::size_t count )
	  ->basic_string_view<CharT>;

#ifndef NOSTRING
	template<typename CharT, typename Traits, typename Allocator>
	basic_string_view( std::basic_string<CharT, Traits, Allocator> const &str )
	  ->basic_string_view<CharT>;

#if false && defined( __cpp_lib_string_view )
	template<typename CharT, typename Traits>
	daw::basic_string_view( std::basic_string_view<CharT, Traits> sv )
	  ->daw::basic_string_view<CharT>;
#endif
#endif
	template<typename CharT, std::size_t N>
	basic_string_view( CharT const ( &cstr )[N] )
	  ->basic_string_view<CharT, default_string_view_bounds_type, N - 1>;
	//
	//
	template<typename CharT, typename Bounds, std::ptrdiff_t Ex>
	constexpr void swap( basic_string_view<CharT, Bounds, Ex> &lhs,
	                     basic_string_view<CharT, Bounds, Ex> &rhs ) {
		lhs.swap( rhs );
	}

#ifndef NOSTRING
	template<typename CharT, typename Bounds = default_string_view_bounds_type,
	         typename Traits, typename Allocator>
	[[nodiscard, deprecated]] basic_string_view<CharT, Bounds>
	to_string_view( std::basic_string<CharT, Traits, Allocator> &&str ) noexcept {
		return basic_string_view<CharT, Bounds>( str.data( ), str.size( ) );
	}
#endif

	template<typename CharT, typename Bounds = default_string_view_bounds_type>
	[[nodiscard]] constexpr basic_string_view<CharT, Bounds>
	make_string_view_it( CharT const *first, CharT const *last ) noexcept {
		return basic_string_view<CharT, Bounds>( first, last );
	}

	template<
	  typename RandomIterator,
	  typename CharT = typename std::iterator_traits<RandomIterator>::value_type,
	  typename Bounds = default_string_view_bounds_type>
	[[nodiscard]] constexpr basic_string_view<CharT, Bounds>
	make_string_view_it( RandomIterator first, RandomIterator last ) {
		traits::is_random_access_iterator_test<RandomIterator>( );
		traits::is_input_iterator_test<RandomIterator>( );

		return {std::addressof( *first ), static_cast<std::size_t>( last - first )};
	}

	template<typename CharT, typename Bounds = default_string_view_bounds_type,
	         typename Allocator>
	[[nodiscard]] basic_string_view<CharT, Bounds>
	make_string_view( std::vector<CharT, Allocator> const &v ) noexcept {
		return {v.data( ), v.size( )};
	}

#ifndef NOSTRING
	template<typename Bounds = default_string_view_bounds_type, typename CharT,
	         typename Traits>
	[[nodiscard]] daw::basic_string_view<CharT, Bounds>
	make_string_view( std::basic_string<CharT, Traits> const &str ) {
		return daw::basic_string_view<CharT, Bounds>{str};
	}
#endif

	template<typename Bounds = default_string_view_bounds_type, typename CharT,
	         std::size_t N>
	[[nodiscard]] constexpr daw::basic_string_view<CharT>
	make_string_view( CharT const ( &str )[N] ) {
		return daw::basic_string_view<CharT, Bounds, N - 1>( str, N );
	}

	template<typename CharT, typename Bounds>
	[[nodiscard]] constexpr daw::basic_string_view<CharT, Bounds>
	make_string_view( daw::basic_string_view<CharT, Bounds> sv ) {
		return sv;
	}
	// basic_string_view / something else
	//
	namespace string_view_details::detectors {
		template<typename T, typename CharT = char,
		         typename Bounds = default_string_view_bounds_type>
		using can_be_string_view =
		  decltype( daw::basic_string_view<CharT, Bounds>( T{} ) );
	}

	template<typename T, typename CharT = char,
	         typename Bounds = default_string_view_bounds_type>
	constexpr bool can_be_string_view =
	  daw::is_detected_v<string_view_details::detectors::can_be_string_view, T,
	                     CharT, Bounds>;

	template<typename CharT, typename BL, std::ptrdiff_t ExL, typename BR,
	         std::ptrdiff_t ExR>
	[[nodiscard]] constexpr bool
	operator==( basic_string_view<CharT, BL, ExL> lhs,
	            basic_string_view<CharT, BR, ExR> rhs ) {
		return lhs.compare( rhs ) == 0;
	}

#ifndef NOSTRING
	template<typename CharT, typename Bounds, std::ptrdiff_t Ex, typename Traits,
	         typename Allocator>
	[[nodiscard]] constexpr bool
	operator==( basic_string_view<CharT, Bounds, Ex> lhs,
	            std::basic_string<CharT, Traits, Allocator> const &rhs ) {
		return lhs.compare( basic_string_view<CharT, Bounds>( rhs.data( ),
		                                                      rhs.size( ) ) ) == 0;
	}
#endif
	template<typename CharT, typename Bounds, std::ptrdiff_t Ex>
	[[nodiscard]] constexpr bool
	operator==( basic_string_view<CharT, Bounds, Ex> lhs, CharT const *rhs ) {
		return lhs.compare( basic_string_view<CharT, Bounds>( rhs ) ) == 0;
	}
#ifndef NOSTRING
	template<typename CharT, typename Traits, typename Bounds, std::ptrdiff_t Ex>
	[[nodiscard]] constexpr bool
	operator==( std::basic_string<CharT, Traits> const &lhs,
	            basic_string_view<CharT, Bounds, Ex> rhs ) {
		return basic_string_view<CharT, Bounds>( lhs.data( ), lhs.size( ) )
		         .compare( rhs ) == 0;
	}
#endif
	template<typename CharT, typename Bounds, std::ptrdiff_t Ex>
	[[nodiscard]] constexpr bool
	operator==( CharT const *lhs, basic_string_view<CharT, Bounds, Ex> rhs ) {
		return basic_string_view<CharT, Bounds>( lhs ).compare( rhs ) == 0;
	}

	template<typename CharT, typename BL, std::ptrdiff_t ExL, typename BR,
	         std::ptrdiff_t ExR>
	[[nodiscard]] constexpr bool
	operator!=( basic_string_view<CharT, BL, ExL> lhs,
	            basic_string_view<CharT, BR, ExR> rhs ) {
		return lhs.compare( rhs ) != 0;
	}

#ifndef NOSTRING
	template<typename CharT, typename Bounds, std::ptrdiff_t Ex, typename Traits,
	         typename Allocator>
	[[nodiscard]] constexpr bool
	operator!=( basic_string_view<CharT, Bounds, Ex> lhs,
	            std::basic_string<CharT, Traits, Allocator> const &rhs ) {
		return lhs.compare( basic_string_view<CharT, Bounds>( rhs.data( ),
		                                                      rhs.size( ) ) ) != 0;
	}
#endif
	template<typename CharT, typename Bounds, std::ptrdiff_t Ex>
	[[nodiscard]] constexpr bool
	operator!=( basic_string_view<CharT, Bounds, Ex> lhs, CharT const *rhs ) {
		return lhs.compare( basic_string_view<CharT, Bounds>( rhs ) ) != 0;
	}
#ifndef NOSTRING
	template<typename CharT, typename Traits, typename Bounds, std::ptrdiff_t Ex>
	[[nodiscard]] constexpr bool
	operator!=( std::basic_string<CharT, Traits> const &lhs,
	            basic_string_view<CharT, Bounds, Ex> rhs ) {
		return basic_string_view<CharT, Bounds>( lhs.data( ), lhs.size( ) )
		         .compare( rhs ) != 0;
	}
#endif
	template<typename CharT, typename Bounds, std::ptrdiff_t Ex>
	[[nodiscard]] constexpr bool
	operator!=( CharT const *lhs, basic_string_view<CharT, Bounds, Ex> rhs ) {
		return basic_string_view<CharT, Bounds>( lhs ).compare( rhs ) != 0;
	}

	template<typename CharT, typename BL, std::ptrdiff_t ExL, typename BR,
	         std::ptrdiff_t ExR>
	[[nodiscard]] constexpr bool
	operator<( basic_string_view<CharT, BL, ExL> lhs,
	           basic_string_view<CharT, BR, ExR> rhs ) {
		return lhs.compare( rhs ) < 0;
	}

#ifndef NOSTRING
	template<typename CharT, typename Bounds, std::ptrdiff_t Ex, typename Traits,
	         typename Allocator>
	[[nodiscard]] constexpr bool
	operator<( basic_string_view<CharT, Bounds, Ex> lhs,
	           std::basic_string<CharT, Traits, Allocator> const &rhs ) {
		return lhs.compare(
		         basic_string_view<CharT, Bounds>( rhs.data( ), rhs.size( ) ) ) < 0;
	}
#endif
	template<typename CharT, typename Bounds, std::ptrdiff_t Ex>
	[[nodiscard]] constexpr bool
	operator<( basic_string_view<CharT, Bounds, Ex> lhs, CharT const *rhs ) {
		return lhs.compare( basic_string_view<CharT, Bounds>( rhs ) ) < 0;
	}
#ifndef NOSTRING
	template<typename CharT, typename Traits, typename Bounds, std::ptrdiff_t Ex>
	[[nodiscard]] constexpr bool
	operator<( std::basic_string<CharT, Traits> const &lhs,
	           basic_string_view<CharT, Bounds, Ex> rhs ) {
		return basic_string_view<CharT, Bounds>( lhs.data( ), lhs.size( ) )
		         .compare( rhs ) < 0;
	}
#endif
	template<typename CharT, typename Bounds, std::ptrdiff_t Ex>
	[[nodiscard]] constexpr bool
	operator<( CharT const *lhs, basic_string_view<CharT, Bounds, Ex> rhs ) {
		return basic_string_view<CharT, Bounds>( lhs ).compare( rhs ) < 0;
	}

	template<typename CharT, typename BL, std::ptrdiff_t ExL, typename BR,
	         std::ptrdiff_t ExR>
	[[nodiscard]] constexpr bool
	operator<=( basic_string_view<CharT, BL, ExL> lhs,
	            basic_string_view<CharT, BR, ExR> rhs ) {
		return lhs.compare( rhs ) <= 0;
	}

#ifndef NOSTRING
	template<typename CharT, typename Bounds, std::ptrdiff_t Ex, typename Traits,
	         typename Allocator>
	[[nodiscard]] constexpr bool
	operator<=( basic_string_view<CharT, Bounds, Ex> lhs,
	            std::basic_string<CharT, Traits, Allocator> const &rhs ) {
		return lhs.compare( basic_string_view<CharT, Bounds>( rhs.data( ),
		                                                      rhs.size( ) ) ) <= 0;
	}
#endif
	template<typename CharT, typename Bounds, std::ptrdiff_t Ex>
	[[nodiscard]] constexpr bool
	operator<=( basic_string_view<CharT, Bounds, Ex> lhs, CharT const *rhs ) {
		return lhs.compare( basic_string_view<CharT, Bounds>( rhs ) ) <= 0;
	}
#ifndef NOSTRING
	template<typename CharT, typename Traits, typename Bounds, std::ptrdiff_t Ex>
	[[nodiscard]] constexpr bool
	operator<=( std::basic_string<CharT, Traits> const &lhs,
	            basic_string_view<CharT, Bounds, Ex> rhs ) {
		return basic_string_view<CharT, Bounds>( lhs.data( ), lhs.size( ) )
		         .compare( rhs ) <= 0;
	}
#endif
	template<typename CharT, typename Bounds, std::ptrdiff_t Ex>
	[[nodiscard]] constexpr bool
	operator<=( CharT const *lhs, basic_string_view<CharT, Bounds, Ex> rhs ) {
		return basic_string_view<CharT, Bounds>( lhs ).compare( rhs ) <= 0;
	}

	template<typename CharT, typename BL, std::ptrdiff_t ExL, typename BR,
	         std::ptrdiff_t ExR>
	[[nodiscard]] constexpr bool
	operator>( basic_string_view<CharT, BL, ExL> lhs,
	           basic_string_view<CharT, BR, ExR> rhs ) {
		return lhs.compare( rhs ) > 0;
	}

#ifndef NOSTRING
	template<typename CharT, typename Bounds, std::ptrdiff_t Ex, typename Traits,
	         typename Allocator>
	[[nodiscard]] constexpr bool
	operator>( basic_string_view<CharT, Bounds, Ex> lhs,
	           std::basic_string<CharT, Traits, Allocator> const &rhs ) {
		return lhs.compare(
		         basic_string_view<CharT, Bounds>( rhs.data( ), rhs.size( ) ) ) > 0;
	}
#endif
	template<typename CharT, typename Bounds, std::ptrdiff_t Ex>
	[[nodiscard]] constexpr bool
	operator>( basic_string_view<CharT, Bounds, Ex> lhs, CharT const *rhs ) {
		return lhs.compare( basic_string_view<CharT, Bounds>( rhs ) ) > 0;
	}
#ifndef NOSTRING
	template<typename CharT, typename Traits, typename Bounds, std::ptrdiff_t Ex>
	[[nodiscard]] constexpr bool
	operator>( std::basic_string<CharT, Traits> const &lhs,
	           basic_string_view<CharT, Bounds, Ex> rhs ) {
		return basic_string_view<CharT, Bounds>( lhs.data( ), lhs.size( ) )
		         .compare( rhs ) > 0;
	}
#endif
	template<typename CharT, typename Bounds, std::ptrdiff_t Ex>
	[[nodiscard]] constexpr bool
	operator>( CharT const *lhs, basic_string_view<CharT, Bounds, Ex> rhs ) {
		return basic_string_view<CharT, Bounds>( lhs ).compare( rhs ) > 0;
	}

	template<typename CharT, typename BL, std::ptrdiff_t ExL, typename BR,
	         std::ptrdiff_t ExR>
	[[nodiscard]] constexpr bool
	operator>=( basic_string_view<CharT, BL, ExL> lhs,
	            basic_string_view<CharT, BR, ExR> rhs ) {
		return lhs.compare( rhs ) >= 0;
	}

#ifndef NOSTRING
	template<typename CharT, typename Bounds, std::ptrdiff_t Ex, typename Traits,
	         typename Allocator>
	[[nodiscard]] constexpr bool
	operator>=( basic_string_view<CharT, Bounds, Ex> lhs,
	            std::basic_string<CharT, Traits, Allocator> const &rhs ) {
		return lhs.compare( basic_string_view<CharT, Bounds>( rhs.data( ),
		                                                      rhs.size( ) ) ) >= 0;
	}
#endif
	template<typename CharT, typename Bounds, std::ptrdiff_t Ex>
	[[nodiscard]] constexpr bool
	operator>=( basic_string_view<CharT, Bounds, Ex> lhs, CharT const *rhs ) {
		return lhs.compare( basic_string_view<CharT, Bounds>( rhs ) ) >= 0;
	}
#ifndef NOSTRING
	template<typename CharT, typename Traits, typename Bounds, std::ptrdiff_t Ex>
	[[nodiscard]] constexpr bool
	operator>=( std::basic_string<CharT, Traits> const &lhs,
	            basic_string_view<CharT, Bounds, Ex> rhs ) {
		return basic_string_view<CharT, Bounds>( lhs.data( ), lhs.size( ) )
		         .compare( rhs ) >= 0;
	}
#endif
	template<typename CharT, typename Bounds, std::ptrdiff_t Ex>
	[[nodiscard]] constexpr bool
	operator>=( CharT const *lhs, basic_string_view<CharT, Bounds, Ex> rhs ) {
		return basic_string_view<CharT, Bounds>( lhs ).compare( rhs ) >= 0;
	}

#ifndef NOSTRING
	template<typename CharT, typename Traits, typename Allocator, typename Bounds,
	         std::ptrdiff_t Ex>
	[[nodiscard]] auto
	operator+( std::basic_string<CharT, Traits, Allocator> lhs,
	           daw::basic_string_view<CharT, Bounds, Ex> rhs ) {
		lhs.reserve( lhs.size( ) + rhs.size( ) );
		daw::algorithm::copy( rhs.begin( ), rhs.end( ), std::back_inserter( lhs ) );
		return lhs;
	}

	template<typename CharT, typename Bounds, std::ptrdiff_t Ex, std::size_t N>
	[[nodiscard]] auto
	operator+( CharT ( &lhs )[N],
	           daw::basic_string_view<CharT, Bounds, Ex> rhs ) {
		static_assert( N > 0 );
		std::basic_string<CharT> result;
		result.reserve( ( N - 1 ) + rhs.size( ) );
		auto dest = std::back_inserter( result );
		daw::algorithm::copy_n( lhs, dest, N - 1 );
		daw::algorithm::copy( rhs.begin( ), rhs.end( ), dest );
		return result;
	}

	template<typename CharT, typename Bounds, std::ptrdiff_t Ex>
	[[nodiscard]] auto
	operator+( CharT const *lhs, daw::basic_string_view<CharT, Bounds, Ex> rhs ) {
		std::size_t const lhs_len = details::strlen<std::size_t>( lhs );
		std::basic_string<CharT> result;
		result.reserve( lhs_len + rhs.size( ) );
		auto dest = std::back_inserter( result );
		daw::algorithm::copy_n( lhs, dest, lhs_len );
		daw::algorithm::copy( rhs.begin( ), rhs.end( ), dest );
		return result;
	}

	template<typename CharT, typename Bounds, std::ptrdiff_t Ex, typename Traits,
	         typename Allocator>
	[[nodiscard]] auto
	operator+( daw::basic_string_view<CharT, Bounds, Ex> lhs,
	           std::basic_string<CharT, Traits, Allocator> const &rhs ) {
		std::basic_string<CharT, Traits, Allocator> result;
		result.reserve( lhs.size( ) + rhs.size( ) );
		daw::algorithm::copy( lhs.begin( ), lhs.end( ),
		                      std::back_inserter( result ) );
		result += rhs;
		return result;
	}

	template<typename CharT, typename Bounds, std::ptrdiff_t Ex, std::size_t N>
	[[nodiscard]] auto operator+( daw::basic_string_view<CharT, Bounds, Ex> lhs,
	                              CharT ( &rhs )[N] ) {
		static_assert( N > 0 );
		std::basic_string<CharT> result;
		result.reserve( lhs.size( ) + ( N - 1 ) );
		auto dest = std::back_inserter( result );
		daw::algorithm::copy( lhs.begin( ), lhs.end( ), dest );
		daw::algorithm::copy_n( rhs, dest, N - 1 );
		return result;
	}

	template<typename CharT, typename Bounds, std::ptrdiff_t Ex>
	[[nodiscard]] auto operator+( daw::basic_string_view<CharT, Bounds, Ex> lhs,
	                              CharT const *rhs ) {
		std::size_t const rhs_len = details::strlen<std::size_t>( rhs );
		std::basic_string<CharT> result;
		result.reserve( lhs.size( ) + rhs_len );
		auto dest = std::back_inserter( result );
		daw::algorithm::copy( lhs.begin( ), lhs.end( ), dest );
		daw::algorithm::copy_n( rhs, dest, rhs_len );
		return result;
	}
#endif

	template<typename CharT, typename Traits = std::char_traits<CharT>,
	         typename Allocator = std::allocator<CharT>, typename Bounds,
	         std::ptrdiff_t Ex, typename UnaryPredicate,
	         std::enable_if_t<traits::is_unary_predicate_v<UnaryPredicate, CharT>,
	                          std::nullptr_t> = nullptr>
	[[nodiscard]] auto split( daw::basic_string_view<CharT, Bounds, Ex> str,
	                          UnaryPredicate pred ) {

		class sv_arry_t {
			std::vector<daw::basic_string_view<CharT, Bounds>> data;

		public:
			sv_arry_t( std::vector<daw::basic_string_view<CharT, Bounds>> v )
			  : data( daw::move( v ) ) {}

			decltype( auto ) operator[]( std::size_t p ) const {
				return data[p];
			}

			std::size_t size( ) const {
				return data.size( );
			}

			bool empty( ) const {
				return data.empty( );
			}

			decltype( auto ) begin( ) const {
				return data.cbegin( );
			}

			decltype( auto ) cbegin( ) const {
				return data.cbegin( );
			}

			decltype( auto ) rbegin( ) const {
				return data.crbegin( );
			}

			decltype( auto ) crbegin( ) const {
				return data.crbegin( );
			}

			decltype( auto ) end( ) const {
				return data.cend( );
			}

			decltype( auto ) cend( ) const {
				return data.cend( );
			}

			decltype( auto ) rend( ) const {
				return data.crend( );
			}

			decltype( auto ) crend( ) const {
				return data.crend( );
			}

#ifndef NOSTRING

			auto as_strings( ) const {
				using string_type = std::basic_string<CharT, Traits, Allocator>;
				using vector_alloc = typename std::allocator_traits<
				  Allocator>::template rebind_alloc<string_type>;
				std::vector<string_type, vector_alloc> tmp{};
				tmp.reserve( size( ) );
				for( auto const &s : data ) {
					tmp.push_back( s.to_string( ) );
				}
				return tmp;
			}
#endif
		};

		std::vector<daw::basic_string_view<CharT, Bounds>> v{};
		auto last_pos = str.cbegin( );
		while( !str.empty( ) ) {
			auto sz = std::min( str.size( ), str.find_first_of_if( pred ) );
			v.emplace_back( last_pos, sz );
			if( sz == str.npos ) {
				break;
			}
			str.remove_prefix( sz + 1 );
			last_pos = str.cbegin( );
		}
		v.shrink_to_fit( );
		return sv_arry_t( daw::move( v ) );
	}

	template<typename CharT, typename Bounds, ptrdiff_t Ex>
	[[nodiscard]] auto split( daw::basic_string_view<CharT, Bounds, Ex> str,
	                          CharT const delemiter ) {
		return split( str, [delemiter]( CharT c ) { return c == delemiter; } );
	}

	template<typename CharT, typename Bounds, std::ptrdiff_t Ex, std::size_t N>
	[[nodiscard]] auto split( daw::basic_string_view<CharT, Bounds, Ex> str,
	                          CharT const ( &delemiter )[N] ) {
		static_assert( N == 2,
		               "string literal used as delemiter.  One 1 value is "
		               "supported (e.g. \",\" )" );
		return split( str, [delemiter]( CharT c ) { return c == delemiter[0]; } );
	}

#ifndef NOSTRING
	template<typename Bounds = default_string_view_bounds_type, typename CharT,
	         typename Traits, typename Allocator, typename Delemiter>
	[[nodiscard]] auto
	split( std::basic_string<CharT, Traits, Allocator> const &str, Delemiter d ) {
		return split( make_string_view<Bounds>( str ), d );
	}
#endif
	template<typename Bounds = default_string_view_bounds_type, typename CharT,
	         std::size_t N, typename Delemiter>
	[[nodiscard]] auto split( CharT const ( &str )[N], Delemiter d ) {
		return split( basic_string_view<CharT, Bounds>( str, N ), d );
	}

	template<typename CharT, typename Bounds, std::ptrdiff_t Ex, typename OStream,
	         std::enable_if_t<daw::traits::is_ostream_like_v<OStream, CharT>,
	                          std::nullptr_t> = nullptr>
	OStream &operator<<( OStream &os,
	                     daw::basic_string_view<CharT, Bounds, Ex> v ) {
		if( os.good( ) ) {
			auto const size = v.size( );
			auto const w = static_cast<std::size_t>( os.width( ) );
			if( w <= size ) {
				os.write( v.data( ), static_cast<intmax_t>( size ) );
			} else {
				daw::details::sv_insert_aligned( os, v );
			}
			os.width( 0 );
		}
		return os;
	}

	namespace string_view_literals {
		[[nodiscard]] constexpr string_view
		operator"" _sv( char const *str, std::size_t len ) noexcept {
			return daw::string_view{str, len};
		}

		[[nodiscard]] constexpr u16string_view
		operator"" _sv( char16_t const *str, std::size_t len ) noexcept {
			return daw::u16string_view{str, len};
		}

		[[nodiscard]] constexpr u32string_view
		operator"" _sv( char32_t const *str, std::size_t len ) noexcept {
			return daw::u32string_view{str, len};
		}

		[[nodiscard]] constexpr wstring_view
		operator"" _sv( wchar_t const *str, std::size_t len ) noexcept {
			return daw::wstring_view{str, len};
		}
	} // namespace string_view_literals

	template<typename CharT, typename Bounds, ptrdiff_t Extent>
	[[nodiscard]] constexpr size_t
	fnv1a_hash( daw::basic_string_view<CharT, Bounds, Extent> sv ) {
		return fnv1a_hash( sv.data( ), sv.size( ) );
	}

	template<std::size_t HashSize = sizeof( std::size_t ), typename CharT,
	         typename Bounds, ptrdiff_t Extent>
	[[nodiscard]] constexpr size_t
	generic_hash( daw::basic_string_view<CharT, Bounds, Extent> sv ) {
		return generic_hash<HashSize>( sv.data( ), sv.size( ) );
	}

#ifndef NOSTRING
	template<typename CharT, typename Traits = std::char_traits<CharT>,
	         typename Allocator = std::allocator<CharT>, typename Bounds,
	         std::ptrdiff_t Ex>
	[[nodiscard]] std::basic_string<CharT, Traits, Allocator>
	to_string( daw::basic_string_view<CharT, Bounds, Ex> sv ) {
		return {sv.begin( ), sv.end( )};
	}
#endif
} // namespace daw

namespace std {
	// TODO use same function as string without killing performance of creating
	// a string
	template<typename CharT, typename Bounds, std::ptrdiff_t Ex>
	struct hash<daw::basic_string_view<CharT, Bounds, Ex>> {
		[[nodiscard]] constexpr size_t
		operator( )( daw::basic_string_view<CharT, Bounds, Ex> s ) {
			return daw::fnv1a_hash( s );
		}
	};
} // namespace std
