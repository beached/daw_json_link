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

#include "../daw_move.h"

namespace daw {
	namespace traits {
		namespace detectors {
			template<typename Function, typename... Args>
			using callable_with =
			  decltype( std::declval<Function>( )( std::declval<Args>( )... ) );

			template<typename BinaryPredicate, typename T, typename U = T>
			using binary_predicate = callable_with<BinaryPredicate, T, U>;

			template<typename UnaryPredicate, typename T>
			using unary_predicate = callable_with<UnaryPredicate, T>;

			template<typename T, typename U>
			using less_than_comparable =
			  decltype( std::declval<T>( ) < std::declval<U>( ) );

			template<typename T, typename U>
			using equal_less_than_comparable =
			  decltype( std::declval<T>( ) <= std::declval<U>( ) );

			template<typename T, typename U>
			using greater_than_comparable =
			  decltype( std::declval<T>( ) > std::declval<U>( ) );

			template<typename T, typename U>
			using equal_greater_than_comparable =
			  decltype( std::declval<T>( ) >= std::declval<U>( ) );

			namespace details {
				template<typename T, typename U>
				[[maybe_unused]] void swap( T &lhs, U &rhs ) {
					using std::swap;
					swap( lhs, rhs );
				}
			} // namespace details

			template<typename T>
			using swappable =
			  decltype( details::swap( std::declval<T>( ), std::declval<T>( ) ) );

			template<typename Iterator, typename T>
			using assignable =
			  decltype( *std::declval<Iterator>( ) = std::declval<T>( ) );

			template<typename T, typename U>
			using equality_comparable =
			  decltype( std::declval<T>( ) == std::declval<U>( ) );

			template<typename T, typename U>
			using inequality_comparable =
			  decltype( std::declval<T>( ) != std::declval<U>( ) );

			template<typename T>
			using std_begin_detector = decltype( std::begin( std::declval<T>( ) ) );

			template<typename T>
			using adl_begin_detector = decltype( begin( std::declval<T>( ) ) );

			template<typename T>
			using std_end_detector = decltype( std::end( std::declval<T>( ) ) );

			template<typename T>
			using adl_end_detector = decltype( end( std::declval<T>( ) ) );

			template<typename T>
			using dereferenceable = decltype( *std::declval<T>( ) );

			template<typename T>
			using has_integer_subscript = decltype( std::declval<T>( )[0] );

			template<typename T>
			using has_size =
			  decltype( std::declval<size_t &>( ) = std::declval<T>( ).size( ) );

			template<typename T>
			using is_array_array = decltype( std::declval<T>( )[0][0] );

			template<typename T>
			using has_empty =
			  decltype( std::declval<bool &>( ) = std::declval<T>( ).empty( ) );

			template<typename T>
			using has_append_operator = decltype(
			  std::declval<T &>( ) += std::declval<has_integer_subscript<T>>( ) );

			template<typename T>
			using has_append =
			  decltype( std::declval<T>( ).append( std::declval<T>( ) ) );

			template<typename T, typename U>
			using has_addition_operator =
			  decltype( std::declval<T>( ) + std::declval<U>( ) );

			template<typename T, typename U>
			using has_subtraction_operator =
			  decltype( std::declval<T>( ) - std::declval<U>( ) );

			template<typename T, typename U>
			using has_multiplication_operator =
			  decltype( std::declval<T>( ) * std::declval<U>( ) );

			template<typename T, typename U>
			using has_division_operator =
			  decltype( std::declval<T>( ) / std::declval<U>( ) );

			template<typename T, typename U>
			using has_compound_assignment_add_operator =
			  decltype( std::declval<T &>( ) += std::declval<U>( ) );

			template<typename T, typename U>
			using has_compound_assignment_sub_operator =
			  decltype( std::declval<T &>( ) -= std::declval<U>( ) );

			template<typename T, typename U>
			using has_compound_assignment_mul_operator =
			  decltype( std::declval<T &>( ) *= std::declval<U>( ) );

			template<typename T, typename U>
			using has_compound_assignment_div_operator =
			  decltype( std::declval<T &>( ) /= std::declval<U>( ) );

			template<typename T, typename U>
			using has_modulus_operator =
			  decltype( std::declval<T>( ) % std::declval<U>( ) );

			template<typename T>
			using has_increment_operator = decltype( ++std::declval<T &>( ) );

			template<typename T>
			using has_decrement_operator = decltype( --std::declval<T &>( ) );
		} // namespace detectors

		namespace traits_details {
			template<typename...>
			struct is_single_void_arg_t : std::false_type {};

