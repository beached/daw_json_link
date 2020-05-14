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

#include <type_traits>

#include "../cpp_17.h"
#include "daw_traits_impl.h"

namespace daw {
	namespace traits {
		template<typename Container>
		constexpr bool is_container_like_test( ) noexcept {
			static_assert( is_container_like_v<Container>,
			               "Container does not fullfill the Container concept" );
			static_assert( has_begin<Container>,
			               "Container does not have a begin( Container ) or "
			               "std::begin( Container ) overload" );
			static_assert( has_end<Container>,
			               "Container does not have a end( Container ) or std::end( "
			               "Container ) overload" );
			return true;
		}

		template<typename Container>
		constexpr bool is_container_like = is_container_like_test<Container>( );

		//////////////////////////////////////////////////////////////////////////
		/// Summary: is like a regular type see
		/// http://www.stepanovpapers.com/DeSt98.pdf
		/// Now using C++'s definition of Regular
		/// http://en.cppreference.com/w/cpp/experimental/ranges/concepts/Regular
		template<typename T>
		inline constexpr bool is_regular_v =
		  all_true_v<std::is_default_constructible_v<T>,
		             std::is_copy_constructible_v<T>,
		             std::is_move_constructible_v<T>, std::is_copy_assignable_v<T>,
		             std::is_move_assignable_v<T>, is_equality_comparable_v<T>,
		             is_inequality_comparable_v<T>>;

		template<typename T>
		constexpr bool is_regular_test( ) noexcept {
			static_assert( is_regular_v<T>,
			               "T does not fullfill the concept of a regular type. See "
			               "http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2017/"
			               "p0840r0.html" );
			static_assert( std::is_default_constructible_v<T>,
			               "T is not default constructable" );
			static_assert( std::is_copy_constructible_v<T>,
			               "T is not copy constructible" );
			static_assert( std::is_move_constructible_v<T>,
			               "T is not move constructible" );
			static_assert( std::is_copy_assignable_v<T>, "T is not copy assignable" );
			static_assert( std::is_move_assignable_v<T>, "T is not move assignable" );
			static_assert( is_equality_comparable_v<T>,
			               "T is not equality comparable" );
			static_assert( is_inequality_comparable_v<T>,
			               "T is not inequality comparable" );
			return true;
		}

		template<typename T>
		inline constexpr bool is_regular = is_regular_test<T>( );

		// Iterator Concepts
		// is_iterator
		template<typename Iterator>
		inline constexpr bool is_iterator_v =
		  all_true_v<std::is_copy_constructible_v<Iterator>,
		             std::is_copy_assignable_v<Iterator>,
		             std::is_destructible_v<Iterator>,
		             traits_details::is_incrementable_v<Iterator>,
		             traits_details::has_value_type_v<Iterator>,
		             traits_details::has_difference_type_v<Iterator>,
		             traits_details::has_reference_v<Iterator>,
		             traits_details::has_pointer_v<Iterator>,
		             traits_details::has_iterator_category_v<Iterator>,
		             is_swappable_v<Iterator>>;

		template<typename Iterator>
		constexpr bool is_iterator_test( ) noexcept {
			static_assert( is_iterator_v<Iterator>,
			               "Iterator does not fullfill the Iterator concept.  See "
			               "https://en.cppreference.com/w/cpp/named_req/Iterator" );

			static_assert( std::is_copy_constructible_v<Iterator>,
			               "Iterator is not copy constructable" );
			static_assert( std::is_copy_assignable_v<Iterator>,
			               "Iterator is not copy assignable" );
			static_assert( std::is_destructible_v<Iterator>,
			               "Iterator is not destructable" );
			static_assert( is_dereferenceable_v<Iterator>,
			               "Iterator is not dereferenceable" );
			static_assert( traits_details::is_incrementable_v<Iterator>,
			               "Iterator is not incrementable" );
			static_assert( traits_details::has_value_type_v<Iterator>,
			               "Iterator does not expose value_type type alias" );
			static_assert( traits_details::has_difference_type_v<Iterator>,
			               "Iterator does not expose difference type alias" );
			static_assert( traits_details::has_reference_v<Iterator>,
			               "Iterator does not expose reference type alias" );
			static_assert( traits_details::has_pointer_v<Iterator>,
			               "Iterator does not expose pointer type alias" );
			static_assert( traits_details::has_iterator_category_v<Iterator>,
			               "Iterator does not expose iterator category type alias" );
			static_assert( is_swappable_v<Iterator>, "Iterator is not swappable" );
			return true;
		}

