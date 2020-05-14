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
#include "daw_range_collection.h"
#include "daw_range_common.h"
#include "daw_reference.h"
#include "daw_traits.h"

namespace daw {
	namespace range {

		template<typename Iterator>
		class ReferenceRange {
			using is_range_reference = std::true_type;
			using referenced_value_type =
			  std::remove_cv_t<typename std::iterator_traits<Iterator>::value_type>;
			using values_type = std::vector<daw::Reference<referenced_value_type>>;
			values_type mutable m_values;

		public:
			using value_type = decltype( m_values.front( ).get( ) );
			using reference = decltype( m_values.front( ) );
			using const_reference = const reference;
			using iterator = Iterator;
			using const_iterator = const iterator;
			using difference_type = typename std::iterator_traits<decltype(
			  m_values.begin( ) )>::difference_type;

			ReferenceRange( ) = default;
			ReferenceRange( ReferenceRange const & ) = default;
			ReferenceRange( ReferenceRange && ) = default;
			~ReferenceRange( ) = default;
			ReferenceRange &operator=( ReferenceRange const & ) = default;
			ReferenceRange &operator=( ReferenceRange && ) = default;

			ReferenceRange( iterator first, iterator last )
			  : m_values( daw::to_reference_vector( first, last ) ) {}

			bool at_end( ) const {
				return begin( ) == end( );
			}

			bool empty( ) const {
				return !( begin( ) != end( ) );
			}

			auto begin( ) {
				return m_values.begin( );
			}

			auto begin( ) const {
				return m_values.begin( );
			}

			auto cbegin( ) const {
				return m_values.begin( );
			}

			auto end( ) {
				return m_values.end( );
			}

			auto end( ) const {
				return m_values.end( );
			}

			auto cend( ) const {
				return m_values.end( );
			}

			auto &front( ) {
				return m_values.front( );
			}

			auto const &front( ) const {
				return m_values.front( );
			}

			auto &back( ) {
				return m_values.back( );
			}

			auto const &back( ) const {
				return m_values.back( );
			}

			auto size( ) const {
				return m_values.size( );
			}

			auto &operator[]( size_t pos ) {
				return m_values[pos].get( );
			}

			auto const &operator[]( size_t pos ) const {
				return m_values[pos].get( );
			}

			bool operator==( ReferenceRange const &other ) const {
				return m_values == other.m_values;
			}

			bool operator!=( ReferenceRange const &other ) const {
				return m_values != other.m_values;
			}

			template<typename Value>
			auto find( Value const &value ) const {
				return std::find( begin( ), end( ), value );
			}

			template<typename UnaryPredicate>
			auto find_if( UnaryPredicate predicate ) const {
				return std::find_if( begin( ), end( ), predicate );
			}

			ReferenceRange copy( ) const {
				return ReferenceRange( *this );
			}

			ReferenceRange &sort( ) {
				std::sort( begin( ), end( ) );
				return *this;
			}

			ReferenceRange sort( ) const {
				return copy( ).sort( );
			}

			template<typename UnaryPredicate>
			ReferenceRange &sort( UnaryPredicate predicate ) {
				std::sort( begin( ), end( ), [&predicate]( auto const &v ) {
					return predicate( v.get( ) );
				} );
				return *this;
			}

			template<typename UnaryPredicate>
			ReferenceRange sort( UnaryPredicate predicate ) const {
				return copy( ).sort( predicate );
			}

			ReferenceRange &stable_sort( ) {
				std::stable_sort( begin( ), end( ) );
				return *this;
			}

			ReferenceRange stable_sort( ) const {
				return copy( ).stable_sort( );
			}

			template<typename UnaryPredicate>
			ReferenceRange &stable_sort( UnaryPredicate predicate ) {
				std::stable_sort( begin( ), end( ), [&predicate]( auto const &v ) {
					return predicate( v.get( ) );
				} );
				return *this;
			}

			template<typename UnaryPredicate>
			ReferenceRange stable_sort( UnaryPredicate predicate ) const {
				return copy( ).sort( predicate );
			}

			ReferenceRange &unique( ) {
				erase( std::unique( begin( ), end( ) ), end( ) );
				return *this;
			}