			template<>
			struct is_single_void_arg_t<void> : std::true_type {};
		} // namespace traits_details

		template<typename... Args>
		inline constexpr bool is_single_void_arg_v =
		  traits_details::is_single_void_arg_t<Args...>::value;

		template<typename Function, typename... Args>
		inline constexpr bool is_callable_v =
		  is_detected_v<traits::detectors::callable_with, Function, Args...> or
		  ( is_single_void_arg_v<Args...> and
		    is_detected_v<traits::detectors::callable_with, Function> );

		template<typename Result, typename Function, typename... Args>
		inline constexpr bool is_callable_convertible_v =
		  is_detected_convertible_v<Result, traits::detectors::callable_with,
		                            Function, Args...>;
		namespace traits_details {
			template<typename Function, typename... Args,
			         std::enable_if_t<is_callable_v<Function, Args...>,
			                          std::nullptr_t> = nullptr>
			constexpr bool is_nothrow_callable_test( ) noexcept {
				return noexcept(
				  std::declval<Function>( )( std::declval<Args>( )... ) );
			}

			template<
			  typename Function, typename... Args,
			  std::enable_if_t<not daw::traits::is_callable_v<Function, Args...>,
			                   std::nullptr_t> = nullptr>
			constexpr bool is_nothrow_callable_test( ) noexcept {
				return false;
			}
		} // namespace traits_details

		template<typename Function, typename... Args>
		inline constexpr bool is_nothrow_callable_v =
		  traits_details::is_nothrow_callable_test<Function, Args...>( );

		template<typename Function, typename... Args>
		using is_callable_t = typename is_detected<traits::detectors::callable_with,
		                                           Function, Args...>::type;

		template<typename T>
		using make_fp = std::add_pointer_t<T>;

		template<typename T, typename U = T>
		inline constexpr bool has_addition_operator_v =
		  daw::is_detected_v<detectors::has_addition_operator, T, U>;

		template<typename T, typename U = T>
		inline constexpr bool has_subtraction_operator_v =
		  daw::is_detected_v<detectors::has_subtraction_operator, T, U>;

		template<typename T, typename U = T>
		inline constexpr bool has_multiplication_operator_v =
		  daw::is_detected_v<detectors::has_multiplication_operator, T, U>;

		template<typename T, typename U = T>
		inline constexpr bool has_division_operator_v =
		  daw::is_detected_v<detectors::has_division_operator, T, U>;

		template<typename T, typename U>
		inline constexpr bool has_compound_assignment_add_operator_v =
		  daw::is_detected_v<detectors::has_compound_assignment_add_operator, T, U>;

		template<typename T, typename U>
		inline constexpr bool has_compound_assignment_sub_operator_v =
		  daw::is_detected_v<detectors::has_compound_assignment_sub_operator, T, U>;

		template<typename T, typename U>
		inline constexpr bool has_compound_assignment_mul_operator_v =
		  daw::is_detected_v<detectors::has_compound_assignment_mul_operator, T, U>;

		template<typename T, typename U>
		inline constexpr bool has_compound_assignment_div_operator_v =
		  daw::is_detected_v<detectors::has_compound_assignment_div_operator, T, U>;

		template<typename T, typename U = T>
		inline constexpr bool has_modulus_operator_v =
		  daw::is_detected_v<detectors::has_modulus_operator, T, U>;

		template<typename T>
		inline constexpr bool has_increment_operator_v =
		  daw::is_detected_v<detectors::has_increment_operator, T>;

		template<typename T>
		inline constexpr bool has_decrement_operator_v =
		  daw::is_detected_v<detectors::has_decrement_operator, T>;

		template<typename String>
		inline constexpr bool has_integer_subscript_v =
		  daw::is_detected_v<detectors::has_integer_subscript, String>;

		template<typename String>
		inline constexpr bool has_size_memberfn_v =
		  daw::is_detected_v<detectors::has_size, String>;

		template<typename String>
		inline constexpr bool has_empty_memberfn_v =
		  daw::is_detected_v<detectors::has_empty, String>;

		template<typename String>
		inline constexpr bool has_append_memberfn_v =
		  daw::is_detected_v<detectors::has_append, String>;

		template<typename String>
		inline constexpr bool has_append_operator_v =
		  daw::is_detected_v<detectors::has_append_operator, String>;

		template<typename T, typename U = T>
		using is_equality_comparable =
		  is_detected_convertible<bool, detectors::equality_comparable, T, U>;

		template<typename T, typename U = T>
		inline constexpr bool is_equality_comparable_v =
		  is_detected_convertible_v<bool, detectors::equality_comparable, T, U>;