		template<typename Iterator>
		inline constexpr bool is_iterator = is_iterator_test<Iterator>( );

		// is_output_iterator
		template<typename OutputIterator,
		         typename T =
		           typename std::iterator_traits<OutputIterator>::value_type>
		inline constexpr bool is_output_iterator_v =
		  all_true_v<is_iterator_v<OutputIterator>,
		             is_assignable_iterator_v<OutputIterator, T>>;

		template<
		  typename OutputIterator,
		  typename T = typename std::iterator_traits<OutputIterator>::value_type>
		constexpr bool is_output_iterator_test( ) noexcept {
			static_assert(
			  is_output_iterator_v<OutputIterator, T>,
			  "OutputIterator does not fullfill the OutputIterator concept.  See "
			  "https://en.cppreference.com/w/cpp/named_req/OutputIterator" );
			is_iterator_test<OutputIterator>( );
			static_assert( is_assignable_iterator_v<OutputIterator, T>,
			               "OutputIterator is not assignable" );
			return true;
		}

		template<
		  typename OutputIterator,
		  typename T = typename std::iterator_traits<OutputIterator>::value_type>
		inline constexpr bool
		  is_output_iterator = is_output_iterator_test<OutputIterator, T>( );

		// is_input_iterator
		template<typename InputIterator,
		         typename T =
		           std::decay_t<decltype( *std::declval<InputIterator>( ) )>>
		inline constexpr bool is_input_iterator_v = all_true_v<
		  is_iterator_v<InputIterator>, is_equality_comparable_v<InputIterator>,
		  std::is_convertible_v<decltype( *std::declval<InputIterator>( ) ), T>>;

		template<typename InputIterator>
		constexpr bool is_input_iterator_test( ) noexcept {
			static_assert(
			  is_input_iterator_v<InputIterator>,
			  "InputIterator does not fullfill the InputIterator concept.  See "
			  "https://en.cppreference.com/w/cpp/named_req/InputIterator" );

			is_iterator_test<InputIterator>( );

			using T = decltype( *std::declval<InputIterator>( ) );
			using U = remove_cvref_t<T>;

			static_assert( is_equality_comparable_v<InputIterator>,
			               "InputIterator is not equality_comparable" );

			static_assert(
			  std::is_convertible_v<T, U>,
			  "InputIterator's dereferenced value is not convertible to itself" );
			return true;
		}

		template<typename InputIterator>
		inline constexpr bool
		  is_input_iterator = is_input_iterator_test<InputIterator>( );

		template<typename InOutIterator,
		         typename T =
		           typename std::iterator_traits<InOutIterator>::value_type>
		inline constexpr bool is_inout_iterator_v =
		  all_true_v<is_input_iterator_v<InOutIterator>,
		             is_output_iterator_v<InOutIterator, T>>;

		template<typename InOutIterator, typename T = typename std::iterator_traits<
		                                   InOutIterator>::value_type>
		constexpr bool is_inout_iterator_test( ) noexcept {
			static_assert( is_inout_iterator_v<InOutIterator, T>,
			               "InOutIterator does not fullfill the Input and Output "
			               "Iterator concepts" );
			is_input_iterator_test<InOutIterator>( );
			is_output_iterator_test<InOutIterator, T>( );

			return true;
		}

		template<typename InOutIterator, typename T = typename std::iterator_traits<
		                                   InOutIterator>::value_type>
		inline constexpr bool
		  is_inout_iterator = is_input_iterator_test<InOutIterator, T>( );

		// is_forward_iterator
		template<typename ForwardIterator>
		inline constexpr bool is_forward_access_iterator_v =
		  all_true_v<is_iterator_v<ForwardIterator>,
		             std::is_default_constructible_v<ForwardIterator>>;

