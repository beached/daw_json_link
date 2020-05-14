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
#include <cstddef>
#include <iterator>
#include <numeric>
#include <random>
#include <utility>
#include <vector>

#include "daw_algorithm.h"
#include "daw_range_common.h"
#include "daw_range_reference.h"
#include "daw_reference.h"
#include "daw_traits.h"

namespace daw {
	namespace range {
		template<typename Iterator>
		class ReferenceRange;

		template<typename T>
		struct CollectionRange {
			using is_range_collection = std::true_type;
			using value_type = daw::traits::root_type_t<T>;
			using values_type = std::vector<value_type>;

		private:
			values_type m_values;

		public:
			using reference = typename values_type::reference;
			using const_reference = typename values_type::const_reference;
			using iterator = typename values_type::iterator;
			using const_iterator = typename values_type::const_iterator;
			using difference_type =
			  typename std::iterator_traits<iterator>::difference_type;

			CollectionRange( ) = default;

			template<typename Collection>
			CollectionRange( Collection const &collection )
			  : m_values( impl::to_vector( collection ) ) {}

			template<typename IteratorF, typename IteratorL>
			CollectionRange( IteratorF first, IteratorL last )
			  : m_values( impl::to_vector( first, last ) ) {}

			CollectionRange( CollectionRange const & ) = default;
			CollectionRange( CollectionRange && ) = default;
			~CollectionRange( ) = default;
			CollectionRange &operator=( CollectionRange const & ) = default;
			CollectionRange &operator=( CollectionRange && ) = default;

			bool at_end( ) const noexcept {
				return begin( ) == end( );
			}

			bool empty( ) const noexcept {
				return !( begin( ) != end( ) );
			}

			iterator begin( ) noexcept {
				return m_values.begin( );
			}

			iterator end( ) noexcept {
				return m_values.end( );
			}

			const_iterator begin( ) const noexcept {
				return m_values.begin( );
			}

			const_iterator end( ) const noexcept {
				return m_values.end( );
			}

			const_iterator cbegin( ) const noexcept {
				return m_values.cbegin( );
			}

			const_iterator cend( ) const noexcept {
				return m_values.cend( );
			}

			reference front( ) {
				return m_values.front( );
			}

			const_reference front( ) const {
				return m_values.front( );
			}

			reference back( ) {
				return m_values.back( );
			}

			const_reference back( ) const {
				return m_values.back( );
			}

			size_t size( ) const noexcept {
				return m_values.size( );
			}

			reference operator[]( size_t pos ) {
				return m_values[pos].get( );
			}

			const_reference operator[]( size_t pos ) const {
				return m_values[pos].get( );
			}

			bool operator==( CollectionRange const &other ) const {
				return std::equal( begin( ), end( ), other.begin( ), other.end( ) );
			}

			bool operator!=( CollectionRange const &other ) const {
				return !std::equal( begin( ), end( ), other.begin( ), other.end( ) );
			}

			template<typename... Args>
			void push_back( Args &&... args ) {
				m_values.push_back( std::forward<Args>( args )... );
			}

			template<typename Value>
			iterator find( Value const &value ) const {
				return std::find( begin( ), end( ), value );
			}

			template<typename UnaryPredicate>
			iterator find_if( UnaryPredicate predicate ) const {
				return std::find_if( begin( ), end( ), predicate );
			}

			template<typename U>
			decltype( auto ) accumulate( U &&init ) const {
				return std::accumulate( begin( ), end( ), std::forward<U>( init ) );
			}

			template<typename U, typename BinaryOperator>
			decltype( auto ) accumulate( U &&init, BinaryOperator oper ) const {
				return std::accumulate( begin( ), end( ), std::forward<U>( init ),
				                        oper );
			}

			template<typename Value>
			bool contains( Value const &value ) const {
				return std::find( begin( ), end( ), value ) != end( );
			}

			template<typename Value, typename UnaryPredicate>
			bool contains( Value const &value, UnaryPredicate predicate ) const {
				auto pred2 = [&value, &predicate]( Value const &val ) {
					return predicate( value, val );
				};
				return std::find_if( begin( ), end( ), pred2 ) != end( );
			}

			decltype( auto ) sort( ) const {
				return make_range_reference( *this ).sort( );
			}

			CollectionRange &sort( ) {
				std::sort( begin( ), end( ) );
				return *this;
			}

			template<typename UnaryPredicate>
			auto sort( UnaryPredicate predicate ) const {
				return make_range_reference( *this ).sort( predicate );
			}

