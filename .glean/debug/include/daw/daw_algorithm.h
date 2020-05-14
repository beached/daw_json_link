// The MIT License (MIT)
//
// Copyright (c) 2013-2020 Darrell Wright
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
#include <functional>
#include <iterator>
#include <optional>
#include <stdexcept>
#include <type_traits>
#include <utility>
#include <vector>

#include "cpp_17.h"
#include "daw_do_n.h"
#include "daw_enable_if.h"
#include "daw_exception.h"
#include "daw_move.h"
#include "daw_swap.h"
#include "daw_traits.h"
#include "daw_view.h"
#include "impl/daw_math_impl.h"

namespace daw {
	template<typename Map, typename Key>
	constexpr decltype( auto ) try_get( Map &container, Key &&k ) {
		auto pos = container.find( std::forward<Key>( k ) );
		using result_t = daw::remove_cvref_t<decltype( *pos )>;
		if( pos == std::end( container ) ) {
			return std::optional<result_t>{};
		}
		return std::optional<result_t>( std::in_place, *pos );
	}

	template<typename Map, typename Key>
	constexpr decltype( auto ) try_get( Map const &container, Key &&k ) {
		auto pos = container.find( std::forward<Key>( k ) );
		using result_t = daw::remove_cvref_t<decltype( *pos )>;
		if( pos == std::end( container ) ) {
			return std::optional<result_t>{};
		}
		return std::optional<result_t>( std::in_place, *pos );
	}

	template<typename Distance, typename Iterator>
	constexpr void advance_many( Distance d, Iterator &it ) {
		daw::advance( it, d );
	}

	template<typename Distance, typename Iterator, typename Iterator2,
	         typename... Iterators>
	constexpr void advance_many( Distance d, Iterator &it, Iterator2 &it2,
	                             Iterators &... its ) {

		daw::advance( it, d );
		advance_many( d, it2, its... );
	}

	// Iterator movement functions
	namespace algorithm_details {
		template<typename Iterator, typename Iterator2, typename Iterator3,
		         typename Distance,
		         daw::enable_when_t<
		           not traits::is_random_access_iterator<Iterator>> = nullptr>
		constexpr void safe_advance_impl( Iterator2 const first, Iterator &it,
		                                  Iterator3 const last,
		                                  Distance dist ) noexcept {

			// Move it forward towards last
			for( ; it != last and dist > 0; --dist, ++it ) {}

			// Move it backwards towards first
			for( ; it != first and dist < 0; ++dist, --it ) {}
		}

		template<
		  typename Iterator, typename Iterator2, typename Iterator3,
		  typename Distance,
		  daw::enable_when_t<traits::is_random_access_iterator<Iterator>> = nullptr>
		constexpr void safe_advance_impl( Iterator2 const first, Iterator &it,
		                                  Iterator3 const last,
		                                  Distance dist ) noexcept {

			if( dist < 0 ) {
				auto const dist_to_first = it - first;
				if( dist_to_first < 0 ) {
					it = first;
					return;
				}
				dist = -dist;
				it -= daw::min( static_cast<ptrdiff_t>( dist_to_first ),
				                static_cast<ptrdiff_t>( dist ) );
				return;
			}
			auto const dist_to_last = last - it;
			if( dist_to_last < 0 ) {
				it = last;
				return;
			}
			it += daw::min( static_cast<ptrdiff_t>( dist_to_last ),
			                static_cast<ptrdiff_t>( dist ) );
		}
	} // namespace algorithm_details

	/// @brief Advance Iterator within the bounds of container
	/// @tparam Container Container type who's iterators are of type Iterator
	/// @tparam Iterator Iterator pointing to members of container.
	/// @param container container to set bounds on iterator
	/// @param it iterator to move.  It is undefined behavior if iterator is
	/// outside the range [std::begin(container), std::end(container)]
	/// @param distance how far to move
	template<typename Container, typename Iterator, typename Distance>
	constexpr void safe_advance( Container &container, Iterator &it,
	                             Distance distance ) noexcept {

		traits::is_container_like_test<Container>( );
		traits::is_iterator_test<Iterator>( );

		using std::begin;
		using std::end;
		auto const d = static_cast<ptrdiff_t>( distance );
		algorithm_details::safe_advance_impl( begin( container ), it,
		                                      end( container ), d );
	}

	/// @brief Advance Iterator within the bounds of container
	/// @tparam Container Container type who's iterators are of type Iterator
	/// @tparam Iterator Iterator pointing to members of container.
	/// @param container container to set bounds on iterator
	/// @param it iterator to move.  It is undefined behavior if iterator is
	/// outside the range [std::begin(container), std::end(container)]
	/// @param distance how far to move
	template<typename Container, typename Iterator, typename Distance>
	constexpr void safe_advance( Container const &container, Iterator &it,
	                             Distance distance ) noexcept {

		traits::is_container_like_test<Container>( );
		traits::is_iterator_test<Iterator>( );

		using std::begin;
		using std::end;
		auto const d = static_cast<ptrdiff_t>( distance );
		algorithm_details::safe_advance_impl( begin( container ), it,
		                                      end( container ), d );
	}

	/// @brief Advance iterator n steps forward but do not go past last.
	/// Undefined if it > last
	/// @tparam Iterator Type of Iterator to advance
	/// @param it iterator to advance
	/// @param last boundary for it
	/// @param n number of steps to advance forwards
	/// @return The resulting iterator advanced n steps
	template<typename Iterator, typename Distance = size_t>
	constexpr Iterator
	safe_next( Iterator it, Iterator const last,
	           Distance n = 1U ) noexcept( noexcept( daw::next( it, n ) ) ) {

		traits::is_iterator_test<Iterator>( );
		auto const d = static_cast<ptrdiff_t>( n );
		algorithm_details::safe_advance_impl( it, it, last, d );
		return it;
	}

	/// @brief Advance iterator n steps backward but do not go past first.
	/// Undefined if it < first
	/// @tparam Iterator Type of Iterator to advance
	/// @param it iterator to advance
	/// @param first boundary for it
	/// @param n number of steps to advance backwards
	/// @return The resulting iterator advanced n steps
	template<typename Iterator, typename Distance>
	constexpr Iterator
	safe_prev( Iterator it, Iterator first,
	           Distance n = 1 ) noexcept( noexcept( daw::prev( it, n ) ) ) {

		traits::is_iterator_test<Iterator>( );
		auto const d = static_cast<ptrdiff_t>( n );
		algorithm_details::safe_advance_impl( first, it, it, -d );
		return it;
	}

	/// @brief Take iterator return from begin of a container and return the
	/// result of running next with n steps
	/// @tparam Container Container type iterator will come from
	/// @param container container to get iterator from
	/// @param n how many steps to move forward from begin
	/// @return an iterator referencing a value in container n steps from begin
	template<typename Container>
	constexpr auto
	begin_at( Container &container,
	          size_t n ) noexcept( noexcept( std::begin( container ) ) )
	  -> decltype( std::begin( container ) ) {

		traits::is_container_like_test<Container>( );
		auto result = std::begin( container );
		safe_advance( container, result, static_cast<ptrdiff_t>( n ) );
		return result;
	}

	/// @brief Take iterator return from begin of a container and return the
	/// result of running next with n steps
	/// @tparam Container Container type iterator will come from
	/// @param container container to get iterator from
	/// @param n how many steps to move forward from begin
	/// @return an iterator referencing a value in container n steps from begin
	template<typename Container>
	constexpr auto
	begin_at( Container const &container,
	          size_t n ) noexcept( noexcept( std::cbegin( container ) ) )
	  -> decltype( std::cbegin( container ) ) {

		traits::is_container_like_test<Container>( );

		auto result = std::cbegin( container );
		safe_advance( container, result, static_cast<ptrdiff_t>( n ) );
		return result;
	}
} // namespace daw

namespace daw::algorithm {
	template<typename Lhs>
	constexpr auto const &min_item( Lhs const &lhs ) noexcept {
		return lhs;
	}

	template<typename Lhs, typename... Ts,
	         daw::enable_when_t<( sizeof...( Ts ) > 0 )> = nullptr>
	constexpr auto const &min_item( Lhs const &lhs, Ts const &... ts ) noexcept {
		auto const &rhs = min_item( ts... );
		return lhs < rhs ? lhs : rhs;
	}

	template<typename Lhs>
	constexpr auto const &max_item( Lhs const &lhs ) noexcept {
		return lhs;
	}

	template<typename Lhs, typename... Ts,
	         daw::enable_when_t<( sizeof...( Ts ) > 0 )> = nullptr>
	constexpr auto const &max_item( Lhs const &lhs, Ts const &... ts ) noexcept {
		auto const &rhs = max_item( ts... );
		return lhs > rhs ? lhs : rhs;
	}

	namespace algorithm_details {
		template<typename ForwardIterator>
		constexpr ForwardIterator midpoint( ForwardIterator a, ForwardIterator b ) {

			traits::is_forward_access_iterator_test<ForwardIterator>( );

			daw::exception::precondition_check(
			  a <= b,
			  " Cannot find a midpoint unless the first parameter is <= the "
			  "second" );

			return daw::next( a, daw::distance( a, b ) / 2 );
		}
	} // namespace algorithm_details

	template<typename ForwardIterator, typename Value,
	         typename Compare = std::less<>>
	constexpr ForwardIterator lower_bound( ForwardIterator first,
	                                       ForwardIterator last, Value &&value,
	                                       Compare cmp = {} ) {
		// Precondition checks
		traits::is_forward_access_iterator_test<ForwardIterator>( );
		traits::is_input_iterator_test<ForwardIterator>( );
		traits::is_predicate_test<Compare, decltype( *first ), Value>( );

		while( first != last ) {
			auto mid = std::next( first, std::distance( first, last ) / 2 );
			if( cmp( *mid, value ) ) {
				first = std::next( mid );
			} else {
				last = mid;
			}
		}
		return first;
	}