			ReferenceRange unique( ) const {
				return copy( ).unique( );
			}

			template<typename UnaryPredicate>
			ReferenceRange &unique( UnaryPredicate predicate ) {
				erase( std::unique( begin( ), end( ),
				                    [&predicate]( auto const &v ) {
					                    return predicate( v.get( ) );
				                    } ),
				       end( ) );
				return *this;
			}

			template<typename UnaryPredicate>
			ReferenceRange unique( UnaryPredicate predicate ) const {
				return copy( ).unique( predicate );
			}

			template<typename UnaryPredicate>
			ReferenceRange &partition( UnaryPredicate predicate ) {
				std::partition( begin( ), end( ), [&predicate]( auto const &v ) {
					return predicate( v.get( ) );
				} );
				return *this;
			}

			template<typename UnaryPredicate>
			ReferenceRange partition( UnaryPredicate predicate ) const {
				return copy( ).partition( predicate );
			}

			template<typename UnaryPredicate>
			auto partition_it( UnaryPredicate predicate ) {
				auto mid = std::partition( begin( ), end( ), predicate );
				return std::make_pair( mid, *this );
			}

			template<typename UnaryPredicate>
			auto partition_it( UnaryPredicate predicate ) const {
				return copy( ).partition_it( predicate );
			}

			template<typename UnaryPredicate>
			ReferenceRange &stable_partition( UnaryPredicate predicate ) {
				std::stable_partition(
				  m_values.begin( ), m_values.end( ),
				  [&predicate]( auto const &v ) { return predicate( v.get( ) ); } );
				return *this;
			}

			template<typename UnaryPredicate>
			ReferenceRange stable_partition( UnaryPredicate predicate ) const {
				return copy( ).stable_partition( predicate );
			}

			template<typename UnaryPredicate>
			auto stable_partition_it( UnaryPredicate predicate ) {
				auto mid = std::stable_partition(
				  begin( ), end( ),
				  [&predicate]( auto const &v ) { return predicate( v.get( ) ); } );
				return std::make_pair( mid, *this );
			}

			template<typename UnaryPredicate>
			auto stable_partition_it( UnaryPredicate predicate ) const {
				return copy( ).partition_it( predicate );
			}

			template<typename T>
			auto accumulate( T &&init ) const {
				return std::accumulate( begin( ), end( ), std::forward<T>( init ) );
			}

			template<typename T, typename BinaryOperator>
			auto accumulate( T &&init, BinaryOperator oper ) const {
				return std::accumulate( begin( ), end( ), std::forward<T>( init ),
				                        [&oper]( auto const &a, auto const &b ) {
					                        return oper( a.get( ), b.get( ) );
				                        } );
			}