			template<typename UnaryPredicate>
			CollectionRange &sort( UnaryPredicate predicate ) {
				std::sort( begin( ), end( ), predicate );
				return *this;
			}

			decltype( auto ) stable_sort( ) const {
				return make_range_reference( *this ).stable_sort( );
			}

			CollectionRange &stable_sort( ) {
				std::stable_sort( begin( ), end( ) );
				return *this;
			}

			template<typename UnaryPredicate>
			decltype( auto ) stable_sort( UnaryPredicate predicate ) const {
				return make_range_reference( *this ).stable_sort( predicate );
			}

			template<typename UnaryPredicate>
			CollectionRange &stable_sort( UnaryPredicate predicate ) {
				std::stable_sort( begin( ), end( ), predicate );
				return *this;
			}

			decltype( auto ) unique( ) const {
				return make_range_reference( *this ).unique( );
			}

			CollectionRange &unique( ) {
				m_values.erase( std::unique( m_values.begin( ), m_values.end( ) ),
				                m_values.end( ) );
				return *this;
			}

			template<typename UnaryPredicate>
			decltype( auto ) unique( UnaryPredicate predicate ) const {
				return make_range_reference( *this ).unique( predicate );
			}

			template<typename UnaryPredicate>
			CollectionRange &unique( UnaryPredicate predicate ) {
				m_values.erase(
				  std::unique( m_values.begin( ), m_values.end( ), predicate ),
				  m_values.end( ) );
				return *this;
			}

			template<typename UnaryPredicate>
			decltype( auto ) partition( UnaryPredicate predicate ) const {
				return make_range_reference( *this ).partition( predicate );
			}

			template<typename UnaryPredicate>
			CollectionRange &partition( UnaryPredicate predicate ) {
				std::partition( begin( ), end( ), predicate );
				return *this;
			}

			template<typename UnaryPredicate>
			decltype( auto ) stable_partition( UnaryPredicate predicate ) const {
				return make_range_reference( *this ).stable_partition( predicate );
			}

			template<typename UnaryPredicate>
			CollectionRange &stable_partition( UnaryPredicate predicate ) {
				std::stable_partition( begin( ), end( ), predicate );
				return *this;
			}

			template<typename UnaryOperator>
			CollectionRange &transform( UnaryOperator oper ) const {
				using v_t = daw::traits::root_type_t<decltype( oper( front( ) ) )>;
				auto result = CollectionRange<v_t>( );
				std::transform( std::begin( m_values ), std::end( m_values ),
				                std::back_inserter( result ), oper );
				return result;
			}

			template<typename UnaryPredicate>
			CollectionRange &erase( UnaryPredicate predicate ) {
				m_values.erase(
				  std::remove_if( m_values.begin( ), m_values.end( ), predicate ),
				  m_values.end( ) );
				return *this;
			}

			template<typename UnaryPredicate>
			decltype( auto ) erase( UnaryPredicate predicate ) const {
				return make_range_reference( *this ).erase( predicate );
			}

			template<typename Value>
			decltype( auto ) erase_where_equal_to( Value const &value ) {
				return erase( [&value]( auto const &current_value ) {
					return value == current_value;
				} );
			}

			template<typename Value>
			decltype( auto ) erase_where_equal_to( Value const &value ) const {
				return make_range_reference( *this ).erase_where_equal_to( value );
			}

			template<typename UnaryPredicate>
			decltype( auto ) where( UnaryPredicate predicate ) {
				return erase(
				  [predicate]( auto const &v ) { return !predicate( v ); } );
			}

			template<typename UnaryPredicate>
			decltype( auto ) where( UnaryPredicate predicate ) const {
				return make_range_reference( *this ).where( predicate );
			}

			template<typename Value>
			decltype( auto ) where_equal_to( Value const &value ) {
				return where( [&value]( auto const &current_value ) {
					return value == current_value;
				} );
			}

			template<typename Value>
			decltype( auto ) where_equal_to( Value const &value ) const {
				return make_range_reference( *this ).where_equal_to( value );
			}

			template<typename Container>
			decltype( auto ) as( ) const {
				Container result;
				std::transform( begin( ), end( ), std::back_inserter( result ),
				                []( auto const &rv ) { return rv.get( ); } );
				return result;
			}

			decltype( auto ) as_vector( ) const {
				return as<values_type>( );
			}

			template<typename Function>
			CollectionRange &for_each( Function function ) const {
				for( auto const &v : m_values ) {
					function( v );
				}
				return *this;
			}