	template<class InputIterator, class T>
	constexpr InputIterator find( InputIterator first, InputIterator last,
	                              T const &value ) {
		for( ; first != last; ++first ) {
			if( *first == value ) {
				return first;
			}
		}
		return last;
	}

	template<typename InputIterator, typename UnaryPredicate>
	constexpr InputIterator find_if( InputIterator first, InputIterator last,
	                                 UnaryPredicate &&unary_predicate ) {
		traits::is_input_iterator_test<InputIterator>( );
		traits::is_unary_predicate_test<UnaryPredicate, decltype( *first )>( );

		while( first != last ) {
			if( unary_predicate( *first ) ) {
				return first;
			}
			++first;
		}
		return last;
	}
	template<typename InputIterator, typename UnaryPredicate>
	constexpr InputIterator find_if_not( InputIterator first, InputIterator last,
	                                     UnaryPredicate &&unary_predicate ) {
		traits::is_input_iterator_test<InputIterator>( );
		traits::is_unary_predicate_test<UnaryPredicate, decltype( *first )>( );

		while( first != last ) {
			if( not unary_predicate( *first ) ) {
				return first;
			}
			++first;
		}
		return last;
	}

	template<typename ForwardIterator, typename UnaryPredicate>
	constexpr ForwardIterator partition( ForwardIterator first,
	                                     ForwardIterator last,
	                                     UnaryPredicate &&unary_predicate ) {
		// Precondition checks
		traits::is_forward_access_iterator_test<ForwardIterator>( );
		traits::is_input_iterator_test<ForwardIterator>( );
		traits::is_unary_predicate_test<UnaryPredicate, decltype( *first )>( );

		first = find_if_not( first, last, unary_predicate );
		if( first == last ) {
			return first;
		}

		for( auto it = std::next( first ); it != last; ++it ) {
			if( unary_predicate( *it ) ) {
				daw::iter_swap( it, first );
				++first;
			}
		}
		return first;
	}

	template<typename ForwardIterator, typename Value,
	         typename Predicate = std::less<>>
	constexpr ForwardIterator
	binary_search( ForwardIterator first, ForwardIterator const last,
	               Value &&value, Predicate less_than = Predicate{} ) {

		traits::is_forward_access_iterator_test<ForwardIterator>( );
		traits::is_input_iterator_test<ForwardIterator>( );
		traits::is_predicate_test<Predicate, decltype( *first ), Value>( );

		exception::precondition_check(
		  first < last, ": First position must be less than second" );

		auto it_last = last;

		while( first < it_last ) {
			auto const mid = algorithm_details::midpoint( first, it_last );
			if( less_than( *mid, value ) ) {
				first = mid;
				daw::advance( first, 1 );
			} else if( less_than( value, *mid ) ) {
				it_last = mid;
			} else { // equal
				return mid;
			}
		}
		return last;
	}

	template<typename Container>
	constexpr auto
	rbegin2( Container &container ) noexcept( noexcept( container.rbegin( ) ) )
	  -> decltype( container.rbegin( ) ) {

		traits::is_container_like_test<Container>( );
		return container.rbegin( );
	}

	template<typename Container>
	constexpr auto
	crbegin2( Container &container ) noexcept( noexcept( container.crbegin( ) ) )
	  -> decltype( container.crbegin( ) ) {

		traits::is_container_like_test<Container>( );
		return container.crbegin( );
	}

	template<typename Container>
	constexpr auto
	rend2( Container &container ) noexcept( noexcept( container.rend( ) ) )
	  -> decltype( container.rend( ) ) {

		traits::is_container_like_test<Container>( );
		return container.rend( );
	}

	template<typename Container>
	constexpr auto
	crend2( Container &container ) noexcept( noexcept( container.crend( ) ) )
	  -> decltype( container.crend( ) ) {

		traits::is_container_like_test<Container>( );
		return container.crend( );
	}

	template<typename Value>
	[[deprecated( "use vector::data( ) member" )]] inline Value const *
	to_array( std::vector<Value> const &values ) {

		return values.data( );
	}

	template<typename Value>
	[[deprecated( "use vector::data( ) member" )]] inline Value *
	to_array( std::vector<Value> &values ) {

		return values.data( );
	}

	template<typename Ptr>
	[[deprecated]] constexpr bool is_null_ptr( Ptr const *const ptr ) noexcept {
		return nullptr == ptr;
	}

	/// @brief Reverser eg for( auto item: reverse( container ) ) { }
	namespace algorithm_details {
		template<typename Fwd>
		struct Reverser_generic {
			Fwd &fwd;

			explicit Reverser_generic( Fwd &fwd_ )
			  : fwd( fwd_ ) {}

			typedef std::reverse_iterator<typename Fwd::iterator> reverse_iterator;

			reverse_iterator begin( ) {
				return reverse_iterator( std::end( fwd ) );
			}

			reverse_iterator end( ) {
				return reverse_iterator( std::begin( fwd ) );
			}
		};

		template<typename Fwd>
		struct Reverser_special {
			Fwd &fwd;

			explicit Reverser_special( Fwd &fwd_ )
			  : fwd( fwd_ ) {}

			auto begin( ) -> decltype( fwd.rbegin( ) ) {
				return fwd.rbegin( );
			}

			auto end( ) -> decltype( fwd.rbegin( ) ) {
				return fwd.rend( );
			}
		};

		template<typename Fwd>
		auto reverse_impl( Fwd &fwd, long )
		  -> decltype( Reverser_generic<Fwd>( fwd ) ) {
			return Reverser_generic<Fwd>( fwd );
		}

		template<typename Fwd>
		auto reverse_impl( Fwd &fwd, int )
		  -> decltype( fwd.rbegin( ), Reverser_special<Fwd>( fwd ) ) {
			return Reverser_special<Fwd>( fwd );
		}
	} // namespace algorithm_details

	template<typename Fwd>
	auto reverse( Fwd &&fwd )
	  -> decltype( algorithm_details::reverse_impl( fwd, int( 0 ) ) ) {
		static_assert( not std::is_rvalue_reference_v<Fwd &&>,
		               "Cannot pass rvalue_reference to reverse()" );
		return algorithm_details::reverse_impl( fwd, int( 0 ) );
	}

	template<typename ValueType>
	constexpr bool are_equal( ValueType ) noexcept {
		return true;
	}

	template<typename ValueType>
	constexpr bool are_equal( ValueType v1, ValueType v2 ) {
		return v1 == v2;
	}

	template<typename ValueType, typename... Values>
	constexpr bool are_equal( ValueType v1, ValueType v2, Values... others ) {
		return are_equal( v1, v2 ) and are_equal( others... );
	}

	/// @brief Performs a left rotation on a range of elements.
	/// @tparam ForwardIterator type of Iterator for items in range
	/// @tparam LastType type that is equal to ForwardIterator when end of range
	/// reached
	/// @param first first item in range
	/// @param middle middle of range, first item in new range
	/// @param last last item in range
	template<typename ForwardIterator, typename LastType>
	constexpr void rotate(
	  ForwardIterator first, ForwardIterator middle,
	  LastType last ) noexcept( noexcept( daw::cswap( *first, *middle ) ) ) {

		traits::is_forward_access_iterator_test<ForwardIterator>( );
		traits::is_inout_iterator_test<ForwardIterator>( );

		ForwardIterator tmp = middle;
		while( first != tmp ) {
			daw::cswap( *first, *tmp );
			++first;
			++tmp;
			if( tmp == last ) {
				tmp = middle;
			} else if( first == middle ) {
				middle = tmp;
			}
		}
	}

	template<typename ForwardIterator>
	constexpr std::pair<ForwardIterator, ForwardIterator>
	slide( ForwardIterator first, ForwardIterator last, ForwardIterator target ) {

		traits::is_forward_access_iterator_test<ForwardIterator>( );
		traits::is_input_iterator_test<ForwardIterator>( );

		if( target < first ) {
			return std::make_pair( target,
			                       daw::algorithm::rotate( target, first, last ) );
		}

		if( last < target ) {
			return std::make_pair( daw::algorithm::rotate( first, last, target ),
			                       target );
		}

		return std::make_pair( first, last );
	}

	template<typename ForwardIterator, typename Predicate>
	std::pair<ForwardIterator, ForwardIterator>
	gather( ForwardIterator first, ForwardIterator last, ForwardIterator target,
	        Predicate predicate ) {

		traits::is_forward_access_iterator_test<ForwardIterator>( );
		traits::is_input_iterator_test<ForwardIterator>( );

		auto start =
		  std::stable_partition( first, target, [&predicate]( auto &&... args ) {
			  return not predicate( std::forward<decltype( args )>( args )... );
		  } );

		auto finish = std::stable_partition( target, last, predicate );

		return std::make_pair( start, finish );
	}

	template<typename T>
	constexpr T clamp( T &&value, T &&max_value ) noexcept {
		if( value > max_value ) {
			return std::forward<T>( max_value );
		}
		return std::forward<T>( value );
	}

	template<size_t N>
	struct tuple_functor {
		template<typename Tuple, typename Func>
		static constexpr void run( std::size_t i, Tuple &&tp, Func &&func ) {
			constexpr size_t const I = ( N - 1 );
			switch( i ) {
			case I:
				daw::invoke( std::forward<Func>( func ),
				             std::get<I>( std::forward<Tuple>( tp ) ) );
				break;
			default:
				tuple_functor<I>::run( i, std::forward<Tuple>( tp ),
				                       std::forward<Func>( func ) );
			}
		}
	}; // namespace algorithm

	template<>
	struct tuple_functor<0> {
		template<typename Tuple, typename Func>
		constexpr static void run( size_t, Tuple &&, Func && ) noexcept {}
	}; // struct tuple_functor

	template<typename ForwardIterator, typename UnaryPredicate>
	constexpr ForwardIterator find_last_of( ForwardIterator first,
	                                        ForwardIterator const last,
	                                        UnaryPredicate pred ) {

		traits::is_forward_access_iterator_test<ForwardIterator>( );
		traits::is_unary_predicate_test<UnaryPredicate, decltype( *first )>( );

		auto prev = last;
		while( first != last ) {
			if( not pred( *first ) ) {
				break;
			}
			prev = first;
			++first;
		}
		return prev;
	}