		template<typename ForwardIterator>
		constexpr bool is_forward_access_iterator_test( ) noexcept {
			static_assert(
			  is_forward_access_iterator_v<ForwardIterator>,
			  "ForwardIterator does not fullfill the RandomIterator concept.  See "
			  "https://en.cppreference.com/w/cpp/named_req/ForwardIterator" );

			is_iterator_test<ForwardIterator>( );

			static_assert( std::is_default_constructible_v<ForwardIterator>,
			               "ForwardIterator is not default constructible" );
			// Cannot express MultiPass // auto b = a; *a++; *b;
			return true;
		}

		// is_bidirectional_access_iterator
		template<typename BidirectionalIterator>
		inline constexpr bool is_bidirectional_access_iterator_v =
		  all_true_v<is_forward_access_iterator_v<BidirectionalIterator>,
		             traits::has_decrement_operator_v<BidirectionalIterator>>;

		template<typename BidirectionalIterator>
		inline constexpr bool is_bidirectional_access_iterator_test( ) noexcept {
			static_assert(
			  is_bidirectional_access_iterator_v<BidirectionalIterator>,
			  "BidirectionalIterator does not fullfill the RandomIterator concept.  "
			  "See "
			  "https://en.cppreference.com/w/cpp/named_req/BidirectionalIterator" );

			is_forward_access_iterator_test<BidirectionalIterator>( );
			static_assert( traits::has_decrement_operator_v<BidirectionalIterator>,
			               "BidirectionalIterator does not have decrement operator" );
			return true;
		}

		template<typename BidirectionalIterator>
		inline constexpr bool is_bidirectional_access_iterator =
		  is_bidirectional_access_iterator_test<BidirectionalIterator>( );

		// is_random_access_iterator
		template<typename RandomIterator>
		inline constexpr bool is_random_access_iterator_v = daw::all_true_v<
		  is_bidirectional_access_iterator_v<RandomIterator>,
		  traits::has_addition_operator_v<RandomIterator, int>,
		  traits::has_addition_operator_v<int, RandomIterator>,
		  traits::has_subtraction_operator_v<RandomIterator, int>,
		  traits::has_subtraction_operator_v<RandomIterator>,
		  traits::has_compound_assignment_add_operator_v<RandomIterator, int>,
		  traits::has_compound_assignment_sub_operator_v<RandomIterator, int>,
		  is_less_than_comparable_v<RandomIterator>,
		  is_greater_than_comparable_v<RandomIterator>,
		  is_equal_less_than_comparable_v<RandomIterator>,
		  is_equal_greater_than_comparable_v<RandomIterator>,
		  traits::has_integer_subscript_v<RandomIterator>>;

		template<typename RandomIterator>
		constexpr bool is_random_access_iterator_test( ) noexcept {
			static_assert(
			  is_random_access_iterator_v<RandomIterator>,
			  "RandomIterator does not fullfill the RandomIterator concept.  See "
			  "https://en.cppreference.com/w/cpp/named_req/RandomAccessIterator" );
			is_bidirectional_access_iterator_test<RandomIterator>( );
			// Mathematics
			static_assert(
			  traits::has_addition_operator_v<RandomIterator, int>,
			  "RandomIterator does not support addition with integer types" );
			static_assert(
			  traits::has_addition_operator_v<int, RandomIterator>,
			  "RandomIterator does not support addition with integer types" );
			static_assert(
			  traits::has_subtraction_operator_v<RandomIterator, int>,
			  "RandomIterator does not support subtraction with integer types" );
			static_assert(
			  traits::has_subtraction_operator_v<RandomIterator>,
			  "RandomIterator does not support subtraction with RandomIterators" );
			// Compound Assignment
			static_assert(
			  traits::has_compound_assignment_add_operator_v<RandomIterator, int>,
			  "RandomIterator does not support compound assignment "
			  "addition += operations" );
			static_assert(
			  traits::has_compound_assignment_sub_operator_v<RandomIterator, int>,
			  "RandomIterator does not support compound assignment "
			  "subtraction -= operations" );
			// Comparison
			static_assert(
			  is_less_than_comparable_v<RandomIterator>,
			  "RandomIterator does not support comparison < operations" );
			static_assert(
			  is_greater_than_comparable_v<RandomIterator>,
			  "RandomIterator does not support comparison > operations" );
			static_assert(
			  is_equal_less_than_comparable_v<RandomIterator>,
			  "RandomIterator does not support comparison <= operations" );
			static_assert(
			  is_equal_greater_than_comparable_v<RandomIterator>,
			  "RandomIterator does not support comparison >= operations" );
			// Subscript
			static_assert( traits::has_integer_subscript_v<RandomIterator>,
			               "RandomIterator does not support subscript operator" );
			return true;
		}