		template<typename T, typename U = T>
		inline constexpr bool is_inequality_comparable_v =
		  is_detected_convertible_v<bool, detectors::inequality_comparable, T, U>;

		template<typename T, typename U = T>
		inline constexpr bool is_less_than_comparable_v =
		  is_detected_convertible_v<bool, detectors::less_than_comparable, T, U>;

		template<typename T, typename U = T>
		inline constexpr bool is_equal_less_than_comparable_v =
		  is_detected_convertible_v<bool, detectors::equal_less_than_comparable, T,
		                            U>;

		template<typename T, typename U = T>
		inline constexpr bool is_greater_than_comparable_v =
		  is_detected_convertible_v<bool, detectors::greater_than_comparable, T, U>;

		template<typename T, typename U = T>
		inline constexpr bool is_equal_greater_than_comparable_v =
		  is_detected_convertible_v<bool, detectors::equal_greater_than_comparable,
		                            T, U>;

		template<typename Iterator,
		         typename T = typename std::iterator_traits<Iterator>::value_type>
		inline constexpr bool is_assignable_iterator_v =
		  is_detected_v<detectors::assignable, Iterator, T>;

		template<typename L, typename R>
		inline constexpr bool is_comparable_v =
		  is_equality_comparable_v<L, R> and is_equality_comparable_v<R, L>;

		template<typename Container>
		inline constexpr bool has_begin =
		  is_detected_v<detectors::std_begin_detector, Container> or
		  is_detected_v<detectors::adl_begin_detector, Container>;

		template<typename Container>
		inline constexpr bool has_end =
		  is_detected_v<detectors::std_end_detector, Container> or
		  is_detected_v<detectors::adl_end_detector, Container>;

		template<typename Container>
		inline constexpr bool is_container_like_v =
		  has_begin<Container> and has_end<Container>;

		namespace traits_details {
			namespace is_iter {
				template<typename T>
				using has_value_type = typename std::iterator_traits<T>::value_type;

				template<typename T>
				using has_difference_type =
				  typename std::iterator_traits<T>::difference_type;

				template<typename T>
				using has_reference = typename std::iterator_traits<T>::reference;

				template<typename T>
				using has_pointer = typename std::iterator_traits<T>::pointer;

				template<typename T>
				using has_iterator_category =
				  typename std::iterator_traits<T>::iterator_category;

				template<typename T>
				using is_incrementable = decltype( ++std::declval<T &>( ) );
			} // namespace is_iter

			template<typename T>
			inline constexpr bool is_incrementable_v =
			  std::is_same_v<T &, daw::detected_t<is_iter::is_incrementable, T>>;

			template<typename T>
			inline constexpr bool has_value_type_v =
			  daw::is_detected_v<is_iter::has_value_type, T>;

			template<typename T>
			inline constexpr bool has_difference_type_v =
			  daw::is_detected_v<is_iter::has_difference_type, T>;

			template<typename T>
			inline constexpr bool has_reference_v =
			  daw::is_detected_v<is_iter::has_reference, T>;

			template<typename T>
			inline constexpr bool has_pointer_v =
			  daw::is_detected_v<is_iter::has_pointer, T>;

			template<typename T>
			inline constexpr bool has_iterator_category_v =
			  daw::is_detected_v<is_iter::has_iterator_category, T>;

			template<typename T>
			inline constexpr bool has_iterator_trait_types_v =
			  has_value_type_v<T> and has_difference_type_v<T> and has_reference_v<T>
			    and has_pointer_v<T> and has_iterator_category_v<T>;
		} // namespace traits_details
		template<typename T>
		using is_dereferenceable_t =
		  typename is_detected<detectors::dereferenceable, T>::type;

		template<typename T>
		using is_dereferenceable = is_detected<detectors::dereferenceable, T>;

		template<typename T>
		inline constexpr bool is_dereferenceable_v =
		  is_detected_v<detectors::dereferenceable, T>;
	} // namespace traits

	namespace traits_details {
		template<typename, typename = std::nullptr_t>
		struct void_function;