	template<typename ForwardIterator, typename UnaryPredicate>
	constexpr ForwardIterator find_first_of( ForwardIterator first,
	                                         ForwardIterator last,
	                                         UnaryPredicate pred ) {

		traits::is_forward_access_iterator_test<ForwardIterator>( );
		traits::is_unary_predicate_test<UnaryPredicate, decltype( *first )>( );

		while( first != last ) {
			if( pred( *first ) ) {
				break;
			}
			++first;
		}
		return first;
	}

	template<typename Iterator, typename UnaryPredicate>
	constexpr auto find_first_range_of( Iterator first, Iterator const last,
	                                    UnaryPredicate pred ) {

		traits::is_unary_predicate_test<UnaryPredicate, decltype( *first )>( );

		first = find_first_of( first, last, pred );
		auto second = find_last_of( first, last, pred );
		if( second != last ) {
			second = daw::next( second );
		}
		return std::make_pair( first, second );
	}

	template<typename Iterator, typename Value>
	auto split( Iterator first, Iterator last, Value const &value ) {

		traits::is_iterator_test<Iterator>( );
		using value_type = daw::traits::root_type_t<decltype( *first )>;

		auto result = std::vector<std::vector<value_type>>( );
		auto temp = std::vector<value_type>( );

		for( auto it = first; it != last; ++it ) {
			if( value == *it ) {
				result.push_back( daw::move( temp ) );
				temp.clear( );
			} else {
				temp.push_back( *it );
			}
		}
		return result;
	}

	/// @brief Returns true if any function returns true for the value
	/// @param value Argument to UnaryPredicate
	/// @param func A UnaryPredicate that returns true/false
	/// @return The result of func
	template<typename Value, typename UnaryPredicate>
	constexpr bool
	satisfies_one( Value &&value,
	               UnaryPredicate &&func ) noexcept( noexcept( func( value ) ) ) {

		traits::is_unary_predicate_test<UnaryPredicate, Value>( );

		return daw::invoke( std::forward<UnaryPredicate>( func ),
		                    std::forward<Value>( value ) );
	}

	/// @brief Returns true if any function returns true for the value
	/// @param value Argument to UnaryPredicate(s)
	/// @param func A UnaryPredicate that returns true/false
	/// @param funcs UnaryPredicates that return true/false
	/// @return True if any of the func/funcs return true(e.g. like OR)
	template<typename Value, typename UnaryPredicate, typename... UnaryPredicates>
	constexpr bool satisfies_one( Value &&value, UnaryPredicate &&func,
	                              UnaryPredicates &&... funcs ) {

		traits::is_unary_predicate_test<UnaryPredicate, Value>( );

		return daw::invoke( std::forward<UnaryPredicate>( func ), value ) or
		       satisfies_one( std::forward<Value>( value ),
		                      std::forward<UnaryPredicates>( funcs )... );
	}

	/// @brief Returns true if any function returns true for any value in range
	/// @param first iterator pointing to the beginning of the range inclusively
	/// @param last iterator pointing to the end of the range exclusively
	/// @param func A UnaryPredicate that returns true/false
	/// @param funcs UnaryPredicates that return true/false
	/// @return True if any of the func/funcs return true(e.g. like OR) for any
	/// value in range
	template<typename Iterator, typename Iterator2, typename UnaryPredicate,
	         typename... UnaryPredicates,
	         daw::enable_when_t<std::conjunction_v<
	           traits::is_dereferenceable<Iterator2>,
	           traits::is_equality_comparable<daw::traits::deref_t<Iterator2>>>> =
	           nullptr>
	constexpr bool satisfies_one(
	  Iterator first, Iterator2 last, UnaryPredicate &&func,
	  UnaryPredicates
	    &&... funcs ) noexcept( noexcept( satisfies_one( *first, func,
	                                                     funcs... ) ) ) {

		traits::is_iterator_test<Iterator>( );
		traits::is_unary_predicate_test<UnaryPredicate, decltype( *first )>( );

		while( first != last ) {
			if( satisfies_one( *first, func, funcs... ) ) {
				return true;
			}
			++first;
		}
		return false;
	}

	/// @brief Returns true if function returns true for the value
	/// @param value Argument to UnaryPredicate
	/// @param func A UnaryPredicate that returns true/false
	/// @return The result of func
	template<typename Value, typename UnaryPredicate>
	constexpr bool
	satisfies_all( Value &&value,
	               UnaryPredicate &&func ) noexcept( noexcept( func( value ) ) ) {

		traits::is_unary_predicate_test<UnaryPredicate, Value>( );

		return daw::invoke( std::forward<UnaryPredicate>( func ),
		                    std::forward<Value>( value ) );
	}

	/// @brief Returns true if all function(s) returns true for the value
	/// @param value Argument to UnaryPredicate(s)
	/// @param func A UnaryPredicate that returns true/false
	/// @param funcs UnaryPredicates that return true/false
	/// @return True if any of the func/funcs return true(e.g. like OR)
	template<typename Value, typename UnaryPredicate, typename... UnaryPredicates>
	constexpr bool satisfies_all( Value &&value, UnaryPredicate &&func,
	                              UnaryPredicates &&... funcs ) {

		traits::is_unary_predicate_test<UnaryPredicate, Value>( );

		auto const result =
		  daw::invoke( std::forward<UnaryPredicate>( func ), value );
		return result and
		       satisfies_all( std::forward<Value>( value ),
		                      std::forward<UnaryPredicates>( funcs )... );
	}

	/// @brief Returns true if all function(s) returns true for all values in
	/// range
	/// @param first iterator pointing to the beginning of the range inclusively
	/// @param last iterator pointing to the end of the range exclusively
	/// @param func A UnaryPredicate that returns true/false
	/// @param funcs UnaryPredicates that return true/false
	/// @return True if any of the func/funcs return true(e.g. like OR) for any
	/// value in range
	template<typename Iterator, typename Iterator2, typename UnaryPredicate,
	         typename... UnaryPredicates,
	         daw::enable_when_t<std::conjunction_v<
	           traits::is_dereferenceable<Iterator2>,
	           traits::is_equality_comparable<daw::traits::deref_t<Iterator2>>>> =
	           nullptr>
	constexpr bool satisfies_all(
	  Iterator first, Iterator2 last, UnaryPredicate &&func,
	  UnaryPredicates
	    &&... funcs ) noexcept( noexcept( satisfies_one( *first, func,
	                                                     funcs... ) ) ) {

		traits::is_unary_predicate_test<UnaryPredicate, decltype( *first )>( );

		while( first != last ) {
			if( not satisfies_all( *first, func, funcs... ) ) {
				return false;
			}
			++first;
		}
		return true;
	}

	namespace algorithm_details {
		template<typename Lower, typename Upper>
		class in_range {
			Lower m_lower;
			Upper m_upper;

		public:
			template<typename L, typename U>
			constexpr in_range( L &&lower, U &&upper )
			  : m_lower( std::forward<L>( lower ) )
			  , m_upper( std::forward<U>( upper ) ) {

				daw::exception::precondition_check<std::out_of_range>( lower <= upper );
			}

			template<typename T>
			constexpr bool operator( )( T &&value ) const {
				return m_lower <= value and std::forward<T>( value ) <= m_upper;
			}
		}; // in_range

		template<typename Value>
		class equal_to {
			Value m_value;

		public:
			template<typename V,
			         daw::enable_when_t<std::is_convertible_v<V, Value>> = nullptr>
			constexpr equal_to( V &&value )
			  : m_value( std::forward<V>( value ) ) {}

			template<typename T>
			constexpr bool operator( )( T &&value ) const {
				return std::forward<T>( value ) == m_value;
			}
		}; // equal_to

		template<typename Value>
		class less_than {
			Value m_value;

		public:
			template<typename V,
			         daw::enable_when_t<std::is_convertible_v<V, Value>> = nullptr>
			constexpr less_than( V &&value )
			  : m_value( std::forward<V>( value ) ) {}

			template<typename T>
			constexpr bool operator( )( T &&value ) const {
				return std::forward<T>( value ) < m_value;
			}
		}; // less_than

		template<typename Value>
		class greater_than {
			Value m_value;

		public:
			template<typename V,
			         daw::enable_when_t<std::is_convertible_v<V, Value>> = nullptr>
			constexpr greater_than( V &&value )
			  : m_value( std::forward<V>( value ) ) {}

			template<typename T>
			constexpr bool operator( )( T &&value ) const {
				return std::forward<T>( value ) > m_value;
			}
		}; // greater_than

		template<typename Value>
		class greater_than_or_equal_to {
			Value m_value;

		public:
			template<typename V,
			         daw::enable_when_t<std::is_convertible_v<V, Value>> = nullptr>
			constexpr greater_than_or_equal_to( V &&value )
			  : m_value( std::forward<V>( value ) ) {}

			template<typename T>
			constexpr bool operator( )( T &&value ) const {
				return std::forward<T>( value ) >= m_value;
			}
		}; // greater_than_or_equal_to

		template<typename Value>
		class less_than_or_equal_to {
			Value m_value;

		public:
			template<typename V,
			         daw::enable_when_t<std::is_convertible_v<V, Value>> = nullptr>
			constexpr less_than_or_equal_to( V &&value )
			  : m_value( std::forward<V>( value ) ) {}

			template<typename T>
			constexpr bool operator( )( T &&value ) const {
				return std::forward<T>( value ) <= m_value;
			}
		}; // less_than_or_equal_to
	}    // namespace algorithm_details

	/// @brief Returns a callable that returns true if the value passed is in
	/// the range [Lower, Upper]
	/// @param lower The lowest value allowed
	/// @param upper The largest value allowed
	/// @returns true if value passed to operator( ) is within the range [lower,
	/// upper]
	template<typename Lower, typename Upper>
	constexpr auto in_range( Lower &&lower, Upper &&upper ) {
		return algorithm_details::in_range<Lower, Upper>(
		  std::forward<Lower>( lower ), std::forward<Upper>( upper ) );
	}