		template<typename RandomIterator>
		inline constexpr bool is_random_access_iterator =
		  is_random_access_iterator_test<RandomIterator>( );

		template<typename Sortable>
		inline constexpr bool is_sortable_container_v =
		  all_true_v<traits::is_container_like_v<Sortable>,
		             is_random_access_iterator_v<decltype(
		               std::begin( std::declval<Sortable>( ) ) )>>;

		template<typename Sortable>
		constexpr bool is_sortable_container_test( ) noexcept {
			static_assert(
			  is_sortable_container_v<Sortable>,
			  "Sortable does not fullfill the requirements of the Sortable concept" );
			static_assert( traits::is_container_like<Sortable>,
			               "Sortable does not fullfill the requirements of the "
			               "Container concept" );

			using sortable_iterator_t =
			  decltype( std::begin( std::declval<Sortable>( ) ) );
			static_assert( is_random_access_iterator<sortable_iterator_t>, "" );

			return true;
		}
		template<typename Predicate, typename... Args>
		inline constexpr bool is_predicate_v =
		  is_detected_convertible_v<bool, traits::detectors::callable_with,
		                            Predicate, Args...>;

		template<typename Predicate, typename... Args>
		constexpr bool is_predicate_test( ) noexcept {
			static_assert(
			  is_predicate_v<Predicate, Args...>,
			  "Predicate does not satisfy the  Predicate concept.  See "
			  "http://en.cppreference.com/w/cpp/concept/Predicate for more "
			  "information" );
			return true;
		}

		template<typename Predicate, typename... Args>
		inline constexpr bool
		  is_predicate = is_predicate_test<Predicate, Args...>( );

		template<typename BinaryPredicate, typename T, typename U = T>
		inline constexpr bool is_binary_predicate_v =
		  is_predicate_v<BinaryPredicate, T, U>;

		template<typename BinaryPredicate, typename T, typename U = T>
		constexpr bool is_binary_predicate_test( ) noexcept {
			static_assert(
			  is_binary_predicate_v<BinaryPredicate, T, U>,
			  "BinaryPredicate does not satisfy the Binary Predicate concept.  See "
			  "http://en.cppreference.com/w/cpp/concept/BinaryPredicate for more "
			  "information" );
			return true;
		}

		template<typename BinaryPredicate, typename T, typename U = T>
		inline constexpr bool
		  is_binary_predicate = is_binary_predicate_test<BinaryPredicate, T, U>( );

		template<typename Compare, typename T, typename U = T>
		inline constexpr bool is_compare_v = is_binary_predicate_v<Compare, T, U>;

		template<typename Compare, typename T, typename U = T>
		constexpr bool is_compare_test( ) noexcept {
			static_assert(
			  is_compare_v<Compare, T, U>,
			  "Compare function does not meet the requirements of the Compare "
			  "concept. "
			  "http://en.cppreference.com/w/cpp/concept/Compare" );

			return true;
		}

		template<typename UnaryPredicate, typename T>
		inline constexpr bool is_unary_predicate_v =
		  is_predicate_v<UnaryPredicate, T>;

		template<typename UnaryPredicate, typename T>
		constexpr bool is_unary_predicate_test( ) noexcept {
			static_assert(
			  is_unary_predicate_v<UnaryPredicate, T>,
			  "UnaryPredicate does not satisfy the Unary Predicate concept.  See "
			  "http://en.cppreference.com/w/cpp/concept/UnaryPredicate for more "
			  "information" );
			return true;
		}

		template<typename UnaryPredicate, typename T>
		inline constexpr bool
		  is_unary_predicate = is_unary_predicate_test<UnaryPredicate, T>( );

	} // namespace traits
} // namespace daw
