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
#include <type_traits>

#include "daw_move.h"
#include "daw_range.h"
#include "daw_traits.h"

namespace daw {
	namespace algorithm {
		template<typename Container,
		         std::enable_if_t<traits::is_container_like_v<Container>,
		                          std::nullptr_t> = nullptr>
		Container &sort( Container &container ) noexcept( noexcept(
		  std::sort( std::begin( container ), std::end( container ) ) ) ) {

			std::sort( std::begin( container ), std::end( container ) );
			return container;
		}

		template<typename Container, typename Compare,
		         std::enable_if_t<traits::is_container_like_v<Container>,
		                          std::nullptr_t> = nullptr>
		Container &sort( Container &container, Compare compare ) noexcept( noexcept(
		  std::sort( std::begin( container ), std::end( container ), compare ) ) ) {
			static_assert(
			  traits::is_binary_predicate_v<Compare,
			                                decltype( *std::begin( container ) ),
			                                decltype( *std::end( container ) )>,
			  "Compare does not satisfy the Binary Predicate concept.  See "
			  "http://en.cppreference.com/w/cpp/concept/BinaryPredicate for more "
			  "information" );

			std::sort( std::begin( container ), std::end( container ), compare );
			return container;
		}

		template<typename Container,
		         std::enable_if_t<traits::is_container_like_v<Container>,
		                          std::nullptr_t> = nullptr>
		Container &stable_sort( Container &container ) noexcept( noexcept(
		  std::stable_sort( std::begin( container ), std::end( container ) ) ) ) {

			std::stable_sort( std::begin( container ), std::end( container ) );
			return container;
		}

		template<typename Container, typename Compare,
		         std::enable_if_t<traits::is_container_like_v<Container>,
		                          std::nullptr_t> = nullptr>
		Container &stable_sort( Container &container, Compare compare ) noexcept(
		  noexcept( std::stable_sort( std::begin( container ),
		                              std::end( container ), compare ) ) ) {

			static_assert(
			  traits::is_binary_predicate_v<Compare,
			                                decltype( *std::begin( container ) ),
			                                decltype( *std::end( container ) )>,
			  "Compare does not satisfy the Binary Predicate concept.  See "
			  "http://en.cppreference.com/w/cpp/concept/BinaryPredicate for more "
			  "information" );

			std::stable_sort( std::begin( container ), std::end( container ),
			                  compare );
			return container;
		}

		template<typename Container, typename Value,
		         std::enable_if_t<traits::is_container_like_v<Container>,
		                          std::nullptr_t> = nullptr>
		decltype( auto ) find( Container &container, Value const &value ) noexcept(
		  noexcept( std::find( std::begin( container ), std::end( container ),
		                       value ) ) ) {

			return std::find( std::begin( container ), std::end( container ), value );
		}

		template<typename Container, typename Value,
		         std::enable_if_t<traits::is_container_like_v<Container>,
		                          std::nullptr_t> = nullptr>
		decltype( auto )
		find( Container const &container, Value const &value ) noexcept( noexcept(
		  std::find( std::begin( container ), std::end( container ), value ) ) ) {

			return std::find( std::begin( container ), std::end( container ), value );
		}

		template<typename Container, typename UnaryPredicate,
		         std::enable_if_t<traits::is_container_like_v<Container>,
		                          std::nullptr_t> = nullptr>
		decltype( auto )
		find_if( Container &container, UnaryPredicate pred ) noexcept( noexcept(
		  std::find_if( std::begin( container ), std::end( container ), pred ) ) ) {

			static_assert(
			  traits::is_unary_predicate_v<UnaryPredicate,
			                               decltype( *std::begin( container ) ),
			                               decltype( *std::end( container ) )>,
			  "Compare does not satisfy the Unary Predicate concept.  See "
			  "http://en.cppreference.com/w/cpp/concept/Predicate for more "
			  "information" );

			return std::find_if( std::begin( container ), std::end( container ),
			                     pred );
		}