	/// @brief Returns a callable that returns true if value passed is equal to
	/// value construct with
	/// @param value Value to test with
	/// @returns True if value passed to operator( ) is equal to value
	/// constructed with
	template<typename Value>
	constexpr auto equal_to( Value &&value ) {
		return algorithm_details::equal_to<Value>( std::forward<Value>( value ) );
	}

	/// @brief Returns a callable that returns true if value passed is greater
	/// than the value constructed with
	/// @param value Value to test with
	/// @returns True if value passed to operator( ) is greater than to value
	/// constructed with
	template<typename Value>
	constexpr auto greater_than( Value &&value ) {
		return algorithm_details::greater_than<Value>(
		  std::forward<Value>( value ) );
	}

	/// @brief Returns a callable that returns true if value passed is greater
	/// than or equal to the value constructed with
	/// @param value Value to test with
	/// @returns True if value passed to operator( ) is greater than or equal to
	/// to value constructed with
	template<typename Value>
	constexpr auto greater_than_or_equal_to( Value &&value ) {
		return algorithm_details::greater_than_or_equal_to<Value>(
		  std::forward<Value>( value ) );
	}

	/// @brief Returns a callable that returns true if value passed is less than
	/// the value constructed with
	/// @param value Value to test with
	/// @returns True if value passed to operator( ) is less than to value
	/// constructed with
	template<typename Value>
	constexpr auto less_than( Value &&value ) {
		return algorithm_details::less_than<Value>( std::forward<Value>( value ) );
	}

	/// @brief Returns a callable that returns true if value passed is less than
	/// or equal to the value constructed with
	/// @param value Value to test with
	/// @returns True if value passed to operator( ) is less than or equal to to
	/// value constructed with
	template<typename Value>
	constexpr auto less_than_or_equal_to( Value &&value ) {
		return algorithm_details::less_than_or_equal_to<Value>(
		  std::forward<Value>( value ) );
	}

	/// @brief Returns true if the first range [first1, last1) is
	/// lexigraphically less than the second range [first2, last2)
	/// @tparam InputIterator1 Iterator type for start of range 1
	/// @tparam LastType1 Type for value at end of range 1
	/// @tparam InputIterator2 Iterator type for start of range 2
	/// @tparam LastType2 Type for value at end of range 2
	/// @tparam Compare Type for comp callback that fullfills Compare concept
	/// @param first1 Start of range 1
	/// @param last1 End of range 1
	/// @param first2 Start of range 1
	/// @param last2 End of Range 1
	/// @param comp Comparison function that returns true if value1 < value2
	/// @return true of range 1 is lexigraphically less than range 2 using
	/// supplied comparison
	template<typename InputIterator1, typename LastType1, typename InputIterator2,
	         typename LastType2, typename Compare = std::less<>>
	constexpr bool lexicographical_compare(
	  InputIterator1 first1, LastType1 last1, InputIterator2 first2,
	  LastType2 last2,
	  Compare comp = Compare{} ) noexcept( noexcept( comp( *first1, *first2 ) !=
	                                                 comp( *first2,
	                                                       *first1 ) ) ) {

		traits::is_input_iterator_test<InputIterator1>( );
		traits::is_input_iterator_test<InputIterator2>( );
		traits::is_compare_test<Compare, decltype( *first1 ),
		                        decltype( *first2 )>( );
		traits::is_compare_test<Compare, decltype( *first2 ),
		                        decltype( *first1 )>( );

		while( ( first1 != last1 ) and ( first2 != last2 ) ) {
			if( daw::invoke( comp, *first1, *first2 ) ) {
				return true;
			}
			if( daw::invoke( comp, *first2, *first1 ) ) {
				return false;
			}
			++first1;
			++first2;
		}
		return ( first1 == last1 ) and ( first2 != last2 );
	}

	template<typename InputIterator1, typename LastType1, typename InputIterator2,
	         typename LastType2, typename LessCompare = std::less<>,
	         typename Equality = std::equal_to<>>
	constexpr int compare_range( InputIterator1 first1, LastType1 last1,
	                             InputIterator2 first2, LastType2 last2,
	                             LessCompare less_comp = LessCompare{},
	                             Equality eq = Equality{} ) {

		traits::is_input_iterator_test<InputIterator1>( );
		traits::is_input_iterator_test<InputIterator2>( );
		static_assert( std::is_invocable_v<LessCompare, decltype( *first1 ),
		                                   decltype( *first2 )> );
		static_assert(
		  std::is_invocable_v<Equality, decltype( *first1 ), decltype( *first2 )> );

		while( first1 != last1 and first2 != last2 ) {
			if( not daw::invoke( eq, *first1, *first2 ) ) {
				if( daw::invoke( less_comp, *first1, *first2 ) ) {
					return -1;
				}
				return 1;
			}
			++first1;
			++first2;
		}
		if( first1 == last1 and first2 == last2 ) {
			return 0;
		}
		if( first1 == last1 ) {
			return -1;
		}
		return 1;
	}

	/// @brief Apply the TransformFunction on the value referenced by range
	/// [first, last) when the predicate returns true for that value
	/// @tparam ForwardIterator Type of Iterator for start of range
	/// @tparam LastType Type for representing end of range
	/// @tparam OutputIterator Iterator for output range
	/// @tparam UnaryPredicate A unary predicate that takes the dereferenced
	/// InputIterator as an arugment
	/// @tparam TransformFunction Takes the dereferenced InputIterator as an
	/// arugment and returns a value assignable to the dereferenced
	/// OutputIterator
	/// @param first first item in range [first, last)
	/// @param last last item in range [first, last)
	/// @param first_out Output iterator written to when predicate returns true
	/// @param pred predicate to determine if a transform should happen
	/// @param trans transform function to convert from input range to output
	/// range
	/// @return The end of the output range
	template<typename ForwardIterator, typename LastType, typename OutputIterator,
	         typename UnaryPredicate, typename TransformFunction>
	constexpr OutputIterator
	transform_if( ForwardIterator first, LastType const last,
	              OutputIterator first_out, UnaryPredicate pred,
	              TransformFunction trans ) {

		traits::is_forward_access_iterator_test<ForwardIterator>( );
		traits::is_input_iterator_test<ForwardIterator>( );
		traits::is_unary_predicate_test<UnaryPredicate, decltype( *first )>( );
		traits::is_output_iterator_test<OutputIterator,
		                                decltype( pred( *first ) )>( );

		static_assert( traits::is_callable_v<TransformFunction, decltype( *first )>,
		               "TransformFunction does not accept a single argument of "
		               "the dereferenced type of first" );

		while( first != last ) {
			if( daw::invoke( pred, *first ) ) {
				*first_out = daw::invoke( trans, *first );
				++first_out;
			}
			++first;
		}
		return first_out;
	}

	/// @brief Run the transform function unary_op on n elements of range
	/// started by first
	/// @tparam InputIterator input range iterator type
	/// @tparam OutputIterator output range iterator type
	/// @tparam UnaryOperation callable that takes the dereferenced value from
	/// input range and is assignable to the dereferenced value of output range
	/// @param first first element in input range [first, first + count)
	/// @param first_out first element in output range [first_out, first_out +
	/// count)
	/// @param count number of items to process
	/// @param unary_op callable that transforms items from input range to items
	/// of output range
	/// @return last item in output range
	template<typename InputIterator, typename OutputIterator,
	         typename UnaryOperation>
	constexpr auto transform_n(
	  InputIterator first, OutputIterator first_out, size_t count,
	  UnaryOperation unary_op ) noexcept( noexcept( *first_out =
	                                                  unary_op( *first ) ) ) {

		traits::is_input_iterator_test<InputIterator>( );
		static_assert(
		  traits::is_callable_v<UnaryOperation, decltype( *first )> or
		    traits::is_callable_v<UnaryOperation, decltype( *first ), size_t>,
		  "UnaryOperation does not accept a single argument of the "
		  "dereferenced type of first or with an addditional argument of "
		  "size_t" );

		traits::is_output_iterator_test<OutputIterator,
		                                decltype( unary_op( *first ) )>( );

		if constexpr( traits::is_callable_v<UnaryOperation, decltype( *first )> ) {
			while( count-- > 0 ) {
				*first_out = unary_op( *first );
				++first;
				++first_out;
			}
		} else {
			for( size_t n = 0U; n < count; ++n, ++first, ++first_out ) {
				*first_out = *unary_op( *first, n );
			}
		}
		struct result_t {
			InputIterator input;
			OutputIterator output;
		};
		return result_t{first, first_out};
	}

	/// @brief Transform range [first, last) and output to range [first_out,
	/// first_out + std::distance( first, last ))
	/// @tparam InputIterator input range iterator type
	/// @tparam LastType type of Iterator marking end of input range
	/// @tparam OutputIterator output range iterator type
	/// @tparam UnaryOperation callable that takes the dereferenced value from
	/// input range and is assignable to the dereferenced value of output range
	/// @param first first element in input range [first, first + count)
	/// @param last end of input range
	/// @param first_out first element in output range [first_out, first_out +
	/// count)
	/// @param unary_op callable that transforms items from input range to items
	/// of output range
	/// @return last item in output range
	template<typename InputIterator, typename LastType, typename OutputIterator,
	         typename UnaryOperation>
	constexpr OutputIterator transform(
	  InputIterator first, LastType last, OutputIterator first_out,
	  UnaryOperation unary_op ) noexcept( std::
	                                        is_nothrow_constructible_v<
	                                          decltype( *first_out ),
	                                          decltype( unary_op( *first ) )> ) {

		static_assert( std::is_assignable_v<decltype( *first_out ),
		                                    decltype( unary_op( *first ) )>,
		               "Cannot assign the result of unary_op to that of the value "
		               "type of the output iterator" );
		traits::is_input_iterator_test<InputIterator>( );

		static_assert( traits::is_callable_v<UnaryOperation, decltype( *first )>,
		               "UnaryOperation does not accept a single argument of the "
		               "dereferenced type of first" );

		traits::is_output_iterator_test<OutputIterator,
		                                decltype( unary_op( *first ) )>( );

		while( first != last ) {
			*first_out = daw::invoke( unary_op, *first );
			++first;
			++first_out;
		}
		return first_out;
	}