		template<typename Function>
		struct void_function<
		  Function, std::enable_if_t<std::is_default_constructible_v<Function>,
		                             std::nullptr_t>> {

			Function function;

			constexpr void_function( ) noexcept(
			  std::is_nothrow_constructible_v<Function> ) = default;

			explicit constexpr void_function( Function const &func ) noexcept(
			  std::is_nothrow_copy_constructible_v<Function> )
			  : function( func ) {}

			explicit constexpr void_function( Function &&func ) noexcept(
			  std::is_nothrow_move_constructible_v<Function> )
			  : function( daw::move( func ) ) {}

			explicit constexpr operator bool( ) noexcept(
			  noexcept( static_cast<bool>( std::declval<Function>( ) ) ) ) {

				return static_cast<bool>( function );
			}

			template<typename... Args,
			         std::enable_if_t<traits::is_callable_v<Function, Args...>,
			                          std::nullptr_t> = nullptr>
			constexpr void operator( )( Args &&... args ) noexcept(
			  traits::is_nothrow_callable_v<Function, Args...> ) {

				function( std::forward<Args>( args )... );
			}
		};

		template<typename Function>
		struct void_function<
		  Function, std::enable_if_t<not std::is_default_constructible_v<Function>,
		                             std::nullptr_t>> {

			Function function;

			explicit constexpr void_function( Function const &func ) noexcept(
			  std::is_nothrow_copy_constructible_v<Function> )
			  : function( func ) {}

			explicit constexpr void_function( Function &&func ) noexcept(
			  std::is_nothrow_move_constructible_v<Function> )
			  : function( daw::move( func ) ) {}

			explicit constexpr operator bool( ) noexcept(
			  noexcept( static_cast<bool>( std::declval<Function>( ) ) ) ) {

				return static_cast<bool>( function );
			}

			template<typename... Args,
			         std::enable_if_t<traits::is_callable_v<Function, Args...>,
			                          std::nullptr_t> = nullptr>
			constexpr void operator( )( Args &&... args ) noexcept(
			  traits::is_nothrow_callable_v<Function, Args...> ) {
				function( std::forward<Args>( args )... );
			}
		};

	} // namespace traits_details

	template<typename Function>
	constexpr auto make_void_function( Function &&func ) noexcept(
	  noexcept( traits_details::void_function<Function>(
	    std::forward<Function>( func ) ) ) ) {

		return traits_details::void_function<Function>(
		  std::forward<Function>( func ) );
	}

	template<size_t N, typename... Args>
	using type_n_t = std::tuple_element_t<N, std::tuple<Args...>>;

	template<bool Condition, typename IfTrue, typename IfFalse>
	struct type_if_else {
		using type = IfFalse;
	};

	template<typename IfTrue, typename IfFalse>
	struct type_if_else<true, IfTrue, IfFalse> {
		using type = IfTrue;
	};

	template<bool Condition, typename IfTrue, typename IfFalse>
	using if_else_t = typename type_if_else<Condition, IfTrue, IfFalse>::type;

	template<bool B, typename T = std::nullptr_t>
	using required = std::enable_if_t<B, T>;

	template<size_t N, typename... Args>
	struct pack_type {
		using type = std::decay_t<std::tuple_element_t<N, std::tuple<Args...>>>;
	};

	template<size_t N, typename... Args>
	using pack_type_t = typename pack_type<N, Args...>::type;
	namespace traits_details {
		struct non_constructor {};

		template<bool>
		struct delete_default_constructor_if {
			constexpr delete_default_constructor_if( ) noexcept {}
			constexpr delete_default_constructor_if( non_constructor ) noexcept {}
		};

		template<>
		struct delete_default_constructor_if<true> {
			delete_default_constructor_if( ) = delete;

			[[maybe_unused]] ~delete_default_constructor_if( ) noexcept = default;

			[[maybe_unused]] constexpr delete_default_constructor_if(
			  non_constructor ) noexcept {}

			[[maybe_unused]] constexpr delete_default_constructor_if(
			  delete_default_constructor_if const & ) {}

			[[maybe_unused]] constexpr delete_default_constructor_if &
			operator=( delete_default_constructor_if const & ) noexcept {
				return *this;
			}

			[[maybe_unused]] constexpr delete_default_constructor_if(
			  delete_default_constructor_if && ) noexcept {}

			[[maybe_unused]] constexpr delete_default_constructor_if &
			operator=( delete_default_constructor_if && ) noexcept {
				return *this;
			}
		};

		template<bool>
		struct delete_copy_constructor_if {
			constexpr delete_copy_constructor_if( ) noexcept {}
			constexpr delete_copy_constructor_if( non_constructor ) noexcept {}
		};

		template<>
		struct delete_copy_constructor_if<true> {
			delete_copy_constructor_if( delete_copy_constructor_if const & ) = delete;

			[[maybe_unused]] constexpr delete_copy_constructor_if(
			  non_constructor ) noexcept {}

			[[maybe_unused]] constexpr delete_copy_constructor_if( ) noexcept {}