			template<typename UnaryOperator>
			auto transform( UnaryOperator oper ) const {
				using v_t = decltype( oper( *begin( ) ) );
				auto result = daw::range::make_range_collection<v_t>( );
				std::transform( begin( ), end( ), std::back_inserter( result ),
				                [&oper]( auto const &v ) { return oper( v.get( ) ); } );
				return result;
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

			template<typename UnaryPredicate>
			ReferenceRange &erase( UnaryPredicate predicate ) {
				m_values.erase( std::remove_if( m_values.begin( ), m_values.end( ),
				                                [&predicate]( auto const &v ) {
					                                return predicate( v.get( ) );
				                                } ),
				                m_values.end( ) );
				return *this;
			}

			template<typename UnaryPredicate>
			ReferenceRange erase( UnaryPredicate predicate ) const {
				return copy( ).erase( predicate );
			}

			template<typename Value>
			ReferenceRange erase_where_equal_to( Value const &value ) const {
				return erase( [&value]( auto const &current_value ) {
					return value == current_value;
				} );
			}

			template<typename Value>
			ReferenceRange &erase_where_equal_to( Value const &value ) {
				return erase( [&value]( auto const &current_value ) {
					return value == current_value;
				} );
			}

			template<typename UnaryPredicate>
			ReferenceRange where( UnaryPredicate predicate ) const {
				return erase(
				  [predicate]( auto const &v ) { return !predicate( v ); } );
			}

			template<typename UnaryPredicate>
			ReferenceRange &where( UnaryPredicate predicate ) {
				return erase(
				  [predicate]( auto const &v ) { return !predicate( v ); } );
			}

			template<typename Value>
			ReferenceRange where_equal_to( Value const &value ) const {
				return where( [&value]( auto const &current_value ) {
					return value == current_value;
				} );
			}

			template<typename Value>
			ReferenceRange &where_equal_to( Value const &value ) {
				return where( [&value]( auto const &current_value ) {
					return value == current_value;
				} );
			}

			template<typename Container>
			auto as( ) const {
				Container result;
				for( auto const &v : *this ) {
					result.push_back( v.get( ) );
				}
				return result;
			}

			auto as_vector( ) const {
				return as<std::vector<referenced_value_type>>( );
			}

			template<typename Function>
			ReferenceRange for_each( Function function ) {
				for( auto const &v : m_values ) {
					function( v.get( ) );
				}
				return *this;
			}

			template<typename Function>
			ReferenceRange for_each( Function function ) const {
				return copy( ).for_each( function );
			}

			ReferenceRange &shuffle( ) {
				static std::random_device rd;
				static std::mt19937 g( rd( ) );
				std::shuffle( begin( ), end( ), g );
				return *this;
			}

			ReferenceRange shuffle( ) const {
				return copy( ).shuffle( );
			}

			template<typename UniformRandomNumberGenerator>
			ReferenceRange &shuffle( UniformRandomNumberGenerator &&urng ) {
				std::shuffle( begin( ), end( ),
				              std::forward<UniformRandomNumberGenerator>( urng ) );
				return *this;
			}

			template<typename UniformRandomNumberGenerator>
			ReferenceRange shuffle( UniformRandomNumberGenerator &&urng ) const {
				return copy( ).shuffle(
				  std::forward<UniformRandomNumberGenerator>( urng ) );
			}
		}; // class ReferenceRange

		namespace impl {
			template<typename Container, typename = void>
			auto make_range_reference( Container &container ) {
				using iterator =
				  std::remove_const_t<decltype( std::begin( container ) )>;
				return ReferenceRange<iterator>( std::begin( container ),
				                                 std::end( container ) );
			}

			template<typename IteratorF, typename IteratorL>
			auto make_range_reference( IteratorF first, IteratorL last ) {
				using iterator = typename std::remove_const_t<IteratorF>;
				return ReferenceRange<iterator>( first, last );
			}

			template<typename Iterator>
			auto make_range_reference(
			  daw::range::ReferenceRange<Iterator> const &container ) {
				using iterator = typename std::remove_const_t<Iterator>;
				return ReferenceRange<iterator>( container.begin( ), container.end( ) );
			}
		} // namespace impl
		template<typename Arg, typename... Args>
		auto make_range_reference( Arg &&arg, Args &&... args ) {
			return daw::range::impl::make_range_reference(
			  std::forward<Arg>( arg ), std::forward<Args>( args )... );
		}

		namespace details {
			template<typename T>
			std::false_type is_range_reference_impl( T const &, long );

			template<typename T>
			auto is_range_reference_impl( T const &value, int ) ->
			  typename T::is_range_reference;
		} // namespace details

		template<typename T>
		struct is_range_reference : decltype( details::is_range_reference_impl(
		                              std::declval<T const &>( ), 1 ) ) {};

		template<typename T>
		using is_range_reference_t = typename is_range_reference<T>::type;
		template<typename T>
		constexpr bool is_range_reference_v = is_range_reference<T>::value;

		template<typename OStream, typename Iterator,
		         std::enable_if_t<daw::traits::is_ostream_like_lite_v<OStream>,
		                          std::nullptr_t> = nullptr>
		OStream &operator<<( OStream &os, ReferenceRange<Iterator> const &rng ) {
			os << "{";
			if( !rng.empty( ) ) {
				for( auto it = rng.cbegin( ); it != rng.cend( ); ++it ) {
					os << " " << it->get( );
				}
			}
			os << " }";
			return os;
		}

	} // namespace range
} // namespace daw