	/// @brief Transform input range [first, last) to output range [first_out,
	/// first_out + std::distance(first, last)).
	/// @tparam InputIterator type of Iterator of input range
	/// @tparam LastType type of Iterator marking end of input range
	/// @tparam OutputIterator type of iterator for output range
	/// @tparam BinaryOperation a callable that takes the type of the
	/// dereference input range iterator and the output range as argument,
	/// returning the next position in output range
	/// @param first start of input range
	/// @param last end of input range
	/// @param first_out first item in output range
	/// @param binary_op transformation function
	/// @return end of output range written to
	template<typename InputIterator, typename LastType, typename OutputIterator,
	         typename BinaryOperation>
	constexpr OutputIterator transform_it(
	  InputIterator first, LastType last, OutputIterator first_out,
	  BinaryOperation
	    binary_op ) noexcept( noexcept( first_out = binary_op( *first++,
	                                                           first_out ) ) ) {
		traits::is_input_iterator_test<InputIterator>( );
		traits::is_iterator_test<OutputIterator>( ); // binary_op sets the value
		// so we cannot test if is
		// output iterator

		while( first != last ) {
			first_out = daw::invoke( binary_op, *first, first_out );
			++first;
		}
		return first_out;
	}

	/// @brief Copy input range [first, last) to output range [first_out,
	/// first_out + std::distance( first, last ))
	/// @tparam InputIterator type of Iterator of input range
	/// @tparam LastType type of Iterator marking end of input range
	/// @tparam OutputIterator type of iterator for output range
	/// @param first start of input range
	/// @param last end of input range
	/// @param first_out first item in output range
	/// @return end of output range written to
	template<typename InputIterator, typename LastType, typename OutputIterator>
	constexpr OutputIterator
	copy( InputIterator first, LastType last,
	      OutputIterator first_out ) noexcept( noexcept( *first_out = *first ) ) {

		traits::is_input_iterator_test<InputIterator>( );
		traits::is_output_iterator_test<OutputIterator, decltype( *first )>( );

		while( first != last ) {
			*first_out = *first;
			++first;
			++first_out;
		}
		return first_out;
	}

	/// @brief Copy input range [first, last) to output range [first_out,
	/// first_out + std::distance( first, last ))
	/// @tparam InputIterator type of Iterator of input range
	/// @tparam LastType type of Iterator marking end of input range
	/// @tparam OutputIterator type of iterator for output range
	/// @tparam UnaryPredicate type predicate determining if to copy element
	/// @param first start of input range
	/// @param last end of input range
	/// @param destination first item in output range
	/// @param pred A unary predicate that determines if a copy is done
	/// @return end of output range written to
	template<typename InputIterator, typename LastType, typename OutputIterator,
	         typename UnaryPredicate>
	constexpr OutputIterator copy_if( InputIterator first, LastType last,
	                                  OutputIterator destination,
	                                  UnaryPredicate pred ) {
		static_assert(
		  traits::is_unary_predicate_v<UnaryPredicate, decltype( *first )>,
		  "Compare does not satisfy the Unary Predicate concept.  See "
		  "http://en.cppreference.com/w/cpp/concept/Predicate for more "
		  "information" );

		while( first != last ) {
			if( daw::invoke( pred, *first ) ) {
				*destination = *first;
				++destination;
			}
			++first;
		}
		return destination;
	}

	/// @brief Copy input range [first, last) to output range [first_out,
	/// first_out + count)
	/// @tparam InputIterator type of Iterator of input range
	/// @tparam OutputIterator type of iterator for output range
	/// @param first start of input range
	/// @param first_out first item in output range
	/// @param count number of items to copy
	/// @return end of output range written to
	template<typename InputIterator, typename OutputIterator>
	constexpr auto
	copy_n( InputIterator first, OutputIterator first_out,
	        size_t count ) noexcept( noexcept( *first_out = *first ) ) {

		traits::is_input_iterator_test<InputIterator>( );
		traits::is_output_iterator_test<OutputIterator, decltype( *first )>( );

		while( count-- > 0 ) {
			*first_out = *first;
			++first;
			++first_out;
		}
		struct result_t {
			InputIterator input;
			OutputIterator output;
		};
		return result_t{first, first_out};
	}

	/// @brief Move values from input range [first, last) to output range
	/// [first_out, std::distance( first, last))
	///
	/// @tparam InputIterator type of Iterator of input range
	/// @tparam LastType type of Iterator marking end of input range
	/// @tparam OutputIterator type of iterator for output range
	/// @param first start of input range
	/// @param last end of input range
	/// @param first_out first item in output range
	/// @return end of output range written to
	template<typename InputIterator, typename LastType, typename OutputIterator>
	constexpr OutputIterator
	move( InputIterator first, LastType const last,
	      OutputIterator
	        first_out ) noexcept( noexcept( *first_out = daw::move( *first ) ) ) {

		traits::is_input_iterator_test<InputIterator>( );
		traits::is_output_iterator_test<OutputIterator,
		                                decltype( daw::move( *first ) )>( );

		while( first != last ) {
			*first_out = daw::move( *first );
			++first;
			++first_out;
		}
		return first_out;
	}

	/// @brief Move values from input range [first, last) to output range
	/// [first_out, first_out + count)
	/// @tparam InputIterator type of Iterator of input range
	/// @tparam OutputIterator type of iterator for output range
	/// @param first start of input range
	/// @param first_out first item in output range
	/// @param count number of items to move
	/// @return end of output range written to
	template<typename InputIterator, typename OutputIterator>
	constexpr auto move_n(
	  InputIterator first, OutputIterator first_out,
	  size_t count ) noexcept( noexcept( *first_out = daw::move( *first ) ) ) {

		traits::is_input_iterator_test<InputIterator>( );
		traits::is_output_iterator_test<OutputIterator,
		                                decltype( daw::move( *first ) )>( );

		while( count-- > 0 ) {
			*first_out = daw::move( *first );
			++first;
			++first_out;
		}
		struct result_t {
			InputIterator input;
			OutputIterator output;
		};
		return result_t{first, first_out};
	}

	/// @brief Determine if two ranges [first1, last1) and [first2, last2) using
	/// pred
	/// @tparam InputIterator1 type of Iterator of first input range
	/// @tparam LastType1 type of Iterator marking end of first input range
	/// @tparam InputIterator2 type of Iterator of second input range
	/// @tparam LastType2 type of Iterator marking end of second input range
	/// @tparam Compare type of predicate fullfilling Compare concept
	/// @param first1 start of first input range
	/// @param last1 end of first input range
	/// @param first2 start of second input range
	/// @param last2 end of second input range
	/// @param comp predicate to determine equality of elements
	/// @return true if both ranges are equal
	template<typename InputIterator1, typename LastType1, typename InputIterator2,
	         typename LastType2, typename Compare = std::equal_to<>>
	constexpr bool equal( InputIterator1 first1, LastType1 last1,
	                      InputIterator2 first2, LastType2 last2,
	                      Compare comp = Compare{} ) {

		traits::is_input_iterator_test<InputIterator1>( );
		traits::is_input_iterator_test<InputIterator2>( );
		traits::is_compare_test<Compare, decltype( *first1 ),
		                        decltype( *first2 )>( );

		while( ( first1 != last1 ) and ( first2 != last2 ) and
		       daw::invoke( comp, *first1, *first2 ) ) {
			++first1;
			++first2;
		}
		return first1 == last1 and first2 == last2;
	}

	/// @brief Determine if two ranges [first1, last1) and [first2, last2) using
	/// pred
	/// @tparam InputIterator1 type of Iterator of first input range
	/// @tparam LastType1 type of Iterator marking end of first input range
	/// @tparam InputIterator2 type of Iterator of second input range
	/// @tparam LastType2 type of Iterator marking end of second input range
	/// @tparam Compare type of predicate fullfilling Compare concept
	/// @param first1 start of first input range
	/// @param last1 end of first input range
	/// @param first2 start of second input range
	/// @param last2 end of second input range
	/// @param comp predicate to determine equality of elements
	/// @return true if both ranges are equal
	template<typename InputIterator1, typename LastType1, typename InputIterator2,
	         typename LastType2, typename Compare = std::not_equal_to<>>
	constexpr bool not_equal( InputIterator1 first1, LastType1 last1,
	                          InputIterator2 first2, LastType2 last2,
	                          Compare comp = Compare{} ) {

		traits::is_input_iterator_test<InputIterator1>( );
		traits::is_input_iterator_test<InputIterator2>( );
		traits::is_compare_test<Compare, decltype( *first1 ),
		                        decltype( *first2 )>( );

		while( ( first1 != last1 ) and ( first2 != last2 ) and
		       comp( *first1, *first2 ) ) {
			++first1;
			++first2;
		}
		return first1 == last1 and first2 == last2;
	}

	/// @brief Determine if two ranges [first1, last1) and [first2, first2 +
	/// std::distance( first1, last1 )) are equal
	/// @tparam InputIterator1 type of Iterator of first input range
	/// @tparam LastType type of Iterator marking end of first input range
	/// @tparam InputIterator2 type of Iterator of second input range
	/// @param first1 start of first input range
	/// @param last1 end of first input range
	/// @param first2 start of second input range
	/// @return true if both ranges are equal
	template<typename InputIterator1, typename LastType, typename InputIterator2>
	constexpr bool equal( InputIterator1 first1, LastType last1,
	                      InputIterator2 first2 ) {

		traits::is_input_iterator_test<InputIterator1>( );
		traits::is_input_iterator_test<InputIterator2>( );

		while( first1 != last1 and std::equal_to<>{}( *first1, *first2 ) ) {
			++first1;
			++first2;
		}
		return not( first1 != last1 );
	}