		template<typename Container, typename UnaryPredicate,
		         std::enable_if_t<traits::is_container_like_v<Container>,
		                          std::nullptr_t> = nullptr>
		decltype( auto )
		find_if( Container const &container, UnaryPredicate pred ) noexcept(
		  noexcept( std::find_if( std::begin( container ), std::end( container ),
		                          pred ) ) ) {

			static_assert(
			  traits::is_unary_predicate_v<UnaryPredicate,
			                               decltype( *std::begin( container ) ),
			                               decltype( *std::end( container ) )>,
			  "Compare does not satisfy the Unary Predicate concept.  See "
			  "http://en.cppreference.com/w/cpp/concept/Predicate for more "
			  "information" );

			return std::find_if( std::begin( container ), std::end( container ),
			                     pred );
		}

		template<typename Container, typename Value>
		auto erase_remove( Container &container, Value const &value )
		  -> decltype( container.erase( std::end( container ),
		                                std::end( container ) ) ) {
			return container.erase(
			  std::remove( std::begin( container ), std::end( container ), value ),
			  std::end( container ) );
		}

		template<typename Container, typename UnaryPredicate>
		auto erase_remove_if( Container &container, UnaryPredicate pred )
		  -> decltype( container.erase( std::end( container ),
		                                std::end( container ) ) ) {
			return container.erase(
			  std::remove_if( std::begin( container ), std::end( container ), pred ),
			  std::end( container ) );
		}

		template<typename Container, typename UnaryPredicate>
		auto partition( Container &container, UnaryPredicate pred )
		  -> decltype( begin( container ) ) {
			return std::partition( std::begin( container ), std::end( container ),
			                       pred );
		}

		template<typename Container, typename UnaryPredicate>
		auto stable_partition( Container &container, UnaryPredicate pred )
		  -> decltype( begin( container ) ) {
			return std::stable_partition( std::begin( container ),
			                              std::end( container ), pred );
		}

		template<typename Container, typename T>
		T accumulate( Container const &container, T init ) {
			return std::accumulate( std::begin( container ), std::end( container ),
			                        daw::move( init ) );
		}

		template<typename Container, typename T, typename BinaryOperator,
		         std::enable_if_t<daw::traits::is_container_like_v<
		                            std::remove_reference_t<Container>>,
		                          std::nullptr_t> = nullptr>
		constexpr T accumulate( Container const &container, T init,
		                        BinaryOperator oper ) {
			return daw::algorithm::accumulate( std::begin( container ),
			                                   std::end( container ),
			                                   daw::move( init ), oper );
		}

		template<typename Container, typename UnaryOperator,
		         std::enable_if_t<traits::is_container_like_v<Container>,
		                          std::nullptr_t> = nullptr>
		auto map( Container const &container, UnaryOperator unary_operator ) {

			static_assert(
			  traits::is_callable_v<UnaryOperator,
			                        decltype( *std::cbegin( container ) )>,
			  "UnaryOperator is not callable with the values stored in Container" );

			using result_t = std::vector<decltype(
			  unary_operator( std::declval<typename std::iterator_traits<decltype(
			                    std::cbegin( container ) )>::value_type>( ) ) )>;
			result_t result;
			std::transform( std::cbegin( container ), std::cend( container ),
			                std::back_inserter( result ), unary_operator );
			return result;
		}

		template<
		  typename Container, typename OutputIterator, typename UnaryOperator,
		  std::enable_if_t<traits::is_container_like_v<Container>, std::nullptr_t> =
		    nullptr>
		void
		map( Container const &container, OutputIterator &first_out,
		     UnaryOperator
		       unary_operator ) noexcept( noexcept( std::
		                                              transform(
		                                                std::cbegin( container ),
		                                                std::cend( container ),
		                                                first_out,
		                                                unary_operator ) ) ) {

			static_assert(
			  traits::is_callable_v<UnaryOperator,
			                        decltype( *std::cbegin( container ) )>,
			  "UnaryOperator is not callable with the values stored in Container" );

			static_assert(
			  std::is_convertible_v<decltype(
			                          unary_operator( *std::cbegin( container ) ) ),
			                        decltype( *first_out )>,
			  "Output of UnaryOperator cannot be assigned to *first_out" );

			std::transform( std::cbegin( container ), std::cend( container ),
			                first_out, unary_operator );
		}