			[[maybe_unused]] ~delete_copy_constructor_if( ) noexcept = default;

			[[maybe_unused]] constexpr delete_copy_constructor_if &
			operator=( delete_copy_constructor_if const & ) noexcept {
				return *this;
			}

			[[maybe_unused]] constexpr delete_copy_constructor_if(
			  delete_copy_constructor_if && ) noexcept {}

			[[maybe_unused]] constexpr delete_copy_constructor_if &
			operator=( delete_copy_constructor_if && ) noexcept {
				return *this;
			}
		};

		template<bool>
		struct delete_copy_assignment_if {
			constexpr delete_copy_assignment_if( ) noexcept {}
			constexpr delete_copy_assignment_if( non_constructor ) noexcept {}
		};

		template<>
		struct delete_copy_assignment_if<true> {
			delete_copy_assignment_if &
			operator=( delete_copy_assignment_if const & ) = delete;

			[[maybe_unused]] constexpr delete_copy_assignment_if(
			  non_constructor ) noexcept {}

			[[maybe_unused]] constexpr delete_copy_assignment_if( ) noexcept {}

			[[maybe_unused]] ~delete_copy_assignment_if( ) noexcept = default;

			[[maybe_unused]] constexpr delete_copy_assignment_if(
			  delete_copy_assignment_if const & ) noexcept {}

			[[maybe_unused]] constexpr delete_copy_assignment_if(
			  delete_copy_assignment_if && ) noexcept {}

			[[maybe_unused]] constexpr delete_copy_assignment_if &
			operator=( delete_copy_assignment_if && ) noexcept {
				return *this;
			}
		};

		template<bool>
		struct delete_move_constructor_if {
			constexpr delete_move_constructor_if( ) noexcept {}
			constexpr delete_move_constructor_if( non_constructor ) noexcept {}
		};

		template<>
		struct delete_move_constructor_if<true> {
			delete_move_constructor_if( delete_move_constructor_if const & ) =
			  default;

			[[maybe_unused]] constexpr delete_move_constructor_if(
			  non_constructor ) noexcept {}

			[[maybe_unused]] constexpr delete_move_constructor_if( ) noexcept {}

			[[maybe_unused]] ~delete_move_constructor_if( ) noexcept = default;

			[[maybe_unused]] constexpr delete_move_constructor_if &
			operator=( delete_move_constructor_if const & ) noexcept {
				return *this;
			}
			[[maybe_unused]] constexpr delete_move_constructor_if(
			  delete_move_constructor_if && ) noexcept {}

			constexpr delete_move_constructor_if &
			operator=( delete_move_constructor_if && ) noexcept = delete;
		};

		template<bool>
		struct delete_move_assignment_if {
			constexpr delete_move_assignment_if( ) noexcept {}
			constexpr delete_move_assignment_if( non_constructor ) noexcept {}
		};

		template<>
		struct delete_move_assignment_if<true> {
			[[maybe_unused]] delete_move_assignment_if &
			operator=( delete_move_assignment_if const & ) = default;

			[[maybe_unused]] constexpr delete_move_assignment_if(
			  non_constructor ) noexcept {}

			[[maybe_unused]] constexpr delete_move_assignment_if( ) noexcept {}

			[[maybe_unused]] ~delete_move_assignment_if( ) noexcept = default;

			[[maybe_unused]] constexpr delete_move_assignment_if(
			  delete_move_assignment_if const & ) noexcept {}

			[[maybe_unused]] constexpr delete_move_assignment_if(
			  delete_move_assignment_if && ) noexcept {}

			constexpr delete_move_assignment_if &
			operator=( delete_move_assignment_if && ) noexcept = delete;
		};
	} // namespace traits_details

	template<typename T, bool B = true>
	using enable_default_constructor =
	  traits_details::delete_default_constructor_if<
	    not std::is_default_constructible_v<T> and B>;

	template<typename T, bool B = true>
	using enable_copy_constructor = traits_details::delete_copy_constructor_if<
	  not std::is_copy_constructible_v<T> and B>;

	template<typename T, bool B = true>
	using enable_copy_assignment = traits_details::delete_copy_assignment_if<
	  not std::is_copy_assignable_v<T> and B>;

	template<typename T, bool B = true>
	using enable_move_constructor = traits_details::delete_move_constructor_if<
	  not std::is_move_constructible_v<T> and B>;

	template<typename T, bool B = true>
	using enable_move_assignment = traits_details::delete_move_assignment_if<
	  not std::is_move_assignable_v<T> and B>;

	struct nothing {};
} // namespace daw