	/// @brief Determine if two ranges [first1, last1) and [first2, first2 +
	/// std::distance( first1, last1 )) are equal
	/// @tparam InputIterator1 type of Iterator of first input range
	/// @tparam LastType type of Iterator marking end of first input range
	/// @tparam InputIterator2 type of Iterator of second input range
	/// @param first1 start of first input range
	/// @param last1 end of first input range
	/// @param first2 start of second input range
	/// @return true if both ranges are equal
	template<typename InputIterator1, typename LastType, typename InputIterator2>
	constexpr bool not_equal( InputIterator1 first1, LastType last1,
	                          InputIterator2 first2 ) {

		traits::is_input_iterator_test<InputIterator1>( );
		traits::is_input_iterator_test<InputIterator2>( );

		while( first1 != last1 and std::not_equal_to<>{}( *first1, *first2 ) ) {
			++first1;
			++first2;
		}
		return not( first1 != last1 );
	}

	/// @brief Returns an iterator pointing to the first element in the range
	/// [first, last) that is greater than value, or last if no such element is
	/// found.
	/// @tparam ForwardIterator Iteratot type pointing to range
	/// @tparam T a value comparable to the dereferenced RandomIterator
	/// @param first first item in range
	/// @param last end of range
	/// @param value value to compare to
	/// @return position of first element greater than value or last
	template<typename ForwardIterator, typename T, typename Compare = std::less<>>
	constexpr ForwardIterator upper_bound(
	  ForwardIterator first, ForwardIterator last, T const &value,
	  Compare comp =
	    Compare{} ) noexcept( noexcept( daw::advance( first, 1 ) ) and
	                          noexcept( ++first ) and
	                          noexcept( daw::distance( first, last ) ) ) {

		traits::is_forward_access_iterator_test<ForwardIterator>( );
		traits::is_input_iterator_test<ForwardIterator>( );

		auto count = daw::distance( first, last );
		while( count > 0 ) {
			auto it = first;
			auto step = count / 2;
			daw::advance( it, step );
			if( not daw::invoke( comp, value, *it ) ) {
				++it;
				first = it;
				count -= step + 1;
			} else {
				count = step;
			}
		}
		return first;
	}

	template<typename RandomIterator, typename Compare = std::less<>>
	constexpr void nth_element(
	  RandomIterator first, RandomIterator nth, RandomIterator const last,
	  Compare comp = Compare{} ) noexcept( noexcept( comp( *first, *nth ) ) &&
	                                       noexcept( daw::cswap( *first,
	                                                             *nth ) ) ) {

		traits::is_random_access_iterator_test<RandomIterator>( );
		traits::is_inout_iterator_test<RandomIterator>( );

		traits::is_compare_test<Compare, decltype( *first ), decltype( *nth )>( );

		if( not( first != last ) ) {
			return;
		}
		while( first != nth ) {
			auto min_idx = first;
			auto j = daw::next( first );
			while( j != last ) {
				if( daw::invoke( comp, *j, *min_idx ) ) {
					min_idx = j;
					daw::cswap( *first, *min_idx );
				}
				++j;
			}
			++first;
		}
	}

	/// @brief Examines the range [first, last) and finds the largest range
	/// beginning at first in which the elements are sorted in ascending order.
	/// @tparam ForwardIterator Iterator type used to hold range
	/// @tparam Compare Comparision function object type that satifies the
	/// Compare concept
	/// @param first first item in range
	/// @param last	end of range
	/// @param comp comparision function object
	/// @return	ForwardIterator with the last sorted item
	template<typename ForwardIterator, typename Compare = std::less<>>
	constexpr ForwardIterator is_sorted_until(
	  ForwardIterator first, ForwardIterator last,
	  Compare comp = Compare{} ) noexcept( noexcept( comp( *first, *first ) ) ) {

		traits::is_forward_access_iterator_test<ForwardIterator>( );
		traits::is_input_iterator_test<ForwardIterator>( );
		traits::is_compare_test<Compare, decltype( *first )>( );

		if( not( first != last ) ) {
			return last;
		}
		auto next_it = daw::next( first );
		while( next_it != last ) {
			if( daw::invoke( comp, *next_it, *first ) ) {
				return next_it;
			}
			first = next_it;
			++next_it;
		}
		return last;
	}

	template<typename ForwardIterator, typename LastType,
	         typename Compare = std::less<>>
	constexpr ForwardIterator is_sorted_until(
	  ForwardIterator first, LastType last,
	  Compare comp = Compare{} ) noexcept( noexcept( comp( *first, *first ) ) ) {

		traits::is_forward_access_iterator_test<ForwardIterator>( );
		traits::is_input_iterator_test<ForwardIterator>( );
		traits::is_compare_test<Compare, decltype( *first )>( );

		if( first != last ) {
			auto i = first;
			while( ++i != last ) {
				if( daw::invoke( comp, *i, *first ) ) {
					return i;
				}
				first = i;
			}
		}
		return last;
	}

	template<typename ForwardIterator, typename LastType,
	         typename Compare = std::less<>>
	constexpr bool is_sorted(
	  ForwardIterator first, LastType last,
	  Compare comp = Compare{} ) noexcept( noexcept( comp( *first, *first ) ) ) {

		return daw::algorithm::is_sorted_until( first, last, comp ) == last;
	}

	template<typename ForwardIterator, typename T>
	constexpr ForwardIterator fill_n( ForwardIterator first, size_t count,
	                                  T &&value ) noexcept {

		traits::is_forward_access_iterator_test<ForwardIterator>( );

		while( count-- > 0 ) {
			*first = value;
			++first;
		}
		return first;
	}

	template<typename ForwardIterator, typename LastType, typename T>
	constexpr void fill( ForwardIterator first, LastType last,
	                     T &&value ) noexcept {

		traits::is_forward_access_iterator_test<ForwardIterator>( );
		while( first != last ) {
			*first = value;
			++first;
		}
	}

	template<typename InputIterator, typename OutputIterator,
	         typename UnaryOperation>
	constexpr OutputIterator
	map( InputIterator first, InputIterator const last, OutputIterator first_out,
	     UnaryOperation unary_op ) noexcept( noexcept( *daw::next( first_out ) =
	                                                     unary_op( *daw::next(
	                                                       first ) ) ) ) {

		traits::is_input_iterator_test<InputIterator>( );
		traits::is_output_iterator_test<OutputIterator>( );
		traits::is_unary_predicate_test<UnaryOperation, decltype( *first )>( );

		while( first != last ) {
			*first_out = daw::invoke( unary_op, *first );
			++first;
			++first_out;
		}
		return first_out;
	}

	template<typename InputIterator1, typename InputIterator2,
	         typename OutputIterator, typename BinaryOperation>
	constexpr OutputIterator map(
	  InputIterator1 first1, InputIterator1 const last1, InputIterator2 first2,
	  OutputIterator first_out,
	  BinaryOperation
	    binary_op ) noexcept( noexcept( *daw::next( first_out ) =
	                                      binary_op( *daw::next( first1 ),
	                                                 *daw::next( first2 ) ) ) ) {

		traits::is_input_iterator_test<InputIterator1>( );
		traits::is_input_iterator_test<InputIterator2>( );
		traits::is_output_iterator_test<OutputIterator>( );
		traits::is_binary_predicate_test<BinaryOperation, decltype( *first1 ),
		                                 decltype( *first2 )>( );

		while( first1 != last1 ) {
			*first_out = daw::invoke( binary_op, *first1, *first2 );
			++first1;
			++first2;
			++first_out;
		}
		return first_out;
	}

	template<typename T, typename RandomIterator, typename RandomIteratorLast,
	         typename BinaryOperation>
	constexpr T reduce(
	  RandomIterator first, RandomIteratorLast last, T init,
	  BinaryOperation
	    binary_op ) noexcept( noexcept( init = binary_op( init, *first++ ) ) ) {

		static_assert(
		  traits::is_binary_predicate_v<BinaryOperation, T, decltype( *first )>,
		  "BinaryOperation passed to reduce must take two values referenced by "
		  "first. e.g binary_op( "
		  "init, *first) ) "
		  "must be valid" );

		static_assert(
		  std::is_convertible_v<decltype( binary_op( init, *first++ ) ), T>,
		  "Result of BinaryOperation must be convertable to type of value "
		  "referenced by RandomIterator. "
		  "e.g. *first = binary_op( *first, *(first + 1) ) must be valid." );

		while( first != last ) {
			init = daw::invoke( binary_op, init, *first );
			++first;
		}
		return daw::move( init );
	}

	template<typename InputIterator1, typename InputIterator1Last,
	         typename InputIterator2, typename T, typename ReduceFunction,
	         typename MapFunction>
	constexpr T map_reduce(
	  InputIterator1 first1, InputIterator1Last last1, InputIterator2 first2,
	  T init, ReduceFunction reduce_func,
	  MapFunction map_func ) noexcept( noexcept( reduce_func( init,
	                                                          map_func(
	                                                            *first1,
	                                                            *first2 ) ) ) ) {

		static_assert( traits::is_iterator_v<InputIterator1>,
		               "Iterator1 passed to map_reduce does not meet the "
		               "requirements of the Iterator concept "
		               "http://en.cppreference.com/w/cpp/concept/Iterator" );

		static_assert( traits::is_iterator_v<InputIterator2>,
		               "Iterator2 passed to map_reduce does not meet the "
		               "requirements of the Iterator concept "
		               "http://en.cppreference.com/w/cpp/concept/Iterator" );

		static_assert(
		  traits::is_binary_predicate_v<MapFunction, decltype( *first1 ),
		                                decltype( *first2 )>,
		  "BinaryOperation map_func passed take two values "
		  "referenced by first. e.g map_func( *first1, "
		  "*first2 ) must be valid" );

		static_assert(
		  traits::is_binary_predicate_v<ReduceFunction, T,
		                                decltype( map_func( *first1, *first2 ) )>,
		  "BinaryOperation reduce_func must take two values referenced by first. "
		  "e.g reduce_func( init, "
		  "map_func( *first1, *first2 ) ) must be valid" );

		while( first1 != last1 ) {
			init = daw::invoke( reduce_func, init,
			                    daw::invoke( map_func, *first1, *first2 ) );
			++first1;
			++first2;
		}
		return init;
	}