			template<typename Function>
			CollectionRange &for_each( Function function ) {
				for( auto &v : m_values ) {
					function( v );
				}
				return *this;
			}

			template<typename UniformRandomNumberGenerator>
			CollectionRange &shuffle( UniformRandomNumberGenerator &&urng ) {
				std::shuffle( begin( ), end( ),
				              std::forward<UniformRandomNumberGenerator>( urng ) );
				return *this;
			}

			template<typename UniformRandomNumberGenerator>
			decltype( auto ) shuffle( UniformRandomNumberGenerator &&urng ) const {
				return make_range_reference( *this ).shuffle(
				  std::forward<UniformRandomNumberGenerator>( urng ) );
			}

			CollectionRange &shuffle( ) {
				static std::random_device rd;
				static std::mt19937 g( rd( ) );
				return shuffle( g );
			}

			decltype( auto ) shuffle( ) const {
				return make_range_reference( *this ).shuffle( );
			}
		}; // struct CollectionRange

		namespace impl {
			template<typename ValueType>
			CollectionRange<ValueType> make_range_collection( ) {
				return CollectionRange<ValueType>{};
			}

			template<typename Container, typename = void>
			auto make_range_collection( Container const &container ) {
				using ValueType = impl::cleanup_t<typename std::iterator_traits<
				  typename Container::iterator>::value_type>;
				return CollectionRange<ValueType>{container};
			}

			template<typename IteratorF, typename IteratorL>
			auto make_range_collection( IteratorF first, IteratorL last ) {
				using ValueType =
				  impl::cleanup_t<typename std::iterator_traits<IteratorF>::value_type>;
				return CollectionRange<ValueType>{first, last};
			}

			template<typename Iterator>
			auto make_range_collection(
			  daw::range::ReferenceRange<Iterator> const &collection ) {
				using ValueType =
				  impl::cleanup_t<typename std::iterator_traits<Iterator>::value_type>;
				CollectionRange<ValueType> result{};
				std::transform( std::begin( collection ), std::end( collection ),
				                std::back_inserter( result ),
				                []( auto const &rv ) { return rv.get( ); } );
				return result;
			}
		} // namespace impl

		template<typename Arg, typename... Args>
		decltype( auto ) make_range_collection( Arg &&arg, Args &&... args ) {
			return daw::range::impl::make_range_collection(
			  std::forward<Arg>( arg ), std::forward<Args>( args )... );
		}

		namespace details {
			template<typename T>
			std::false_type is_range_collection_impl( T const &, long );

			template<typename T>
			auto is_range_collection_impl( T const &value, int ) ->
			  typename T::is_range_collection;
		} // namespace details

		template<typename T>
		struct is_range_collection : decltype( details::is_range_collection_impl(
		                               std::declval<T const &>( ), 1 ) ) {};

		template<typename T>
		using is_range_collection_t = typename is_range_collection<T>::type;

		template<typename T>
		constexpr bool is_range_collection_v = is_range_collection<T>::value;

		namespace impl {
			template<typename Arg, typename... Args>
			decltype( auto ) from( std::true_type, Arg &&arg, Args &&... args ) {
				return make_range_reference( std::forward<Arg>( arg ),
				                             std::forward<Args>( args )... );
			}

			template<typename Arg, typename... Args>
			decltype( auto ) from( std::false_type, Arg &&arg, Args &&... args ) {
				return make_range_collection( std::forward<Arg>( arg ),
				                              std::forward<Args>( args )... );
			}

		} // namespace impl

		template<typename Arg, typename... Args>
		decltype( auto ) from( Arg &&arg, Args &&... args ) {
			return impl::from( typename std::is_const<Arg>::type{},
			                   std::forward<Arg>( arg ),
			                   std::forward<Args>( args )... );
		}

		template<typename Arg, typename... Args>
		auto from_mutable( Arg &&arg, Args &&... args ) {
			return impl::from( std::false_type( ), std::forward<Arg>( arg ),
			                   std::forward<Args>( args )... );
		}

		template<typename OStream, typename T,
		         std::enable_if_t<daw::traits::is_ostream_like_lite_v<OStream>,
		                          std::nullptr_t> = nullptr>
		OStream &operator<<( OStream &os, CollectionRange<T> const &rng ) {
			os << "{";
			if( !rng.empty( ) ) {
				for( auto it = rng.cbegin( ); it != rng.cend( ); ++it ) {
					os << " " << *it;
				}
			}
			os << " }";
			return os;
		}
	} // namespace range
} // namespace daw