		template<
		  typename Container, typename OutputIterator, typename UnaryOperator,
		  std::enable_if_t<traits::is_container_like_v<Container>, std::nullptr_t> =
		    nullptr>
		void
		map( Container const &container, OutputIterator &&first_out,
		     UnaryOperator
		       unary_operator ) noexcept( noexcept( std::
		                                              transform(
		                                                std::cbegin( container ),
		                                                std::cend( container ),
		                                                first_out,
		                                                unary_operator ) ) ) {

			static_assert(
			  traits::is_callable_v<UnaryOperator,
			                        decltype( *std::cbegin( container ) )>,
			  "UnaryOperator is not callable with the values stored in Container" );

			std::transform( std::cbegin( container ), std::cend( container ),
			                first_out, unary_operator );
		}

		// max_element
		template<typename Container, typename UnaryOperator,
		         std::enable_if_t<traits::is_container_like_v<Container>,
		                          std::nullptr_t> = nullptr>
		decltype( auto ) max_element( Container &container ) noexcept( noexcept(
		  std::max_element( std::cbegin( container ), std::cend( container ) ) ) ) {

			return std::max_element( std::cbegin( container ),
			                         std::cend( container ) );
		}

		template<
		  typename Container, typename OutputIterator, typename UnaryOperator,
		  std::enable_if_t<traits::is_container_like_v<Container>, std::nullptr_t> =
		    nullptr>
		decltype( auto ) max_element( Container const &container ) noexcept(
		  noexcept( std::max_element( std::cbegin( container ),
		                              std::cend( container ) ) ) ) {

			return std::max_element( std::cbegin( container ),
			                         std::cend( container ) );
		}

		template<typename Container, typename Compare,
		         std::enable_if_t<traits::is_container_like_v<Container>,
		                          std::nullptr_t> = nullptr>
		decltype( auto )
		max_element( Container &container, Compare compare ) noexcept(
		  noexcept( std::max_element( std::begin( container ),
		                              std::end( container ), compare ) ) ) {

			static_assert(
			  traits::is_binary_predicate_v<Compare,
			                                decltype( *std::begin( container ) ),
			                                decltype( *std::end( container ) )>,
			  "Compare does not satisfy the Binary Predicate concept.  See "
			  "http://en.cppreference.com/w/cpp/concept/BinaryPredicate for more "
			  "information" );

			return std::max_element( std::begin( container ), std::end( container ),
			                         compare );
		}

		template<typename Container, typename Compare,
		         std::enable_if_t<traits::is_container_like_v<Container>,
		                          std::nullptr_t> = nullptr>
		decltype( auto )
		max_element( Container const &container, Compare compare ) noexcept(
		  noexcept( std::max_element( std::begin( container ),
		                              std::end( container ), compare ) ) ) {

			static_assert(
			  traits::is_binary_predicate_v<Compare,
			                                decltype( *std::begin( container ) ),
			                                decltype( *std::end( container ) )>,
			  "Compare does not satisfy the Binary Predicate concept.  See "
			  "http://en.cppreference.com/w/cpp/concept/BinaryPredicate for more "
			  "information" );

			return std::max_element( std::begin( container ), std::end( container ),
			                         compare );
		}

		// contains
		template<typename Container, typename Value>
		bool contains( Container const &container, Value const &value ) {
			return std::find( std::begin( container ), std::end( container ),
			                  value ) != std::end( container );
		}

		template<typename Container, typename Value, typename UnaryPredicate>
		bool contains( Container const &container, Value const &value,
		               UnaryPredicate pred ) {
			auto pred2 = [&value, &pred]( Value const &val ) {
				return pred( value, val );
			};
			return std::find_if( std::begin( container ), std::end( container ),
			                     pred2 ) != std::end( container );
		}

		template<typename Container, typename UnaryPredicate>
		auto where( Container &container, UnaryPredicate predicate ) {
			using value_type = typename std::iterator_traits<decltype(
			  std::begin( container ) )>::value_type;
			std::vector<std::reference_wrapper<value_type>> result;
			for( auto &v : container ) {
				if( predicate( v ) ) {
					result.push_back( std::ref( v ) );
				}
			}
			return result;
		}

		template<typename InputIterator, typename UnaryPredicate>
		auto where( InputIterator first, InputIterator last,
		            UnaryPredicate predicate ) {
			auto rng = make_range( first, last );
			return where( rng, predicate );
		}
	} // namespace algorithm
} // namespace daw