	template<typename ForwardIterator1, typename ForwardIterator2,
	         typename Compare = std::equal_to<>>
	constexpr ForwardIterator1
	search( ForwardIterator1 first, ForwardIterator1 last,
	        ForwardIterator2 s_first, ForwardIterator2 s_last,
	        Compare comp =
	          Compare{} ) noexcept( noexcept( not comp( *first, *s_first ) ) ) {

		static_assert(
		  traits::is_forward_access_iterator_v<ForwardIterator1>,
		  "ForwardIterator1 passed to search does not meet the requirements of "
		  "the ForwardIterator concept "
		  "http://en.cppreference.com/w/cpp/concept/ForwardIterator" );

		static_assert(
		  traits::is_forward_access_iterator_v<ForwardIterator2>,
		  "ForwardIterator2 passed to search does not meet the requirements of "
		  "the ForwardIterator concept "
		  "http://en.cppreference.com/w/cpp/concept/ForwardIterator" );

		static_assert(
		  traits::is_compare_v<Compare, decltype( *first ), decltype( *s_first )>,
		  "Compare function does not meet the requirements of the Compare "
		  "concept. "
		  "http://en.cppreference.com/w/cpp/concept/Compare" );

		for( ;; ++first ) {
			ForwardIterator1 it = first;
			for( ForwardIterator2 s_it = s_first;; ++it, ++s_it ) {
				if( s_it == s_last ) {
					return first;
				}
				if( it == last ) {
					return last;
				}
				if( not daw::invoke( comp, *it, *s_it ) ) {
					break;
				}
			}
		}
	}

	template<typename InputIterator, typename T>
	constexpr T accumulate( InputIterator first, InputIterator last,
	                        T init ) noexcept {
		for( ; first != last; ++first ) {
			init = daw::move( init ) + *first;
		}
		return daw::move( init );
	}

	/***
	 * Accumulate values in range
	 * @tparam InputIterator type of first iterator argument
	 * @tparam LastType type of sentinal marking end of range
	 * @tparam T initial value to start accumulating at
	 * @tparam BinaryOperation Callable that takes the current sum and next value
	 * and returns the new sum
	 * @param first beginning position in range
	 * @param last end of range
	 * @param init initial value of sum
	 * @param binary_op operation to run
	 * @return sum of values
	 */
	template<typename InputIterator, typename LastType, typename T,
	         typename BinaryOperation = std::plus<>,
	         daw::enable_when_t<
	           not daw::traits::is_container_like_v<InputIterator>> = nullptr>
	constexpr T accumulate(
	  InputIterator first, LastType last, T init,
	  BinaryOperation binary_op =
	    BinaryOperation{} ) noexcept( noexcept( binary_op( daw::move( init ),
	                                                       *first ) ) ) {

		/*
static_assert(
	traits::is_iterator_v<InputIterator>,
	"Iterator passed to accumulate does not meet the requirements "
	"of the Iterator concept "
	"http://en.cppreference.com/w/cpp/concept/Iterator" );
	*/

		while( first != last ) {
			init = daw::invoke( binary_op, daw::move( init ), *first );
			++first;
		}
		return daw::move( init );
	}

	template<class ForwardIterator, typename Compare = std::less<>>
	constexpr ForwardIterator max_element( ForwardIterator first,
	                                       ForwardIterator last,
	                                       Compare &&comp = Compare{} ) {
		if( first == last ) {
			return last;
		}
		auto largest = first;
		++first;
		while( first != last ) {
			if( daw::invoke( comp, *largest, *first ) ) {
				largest = first;
			}
			++first;
		}
		return largest;
	}

	/// @brief return the min and max of two items sorted
	///	@tparam T of items to evaluate
	/// @tparam Compare predicate that returns true if lhs < rhs
	/// @param a item 1
	/// @param b item 2
	/// @param comp comparison predicate
	/// @return a std::pair<T, T> that has the first member holding min(a, b)
	/// and second max(a, b)
	template<typename T, typename Compare = std::less<>>
	constexpr std::pair<T, T> minmax_item( T a, T b,
	                                       Compare comp = Compare{} ) noexcept {
		static_assert( traits::is_compare_v<Compare, T>,
		               "Compare function does not meet the requirements of the "
		               "Compare concept. "
		               "http://en.cppreference.com/w/cpp/concept/Compare" );
		if( daw::invoke( comp, b, a ) ) {
			return std::pair<T, T>{daw::move( b ), daw::move( a )};
		}
		return std::pair<T, T>{daw::move( a ), daw::move( b )};
	}

	template<typename ForwardIterator, typename LastType,
	         typename Compare = std::less<>>
	constexpr auto minmax_element(
	  ForwardIterator first, LastType last,
	  Compare comp = Compare{} ) noexcept( noexcept( comp( *first, *first ) ) ) {

		traits::is_forward_access_iterator_test<ForwardIterator>( );
		traits::is_input_iterator_test<ForwardIterator>( );
		traits::is_compare_test<Compare, decltype( *first )>( );

		struct {
			ForwardIterator min_element;
			ForwardIterator max_element;
		} result{first, first};

		if( not( first != last ) ) {
			return result;
		}
		++first;
		if( not( first != last ) ) {
			return result;
		}
		if( daw::invoke( comp, *first, *result.min_element ) ) {
			result.min_element = first;
		} else {
			result.max_element = first;
		}

		first = daw::next( first );
		while( first != last ) {
			auto i = first;
			first = daw::next( first );
			if( not( first != last ) ) {
				if( daw::invoke( comp, *i, *result.min_element ) ) {
					result.min_element = i;
				} else if( not daw::invoke( comp, *i, *result.max_element ) ) {
					result.max_element = i;
				}
				break;
			} else {
				if( daw::invoke( comp, *first, *i ) ) {
					if( daw::invoke( comp, *first, *result.min_element ) ) {
						result.min_element = first;
					}
					if( not( daw::invoke( comp, *i, *result.max_element ) ) ) {
						result.max_element = i;
					}
				} else {
					if( daw::invoke( comp, *i, *result.min_element ) ) {
						result.min_element = i;
					}
					if( not( daw::invoke( comp, *first, *result.max_element ) ) ) {
						result.max_element = first;
					}
				}
			}
			++first;
		}
		return result;
	}

	template<typename InputIterator1, typename LastType1, typename InputIterator2,
	         typename LastType2, typename OutputIterator,
	         typename Compare = std::less<>>
	constexpr OutputIterator set_intersection(
	  InputIterator1 first1, LastType1 last1, InputIterator2 first2,
	  LastType2 last2, OutputIterator d_first,
	  Compare &&comp =
	    Compare{} ) noexcept( noexcept( comp( *first2, *first1 ) ) and
	                          noexcept( comp( *first1, *first2 ) ) ) {

		while( first1 != last1 and first2 != last2 ) {
			if( daw::invoke( comp, *first1, *first2 ) ) {
				++first1;
			} else {
				if( not daw::invoke( comp, *first2, *first1 ) ) {
					*d_first++ = *first1++;
				}
				++first2;
			}
		}
		return d_first;
	}

	template<typename Iterator, typename EndIterator, typename T>
	constexpr void iota( Iterator first, EndIterator last, T start_value ) {
		while( first != last ) {
			*first = start_value;
			++first;
			++start_value;
		}
	}

	template<typename Iterator, typename EndIterator, typename T, typename Setter>
	constexpr void iota( Iterator first, EndIterator last, T start_value,
	                     Setter setter ) {
		while( first != last ) {
			setter( *first, start_value );
			++first;
			++start_value;
		}
	}

	template<typename ForwardIterator, typename T>
	constexpr ForwardIterator remove( ForwardIterator first, ForwardIterator last,
	                                  T const &value ) {
		first = daw::algorithm::find( first, last, value );
		if( first != last ) {
			for( ForwardIterator i = first; ++i != last; ) {
				if( not( *i == value ) ) {
					*first++ = daw::move( *i );
				}
			}
		}
		return first;
	}

	template<class ForwardIterator, class UnaryPredicate>
	constexpr ForwardIterator remove_if(
	  ForwardIterator first, ForwardIterator last,
	  UnaryPredicate &&pred ) noexcept( noexcept( daw::invoke( pred,
	                                                           *first ) ) ) {
		first = daw::algorithm::find_if( first, last, pred );
		if( first != last ) {
			for( ForwardIterator i = first; ++i != last; ) {
				if( not daw::invoke( pred, *i ) ) {
					*first++ = daw::move( *i );
				}
			}
		}
		return first;
	}

	template<class ForwardIterator, class UnaryPredicate, typename Function>
	constexpr ForwardIterator consume_if(
	  ForwardIterator first, ForwardIterator last, UnaryPredicate &&pred,
	  Function &&func ) noexcept( noexcept( daw::invoke( pred, *first ) ) ) {
		first = daw::algorithm::find_if( first, last, pred );
		if( first != last ) {
			for( ForwardIterator i = first; ++i != last; ) {
				if( not daw::invoke( pred, *i ) ) {
					(void)daw::invoke( func, *i );
					*first++ = daw::move( *i );
				}
			}
		}
		return first;
	}

	template<typename Compare = std::equal_to<>>
	struct all_equal {
		template<typename ForwardIterator, typename LastType>
		constexpr bool operator( )( ForwardIterator first, LastType last,
		                            Compare comp = Compare{} ) const {
			if( first == last ) {
				return true;
			}
			for( auto it = std::next( first ); it != last; ++it ) {
				if( not comp( *first, *it ) ) {
					return false;
				}
			}
			return true;
		}
	};

	template<typename Function, typename Iterator1, typename LastType,
	         typename OutputIterator, typename... Iterators>
	constexpr OutputIterator
	cartesian_product_map( Function func, Iterator1 first1, LastType last1,
	                       OutputIterator out_it, Iterators... its ) {

		static_assert(
		  std::is_invocable_v<Function, decltype( *first1 ), decltype( *its )...>,
		  "Function must be invokable with dereferenced iterators" );
		while( first1 != last1 ) {
			// std invoke isn't constexpr
			*out_it = daw::invoke( func, *first1, *its... );
			daw::advance_many( 1, first1, out_it, its... );
		}
		return out_it;
	}
	template<typename InputIt1, typename InputIt2, typename OutputIt,
	         typename Func>
	[[deprecated( "Use cartesian_product_map" )]] constexpr OutputIt
	transform_many( InputIt1 first1, InputIt1 last1, InputIt2 first2,
	                OutputIt first_out, Func func ) {

		return cartesian_product_map( daw::move( func ), first1, last1, first_out,
		                              first2 );
	}

	template<typename InputIt1, typename InputIt2, typename InputIt3,
	         typename OutputIt, typename Func>
	[[deprecated( "Use cartesian_product_map" )]] constexpr OutputIt
	transform_many( InputIt1 first1, InputIt1 last1, InputIt2 first2,
	                InputIt3 first3, OutputIt first_out, Func func ) {

		return cartesian_product_map( daw::move( func ), first1, last1, first_out,
		                              first2, first3 );
	}

	template<typename InputIt1, typename InputIt2, typename InputIt3,
	         typename InputIt4, typename OutputIt, typename Func>
	[[deprecated( "Use cartesian_product_map" )]] constexpr OutputIt
	transform_many( InputIt1 first1, InputIt1 last1, InputIt2 first2,
	                InputIt3 first3, InputIt4 first4, OutputIt first_out,
	                Func func ) {

		return cartesian_product_map( daw::move( func ), first1, last1, first_out,
		                              first2, first3, first4 );
	}

	template<typename InputIt1, typename InputIt2, typename InputIt3,
	         typename InputIt4, typename InputIt5, typename OutputIt,
	         typename Func>
	[[deprecated( "Use cartesian_product_map" )]] constexpr OutputIt
	transform_many( InputIt1 first1, InputIt1 last1, InputIt2 first2,
	                InputIt3 first3, InputIt4 first4, InputIt4 first5,
	                OutputIt first_out, Func func ) {

		return cartesian_product_map( daw::move( func ), first1, last1, first_out,
		                              first2, first3, first4, first5 );
	}

	template<typename Function, typename Iterator1, typename LastType,
	         typename... Iterators>
	constexpr void cartesian_product( Function func, Iterator1 first1,
	                                  LastType last1, Iterators... its ) {

		static_assert(
		  std::is_invocable_v<Function, decltype( *first1 ), decltype( *its )...>,
		  "Function must be invokable with dereferenced iterators" );

		while( first1 != last1 ) {
			// std invoke isn't constexpr
			daw::invoke( func, *first1, *its... );
			daw::advance_many( 1, first1, its... );
		}
	}

	template<typename InputIterator, typename OutputIterator,
	         typename BinaryOperator = std::plus<>>
	constexpr OutputIterator partial_sum( InputIterator first, InputIterator last,
	                                      OutputIterator first_out,
	                                      BinaryOperator op = BinaryOperator{} ) {

		if( first == last ) {
			return first_out;
		}

		using value_type = typename std::iterator_traits<InputIterator>::value_type;

		value_type sum = *first;
		*first_out = sum;
		++first_out;
		++first;
		while( first != last ) {
			sum = daw::invoke( op, daw::move( sum ), *first );
			++first;

			*first_out = sum;
			++first_out;
		}
		return first_out;
	}

	namespace algorithm_details {
		template<intmax_t Pos0, intmax_t Pos1, typename Iterator,
		         typename Compare = std::less<>>
		constexpr void swap_if( Iterator first,
		                        Compare comp = Compare{} ) noexcept {
			auto const f = std::next( first, Pos0 );
			auto const l = std::next( first, Pos1 );
			if( not comp( *f, *l ) ) {
				daw::iter_swap( f, l );
			}
		}
	} // namespace algorithm_details

	template<typename From, typename To, typename Query>
	constexpr void extract_to( From &from, To &to, Query &&q ) {
		while( auto &&item = from.extract( q ) ) {
			to.insert( std::move( item ) );
		}
	}

	template<typename From, typename To>
	constexpr void extract_all( From &from, To &to ) {
		while( not from.empty( ) ) {
			to.insert( from.extract( std::begin( from ) ) );
		}
	}

	template<typename From, typename To, typename Predicate>
	constexpr void extract_matching( From &from, To &to, Predicate &&pred ) {
		static_assert(
		  std::is_invocable_v<Predicate, decltype( *std::begin( from ) )> );
		auto it = std::begin( from );
		while( it != std::end( from ) ) {
			if( daw::invoke( pred, *it ) ) {
				// Iterator is invalidated inside extract_to,
				// incrementing here ensures that we do not have an
				// iterator to the current item
				extract_to( from, to, it++ );
			} else {
				++it;
			}
		}
	}

	template<class InputIt, class UnaryPredicate>
	constexpr bool all_of( InputIt first, InputIt last, UnaryPredicate &&p ) {
		return daw::algorithm::find_if_not(
		         first, last, std::forward<UnaryPredicate>( p ) ) == last;
	}

	template<class InputIt, class UnaryPredicate>
	constexpr bool any_of( InputIt first, InputIt last, UnaryPredicate &&p ) {
		return daw::algorithm::find_if( first, last,
		                                std::forward<UnaryPredicate>( p ) ) != last;
	}
	template<class InputIt, class UnaryPredicate>
	constexpr bool none_of( InputIt first, InputIt last, UnaryPredicate &&p ) {
		return find_if( first, last, std::forward<UnaryPredicate>( p ) ) == last;
	}

	template<size_t MinSize = 1, typename BidirectionalIterator>
	std::vector<daw::view<BidirectionalIterator>>
	partition_range( BidirectionalIterator first, BidirectionalIterator last,
	                 size_t count ) {
		static_assert( MinSize > 0 );
		auto v = daw::view( first, last );
		auto result = std::vector<daw::view<BidirectionalIterator>>( );
		result.reserve( count );
		auto sz = v.size( ) / count;
		if( sz < MinSize ) {
			sz = MinSize;
		}
		while( count-- > 1 ) {
			result.push_back( v.pop_front( sz ) );
		}
		result.push_back( v );
		return result;
	}

	template<size_t MinSize = 1, typename BidirectionalIterator>
	std::vector<daw::view<BidirectionalIterator>>
	partition_range( daw::view<BidirectionalIterator> rng, size_t count ) {
		return partition_range<MinSize>( rng.begin( ), rng.end( ), count );
	}

	template<typename Iterator, typename Predicate>
	constexpr std::optional<size_t> find_index_if( Iterator first, Iterator last,
	                                               Predicate &&pred ) {
		if( first == last ) {
			return {};
		}
		size_t idx = 0;
		while( first != last ) {
			if( pred( *first ) ) {
				return {idx};
			}
			++idx;
			++first;
		}
		return {};
	}

	template<typename Iterator, typename T>
	constexpr std::optional<size_t> find_index( Iterator first, Iterator last,
	                                            T const &value ) {

		return find_index_if( first, last, [&]( auto &&v ) {
			return std::forward<decltype( v )>( v ) == value;
		} );
	}

	template<typename RandomIterator, typename Compare = std::less<>>
	RandomIterator find_unsorted( RandomIterator first, RandomIterator last,
	                              Compare comp = Compare{} ) {
		auto second = std::next( first );
		auto second_last = std::prev( last );

		auto result = std::mismatch( first, second_last, second,
		                             [&]( auto const &lhs, auto const &rhs ) {
			                             return not comp( rhs, lhs );
		                             } )
		                .first;
		if( result == second_last ) {
			return last;
		}
		return result;
	}

	/***
	 * Count the number of times the predicate returns true
	 * @param first beginning of range
	 * @param last end of range
	 * @param pred unary predicate
	 * @return count of the number of times the prediate returns true
	 */
	template<typename ResultType = size_t, typename Iterator, typename Last,
	         typename Predicate>
	constexpr ResultType count_if( Iterator first, Last last, Predicate pred ) {
		static_assert( std::is_integral_v<ResultType> );
		ResultType result = 0;
		while( first != last ) {
			if( pred( *first ) ) {
				++result;
			}
			++first;
		}
		return result;
	}

	template<typename Iterator, typename IteratorLast, typename Value>
	constexpr size_t find_index_of( Iterator first, IteratorLast last,
	                                Value const &v ) {
		auto const beggining = first;
		while( first != last and *first != v ) {
			++first;
		}
		return static_cast<size_t>( std::distance( beggining, first ) );
	}

	template<typename Iterator, typename IteratorLast, typename Predicate>
	constexpr size_t find_index_of_if( Iterator first, IteratorLast last,
	                                   Predicate pred ) {
		auto const beginning = first;
		while( first != last and not pred( *first ) ) {
			++first;
		}
		return static_cast<size_t>( std::distance( beginning, first ) );
	}

	template<typename Iterator, typename Last, typename Value,
	         typename Compare = std::equal_to<>>
	constexpr bool contains( Iterator first, Last last, Value const &value,
	                         Compare cmp = Compare{} ) {
		while( first != last ) {
			if( cmp( *first, value ) ) {
				return true;
			}
			++first;
		}
		return false;
	}

	template<typename Iterator, typename Last, typename Predicate>
	constexpr bool contains_if( Iterator first, Last last, Predicate pred ) {
		while( first != last ) {
			if( pred( *first ) ) {
				return true;
			}
			++first;
		}
		return false;
	}

	template<typename ForwardIterator, typename BinaryPredicate>
	constexpr ForwardIterator adjacent_find( ForwardIterator first,
	                                         ForwardIterator last,
	                                         BinaryPredicate p ) {
		if( first == last ) {
			return last;
		}
		ForwardIterator next = first;
		++next;
		while( next != last ) {
			if( p( *first, *next ) ) {
				return first;
			}
			++next;
			++first;
		}
		return last;
	}
} // namespace daw::algorithm
